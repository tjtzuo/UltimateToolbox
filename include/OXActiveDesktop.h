//////////////////////////////////////////////////////////////////////
// ==========================================================================
//						   Class Specification : 
//							COXActiveDesktop
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////
// OXActiveDesktop.h: interface for the COXActiveDesktop class.
/*

	DESCRIPTION
	
	The COXActiveDesktop class is a wrapper for IActiveDesktop interface.
	In order to use this class the system should have Active Desktop installed.
	Usually it comes with Internet Explorer 4. To make sure the system has 
	it check version of shell32.dll - it must be 4.71 or later.
	The program that will use this class should have two calls ::CoInitialize()
	(usually in CWinApp::InitInstance()) and ::CoUninitialize() - (usually in
	CWinApp::ExitInstance()). The first function of the class that 
	should be called is COXActiveDesktop::Init() that must succeed in order 
	to continue to work with the class. Once this function has been called 
	it is possible to make calls to another functions.

	With these functions it is possible to create new items( AddItems()),
	modify items ( ModifyItem()) and remove them ( RemoveItem()).
	The items maybe like HTML file, website, control etc. 
	

*/

#if !defined(_OXACTIVEDESKTOP_H__)
#define _OXACTIVEDESKTOP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// v9.3 - update 05 - removed
//#include "stdafx.h"

#if !defined ADDURL_SILENT
#error You must include "wininet.h" before <afxdisp.h> and before  afxole.h in stdafx.h.
#endif

#include "oxdllext.h"

#define OX_ACTDESK_CHECK_ERROR(ERROR_RESULT) 	if (FAILED(hRslt))\
		{\
			m_nError=HRESULT_CODE(hRslt);\
			return ERROR_RESULT;\
		}
///////////////////////////////////////////////////////////////////////////
// Global functions
///////////////////////////////////////////////////////////////////////////

//copy TCHAR string to wide-character string.
//The caller is responsible for freeing memory 
//by operator delete []
LPWSTR GetWcharString(LPCTSTR pszSource);

//copy wide character string to TCHAR string.
//The caller is responsible for freeing memory 
//by operator delete []
LPTSTR GetTcharString(LPWSTR pszwSource);



///////////////////////////////////////////////////////////////////////////
// COXActiveDesktop
///////////////////////////////////////////////////////////////////////////
class OX_CLASS_DECL COXActiveDesktop  
{

public:

	// --- In  :	hWnd - handle of the parent window
	// --- Out : 
	// --- Returns:	
	// --- Effect : Constructor
	COXActiveDesktop(HWND hWnd=NULL);

	// --- In  :	Src - the source object 
	// --- Out : 
	// --- Returns:	
	// --- Effect : Copy constructor
	COXActiveDesktop(const COXActiveDesktop& Src);

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE if failed
	// --- Effect : Releases IActiveDesktop interface and reload it.
	BOOL Refresh();

	// --- In  :	pBuffer - pointer to the buffer of 8 byte size
	//						  with the new pattern
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Sets new pattern for Active Desktop
	BOOL SetPattern(BYTE* pBuffer);

	// --- In  :	
	// --- Out : pBuffer - pointer to the buffer of at least 8 bytes
	//						  size to allocate current pattern of the Active Desktop
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Retrieves current pattern of the Active Desktop
	BOOL GetPattern(BYTE* pBuffer);

	// --- In  :	
	// --- Out : 
	// --- Returns:	Last error
	// --- Effect : Call this function to get the last error
	inline UINT GetLastError(){	return m_nError;}

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : The first function that must be called to initialize
	//				the interface
	BOOL Init();
	
	// --- In  :	
	// --- Out : 
	// --- Returns:	Pointer to the IActiveDesktop interface
	// --- Effect : Call this function to obtain pointer to the 
	//				IActive Desktop interface
	inline IActiveDesktop* GetActiveDesktop()
	{
		ASSERT(m_pActiveDesktop); m_pActiveDesktop->AddRef();
		return m_pActiveDesktop;
	}

	// --- In  :	pComp - pointer to a COMPONENT structure that 
	//						describes new item.
	//				pszSource - NULL terminated URL string
	//				bDisplayDialog - TRUE to show dialog, FALSE otherwise
	//				bApply - TRUE to apply changes immediately
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Adds item to the Active Desktop
	BOOL AddItem(LPCOMPONENT pComp, LPCTSTR pszSource=NULL, 
		BOOL bDisplayDialog=FALSE, BOOL bApply=TRUE);

	// --- In  :	hWnd - handle of the parent window
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets new parent window
	inline void SetParent(HWND hWnd) {
		ASSERT(::IsWindow(hWnd)); m_hWnd=hWnd;}

	// --- In  :	
	// --- Out : 
	// --- Returns:	Handle of the parent window
	// --- Effect : Call this function to get handle of the parent window
	inline HWND GetParent() { return m_hWnd;}

	// --- In  :	pszFileName - HTML filename will be created
	//				pComp - component describing the specified item
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Generates HTML file for specified item
	BOOL CreateHTML(LPCTSTR pszFileName, LPCOMPONENT pComp);

	// --- In  :	pszSource - null terminated string that contains
	//							URL for HTML items or CLSID for controls
	// --- Out : 
	// --- Returns:	Pointer to the component. This pointer should be deleted by caller
	//				with operator delete
	// --- Effect : Returns pointer to COMPONENT structure describing specified item
	LPCOMPONENT GetComponent(LPCTSTR pszSource);

	// --- In  :	dwID - ID of the component (dwID member of the COMPONENT structure)
	// --- Out : 
	// --- Returns:	Pointer to the component. This pointer should be deleted by caller
	//				with operator delete
	// --- Effect : Returns pointer to COMPONENT structure describing specified item
	LPCOMPONENT GetComponent(DWORD dwID);

	// --- In  :	nComponent - number of the component. Must be less than returned
	//							 by GetItemCount()
	// --- Out : 
	// --- Returns:	Pointer to the component. This pointer should be deleted by caller
	//				with operator delete
	// --- Effect : Returns pointer to COMPONENT structure describing specified item
	LPCOMPONENT GetComponent(int nComponent);

	// --- In  :	
	// --- Out : 
	// --- Returns:	Number of the components in Active Desktop
	// --- Effect : Call this function to retrieve number of the component
	//				on the Active Desktop
	int GetItemCount();

	// --- In  :	
	// --- Out : pEnable - pointer to the BOOL variable that 
	//						shows enabled status of Active Desktop
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to find out if the Active Desktop is enabled 
	BOOL IsDesktopEnabled(BOOL* pEnable);

	// --- In  :	
	// --- Out : pEnable - pointer to the BOOL variable that 
	//						shows enabled status of the components on Active Desktop
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to find out if the components on Active Desktop is enabled 
	BOOL IsComponentsEnabled(BOOL* pEnable);

	// --- In  :	
	// --- Out :	sPattern - string that will repesents pattern with
	//							decimal numbers
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to get pattern as string
	BOOL GetPattern(CString& sPattern);

	// --- In  :	
	// --- Out : sWallpaper - filename of the wallpaper image
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to get wallpaper filename
	BOOL GetWallpaper(CString& sWallpaper);

	// --- In  :	
	// --- Out : 
	// --- Returns:	Wallpaper style(WPSTYLE_CENTER 
	//								WPSTYLE_TILE 
	//								WPSTYLE_STRETCH 
	//								WPSTYLE_MAX 
	//								) on success or -1 on error
	// --- Effect : Call this function to get the wallpaper style
	int GetWallpaperStyle();

	// --- In  : pComp - pointer to the modified COMPONENT structure
	//			 dwFlags - flags that shows how to apply changes	
	//			 bApply - TRUE if apply changes now or FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Modifies existing component
	BOOL ModifyItem(LPCOMPONENT pComp, DWORD dwFlags=AD_APPLY_ALL | AD_APPLY_FORCE, BOOL bApply=TRUE);

	// --- In  :	pComp - pointer to the COMPONENT structure describing item to remove
	//				bApply - TRUE if apply now, FALSE otherwise
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to remove specified item.
	BOOL RemoveItem(LPCOMPONENT pComp, BOOL bApply=TRUE);

	// --- In  :	bEnable - TRUE to enable components or FALSE to disable
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : 
	BOOL SetComponentsEnable(BOOL bEnable);

	// --- In  :	bEnable - TRUE to enable Active Desktop or FALSE to disable
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : 
	BOOL SetDesktopEnable(BOOL bEnable);

	// --- In  :	sPattern - string with 8 decimal numbers 
	//							describing the new pattern will be set
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to set new pattern
	BOOL SetPattern(CString sPattern);

	// --- In  :	sWallpaper - filename of the new wallpaper will be set on.
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to set new wallpaper for the Active Desktop
	BOOL SetWallpaper(CString sWallpaper);

	// --- In  :	dwStyle - new style will be applied for the wallpaper
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Call this function to apply the new style for the wallpaper
	BOOL SetWallpaperStyle(DWORD dwStyle);

	
	// --- In  :	dwFlags - flags determines how the changes will be applied
	//							(AD_APPLY_SAVE 
	//							AD_APPLY_HTMLGEN 
	//							AD_APPLY_REFRESH 
	//							AD_APPLY_ALL 
	//							AD_APPLY_FORCE )
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect : Applies changes for the Active Desktop.
	BOOL ApplyChanges(DWORD dwFlags=AD_APPLY_ALL);

	const COXActiveDesktop& operator=(const COXActiveDesktop& Src);

	virtual ~COXActiveDesktop();

protected:
	HWND m_hWnd;
	UINT m_nError;
	IActiveDesktop* m_pActiveDesktop;
};

#endif // !defined(_OXACTIVEDESKTOP_H__)
