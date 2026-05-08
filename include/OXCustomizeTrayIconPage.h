// ==========================================================================
//			Class Specification: COXCustomizeTrayIconPage
// ==========================================================================

// Header file : OXCustomizeTrayIconPage.h

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

COXCustomizeTrayIconPage is COXCustomizePage-derived class. It implements one of the 
predefined (standard) customize pages that is responsible for customizing of the 
appearance of the trayicons. 

This standard page will be automatically inserted in the customize manager if you
add the following define to your stdafx.h file:

	#define OX_CUSTOMIZE_TRAYICON

In order to initialize the page you must call the following function:

InitializeTrayIcon(HICON hIcon, UINT nContextMenuResourceID,
				   LPCTSTR lpszIconTooltip=NULL, 
				   BOOL bShowTrayIcon=TRUE, 
				   BOOL bUseRightClick=TRUE, 
				   LPCTSTR lpszIconSourceFileName=NULL,
				   LPCTSTR lpszProfileName=_T("CustomizeTrayIcon"))

This class implements the set of virtual functions defined in the COXCustomizePage 
class that must be implemented by any page.

The best way of learning about Customize manager capabilities is to take look at the 
VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. In the sample menu choose "View"-"Customize" in order to display the 
Customize manager window. 



Dependency:
	#include "OXCustomizeTrayIconPage.h"


Source code files:
	"OXCustomizeTrayIconPage.cpp"		
	"OXCustomizePage.cpp"		
	"OXSeparator.cpp"		
	"OXHistoryCombo.cpp"
	"OXTaskbarIcon.cpp"
	"OXImageListBox.cpp"
	

Resource files:
	"OXHistoryCombo.rc"		


*/
#if !defined(_OXCUSTOMIZETRAYICONPAGE_H__)
#define _OXCUSTOMIZETRAYICONPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#include "OXCustomizePage.h"
#include "OXSeparator.h"
#include "OXHistoryCombo.h"
#include "OXTaskbarIcon.h"
#include "OXImageListBox.h"
#include "OXMainRes.h"

class COXCustomizeTrayIconPage;

class OX_CLASS_DECL COXCustomizeTaskbarIcon : public COXTaskbarIcon
{
public:
	COXCustomizeTaskbarIcon() : m_pTrayIconPage(NULL) {};

public:
	COXCustomizeTrayIconPage* m_pTrayIconPage;
	
	//{{AFX_VIRTUAL(COXCustomizeTaskbarIcon)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(COXCustomizeTaskbarIcon)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COXCustomizeTrayIconPage dialog

class OX_CLASS_DECL COXCustomizeTrayIconPage : public COXCustomizePage
{
	DECLARE_DYNCREATE(COXCustomizeTrayIconPage)
// Construction
public:
	COXCustomizeTrayIconPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(COXCustomizeTrayIconPage)
	enum { IDD = IDD_OX_PAGE_TRAYICON };
	CStatic	m_staticDefaultIcon;
	COXSeparator	m_sepSelectIcon;
	COXSeparator	m_sepDisplayMenuEvent;
	COXImageListBox	m_listIcons;
	COXHistoryCombo	m_comboIconSource;
	BOOL	m_bShowTrayIcon;
	CString	m_sIconTooltip;
	int		m_nDisplayMenuEvent;
	int		m_nIconSourceType;
	CString m_sIconSourceFileName;
	//}}AFX_DATA

protected:
	COXCustomizeTaskbarIcon m_trayIcon;
	CMenu m_contextMenu;
	HICON m_hOrigIcon;
	HICON m_hUserIcon;
	int m_nIconIndex;
	BOOL m_bUseRightClick;
	BOOL m_bUseDefaultIcon;


public:
	virtual BOOL Load(const COXCustomizeManager* pCustomizeManager);
	virtual void Unload();
	virtual BOOL IsSupportingApplyChanges() { return TRUE; }
	virtual BOOL ApplyChanges();

	virtual CString GetTitle() const { 
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZETRAYTITLE));
		return sRet;
		//_T("Tray Icon"); 
	}
	virtual LPCTSTR GetImageResource() const { 
		return MAKEINTRESOURCE(IDB_OX_CUSTOMIZE_TRAYICON); 
	}
	virtual CString GetTooltip() const {
		CString sRet;
		VERIFY(sRet.LoadString(IDS_OX_CSTMZETRAYTOOLTIP));
		return sRet;
		//_T("Customize TrayIcon"); 
	}

	virtual BOOL OnCloseManager(BOOL bIsOk);

	virtual BOOL InitializeTrayIcon(HICON hIcon, UINT nContextMenuResourceID,
		LPCTSTR lpszIconTooltip=NULL, BOOL bShowTrayIcon=TRUE, 
		BOOL bUseRightClick=TRUE, LPCTSTR lpszIconSourceFileName=NULL,
		LPCTSTR lpszProfileName=_T("CustomizeTrayIcon"));

	inline COXTaskbarIcon* GetTrayIcon() { return &m_trayIcon; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCustomizeTrayIconPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COXCustomizeTrayIconPage();   // destructor

	virtual void DoPopupMenu();

protected:
	virtual void OnInitDialog();

	virtual void SetupTrayIcon();

	// Generated message map functions
	//{{AFX_MSG(COXCustomizeTrayIconPage)
	afx_msg void OnRadioIconSourceDefault();
	afx_msg void OnRadioIconSourceFile();
	afx_msg void OnSelchangeComboIconsourceFilename();
	afx_msg void OnEditchangeComboIconsourceFilename();
	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCUSTOMIZETRAYICONPAGE_H__)
