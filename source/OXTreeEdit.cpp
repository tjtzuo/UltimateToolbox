// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// OXTreeEdit.cpp : implementation file
//
// Version: 9.3


#include "stdafx.h"
#include "OXTreeEdit.h"
#include "UTB64Bit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COXTreeEdit

COXTreeEdit::COXTreeEdit()
{
	m_bKeepColumnSize = FALSE;
	m_bKeepPos = FALSE;
	m_bHasBorder = TRUE;
	m_nDeltaX = 14;
	m_nDeltaY = 6;
}

COXTreeEdit::~COXTreeEdit()
{
}


BEGIN_MESSAGE_MAP(COXTreeEdit, CEdit)
	//{{AFX_MSG_MAP(COXTreeEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXTreeEdit message handlers

void COXTreeEdit::KeepPos(CPoint pt,DWORD dwAlign)
{
	m_bKeepPos = TRUE;
	m_pos = pt;
	m_dwAlign = dwAlign;
	if(::IsWindow(m_hWnd))
	{
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|dwAlign;
		::SetWindowLongPtr(m_hWnd,GWL_STYLE,dwStyle);
	}
}

void COXTreeEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{

	CEdit::OnWindowPosChanging(lpwndpos);

	if (m_bHasBorder)
	{
		CString sWndText;
		GetWindowText(sWndText);
		int nTextLen = GetWindowTextLength();
		if(!nTextLen)
		{
			//there are no text in edit window
			// use sample text to measure
			sWndText = _T("W");
		}
		CDC *pDC = GetDC();
		CFont *pOldFont = pDC->SelectObject(GetFont());
		CSize sz = pDC->GetTextExtent(sWndText);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		sz.cx += m_nDeltaX;
		sz.cy += m_nDeltaY;
		lpwndpos->cx = sz.cx;
		lpwndpos->cy = sz.cy;
	}
	else
		lpwndpos->y -= 1;


	if (m_bKeepPos)
		switch(m_dwAlign)
		{
		case ES_LEFT:
			if (m_bHasBorder)
			{
				lpwndpos->x = m_pos.x;
				lpwndpos->y = m_pos.y;
			}
			else
				lpwndpos->x = m_pos.x - 2;
			break;
		case ES_RIGHT:
			lpwndpos->x = m_pos.x - lpwndpos->cx;
			if (m_bHasBorder)
				lpwndpos->y = m_pos.y;
			break;
		case ES_CENTER:
			lpwndpos->x = m_pos.x - lpwndpos->cx/2;
			if (m_bHasBorder)
				lpwndpos->y = m_pos.y;
			break;
		default:
			// unknown style found!
			ASSERT(FALSE);
			break;
		}

	if (m_bKeepColumnSize)
	{
		if (m_bHasBorder)
			lpwndpos->cx = m_szBounds.cx - lpwndpos->x;
		else
			lpwndpos->cx = m_szBounds.cx;
	}
	else if (m_bKeepBounds)
	{
		if (m_bHasBorder)
		{
			if(lpwndpos->x + lpwndpos->cx > m_szBounds.cx)
				lpwndpos->cx = m_szBounds.cx - lpwndpos->x;
		}
		else
		{
			if (lpwndpos->cx < m_szBounds.cx)
				lpwndpos->cx = m_szBounds.cx;
		}
	}
}

void COXTreeEdit::Init()
{
	m_bKeepPos = FALSE;
	m_bKeepBounds = FALSE;
	m_bKeepColumnSize = FALSE;
	m_dwAlign = 0;
}

void COXTreeEdit::KeepBounds(CSize sz)
{
	m_bKeepBounds = TRUE;
	m_bKeepColumnSize = FALSE;
	m_szBounds = sz;
}

void COXTreeEdit::KeepColumnSize(CSize sz)
{
	m_bKeepBounds = FALSE;
	m_bKeepColumnSize = TRUE;
	m_szBounds = sz;
}

void COXTreeEdit::OnPaint() 
{
	if (!m_bHasBorder)
	{
		CPaintDC dc(this); // device context for painting
		
		// Do not call CEdit::OnPaint() for painting messages
		CString strText;
		GetWindowText(strText);
		CFont* pOldFont = dc.SelectObject(GetFont());
		dc.TextOut(5, 2, strText);
		dc.SelectObject(pOldFont);
	}
	else
		Default();
}


void COXTreeEdit::SetBorder(BOOL bHasBorder)
{
	m_bHasBorder = bHasBorder;
}
