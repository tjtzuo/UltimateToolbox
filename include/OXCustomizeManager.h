// ==========================================================================
//			Class Specification: COXCustomizeManager
// ==========================================================================

// Header file : OXCustomizeManager.h

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

COXCustomizeManager is CDialog derived class that serves as a control center that
provides customization functionality for different aspects of your application.
COXCustomizeManager is a container of customize page(s). Each customize page is
responsible for customization of a particular area of your application functionality
(e.g. customizing menus and toolbars). Refer to COXCustomizePage class overview and 
overview for standard customize pages classes (like COXCustomizeCommandsPage).

COXCustomizeManager is responsible for initializing of all customize pages inserted
in the manager. It organizes all pages in groups (by default there is only one group 
is defined - the one that contains standard (predefined) customize pages) and provides 
navigation facilities by using shortcut bar control (COXShortcutBar class). 

COXCustomizeManager is designed in the way it can be easily overridden in order 
to provide support for custom-defined customize pages. It has a lot of virtual 
functions that define its behavior. Those functions can be overridden in order to 
provide an additional customization functionality. 

Despite the fact that customize manager is easily expandable in most cases a programmer
will use a predefined set of standard customize pages (Customize Commands, 
Customize Toolbars, Customize Workspace, etc.). COXCustomizeManager provides
interface functions for accessing the standard pages and their initialization 
functions (refer to the class reference for details). In most cases a customization 
feature will require a programmer to call only one initialization function.

COXCustomizeManager provides rich set of functions for navigating through customize
pages programmatically. These will be useful mostly for programmers who decide
to derive their own classes and provide additional functionality. 

We developed a lot of predefined customize pages that should satisfy the majority
of the common needs for customization (toolbars, menus, accelerator tables, etc.). 
All standard pages are documented and the entire list of them can be found in the
overview for COXCustomizePage (the base class for any customize page).

Note, that you don't have to implement your own COXCustomizeManager-derived class
in order to specify what standard pages will be included in the customize manager.
Moreover, you don't have to include the code that implements the standard pages 
if you are not interested in at all by using corresponding defines. For example in 
order to include support for customizing toolbars and menus you need to include the
following define in your stdafx.h file:

#define OX_CUSTOMIZE_TOOLBARS

The list of all defines that correspond to standard pages can be found in the
COXCustomizePage overview.

Below you will find the details on how you can use customize manager in your 
application. (Note that some of the standard pages might require additional 
initialization steps).



  Usage

1)	In your stdafx.h file include defines for the standard pages you are going 
	to use in the customize manager:

		#define OX_CUSTOMIZE_TOOLBARS
		#define OX_CUSTOMIZE_COMMANDS

	These defines will include "Customize Toolbars" and "Customize Commands"
	standard pages.

2)	Declare a member variable of COXCustomizeManager class in the main window
	of your application (usually CMainFrame class for SDI/MDI applications)

		// customize manager
		COXCustomizeManager	m_customizeManager;

3)  Handle WM_CREATE message for the main window and in the handler call 
	initialization routines for the pages included in the customize manager:

		#ifdef OX_CUSTOMIZE_TOOLBARS
			VERIFY(m_customizeManager.InitializeToolbars());
		#endif	//	OX_CUSTOMIZE_TOOLBARS
		#ifdef OX_CUSTOMIZE_COMMANDS
			VERIFY(m_customizeManager.InitializeCommands());
		#endif	//	OX_CUSTOMIZE_COMMANDS
	
	This code will initialize "Customize Toolbars" and "Customize Commands"
	standard pages. Note that you have to refer to the documentation for 
	appropriate way of initializing of any particular standard customize
	routines because some of them require additional initialization step(s).

4)	If you want to save the state of the pages (pages allows you to save the 
	changed settings in the registry) you have to handle WM_CLOSE message
	for the main window and add the following line of code to this handler:

		m_customizeManager.SaveWorkspaceState();


5)	In order to display the customize manager (it is always displayed as modeless 
	window) you can use the following code:

		if(!::IsWindow(m_customizeManager.GetSafeHwnd()))
		{
			VERIFY(m_customizeManager.Create(COXCustomizeManager::IDD,this));
		}
		m_customizeManager.SetActiveWindow();
		m_customizeManager.ShowWindow(SW_SHOW);



That's all you need to do. 

Refer to the COXCustomizePage documentation for more details on customize pages.


Note!!!	COXCustomizeManager automatically recognizes if it is being used in
		with docking windows framework. It behaves differently in docking
		windows environment. If your application doesn't use docking framework
		then you must to add the following define to your stdafx.h file:

		#define OX_CUSTOMIZE_NOTSUPPORTING_DOCKING_FRAMEWORK




The best way of learning about Customize manager capabilities is to take look at the 
VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. In the sample menu choose "View"-"Customize" in order to display the 
Customize manager window. 




Dependency:
	#include "OXCustomizeManager.h"


Source code files:
	"OXCustomizeManager.cpp"		
	"OXCustomizePage.cpp"		
	"OXLayoutManager.cpp"		
	"OXShortcutBar.cpp"		
	"OXSeparator.cpp"		
	"OXStatic.cpp"	
	

Resource files:
	"OXCustomizeManager.rc"		


plus dependencies defined for the pages that are included in the customize manager.
Refer to the corresponding page overviews for details on their dependencies
	
*/

#if !defined(_OXCUSTOMIZEMANAGER_H__)
#define _OXCUSTOMIZEMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXLayoutManager.h"
#include "OXShortcutBar.h"
#include "OXStatic.h"
#include "OXSeparator.h"

#include "OXCustomizePage.h"
#include "OXMainRes.h"


#ifndef IDR_MAINFRAME
#define IDR_MAINFRAME			128
#endif	//	IDR_MAINFRAME


#if defined _LINK_TO_UTB_IN_EXTDLL || defined _BUILD_UTB_INTO_EXTDLL 
	#ifndef OX_CUSTOMIZE_CAPTION
	#define OX_CUSTOMIZE_CAPTION
	#endif
	#ifndef OX_CUSTOMIZE_TABBEDMDI
	#define OX_CUSTOMIZE_TABBEDMDI
	#endif
	#ifndef OX_CUSTOMIZE_WORKSPACE_STATE
	#define OX_CUSTOMIZE_WORKSPACE_STATE
	#endif
	#ifndef OX_CUSTOMIZE_TOOLBARS
	#define OX_CUSTOMIZE_TOOLBARS
	#endif
	#ifndef OX_CUSTOMIZE_COMMANDS
	#define OX_CUSTOMIZE_COMMANDS
	#endif
	#ifndef OX_CUSTOMIZE_BACKGROUND
	#define OX_CUSTOMIZE_BACKGROUND
	#endif
	#ifndef OX_CUSTOMIZE_COOLCONTROLS
	#define OX_CUSTOMIZE_COOLCONTROLS
	#endif
	#ifndef OX_CUSTOMIZE_TRAYICON
	#define OX_CUSTOMIZE_TRAYICON
	#endif
	#ifndef OX_CUSTOMIZE_INSTANCE_MANAGER
	#define OX_CUSTOMIZE_INSTANCE_MANAGER
	#endif
	#ifndef OX_CUSTOMIZE_SPLASHWINDOW
	#define OX_CUSTOMIZE_SPLASHWINDOW
	#endif
	#ifndef OX_CUSTOMIZE_INTELLIMOUSE
	#define OX_CUSTOMIZE_INTELLIMOUSE
	#endif
	#ifndef OX_CUSTOMIZE_SHORTKEYS
	#define OX_CUSTOMIZE_SHORTKEYS
	#endif
#endif



#ifndef OX_CUSTOMIZE_NOTSUPPORTING_DOCKING_FRAMEWORK
#include "OXFrameWndDock.h"
#endif	//	OX_CUSTOMIZE_NOTSUPPORTING_DOCKING_FRAMEWORK

#ifdef OX_CUSTOMIZE_CAPTION
#include "OXCustomizeCaptionPage.h"
#endif	//	OX_CUSTOMIZE_CAPTION

#ifdef OX_CUSTOMIZE_TABBEDMDI
#include "OXCustomizeTabbedMDIPage.h"
#endif	//	OX_CUSTOMIZE_TABBEDMDI

#ifdef OX_CUSTOMIZE_WORKSPACE_STATE
#include "OXCustomizeWorkspaceStatePage.h"
#endif	//	OX_CUSTOMIZE_WORKSPACE_STATE

#ifdef OX_CUSTOMIZE_TOOLBARS
#include "OXCustomizeToolbarsPage.h"
#endif	//	OX_CUSTOMIZE_TOOLBARS

#ifdef OX_CUSTOMIZE_COMMANDS
#include "OXCustomizeCommandsPage.h"
#endif	//	OX_CUSTOMIZE_COMMANDS

#ifdef OX_CUSTOMIZE_BACKGROUND
#include "OXCustomizeBackgroundPage.h"
#endif	//	OX_CUSTOMIZE_BACKGROUND

#ifdef OX_CUSTOMIZE_COOLCONTROLS
#include "OXCustomizeCoolControlsPage.h"
#endif	//	OX_CUSTOMIZE_COOLCONTROLS

#ifdef OX_CUSTOMIZE_TRAYICON
#include "OXCustomizeTrayIconPage.h"
#endif	//	OX_CUSTOMIZE_TRAYICON

#ifdef OX_CUSTOMIZE_INSTANCE_MANAGER
#include "OXCustomizeInstanceManagerPage.h"
#endif	//	OX_CUSTOMIZE_INSTANCE_MANAGER

#ifdef OX_CUSTOMIZE_SPLASHWINDOW
#include "OXCustomizeSplashWindowPage.h"
#endif	//	OX_CUSTOMIZE_SPLASHWINDOW

#ifdef OX_CUSTOMIZE_INTELLIMOUSE
#include "OXCustomizeIntelliMousePage.h"
#endif	//	OX_CUSTOMIZE_INTELLIMOUSE

#ifdef OX_CUSTOMIZE_SHORTKEYS
#include "OXCustomizeShortkeysPage.h"
#endif	//	OX_CUSTOMIZE_SHORTKEYS


#ifndef ID_OXCUSTOMIZE_TBIMAGE_WIDTH
#define ID_OXCUSTOMIZE_TBIMAGE_WIDTH			16
#endif	//	ID_OXCUSTOMIZE_TBIMAGE_WIDTH
#ifndef ID_OXCUSTOMIZE_TBIMAGE_HEIGHT
#define ID_OXCUSTOMIZE_TBIMAGE_HEIGHT			16
#endif	//	ID_OXCUSTOMIZE_TBIMAGE_HEIGHT

// enumerator of ids of the standard pages
enum CustomizeManagerPage { CUSTOMPAGE=-1, CAPTION_PAINTER=0, TABBEDMDI=1, 
							WORKSPACE_STATE=2, TOOLBARS=3, COMMANDS=4, 
							BACKGROUND_PAINTER=5, COOLCONTROLS=6, TRAYICON=7, 
							INSTANCE_MANAGER=8, SPLASHWINDOW=9, INTELLIMOUSE=10, 
							SHORTKEYS=11 };

typedef CMap<COXCustomizePage*,COXCustomizePage*,DWORD,DWORD> CMapPages;
typedef CMap<COXCustomizePage*,COXCustomizePage*,CString,CString> CMapPageTooltips;


class OX_CLASS_DECL COXCustomizeManager : public CDialog
{
public:
	// --- In  :	pParent	-	points to parent window. If NULL is specified the
	//							main window of the application is used as parent 
	//							window
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXCustomizeManager(CWnd* pParent = NULL);

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXCustomizeManager();

// Dialog Data
	//{{AFX_DATA(COXCustomizeManager)
	enum { IDD = IDD_OX_CUSTOMIZEMANAGER_DIALOG };
	COXSeparator m_sepTitle;
	COXSeparator m_sepBottom;
	COXShortcutBar m_shb;
	COXStatic m_title;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCustomizeManager)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


protected:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Called at the initialization stage once in order to populate 
	//				the manager with customize pages. Advanced overridable, use
	//				your own implementation in the case you need to add 
	//				non-standard pages
	virtual void PopulatePages();


	// --- In  :	page	-	ID of the standandard page. Can be one of the 
	//							following:
	//
	//							CAPTION_PAINTER		-	page for customizing
	//													caption painter settings
	//							TABBEDMDI			-	page for customizing
	//													Tabbed MDI interface settings
	//							WORKSPACE_STATE		-	page for customizing
	//													workspace state settings
	//							TOOLBARS			-	page for customizing
	//													toolbars appearance
	//							COMMANDS			-	page for customizing
	//													toolbars and menus contents
	//							BACKGROUND_PAINTER	-	page for customizing
	//													background painter settings
	//							COOLCONTROLS		-	page for customizing
	//													cool controls settings
	//							TRAYICON			-	page for customizing
	//													tray icon settings
	//							INSTANCE_MANAGER	-	page for customizing
	//													the instance manager settings
	//							SPLASHWINDOW		-	page for customizing
	//													splash window appearance
	//							INTELLIMOUSE		-	page for customizing
	//													IntelliMouse support
	//							SHORTKEYS			-	page for customizing
	//													shortkeys table
	// --- Out : 
	// --- Returns:	TRUE if specified page will be included in the manager; 
	//				FALSE otherwise
	// --- Effect : Called internally in order to add standard page to the manager. 
	//				Advanced overridable, default implementation always returns TRUE.
	//				Use your own implementation in the case you need to prevent the 
	//				inclusion of any standard customize page
	virtual BOOL IsIncludingStandardPage(CustomizeManagerPage page) 
	{ 
		UNREFERENCED_PARAMETER(page);
		return TRUE; 
	}
	

	// --- In  :	bIsOk	-	TRUE if the customize manager window is being closed 
	//							as a result of pressing "OK" button; FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE if manager dialog window was successfully closed; 
	//				FALSE otherwise
	// --- Effect : Called to close active customize manager. Advanced overridable, 
	//				use	your own implementation in the case you need to handle the 
	//				close event
	virtual BOOL CloseManager(BOOL bIsOk);	


public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	Pointer to the parent window for customize pages
	// --- Effect : Retrieves the parent window for customize pages. Advanced 
	//				overridable, use your own implementation in the case you need to 
	//				specify a different parent window for customize pages
	virtual CWnd* GetSite() { return this; }


	// --- In  :	lpszGroup	-	the name of a group in the customize manager.
	//								By default it is set to DEFAULT_CUSTOMIZE_GROUP,
	//								the group that contains standard pages
	//				hGroup		-	handle to a group in the customize manager.
	// --- Out : 
	// --- Returns:	The number of customize pages in the specified group
	// --- Effect : Retrieves the number of customize pages in the specified group. 
	//				All standard pages are created in one group which name is
	//				defined as DEFAULT_CUSTOMIZE_GROUP constant
	inline int GetPageCount(LPCTSTR lpszGroup=DEFAULT_CUSTOMIZE_GROUP) const 
	{
		HSHBGROUP hGroup=m_shb.FindGroupByTitle(lpszGroup);
		return GetPageCount(hGroup);
	}
	inline int GetPageCount(HSHBGROUP hGroup) const 
	{
		if(hGroup==NULL)
			return 0;
		return m_shb.GetLCItemCount(hGroup);
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	The number of customize pages in all groups
	// --- Effect : Retrieves the number of customize pages in all groups
	int GetAllPageCount() const;


	// --- In  :	pCustomizePage	-	pointer to a created page to be added
	//									to customize manager
	// --- Out : 
	// --- Returns:	TRUE if the specified page was successfully added to 
	//				customize manager; FALSE otherwise
	// --- Effect : Adds new page to customize manager. The name of the group 
	//				the specified page belongs to is retrieve by calling corresponding
	//				function defined in COXCustomizePage class (the base class for
	//				all customize pages). When corresponding group is found the page 
	//				is being added to the end of existing pages in the found group
	inline BOOL AddPage(COXCustomizePage* pCustomizePage) 
	{
		return InsertPage(pCustomizePage,GetPageCount(pCustomizePage->GetGroup()));
	}

	// --- In  :	pCustomizePage	-	pointer to a created page to be inserted
	//									in customize manager
	//				nPageIndex		-	position at which the specified page will be
	//									inserted in the corresponding group
	// --- Out : 
	// --- Returns:	TRUE if the specified page was successfully inserted in 
	//				customize manager; FALSE otherwise
	// --- Effect : Inserts new page in customize manager. The name of the group 
	//				the specified page belongs to is retrieve by calling corresponding
	//				function defined in COXCustomizePage class (the base class for
	//				all customize pages). When corresponding group is found the page 
	//				is being inserted at the position specified by nPageIndex 
	//				parameter. Advanced overridable, use your own implementation in 
	//				the case you need to handle the process of inserting new 
	//				customize page
	virtual BOOL InsertPage(COXCustomizePage* pCustomizePage, int nPageIndex);


	// --- In  :	pCustomizePage	-	pointer to a page to be deleted from
	//									customize manager
	// --- Out : 
	// --- Returns:	TRUE if the specified page was successfully deleted from 
	//				customize manager; FALSE otherwise
	// --- Effect : Deletes a page from customize manager. Advanced overridable,
	//				use your own implementation if you need to handle the process 
	//				of deleting a customize page
	virtual BOOL DeletePage(COXCustomizePage* pCustomizePage);


	// --- In  :	pageType		-	ID of standard page, refer to 
	//									IsIncludingStandardPage() function 
	//									documentation for the values this parameter 
	//									can take
	//				pCustomizePage	-	a pointer to a customize page to find 
	//				lpszTitle		-	title of a customize page to find
	//				lpszGroup		-	title of a group where the specified page
	//									belongs to
	// --- Out :	hGroupTest		-	group in the shortcut bar where the specified 
	//									page icon is displayed
	//				nIndexTest		-	index of the position in the hGroupTest group 
	//									in the shortcut bar at which the specified page
	//									icon is displayed
	// --- Returns:	TRUE if the specified page was successfully found; 
	//				FALSE otherwise
	// --- Effect : Finds the position of the specified customize page icon in the
	//				customize manager shortcut bar. Whenever a page is added to 
	//				customize manager the new item that identifies this page will be
	//				created in the shortcut bar. Information on the name of shortcut 
	//				bar group the page belongs to, the item icon and the item title 
	//				will be retrieved by calling corresponding functions defined in 
	//				the COXCustomizePage class (base class for all customize pages)
	BOOL FindPage(CustomizeManagerPage pageType, 
		HSHBGROUP& hGroupTest, int& nIndexTest) const;
	BOOL FindPage(COXCustomizePage* pCustomizePage, 
		HSHBGROUP& hGroupTest, int& nIndexTest) const;
	BOOL FindPage(LPCTSTR lpszTitle, LPCTSTR lpszGroup, 
		HSHBGROUP& hGroupTest, int& nIndexTest) const;


	// --- In  :	hGroup		-	group in the shortcut bar where  a page icon is 
	//								displayed
	//				nIndex		-	index of the position in the hGroupTest group 
	//								in the shortcut bar at which a page icon is 
	//								displayed
	// --- Out :	
	// --- Returns:	A pointer to the customize page object which icon is located
	//				at the specified position in the customize manager shortcut bar
	//				if found; NULL otherwise
	// --- Effect : Finds a customize page by position of its icon in the customize 
	//				manager shortcut bar. Whenever a page is added to 
	//				customize manager the new item that identifies this page will be
	//				created in the shortcut bar. Knowing shortcut bar group and item
	//				index any customize page added to customize manager can be
	//				uniquely identified
	COXCustomizePage* GetPageByLocation(HSHBGROUP hGroup, int nIndex) const;


	// --- In  :	hGroup		-	group in the shortcut bar where  a page icon is 
	//								displayed
	//				nIndex		-	index of the position in the hGroupTest group 
	//								in the shortcut bar at which a page icon is 
	//								displayed
	// --- Out :	
	// --- Returns:	A pointer to the currently active customize page object
	// --- Effect : Retrieves currently active customize page
	inline COXCustomizePage* GetActivePage() const { return m_pActivePage; }


	// --- In  :	pageType		-	ID of standard page, refer to 
	//									IsIncludingStandardPage() function 
	//									documentation for the values this parameter 
	//									can take
	//				pCustomizePage	-	a pointer to a customize page to activate 
	//				lpszTitle		-	title of a customize page to activate
	//				lpszGroup		-	title of a group where the specified page
	//									belongs to
	//				hGroup			-	group in the shortcut bar where  a page icon is 
	//									displayed
	//				nIndex			-	index of the position in the hGroupTest group 
	//									in the shortcut bar at which a page icon is 
	//									displayed
	// --- Out :	
	// --- Returns:	TRUE if the specified page was successfully activated; 
	//				FALSE otherwise
	// --- Effect : Activates the specified customize page. The corresponding item
	//				in the customize manager shortcut bar will be activated and the
	//				specified page will be loaded while previously active page
	//				will be unloaded. Advanced overridable, use your own implementation 
	//				in the case you need to handle customize page activation event
	BOOL ActivatePage(CustomizeManagerPage pageType);
	BOOL ActivatePage(COXCustomizePage* pCustomizePage);
	BOOL ActivatePage(LPCTSTR lpszTitle, LPCTSTR lpszGroup);
	virtual BOOL ActivatePage(HSHBGROUP hGroup, int nIndex);


	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect : Recalculates the rectangle within customize manager window
	//				that can be taken by a customize page. Call this function 
	//				whenever the positioning of the controls in customize manager
	//				is changed. Automatically called from WM_SIZE handler
	void RecalcPageRect();

	// --- In  :	
	// --- Out : 
	// --- Returns:	The rectangle within customize manager window that can be taken 
	//				by a customize page
	// --- Effect : Retrieves the rectangle within customize manager window
	//				that can be taken by a customize page.
	inline CRect GetPageRect() const {return m_rectPage; }

	
	// --- In  :	
	// --- Out : 
	// --- Returns:	A pointer to internal shortcut bar control that is used to 
	//				display all customize page icons
	// --- Effect : Retrieves a pointer to internal shortcut bar control. This 
	//				shortcut bar is populated with items that corresponds to all 
	//				inserted customize pages.
	inline COXShortcutBar* GetShortcutBar() { return &m_shb; }


	// --- In  :	
	// --- Out : 
	// --- Returns:	A pointer to internal array of all inserted customize pages
	// --- Effect : Retrieves a pointer to internal array of all inserted customize 
	//				pages. You might need to use this array in order to iterate 
	//				through all inserted pages and apply the same routine.
	inline CMapPages* GetMapPages() { return &m_mapPages; }

	// --- In  :	
	// --- Out : 
	// --- Returns:	A pointer to internal array of all customize page tooltips
	// --- Effect : Retrieves a pointer to internal array of all customize page
	//				tooltips. 
	inline CMapPageTooltips* GetMapPageTooltips() { return &m_mapTooltips; }


	// --- In  :	pCustomizePage	-	a pointer to a customize page 
	// --- Out : 
	// --- Returns: A tooltip for the specified page
	// --- Effect : Retrieves a tooltip for the specified page. 
	inline CString GetPageTooltip(COXCustomizePage* pCustomizePage) const 
	{
		CString sTooltip(_T(""));
		m_mapTooltips.Lookup(pCustomizePage,sTooltip);
		return sTooltip;
	}

	// --- In  :	pCustomizePage	-	a pointer to a customize page 
	//				lpszTooltip		-	new tooltip for the specified page
	// --- Out : 
	// --- Returns: TRUE if new tooltip for the specified page was set successfully;
	//				FALSE otherwise
	// --- Effect : Sets new tooltip for the specified page. 
	BOOL SetPageTooltip(COXCustomizePage* pCustomizePage, LPCTSTR lpszTooltip);


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if this customize manager belongs to the framework that 
	//				supports Ultimate Toolbox Docking Windows Framework (mainframe
	//				window is derived from COXFrameWndSizeDock class); FALSE otherwise
	// --- Effect : Retrieves the flag that specifies if customize manager belongs 
	//				to the framework that supports Ultimate Toolbox Docking Windows 
	//				Framework
	inline BOOL IsDockingFramework() const 
	{
#ifndef OX_CUSTOMIZE_NOTSUPPORTING_DOCKING_FRAMEWORK
		CWnd* pWnd=AfxGetMainWnd();
		ASSERT(pWnd!=NULL);
		return (pWnd->IsKindOf(RUNTIME_CLASS(COXFrameWndSizeDock)) ||
			pWnd->IsKindOf(RUNTIME_CLASS(COXMDIFrameWndSizeDock)));
#else
		return FALSE;
#endif	//	OX_CUSTOMIZE_NOTSUPPORTING_DOCKING_FRAMEWORK
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if this customize manager belongs to the MDI framework; 
	//				FALSE otherwise
	// --- Effect : Retrieves the flag that specifies if customize manager belongs 
	//				to MDI framework or not
	inline BOOL IsMDIFramework() const 
	{
		CWnd* pWnd=AfxGetMainWnd();
		ASSERT(pWnd!=NULL);
		return pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd));
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if the state of a framework this customize manager 
	//				belongs was saved in the registry successfully; FALSE otherwise
	// --- Effect : Saves in the registry the state of the framework this customize 
	//				manager belongs to. Call this function before the application is 
	//				closed. Advanced overridable, use your own implementation if you 
	//				need to save any additional information.
	virtual BOOL SaveWorkspaceState();


protected:
	// image list for icons associated with pages (32x32 size)
	CImageList m_ilShortcutBar;

	// rectangle for the page dialog
	CRect m_rectPage;
	// pointer to currently active page
	COXCustomizePage* m_pActivePage;

	// layout manager object that can be used in order to resize child controls
	// whenever size of the dialog is changed
	COXLayoutManager m_LayoutManager;

	// map of all created customize pages (in terms of C++ objects)
	CMapPages m_mapObjects;
	// map of all created customize pages (in terms of window objects)
	CMapPages m_mapPages;
	// map that associates customize pages with their tooltips
	CMapPageTooltips m_mapTooltips;

	// flag that specifies that the dialog has been completely initialized
	BOOL m_bInitialized;


	// pedefined pages
	//
#ifdef OX_CUSTOMIZE_CAPTION
	// page for caption painter settings
	COXCustomizeCaptionPage m_pageCaption;
#endif	//	OX_CUSTOMIZE_CAPTION

#ifdef OX_CUSTOMIZE_TABBEDMDI
	// page for tabbed MDI settings
	COXCustomizeTabbedMDIPage m_pageTabbedMDI;
#endif	//	OX_CUSTOMIZE_TABBEDMDI

#ifdef OX_CUSTOMIZE_WORKSPACE_STATE
	// page for caption painter settings
	COXCustomizeWorkspaceStatePage m_pageWorkspaceState;
#endif	//	OX_CUSTOMIZE_WORKSPACE_STATE

#ifdef OX_CUSTOMIZE_TOOLBARS
	// page for caption painter settings
	COXCustomizeToolbarsPage m_pageToolbars;
#endif	//	OX_CUSTOMIZE_TOOLBARS

#ifdef OX_CUSTOMIZE_COMMANDS
	// page for caption painter settings
	COXCustomizeCommandsPage m_pageCommands;
#endif	//	OX_CUSTOMIZE_COMMANDS

#ifdef OX_CUSTOMIZE_BACKGROUND
	// page for background painter settings
	COXCustomizeBackgroundPage m_pageBackground;
#endif	//	OX_CUSTOMIZE_BACKGROUND

#ifdef OX_CUSTOMIZE_COOLCONTROLS
	// page for cool controls settings
	COXCustomizeCoolControlsPage m_pageCoolControls;
#endif	//	OX_CUSTOMIZE_COOLCONTROLS

#ifdef OX_CUSTOMIZE_TRAYICON
	// page for tray icon settings
	COXCustomizeTrayIconPage m_pageTrayIcon;
#endif	//	OX_CUSTOMIZE_TRAYICON

#ifdef OX_CUSTOMIZE_INSTANCE_MANAGER
	// page for tray icon settings
	COXCustomizeInstanceManagerPage m_pageInstanceManager;
#endif	//	OX_CUSTOMIZE_INSTANCE_MANAGER

#ifdef OX_CUSTOMIZE_SPLASHWINDOW
	// page for splash window settings
	COXCustomizeSplashWindowPage m_pageSplashWindow;
#endif	//	OX_CUSTOMIZE_SPLASHWINDOW

#ifdef OX_CUSTOMIZE_INTELLIMOUSE
	// page for intelli mouse suport settings
	COXCustomizeIntelliMousePage m_pageIntelliMouse;
#endif	//	OX_CUSTOMIZE_INTELLIMOUSE

#ifdef OX_CUSTOMIZE_SHORTKEYS
	// page for shortkeys settings
	COXCustomizeShortkeysPage m_pageShortkeys;
#endif	//	OX_CUSTOMIZE_SHORTKEYS


public:

#ifdef OX_CUSTOMIZE_CAPTION
	virtual BOOL InitializeCaptionPainter(BOOL bPaintCaption=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeCaptionPainter"), 
		BOOL bOnlyMainWindow=FALSE) 
	{ 
		ASSERT(GetCaptionPage()!=NULL);
		return GetCaptionPage()->
			InitializeCaptionPainter(bPaintCaption,lpszProfileName,bOnlyMainWindow); 
	}

	virtual COXCaptionPainterOrganizer* GetCaptionPainterOrganizer() 
	{
		ASSERT(GetCaptionPage()!=NULL);
		return GetCaptionPage()->GetPainterOrganizer();
	}

	virtual COXCustomizeCaptionPage* GetCaptionPage() { return &m_pageCaption; }
#endif	//	OX_CUSTOMIZE_CAPTION


#ifdef OX_CUSTOMIZE_TABBEDMDI
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
		BOOL bSupportTabbedMDI=TRUE) 
	{ 
		ASSERT(GetTabbedMDIPage()!=NULL);
		return GetTabbedMDIPage()->InitializeTabbedMDI(dwTabCtrlStyle,
			dwOffset,lpszProfileName,bSupportTabbedMDI); 
	}

	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to internal COXTabClientWnd object
	// --- Effect : Retrieves internal COXTabClientWnd object that implements
	//				TabbedMDI functionality
	virtual COXTabClientWnd* GetTabClientWnd() 
	{
		ASSERT(GetTabbedMDIPage()!=NULL);
		return GetTabbedMDIPage()->GetTabClientWnd();
	}

	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Customize TabbedMDI page
	// --- Effect : Retrieves a pointer to Customize TabbedMDI page. This page 
	//				can be used to customize the appearance of TabbedMDI interface.
	//				TabbedMDI is MDI extension that displays standard tab control
	//				in which we create an item for every MDIChild window. The window 
	//				icon and text will be associated with the corresponding tab item. 
	//				Using tab control you can switch very fast between MDIChild 
	//				windows just clicking on the tab item. Refer to TabbedMDI 
	//				documentation for more details
	virtual COXCustomizeTabbedMDIPage* GetTabbedMDIPage() 
	{ 
		return &m_pageTabbedMDI; 
	}
#endif	//	OX_CUSTOMIZE_TABBEDMDI


#ifdef OX_CUSTOMIZE_WORKSPACE_STATE
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
	// --- Returns: A pointer to COXWorkspaceState object that is responsible for 
	//				saving/loading workspace state
	// --- Effect : Retrieves a pointer to internal COXWorkspaceState object
	virtual COXWorkspaceState* GetWorkspaceStateObject() 
	{
		ASSERT(GetWorkspaceStatePage()!=NULL);
		return GetWorkspaceStatePage()->GetWorkspaceStateObject();
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Customize Workspace State page
	// --- Effect : Retrieves a pointer to Customize Workspace State page. This page 
	//				can be used to specify what aspect of application's workspace
	//				state must be saved/loaded. Also it allows you to specify what 
	//				customize pages must be displayed in the customize manager 
	//				shortcut bar and what ones must be hidden
	virtual COXCustomizeWorkspaceStatePage* GetWorkspaceStatePage() 
	{ 
		return &m_pageWorkspaceState; 
	}
#endif	//	OX_CUSTOMIZE_WORKSPACE_STATE


#ifdef OX_CUSTOMIZE_TOOLBARS
	// --- In  :	bCoolLook		-	if TRUE the toolbars will be displayed in
	//									cool mode (flat buttons). By default set 
	//									to TRUE
	//				bShowGripper	-	if TRUE a gripper will be displayed on the
	//									top/left side of application's toolbars. 
	//									By default set to TRUE
	//				bShowTooltips	-	if TRUE a tooltip will be displayed for
	//									toolbars buttons. By default set to TRUE
	//				nBorderSize		-	the border size in pixels for toolbars. 
	//									By default set to 0
	//				lpszProfileName	-	the name of the hive in the registry where 
	//									toolbars' appearance settings is saved. If
	//									non-NULL value is set then settings are
	//									retrieved from the registry right away. 
	//									These can overwrite the values specified in 
	//									the previous parameters
	// --- Out : 
	// --- Returns: TRUE if toolbars were initialized successfully; FALSE otherwise
	// --- Effect : Initializes the settings that specifies the appearance of the 
	//				application's toolbars
	virtual BOOL InitializeToolbars(BOOL bCoolLook=TRUE, BOOL bShowGripper=TRUE,
		BOOL bShowTooltips=TRUE, int nBorderSize=0, 
		LPCTSTR lpszProfileName=_T("CustomizeToolbars")) 
	{ 
		ASSERT(GetToolbarsPage()!=NULL);
		return GetToolbarsPage()->InitializeToolbars(bCoolLook,bShowGripper,
			bShowTooltips,nBorderSize,lpszProfileName); 
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Customize Toolbars page
	// --- Effect : Retrieves a pointer to Customize Toolbars page. This page can 
	//				be used to specify the appearance of the application's toolbars;
	//				create new toolbars; delete existing toolbars; rename existing 
	//				toolbars; reset the contents of the toolbars to their original 
	//				set of commands.
	virtual COXCustomizeToolbarsPage* GetToolbarsPage() { return &m_pageToolbars; }
#endif	//	OX_CUSTOMIZE_TOOLBARS


#ifdef OX_CUSTOMIZE_COMMANDS
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
		return InitializeCommands(MAKEINTRESOURCE(nCustomImagesResourceID),
			clrMask,bShowIconsInMenu,bShowOnlyRecentlyUsedMenuItems,lpszProfileName);
	}
	virtual BOOL InitializeCommands(LPCTSTR lpszCustomImagesResource,
		COLORREF clrMask=RGB(192,192,192), BOOL bShowIconsInMenu=TRUE,
		BOOL bShowOnlyRecentlyUsedMenuItems=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeCommands")) 
	{ 
		ASSERT(GetCommandsPage()!=NULL);
		return GetCommandsPage()->InitializeCommands(lpszCustomImagesResource,
			clrMask,bShowIconsInMenu,bShowOnlyRecentlyUsedMenuItems,lpszProfileName); 
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Customize Commands page
	// --- Effect : Retrieves a pointer to Customize Commands page. This page can 
	//				be used to customize the contents of application's toolbars and
	//				menus. New items can be added, existing deleted or reallocated.
	//				Also the appearance of items can be customized.
	virtual COXCustomizeCommandsPage* GetCommandsPage() { return &m_pageCommands; }

	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to internal COXBitmapMenuOrganizer object
	// --- Effect : Retrieves a pointer to internal COXBitmapMenuOrganizer object. 
	//				This object is used to handle menus in the framework.
	virtual COXBitmapMenuOrganizer* GetBMOrganizer()  
	{
		ASSERT(GetCommandsPage()!=NULL);
		return GetCommandsPage()->GetBMOrganizer();
	}
#endif	//	OX_CUSTOMIZE_COMMANDS


#ifdef OX_CUSTOMIZE_BACKGROUND
	virtual BOOL InitializeBackgroundPainter(const BOOL bPaintBackground,
		const BOOL bOnlyClientArea,	LPCTSTR lpszDIBResourceName, 
		const PaintType paintType=Tile, const COLORREF clrBk=CLR_DEFAULT, 
		LPCTSTR lpszProfileName=_T("CustomizeBackgroundPainter")) 
	{ 
		ASSERT(GetBackgroundPage()!=NULL);
		return GetBackgroundPage()->InitializeBackgroundPainter(bPaintBackground,
			bOnlyClientArea,lpszDIBResourceName,paintType,clrBk,lpszProfileName); 
	}

	virtual BOOL InitializeBackgroundPainter(const BOOL bPaintBackground,
		const BOOL bOnlyClientArea,	const UINT nDIBResourceID, 
		const PaintType paintType=Tile, const COLORREF clrBk=CLR_DEFAULT, 
		LPCTSTR lpszProfileName=_T("CustomizeBackgroundPainter")) 
	{ 
		ASSERT(GetBackgroundPage()!=NULL);
		return GetBackgroundPage()->InitializeBackgroundPainter(bPaintBackground,
			bOnlyClientArea,nDIBResourceID,paintType,clrBk,lpszProfileName); 
	}

	virtual BOOL InitializeBackgroundPainter(const BOOL bPaintBackground,
		const BOOL bOnlyClientArea, const CString& sDIBFileName, 
		const PaintType paintType=Tile, const COLORREF clrBk=CLR_DEFAULT, 
		LPCTSTR lpszProfileName=_T("CustomizeBackgroundPainter")) 
	{ 
		ASSERT(GetBackgroundPage()!=NULL);
		return GetBackgroundPage()->InitializeBackgroundPainter(bPaintBackground,
			bOnlyClientArea,sDIBFileName,paintType,clrBk,lpszProfileName); 
	}

	virtual BOOL InitializeBackgroundPainter(const BOOL bPaintBackground,
		const BOOL bOnlyClientArea, const COXDIB* pDIB, 
		const PaintType paintType=Tile, const COLORREF clrBk=CLR_DEFAULT, 
		LPCTSTR lpszProfileName=_T("CustomizeBackgroundPainter")) 
	{ 
		ASSERT(GetBackgroundPage()!=NULL);
		return GetBackgroundPage()->InitializeBackgroundPainter(bPaintBackground,
			bOnlyClientArea,pDIB,paintType,clrBk,lpszProfileName); 
	}

	virtual COXBackgroundPainterOrganizer* GetBackgroundPainterOrganizer() 
	{
		ASSERT(GetBackgroundPage()!=NULL);
		return GetBackgroundPage()->GetBackgroundOrganizer();
	}

	virtual COXCustomizeBackgroundPage* GetBackgroundPage() 
	{ 
		return &m_pageBackground; 
	}
#endif	//	OX_CUSTOMIZE_BACKGROUND


#ifdef OX_CUSTOMIZE_COOLCONTROLS
	virtual BOOL InitializeCoolControls(BOOL bConvertToCool=FALSE, 
		LPCTSTR lpszProfileName=_T("CustomizeCoolControls")) 
	{ 
		ASSERT(GetCoolControlsPage()!=NULL);
		return GetCoolControlsPage()->
			InitializeCoolControls(bConvertToCool,lpszProfileName); 
	}

	virtual COXCoolControlOrganizer* GetCoolControlOrganizer() 
	{
		ASSERT(GetCoolControlsPage()!=NULL);
		return GetCoolControlsPage()->GetCoolControlOrganizer();
	}

	virtual COXCustomizeCoolControlsPage* GetCoolControlsPage() 
	{ 
		return &m_pageCoolControls; 
	}
#endif	//	OX_CUSTOMIZE_COOLCONTROLS


#ifdef OX_CUSTOMIZE_TRAYICON
	virtual BOOL InitializeTrayIcon(HICON hIcon, UINT nContextMenuResourceID,
		LPCTSTR lpszIconTooltip=NULL, BOOL bShowTrayIcon=TRUE, 
		BOOL bUseRightClick=TRUE, LPCTSTR lpszIconSourceFileName=NULL,
		LPCTSTR lpszProfileName=_T("CustomizeTrayIcon")) 
	{ 
		ASSERT(GetTrayIconPage()!=NULL);
		return GetTrayIconPage()->InitializeTrayIcon(hIcon,nContextMenuResourceID,
			lpszIconTooltip,bShowTrayIcon,bUseRightClick,
			lpszIconSourceFileName,lpszProfileName); 
	}

	inline BOOL InitializeTrayIcon(UINT nIconResourceID, UINT nContextMenuResourceID,
		LPCTSTR lpszIconTooltip=NULL, BOOL bShowTrayIcon=TRUE, 
		BOOL bUseRightClick=TRUE, LPCTSTR lpszIconSourceFileName=NULL,
		LPCTSTR lpszProfileName=_T("CustomizeTrayIcon")) 
	{ 
		return InitializeTrayIcon(MAKEINTRESOURCE(nIconResourceID),
			nContextMenuResourceID,
			lpszIconTooltip,bShowTrayIcon,bUseRightClick,
			lpszIconSourceFileName,lpszProfileName); 
	}

	inline BOOL InitializeTrayIcon(LPCTSTR lpszIconResource, 
		UINT nContextMenuResourceID, LPCTSTR lpszIconTooltip=NULL, 
		BOOL bShowTrayIcon=TRUE, BOOL bUseRightClick=TRUE, 
		LPCTSTR lpszIconSourceFileName=NULL,
		LPCTSTR lpszProfileName=_T("CustomizeTrayIcon")) 
	{ 
		HICON hIcon=::LoadIcon(AfxGetResourceHandle(),lpszIconResource);
		if(hIcon==NULL)
			return FALSE;
		return InitializeTrayIcon(hIcon,nContextMenuResourceID,
			lpszIconTooltip,bShowTrayIcon,bUseRightClick,
			lpszIconSourceFileName,lpszProfileName); 
	}

	virtual COXTaskbarIcon* GetTrayIcon() 
	{
		ASSERT(GetTrayIconPage()!=NULL);
		return GetTrayIconPage()->GetTrayIcon();
	}

	virtual COXCustomizeTrayIconPage* GetTrayIconPage() { return &m_pageTrayIcon; }
#endif	//	OX_CUSTOMIZE_TRAYICON


#ifdef OX_CUSTOMIZE_INSTANCE_MANAGER
	// --- In  :	nMaxAllowedInstances	-	the maximum number of instances
	//											of the application allowed to run 
	//											simultaneously (-1 means there is 
	//											no limit)
	//				lpszProfileName			-	the name of the hive in the registry 
	//											where instance manager settings is 
	//											saved. If non-NULL value is set then 
	//											settings is retrieved from the 
	//											registry right away. These can 
	//											overwrite the value specified in 
	//											the previous parameter
	// --- Out : 
	// --- Returns: TRUE if instance manager was initialized successfully; 
	//				FALSE otherwise
	// --- Effect : Initializes and retrieve settings for instance manager object
	//				that can be used in order to access instances of the same 
	//				application running simultaneously
	virtual BOOL InitializeInstanceManager(
		int nMaxAllowedInstances=OX_MAX_NUM_INSTANCES_IN_LIST,
		LPCTSTR lpszProfileName=_T("CustomizeInstanceManager")) 
	{ 
		ASSERT(GetInstanceManagerPage()!=NULL);
		return GetInstanceManagerPage()->
			InitializeInstanceManager(nMaxAllowedInstances,lpszProfileName); 
	}

	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to internal COXInstanceManager object
	// --- Effect : Retrieves internal COXInstanceManager object
	virtual COXInstanceManager* GetInstanceManager() 
	{
		ASSERT(GetInstanceManagerPage()!=NULL);
		return GetInstanceManagerPage()->GetInstanceManager();
	}

	// --- In  :	bActivatePrevious	-	if TRUE then in the case the 
	//										number of allowed instances 
	//										has been reached, the last created 
	//										instance will be activated
	// --- Out : 
	// --- Returns: TRUE if the maximum allowed number of instances has been reached;
	//				FALSE otherwise
	// --- Effect : Retrieves a flag that specifies if the maximum allowed number of 
	//				instances has been reached. This function must be called in 
	//				OnCreate() handler of main window of the application before 
	//				calling the default implementation. In the case it returns TRUE
	//				you have to return -1 right away; otherwise you can go on 
	//				with creation of the main window of the application
	virtual BOOL CheckMaxAllowedInstances(BOOL bActivatePrevious=TRUE) 
	{
		ASSERT(GetInstanceManager()!=NULL);
		return GetInstanceManager()->CheckMaxAllowedInstances(bActivatePrevious);
	} 

	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Instance Manager page
	// --- Effect : Retrieves a pointer to Instance Manager page. This page can 
	//				be used to limit the number of simultaneously running instances
	//				of the application. Also it lists all running instances and
	//				allows you to apply some standard operations to them 
	//				("Close","Switch to...","Bring To Front","Maximize",
	//				"Minimize","Restore")
	virtual COXCustomizeInstanceManagerPage* GetInstanceManagerPage() 
	{ 
		return &m_pageInstanceManager; 
	}
#endif	//	OX_CUSTOMIZE_INSTANCE_MANAGER


#ifdef OX_CUSTOMIZE_SPLASHWINDOW
	virtual BOOL InitializeSplashWindow(UINT nBitmapResourceID, 
		BOOL bShowSplashWnd=TRUE, int nDisplayFor=-1, 
		BOOL bUserCanCancel=FALSE, COLORREF clrTransparent=CLR_DEFAULT, 
		LPPOINT pColorPoint=NULL, BYTE nTolerance=0, 
		LPCTSTR lpszProfileName=_T("CustomizeSplashWindow")) 
	{ 
		ASSERT(GetSplashWindowPage()!=NULL);
		return GetSplashWindowPage()->InitializeSplashWindow(nBitmapResourceID,
			bShowSplashWnd,nDisplayFor,bUserCanCancel,clrTransparent,pColorPoint,
			nTolerance,lpszProfileName); 
	}

	virtual COXSplashWnd* GetSplashWindow() 
	{
		ASSERT(GetSplashWindowPage()!=NULL);
		return GetSplashWindowPage()->GetSplashWindow();
	}

	virtual COXCustomizeSplashWindowPage* GetSplashWindowPage() { 
		return &m_pageSplashWindow; 
	}
#endif	//	OX_CUSTOMIZE_SPLASHWINDOW


#ifdef OX_CUSTOMIZE_INTELLIMOUSE
	virtual BOOL InitializeIntelliMouse(BOOL bSupportIntelliMouse=TRUE, 
		UINT nStartEventID=WM_MBUTTONDOWN, DWORD dwKeyState=0, BOOL bOnlyViews=TRUE,
		LPCTSTR lpszProfileName=_T("CustomizeIntelliMouse")) 
	{ 
		ASSERT(GetIntelliMousePage()!=NULL);
		return GetIntelliMousePage()->InitializeIntelliMouse(bSupportIntelliMouse,
			nStartEventID,dwKeyState,bOnlyViews,lpszProfileName); 
	}

	virtual COXIntelliMouseOrganizer* GetIntelliMouseOrganizer() 
	{
		ASSERT(GetIntelliMousePage()!=NULL);
		return GetIntelliMousePage()->GetIntelliMouseOrganizer();
	}

	virtual COXCustomizeIntelliMousePage* GetIntelliMousePage() { 
		return &m_pageIntelliMouse; 
	}
#endif	//	OX_CUSTOMIZE_INTELLIMOUSE


#ifdef OX_CUSTOMIZE_SHORTKEYS
	// --- In  :	pFrameWnd		-	pointer to main frame window of the 
	//									application
	//				lpszProfileName	-	the name of the hive in the registry 
	//									where updated accelerator tables contents
	//									is saved. If non-NULL value is set then 
	//									the contents is retrieved from the registry 
	//									and applied right away.
	// --- Out : 
	// --- Returns: TRUE if information on the application's accelerator tables was 
	//				initialized successfully; FALSE otherwise
	// --- Effect : Initializes the information on the application's accelerator 
	//				tables and retrieves their saved in the registry contents and 
	//				applies it
	virtual BOOL InitializeShortkeys(CFrameWnd* pFrameWnd, 
		LPCTSTR lpszProfileName=_T("CustomizeShortkeys")) 
	{ 
		ASSERT(GetShortkeysPage()!=NULL);
		return GetShortkeysPage()->InitializeShortkeys(pFrameWnd,lpszProfileName); 
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to COXShortkeysOrganizer object that is responsible for 
	//				saving/loading accelerator table contents and applying it to the 
	//				application
	// --- Effect : Retrieves a pointer to internal COXShortkeysOrganizer object
	virtual COXShortkeysOrganizer* GetShortkeysOrganizer() 
	{
		ASSERT(GetShortkeysPage()!=NULL);
		return GetShortkeysPage()->GetShortkeysOrganizer();
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to Customize Shortkeys page
	// --- Effect : Retrieves a pointer to Customize Shortkeys page. This page can 
	//				be used to customize the contents of application's accelerator
	//				table. New shortkeys can be assigned to existing commands
	//				or removed from accelerator table.
	virtual COXCustomizeShortkeysPage* GetShortkeysPage() { return &m_pageShortkeys; }
#endif	//	OX_CUSTOMIZE_SHORTKEYS

	//
	////////////////////////////////////

	// Generated message map functions
protected:
	//{{AFX_MSG(COXCustomizeManager)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnButtonApply();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnActivateItem(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnGetItemInfoTip(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////

#endif // !defined(_OXCUSTOMIZEMANAGER_H__)
