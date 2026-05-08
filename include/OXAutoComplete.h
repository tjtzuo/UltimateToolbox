// ==========================================================================
// 					Class Specification : COXAutoComplete
// ==========================================================================
// Header file : OXAutoComplete.h
// Version: 9.3
// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
//
// //////////////////////////////////////////////////////////////////////////
//
// Description:
//
//	 The COXAutoComplete class provided similar capabilities to the user
//	like IE addressbar. It will remember your last values you've entered in
//	the edit box (number of the values for a window you can set by 
//	SetDepth()) function. And while you are typing it will suggest you some values
//	from your most recently used ones. 
//	To use this class you may have just one instance for you application.
//	Call Attach() function for every window you want to provide this
//	functionality. If you want to provide separate list of values for
//	the windows you should supply different names for every window in
//	Attach(). The data is persistent and will be saved in the registry.
//	You do not need call Detach() - it will be done on destruction.
//	To add value to the list you must call Complete() function telling
//	to the COXAutoComplete to add this value to the list.
//	Usually it will be when user pressed enter.
//	The class has two flags that define his behavior.
//	When you call Attach() if you set OX_AUTOCOMPLETE_LIST flag, it will
//	bring you list box to show you possible values.
//	If you set OX_AUTOCOMPLETE_APPEND style it will fill edit box 
//	with recommended value. By default all they are set on.
//	
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
//////////////////////////////////////////////////////////////////////
//
// Dependencies:
// OXAutoListBox.h
// OXAutoComplete.h
// OXRegistryValFile.h
//
// OXAutoListBox.cpp
// OXAutoComplete.cpp
// OXRegistryValFile.cpp


#if !defined(_OXAUTOCOMPLETE_H__)
#define _OXAUTOCOMPLETE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "oxdllext.h"
#include "afxtempl.h"
#include "OXAutoListBox.h"
#include "UTB64Bit.h"

#ifndef OX_AUTOCOMPLETE_DEPTHDEFAULT
#define OX_AUTOCOMPLETE_DEPTHDEFAULT 20
#endif

#ifndef OX_AUTOCOMPLETE_NAMEDEFAULT
#define OX_AUTOCOMPLETE_NAMEDEFAULT _T("Common")
#endif

#ifndef OX_AUTOCOMPLETE_VERSION
#define OX_AUTOCOMPLETE_VERSION 0x100
#endif

#ifndef OX_AUTOCOMPLETE_HEIGHTDEFAULTMAX 
#define OX_AUTOCOMPLETE_HEIGHTDEFAULTMAX 60
#endif

#define OX_AUTOCOMPLETE_LIST 0x01
#define OX_AUTOCOMPLETE_APPEND 0x02

///////////////////////////////////////////////////////////////////////////
//
// class COXAutoStorage
//
///////////////////////////////////////////////////////////////////////////
//
// Description:
// Internal helper class
//
///////////////////////////////////////////////////////////////////////////

class COXAutoStorage : public CObject
{
public:
	UINT GetDepth();
	void SetDepth(UINT nDepth);
	COXAutoStorage(LPCTSTR lpszName=NULL, 
		UINT nDepth=OX_AUTOCOMPLETE_DEPTHDEFAULT);
	~COXAutoStorage();
	CString GetName() const { return m_sName;}
	UINT GetMatchedStrings(CString sText, CStringArray& arsStrings);
	BOOL AddString(CString sText);
	BOOL Load();
	BOOL Save();
	inline int GetCount() { return PtrToInt( m_arsContents.GetSize() );}
protected:
	CString m_sName;
	UINT	m_nDepth;
	CStringArray m_arsContents;
};

///////////////////////////////////////////////////////////////////////////
//
// class COXAutoComplete
//
///////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXAutoComplete : public CObject  
{

//functions:
public:

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructor
	COXAutoComplete(HWND hParentWnd=NULL);

	// --- In  : hWnd - handle of the window to attach to 
	//			 lpszStorageName - name of the entry in the registry where
	//							   list of entries is saved
	//			 dwOptions - options - see description of the class
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : Attaches list of entries to the window
	BOOL Attach(HWND hWnd, LPCTSTR lpszStorageName=NULL,
		DWORD dwOptions=OX_AUTOCOMPLETE_LIST | OX_AUTOCOMPLETE_APPEND);

	// --- In  : pWnd - A pointer to the window to attach to 
	//			 lpszStorageName - name of the entry in the registry where
	//							   list of entries is saved
	//			 dwOptions - options - see description of the class
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : Attaches list of entries to the window
	BOOL Attach(CWnd* pWnd, LPCTSTR lpszStorageName=NULL, 
		DWORD dwOptions=OX_AUTOCOMPLETE_LIST | OX_AUTOCOMPLETE_APPEND);

	// --- In  : hWnd - A handle to the window to detach from
	// --- Out : 
	// --- Returns :
	// --- Effect : Detaches the window
	void Detach(HWND hWnd);

	// --- In  : pWnd - A pointer to the window to detach from
	// --- Out : 
	// --- Returns :
	// --- Effect : Detaches the window
	void Detach(CWnd* pWnd=NULL);

	// --- In  : hWnd - A handle of the window to add entry to the list from
	// --- Out : 
	// --- Returns :
	// --- Effect : Adds entry to the list for
	//				the specified window
	void Complete(HWND hWnd);

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Hides listbox
	void Hide();

	// --- In  : hWnd - A handle to the window 
	// --- Out : 
	// --- Returns : maximum number of entries in the list for the window
	// --- Effect : Returns maximum number of the entries in the list
	int GetDepth(HWND hWnd);

	// --- In  : hWnd - A handle to the window 
	//			 nDepth - maximum number of entries in the list for the window
	// --- Out : 
	// --- Returns :
	// --- Effect :Sets new maximum entries for the list
	void SetDepth(UINT nDepth, HWND hWnd);

	COXAutoStorage* GetStorage(HWND hWnd);
	virtual ~COXAutoComplete();

protected:

	// --- In  : hwnd - A handle of the window where text has been changed
	//			 sNewText - a new text of the window
	// --- Out : 
	// --- Returns : TRUE on success, FALSE otherwise
	// --- Effect : This function called internally when text has been 
	//              changed in a window that was attached
	virtual BOOL OnContentsChange(HWND hwnd, CString sNewText);

	void ChangeSel(int nKey);
	static	LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam,
				LPARAM lParam);
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, 
				LPARAM lParam);

//members
public:
	void SetParent(HWND hParentWnd);

	//handle of the window where listbox currently is attached to
	HWND m_hAttached;

protected:
	HWND m_hParent;
	BOOL m_bUpdate;
	COXAutoListBox m_lstBox;
	CMap<HWND,HWND,COXAutoStorage*,COXAutoStorage*> m_mpStorage;
	CMap<HWND,HWND,DWORD,DWORD> m_mpOptions;
	HHOOK m_hkMsg;
	HHOOK m_hkKbrd;
	static COXAutoComplete* m_pThis;
};

#endif // !defined(_OXAUTOCOMPLETE_H__)
