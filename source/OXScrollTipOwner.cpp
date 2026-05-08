// ==========================================================================
//				Class Implementation : COXScrollTipOwner
// ==========================================================================

// Source file : OXScrollTipOwner.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXScrollTipOwner.h"
#ifndef __OXMFCIMPL_H__
#if _MFC_VER >= 0x0700
#if _MFC_VER >= 1400
#include <afxtempl.h>
#endif
#include <..\src\mfc\afximpl.h>
#else
#include <..\src\afximpl.h>
#endif
#define __OXMFCIMPL_H__
#endif

#include "UTB64Bit.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXScrollTipOwner, CObject)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
// ... All codes strings musr have length 2
static TCHAR szPageCode[] = _T("%p");
static TCHAR szPosInPageCode[] = _T("%l");
static TCHAR szAbsPosCode[] = _T("%a");

LPCTSTR COXScrollTipOwner::m_pszPageCode = szPageCode;
LPCTSTR COXScrollTipOwner::m_pszPosInPageCode = szPosInPageCode;
LPCTSTR COXScrollTipOwner::m_pszAbsPosCode = szAbsPosCode;


CMap<HWND, HWND, COXScrollTipOwner*, COXScrollTipOwner*> COXScrollTipOwner::m_allScrollTipOwners;

const int COXScrollTipOwner::m_nHorizontalGap = 15; // used for vertical scrollbar
const int COXScrollTipOwner::m_nVerticalGap = 10; // used for horizontal scrollbar

// Data members -------------------------------------------------------------
// protected:
// HWND m_hWnd;
// --- The hendle to the window that is subclassed.
//     This is a window with the styles WS_VSCROLL or WS_HSCROLL or
//	   the parent window of a scrollbar

// WNDPROC m_pfnSuper;
// --- The origianl window procedure before subclassing by this or
//     another COXScrollTipOwner object

// BOOL m_bHorizontal;
// --- Whether this is horizontal scroll bar (FALSE for a vertical one)

// HWND m_hScrollBar;
// --- Handle to the scrollbar itself (if m_hWnd does not have the
//     WS_VSCROLL or WS_HSCROLL styles), NULL otherwise

// COXScrollTip m_scrollTip;
// --- The scroll tip window for the scrollbar

// CString m_sMask;
// --- The text mask to be used by the scroll tip window

// BOOL m_bLeftTop;
// --- TRUE to align at the left hand side (vertical) or at the top (horizontal)
// --- FALSE to align at the right hand side (vertical) or at the bottom (horizontal)

// BOOL m_bPageAlign;
// --- Whether the scroll tip should aligned to the top of the current page

// COXScrollTipOwner* m_pPrevHandler;
// --- Pointer to the previous COXScrollTipOwner object that has handled
//     the windows message.  May be NULL
// COXScrollTipOwner* m_pNextHandler;
// --- Pointer to the next COXScrollTipOwner object that will handled
//     the windows message.  May be NULL

// static CMap<HWND, HWND, COXScrollTipOwner*, COXScrollTipOwner*> m_allScrollTipOwners;
// --- A map of all the windows that have been subclassed and that are associated with
//     one or more COXScrollTipOwner object.  The object that is in the map is the one at
//	   the top of the list.  this object will handle the windows messages first

// static const int m_nHorizontalGap;
// --- The gap in pixels that should be used between the scroll tip and the 
//     vertical scroll bar
// static const int m_nVerticalGap;
// --- The gap in pixels that should be used between the scroll tip and the 
//     horizontal scroll bar

// Note : All the COXScrollTipOwner objects that handle the same window
//         are organized in a double linked list.  they have all subclassed
//		   the same window.
//		  The top of the list is found in the map m_allScrollTipOwners.
//		  This object will handle the messages first.
//		  The next object is m_pNextHandler.  The next of that will be m_pNextHandler,
//		   and so on.
//		  When m_pNextHandler == NULL, we have arrived at the end of the list
//		   and now the original windows procedure will be used (m_pfnSuper).
//		  Additional handlers for the same window (COXScrollTipOwner objects)
//		   may be dynamically added or removed.  That's why a double linked list
//		   with a m_pPrevHandler pointer is handy.
//		  All m_pfnSuper members are the same for the same window.
//		  No other subclassing may be inserted in this list.
//		  So an MFC subclassed object may exist at the end of the list (m_pfnSuper)
//			but not in the middle.

//   L        A         B         C                 ORG
// +---+    +----+    +----+    +----+           +-------+
// |  ----->|   <-------  <-------   |           |       |
// +---+    |    |    |    |    |    |           | ^ ^ ^ |
// |   |    |   ------->  ------->   |           +-|-|-|-+
// +---+    +---|+    +--|-+    +--|-+             | | |
// |   |        |        |         +---------------+ | |
// +---+        |        +---------------------------+ |
// |   |        +--------------------------------------+

// L : The m_allScrollTipOwners map of which the value associated with 
//     a HWND pointe to the top of a list
// A, B, C : COXScrollTipOwner objects associated with the same window
// ORG : The original windows procedure

// private:

// Member functions ---------------------------------------------------------
// public:

COXScrollTipOwner::COXScrollTipOwner()
:
m_hWnd(NULL),
m_pfnSuper(NULL),
m_bHorizontal(FALSE),
m_hScrollBar(NULL),
m_sMask(_T("%a")),
m_bLeftTop(TRUE),
m_bPageAlign(FALSE),
m_pPrevHandler(NULL),
m_pNextHandler(NULL)
{
	ASSERT_VALID(this);
}


BOOL COXScrollTipOwner::InterceptHorizontalScroll(CWnd* pWnd, BOOL bForceStyle /* = FALSE */)
{
	return SubclassScrollTipOwner(pWnd, TRUE, bForceStyle);
}

BOOL COXScrollTipOwner::InterceptVerticalScroll(CWnd* pWnd, BOOL bForceStyle /* = FALSE */)
{
	return SubclassScrollTipOwner(pWnd, FALSE, bForceStyle);
}

void COXScrollTipOwner::RemoveScrollTipSupport()
{
	UnsubclassScrollTipOwner();
}

void COXScrollTipOwner::SetMask(LPCTSTR pszMask)
{
	m_sMask = pszMask;
}

CString COXScrollTipOwner::GetMask() const
{
	return m_sMask;
}

void COXScrollTipOwner::SetLeftTop(BOOL bLeftTop /* = TRUE */)
{
	m_bLeftTop = bLeftTop;
}

BOOL COXScrollTipOwner::GetLeftTop() const
{
	return m_bLeftTop;
}

void COXScrollTipOwner::SetPageAlign(BOOL bPageAlign /* = TRUE */)
{
	m_bPageAlign = bPageAlign;
}

BOOL COXScrollTipOwner::GetPageAlign() const
{
	return m_bPageAlign;
}

void COXScrollTipOwner::EnableFastBackgroundRepaint(BOOL bEnable /* = TRUE */)
{
	m_scrollTip.EnableFastBackgroundRepaint(bEnable);
}

#ifdef _DEBUG
void COXScrollTipOwner::AssertValid() const
{
	CObject::AssertValid();
	//  m_hWnd and m_pfnSuper must be both NULL or noth not NULL
	if (m_hWnd == NULL)
	{
		ASSERT(m_pfnSuper == NULL);
		ASSERT(m_hScrollBar == NULL);
		ASSERT(m_pPrevHandler == NULL);
		ASSERT(m_pNextHandler == NULL);
		ASSERT(m_scrollTip.m_hWnd == NULL);
	}
	else
		ASSERT(m_pfnSuper != NULL);
}

void COXScrollTipOwner::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

COXScrollTipOwner::~COXScrollTipOwner()
{
	ASSERT_VALID(this);

	// ... If still subclassed by this class, unsubclass
	UnsubclassScrollTipOwner();
}

// protected:
BOOL COXScrollTipOwner::SubclassScrollTipOwner(CWnd* pWnd, BOOL bHorizontal, BOOL bForceStyle)
// --- In  : pWnd : The window with the WS_HSCROLL or WS_VSCROLL style
//					or the scrollbar itself
//           bHorizontal : Whether the horizontal scrollbar should be used or not
//			 bForceStyle : Whether the window should be accepted as having the
//                         WS_HSCROLL or WS_VSCROLL  style even if it does not.
// --- Out : 
// --- Returns : Whhether it was successful or not
// --- Effect : This function subclasses the specified window or the
//				 parent of the scrollbar
//				It inserts this object into the double linked list
//				 and puts itself at the top
{
	ASSERT(pWnd != NULL);
	ASSERT_VALID(pWnd);

	// ... Should not be subclasses by this control yet
	ASSERT(m_hWnd == NULL);
	ASSERT(m_pfnSuper == NULL);
	ASSERT(m_pPrevHandler == NULL);
	ASSERT(m_pNextHandler == NULL);

	if (pWnd->GetSafeHwnd() != NULL)
	{
		// Check whether pWnd is a scrollbar itself, or is window with 
		// build in scroll bars (e.g. edit control)
		DWORD nStyle = pWnd->GetStyle();
		if (bForceStyle || ((nStyle & WS_HSCROLL) == WS_HSCROLL) || 
			((nStyle & WS_VSCROLL) == WS_VSCROLL))
		{
			// Scrollable window
			m_hWnd = pWnd->GetSafeHwnd();
			CScrollBar* pScrollBar=
				pWnd->GetScrollBarCtrl(bHorizontal ? SB_HORZ : SB_VERT);
			if(pScrollBar!=NULL)
				m_hScrollBar=pScrollBar->GetSafeHwnd();
			else
				m_hScrollBar=NULL;
		}
		else
		{
			// Scrollbar (should be child window)
			ASSERT((nStyle & WS_CHILD) == WS_CHILD);
			m_hScrollBar = pWnd->GetSafeHwnd();
			m_hWnd = ::GetParent(m_hScrollBar);
			ASSERT(::IsWindow(m_hWnd));
			// ... The specified orientation should match the style
			ASSERT(bHorizontal == !((nStyle & SBS_VERT) == SBS_VERT));
		}

		// ... Subclass window (Windows way not MFC : because may already be subclassed by MFC)
		COXScrollTipOwner* pPrevScrollTipOwner = NULL;
		m_pfnSuper = (WNDPROC)::GetWindowLongPtr(m_hWnd, GWL_WNDPROC);
		if (m_pfnSuper != GlobalScrollTipOwnerProc)
		{
			::SetWindowLongPtr(m_hWnd, GWL_WNDPROC, (LONG_PTR)GlobalScrollTipOwnerProc);
			//... This window should not be using GlobalScrollTipProc yet
			VERIFY(!m_allScrollTipOwners.Lookup(m_hWnd, pPrevScrollTipOwner));
		}
		else
		{
			// Has already been subclassed with window procedure GlobalScrollTipProc()
			// We will put us at the top of the chain
			VERIFY(m_allScrollTipOwners.Lookup(m_hWnd, pPrevScrollTipOwner));
			ASSERT(pPrevScrollTipOwner->m_pPrevHandler == NULL);

			pPrevScrollTipOwner->m_pPrevHandler = this;
			m_pNextHandler = pPrevScrollTipOwner;

			// Get the actual next window procedure that is not ours
			m_pfnSuper = m_pNextHandler->m_pfnSuper;
			ASSERT(m_pfnSuper != GlobalScrollTipOwnerProc);
		}

		// ... Store at the top of the global map
		m_allScrollTipOwners.SetAt(m_hWnd, this);

		// ... Store whether we are interested in horizontal or vertical scroll bar
		m_bHorizontal = bHorizontal;

		// Create the scroll tip window (still invisible)
		CreateScrollTip();
	}

	ASSERT_VALID(this);
	return (m_hWnd != NULL);;
}

void COXScrollTipOwner::UnsubclassScrollTipOwner()
// --- In  : 
// --- Out : 
// --- Returns : 
// --- Effect : This function unsubclasses the window 
//				It removes this object from the double linked list
//				When it is the last in the list it restores the original
//				windows procedure
{
	ASSERT_VALID(this);

	if (m_hWnd != NULL)
	{
		// Remove us from the chain objects
		if (m_pNextHandler != NULL)
			m_pNextHandler->m_pPrevHandler = m_pPrevHandler;
		if (m_pPrevHandler != NULL)
			m_pPrevHandler->m_pNextHandler = m_pNextHandler;

		// If we were the top of the global map, put in second
		COXScrollTipOwner* pTopScrollTipOwner = NULL;
		VERIFY(m_allScrollTipOwners.Lookup(m_hWnd, pTopScrollTipOwner));
		if (pTopScrollTipOwner == this)
		{
			ASSERT(m_pPrevHandler == NULL);
			m_allScrollTipOwners.SetAt(m_hWnd, m_pNextHandler);
		}

		// If we were the last one in the chain, put back original window procedure
		if ((m_pNextHandler == NULL) && (m_pPrevHandler == NULL))
		{
			// GlobalScrollTipProc is not used anymore : set WNDPROC back to original value
			ASSERT(m_pfnSuper != GlobalScrollTipOwnerProc);
			::SetWindowLongPtr(m_hWnd, GWL_WNDPROC, (LONG_PTR)m_pfnSuper);
			// ... remove use from global map
			m_allScrollTipOwners.RemoveKey(m_hWnd);
		}

		m_hWnd = NULL;
		m_hScrollBar = NULL;
		m_pfnSuper = NULL;
		m_pNextHandler = NULL;
		m_pPrevHandler = NULL;
	}

	// Destroy the scroll tip window if it still exists
	DestroyScrollTip();

	ASSERT_VALID(this);
}

LRESULT CALLBACK COXScrollTipOwner::GlobalScrollTipOwnerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// --- In  : hWnd : 
//			 uMsg : 
//			 wParam : 
//			 lParam :
// --- Out : 
// --- Returns : The result of the message
// --- Effect : This is the global windows procedure of all the COXScrollTipOwner
//              objects that have subclasses a window
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	COXScrollTipOwner* pScrollTipOwner = NULL;

	VERIFY(m_allScrollTipOwners.Lookup(hWnd, pScrollTipOwner));
	ASSERT_VALID(pScrollTipOwner);
	return pScrollTipOwner->ScrollTipOwnerProc(hWnd, uMsg, wParam, lParam);
}

LRESULT COXScrollTipOwner::ScrollTipOwnerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// --- In  : hWnd : 
//			 uMsg : 
//			 wParam : 
//			 lParam :
// --- Out : 
// --- Returns : The result of the message
// --- Effect : This is the member function called by the windows procedure of the 
//				COXScrollTipOwner object
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT_VALID(this);
	ASSERT(hWnd == m_hWnd);

	BOOL bHandled = FALSE;

	// Let the correct function handle the message
	switch (uMsg)
	{
	case WM_HSCROLL:
		{
			UINT nSBCode = LOWORD(wParam);
			int nPos = (signed short)HIWORD(wParam);
			HWND hScrollBar = (HWND)lParam;
			bHandled = OnHScroll(nSBCode, nPos, hScrollBar);
			break;
		}
	case WM_VSCROLL:
		{
			UINT nSBCode = LOWORD(wParam);
			int nPos = (signed short)HIWORD(wParam);
			HWND hScrollBar = (HWND)lParam;
			bHandled = OnVScroll(nSBCode, nPos, hScrollBar);
			break;
		}
	default:
		// Just fall through
		break;
	}

	if (bHandled)
		// ... Return that we handled the message
		return 0;
	else
	{
		// ... Let the next window procedure handle the other messages
		if (m_pNextHandler != NULL)
		{
			// Let the next in the chain handle it
			ASSERT_VALID(m_pNextHandler);
			return m_pNextHandler->ScrollTipOwnerProc(hWnd, uMsg, wParam, lParam);
		}
		else
		{
			// Let the original message procedure handle it
			ASSERT(m_pfnSuper != NULL);
			ASSERT(m_pfnSuper != GlobalScrollTipOwnerProc);
			return CallWindowProc(m_pfnSuper, hWnd, uMsg, wParam, lParam);
		}
	}
}

BOOL COXScrollTipOwner::OnHScroll(UINT nSBCode, int /* nPos */, HWND hScrollBar)
// --- In  : nSBCode : the scrollbar code (see CWnd::OnHScroll)
//			 nPos : The current position of the thumb
//			 hScrollBar : The window hanlde of the scroll bar (may be NULL)
// --- Out : 
// --- Returns : Whether this message was handled
// --- Effect : Passes the relevant messages to the scroll tip
{
	if (!m_bHorizontal || (hScrollBar != m_hScrollBar))
		// ... We are not interested in vertical scrolling
		//     or in scrolling of another scrollbar
		return FALSE;

	CString sCode;
	switch (nSBCode)
	{
	case SB_ENDSCROLL:
		// Hide the scroll tip if the scrolling is ended
		m_scrollTip.Show(FALSE);
		break;
	case SB_THUMBTRACK:
		// Adjust the scroll tip text and position during scrolling
		AdjustScrollTip();
		break;
	default:
		// ... Do nothing by default
		break;
	}

	// ... Give the base window procedure a change to handle it as well
	return FALSE;
}

BOOL COXScrollTipOwner::OnVScroll(UINT nSBCode, int /* nPos */, HWND hScrollBar)
// --- In  : nSBCode : the scrollbar code (see CWnd::OnVScroll)
//			 nPos : The current position of the thumb
//			 hScrollBar : The window hanlde of the scroll bar (may be NULL)
// --- Out : 
// --- Returns : Whether this message was handled
// --- Effect : Passes the relevant messages to the scroll tip
{
	if (m_bHorizontal || (hScrollBar != m_hScrollBar))
		// ... We are not interested in horizontal scrolling
		//     or in scrolling of another scrollbar
		return FALSE;

	CString sCode;
	switch (nSBCode)
	{
	case SB_ENDSCROLL:
		// Hide the scroll tip if the scrolling is ended
		m_scrollTip.Show(FALSE);
		break;
	case SB_THUMBTRACK:
		// Adjust the scroll tip text and position during scrolling
		AdjustScrollTip();
		break;
	default:
		// ... Do nothing by default
		break;
	}

	// ... Give the base window procedure a change to handle it as well
	return FALSE;
}

BOOL COXScrollTipOwner::CreateScrollTip()
// --- In  : 
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : Creates the scroll tip window associated with this object
{
	ASSERT(::IsWindow(m_hWnd));

	// Create the scrolltip window 
	ASSERT(m_scrollTip.m_hWnd == NULL);
	if (!m_scrollTip.Create(CWnd::FromHandle(m_hWnd)))
	{
		TRACE0("COXScrollTipOwner::CreateScrollTip : Failed to create scroll tip control\n");
		return FALSE;
	}
	ASSERT(::IsWindow(m_scrollTip.m_hWnd));
	return TRUE;
}

void COXScrollTipOwner::DestroyScrollTip()
// --- In  : 
// --- Out : 
// --- Returns : 
// --- Effect : Destroys the scroll tip window associated with this object
{
	// Destroy the scroll tip window if it still exists
	if (m_scrollTip.m_hWnd != NULL)
		m_scrollTip.DestroyWindow();
}

BOOL COXScrollTipOwner::AdjustScrollTip()
// --- In  : 
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : Adjusts the text and position of the scroll tip
{
	CString sText;
	int nPage = 0;
	int nPagePos = 0;
	int nAbsPos = 0;
	int nPageSize = 0;

	// Get the current position
	if (!GetScrollInfo(nPage, nPagePos, nAbsPos, nPageSize))
		return FALSE;

	// Build a nice text and set it
	sText = FillMask(nPage + 1, nPagePos + 1, nAbsPos + 1);
	CSize newSize = m_scrollTip.ComputeSize(sText);

	// Change the position (uses the current size)
	CPoint newPos = ComputeScrollTipPosition(nPage, nPagePos, nAbsPos, nPageSize, newSize);
	m_scrollTip.Adjust(CRect(newPos, newSize), sText);

	return TRUE;
}

BOOL COXScrollTipOwner::GetScrollInfo(int& nPage, int& nPagePos, int& nAbsPos, int& nPageSize)
// --- In  : 
// --- Out : nPage : The current page
//			 nPagePos : The current position within the page
//			 nAbsPos : The current absolute position
//			 nPageSize : The size of one page
// --- Returns : Whether it succeeded or not
// --- Effect : Get the current values of the scrollbar
//				All numbers are zero-based
{
	// Initialize return values
	nPage = 0;
	nPagePos = 0;
	nAbsPos = 0;

	// Get the scroll parameters
	SCROLLINFO scrollInfo;
	::ZeroMemory(&scrollInfo, sizeof(scrollInfo));
	scrollInfo.cbSize = sizeof(scrollInfo);
	scrollInfo.fMask = SIF_ALL;
	HWND hScrollWnd = m_hScrollBar == NULL ? m_hWnd : m_hScrollBar;
	int nBar = (m_hScrollBar == NULL ? (m_bHorizontal ? SB_HORZ : SB_VERT) : SB_CTL);
	if (!::GetScrollInfo(hScrollWnd, nBar, &scrollInfo))
		// ... Failed to get scroll info
		return FALSE;

	// ... Avoid division by zero
	if (scrollInfo.nPage != 0)
	{
		nPage = scrollInfo.nTrackPos / scrollInfo.nPage;
		nPagePos = scrollInfo.nTrackPos % scrollInfo.nPage;
	}
	nAbsPos = scrollInfo.nTrackPos;
	nPageSize = scrollInfo.nPage;
	return TRUE;
}

CString COXScrollTipOwner::FillMask(int nPage, int nPagePos, int nAbsPos)
// --- In  : nPage : The current page
//			 nPagePos : The current position within the page
//			 nAbsPos : The current absolute position
// --- Out : 
// --- Returns : The text for the scroll tip
// --- Effect : This function replaces certain parts (e.g. "%p" by its current value
{
	CString sResult = m_sMask;
	// Substiture %p, %l and %a by the page number, the position within the page and 
	// the absolute position

	// Codes must all have length 2
	ASSERT(_tcslen(m_pszPageCode) == 2);
	ASSERT(_tcslen(m_pszPosInPageCode) == 2);
	ASSERT(_tcslen(m_pszAbsPosCode) == 2);

	// Build numeric strings
	CString sPage;
	CString sPagePos;
	CString sAbsPos;
	sPage.Format(_T("%i"), nPage);
	sPagePos.Format(_T("%i"), nPagePos);
	sAbsPos.Format(_T("%i"), nAbsPos);

	// Substitute page code
	int nPos = sResult.Find(m_pszPageCode);
	while (0 <= nPos)
	{
		sResult = sResult.Left(nPos) + sPage + sResult.Mid(nPos + 2);
		nPos = sResult.Find(m_pszPageCode);
	}

	// Substitute pos in pagecode
	nPos = sResult.Find(m_pszPosInPageCode);
	while (0 <= nPos)
	{
		sResult = sResult.Left(nPos) + sPagePos + sResult.Mid(nPos + 2);
		nPos = sResult.Find(m_pszPosInPageCode);
	}

	// Substitute absolute pos code
	nPos = sResult.Find(m_pszAbsPosCode);
	while (0 <= nPos)
	{
		sResult = sResult.Left(nPos) + sAbsPos + sResult.Mid(nPos + 2);
		nPos = sResult.Find(m_pszAbsPosCode);
	}

	return sResult;
}

CPoint COXScrollTipOwner::ComputeScrollTipPosition(int /* nPage */, 
												   int /* nPagePos */, int nAbsPos, int nPageSize, CSize newSize)
												   // --- In  : nPage : The current page
												   //			 nPagePos : The current position within the page
												   //			 nAbsPos : The current absolute position
												   //			 nPageSize : The size of one page
												   //			 nNewSize : The new window size of the scroll tip
												   // --- Out : 
												   // --- Returns : The new top left position of the scroll tip (screen coordinates)
												   // --- Effect : This function computes the new position of the scroll tip window
{
	CPoint pt(0,0);

	// Get an approximate absolute position of the scroll range.
	// This is the range between the two arrows (not including).
	// ... Size of borders and splitters is not taken into account
	CRect scrollRangeRect(0, 0, 0, 0);
	if (m_hScrollBar == NULL)
	{
		// ... Get the window and client rect from the owner window (both in screen coordinates)
		CRect windowRect;
		CRect clientRect;
		::GetWindowRect(m_hWnd, windowRect);
		::GetClientRect(m_hWnd, clientRect);
		::ClientToScreen(m_hWnd, &clientRect.TopLeft());
		::ClientToScreen(m_hWnd, &clientRect.BottomRight());

		if (m_bHorizontal)
			scrollRangeRect = CRect(clientRect.left, clientRect.bottom, clientRect.right, windowRect.bottom);
		else
			scrollRangeRect = CRect(clientRect.right, clientRect.top, windowRect.right, clientRect.bottom);
	}
	else
	{
		::GetWindowRect(m_hScrollBar, scrollRangeRect);
	}

	// Adjust for the size of the arrows
	if (m_bHorizontal)
	{
		scrollRangeRect.left += m_scrollTip.GetHorizontalArrowWidth();
		scrollRangeRect.right -= m_scrollTip.GetHorizontalArrowWidth();
	}
	else
	{
		scrollRangeRect.top += m_scrollTip.GetVerticalArrowHeight();
		scrollRangeRect.bottom -= m_scrollTip.GetVerticalArrowHeight();
	}

	// Align to the first line of the page if necessary
	if (m_bPageAlign && (nPageSize != 0))
		nAbsPos = (nAbsPos / nPageSize) * nPageSize;

	// Compute the main position (horizontal : x, vertical : y)

	// ... Get the scroll range
	int nMinRange = 0;
	int nMaxRange = 0;
	HWND hScrollWnd = m_hScrollBar == NULL ? m_hWnd : m_hScrollBar;
	int nBar = (m_hScrollBar == NULL ? (m_bHorizontal ? SB_HORZ : SB_VERT) : SB_CTL);
	::GetScrollRange(hScrollWnd, nBar, &nMinRange, &nMaxRange);

	// The next part has been remarked because we ALWAYS show the current position of the 
	// top of the scrolling thumb.

	// ... Center the middle of the scroll tip window around the requested position
	if (nMinRange != nMaxRange)
	{
		if (m_bHorizontal)
			pt.x = scrollRangeRect.left + (scrollRangeRect.Width() * (nAbsPos - nMinRange)) / (nMaxRange - nMinRange) 
			- newSize.cx / 2;
		else
			pt.y = scrollRangeRect.top + (scrollRangeRect.Height() * (nAbsPos - nMinRange)) / (nMaxRange - nMinRange)
			- newSize.cy / 2;
	}
	else
	{
		if (m_bHorizontal)
			pt.x = scrollRangeRect.left;
		else
			pt.y = scrollRangeRect.top;
	}

	// Compute the other position (horizontal : y, vertical : x)
	if (m_bHorizontal)
	{
		if (m_bLeftTop)
			pt.y = scrollRangeRect.top -  newSize.cy - m_nVerticalGap;
		else
			pt.y = scrollRangeRect.bottom + m_nVerticalGap;
	}
	else
	{
		if (m_bLeftTop)
			pt.x = scrollRangeRect.left - newSize.cx - m_nHorizontalGap;
		else
			pt.x = scrollRangeRect.right + m_nHorizontalGap;
	}

	return pt;
}

// private:

// ==========================================================================
