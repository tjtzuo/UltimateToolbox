// ==========================================================================
//			Class Specification: COXCustomizeCommandsPage
// ==========================================================================

// Header file : OXCustomizeCommandsPage.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

/*
	Overview

COXCustomizeCommandsPage is COXCustomizePage-derived class. It implements one of the 
predefined (standard) customize pages that is responsible for customizing of the 
contents of the application toolbars and menus. You can add new buttons to any 
toolbar or new menu item or even popup menu or submenu to any menu. Buttons or menu 
items can be deleted, copied or reallocated. It also provides a functionality 
for customizing the appearance of menu items and toolbar buttons (you can change the 
image and text associated with the particular command). The changed contents is 
saved in the registry when application is closed and when it is opened next time the 
last used contents will be retrieved from the registry and applied

The customization functionality provided by this class depend on the fact that you 
use the COXCoolToolBar class for your toolbars and the COXMenuBar class for your 
menu (that means that in SDI/MDI application your CMainFrame  window must be declared
using COXMenuBarFrame template class).

This standard page will be automatically inserted in the customize manager if you
add the following define to your stdafx.h file:

	#define OX_CUSTOMIZE_COMMANDS

In order to initialize the page you must call the following function:

	virtual BOOL InitializeCommands(LPCTSTR lpszCustomImagesResource,
									COLORREF clrMask=RGB(192,192,192), 
									BOOL bShowIconsInMenu=TRUE, 
									LPCTSTR lpszProfileName=_T("CustomizeCommands"));

This class implements the set of virtual functions defined in the COXCustomizePage 
class that must be implemented by any page.




The best way of learning about Customize manager capabilities is to take look at the 
VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. In the sample menu choose "View"-"Customize" in order to display the 
Customize manager window. 



Dependency:
	#include "OXCustomizeCommandsPage.h"


Source code files:
	"OXCustomizeCommandsPage.cpp"		
	"OXCustomizePage.cpp"		
	"OXLayoutManager.cpp"		
	"OXSeparator.cpp"		
	"OXBitmapButton.cpp"
	"OXCoolToolBar.cpp"
	"OXMenuBar.cpp"
	"OXBitmapMenuOrganizer.cpp"
	"OXBitmapMenu.cpp"
	"OXButtonAppearanceDlg.cpp"
	"OXImageListBox.cpp"
	"OXDragDropCommands.cpp"
	

Resource files:
	"OXCustomizeManager.rc"		
	"OXBitmapButton.rc"		


*/

#if !defined(_OXCUSTOMIZECOMMANDSPAGE_H__)
#define _OXCUSTOMIZECOMMANDSPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXCustomizePage.h"
#include "OXBitmapButton.h"
#include "OXSeparator.h"
#include "OXBitmapMenuOrganizer.h"
#include "OXMenuBar.h"
#include "OXMainRes.h"

/////////////////////////////////////////////////////////////////////////////
// COXCustomizeCommandsPage dialog

class OX_CLASS_DECL COXCustomizeCommandsPage : public COXCustomizePage
{
	DECLARE_DYNCREATE(COXCustomizeCommandsPage)
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXCustomizeCommandsPage();

// Dialog Data
	//{{AFX_DATA(COXCustomizeCommandsPage)
	enum { IDD = IDD_OX_PAGE_COMMANDS };
	COXSeparator	m_sepDescription;
	COXSeparator	m_sepCommands;
	COXSeparator	m_sepOptions;
	COXBitmapButton	m_btnModifySelection;
	CTreeCtrl	m_treeCommands;
	CString	m_sDescription;
	BOOL	m_bShowIconsInMenu;
	BOOL	m_bShowOnlyRecentlyUsedMenuItems;
	//}}AFX_DATA


protected:
	// to show bitmaps in menu
	COXBitmapMenuOrganizer m_menuOrganizer;

	// to customize menus
	CMap<COXBitmapMenuPopupWnd*,COXBitmapMenuPopupWnd*,int,int> m_mapBMPopupWnd;

	// array of IDs that will be set for all toolbars
	UINT* m_arrCommandIds;

	// map of command IDs and corresponding image indexes. If there is no
	// image associated with a command then image index equals -1
	CMap<UINT,UINT,int,int> m_mapCommands;

	// map of custom command IDs and corresponding description strings. 
	// All custom command IDs are saved in m_mapCommands as well where 
	// they are associated with images
	CMap<UINT,UINT,CString,CString> m_mapCustomCommands;

	// map of custom command IDs and corresponding hierarchy strings. All custom command IDs
	// are saved in m_mapCommands as well where they are associated with images
	CMap<UINT,UINT,CString,CString> m_mapCustomCommandsHierarchy;

	// image list of all images for all commands
	CImageList m_ilImages;

	// image list of all custom images
	CImageList m_ilCustomImages;

	// currently customizable toolbar
	COXCoolToolBar* m_pCustomizedToolbar;

	// currently customizable popup menu (index of the button in the menu bar)
	int m_nCustomizedPopupMenuIndex;

	// handle to current custom button
	HWND m_hCustomButtonWnd;

public:
	// --- In  :	pCustomizeManager	-	a pointer to COXCustomizeManager object
	//										that contains this page and is about to 
	//										load it
	// --- Out : 
	// --- Returns:	TRUE if page was successfully loaded; FALSE otherwise
	// --- Effect : Called by customize manager while activating the page. 
	//				This function might be called multiple times. Before 
	//				activating another page customize manager will call
	//				Unload() function
	virtual BOOL Load(const COXCustomizeManager* pCustomizeManager);

	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect : Called by customize manager when currently active page is about 
	//				to be deactivated. Matches Load() function call
	virtual void Unload();
	
	
	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if changes can be applied immediately; FALSE otherwise
	// --- Effect : Retrieves the flag that specifies that changes in toolbar 
	//				appearance settingd can be applied immediately. 
	//				Called internally by customize manager in order to enable/disable
	//				the "Apply" button.
	virtual BOOL IsSupportingApplyChanges() { return FALSE; }


	// --- In  :	
	// --- Out : 
	// --- Returns: The title of the page as it appears in customize manager's 
	//				shortcut bar
	// --- Effect : Retrieves the title of the page. This string will appear on the 
	//				corresponding item in the customize manager's shortcut bar
	virtual CString GetTitle() const {
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZECOMMANDSTITLE));
		return sRet;
		/*_T("Commands"); */}

	// --- In  :	
	// --- Out : 
	// --- Returns: The resource string of the image of the page as it appears in 
	//				customize manager's shortcut bar
	// --- Effect : Retrieves the resource string of the image of the page. 
	//				This image will appear on the corresponding item in the customize 
	//				manager's shortcut bar
	virtual LPCTSTR GetImageResource() const 
	{ 
		return MAKEINTRESOURCE(IDB_OX_CUSTOMIZE_COMMANDS); 
	}
	
	// --- In  :	
	// --- Out : 
	// --- Returns: The tooltip for the page item as it appears in customize manager's 
	//				shortcut bar
	// --- Effect : Retrieves the tooltip for the page item. 
	virtual CString GetTooltip() const 
	{ 
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZECOMMANDSTOOLTIP));
		return sRet;
		//_T("Customize the contents of the toolbars and menus"); 
	}


	// --- In  :	bIsOk	-	TRUE if the customize manager window is being closed 
	//							as a result of pressing "OK" button; FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE if the page was successfully closed; FALSE otherwise
	// --- Effect : Called by customize manager to close the page. Advanced 
	//				overridable, use your own implementation in the case you 
	//				need to handle the close event.
	virtual BOOL OnCloseManager(BOOL bIsOk);


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if the contents of the application's toolbars and menus 
	//				was saved in the registry successfully; FALSE otherwise
	// --- Effect : Saves in the registry the contents of the application's 
	//				toolbars and menus. Called internally by customize manager.
	virtual BOOL SaveWorkspaceState();


	// --- In  :	nCustomImagesResourceID	-	ID of the bitmap that specifies 
	//											customize images that can be 
	//											associated with existing commands
	//											and displayed next to buttons in 
	//											toolbars and menu items in menus.
	//											We provide predefined set of
	//											customize images
	//				lpszCustomImagesResource-	resource string the defines the 
	//											bitmap that specifies 
	//											customize images that can be 
	//											associated with existing commands
	//											and displayed next to buttons in 
	//											toolbars and menu items in menus.
	//				clrMask					-	color of the mask for the specified
	//											bitmap. By default set to 
	//											RGB(192,192,192)
	//				bShowIconsInMenu		-	if TRUE an images will be displayed 
	//											next to menu items in menus. 
	//											By default set to TRUE
	//				bShowOnlyRecentlyUsedMenuItems	-	if TRUE then only recently 
	//											used menu items will be displayed.
	//											By default set to TRUE
	//				lpszProfileName			-	the name of the hive in the registry 
	//											where commands settings is saved. If
	//											non-NULL value is set then settings 
	//											is retrieved from the registry right 
	//											away. These can overwrite the values 
	//											specified in the previous parameters.
	//											Also the saved contents of the 
	//											application's toolbars and menus will 
	//											retrieved and applied
	// --- Out : 
	// --- Returns: TRUE if information on the application's commands was initialized 
	//				successfully; FALSE otherwise
	// --- Effect : Initializes the information on the application's commands and 
	//				retrieves the saved contents of the application's toolbars and 
	//				menus and applies it
	inline BOOL InitializeCommands(
		UINT nCustomImagesResourceID=IDB_OX_IL_CUSTOMIZE_CUSTOM_IMAGES,
		COLORREF clrMask=RGB(192,192,192), BOOL bShowIconsInMenu=TRUE, 
		BOOL bShowOnlyRecentlyUsedMenuItems=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeCommands")) 
	{
		return InitializeCommands(MAKEINTRESOURCE(nCustomImagesResourceID),clrMask,
			bShowIconsInMenu,bShowOnlyRecentlyUsedMenuItems,lpszProfileName);
	}
	virtual BOOL InitializeCommands(LPCTSTR lpszCustomImagesResource,
		COLORREF clrMask=RGB(192,192,192), BOOL bShowIconsInMenu=TRUE, 
		BOOL bShowOnlyRecentlyUsedMenuItems=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeCommands"));


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to internal COXBitmapMenuOrganizer object
	// --- Effect : Retrieves a pointer to internal COXBitmapMenuOrganizer object. 
	//				This object is used to handle menus in the framework.
	virtual COXBitmapMenuOrganizer* GetBMOrganizer() { return &m_menuOrganizer; }


	// --- In  :	pToolbar	-	a pointer to a toolbar which contents 
	//								will be saved
	// --- Out : 
	// --- Returns: TRUE if the toolbar's contents was successfully saved;
	//				FALSE otherwise
	// --- Effect : Saves the contents of the specified toolbar to the registry
	virtual BOOL SaveToolbarState(CToolBar* pToolbar);

	// --- In  :	pToolbar	-	a pointer to a toolbar which contents 
	//								will be retrieved from the registry and applied
	// --- Out : 
	// --- Returns: TRUE if the toolbar's contents was successfully retrieved and
	//				applied; FALSE otherwise
	// --- Effect : Retrieves the contents of the specified toolbar from the registry
	//				and applies it
	virtual BOOL LoadToolbarState(CToolBar* pToolbar);


	// --- In  :	pToolbar	-	a pointer to a toolbar that should be converted 
	//								in order to be customizable
	// --- Out : 
	// --- Returns: 
	// --- Effect : Converts the specified toolbar to be customizable by this page
	virtual void ConvertToolbar(CToolBar* pToolbar);

	virtual BOOL AddCustomCommand(UINT nCmdID, LPCTSTR pszCmdText, 
		int nImageIndex=-1, LPCTSTR pszHierarchy=_T("Custom commands"));

	virtual BOOL AddCustomCommand(UINT nCmdID, int nIDCmd=IDS_OX_CSTMZECOMMANDSADDCUSTOM/* "Custom command" */, 
		int nImageIndex=-1, int nIDHierarchy=IDS_OX_CSTMZECOMMANDSHIERARCHY);//"Custom commands"

	virtual BOOL IsCustomCommand(UINT nCmdID);
	virtual BOOL GetCustomCommand(UINT nCmdID, CString& sText, 
		int& nImageIndex, CString& sHierarchy);

protected:
	// retrieves internal variables
	void GetVars();
	// adds toolbars' custom buttons to tree
	void AddCustomButtonsFromToolbars(HTREEITEM hParentItem);
	// retrieves the set of command from a menu (recursive function)
	void RetrieveCommandsFromMenu(HMENU hMenu);
	// adds the contents of the specified menu to tree (recursive function)
	void AddCommandsFromMenuToTree(HMENU hMenu, HTREEITEM hParentItem);
	// searches for specified item in the tree
	HTREEITEM FindMenuItemInTree(CString sText, HTREEITEM hParentItem) const;
	// adds custom command to tree 
	BOOL AddCustomCommandsToTree();
	// searches for specified command in the tree
	HTREEITEM FindCommandInTree(UINT nCmdID, HTREEITEM hParentItem) const;

	// retrieves a pointer to available COXBitmapMenuPopupWnd object
	// (used internally to customize menu)
	COXBitmapMenuPopupWnd* GetFirstFreeBMPopupWnd(int nMenuLevel=0);
	// hide COXBitmapMenuPopupWnd object up to specified leve
	void ResetPopupMenus(int nMenuLevel=0);
	// retrieves a level this COXBitmapMenuPopupWnd object at
	int GetBMPopupWndLevel(COXBitmapMenuPopupWnd* pBMPopupWnd);

	// build menu starting from the specified parent
	HMENU BuildMenuFromTree(HTREEITEM hParentItem);

	// build array of command ids for toolbars
	void BuildArrayCommandIDs();

	// reposition custom button preview window
	void PreviewCustomButtonWindow();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCustomizeCommandsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXCustomizeCommandsPage();

protected:
	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect : Called internally just after the page window was created. 
	//				Override this function in order to initialize the controls 
	//				on the page. Don't forget to call the default implementation 
	//				unless you duplicate it in your code
	virtual void OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(COXCustomizeCommandsPage)
	afx_msg void OnBegindragTreeCommands(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeCommands(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnDropDownModifySelection(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnTBCustomizeCommand(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBMCustomizeCommand(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckIconsInMenu();
	afx_msg void OnCheckShowOnlyRecentlyUsed();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCUSTOMIZECOMMANDSPAGE_H__)
