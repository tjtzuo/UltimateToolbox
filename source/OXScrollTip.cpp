// ==========================================================================
//				Class Implementation : COXScrollTip
// ==========================================================================

// Source file : OXScrollTip.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXScrollTip.h"

#include "UTBStrOp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXScrollTip, CWnd)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
BOOL COXScrollTip::m_bWindowClassCreated = FALSE;
CString COXScrollTip::m_sWindowClass;

// Data members -------------------------------------------------------------
// protected:
	// CPen m_framePen;
	// --- Therd Januari  pen that should be used to draw the window border (solid COLOR_WINDOWFRAME)

	// CBrush m_backgroundBrush;
	// --- The brush that should be used to fill the background (solid COLOR_INFOBK)

	// COLORREF m_textColor;
	// --- The color used to draw text (COLOR_INFOTEXT)
	// CFont m_font;
	// --- The fint used to draw text (same as of CToolTipCtrl)

	// int m_cxHScroll;
	// int m_cyVScroll;

	// BOOL m_bVisible;
	// --- Whether the window is currently visible

	// CString m_sText;
	// --- The current or new contents of the window
	// BOOL m_bTextChanged;
	// --- Whether the text has been changed in a call to Adjust, but not yet applied

	// CRect m_rect;
	// --- The current or new position and size
	// BOOL m_bRectChanged;
	// --- Whether the rect has been changed in a call to Adjust, but not yet applied

	// BOOL m_bFastBackgroundRepaint;
	// --- Whether WP_PAINTs should be send to the windows that reappear when the 
	//     scroll tip is moved
	//	   If TRUE WM_PAINT will be send, otherwise it will be (automatically) posted (by WIndows)

	// static BOOL m_bWindowClassCreated;
	// --- Whether an attempt has already been made to create a window class

	// static CString m_sWindowClass;
	// --- The The name of the window class

// private:
	
// Member functions ---------------------------------------------------------
// public:

BEGIN_MESSAGE_MAP(COXScrollTip, CWnd)
	//{{AFX_MSG_MAP(COXScrollTip)
	ON_WM_WININICHANGE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COXScrollTip::COXScrollTip()
	:
	m_textColor(0),
	m_cxHScroll(0),
	m_cyVScroll(0),
	m_bVisible(FALSE),
	m_sText(),
	m_bTextChanged(FALSE),
	m_rect(0, 0, 0, 0),
	m_bRectChanged(FALSE),
	m_bFastBackgroundRepaint(TRUE)
	{
	// Get the width of the scroll arrow of horizontal scrollbar and
	// the height of the scroll arrow of vertical scrollbar and
	m_cxHScroll = ::GetSystemMetrics(SM_CXHSCROLL);
	m_cyVScroll = ::GetSystemMetrics(SM_CYVSCROLL);

	ASSERT_VALID(this);
	}

int COXScrollTip::GetHorizontalArrowWidth() const
	{
	return m_cxHScroll;
	}

int COXScrollTip::GetVerticalArrowHeight() const
	{
	return m_cyVScroll;
	}

BOOL COXScrollTip::Create(CWnd* pParentWnd) 
	{
	// Create a new window class if necessary
	if (!m_bWindowClassCreated)
		{
		m_sWindowClass = AfxRegisterWndClass(CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW);
		m_bWindowClassCreated = TRUE;
		}

	ASSERT(m_bWindowClassCreated);
	if (m_sWindowClass.IsEmpty())
		{
		// Have tried to create a window class, but it failed
		TRACE0("COXScrollTip::Create : Could not create window class\n");
		return FALSE;
		}
	
	// Create the window itself (no caption, empty rect and invisible)
	// ... Must use CreateEx() because we want a pop-up window
	// ... We will paint the border frame ourselves
	if (!CWnd::CreateEx(0, m_sWindowClass, _T(""), WS_POPUP, 0, 0, 0, 0, 
		pParentWnd->GetSafeHwnd(), 0))
		{
		TRACE0("COXScrollTip::Create : Failed to create window\n");
		return FALSE;
		}

	ASSERT(!m_bVisible);

	// Get the needed colors now
	AdjustColorsFontsMetrics();

	return TRUE;
	}

void COXScrollTip::Show(BOOL bShow)
	{
	if (bShow)
		ShowWindow(SW_SHOWNA);
	else
		{
		m_rect.SetRectEmpty();
		ShowWindow(SW_HIDE);
		m_sText.Empty();
		}
	m_bVisible = bShow;
	}

void COXScrollTip::Adjust(CRect rect, LPCTSTR pszText)
	{
	if (m_rect != rect)
		{
		m_rect = rect;
		m_bRectChanged = TRUE;
		}

	if (m_sText != pszText)
		{
		m_sText = pszText;
		m_bTextChanged = TRUE;
		}

	if (m_bRectChanged || m_bTextChanged)
		Redraw();

	ASSERT(!m_bRectChanged);
	ASSERT(!m_bTextChanged);
	}

void COXScrollTip::EnableFastBackgroundRepaint(BOOL bEnable /* = TRUE */)
	{
	m_bFastBackgroundRepaint = bEnable;
	}

CSize COXScrollTip::ComputeSize(LPCTSTR pszText)
	{
	CSize size(0,0);
	CDC* pDC = GetDC();
	if (pDC != NULL)
		size = pDC->GetTextExtent(pszText, (int)_tcslen(pszText));
	else
		TRACE0("COXScrollTip::ComputeSize : Failed to get DC\n");
	ReleaseDC(pDC);
	return size;
	}

#ifdef _DEBUG
void COXScrollTip::AssertValid() const
	{
	CWnd::AssertValid();
	}

void COXScrollTip::Dump(CDumpContext& dc) const
	{
	CWnd::Dump(dc);
	}
#endif //_DEBUG

COXScrollTip::~COXScrollTip()
	{
	}

// protected:
// private:

// ==========================================================================


void COXScrollTip::OnWinIniChange(LPCTSTR lpszSection) 
	{
	CWnd::OnWinIniChange(lpszSection);
	
	// Colors and fonts may have changed, adjust them
	AdjustColorsFontsMetrics();
	}

void COXScrollTip::AdjustColorsFontsMetrics()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Adjust the colors, fonts etc. to the new WIndows settings
	{
	// Get the tooltip frame, background and text color
	if (m_framePen.m_hObject != NULL)
		// ... Destroy previous pen
		m_framePen.DeleteObject();
	if (m_backgroundBrush.m_hObject != NULL)
		// ... Destroy previous brush
		m_backgroundBrush.DeleteObject();
	VERIFY(m_framePen.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME)));
	VERIFY(m_backgroundBrush.CreateSolidBrush(::GetSysColor(COLOR_INFOBK)));
	m_textColor = ::GetSysColor(COLOR_INFOTEXT);

	// Get the tooltip font
	if ((HFONT)m_font != NULL)
		// ... Destroy previous font
		m_font.DeleteObject();

	// ... Although the tooltip font can be specified through control panel
	//     there seems to be no way to retrieve this information.
	//	   So we will crete a temporary tooltip to get its font
	CToolTipCtrl toolTip;

	// ... Tooltip control can be created with NULL parent
	//     We only need it for the font, so this will arm nobody
	if (toolTip.Create(NULL))
		{
		CFont* pFont = toolTip.GetFont();
		if (pFont != NULL)
			{
			LOGFONT logFont;
			::ZeroMemory(&logFont, sizeof(logFont));
			pFont->GetLogFont(&logFont);
			m_font.CreateFontIndirect(&logFont);
			}
		toolTip.DestroyWindow();
		}

	if ((HFONT)m_font == NULL)
		{
		TRACE0("COXScrollTip::AdjustColorsFontsMetrics : Could not establish correct font, using default");
		LOGFONT logFont;
		::ZeroMemory(&logFont, sizeof(logFont));
		logFont.lfHeight = -11;
		logFont.lfWeight = 400;
		UTBStr::tcscpy(logFont.lfFaceName, 14, _T("MS Sans Serif"));
		VERIFY(m_font.CreateFontIndirect(&logFont));
		}

	// Use the font in this control
	SetFont(&m_font);

	// The font may have changed, re-align the text within the control
	CString sText;
	GetWindowText(sText);
	SetWindowText(sText);

	// Get the width of the scroll arrow of horizontal scrollbar and
	// the height of the scroll arrow of vertical scrollbar and
	m_cxHScroll = ::GetSystemMetrics(SM_CXHSCROLL);
	m_cyVScroll = ::GetSystemMetrics(SM_CYVSCROLL);
	}


void COXScrollTip::OnPaint() 
	{
	CPaintDC dc(this); 
	
	CRect rect = m_rect;
	ScreenToClient(rect);

	// Use the correct color and font
	CPen* pOldPen = dc.SelectObject(&m_framePen);
	CBrush* pOldBrush = dc.SelectObject(&m_backgroundBrush);
    CFont* pOldFont = dc.SelectObject(&m_font);

	dc.SetBkMode(TRANSPARENT);
  	dc.SetTextColor(m_textColor);  

	// Draw the rectangle (border and fill) and text
	CString sText;
	GetWindowText(sText);
	dc.Rectangle(&rect);
	VERIFY(0 < dc.DrawText(sText, &rect, DT_CENTER));

	// Restore the dc context
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
    dc.SelectObject(pOldFont);
	}

void COXScrollTip::Redraw() 
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Redraws the window with the new rect and text contents
	{
	// First adjust the position
	if (m_bRectChanged)
		{
		// First repaint the area we will not use anymore
		CRect oldRect;
		GetWindowRect(oldRect);
		CRect divRect;
		divRect.SubtractRect(oldRect, m_rect);

		// Move scroll tip window
		SetWindowPos(&CWnd::wndTop, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), 
			SWP_NOACTIVATE);

		if (m_bFastBackgroundRepaint)
			RedrawBackground(divRect);
		}

	// Then adjust the text
	if (m_bTextChanged)
		SetWindowText(m_sText);

	// ... If the window is still invisible, show it now
	if (!m_bVisible)
		Show(TRUE);
	ASSERT(m_bVisible);

	// Redraw the window if position or text have changed
	if (m_bRectChanged || m_bTextChanged)
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW);

	m_bRectChanged = FALSE;
	m_bTextChanged = FALSE;
	}


void COXScrollTip::RedrawBackground(CRect rect)
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Updates the windows that are covered by the specified rect (screen coordinates)
	{
	// We give the windows that are under the top left and bottom right corner of 
	// the specified rect the opportunity the redraw themselves now.
	// Other areas will be repainted through a (posted) WM_PAINT, which will
	// be handled later.
	if (!rect.IsRectEmpty())
		{
		CWnd* pBackTopLeftWnd = CWnd::WindowFromPoint(rect.TopLeft());
		if (pBackTopLeftWnd != NULL)
			{
			pBackTopLeftWnd = pBackTopLeftWnd->GetTopLevelParent();
			ASSERT(pBackTopLeftWnd != NULL);
			pBackTopLeftWnd->RedrawWindow(&rect);
			}
		CWnd* pBackBottomRightWnd = CWnd::WindowFromPoint(rect.BottomRight());
		if ((pBackBottomRightWnd != NULL) && (pBackTopLeftWnd != pBackBottomRightWnd))
			{
			pBackBottomRightWnd = pBackBottomRightWnd->GetTopLevelParent();
			ASSERT(pBackBottomRightWnd != NULL);
			pBackBottomRightWnd->RedrawWindow(&rect);
			}
		}
	}
