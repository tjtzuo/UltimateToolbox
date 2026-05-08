// ==========================================================================
//							Class Specification : 
//				COXBaseBrowseDirEdit & COXBrowseDirEdit
//				COXBaseBrowseDirEdit16 & COXBrowseDirEdit16
//				COXBaseBrowseFileEdit & COXBrowseFileEdit
// ==========================================================================

// Header file : OXBrowseDirEdit.h

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


COXBaseBrowseDirEdit is the template class derived from COXDropEdit, which in its
turn is the  template class that provides the functionality of adding dropdown button
to any edit control. Dropdown button is displayed on the right or left side of the 
edit box and takes all height of the control window. The width of the dropdown button 
can be set programmatically (by default we use the width of the current width of 
the scroll bar). 

COXBaseBrowseDirEdit class provides the implementation of the dropdown event that 
happens when the dropdown button is clicked. This includes displaying the Directory
Picker dialog that implemented using our COXFolderPickerDlg class. You can specify
the title of the Directory Picker dialog using the following function:

	SetBrowseDialogTitle();

To retrieve the title call the function:

	GetBrowseDialogTitle();

The directory that is picked in the Directory Picker dialog is saved and can be retrieved
or programmtically set using these functions:

	GetPath();
	SetPath();


As you can see COXBaseBrowseDirEdit is the template class that allows us to use
any edit control class as the base one. We provide our own COXBaseBrowseDirEdit
derivation that uses our COXEdit class as the base edit control class. The class
is named as COXBrowseDirEdit and declared as following:

	class OX_CLASS_DECL COXBrowseDirEdit : 
		public COXBaseBrowseDirEdit<COXEdit> {};

We chose COXEdit as the base because it provides a lot of functionality that
might be extremely useful (e.g. text and background color, mask).


Also we've desined and implemented the following two template classes that are
derived from COXBaseBrowseDirEdit:

1)	COXBaseBrowseDirEdit16
	
	Provides the same functionality as COXBaseBrowseDirEdit, the only difference is 
	that it uses COXDirectoryDialog class to display the Directory Picker dialog. 

	We provide our own COXBaseBrowseDirEdit16 derivation that uses our COXEdit 
	class as the base edit control class. The class is named as COXBrowseDirEdit16 
	and declared as following:

	class OX_CLASS_DECL COXBrowseDirEdit16 : 
		public COXBaseBrowseDirEdit16<COXEdit> {};


2)	COXBaseBrowseFileEdit

	Provides the same functionality as COXBaseBrowseDirEdit, the only difference is 
	that it uses standard CFileDialog class to display the File Picker dialog. 
	The pointer to the dialog can be retrieved using the following function:

	inline CFileDialog* GetFileDialog() { return &m_fileDlg; }

	We provide our own COXBaseBrowseFileEdit derivation that uses our COXEdit 
	class as the base edit control class. The class is named as COXBrowseFileEdit 
	and declared as following:

	class OX_CLASS_DECL COXBaseBrowseFileEdit : 
		public COXBrowseFileEdit<COXEdit> {};


  


Take a look at the ExtendedEditControls sample that can be found in the 
.\Samples\gui\ExtendedEditControls subdirectory of your Ultimate Toolbox directory.
In the sample check out the 'Drop Edit' page that demonstrate the use of 
COXBrowseDirEdit, COXBrowseDirEdit16 and COXBrowseFileEdit classes.


Dependencies:
	#include "OXBrowseDirEdit.h"

Source files:
	"OXBrowseDirEdit.cpp"			-	COXBrowse* implementation
	"OXEdit.cpp"					-	COXEdit implementation
	"OXFolderPickerDlg.cpp"			-	COXFolderPickerDlg implementation
	"OXShellFolderTree.cpp"			-	COXFolderPickerDlg dependency
	"OXShellNamespaceNavigator.cpp"		
	"OXHistoryCombo.cpp"
	"OXDirDlg.cpp"					-	COXDirectoryDialog implementation
	"CopyTree.cpp"					-	COXDirectoryDialog dependency
	"Dir.cpp"
	"DirList.cpp"
	"DStrList.cpp"
	"File.cpp"
	"FileList.cpp"
	"Path.cpp"
	"PathIter.cpp"
	"XString.cpp"

Resource files:
	"OXFolderPickerDlg.rc"			-	COXFolderPickerDlg template
	"OXDirDlg.rc"					-	COXDirectoryDialog template

*/

#if !defined(_OXBROWSEDIREDIT_H__)
#define _OXBROWSEDIREDIT_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"


#include "OXEdit.h"
#include "OXDropEdit.h"
#include "OXFolderPickerDlg.h"
#include "OXDirDlg.h"
#include "OXMainRes.h"

/////////////////////////////////////////////////////////////////////////////
// COXBaseBrowseDirEdit window

template<class PARENTEDIT>
class COXBaseBrowseDirEdit : public COXDropEdit<PARENTEDIT>
{
// Construction
public:
	// --- In  :	lpszDialogTitle	-	title of the Directory Picker Dialog
	//									that will be used to choose the directory
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBaseBrowseDirEdit(int nID=IDS_OX_BROWSEDIREDITITLE /*_T("Browse for directory")*/) { 
		CString sTitle;
		sTitle.LoadString(nID);
		SetBrowseDialogTitle(sTitle); 
		m_bIsInDropDown=FALSE;
	}

	COXBaseBrowseDirEdit(LPCTSTR lpszDialogTitle) { 
		SetBrowseDialogTitle(lpszDialogTitle); 
		m_bIsInDropDown=FALSE;
	}

// Attributes
public:
	
protected:
	// dialog box title
	CString m_sDialogTitle;
	// flag that specifies if dropdown operation is currently undergoing
	BOOL m_bIsInDropDown;


// Operations
public:

	// --- In  :	
	// --- Out : 
	// --- Returns:	The title of the internal Directory Picker Dialog
	// --- Effect : Retrieves the title of the internal Directory Picker Dialog
	inline CString GetBrowseDialogTitle() const {return m_sDialogTitle; }

	// --- In  :	lpszDialogTitle	-	title of the internal Directory Picker Dialog
	// --- Out : 
	// --- Returns:
	// --- Effect : Sets the title of the internal Directory Picker Dialog
	inline void SetBrowseDialogTitle(const CString& sDialogTitle) {
		m_sDialogTitle=sDialogTitle;
	}


	// --- In  :	
	// --- Out : 
	// --- Returns:	The path that was typed in or chosen using Directory Picker Dialog
	// --- Effect : Retrieves the path set to the conrol
	virtual CString GetPath() const { 
		ASSERT(::IsWindow(GetSafeHwnd()));
		CString sText;
		GetWindowText(sText); 
		return sText;
	}

	// --- In  :	lpszPath	-	directory path to set to the control
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the path to the conrol
	virtual void SetPath(LPCTSTR lpszPath) { 
		ASSERT(::IsWindow(GetSafeHwnd()));
		SetWindowText(lpszPath); 
	}

// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBaseBrowseDirEdit() {};

protected:
	// called every time button is pressed
	virtual void OnDropButton();
};

//////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
void COXBaseBrowseDirEdit<PARENTEDIT>::OnDropButton()
{
	m_bIsInDropDown=TRUE;

	CString sDirectory=GetPath();
	TRY
	{
		// Ask the user to select a directory
		COXFolderPickerDlg folderPicker(sDirectory);
		folderPicker.SetDialogTitle(m_sDialogTitle);
		if(folderPicker.DoModal()==IDOK)
		{
			sDirectory=folderPicker.GetFolderPath();
			SetPath(sDirectory);
		}
		SetFocus();
	}
	CATCH_ALL(px)
	{
		// Catch all exception and NEVER pass them on to the calling
		// function (may not be MFC) !
		TRACE(_T("COXBaseBrowseDirEdit::OnDropButton: Catching exception, aborting function\n"));
	}
	END_CATCH_ALL

	m_bIsInDropDown=FALSE;
}


/////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXBrowseDirEdit : public COXBaseBrowseDirEdit<COXEdit>
{
	DECLARE_DYNCREATE(COXBrowseDirEdit)
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	The path that was typed in or chosen using Directory Picker Dialog
	// --- Effect : Retrieves the path set to the conrol
	virtual CString GetPath() const;

	// --- In  :	lpszPath	-	directory path to set to the control
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the path to the conrol
	virtual void SetPath(LPCTSTR lpszPath);
};


/////////////////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
class COXBaseBrowseDirEdit16 : public COXBaseBrowseDirEdit<PARENTEDIT>
{
// Construction
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBaseBrowseDirEdit16() {};

// Attributes
public:
	
// Operations
public:

// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBaseBrowseDirEdit16() {};

protected:
	// called every time button is pressed
	virtual void OnDropButton();
};

/////////////////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
void COXBaseBrowseDirEdit16<PARENTEDIT>::OnDropButton()
{
	m_bIsInDropDown=TRUE;

	CString sDirectory=GetPath();
	TRY
	{
		DWORD flags=OFN_SHOWHELP|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
		// Ask the user to select a directory
		COXDirectoryDialog dirDlg(sDirectory,m_sDialogTitle,TRUE,flags);
		if(dirDlg.DoModal()==IDOK)
		{
			sDirectory=dirDlg.GetDirectory();
			SetPath(sDirectory);
		}
		SetFocus();
	}
	CATCH_ALL(px)
	{
		// Catch all exception and NEVER pass them on to the calling
		// function (may not be MFC) !
		TRACE(_T("COXBaseBrowseDirEdit16::OnDropButton: Catching exception, aborting function\n"));
	}
	END_CATCH_ALL

	m_bIsInDropDown=FALSE;
}

/////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXBrowseDirEdit16 : public COXBaseBrowseDirEdit16<COXEdit>
{
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	The path that was typed in or chosen using Directory Picker Dialog
	// --- Effect : Retrieves the path set to the conrol
	virtual CString GetPath() const;

	// --- In  :	lpszPath	-	directory path to set to the control
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the path to the conrol
	virtual void SetPath(LPCTSTR lpszPath);

};

/////////////////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
class COXBaseBrowseFileEdit : public COXBaseBrowseDirEdit<PARENTEDIT>
{
// Construction
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBaseBrowseFileEdit() : m_fileDlg(TRUE,NULL,NULL,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("All Files (*.*)|*.*|")) {};

// Attributes
protected:
	// internal object used to choose file
	CFileDialog m_fileDlg;
	
// Operations
public:
	// --- In  :	
	// --- Out : 
	// --- Returns: The pointer to internal CFileDialog object that is used to 
	//				choose a file.
	// --- Effect : Retrieves the  pointer to internal CFileDialog object that is 
	//				used to choose a file. using this object you can change any
	//				of its setting.
	inline CFileDialog* GetFileDialog() { return &m_fileDlg; }

// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBaseBrowseFileEdit() {};

protected:
	// called every time button is pressed
	virtual void OnDropButton();
};

/////////////////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
void COXBaseBrowseFileEdit<PARENTEDIT>::OnDropButton()
{
	m_bIsInDropDown=TRUE;

	CString sFile=GetPath();
	TRY
	{
		if(m_fileDlg.m_ofn.lpstrFile==NULL)
			m_fileDlg.m_ofn.lpstrFile=(LPTSTR)(LPCTSTR)sFile;
		if(m_fileDlg.DoModal()==IDOK)
		{
			sFile=m_fileDlg.GetPathName();
			SetPath(sFile);
		}
		SetFocus();
	}
	CATCH_ALL(px)
	{
		// Catch all exception and NEVER pass them on to the calling
		// function (may not be MFC) !
		TRACE(_T("COXBrowseFileEdit::OnDropButton: Catching exception, aborting function\n"));
	}
	END_CATCH_ALL

	m_bIsInDropDown=FALSE;
}

/////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXBrowseFileEdit : public COXBaseBrowseFileEdit<COXEdit>
{
	DECLARE_DYNCREATE(COXBrowseFileEdit)
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	The path that was typed in or chosen using File Picker Dialog
	// --- Effect : Retrieves the path set to the conrol
	virtual CString GetPath() const;

	// --- In  :	lpszPath	-	file path to set to the control
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the path to the conrol
	virtual void SetPath(LPCTSTR lpszPath);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif / !defined(_OXBROWSEDIREDIT_H__)
