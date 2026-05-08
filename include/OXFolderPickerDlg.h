// ==========================================================================
// 					Class Specification : OXFolderPickerDlg
// ==========================================================================

// Header file : OXFolderPickerDlg.h

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


COXFolderPickerDlg is CDialog derived class. It is a dialog that was designed 
specifically to provide Folder Picker dialog for your applications. 
Internally this dialog uses COXShellFolderTree control to represent 
Shell Namespace folders. It is really easy to use it.

	1)	Instantiate the object of COXFolderPickerDlg. Refer to the 
		documentation on COXFolderPickerDlg constructor for details.

	2)	Call CDialog::DoModal() function in order to display the dialog.

	3)	After the dialog has been closed call 
		COXFolderPickerDlg::GetFolderPath() function to retrieve the 
		chosen folder full path


Dependencies:

	#include "OXFolderPickerDlg.h"


Source code files:

	"OXFolderPickerDlg.cpp"

  The following files implements shell folder tree control used in 
  the dialog and shell namespace navigator:

	"OXShellFolderTree.cpp"	
	"OXShellNamespaceNavigator.cpp"

  The following files implements history combobox used in the dialog:

	"OXHistoryCombo.cpp"
	

Resource files:
	
	include in the "Resource Includes ..." of the resource file of your 
	application the following reference

	#include "OXFolderPickerDlg.rc"

	in this resource file we define the dialog template for 
	COXFolderPickerDlg
*/


// OXFolderPickerDlg.h : header file
//

#if !defined(_OXFOLDERPICKERDLG_H_)
#define _OXFOLDERPICKERDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// dialog template
#include "OXMainRes.h"

#include "OXShellFolderTree.h"
#include "OXHistoryCombo.h"

/////////////////////////////////////////////////////////////////////////////
// COXFolderPickerDlg dialog

class OX_CLASS_DECL COXFolderPickerDlg : public CDialog
{
// Construction
public:
	// --- In:		sCurrentFolder			-	path to folder to be selected
	//											from the very beginning. If 
	//											empty the first folder in 
	//											COXShellFolderTree control 
	//											will be selected as current 
	//											one
	//				sRootFolder				-	path to root folder. If empty
	//											then Desktop folder will be 
	//											used as the root one
	//				bDisplayDesktopItem		-	if TRUE then "Desktop" item will 
	//											be used as the root one
	//				bShowPopupMenu			-	if TRUE then popup context 
	//											menu will be displayed when 
	//											mouse is right clicked over 
	//											folder item in 
	//											COXShellFolderTree control 
	//				bOnlyFileSystemFolders	-	if TRUE the only file system 
	//											folders will be displayed in
	//											COXShellFolderTree control 
	//				bShowFiles				-	if TRUE then files will be 
	//											displayed altogether with folders
	//				lpszFilter				-	the string represent ORed file 
	//											extentions so only the files that have
	//											matching extention will be allowed 
	//											to be included in the tree, e.g.
	//											_T("cpp|h|rc")
	//				bErrorNotify			-	if TRUE then message box will
	//											be displayed all the time
	//											any shell error happened
	//											(e.g. "A:\ is not accessible"
	//											will be displayed when you 
	//											try to explore invalid floppy
	//											disk) while surfing through
	//											COXShellFolderTree control 
	//				pParent					-	pointer to parent window.
	//											Can be NULL.
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Constructs the Folder Picker dialog
	COXFolderPickerDlg(CString sCurrentFolder=_T(""), CString sRootFolder=_T(""),
		BOOL bDisplayDesktopItem=TRUE, BOOL bShowPopupMenu=TRUE,
		BOOL bOnlyFileSystemFolders=TRUE, BOOL bShowFiles=FALSE,
		LPCTSTR lpszFilter=_T(""), BOOL bErrorNotify=TRUE, CWnd* pParent=NULL,
		int nIDTitle=IDS_OX_FOLDERPICKERDLGTITLE);


	// --- In:		
	// --- Out:		 
	// --- Returns:	Full path of the selected folder or empty string if
	//				none or non-file system folder was selected
	// --- Effect : Retrieves the path to selected folder
	inline CString GetFolderPath() const { return m_sCurrentFolder; }


	// --- In:		
	// --- Out:		 
	// --- Returns:	dialog title
	// --- Effect : Retrieves the dialog title
	inline CString GetDialogTitle() const { return m_sTitle; }

	// --- In:		
	// --- Out:		 
	// --- Returns:	dialog title
	// --- Effect : Retrieves the dialog title
	inline void SetDialogTitle(const CString& sTitle) { m_sTitle=sTitle; }

	
// Dialog Data
	//{{AFX_DATA(COXFolderPickerDlg)
	enum { IDD = IDD_OX_FOLDERPICKER_DIALOG };
	COXHistoryCombo	m_cmbCurrentFolder;
	COXShellFolderTree	m_treeShellFolders;
	BOOL	m_bOnlyFileSystemFolders;
	BOOL	m_bShowPopupMenu;
	CString	m_sCurrentFolder;
	BOOL	m_bErrorNotify;
	BOOL	m_bDisplayDesktopItem;
	BOOL	m_bShowFiles;
	//}}AFX_DATA
	CString m_sFilter;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXFolderPickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// path to the root folder
	CString m_sRootFolder;
	// dialog title
	CString m_sTitle;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COXFolderPickerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckOnlyFileSystemFolders();
	afx_msg void OnCheckPopupMenu();
	afx_msg void OnSelChangedTreeShellFolders(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnCheckErrorNotify();
	afx_msg void OnButtonBrowseFolder();
	afx_msg void OnCheckShowFiles();
	afx_msg void OnCheckDisplayDesktopItem();
	afx_msg void OnRefreshAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXFOLDERPICKERDLG_H_)
