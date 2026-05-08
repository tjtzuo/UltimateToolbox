// ==========================================================================
//				Class Implementation : COXGridHeader
// ==========================================================================

// Source file : OXGridHdr.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windowsx.h>
#include "OXGridHdr.h"

#pragma warning (disable : 4102)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#pragma warning (disable : 4102)

IMPLEMENT_DYNAMIC(COXGridHeader, CHeaderCtrl)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
// Data members -------------------------------------------------------------
// protected:

// private:
HHOOK COXGridHeader::m_hMouseHook = NULL;
HWND COXGridHeader::m_hwndPrevMouseMoveWnd = NULL;
// Member functions ---------------------------------------------------------
// public:

COXGridHeader::COXGridHeader()
	: m_bResizing(TRUE), m_nSortCol(-1), m_nSortOrder(0)
	{
	ASSERT_VALID(this);
	}


#ifdef _DEBUG
void COXGridHeader::AssertValid() const
	{
	CHeaderCtrl::AssertValid();
	}

void COXGridHeader::Dump(CDumpContext& dc) const
	{
	CHeaderCtrl::Dump(dc);
	}
#endif //_DEBUG

COXGridHeader::~COXGridHeader()
	{
	}

BEGIN_MESSAGE_MAP(COXGridHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(COXGridHeader)
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKW, OnHdrCtrlNotify)
	ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKA, OnHdrCtrlNotify)
	ON_NOTIFY_REFLECT_EX(HDN_DIVIDERDBLCLICKW, OnHdrCtrlNotify)
	ON_NOTIFY_REFLECT_EX(HDN_DIVIDERDBLCLICKA, OnHdrCtrlNotify)
END_MESSAGE_MAP()

BOOL COXGridHeader::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
	if (!m_bResizing)
		{
		DWORD dwMessagePos=::GetMessagePos();
		CPoint point(GET_X_LPARAM(dwMessagePos),GET_Y_LPARAM(dwMessagePos));

		// point allways in screen coordinates, but HDM_HITTEST needs client coordinates
		ScreenToClient(&point);

		// query the header for the area where it is over for the moment
		HD_HITTESTINFO hdhti;
		hdhti.pt = point;
		SendMessage(HDM_HITTEST,(WPARAM)0,(LPARAM) (HD_HITTESTINFO FAR *)&hdhti);
		
		// If the mouse is over a divider line, halt processing.  This way a standard cursor
		// will be shown.
		if ((hdhti.flags & HHT_ONDIVIDER) == HHT_ONDIVIDER)
			return TRUE;
		}

	return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
	}

BOOL COXGridHeader::OnHdrCtrlNotify(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	*pResult = 0;

	switch (pNMHDR->code)
		{
		case HDN_BEGINTRACKW:
		case HDN_BEGINTRACKA:
		case HDN_DIVIDERDBLCLICKW:
		case HDN_DIVIDERDBLCLICKA:
			*pResult = TRUE & !m_bResizing;
			return TRUE & !m_bResizing;
		}

	return FALSE;
	}

// protected:

void COXGridHeader::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if (m_UxTheme.IsUxThemeLoaded())
	{
		HTHEME hTheme = m_UxTheme.GetWindowTheme(m_hWnd);
		if (hTheme != NULL)
		{

			// Draw the item background
			int iState = HIS_NORMAL;
			if (lpDrawItemStruct->itemState == ODS_SELECTED)
				iState = HIS_PRESSED;
			else
			{
				POINT ptCursor;
				::GetCursorPos(&ptCursor);
				ScreenToClient(&ptCursor);
				CRect rectItem(lpDrawItemStruct->rcItem);
				if (rectItem.PtInRect(ptCursor))
					iState = HIS_HOT;
			}
			m_UxTheme.DrawThemeBackground(hTheme,
				lpDrawItemStruct->hDC,
				HP_HEADERITEM,
				iState,
				&lpDrawItemStruct->rcItem,
				NULL);

			BOOL bDrawArrow;
			if (m_nSortOrder!=0 && lpDrawItemStruct->itemID == (UINT)m_nSortCol)
				bDrawArrow = TRUE;
			else
				bDrawArrow = FALSE;


			// Get the column text and format	
			TCHAR buf[256];	
			HD_ITEM hditem;	
			hditem.mask = HDI_TEXT | HDI_FORMAT;	
			hditem.pszText = buf;
			hditem.cchTextMax = 255;	
			GetItem( lpDrawItemStruct->itemID, &hditem );

			RECT rectText = lpDrawItemStruct->rcItem;
			rectText.left += 9;
			rectText.right -= 9;

			// Determine format for drawing column label
			UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | 
				DT_VCENTER | DT_END_ELLIPSIS ;
			UINT uArrowFormat = DT_SINGLELINE | DT_VCENTER;
			if( hditem.fmt & HDF_CENTER)
			{
				uFormat |= DT_CENTER;
				uArrowFormat |= DT_RIGHT;
				if (bDrawArrow)
					rectText.right -= 12;
			}
			else if( hditem.fmt & HDF_RIGHT)
			{
				uFormat |= DT_RIGHT;
				uArrowFormat |= DT_LEFT;
				if (bDrawArrow)
					rectText.left += 12;
			}
			else
			{
				uFormat |= DT_LEFT;
				uArrowFormat |= DT_RIGHT;
				if (bDrawArrow)
					rectText.right -= 12;
			}

			m_UxTheme.DrawThemeText(hTheme,
				lpDrawItemStruct->hDC,
				HP_HEADERITEM,
				HIS_NORMAL,
				buf,
				-1,
				uFormat,
				0,
				&rectText);

			// Draw the Sort arrow	
			if (bDrawArrow)	
			{
				CDC dc;
				dc.Attach(lpDrawItemStruct->hDC);
				int nSavedDC = dc.SaveDC();
				CFont fontMarlett;
				fontMarlett.CreatePointFont(120, _T("Marlett"));
				dc.SelectObject(&fontMarlett);
				dc.SetBkMode(TRANSPARENT);
				dc.SetTextColor(RGB(128, 128, 128));

				CRect rectArrow = lpDrawItemStruct->rcItem;
				rectArrow.DeflateRect(5, 0, 5, 0);

				if (m_nSortOrder == 1)
					dc.DrawText(_T("5"), -1, &rectArrow, uArrowFormat);
				else
					dc.DrawText(_T("6"), -1, &rectArrow, uArrowFormat);

				// Restore dc	
				dc.RestoreDC(nSavedDC);
				// Detach the dc before returning	
				dc.Detach();
			}
			return;
		}
	}

	CDC dc;
	dc.Attach( lpDrawItemStruct->hDC );	
	// Get the column rect
	CRect rcLabel( lpDrawItemStruct->rcItem );	
	// Save DC
	int nSavedDC = dc.SaveDC();
	// Set clipping region to limit drawing within column	
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcLabel );	
	dc.SelectObject( &rgn );
	rgn.DeleteObject(); 
	// Draw the background
	CBrush brush(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(rcLabel,&brush);	
	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1 ).cx*2;
	// Get the column text and format	
	TCHAR buf[256];	
	HD_ITEM hditem;	
	hditem.mask = HDI_TEXT | HDI_FORMAT;	
	hditem.pszText = buf;
	hditem.cchTextMax = 255;	
	GetItem( lpDrawItemStruct->itemID, &hditem );
	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | 
		DT_VCENTER | DT_END_ELLIPSIS ;	
	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;	
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;
	// Adjust the rect if the mouse button is pressed on it
	if( lpDrawItemStruct->itemState == ODS_SELECTED )	
	{		
		rcLabel.left++;
		rcLabel.top += 2;		
		rcLabel.right++;	
	}
	// Adjust the rect further if Sort arrow is to be displayed
	if( lpDrawItemStruct->itemID == (UINT)m_nSortCol )	
	{
		rcLabel.right -= 3 * offset;	
	}
	rcLabel.left += offset;
	rcLabel.right -= offset;	
	// Draw column label
	if( rcLabel.left < rcLabel.right )
		dc.DrawText(buf,-1,rcLabel, uFormat);
	// Draw the Sort arrow	
	if( m_nSortOrder!=0 && lpDrawItemStruct->itemID == (UINT)m_nSortCol )	
	{
		CRect rcIcon( lpDrawItemStruct->rcItem );
		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject( &penLight );		
		if( m_nSortOrder==1 )		
		{
			// Draw triangle pointing upwards
			dc.MoveTo( rcIcon.right - 2*offset, offset-1);
			dc.LineTo( rcIcon.right - 3*offset/2, rcIcon.bottom - offset );
			dc.LineTo( rcIcon.right - 5*offset/2-2, rcIcon.bottom - offset );
			dc.MoveTo( rcIcon.right - 5*offset/2-1, rcIcon.bottom - offset-1 );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 2*offset, offset-2);		
		}
		else		
		{
			// Draw triangle pointing downwords
			dc.MoveTo( rcIcon.right - 3*offset/2, offset-1);
			dc.LineTo( rcIcon.right - 2*offset-1, rcIcon.bottom - offset + 1 );
			dc.MoveTo( rcIcon.right - 2*offset-1, rcIcon.bottom - offset );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 5*offset/2-1, offset -1 );
			dc.LineTo( rcIcon.right - 3*offset/2, offset -1);		
		}
		// Restore the pen
		dc.SelectObject( pOldPen );	
	}	
	// Restore dc	
	dc.RestoreDC( nSavedDC );
	// Detach the dc before returning	
	dc.Detach();
}

BOOL COXGridHeader::SortColumn(int nCol, int nSortOrder)
{
	ASSERT(nCol>=0 && nCol<=GetItemCount());

	CWnd* pParentWnd=GetParent();
	ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)));

#ifdef _UT_UXTHEME
	
	if (::GetWindowTheme(m_hWnd) != NULL)
	{
		// Change all items to owner draw
		for (int i = 0; i < GetItemCount(); i++)
		{
			HD_ITEM hditem;	
			hditem.mask=HDI_FORMAT;
			GetItem(i,&hditem);
			hditem.fmt|=HDF_OWNERDRAW;
			SetItem(i,&hditem);
		}
		goto out;
	}
#endif // _UT_UXTHEME

	if(nCol!=m_nSortCol)
	{
		// Change the item from ownder drawn	
		HD_ITEM hditem;	
		hditem.mask = HDI_FORMAT;
		GetItem(m_nSortCol, &hditem);
		hditem.fmt &= ~HDF_OWNERDRAW;
		hditem.fmt |= HDF_STRING;
		SetItem(m_nSortCol, &hditem);

		if(nSortOrder!=0)
		{
			// Change the item to ownder drawn	
			HD_ITEM hditem;	
			hditem.mask = HDI_FORMAT;
			GetItem(nCol, &hditem);
			hditem.fmt |= HDF_OWNERDRAW;
			SetItem(nCol, &hditem);
		}

		// Invalidate header control so that it gets redrawn
		Invalidate();
	}

out:
	m_nSortCol=nCol;
	m_nSortOrder=nSortOrder;

	return TRUE;
}

void COXGridHeader::PreSubclassWindow()
{
	// Hook the mouse
	if (m_hMouseHook == NULL)
		m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE, MouseProc, 0, AfxGetApp()->m_nThreadID);
}

void COXGridHeader::OnDestroy() 
{
	// Unhook the mouse
	if (m_hMouseHook)
	{
		::UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = NULL;
	}

	CHeaderCtrl::OnDestroy();
}

LRESULT CALLBACK COXGridHeader::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	if (nCode == HC_ACTION && wParam == WM_MOUSEMOVE)
	{
		MOUSEHOOKSTRUCT* pMH = (MOUSEHOOKSTRUCT*) lParam;

		// If the previous message was for COXSizableMiniDockFrameWnd and the current is not
		// we need to update the caption buttons
		COXGridHeader* pPrev = DYNAMIC_DOWNCAST(COXGridHeader,
			CWnd::FromHandlePermanent(m_hwndPrevMouseMoveWnd));
		if (pPrev == NULL)
			pPrev = DYNAMIC_DOWNCAST(COXGridHeader,
				CWnd::FromHandlePermanent(::GetParent(m_hwndPrevMouseMoveWnd)));

		COXGridHeader* pCurrent = DYNAMIC_DOWNCAST(COXGridHeader,
			CWnd::FromHandlePermanent(pMH->hwnd));
		if (pCurrent == NULL)
			pCurrent = DYNAMIC_DOWNCAST(COXGridHeader,
				CWnd::FromHandlePermanent(::GetParent(pMH->hwnd)));

		if (pPrev != NULL && pCurrent != pPrev)
		{
			// The mouse just left the header control
			pPrev->Invalidate();
		}
		else if (pCurrent != NULL && pCurrent != pPrev)
		{
			// The mouse just entered the header control
			pCurrent->Invalidate();
		}
		m_hwndPrevMouseMoveWnd = pMH->hwnd;
	}

	return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

void COXGridHeader::OnLButtonDown(UINT nFlags, CPoint point) 
{
	Invalidate();
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

void COXGridHeader::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
#ifdef _UT_UXTHEME
	if (::GetWindowTheme(m_hWnd) != NULL)
		Invalidate();
#endif
	CHeaderCtrl::OnMouseMove(nFlags, point);
}


// ==========================================================================
