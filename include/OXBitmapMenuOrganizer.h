// ==========================================================================
// 					Class Specification : COXBitmapMenuOrganizer
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                                            

// //////////////////////////////////////////////////////////////////////////

/*

  OVERVIEW

The COXBitmapMenuOrganizer class allows bitmap images to be seen alongside 
menu items, like those seen in Microsoft Office 97.

The bitmaps can be set explicitly for each menu item or, more simply, a call to a 
function can be made to set the corresponding bitmap for each menu item 
automatically from the toolbars of the Frame Window.

Alongside with displaying images next to menu items this class also implements
the functionality found in Microsoft Office 2000. Some of the menu items can be 
hidden and only so called recently used items is displayed. If there are hidden
items in displayed popup menu then the new item is added at the bottom that 
displays dropdown sign.By clicking on this item the hidden item will be displayed 
(in order to distinct them from recently used items they are displayed using
different background color). 


These classes feature:

	The Office 2000 menu look,

	One function call to add the bitmaps of your toolbar to the menu

	Easy to use interface


This class organizes the correspondence between menu items and their bitmaps 
to be shown. To handle the drawing of the items and their bitmaps, it also 
contains COXBitmapMenu objects. Consider them as owner-drawn submenus.


To add the bitmap in menu functionality, add a COXBitmapMenuOrganizer to the 
application's main window class, usually CMainFrame. Then you can use the 
following functions on the COXBitmapMenuOrganizer to set the bitmaps to menu items:

	BOOL SetMenuImage();
	BOOL SetMenuBitmap();
	BOOL SetMenuIcon();

	These function is called to set a particular image to a menu item.


	BOOL AutoSetMenuImage();

	This function is the most usefull among the all. It automatically sets the 
	images in toolbars of the window to corresponding menu items.


To allow hide items in menu you have to first of all call the following function:
	
	virtual void ShowOnlyRecentlyUsedItems(BOOL bShowOnlyRecentlyUsed);

and specify TRUE as a parameter. Then you can exclude/add any menu items from/to list
of the list of recently used items (by default all menu items are recently used) using
the following functions:

	virtual BOOL ExcludeFromRecentlyUsed(int nCmdID);
	virtual BOOL ExcludeSubmenuFromRecentlyUsed(HMENU hPopupMenu);

	virtual BOOL AddToRecentlyUsed(int nCmdID);
	virtual BOOL AddSubmenuToRecentlyUsed(HMENU hPopupMenu);


Trere are much more functions available for you (details are in the class reference)
but above mentioned functions are the ones that you would need to use.

  
!!!Important Notice!!!
These classes make use of the TB_GETIMAGELIST message. This message is only 
available with the updated common control header files which ship with the 
ActiveX SDK. It also means that code will only work if you have the new 
version of the common control DLL installed. i.e. on of Internet Explorer 3.x, 4.x or 
Office 97 is installed. 

*/


#ifndef OXBITMAPMENUORGANIZER_H
#define OXBITMAPMENUORGANIZER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXDllExt.h"


#include "OXBitmapMenu.h"
#include "OXMainRes.h"




// structure allows to save information about menu item (properties and position)

typedef struct _tagOXSAVEDMENUITEM
{
	int nIndex;
	MENUITEMINFO miInfo;
	CString sText;

	// default constructor
	_tagOXSAVEDMENUITEM() : nIndex(-1), sText(_T(""))
	{
		::ZeroMemory((void*)&miInfo,sizeof(miInfo));
		miInfo.cbSize=sizeof(miInfo);
	}

	// copy constructor
	_tagOXSAVEDMENUITEM(const _tagOXSAVEDMENUITEM& smi)
	{
		ASSERT(sizeof(miInfo)==smi.miInfo.cbSize);

		nIndex=smi.nIndex;
		miInfo=smi.miInfo;
		sText=smi.sText;
	}

	// assignment operator
	_tagOXSAVEDMENUITEM& operator=(const _tagOXSAVEDMENUITEM& smi)
	{
		ASSERT(sizeof(miInfo)==smi.miInfo.cbSize);
		
		if(this==&smi)
			return *this;

		nIndex=smi.nIndex;
		miInfo=smi.miInfo;
		sText=smi.sText;

		return *this;
	}

} OXSAVEDMENUITEM;
//
////////////////////////////////////////////////


class OX_CLASS_DECL COXArrSavedMenuItem : public CArray<OXSAVEDMENUITEM,OXSAVEDMENUITEM&>
{
public:
	// default constructor
	COXArrSavedMenuItem() {};

	// copy constructor
	COXArrSavedMenuItem(const COXArrSavedMenuItem& arrSavedMenuItems)
	{
		Append(arrSavedMenuItems);
	}

	// assignment operator
	COXArrSavedMenuItem& operator=(const COXArrSavedMenuItem& arrSavedMenuItems)
	{
		if(this==&arrSavedMenuItems)
			return *this;

		RemoveAll();
		Append(arrSavedMenuItems);

		return *this;
	}
};


// structure defines an active popup menu and its position on the screen
//
typedef struct _tagOXPOPUPMENUTRACE
{
	// handle to the popup menu
	HMENU m_hPopupMenu;
	// coordinates of the top level corner of the popup menu window
	CPoint m_ptTopLeft;

	// default constructor
	_tagOXPOPUPMENUTRACE() : m_hPopupMenu(NULL), m_ptTopLeft(0,0) {};

	// copy constructor
	_tagOXPOPUPMENUTRACE(const _tagOXPOPUPMENUTRACE& pmt) :
		m_hPopupMenu(pmt.m_hPopupMenu),	m_ptTopLeft(pmt.m_ptTopLeft) {};

	// assignment operator
	_tagOXPOPUPMENUTRACE& operator=(const _tagOXPOPUPMENUTRACE& pmt)
	{
		if(this==&pmt)
			return *this;

		m_hPopupMenu=pmt.m_hPopupMenu;
		m_ptTopLeft=pmt.m_ptTopLeft;

		return *this;
	}

} OXPOPUPMENUTRACE;
//
//////////////////////////////////////////////////////////



class OX_CLASS_DECL COXBitmapMenuOrganizer : public CObject
{
	friend class COXMenuBar;
//Data members --------------------------------------------------------

protected :
	// internal map that identifies a unique string with an image list object
	CMap<CString,LPCTSTR,CImageList*,CImageList*> m_mapStringToImageList;
	// internal map that associates commands with images
	CMap<DWORD,DWORD,COXImageInfo*,COXImageInfo*> m_mapCommandToImageInfo;
	// pointer to attached main frame window 
	CFrameWnd* m_pFrameWnd;

	// handle to the main frame window. Saved for internal purposes
	HWND	m_hWnd;
	// saved address of the window procedure of subclassed main frame window
	WNDPROC m_pfnSuper;
	// global array of all bitmap menu organizer objects
	static CMap<HWND,HWND,COXBitmapMenuOrganizer*,COXBitmapMenuOrganizer*> 
		m_allMenuOrganizers;

	// predefined image list that contains images for check and radio buttons
	CImageList	m_DefaultImgList;
	
	// flag that specifies that the menus is in the customization mode
	BOOL m_bInCutomizationMode;
	// recipient of customization related notifications 
	HWND m_hWndCustomizeOrganizer;

	// list of created bitmap menu objects
	CList<COXBitmapMenu*, COXBitmapMenu*> m_BitmapMenuList;

	// flag that specifies that only recently used items 
	// will be shawn in the menu. The other items will be 
	// displayed only if a user clicks on special item 
	// displayed at the bottom of popup menu
	BOOL m_bShowOnlyRecentlyUsed;
	// map of all commands that should be hidden when 
	// m_bShowOnlyRecentlyUsed is set to TRUE (the commands 
	// that are not recently used)
	CMap<int,int,int,int> m_mapHiddenCommands;
	// map of all submenus that should be hidden when 
	// m_bShowOnlyRecentlyUsed is set to TRUE (the submenus 
	// that are not recently used)
	CMap<HMENU,HMENU,int,int> m_mapHiddenSubmenus;
	// map of the items that has been deleted from corresponding menus
	// as a result of showing only recent used items
	CMap<COXBitmapMenu*,COXBitmapMenu*,COXArrSavedMenuItem,COXArrSavedMenuItem> 
		m_mapSavedMenuItems;

	// array of the chain of displayed popup menus
	CArray<OXPOPUPMENUTRACE,OXPOPUPMENUTRACE&> m_arrPopupMenusTrace;
	// flag specifies that main menu loop is active
	BOOL m_bMainMenuIsActive;
	// the number of currently displayed popup menus
	int m_nActivePopupMenuCount;
	// map of all active popup menus and popup windows that contain the menus
	CMap<HMENU,HMENU,HWND,HWND> m_mapPopupMenuWindows;

	// TD 9.3 this allows deferment of menu deletion in RestoreBitmapMenu
	COXBitmapMenu * m_pDeferedBM;

public:
	// array of created popup menus that should be destroyed at the end
	CArray<HMENU,HMENU> m_arrCreatedPopupMenus;

	// internal flag that specifies that hidden items must be displayed
	BOOL m_bForceToDisplayHiddenItems;

	// handle of the old mouse hook procedure
	static HHOOK g_pfnOldMouseHookProc;
	// handle of the old keyboard hook procedure
	static HHOOK g_pfnOldKeyboardHookProc;
	/////////////////


//Member functions-----------------------------------------------------------------------
public :

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBitmapMenuOrganizer();

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBitmapMenuOrganizer();


	// --- In  :	hWnd	-	handle to main frame window
	// --- Out : 
	// --- Returns:	A pointer to COXBitmapMenuOrganizer object attached to given main 
	//				frame window if succeed; otherwise NULL
	// --- Effect : Static function that allows you to find COXBitmapMenuOrganizer 
	//				object attached to given main frame window
	static COXBitmapMenuOrganizer* FindOrganizer(HWND hWnd);
	

	// --- In  :	hPopupMenu	-	handle to popup menu (submenu) for which we should 
	//								find a path (array of menu item indexes) from 
	//								specified parent menu
	//				hParentMenu	-	handle to a menu that serves as a starting point 
	//								for searching the path to specified
	// --- Out :	arrPath		-	reference to array that will be populated with 
	//								menu indexes that defines the path to given 
	//								submenu if function succeeds
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Static function that allows you to find a path (array of menu 
	//				item indexes) from specified parent menu to specified submenu.
	//				Can be used in order to save the location of submenu item
	static BOOL FindPopupMenuPath(HMENU hPopupMenu, HMENU hParentMenu, 
		CArray<int,int>& arrPath);
	

	// --- In  :	pFrameWnd	-	The frame window which menus will be organized by 
	//								this object
	// --- Out : 
	// --- Returns:	TRUE if succeed; otherwise FALSE
	// --- Effect : Attaches main frame window of application. This function must be 
	//				called before any other action can be performed
	BOOL AttachFrameWnd(CFrameWnd* pFrameWnd);


	// --- In  : 
	// --- Out : 
	// --- Returns:	The frame window which menus were being organized by this object
	// --- Effect : Detaches the attached main frame window
	CFrameWnd* DetachFrameWnd();


	// --- In  :	bCopyText	-	if set to TRUE the contents of converted menus 
	//								will be reset to original state 
	// --- Out : 
	// --- Returns:
	// --- Effect : Object is cleaned up, but no frame window is detached
	void Empty(BOOL bCopyText=TRUE);

	
	// --- In  :  nCommandID	-	ID of the menu item to which the given image
	//								will be set
	//			  pszBitmapID	-	The resource string of the bitmap whose 
	//								image will be set to the menu item
	//			  hIcon			-	Icon as an image
	//			  pbmImage		-	Bitmap as an image
	//			  crMask		-	Mask for the bitmap
	//			  nBitmapIndex	-	The index of the image in the 
	//								bitmap to set to the item
	//			  nCx			-	Width of each button in the bitmap
	//			  nCy			-	Height of the image		
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Assigns the given image to the given menu item. 
	BOOL SetMenuImage(UINT nCommandID, LPCTSTR pszBitmapID, 
		int nBitmapIndex, int nCx);
	BOOL SetMenuImage(UINT nCommandID, HICON hIcon, int nCx, int nCy);
	BOOL SetMenuImage(UINT nCommandID, CBitmap* pbmImage, COLORREF crMask, 
		int nCx, int nCy);

	// --- In  :  nCommandID	-	ID of the menu item to which the given image
	//								will be set
	//			  pszBitmapID	-	The resource string of the bitmap whose 
	//								image will be set to the menu item
	//			  nBitmapIndex	-	The index of the image in the 
	//								bitmap to set to the item 
	//			  nCx			-	Width of each button in the bitmap
	//			  hIcon			-	Icon as an image
	//			  pbmImage		-	Bitmap as an image
	//			  crMask		-	Mask for the bitmap
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Sets the given image in the bitmap to the menu item. 
	//				This image will be displayed when the item is selected
	//				(hot state)
	BOOL SetMenuHotImage(UINT nCommandID, LPCTSTR pszBitmapID, 
		int nBitmapIndex, int nCx);
	BOOL SetMenuHotImage(UINT nCommandID, HICON hIcon);
	BOOL SetMenuHotImage(UINT nCommandID, CBitmap* pbmImage, COLORREF crMask);


	// --- In  :	nCommandID	-	command ID of the menu item
	//				pszBitmapID	-	The resource string of the bitmap 
	//								to set to the menu item
	//				nBitmapID	-	The resource ID of the bitmap 
	//								to set to the menu item
	// --- Out : 
	// --- Returns:	TRUE if succeed; otherwise FALSE
	// --- Effect:	Sets the given bitmap to the given menu item. 
 	BOOL SetMenuBitmap(UINT nCommandID, LPCTSTR pszBitmapID);
	BOOL SetMenuBitmap(UINT nCommandID, UINT nBitmapID);

	// --- In  :	nCommandID	-	command ID of the menu item
	//				pszBitmapID	-	The resource string of the bitmap 
	//								to set to the menu item
	//				nBitmapID	-	The resource ID of the bitmap 
	//								to set to the menu item
	// --- Out : 
	// --- Returns:	TRUE if succeed; otherwise FALSE
	// --- Effect:	Sets the given bitmap to the given menu item. 
	//				This image will be displayed when the item is selected
	//				(hot state)
 	BOOL SetMenuHotBitmap(UINT nCommandID, LPCTSTR pszBitmapID);
 	BOOL SetMenuHotBitmap(UINT nCommandID, UINT nBitmapID);


	// --- In  :	nCommandID	-	command ID of the menu item
	//				pszIconID	-	The resource String of the icon
	//								to set to the menu item
	//				nIconID		-	The resource ID of the icon
	//								to set to the menu item
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Sets the given icon to the menu item
	BOOL SetMenuIcon(UINT nCommandID, LPCTSTR pszIconID);
	BOOL SetMenuIcon(UINT nCommandID, UINT nIconID);


	// --- In  :	nCommandID	-	command ID of the menu item
	//				pszIconID	-	The resource String of the icon
	//								to set to the menu item
	//				nIconID		-	The resource ID of the icon
	//								to set to the menu item
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Sets the given icon to the menu item. 
	//				This image will be displayed when the item is selected
	//				(hot state)
	BOOL SetMenuHotIcon(UINT nCommandID, LPCTSTR pszIconID);
	BOOL SetMenuHotIcon(UINT nCommandID, UINT nIconID);


	// --- In  :	nCommandID	-	command ID of the menu item
	// --- Out : 
	// --- Returns: A pointer to the COXImageInfo object associated with the 
	//				menu item if any exist; otherwise NULL
	// --- Effect : Retrieve the info about the image associated with the menu item
	COXImageInfo* GetMenuItemImageInfo(UINT nCommandID) const;

	
	// --- In  :  
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Sets the toolbar images to the menu items automatically.
	BOOL AutoSetMenuImage();


	// --- In  :  
	// --- Out : 
	// --- Returns:	TRUE if a frame window is attached to the object;
	//				FALSE otherwise	
	// --- Effect : Retrieves the flag that specifies if any frame window is
	//				attached to the object
	inline BOOL IsAttached() const { return (m_pFrameWnd!=NULL); }


	// --- In  :	pPopupMenu	-	Specifies the menu object of the pop-up menu. 
	//								May be temporary and should not be stored 
	//								for later use	
	//				nIndex		-	Specifies the index of the pop-up menu in the 
	//								main menu
	//				bSysMenu	-	TRUE if the pop-up menu is the Control menu; 
	//								otherwise FALSE
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Handles WM_INIPOPUPMENU message. As a result menu items are 
	//				converted to owner-drawn style
	virtual void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);


	// --- In:		nChar	-	Specifies the character that was pressed
	//				nFlags	-	Contains the MF_POPUP flag if the menu 
	//							is a pop-up menu. It contains the MF_SYSMENU flag if 
	//							the menu is a Control menu
	//				pMenu	-	Contains a pointer to the selected CMenu
	// --- Out : 
	// --- Returns:	The high-order word of the return value should contain one of 
	//				the following command codes:
	//
	//						0	Tells Windows to discard the character that the user 
	//							pressed and creates a short beep on the system 
	//							speaker. 
	//						1	Tells Windows to close the current menu. 
	//						2	Informs Windows that the low-order word of the return 
	//							value contains the item number for a specific item. 
	//							This item is selected by Windows. 
	//						
	//				The low-order word is ignored if the high-order word contains 
	//				0 or 1. Applications should process this message when accelerator 
	//				(shortcut) keys are used to select bitmaps placed in a menu
	// --- Effect:	Handles WM_MENUCHAR message in order to support accelerator keys. 
	//				Refer to article Q121623 in MSDN Knowledge base for details
	virtual LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);


	// --- In:		nIDCtl				-	The ID of the control
	//				lpMeasureItemStruct	-	Points to a MEASUREITEMSTRUCT data 
	//										structure that contains the dimensions 
	//										of the owner-draw control
	// --- Out: 
	// --- Returns: TRUE if the message was handled, FALSE otherwise
	// --- Effect:	Handles WM_MEASUREITEM message in order to adjust menu size 
	//				to fit the size of images
	virtual BOOL OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);


	// --- In  :	bInCutomizationMode		-	TRUE if popup menus must be set in 
	//											customization mode, otherwise menus 
	//											will be set in the normal mode
	//				hWndCustomizeOrganizer	-	recipient window of customization 
	//											related notifications
	// --- Out : 
	// --- Returns: 
	// --- Effect : Sets popup menus displayed in the framework to customization
	//				or normal mode
	void SetCutomizationMode(BOOL bInCutomizationMode,
		HWND hWndCustomizeOrganizer=NULL);

	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if bitmap menus in the attached frame are in customization
	//				mode, otherwise FALSE
	// --- Effect : Retrieves the flag that specifies if popup menus in the attached 
	//				frame window are in the customization mode or not
	inline BOOL IsInCustomizationMode() const { return m_bInCutomizationMode; }


	// --- In  :	pBitmapMenu		-	a pointer to the COXBitmapMenu object that
	//									will be converted to fully functional object
	//				bNewBitmapMenu	-	flag that is set to TRUE if pBitmapMenu is 
	//									raw COXBitmapMenu object that have never been 
	//									converted before
	// --- Out : 
	// --- Returns: 
	// --- Effect : Converts the specified object to fully functional object with 
	//				support for displaying bitmaps next to menu items
	virtual void ConvertBitmapMenu(COXBitmapMenu* pBitmapMenu, 
		BOOL bNewBitmapMenu=TRUE);

	// --- In  :	pBitmapMenu		-	a pointer to the COXBitmapMenu object that
	//									will be converted back to normal menu
	//				bCopyText		-	restores the text that was originally
	//									assigned to menu items (when menu was 
	//									converted all items were assigned to be 
	//									ownerdrawn)
	// --- Out : 
	// --- Returns: 
	// --- Effect : Converts the specified object back to standard menu
	virtual void RestoreBitmapMenu(COXBitmapMenu* pBitmapMenu, BOOL bCopyText=TRUE);


	// --- In  :	bShowOnlyRecentlyUsed	-	if TRUE then only recently used items 
	//											will be displayed in popup menus;
	//											if FALSE then popup menus will behave 
	//											in normal way
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the display mode for popup menus. 
	virtual void ShowOnlyRecentlyUsedItems(BOOL bShowOnlyRecentlyUsed);

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE when only recently used items are displayed in popup menus;
	//				FALSE when popup menus behave in normal way
	// --- Effect : Retrieves the display mode for popup menus
	inline BOOL IsShowOnlyRecentlyUsedItems() const { return m_bShowOnlyRecentlyUsed; }


	// --- In  :	nCmdID		-	command ID of the item that should be 
	//								excluded from the list of recently used items
	//				pMenu		-	a pointer to popup menu
	//				nItemIndex	-	index of the menu item in the given popup menu 
	//								that should be excluded from the list of recently 
	//								used items
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Excludes the item with specified command ID or in specified popup 
	//				menu from the list of recently used items. 
	virtual BOOL ExcludeFromRecentlyUsed(int nCmdID);
	BOOL ExcludeFromRecentlyUsed(CMenu* pMenu, int nItemIndex);

	// --- In  :	hPopupMenu	-	a handle to submenu that associated with 
	//								a menu item that will be excluded from the list
	//								of recently used menu items
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Excludes the item that assigned to specified submenu from 
	//				the list of recently used items. 
	virtual BOOL ExcludeSubmenuFromRecentlyUsed(HMENU hPopupMenu);


	// --- In  :	nCmdID		-	command ID of the item that should be added to 
	//								the list of recently used items
	//				pMenu		-	a pointer to popup menu
	//				nItemIndex	-	index of the menu item in the given popup menu 
	//								that should be added to the list of recently 
	//								used items
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Adds the item with specified command ID or in specified popup 
	//				menu to the list of recently used items. 
	virtual BOOL AddToRecentlyUsed(int nCmdID);
	BOOL AddToRecentlyUsed(CMenu* pMenu, int nItemIndex);

	// --- In  :	hPopupMenu	-	a handle to submenu that associated with 
	//								a menu item that will be added to the list
	//								of recently used menu items
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Adds the item that assigned to specified submenu to the list of 
	//				recently used items. 
	virtual BOOL AddSubmenuToRecentlyUsed(HMENU hPopupMenu);


	// --- In  :	nCmdID		-	command ID of an item
	//				pMenu		-	a pointer to popup menu
	//				nItemIndex	-	index of a menu item
	// --- Out : 
	// --- Returns:	TRUE if specified menu item is recently used one; FALSE otherwise
	// --- Effect : Retrieves the flag that specifies whether the given menu item is
	//				recently used one or not. 
	BOOL IsRecentlyUsed(int nCmdID) const;
	BOOL IsRecentlyUsed(CMenu* pMenu, int nItemIndex) const; 

	// --- In  :	hPopupMenu	-	a handle to submenu
	// --- Out : 
	// --- Returns:	TRUE if menu item assigned to specified submenu is recently 
	//				used one; FALSE otherwise
	// --- Effect : Retrieves the flag that specifies whether the menu item assigned
	//				to given submenu is	recently used one or not. 
	BOOL IsSubmenuRecentlyUsed(HMENU hPopupMenu) const;


	// --- In  :	hPopupMenu	-	handle of valid popup menu
	// --- Out : 
	// --- Returns:	TRUE if specified popup menu is top level popup menu of main
	//				menu associated with the attached window
	// --- Effect : Retrieves the flag that specifies whether the given popup menu 
	//				is top level popup menu of main	menu associated with the attached 
	//				window
	BOOL IsTopLevelPopupMenu(HMENU hPopupMenu);


	// --- In  :	lpszSubKey	-	hive in registry where information about
	//								recently used menu commands will be saved
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Saves the list of recently used menu commands in registry
	BOOL SaveRUCommandsState(LPCTSTR lpszSubKey);

	// --- In  :	lpszSubKey	-	hive in registry where information about
	//								recently used menu commands are saved
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Restores the contents of the list of recently used menu commands 
	//				from registry
	BOOL LoadRUCommandsState(LPCTSTR lpszSubKey);


	// --- In  :	hMenu		-	handle to main menu to which current submenu items
	//								positions are relevant
	//				lpszSubKey	-	hive in registry where information about
	//								recently used submenu items positions will be saved
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Saves the list of recently used submenu items positions in registry
	BOOL SaveRUSubmenusState(HMENU hMenu, LPCTSTR lpszSubKey);

	// --- In  :	hMenu		-	handle to main menu to which saved submenu items
	//								positions are relevant
	//				lpszSubKey	-	hive in registry where information about
	//								recently used submenu items positions are saved
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Restores the contents of the list of recently used submenu items 
	//				from registry
	BOOL LoadRUSubmenusState(HMENU hMenu, LPCTSTR lpszSubKey);

	// --- In  : 
	// --- Out : 
	// --- Returns: Succeeded or not
	// --- Effect : Creates the two bitmaps needed to draw a checkmark and a bullet
	BOOL Init(UINT cx=NULL, UINT cy=NULL, UINT nCheckID=NULL, UINT nBulletID=NULL);

protected:

	// saves/load recent used menu commands list from/to archive object
	virtual BOOL SaveRUCommandsContents(CArchive& ar);
	virtual BOOL LoadRUCommandsContents(CArchive& ar);

	// saves/load recent used submenu items list from/to archive object
	virtual BOOL SaveRUSubmenusContents(HMENU hMenu, CArchive& ar);
	virtual BOOL LoadRUSubmenusContents(HMENU hMenu, CArchive& ar);

	// assigns a unique string to new image list object or returns the 
	// existing unique string for registered image list objects
	CString GetUniqueResourceString(LPCTSTR pszResource,BOOL bBitmapIcon=TRUE);
	CString GetUniqueResourceString(UINT_PTR uResourceID,BOOL bBitmapIcon=TRUE);

	// subclasses attached main frame window in standard Windows way (not MFC)
	BOOL SubclassFrameWindow(CFrameWnd* pFrameWnd);
	// unsubclasses attached main frame window
	void UnsubclassFrameWindow();

	// global window procedure for every created COXBitmapMneuOrganizer objects
	static LRESULT CALLBACK GlobalMenuOrganizerProc(HWND hWnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);
	// window procedure that handles menu related messages sent to attached 
	// main frame window
	LRESULT MenuOrganizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// static function that finds specified menu command in the specified menu
	static CMenu* OwnFindPopupMenuFromID(CMenu* pMenu, UINT nID);

	// creates new menu and copies the contents of the given menu into it. 
	// You must not destroy the menu that is created by this function
	HMENU CopyPopupMenu(HMENU hMenuDest);

	// updates internal iterator of currently active popup menus
	void UpdateActivePopupMenuCount();

	// hook procedure for mouse messages
	static LRESULT CALLBACK MenuMouseHookProc(int nCode, WPARAM wParam, 
		LPARAM lParam);
	// hook procedure for keyboard messages
	static LRESULT CALLBACK MenuKeyboardHookProc(int nCode, WPARAM wParam, 
		LPARAM lParam);
};

//This a private class as a data structure used in both 
//COXBitmapMenu and COXBitmapMenuOrganizer
class OX_CLASS_DECL COXImageInfo 
{
protected:
	CImageList* m_pImageList;
	int			m_nIndex;
	BOOL		m_bDefaultImgList;
	int			m_nHotIndex;

public :
	COXImageInfo(CImageList* pImageList,int nIndex, BOOL bDefaultImgList=FALSE,
		int nHotIndex=-1)
		:
		m_pImageList(pImageList),
		m_nIndex(nIndex),
		m_bDefaultImgList(bDefaultImgList),
		m_nHotIndex(nHotIndex)

	{
	}

	inline CImageList* GetImageList() const
	{
		ASSERT((m_pImageList == NULL) || 
			(AfxIsValidAddress(m_pImageList, sizeof(*m_pImageList), FALSE)));
		return m_pImageList;
	}

	inline void SetImageList(CImageList* pImageList)
	{
		ASSERT((pImageList == NULL) || 
			(AfxIsValidAddress(pImageList, sizeof(*pImageList), FALSE)));
		m_pImageList=pImageList;
	}

	inline int GetIndex() const
	{
		return m_nIndex;
	}

	inline void SetIndex(int nIndex)
	{
		m_nIndex=nIndex;
	}

	inline int GetHotIndex() const
	{
		return m_nHotIndex;
	}

	inline void SetHotIndex(int nHotIndex)
	{
		m_nHotIndex=nHotIndex;
	}

	inline BOOL IsDefaultImgList() const
	{
		return m_bDefaultImgList;
	}

	inline void SetDefaultImgList(BOOL bDefaultImgList)
	{
		m_bDefaultImgList=bDefaultImgList;
	}
};

//This a private class as a data structure used in both 
//COXBitmapMenu and COXBitmapMenuOrganizer
class OX_CLASS_DECL COXItemInfo 
{
protected:
	COXImageInfo* m_pImageInfo;
	CString m_sText;
	BOOL m_bRecentUsed;

public :
	COXItemInfo(COXImageInfo* pImageInfo, CString sText)
		:
		m_pImageInfo(pImageInfo),
		m_sText(sText)
	{
	}

	inline void SetImageInfo(COXImageInfo* pImageInfo)
	{
		ASSERT((pImageInfo == NULL) || 
			(AfxIsValidAddress(pImageInfo, sizeof(*pImageInfo), FALSE)));
		m_pImageInfo=pImageInfo;
	}
	
	inline COXImageInfo* GetImageInfo() const
	{
		ASSERT((m_pImageInfo == NULL) || 
			(AfxIsValidAddress(m_pImageInfo, sizeof(*m_pImageInfo), FALSE)));
		return m_pImageInfo;
	}

	inline void SetText(CString sTxt)
	{
		m_sText = sTxt;
	}
	
	inline CString GetText() const
	{
		return m_sText;
	}
};


#endif //OXBITMAPMENUORGANIZER_H
/////////////////////////////////////////////////////////////////////////////
