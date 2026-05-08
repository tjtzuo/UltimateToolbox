// ===================================================================================
// 					Class Implementation : COXRectTracker
// ===================================================================================

// Header file : OXRectTracker.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
// Some portions Copyright (C)1994-5	Micro Focus Inc, 2465 East Bayshore Rd, Palo Alto, CA 94303.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXRectTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXRectTracker::COXRectTracker()
	{
	m_LimitRect.SetRectEmpty();
	}

BOOL COXRectTracker::TrackFromHitTest(int nHitTest, CWnd* pWnd, CPoint point,
									   CWnd* pWndClipTo, BOOL bAllowInvert)
	{
	m_OrigRect = m_rect;		// save original rectangle
	m_bAllowInvert = bAllowInvert;
	int nHandle;
	switch (nHitTest)
		{
		case HTLEFT:
			nHandle = hitLeft;
			break;
		case HTRIGHT:
			nHandle = hitRight;
			break;
		case HTTOP:         
			nHandle = hitTop;
			break;
		case HTTOPLEFT:    
			nHandle = hitTopLeft;
			break;
		case HTTOPRIGHT:    
			nHandle = hitTopRight;
			break;
		case HTBOTTOM:      
			nHandle = hitBottom;
			break;
		case HTBOTTOMLEFT:  
			nHandle = hitBottomLeft;
			break;
		case HTBOTTOMRIGHT: 
			nHandle = hitBottomRight;
			break;
		default:
			nHandle = hitMiddle;		// default is move
			break;
		}
	
	return CRectTracker::TrackHandle(nHandle, pWnd, point, pWndClipTo);
	}


void COXRectTracker::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
	{
	// first, normalize the rectangle for drawing
	CRect rect = *lpRect;
	rect.NormalizeRect();
	
	// convert to client coordinates
	if (pWndClipTo != NULL)
		{
		pWnd->ClientToScreen(&rect);
		pWndClipTo->ScreenToClient(&rect);
		}

	CSize size(0, 0);
	if (!m_bFinalErase)
		{
		size.cx = 2;
		size.cy = 2;
		}
	// and draw it
	if (m_bFinalErase || !m_bErase)
		pDC->DrawDragRect(rect, size, m_rectLast, m_sizeLast);

	m_rectLast = rect;
	m_sizeLast = size; 
	}


void COXRectTracker::AdjustRect(int nHandle, LPRECT lpRect)
	{
	// clips to limiting rectangle...
	if (!m_LimitRect.IsRectNull())
		{
		if (nHandle == hitMiddle)  // if moving then have to ensure size is maintained...
			{
			CSize size = m_OrigRect.Size();
			lpRect->left = __max(m_LimitRect.left , __min(m_LimitRect.right , lpRect->left));
            lpRect->top =  __max(m_LimitRect.top  , __min(m_LimitRect.bottom - 10, lpRect->top ));
            lpRect->right  = lpRect->left + size.cx;
            lpRect->bottom = lpRect->top  + size.cy;
			}
		else
			{		
			CRect iRect;
			iRect.IntersectRect(m_LimitRect, lpRect);
			::CopyRect(lpRect, iRect);		
			}
		}
	
	// enforces minimum width, etc
	CRectTracker::AdjustRect(nHandle, lpRect);
	
	if (m_nStyle & RectTracker_OnlyMoveHorz)
		{
		lpRect->top = m_OrigRect.top;
		lpRect->bottom = m_OrigRect.bottom;
		}
	
	if (m_nStyle & RectTracker_OnlyMoveVert)
		{
		lpRect->left = m_OrigRect.left;
		lpRect->right = m_OrigRect.right;
		}
	
	}


COXRectTracker::~COXRectTracker()
	{
	}


void COXDragRectTracker::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
	{
	COXRectTracker::DrawTrackerRect(lpRect, pWndClipTo, pDC, pWnd);
	}

