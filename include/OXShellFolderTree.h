// ==========================================================================
// 				Class Specification : COXShellFolderTree
// ==========================================================================

// Header file : OXShellFolderTree.h

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


COXShellFolderTree class is derived from CTreeCtrl and implements standard
tree control that is automatically populated with Shell Name space folders
and files (files can be excluded from the tree contents).

As long as COXShellFolderTree is standard tree control all information 
regarding the process of creation of the control is the same as for 
CTreeCtrl. You can explicitly create it using CTreeCtrl::Create function
or put it in dialog template and use it any Dialog Window. The only 
difference is that you don't explicitly populate it with data. You only 
have to call the following function after the control was created:

	BOOL InitializeTree(CString sFolderStartFrom=_T(""));

where you can specify the path to folder that must be used as the root one
(if empty root folder was specified then desktop folder will be used as the 
root one). This function will populate tree control with Shell Namespace 
objects. 

After that user can navigate through the tree control in the common way. 

The following function can be used in order to programmatically expand the 
tree control and select the corresponding item for the specified folder:

	BOOL OpenFolder(CString sFolderToOpen);
  
And, finally, in order to retrieve the full path of folder for the specified
item you can call the following function:

	CString GetFullPath(HTREEITEM hItem) const;


These three function are the ones that will be used most often. Also 
there is set of Set/Get functions that are used in order to set/retrieve 
COXShellFolderTree properties that define the details of the functionality
of the control. Refer to the class function reference for details on them.


Note that by default, COXShellFolderTree displays only system folders but 
you also can setup the control to show the file items as well. In order to 
do that you have to call the following function:

	void SetShowFiles(BOOL bShowFiles);


We have to say that Shell Namespace objects can be deleted, created or renamed
at any time and the process of spying for such changes is not trivial
to implement. Ultimate Toolbox library has the following class that implements
the functionality we need: COXFileWatcher. We decided to use it in the 
COXShellFolderTree in order to update the displayed contents. We
consider it as an advanced functionality that you might or might not like
to have in your applications. By default it is not supported but you can
include this functionality in your application by putting the following define 
in your project settings:

	OXSHELLTREE_WATCHFORDIR


We have to note that COXShellFolderTree uses COXShellNamespaceNavigator class
in order to enumerate Shell Namespace objects. So you have to include
the reference to OXShellNamespaceNavigator.cpp file into your project too
(unless you don't link to the Ultimate Toolbox library file or DLL).


The sample that demonstrates the functionality of the COXShellObjectList class
can be found in the .\Samples\Advanced\FileExplorer. FileExplorer duplicates
the functionality of the Window Explorer appliction and can be used as a good 
example of using Shell Namespace navigation classes together.


Dependencies:

	#include "OXShellFolderTree.h"

Source code files:
	
	"OXShellFolderTree.cpp"	
	"OXShellNamespaceNavigator.cpp"

	"OXFileWatcher.cpp" (if OXSHELLTREE_WATCHFORDIR is defined)

*/



#if !defined(_OXSHELLFOLDERTREE_H_)
#define _OXSHELLFOLDERTREE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif	//	__AFXTEMPL_H__

#ifndef __AFXMT_H__
#include <afxmt.h>
#define __AFXMT_H__
#endif

#include "OXShellNamespaceNavigator.h"

#ifdef OXSHELLTREE_WATCHFORDIR
#include "OXFileWatcher.h"
#endif	//	OXSHELLTREE_WATCHFORDIR


/////////////////////////////////////////////////////////////////////////////
// COXShellFolderTree window

class OX_CLASS_DECL COXShellFolderTree : public CTreeCtrl
{
// Construction
public:
	// --- In:		bEnableContextMenu		-	if TRUE then popup context 
	//											menu will be displayed when 
	//											mouse is right clicked over 
	//											folder item
	//				bOnlyFileSystemFolders	-	if TRUE the only file system 
	//											folders will be displayed
	//				bNotifyError			-	if TRUE then message box will
	//											be displayed all the time
	//											any shell error happened
	//											(e.g. "A:\ is not accessible"
	//											will be displayed when you 
	//											try to explore invalid floppy
	//											disk)
	//				bShowDesktopItem		-	if TRUE then the tree root item
	//											will be the 'Desktop' item
	//				m_bShowFiles			-	if TRUE then files will be displayed
	//											in the tree altogether with folders
	//				lpszFilter				-	the string represents ORed file 
	//											extentions so only the files that have
	//											matching extention will be allowed 
	//											to be included in the tree, e.g.
	//											_T("cpp|h|rc"). If empty then all 
	//											files are allowed to be included
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Constructs the tree control and initializes 
	//				internal settings
	COXShellFolderTree(BOOL bEnableContextMenu=TRUE,
		BOOL bOnlyFileSystemFolders=TRUE, BOOL bNotifyError=TRUE,
		BOOL bShowDesktopItem=TRUE, BOOL bShowFiles=FALSE,
		LPCTSTR lpszFilter=_T(""));

// Attributes
public:

protected:
	// helper object used to navigate through Shell Namespace
	COXShellNamespaceNavigator m_navigator;

	// flag the specify the functionality of the control. Refer to the 
	// constructor parameters for details
	BOOL m_bEnableContextMenu;
	BOOL m_bOnlyFileSystemFolders;
	BOOL m_bNotifyError;

	// flag that specifies if "Desktop" item should be displayed or not
	BOOL m_bShowDesktopItem;
	// flag that specifies if files should be displayed
	BOOL m_bShowFiles;

	// image list used in the control
	CImageList m_imageList;
	// shell image list
	HIMAGELIST m_hShellImageList;
	// map that associate the tree images with the shell images
	CMap<int,int,int,int> m_mapImageIndex;

	// array of extensions that should be used as a filter for the files (if included)
	CArray<CString,CString> m_arrFilter;

	// semaphor that specifies how many times SetRedraw() function has been called,
	// (for internal use)
	int m_nRedraw;

#ifdef OXSHELLTREE_WATCHFORDIR
	COXFileWatcher	m_fileWatcher;
#endif	//	OXSHELLTREE_WATCHFORDIR

	// flag that specifies that one of the tree items is currently edited 
	BOOL m_bEditingItem;

	// handle to the item for which context menu is about to be displayed
	HTREEITEM m_hContextMenuItem;
	// flag that specifies that restoration of the tree item states 
	// must not executed
	BOOL m_bNoRestoreContextMenuItem;

// Operations
public:

	// --- In:		sFolderStartFrom		-	path to root folder. 
	//											If empty then
	//											Desktop folder will be used 
	//											as the root one
	// --- Out:		 
	// --- Returns:	TRUE if the control was successfuly initialized with
	//				Shell Namespace folders
	// --- Effect : Populates the control with Shell Namespace folders
	//				starting from the specified root folder sFolderStartFrom
	BOOL InitializeTree(CString sFolderStartFrom=_T(""));

	// --- In:		sFolderToOpen			-	path to folder to open.
	//				lpFullIDL				-	fully quallyfied IDL of the folder 
	//											to open
	//				bExpand					-	if TRUE then folder, if found, will 
	//											be expanded 
	// --- Out:		 
	// --- Returns:	TRUE if the specified folder was found and selected
	// --- Effect : Expands the folders to the specified sFolderToOpen and 
	//				select it.
	BOOL OpenFolder(CString sFolderToOpen, BOOL bExpand=FALSE);
	BOOL OpenFolder(const LPITEMIDLIST lpFullIDL, BOOL bExpand=FALSE);


	// --- In:		sFolderToFind			-	path to folder to find.
	//				lpFullIDL				-	fully quallyfied IDL of the folder 
	//											to find
	// --- Out:		 
	// --- Returns:	handle to the corresponding tree item if found or NULL if failed 
	// --- Effect : Searches for the specified folder among the currently visible ones
	//				and returns handle to the corresponding tree item if found.
	HTREEITEM FindFolder(CString sFolderToFind);
	HTREEITEM FindFolder(const LPITEMIDLIST lpFullIDL);

	
	// --- In:		bEnableContextMenu		-	if TRUE then popup context 
	//											menu will be displayed when 
	//											mouse is right clicked over 
	//											folder item
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


	// --- In:		bOnlyFileSystemFolders	-	if TRUE the only file system 
	//											folders will be displayed
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies the scope of displayed
	//				Shell Namespace folders
	inline void SetOnlyFileSystemFolders(BOOL bOnlyFileSystemFolders) {
		m_bOnlyFileSystemFolders=bOnlyFileSystemFolders;
	}

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if only file system folders will be displayed or 
	//				FALSE otherwise
	// --- Effect : Retrieves the flag that specifies the scope of displayed
	//				Shell namespace folders
	inline BOOL GetOnlyFileSystemFolders() const { return m_bOnlyFileSystemFolders; }


	// --- In:		bShowFiles	-	if TRUE the files will be displayed altogether
	//								with folders
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies the scopr of displayed
	//				Shell namespace folders
	inline void SetShowFiles(BOOL bShowFiles) { m_bShowFiles=bShowFiles; }

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if only file system folders will be displayed or 
	//				FALSE otherwise
	// --- Effect : Retrieves the flag that specifies the scopr of displayed
	//				Shell namespace folders
	inline BOOL GetShowFiles() const { return m_bShowFiles; }


	// --- In:		bShowDesktopItem	-	if TRUE then "Desktop" item will be
	//										displayed as root item
	// --- Out:		 
	// --- Returns:	Flag that specifies the visibility of the "Desktop" item
	// --- Effect : Sets the flag that specifies the visibility of the "Desktop" item
	inline void SetShowDesktopItem(BOOL bShowDesktopItem) { 
		m_bShowDesktopItem=bShowDesktopItem; 
	}

	// --- In:		
	// --- Out:		 
	// --- Returns:	TRUE if "Desktop" item is displayed, or FALSE otherwise
	// --- Effect : Retrieves the the flag that specifies the visibility of the 
	//				"Desktop" item
	inline BOOL GetShowDesktopItem() const { return m_bShowDesktopItem; }


	// --- In:		bNotifyError			-	if TRUE then message box will
	//											be displayed all the time
	//											any shell error happened
	//											(e.g. "A:\ is not accessible"
	//											will be displayed when you 
	//											try to explore invalid floppy
	//											disk)
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Constructs the tree control and initializes 
	//				internal settings
	inline void SetNotifyError(BOOL bNotifyError) 
	{
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
	// --- Effect : Retrieves the flag that specifies whether user will be
	//				notified about shell errors or not (e.g. "A:\ is not 
	//				accessible" will be displayed when you try to explore 
	//				invalid floppy	disk)
	inline BOOL GetNotifyError() const { return m_bNotifyError; }


	// --- In:		hItem		-	handle to tree item
	// --- Out:		 
	// --- Returns:	Full folder path for the specified tree item
	// --- Effect : Retrieves the full path of Shell Namespace folder. 
	//				If folder doesn't belong to file system then an empty 
	//				string will be returned
	CString GetFullPath(HTREEITEM hItem) const;


	// --- In:		bRedraw		-	if TRUE then any changes to the window will 
	//								cause itto be redrawn, otherwise any changes
	//								will be applied at once after this flag is 
	//								set back to TRUE
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies if changes to the control 
	//				will be applied immediately or not
	void SetRedraw(BOOL bRedraw=TRUE);


	// --- In:		lpszFilter				-	the string represents ORed file 
	//											extentions so only the files that have
	//											matching extention will be allowed 
	//											to be included in the tree, e.g.
	//											_T("cpp|h|rc"). If empty then all 
	//											files are allowed to be included
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the filter that specifies extentions for files to be included
	//				in the tree
	void SetFilter(LPCTSTR lpszExtentions);

	// --- In:		
	// --- Out:		 
	// --- Returns:	The string represents ORed file extentions so only the files that 
	//				have matching extention will be allowed to be included in the tree, 
	//				e.g. _T("cpp|h|rc"). If empty then all files are allowed to be 
	//				included
	// --- Effect : Sets the filter that specifies extentions for files to be included
	//				in the tree
	CString GetFilter() const;


	// --- In:		hItem		-	handle of the item for which the specified
	//								command should be invoked	
	//				nCmdID		-	command ID to be invoked. One of the following:
	//
	//							IDCMD_RENAME	-	rename the item
	//							IDCMD_DELETE	-	delete the item
	//							IDCMD_PROPERTIES-	display the item's properties
	//							IDCMD_CUT		-	cut the item
	//							IDCMD_COPY		-	copy the item
	//							IDCMD_PASTE		-	paste the previously copied item
	//												in the folder if specified item
	//												is folder or in the parent folder
	//												if specified item is file
	// --- Out:		
	// --- Returns:	TRUE if command was successfully invoked or FALSE
	//				otherwise
	// --- Effect : Invokes menu command from the context menu that is associated
	//				with specified item
	inline BOOL InvokeCommand(HTREEITEM hItem, UINT nCmdID) const 
	{
		ASSERT(hItem!=NULL);
		// Get folder info  associated with item
		LPNAMESPACEOBJECT lpNameSpaceObject=(LPNAMESPACEOBJECT)GetItemData(hItem);
		if(lpNameSpaceObject==NULL)
			return FALSE;
		return m_navigator.InvokeCommand(lpNameSpaceObject->lpsfParent,
			lpNameSpaceObject->lpRelativeIDL,nCmdID,CMF_EXPLORE|CMF_CANRENAME);
	}



protected:
	// helper function that navigate through the items within specified
	// parent folder and add them into the tree control
	BOOL FillTreeWithSubfolders(HTREEITEM htiParent, 
		LPSHELLFOLDER lpFolder, LPITEMIDLIST lpFullIDL);

	// sort Shell Namespace folders in the way specified by Shell
	BOOL SortChildren(const HTREEITEM htiParent);

	// called for every Shell Namespace object before including it into the tree
	virtual BOOL IsQualified(LPNAMESPACEOBJECT lpNameSpaceObject)
	{
		UNREFERENCED_PARAMETER(lpNameSpaceObject);
		return TRUE;
	}

	// function is called every time before including the file in the tree in order
	// to check if the file extention matches the filter
	virtual BOOL IsMatchingFilter(LPCTSTR lpszFileName);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXShellFolderTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In:
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Destructs the tree control
	virtual ~COXShellFolderTree();


	// --- In:		lpszFolder	-	name of the folder which contents must be refreshed
	//				htiParent	-	handle to the parent item that define the tree 
	//								brunch that will be repopulated with items.
	// --- Out:		 
	// --- Returns:	TRUE if specified folder was successfully refreshed,
	//				or FALSE otherwise
	// --- Effect : Refreshes the contents of the brunch wich is specified by 
	//				its parent item. If TVI_ROOT is specified then the whole tree
	//				will be repopulated
	BOOL Refresh(LPCTSTR lpszFolder);
	BOOL Refresh(HTREEITEM htiParent=TVI_ROOT);

protected:
	void RestoreStateAfterContextMenu();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXShellFolderTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode(); 
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg BOOL OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg BOOL OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
#ifdef OXSHELLTREE_WATCHFORDIR
	afx_msg LRESULT OnDirChangeNotify(WPARAM wParam, LPARAM lParam);
#endif	//	OXSHELLTREE_WATCHFORDIR
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXSHELLFOLDERTREE_H_)
