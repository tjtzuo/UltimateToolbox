// =============================================================================
// 							Class Implementation : COXTaskbarIcon
// =============================================================================
//
// Source file : 		OXTaskbarIcon.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXTaskbarIcon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXTaskbarIcon, CWnd)

#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(COXTaskbarIcon, CWnd)
	//{{AFX_MSG_MAP(COXTaskbarIcon)
	ON_MESSAGE(WM_NOTIFY_TASKBAR_ICON, OnNotifyTaskbarIcon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Data members -------------------------------------------------------------
// protected:
//		protected:
//			BOOL m_bShowing;		a flag to indicate whether the icon is
//									showing on the taskbar.
//			UINT m_nTaskbarIconID;  the identifier of a taskbar icon, will
//									not be altered after calling Create()
// private:

// Member functions ---------------------------------------------------------
// public:

COXTaskbarIcon::COXTaskbarIcon()
	{
	m_bShowing = FALSE;
	}

COXTaskbarIcon::~COXTaskbarIcon()
	{
	if (::IsWindow(m_hWnd))		  
		{
		// ... Create() has been called
		if (IsShowing()) 
			// ... Delete the icon from taskbar
			Hide();  
		::DestroyWindow(m_hWnd);
		}
	}

BOOL COXTaskbarIcon::Create(UINT nTaskbarIconID /* = 0 */)
	{
	ASSERT(!::IsWindow(m_hWnd));
	m_nTaskbarIconID = nTaskbarIconID;

	// create an invisible, less memory consuming, and global window class
	return CWnd::CreateEx(0, AfxRegisterWndClass(CS_CLASSDC | CS_GLOBALCLASS), NULL, 0,
		0, 0, 0, 0, NULL, NULL);
	}

void COXTaskbarIcon::SetTooltipText(LPCTSTR lpszString, BOOL bRefresh /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));
	SetWindowText(lpszString);
	if (IsShowing() && bRefresh) 
		Refresh();
	}

void COXTaskbarIcon::SetTooltipText(UINT nIDResource, BOOL bRefresh /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));
	CString sTooltipText;
	VERIFY(sTooltipText.LoadString(nIDResource));
	SetTooltipText(sTooltipText, bRefresh);
	}

HICON COXTaskbarIcon::SetIcon(HICON hIcon, BOOL bRefresh /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(hIcon);
	HICON hPrevIcon = CWnd::SetIcon(hIcon, FALSE);
	if (IsShowing() && bRefresh) 
		Refresh();
	return hPrevIcon;
	}

HICON COXTaskbarIcon::SetIcon(UINT nIDResource, BOOL bRefresh /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));
	HICON hIcon = ::LoadIcon(AfxFindResourceHandle(MAKEINTRESOURCE(nIDResource),
		RT_GROUP_ICON), MAKEINTRESOURCE(nIDResource));
	ASSERT(hIcon);
	return SetIcon(hIcon, bRefresh);
	}

HICON COXTaskbarIcon::SetIcon(LPCTSTR lpszResourceName, BOOL bRefresh /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));
	HICON hIcon = ::LoadIcon(AfxFindResourceHandle(lpszResourceName,
		RT_GROUP_ICON), lpszResourceName);
	ASSERT(hIcon);
	return SetIcon(hIcon, bRefresh);
	}

BOOL COXTaskbarIcon::Show()
	{
	ASSERT(::IsWindow(m_hWnd));
	if (IsShowing())
		{
		TRACE0("COXTaskbarIcon::Show : Warning: Called after icon is showed.\n");
		return FALSE;
		}
	BOOL bSuccess = ShellNotify(NIM_ADD);
	if (bSuccess) m_bShowing = TRUE;
	return bSuccess;
	}

BOOL COXTaskbarIcon::Hide()
	{
	ASSERT(::IsWindow(m_hWnd));
	if (!IsShowing())
		{
		TRACE0("COXTaskbarIcon::Hide : Warning: Called before icon is showed.\n");
		return FALSE;
		}
	BOOL bSuccess = ShellNotify(NIM_DELETE);
	if (bSuccess) m_bShowing = FALSE;
	return bSuccess;
	}

BOOL COXTaskbarIcon::Refresh()
	{
	ASSERT(::IsWindow(m_hWnd));
	if (!IsShowing())
		{
		TRACE0("COXTaskbarIcon::Refresh : Warning: Called before icon is showed.\n");
		return FALSE;
		}
	return ShellNotify(NIM_MODIFY);
	}

void COXTaskbarIcon::GetTooltipText(CString& rString) const
	{
	ASSERT(::IsWindow(m_hWnd));
	GetWindowText(rString);
	}


// protected:

BOOL COXTaskbarIcon::ShellNotify(DWORD dwMessage)
// In:	dwMessage, identifier of the message to send, same as Shell_NotifyIcon's dwMessage
//			parameter. This parameter can be one of these values:
//				NIM_ADD		Adds an icon to the taskbar notification area.
//				NIM_DELETE	Deletes an icon from the taskbar notification area.
//				NIM_MODIFY	Modifies an icon in the taskbar notification area. 
// Out:
// Return:	TRUE if successful or FALSE otherwise.
// Effect:	Sends a message to the system to add, modify, or delete a taskbar icon.
//		Similar to Shell_NotifyIcon(WORD dwMessage, PNOTIFYICONDATA pnid), except the
//		PNOTIFYICONDATA struct will be filled return values from member function calls
//		of GetID(), GetIcon(), and GetTooltipText().
	{
	ASSERT(::IsWindow(m_hWnd));

	NOTIFYICONDATA tnid; 
 
    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = m_hWnd;
    tnid.uID = GetID();
	if (dwMessage != NIM_DELETE)
		{
		tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnid.uCallbackMessage = WM_NOTIFY_TASKBAR_ICON;
		tnid.hIcon = GetIcon();
		GetTooltipText(tnid.szTip, sizeof(tnid.szTip));
		}

    return Shell_NotifyIcon(dwMessage, &tnid);
	}

LRESULT COXTaskbarIcon::OnNotifyTaskbarIcon(WPARAM wParam, LPARAM lParam)
// In: wParam, contains the identifier of a taskbar icon who sends this message
//			   should be the same value as m_nTaskbarIconID
//	   lParam, contains the mouse message (within WM_MOUSEFIRST to WM_MOUSELAST)
// Out:
// Return: (not used)
// Effect: this is to handle messages routed by system from the taskbar icon.
//		   will continue to route the msg to this taskbar icon's owner if any,
//		   or to itself if no owner.
	{
	ASSERT(::IsWindow(m_hWnd));
	CWnd* pOwnerWnd = GetOwner();

	if (pOwnerWnd)	
		{
		// ... if owner window is set, route notifications to it
		ASSERT(::IsWindow(pOwnerWnd->m_hWnd));
		pOwnerWnd->PostMessage(WM_NOTIFY_TASKBAR_ICON, wParam, lParam);
		}
	else			
		{			
		// ... if not, translate mouse messages for message handlers 
		//     in derived classes, and post it to myself
		PostMessage((UINT)lParam);
		}
	// ... don't care value
	return -1;  
	}

// private:

// end of OXTaskbarIcon.cpp