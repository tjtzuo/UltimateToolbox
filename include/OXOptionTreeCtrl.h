// ==========================================================================
// 				Class Specification : 	  COXOptionTreeCtrl
// ==========================================================================

// Header file : OXOptionTreeCtrl.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
/*

DESCRIPTION


COXOptionTreeControl is CTreeCtrl derived class that allows you to use check boxes
and radio buttons as tree items. Tree control functionality allows you to organize 
them in groups, which provides a compact way of displaying and navigating through
a big number of options that can be represented using check box or radio button.


In most cases options should be organized in some logical way. We associate a tree
brunch with any of such group of options and call them "control groups". Please note
that the functionality of the tree control allows you to use multiple level of 
dependency between groups and subgroups. In order to add new control group in the 
tree control you have to call the following function:


    AddControlGroup();

Using this function you may create any control groups. This function returns 
a handle to the tree item that was created to display the control group title.
You can use this value to add check boxes, radio buttons or other control groups.

In order to add new check box item in the tree you have to use the following 
function:

	AddCheckBox();

In order to add new radio button item in the tree you have to use the following 
function:

	AddRadioButton();


Note that both of this functions require you to specify a unique ID of new option
control. This way any check box or radio button can be uniquely identified throughout 
the option tree control. In order to retrieve the ID of an option control that is 
specified by its handle to the tree item you can call:

    GetItemFromID();

And if you need to get the handle of a tree item which corresponds to the given ID 
you can call:

	GetIDFromItem();


Using first three function you can easily populate option tree control. 
You also can use all standard CTreeCtrl functions that are used in order to populate 
tree control wih items. 

We would like to emphasize one specific rule that applies to the way 
COXOptionTreeCtrl must be used and is different from the satndard one. You must not
explicitly change the image list associated with tree. We use it internally in order
to display check box and radion button images. But what if you want to display some 
images for control group items? AddControlGroup() function allows you to specify
the image indexes for normal and selected state but these indexes must be returned
by the following function which add new image to the internal image list:

    AddImage();

Using this function you can add as many images as you want. 

Although it is not supposed to be modified outside but we still decided to provide 
a direct access to the internal image list. You may retrieve a pointer to it using 
the following function:

  GetImageList();

One of the possible use of the image list would be replacing of the images we use to 
display check box and radio button. At this moment the following images are predefined:


Index			Image

0				unchecked box
1				checked box
2				unselected radio button
3				selected radio button



After the control is populated and displayed a user can expand and collapse control
groups and change state of option items left clicking mouse or pressing down SPACE key.

Whenever the state of an option item is changed the following notification will 
be send in form of WM_NOTIFY message:

	OTN_OPTIONCHANGED

If you handle the notification you have to cast NMHDR* object to NMOPTIONTREE* object.
NMOPTIONTREE structure provides info about the item, which state has been changed, 
the old state and the new state of the item. NMOPTIONTREE structure is declared 
as following:

typedef struct _tagNMOPTIONTREE
{
    // standard header
	NMHDR hdr;
	// handle to the tree item (check box or radio button, which state 
	// has been changed)
    HTREEITEM hItem;
	// item ID
	UINT uItemID;
	// old item state: OTITEM_UNCHECKED or OTITEM_CHECKED
	int nOldCheck;
	// new item state: OTITEM_UNCHECKED or OTITEM_CHECKED
	int nNewCheck;
} NMOPTIONTREE, * LPNMOPTIONTREE;



The state of any option item can be programmatically retrieved and changed using
the following set of functions:

    SetCheck();
    GetCheck();
    GetCheckedRadioButton();
    IsCheckBox();
    IsRadioButton();



And, finally, we provide the functionality to save to and load from registry 
option settings. Use these two functions to do that:

	SaveState();
	LoadState();



We demonstrate an example of how to use option tree control in the sample 
that can be found in the .\Samples\gui\OptionTree subdirectory.



Dependency:
	#include "OXOptionTreeCtrl.h"


Source code files:
	"OXOptionTreeCtrl.cpp"


Resource includes:
	"OXOptionTreeCtrl.rc"		-	image list for check box and radio button images




*/

#if !defined(_OXOPTIONTREECTRL_H__)
#define _OXOPTIONTREECTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"
#include "OXMainRes.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif


// notification sent when check box or radio button state has been changed
#define OTN_OPTIONCHANGED			TVN_LAST+10

// structure that is used with OTN_OPTIONCHANGED notification. In the handler
// you can cast NMHDR* to pointer to this structure
typedef struct _tagNMOPTIONTREE
{
    // standard header
	NMHDR hdr;
	// handle to the tree item (check box or radio button, which state 
	// has been changed)
    HTREEITEM hItem;
	// item ID
	UINT uItemID;
	// old item state: OTITEM_UNCHECKED or OTITEM_CHECKED
	int nOldCheck;
	// new item state: OTITEM_UNCHECKED or OTITEM_CHECKED
	int nNewCheck;

	_tagNMOPTIONTREE()
	{
		::ZeroMemory(this,sizeof(_tagNMOPTIONTREE));
		hItem=NULL;
		uItemID=NULL;
		nOldCheck=-1;
		nNewCheck=-1;
	}

} NMOPTIONTREE, * LPNMOPTIONTREE;


// item states
#define OTITEM_UNCHECKED			0
#define OTITEM_CHECKED				1

// predefined image indices
#define UNCHECKED_IMAGE_INDEX		0
#define CHECKED_IMAGE_INDEX			1
#define UNRADIOED_IMAGE_INDEX		2
#define RADIO_IMAGE_INDEX			3


/////////////////////////////////////////////////////////////////////////////
// COXOptionTreeCtrl window

class OX_CLASS_DECL COXOptionTreeCtrl : public CTreeCtrl
{
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect:	Contructs the object
	COXOptionTreeCtrl();

// Attributes
public:

protected:
	// internal image list for check boxes and radio buttons
    CImageList m_imageList;
	// internal image list for check boxes and radio buttons 
	// that will be used if background of the window is black
    CImageList m_imageListHighContrast;
	// flag that specifies if High Contrast image list shoul be used 
	// instead of standard one
	BOOL m_bUseHighContrast;
	// map of all inserted option items (item ID - item handle)
    CMap<UINT, UINT, void*, void*> m_mapItems;
	// map of all inserted option items (item handle - read only flag)
    CMap<void*, void*, BOOL, BOOL> m_mapReadOnlyItems;

// Operations
public:

	// --- In  :	pszText				-	group title
	//				hParent				-	parent item
	//				bExpanded			-	TRUE if group must be initially expanded
	//				nImageIndex			-	index of the normal image in the internal 
	//										image list. This must be return by 
	//										AddImage() function.
	//				nSelectedImageIndex	-	index of the selected image in the internal 
	//										image list. This must be return by 
	//										AddImage() function.
	//				hInsertAfter		-	the item in the tree after which this group 
	//										will be inserted.
	// --- Out :	
	// --- Returns: handle to the inserted tree item if succeeded or NULL otherwise
	// --- Effect:	adds new control group to the option tree. Control group can 
	//				consist of check boxes radio buttons and other control groups
    HTREEITEM AddControlGroup(LPCTSTR pszText, HTREEITEM hParent=NULL, 
		BOOL bExpanded=TRUE, int nImageIndex=-1, int nSelectedImageIndex=-1, 
		HTREEITEM hInsertAfter=TVI_LAST);


	// --- In  :	uID				-	unique ID of the check box
	//				pszText			-	check box title
	//				hParent			-	parent item
	//				nCheck			-	check box state:
	//									OTITEM_UNCHECKED	-	unchecked
	//									OTITEM_CHECKED		-	checked
	//				hInsertAfter	-	the item in the tree after which this  
	//									check box will be inserted
	//				bReadOnly		-	if set to TRUE then a user will not be 
	//									able to change the state of the item 
	// --- Out :	
	// --- Returns: handle to the inserted tree item if succeeded or NULL otherwise
	// --- Effect:	adds new check box to the option tree. 
    HTREEITEM AddCheckBox(UINT uID, LPCTSTR pszText, HTREEITEM hParent, 
		int nCheck=OTITEM_UNCHECKED, HTREEITEM hInsertAfter=TVI_LAST, 
		BOOL bReadOnly=FALSE);

    
	// --- In  :	uID				-	unique ID of the radio button
	//				pszText			-	radio button title
	//				hParent			-	parent item
	//				nCheck			-	radio button state:
	//									OTITEM_UNCHECKED	-	selected
	//									OTITEM_CHECKED		-	cleared
	//				hInsertAfter	-	the item in the tree after which this  
	//									radio button will be inserted.
	//				bReadOnly		-	if set to TRUE then a user will not be 
	//									able to change the state of the item 
	// --- Out :	
	// --- Returns: handle to the inserted tree item if succeeded or NULL otherwise
	// --- Effect:	adds new radio button to the option tree. 
	HTREEITEM AddRadioButton(UINT uID, LPCTSTR pszText, HTREEITEM hParent, 
		int nCheck=OTITEM_UNCHECKED, HTREEITEM hInsertAfter=TVI_LAST, 
		BOOL bReadOnly=FALSE);


	// --- In  :	uID				-	unique ID of the check box or radio button
	// --- Out :	
	// --- Returns: handle to the corresponding tree item or NULL if not found
	// --- Effect:	finds option item from its ID. 
    HTREEITEM GetItemFromID(UINT uID) const;

    
	// --- In  :	hItemFrom		-	handle to tree item 
	// --- Out :	
	// --- Returns: ID of the corresponding check box or radio button or 
	//				NULL if not found
	// --- Effect:	finds ID for specified option item. 
	UINT GetIDFromItem(HTREEITEM hItemFrom) const;


	// --- In  :	uID				-	unique ID of the check box or radio button
	//				hItem			-	handle to tree item 
	//				nCheck			-	radio button state to set:
	//									OTITEM_UNCHECKED
	//									OTITEM_CHECKED
	// --- Out :	
	// --- Returns: 
	// --- Effect:	change the state of the specified option item
    void SetCheck(UINT uID, int nCheck);
    void SetCheck(HTREEITEM hItem, int nCheck);


	// --- In  :	uID				-	unique ID of the check box or radio button
	//				hItem			-	handle to tree item 
	// --- Out :	
	// --- Returns: radio button state to set:
	//				OTITEM_UNCHECKED
	//				OTITEM_CHECKED
	// --- Effect:	retrieves the state of the specified option item
    int GetCheck(UINT uID) const;
    int GetCheck(HTREEITEM hItem) const;


	// --- In  :	nIDFirstButton	-	ID of the first radio button in the range
	//				nIDLastButton	-	ID of the last radio button in the range
	// --- Out :	
	// --- Returns: ID of the selected radio button item or NULL if none was found
	// --- Effect:	retrieves the ID of the selected radio button out of range
    UINT GetCheckedRadioButton(UINT nIDFirstButton, UINT nIDLastButton) const;

	// --- In  :	hItem			-	handle to tree item 
	// --- Out :	
	// --- Returns: TRUE if specified item is check box or FALSE otherwise
	// --- Effect:	retrieves the flag that specifies if the given item is 
	//				check box item or not
    BOOL IsCheckBox(HTREEITEM hItem) const;

	// --- In  :	hItem			-	handle to tree item 
	// --- Out :	
	// --- Returns: TRUE if specified item is radio button or FALSE otherwise
	// --- Effect:	retrieves the flag that specifies if the given item is 
	//				radio button item or not
    BOOL IsRadioButton(HTREEITEM hItem) const;


	// --- In  :	hItem			-	handle to tree item 
	// --- Out :	
	// --- Returns: TRUE if specified item is read only item
	// --- Effect:	retrieves the flag that specifies if the given item is 
	//				read only item (the state of the item cannot be changed)
    BOOL IsReadOnly(HTREEITEM hItem) const;

	// --- In  :	uID				-	unique ID of the check box or radio button
	//				hItem			-	handle to tree item 
	//				bReadOnly		-	if set to TRUE then a user will not be 
	//									able to change the state of the item 
	// --- Out :	
	// --- Returns: TRUE if specified item is read only item
	// --- Effect:	retrieves the flag that specifies if the given item is 
	//				read only item (the state of the item cannot be changed)
    void SetReadOnly(UINT uID, BOOL bReadOnly);
    void SetReadOnly(HTREEITEM hItem, BOOL bReadOnly);


	// --- In  :	nImageResourceID	-	numeric ID of the bitmap resource
	//				lpszImageResourceID	-	string ID of the bitmap resource
	//				clrMask				-	color mask for the image
	//				nHighContrastImageResourceID	-	numeric ID of the bitmap 
	//													resource of high contrast
	//													image. If 0 is specified
	//													then the image specified by 
	//													nImageResourceID will be used
	//													as high contrast image as well
	//				lpszHighContrastImageResourceID	-	string ID of the bitmap 
	//													resource of high contrast
	//													image. If NULL is specified
	//													then the image specified by 
	//													lpszImageResourceID will be 
	//													used as high contrast image 
	//													as well
	//				clrHighContrastMask				-	color mask for the high 
	//													contrast image
	// --- Out :	
	// --- Returns: the index of inserted image in the internal image list,
	//				or -1 if failed. 
	// --- Effect:	adds new image to the internal image list that is associated
	//				with the tree. The returned value might be used when
	//				inserting control group elements.
    inline int AddImage(UINT nImageResourceID, COLORREF clrMask=RGB(192,192,192),
		UINT nHighContrastImageResourceID=0, 
		COLORREF clrHighContrastMask=RGB(192,192,192)) 
	{
		return AddImage(MAKEINTRESOURCE(nImageResourceID),clrMask,
			(nHighContrastImageResourceID==0 ? NULL : 
			MAKEINTRESOURCE(nHighContrastImageResourceID)),clrHighContrastMask); 
	}
    int AddImage(LPCTSTR lpszImageResourceID, COLORREF clrMask=RGB(192,192,192),
		LPCTSTR lpszHighContrastImageResourceID=NULL, 
		COLORREF clrHighContrastMask=RGB(192,192,192));


	// --- In  :	
	// --- Out :	
	// --- Returns: pointer to the internal image list
	// --- Effect:	retrieves pointer to the internal image list 
	//				associated with the tree
	inline CImageList* GetImageList() 
	{ 
		return (m_bUseHighContrast ? &m_imageListHighContrast : &m_imageList); 
	}


	// --- In  :	lpszSubKey		-	the name of the subkey with which a 
	//									value is associated
	//				lpszValueName	-	the name of the value to set
	// --- Out :	
	// --- Returns: TRUE if successful, or FALSE otherwise
	// --- Effect:	saves the state of all option items into the registry or *.ini file.
	//				In order to forward all output in registry you have to call
	//				CWinApp::SetRegistryKey() function before.
	BOOL SaveState(LPCTSTR lpszSubKey, LPCTSTR lpszValueName) const;

	// --- In  :	lpszSubKey		-	the name of the subkey with which a 
	//									value is associated
	//				lpszValueName	-	the name of the value to retrieve
	// --- Out :	
	// --- Returns: TRUE if successful, or FALSE otherwise
	// --- Effect:	loads the state of all option items from the registry or *.ini file.
	//				In order to read saved info from registry you have to call
	//				CWinApp::SetRegistryKey() function before.
	BOOL LoadState(LPCTSTR lpszSubKey, LPCTSTR lpszValueName);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXOptionTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL DeleteAllItems( );
	virtual ~COXOptionTreeCtrl();

protected:
	// initializes control, populates internal list with predefined 
	// images for check box and radio button elements and associate with the
	// tree control
	virtual BOOL Initialize();

	// retrieves handle to the last clicked option item.
	HTREEITEM GetClickedOptionItem() const;

	// togles the state of the specified item
    void ToggleItem(HTREEITEM hItem);
	// insert new option item in the tree
    HTREEITEM AddOptionItem(UINT uID, LPTV_INSERTSTRUCT ptvs, BOOL bReadOnly=FALSE);

	// --- In  :	nIDCheckButton	-	ID of the radio button to select
	// --- Out :	
	// --- Returns: 
	// --- Effect:	checks the specified radio button
    void CheckRadioButton(UINT nIDCheckButton);

	// change the state of the specified option item
    void SetCheckBox(HTREEITEM hItem, int nCheck);
    void SetRadioButton(HTREEITEM hItem, int nCheck);
	/////////////////////////////////////////////////////

	// helper function that is used to notify the parent window
	// that the state of an option item has changed. The notification
	// OTN_OPTIONCHANGED is sent in form of WM_NOTIFY message. If you handle
	// the notification you have to cast NMHDR* object to NMOPTIONTREE* object.
	// NMOPTIONTREE structure provides info about the item, which state has been 
	// changed, the old state and the new state of the item. Return value is ignored.
	LRESULT NotifyOptionChanged(HTREEITEM hItem, int nOldCheck, int nNewCheck);

	// checks if high contrast image list should be used
	void CheckHightContrast();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXOptionTreeCtrl)
	afx_msg BOOL OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnDeleteItem(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXOPTIONTREECTRL_H__)
