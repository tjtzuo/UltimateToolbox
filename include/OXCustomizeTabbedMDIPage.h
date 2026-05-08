// ==========================================================================
//			Class Specification: COXCustomizeTabbedMDIPage
// ==========================================================================

// Header file : OXCustomizeTabbedMDIPage.h

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

COXCustomizeTabbedMDIPage is COXCustomizePage-derived class. It implements one of the 
predefined (standard) customize pages that is responsible for customizing of the 
appearance of the TabbedMDI framework. TabbedMDI is MDI extension that displays 
standard tab control in which we create an item for every MDIChild window. The window 
icon and text will be associated with the corresponding tab item. Using tab control 
you can switch very fast between MDIChild windows just clicking on the tab item. 
Refer to TabbedMDI documentation for more details. 

This standard page will be automatically inserted in the customize manager if you
add the following define to your stdafx.h file:

	#define OX_CUSTOMIZE_TABBEDMDI

In order to initialize the page you must call the following function:

	virtual BOOL InitializeTabbedMDI(DWORD dwTabCtrlStyle=DEFAULT_TABCTRLSTYLE,
		DWORD dwOffset=ID_TABOFFSET, LPCTSTR lpszProfileName=_T("CustomizeTabbedMDI")); 

This class implements the set of virtual functions defined in the COXCustomizePage 
class that must be implemented by any page.

Internally this class uses COXTabClientWnd object in order to implement TabbedMDI.


The best way of learning about Customize manager capabilities is to take look at the 
VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. In the sample menu choose "View"-"Customize" in order to display the 
Customize manager window. 



Dependency:
	#include "OXCustomizeTabbedMDIPage.h"


Source code files:
	"OXCustomizeTabbedMDIPage.cpp"		
	"OXCustomizePage.cpp"		
	"OXLayoutManager.cpp"		
	"OXSeparator.cpp"		
	"OXOptionTreeCtrl.cpp"
	"OXTabClientWnd.cpp"
	

Resource files:
	"OXCustomizeManager.rc"		
	"OXOptionTreeCtrl.rc"		


*/

#if !defined(_OXCUSTOMIZETABBEDMDIPAGE_H__)
#define _OXCUSTOMIZETABBEDMDIPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"
#include "OXCustomizePage.h"
#include "OXTabClientWnd.h"
#include "OXOptionTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COXCustomizeTabbedMDIPage dialog

class OX_CLASS_DECL COXCustomizeTabbedMDIPage : public COXCustomizePage
{
	DECLARE_DYNCREATE(COXCustomizeTabbedMDIPage)
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXCustomizeTabbedMDIPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COXCustomizeTabbedMDIPage)
	enum { IDD = IDD_OX_PAGE_TABBEDMDI };
	COXOptionTreeCtrl	m_treeTabCtrlSettings;
	CSpinButtonCtrl	m_spinOffset;
	BOOL	m_bSupportTabbedMDI;
	DWORD	m_dwOffset;
	//}}AFX_DATA

protected:
	// internal object, implements TabbedMDI functionality
	COXTabClientWnd m_MTIClientWnd;
	// represents COXTabClientWnd style
	DWORD m_dwStyle;


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
	virtual BOOL IsSupportingApplyChanges() { return TRUE; }

	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if changes were applied successfully; FALSE otherwise
	// --- Effect : Applies the changes. Called internally by customize manager when 
	//				"Apply" button is pressed.
	virtual BOOL ApplyChanges();


	// --- In  :	
	// --- Out : 
	// --- Returns: The title of the page as it appears in customize manager's 
	//				shortcut bar
	// --- Effect : Retrieves the title of the page. This string will appear on the 
	//				corresponding item in the customize manager's shortcut bar
	virtual CString GetTitle() const {
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZETABBEDMDITITLE));
		return sRet;
		//_T("Tabbed MDI"); 
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
		return MAKEINTRESOURCE(IDB_OX_CUSTOMIZE_TABBEDMDI); 
	}
	
	// --- In  :	
	// --- Out : 
	// --- Returns: The tooltip for the page item as it appears in customize manager's 
	//				shortcut bar
	// --- Effect : Retrieves the tooltip for the page item. 
	virtual CString GetTooltip() const {
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZETABBEDMDITOOLTIP));
		return sRet;
		//_T("Customize Tabbed MDI settings"); 
	}


	// --- In  :	bIsOk	-	TRUE if the customize manager window is being closed 
	//							as a result of pressing "OK" button; FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE if the page was successfully closed; FALSE otherwise
	// --- Effect : Called by customize manager to close the page. Advanced 
	//				overridable, use your own implementation in the case you 
	//				need to handle the close event.
	virtual BOOL OnCloseManager(BOOL bIsOk);


	// --- In  :	dwTabCtrlStyle	-	tab control styles that will be 
	//									used while creating the internal tab control.
	//									Refer to the Windows SDK documentation
	//									for list of all available styles.
	//									The following styles are used by 
	//									default:
	//
	//									TCS_MULTILINE
	//									TCS_BOTTOM
	//									TCS_HOTTRACK
	//									TCS_SCROLLOPPOSITE
	//									TCS_RIGHTJUSTIFY
	//
	//				dwOffset		-	offset in points from the MDIFrame window
	//									client area where the tab control will be 
	//									displayed
	//				lpszProfileName	-	the name of the hive in the registry 
	//									where TabbedMDI settings is saved. 
	//									If non-NULL value is set then 
	//									settings is retrieved from the 
	//									registry right away. These can 
	//									overwrite the values specified in 
	//									the previous parameters
	//				bSupportTabbedMDI-	if TRUE then TabbedMDI will be activated 
	//									immediately
	// --- Out : 
	// --- Returns: TRUE if TabbedMDI was initialized successfully; 
	//				FALSE otherwise
	// --- Effect : Replaces standard MDI with TabbedMDI. TabbedMDI is initialized
	//				with parameters specified by this function arguments or retrieved
	//				from registry. Refer to TabbedMDI documentation for more details
	virtual BOOL InitializeTabbedMDI(DWORD dwTabCtrlStyle=DEFAULT_TABCTRLSTYLE,
		DWORD dwOffset=ID_TABOFFSET, LPCTSTR lpszProfileName=_T("CustomizeTabbedMDI"), 
		BOOL bSupportTabbedMDI=TRUE); 


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to internal COXTabClientWnd object
	// --- Effect : Retrieves internal COXTabClientWnd object that implements
	//				TabbedMDI functionality
	virtual COXTabClientWnd* GetTabClientWnd() { return &m_MTIClientWnd; }


protected:
	// populates the custom page controls with data
	void GetVars();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCustomizeTabbedMDIPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXCustomizeTabbedMDIPage();   // destructor

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
	//{{AFX_MSG(COXCustomizeTabbedMDIPage)
	afx_msg void OnCheckSupportTabbedmdi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COXHelperWnd* m_pHelpWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCUSTOMIZETABBEDMDIPAGE_H__)
