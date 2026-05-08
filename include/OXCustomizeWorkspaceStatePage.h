// ==========================================================================
//			Class Specification: COXCustomizeWorkspaceStatePage
// ==========================================================================

// Header file : OXCustomizeWorkspaceStatePage.h

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

COXCustomizeWorkspaceStatePage is COXCustomizePage-derived class. It implements one 
of the predefined (standard) customize pages that is responsible for saving and 
loading of the application workspace state. it allows you to remember the position and 
state of main frame window, position and state of MDIChild windows (only for MDI 
applications), position of splitter panes for any splitter window defined in 
application main frame and MDIChild windows (only MDI applications), and position
of control bars in mainframe window. It also provides a functionality for 
specifying which customize pages must be displayed in the customize manager shortcut 
bar control and what ones must be hidden. 

This standard page will be automatically inserted in the customize manager if you
add the following define to your stdafx.h file:

	#define OX_CUSTOMIZE_WORKSPACE_STATE

In order to initialize the page you must call the following function:

	virtual BOOL LoadWorkspaceState(BOOL bIncludeMainWnd=TRUE, 
									BOOL bIncludeBars=TRUE,
									BOOL bIncludeChildFrames=TRUE, 
									BOOL bIncludeSplitterPanes=TRUE,
									LPCTSTR lpszProfileName=_T("CustomizeWorkspaceState"));

This class implements the set of virtual functions defined in the COXCustomizePage 
class that must be implemented by any page.

This class internally uses COXWorkspaceState object for managing workspace state.
You can use this function in order to retrieve a pointer to this object:

	COXWorkspaceState* GetWorkspaceStateObject();


!!! NOTE !!!
Call LoadWorkspaceState() function only after the main frame window was successfully
loaded and associated objects were created (like document and view). 
CMainFrame::OnCreate() function doesn't satisfy this requirement. But you can 
call it after CFrameWnd::LoadFrame() has been called (for MDI application) or after
CWinApp::ProcessShellCommand() has been called (for SDI application) in 
InitInstance() of your application CWinApp-derived class


The best way of learning about Customize manager capabilities is to take look at the 
VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. In the sample menu choose "View"-"Customize" in order to display the 
Customize manager window. 



Dependency:
	#include "OXCustomizeWorkspaceStatePage.h"


Source code files:
	"OXCustomizeWorkspaceStatePage.cpp"		
	"OXCustomizePage.cpp"		
	"OXLayoutManager.cpp"		
	"OXSeparator.cpp"		
	"OXOptionTree.cpp"		
	"OXWorkspaceState.cpp"
	"OXChildFrameState.cpp"
	"OXSplitterColRowState.cpp"
	"OXDocTemplateSpy.cpp"
 

	

Resource files:
	"OXCustomizeManager.rc"		
	"OXOptionTree.rc"		


*/

#if !defined(_OXCUSTOMIZEWORKSPACESTATEPAGE_H__)
#define _OXCUSTOMIZEWORKSPACESTATEPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXCustomizePage.h"
#include "OXWorkspaceState.h"
#include "OXOptionTreeCtrl.h"
#include "OXSeparator.h"
#include "OXMainRes.h"


/////////////////////////////////////////////////////////////////////////////
// COXCustomizeWorkspaceStatePage dialog

class OX_CLASS_DECL COXCustomizeWorkspaceStatePage : public COXCustomizePage
{
	DECLARE_DYNCREATE(COXCustomizeWorkspaceStatePage)
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXCustomizeWorkspaceStatePage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COXCustomizeWorkspaceStatePage)
	enum { IDD = IDD_OX_PAGE_WORKSPACE_STATE };
	COXSeparator	m_separator;
	COXOptionTreeCtrl	m_treePages;
	BOOL	m_bIncludeBars;
	BOOL	m_bIncludeMainWnd;
	BOOL	m_bIncludeChildFrames;
	BOOL	m_bIncludeSplitterPanes;
	//}}AFX_DATA

protected:
	// worspace state organizer 
	COXWorkspaceState m_workspaceState;
	// map of all customize pages titles
	CMap<CString,LPCTSTR,BOOL,BOOL> m_mapPageNames;

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
		VERIFY(sRet.LoadString(IDS_OX_CSTMZEWRKSPACETITLE));
		return sRet;
		//_T("Workspace State"); 
	}

	// --- In  :	
	// --- Out : 
	// --- Returns: The resource string of the image of the page as it appears in 
	//				customize manager's shortcut bar
	// --- Effect : Retrieves the resource string of the image of the page. 
	//				This image will appear on the corresponding item in the customize 
	//				manager's shortcut bar
	virtual LPCTSTR GetImageResource() const 
	{ 
		return MAKEINTRESOURCE(IDB_OX_CUSTOMIZE_WORKSPACE_STATE); 
	}

	// --- In  :	
	// --- Out : 
	// --- Returns: The tooltip for the page item as it appears in customize manager's 
	//				shortcut bar
	// --- Effect : Retrieves the tooltip for the page item. 
	virtual CString GetTooltip() const 
	{ 
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZEWRKSPACETOOLTIP));
		return sRet;
		//_T("Customize Workspace State settings"); 
	}


	// --- In  :	bIsOk	-	TRUE if the customize manager window is being closed 
	//							as a result of pressing "OK" button; FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE if the page was successfully closed; FALSE otherwise
	// --- Effect : Called by customize manager to close the page. Advanced 
	//				overridable, use your own implementation in the case you 
	//				need to handle the close event.
	virtual BOOL OnCloseManager(BOOL bIsOk);

	
	// --- In  :	bIncludeMainWnd			-	if TRUE then info about application's 
	//											main window placement will be 
	//											saved/restored
	//				bIncludeBars			-	if TRUE then info about application's 
	//											control bar windows placement will be 
	//											saved/restored
	//				bIncludeChildFrames		-	if TRUE then info about application's 
	//											child windows placement will be 
	//											saved/restored
	//				bIncludeSplitterPanes	-	if TRUE then info about application's
	//											splitter windows panes placement will 
	//											be saved/restored
	//				lpszProfileName			-	the name of the hive in the registry 
	//											where workspace state organizer 
	//											settings are saved. If non-NULL value 
	//											is set then settings are retrieved 
	//											from the registry right away. These 
	//											can overwrite the values specified in 
	//											the previous parameters. Also it 
	//											retrieves information on what customize 
	//											pages must be displayed in the 
	//											customize manager shortcut bar and what
	//											ones must be hidden
	// --- Out : 
	// --- Returns: TRUE if workspace state settings were initialized successfully; 
	//				FALSE otherwise
	// --- Effect : Initializes the application's workspace state
	virtual BOOL LoadWorkspaceState(BOOL bIncludeMainWnd=TRUE, BOOL bIncludeBars=TRUE,
		BOOL bIncludeChildFrames=TRUE, BOOL bIncludeSplitterPanes=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeWorkspaceState"));


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if the state of the subject this customize page responsible 
	//				for was saved in the registry successfully; FALSE otherwise
	// --- Effect : Saves in the registry the state of the subject this customize 
	//				page responsible for. Called internally by customize manager.
	virtual BOOL SaveWorkspaceState(); 


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to COXWorkspaceState object that is responsible for 
	//				saving/loading workspace state
	// --- Effect : Retrieves a pointer to internal COXWorkspaceState object
	inline COXWorkspaceState* GetWorkspaceStateObject() { return &m_workspaceState;	}


	// --- In  :	pCustomizePage	-	pointer to a valid customize page object
	// --- Out : 
	// --- Returns: TRUE if specified page is displayed in the customize manager 
	//				shortcut bar
	// --- Effect : Retrieves a flag that states whether specified page is displayed 
	//				in the customize manager shortcut bar or not
	virtual BOOL IsViewablePage(COXCustomizePage* pCustomizePage);

protected:
	// retrieves internal variables
	void GetVars();
	// retrieves information about inserted customize pages
	void GetPagesInfo();

	// helper function for building unique key for every customize page
	virtual CString BuildPageKey(COXCustomizePage* pCustomizePage) const 
	{
		ASSERT(pCustomizePage!=NULL);
		CString sKey=pCustomizePage->GetTitle()+_T("~!@#$^")+
			pCustomizePage->GetGroup();
		return sKey;
	}


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCustomizeWorkspaceStatePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXCustomizeWorkspaceStatePage();

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
	//{{AFX_MSG(COXCustomizeWorkspaceStatePage)
	afx_msg void OnOptionChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCUSTOMIZEWORKSPACESTATEPAGE_H__)
