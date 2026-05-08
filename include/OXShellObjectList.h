// ==========================================================================
// 				Class Specification : COXShellObjectList
// ==========================================================================

// Header file : OXShellObjectList.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

/*

  DESCRIPTION


COXShellObjectList class is derived from CListCtrl and implements standard
list control that is automatically populated with Shell Name space folders
and files.

As long as COXShellObjectList is standard list control all information 
regarding the process of creation of the control is the same as for 
CListCtrl. You can explicitly create it using CListCtrl::Create function
or put it in dialog template and use it any Dialog Window. The only 
difference is that you don't explicitly populate it with data. You only 
have to call the following function after the control was created:

	BOOL PopulateList(CString sFolderStartFrom=_T(""));

where you can specify the path to folder that must be used as the root one
(if empty root folder was specified then desktop folder will be used as the 
root one). This function will populate list control with Shell Namespace 
objects. You can specify a filter for files to be included in the list control
using this function:

	void SetFilter(LPCTSTR lpszExtentions);


After control is populated userr can navigate through the list control 
in the way they would do that in Windows Explorer. That means that if you 
double click on a folder item the list control will be automatically 
repopulated with its descendands. Or if you right click the context menu 
will popup and you can invoke listed commands on the selected item.

In order to retrieve the full path of a folder or a file for the specified
item you can call the following function:

	CString GetFullPath(int nItem) const;


These functions are the ones that will be used most often. Also 
there is set of Set/Get functions that are used in order to set/retrieve 
COXShellObjectList properties that define the details of the functionality
of the control. Refer to the class function reference for details on them.


We have to say that Shell Namespace objects can be deleted, created or renamed
at any time and the process of spying for such changes is not trivial
to implement. Ultimate Toolbox library has the following class that implements
the functionality we need: COXFileWatcher. We decided to use it in the 
COXShellObjectList in order to update the displayed contents.
We consider it as an advanced functionality that you might or might not like
to have in your applications. By default it is not supported but you can
include this functionality in your application by putting the following define 
in your project settings:

	OXSHELLLIST_WATCHFORDIR


We have to note that COXShellObjectList uses COXShellNamespaceNavigator class
in order to enumerate Shell Namespace objects. So you have to include
the reference to OXShellNamespaceNavigator.cpp file into your project either
(unless you don't link to the Ultimate Toolbox library file or DLL).


The sample that demonstrates the functionality of the COXShellObjectList class
can be found in the .\Samples\Advanced\FileExplorer. FileExplorer duplicates
the functionality of the Window Explorer appliction and can be used as a good 
example of using Shell Namespace navigation classes together.


Dependencies:

	#include "OXShellObjectList.h"

Source code files:
	
	"OXShellObjectList.cpp"	
	"OXShellNamespaceNavigator.cpp"

	"OXFileWatcher.cpp" (if OXSHELLLIST_WATCHFORDIR is defined)

*/



#if !defined(_OXSHELLOBJECTLIST_H_)
#define _OXSHELLOBJECTLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// OXShellObjectList.h : header file
//

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif	//	__AFXTEMPL_H__


#include "OXShellNamespaceNavigator.h"

#ifdef OXSHELLLIST_WATCHFORDIR
#include "OXFileWatcher.h"
#endif	//	OXSHELLLIST_WATCHFORDIR


/////////////////////////////////////////////////////////////////////////////
// COXShellObjectList window

class OX_CLASS_DECL COXShellObjectList : public CListCtrl
{
// Construction
public:
	// --- In:		bEnableContextMenu		-	if TRUE then popup context 
	//											menu will be displayed when 
	//											mouse is right clicked over 
	//											folder item
	//				bOnlyFileSystemItems	-	if TRUE the only file system 
	//											items will be displayed
	//				bNotifyError			-	if TRUE then message box will
	//											be displayed all the time
	//											any shell error happened
	//											(e.g. "A:\ is not accessible"
	//											will be displayed when you 
	//											try to explore invalid floppy
	//											disk)
	//				lpszFilter				-	the string represents ORed file 
	//											extentions so only the files that have
	//											matching extention will be allowed 
	//											to be included in the list, e.g.
	//											_T("cpp|h|rc"). If empty then all 
	//											files are allowed to be included
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Constructs the list control and initializes 
	//				internal settings
	COXShellObjectList(BOOL bEnableContextMenu=TRUE,
		BOOL bOnlyFileSystemItems=TRUE, BOOL bNotifyError=TRUE,
		LPCTSTR lpszFilter=_T(""));

// Attributes
public:

protected:
	// helper object used to navigate through Shell Namespace
	COXShellNamespaceNavigator m_navigator;

	// the folder which contents is viewed in the list
	LPSHELLFOLDER m_lpsfCurrentParentFolder;
	// full IDL of the folder which contents is viewed in the list
	LPITEMIDLIST m_lpFullIDLCurrentParentFolder;

	// flag the specify the functionality of the control. Refer to the 
	// constructor parameters for details
	BOOL m_bEnableContextMenu;
	BOOL m_bOnlyFileSystemItems;
	BOOL m_bNotifyError;
	//////////////////////////////////////////

	// image list used in the control for drawing large images
	CImageList m_imageListLarge;
	// shell image list for large images
	HIMAGELIST m_hShellImageListLarge;

	// image list used in the control for drawing small images
	CImageList m_imageListSmall;
	// shell image list for large images
	HIMAGELIST m_hShellImageListSmall;

	// map that associate the list images with the shell images
	CMap<int,int,int,int> m_mapImageIndex;


	// array of extensions that should be used as a filter for the files
	CArray<CString,CString> m_arrFilter;

	// flag that specifies that list sontrol has been initialized
	BOOL m_bInitialized;

#ifdef OXSHELLLIST_WATCHFORDIR
	// file watcher object that will spy for any changes in the root folder
	COXFileWatcher	m_fileWatcher;
#endif	//	OXSHELLLIST_WATCHFORDIR

	// semaphor that specifies how many times SetRedraw() function has been 
	// called (for internal use)
	int m_nRedraw;

// Operations
public:

	// --- In:		sFolderStartFrom		-	path to root folder. 
	//											If empty then
	//											Desktop folder will be used 
	//											as the root one
	//				lpFolder				-	pointer to IShellFolder interface 
	//											of the root folder
	//				lpFullIDL				-	root folder fully qualified IDL 
	//											
	// --- Out:		 
	// --- Returns:	TRUE if the control was successfuly populated with
	//				Shell Namespace items
	// --- Effect : Populates the control with Shell Namespace items
	//				from the specified root folder
	BOOL PopulateList(CString sFolderStartFrom=_T(""));
	BOOL PopulateList(const LPSHELLFOLDER lpFolder, const LPITEMIDLIST lpFullIDL);


	// --- In:		bEnableContextMenu		-	if TRUE then popup context 
	//											menu will be displayed when 
	//											mouse is right clicked over 
	//											Shell Namespace item
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies the availability of the 
	//				context menu
	inline void SetEnableContextMenu(BOOL bEnableContextMenu) {
		m_bEnableContextMenu=bEnableContextMenu;
	}

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if popup context menu will be shown or 
	//				FALSE otherwise
	// --- Effect : Retrieves the flag that specifies the availability of the 
	//				context menu
	inline BOOL GetEnableContextMenu() const { return m_bEnableContextMenu; }


	// --- In:		bOnlyFileSystemItems	-	if TRUE the only file system 
	//											folders and files will be 
	//											displayed
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies the scope of displayed
	//				Shell Namespace items
	inline void SetOnlyFileSystemItems(BOOL bOnlyFileSystemItems) {
		m_bOnlyFileSystemItems=bOnlyFileSystemItems;
	}

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if only file system folders and files will be 
	//				displayed or FALSE otherwise
	// --- Effect : Retrieves the flag that specifies the scope of the displayed
	//				Shell namespace items
	inline BOOL GetOnlyFileSystemItems() const { return m_bOnlyFileSystemItems; }


	// --- In:		bNotifyError			-	if TRUE then message box will
	//											be displayed all the time
	//											any shell error happened
	//											(e.g. "A:\ is not accessible"
	//											will be displayed when you 
	//											try to explore invalid floppy
	//											disk)
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies whether a user will be
	//				notified about shell errors or not (e.g. "A:\ is not 
	//				accessible" will be displayed when you try to explore 
	//				invalid floppy	disk)
	inline void SetNotifyError(BOOL bNotifyError) {
		m_bNotifyError=bNotifyError;
		if(::IsWindow(GetSafeHwnd()))
		{
			if(m_bNotifyError)
				m_navigator.SetOwnerWnd(this);
			else
				m_navigator.SetOwnerWnd(NULL);
		}
	}

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if all shell errors will be displayed using 
	//				standard message box
	// --- Effect : Retrieves the flag that specifies whether a user will be
	//				notified about shell errors or not (e.g. "A:\ is not 
	//				accessible" will be displayed when you try to explore 
	//				invalid floppy	disk)
	inline BOOL GetNotifyError() const { return m_bNotifyError; }


	// --- In:		nItem		-	index of the list item
	// --- Out:		 
	// --- Returns:	Full path for the specified list item
	// --- Effect : Retrieves the full path of the specified Shell Namespace item. 
	//				If item doesn't belong to file system then an empty 
	//				string will be returned
	CString GetFullPath(int nItem) const;


	// --- In:		bRedraw		-	if TRUE then any changes to the window will 
	//								cause it to be redrawn, otherwise any changes
	//								will be applied at once after this flag is 
	//								set back to TRUE
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies if changes to the control 
	//				will be applied immediately or not
	void SetRedraw(BOOL bRedraw=TRUE);

	
	// --- In:		lpszFilter		-	the string represents ORed file 
	//									extentions so only the files with have
	//									matching extention will be allowed 
	//									to be included in the list, e.g.
	//									_T("cpp|h|rc"). If empty then all 
	//									files are allowed to be included
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the filter that specifies extentions for files 
	//				to be included in the list
	void SetFilter(LPCTSTR lpszExtentions);

	// --- In:		
	// --- Out:		 
	// --- Returns:	The string represents ORed file extentions so only the 
	//				files that have matching extention will be allowed to be 
	//				included in the list, e.g. _T("cpp|h|rc"). If empty then 
	//				all files are allowed to be included
	// --- Effect : Retrieves the filter that specifies extentions for files 
	//				to be included in the list
	CString GetFilter() const;


	// --- In:		nItem		-	index of the item for which the specified
	//								command should be invoked. 
	//				nCmdID		-	command ID to be invoked. One of the 
	//								following:
	//
	//						IDCMD_RENAME	-	rename the item
	//						IDCMD_DELETE	-	delete the item
	//						IDCMD_PROPERTIES-	display the item's properties
	//						IDCMD_CUT		-	cut the item
	//						IDCMD_COPY		-	copy the item
	//						IDCMD_PASTE		-	paste the previously copied item
	//											in the folder if specified item
	//											is folder or in the parent folder
	//											if specified item is file
	// --- Out:		
	// --- Returns:	TRUE if command was successfully invoked or FALSE
	//				otherwise
	// --- Effect : Invokes menu command from the context menu that is associated
	//				with specified item
	inline BOOL InvokeCommand(int nItem, UINT nCmdID) const 
	{
		ASSERT(nItem!=-1);
		// Get folder info  associated with item
		LPNAMESPACEOBJECT lpNameSpaceObject=(LPNAMESPACEOBJECT)GetItemData(nItem);
		ASSERT(lpNameSpaceObject!=NULL);
		return m_navigator.InvokeCommand(lpNameSpaceObject->lpsfParent,
			lpNameSpaceObject->lpRelativeIDL,nCmdID,CMF_EXPLORE|CMF_CANRENAME);
	}


protected:
	// function is called once before displaying the list control
	virtual BOOL Init();

	// helper function that navigate through the items within specified
	// parent folder and add them into the list control
	BOOL FillListWithSubitems(const LPSHELLFOLDER lpFolder, 
		const LPITEMIDLIST lpFullIDL);

	// filter function that can be overridden in order to filter Shell Namespace
	// items displayed in the list control
	virtual BOOL IsQualified(LPNAMESPACEOBJECT lpNameSpaceObject)
	{
		UNREFERENCED_PARAMETER(lpNameSpaceObject);
		return TRUE;
	}
	
	// function is called every time before including the file in the list 
	// in order to check if the file extention matches the filter
	virtual BOOL IsMatchingFilter(LPCTSTR lpszFileName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXShellObjectList)
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In:
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Destructs the list control
	virtual ~COXShellObjectList();


	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if the list contents was successfully refreshed,
	//				or FALSE otherwise
	// --- Effect : Refreshes the contents of the list
	BOOL Refresh();


	// Generated message map functions
protected:
	//{{AFX_MSG(COXShellObjectList)
	afx_msg BOOL OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
#ifdef OXSHELLLIST_WATCHFORDIR
	afx_msg LRESULT OnDirChangeNotify(WPARAM wParam, LPARAM lParam);
#endif	//	OXSHELLLIST_WATCHFORDIR
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXSHELLOBJECTLIST_H_)
