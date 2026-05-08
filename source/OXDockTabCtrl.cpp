// ==========================================================================
// 							Class Implementation : COXDockTabCtrl
// ==========================================================================

// Source file : OXDockTabCtrl.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
                        

#include "stdafx.h"
#include "OXDockTabCtrl.h"
#include "OXCoolToolBar.h"
#include "OXSizeCtrlBar.h"
#include "OXSizeDockBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COXDockTabCtrl

COXDockTabCtrl::COXDockTabCtrl(COXSizeDockBar* pSizeDockBar)
{
	m_pSizeDockBar = pSizeDockBar;
	m_pLastSelectedBar = NULL;
}

COXDockTabCtrl::~COXDockTabCtrl()
{
}


BEGIN_MESSAGE_MAP(COXDockTabCtrl, COXSkinnedTabCtrl)
	//{{AFX_MSG_MAP(COXDockTabCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void COXDockTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ptLButtonDown = point;
	COXSkinnedTabCtrl::OnLButtonDown(nFlags, point);
}

void COXDockTabCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// In order to start dragging the left button must be down and the current point
	// must be at least 3 pixels away from the point where the button was pressed
	if ((nFlags & MK_LBUTTON) && 
		(abs(point.x - m_ptLButtonDown.x) > 3 || abs(point.y - m_ptLButtonDown.y) > 3))
	{
		TCHITTESTINFO thti;
		thti.pt = point;
		int iIndex = HitTest(&thti);

		if (iIndex != -1)
		{
			CControlBar* pBar = GetBar(iIndex);

			// Handle COXCoolToolBar
			COXCoolToolBar* pToolBar = DYNAMIC_DOWNCAST(COXCoolToolBar, pBar);
			if (pToolBar)
			{
				// We have a toolbar
				pToolBar->SaveMouseOffset(CPoint(0, 0));
				pToolBar->SetCapture();
				pToolBar->m_bDragging = true;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				return;
			}

			// Handle COXSizeControlBar
			COXSizeControlBar* pControlBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pBar);
			if (pControlBar)
			{
				// We have a docking window
				pControlBar->SaveMouseOffset(CPoint(0, 0));
				pControlBar->SetCapture();
				pControlBar->m_bDragging = true;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				return;
			}
		}
	}

	COXSkinnedTabCtrl::OnMouseMove(nFlags, point);
}

void COXDockTabCtrl::OnSelChange(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	ShowSelectedTab();
	*pResult = 0;
}

// Inserts the given control bar as a new tab
void COXDockTabCtrl::InsertTab(CControlBar* pBar, int iIndex, BOOL bShowSelectedTab)
{
	// If this is the only control bar in the dock bar do nothing
	int iSizeControlBarCount = m_pSizeDockBar->GetSizeControlBarCount(pBar);
	if (iSizeControlBarCount == 0)
		return;

	// Add a tab for all other size control bars that are docked but not tabbed

	int i = 0;
	for (i = 0; i < m_pSizeDockBar->m_arrBars.GetSize(); i++)
	{
		COXSizeControlBar* pSizeBar = DYNAMIC_DOWNCAST(COXSizeControlBar, m_pSizeDockBar->GetDockedControlBar(i));
		if (pSizeBar != NULL && pSizeBar != pBar && FindTab(pSizeBar) == -1)
		{
			CString strTextOther;
			pSizeBar->GetWindowText(strTextOther);

			TCITEM tciOther;
			tciOther.mask = TCIF_TEXT | TCIF_PARAM;
			tciOther.pszText = strTextOther.GetBuffer(strTextOther.GetLength());
			tciOther.lParam = (LPARAM) pSizeBar;
		
			//Check added for visibility so that hidden control
			//bars don't get shown - Nish - Feb 15th 2005
			if(pSizeBar->IsWindowVisible())
				InsertItem(0, &tciOther);
		}
	}

	// Insert this control bar to the tab control
	CString strText;
	pBar->GetWindowText(strText);

	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = strText.GetBuffer(strText.GetLength());
	tci.lParam = (LPARAM) pBar;

	InsertItem(iIndex, &tci);
	SetCurSel(iIndex);

	// Reshresh the tab control
	m_pSizeDockBar->PositionTabCtrl();

	if (bShowSelectedTab)
		ShowSelectedTab();
	else
	{
		int iSelected = GetCurSel();
		for (i = 0; i < GetItemCount(); i++)
		{
			CControlBar* pBar = GetBar(i);
			if (iSelected != i)
				pBar->GetDockingFrame()->ShowControlBar(pBar, FALSE, TRUE); // hide
		}
	}
}

void COXDockTabCtrl::ShowSelectedTab()
{
	int iSelected = GetCurSel();

	// Show first
	for (int i = 0; i < GetItemCount(); i++)
	{
		CControlBar* pBar = GetBar(i);
		if (iSelected == i)
		{
			pBar->GetDockingFrame()->ShowControlBar(pBar, TRUE, TRUE); // show
		}
		else
		{
			pBar->GetDockingFrame()->ShowControlBar(pBar, FALSE, TRUE); // hide
		}
	}

	CRect rect;
	m_pSizeDockBar->GetClientRect(rect);

	CControlBar* pBar = GetBar(iSelected);
	if (pBar != NULL)
	{
		COXSizeControlBar* pSizeBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pBar);
		if (m_pSizeDockBar->IsBarHorizontal())
		{
			if (m_pLastSelectedBar != NULL)
			{
				pSizeBar->m_HorzDockSize.cx = m_pLastSelectedBar->m_HorzDockSize.cx;
			}
			m_pLastSelectedBar = pSizeBar;
		}
		else // vertical
		{
			CFrameWnd* pMF = (CFrameWnd*) GetParentFrame();//AfxGetMainWnd();
			if (pMF != NULL)
			{
				if (pMF->GetControlBar(AFX_IDW_DOCKBAR_LEFT) == m_pSizeDockBar)
					rect.right -= 3;

				if (pMF->GetControlBar(AFX_IDW_DOCKBAR_RIGHT) == m_pSizeDockBar)
					rect.left += 4;
			}
			
			rect.bottom -= m_pSizeDockBar->GetTabHeight();

			pBar->MoveWindow(rect, TRUE);

			COXSizeControlBar* pSizeBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pBar);
			if (pSizeBar)
			{
				pSizeBar->m_VertDockSize.cx = rect.Width();
				pSizeBar->m_VertDockSize.cy = rect.Height() + 5;
			}
		}
	}
}

void COXDockTabCtrl::RemoveTab(CControlBar* pBar)
{
	// If there are not tabs just exit
	if (GetItemCount() == 0)
		return;

	// If there is only one other tab left we need to remove it as well
	if (m_pSizeDockBar->GetSizeControlBarCount(pBar) == 1)
	{
		COXSizeControlBar* pOther = m_pSizeDockBar->GetFirstDockedSizeControlBar(pBar);
		
		int iOtherIndex = FindTab(pOther);
		DeleteItem(iOtherIndex);
		pOther->GetDockingFrame()->ShowControlBar(pOther, TRUE, TRUE);
	}

	// Remove this tab
	int iIndex = FindTab(pBar);
	DeleteItem(iIndex);

	if (GetItemCount() > 1)
	{
		// Set the current selection to the previous tab
		if (iIndex != 0)
			SetCurSel(iIndex - 1);
		else
			SetCurSel(0);
		ShowSelectedTab();
	}
}

// This function searched the tab control for the given control bar and returns its
// index or -1 if not found.
int COXDockTabCtrl::FindTab(CControlBar* pBar)
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		TCITEM tci;
		tci.mask = TCIF_PARAM;
 		GetItem(i, &tci);

		if (tci.lParam == (LPARAM) pBar)
			return i;
	}
	return -1;
}

// If the point is over the tab control this function returns the index where
// the new tab should be inserted. If the point is not over the tab control it
// returns -1
int COXDockTabCtrl::HitTestTabControl(CPoint point, CControlBar* /*pBar*/)
{
	// Deterine the rectangle of the buttons

	CRect rectTabButtons;
	m_pSizeDockBar->GetWindowRect(rectTabButtons);
	rectTabButtons.top = rectTabButtons.bottom - 48;

	if (rectTabButtons.PtInRect(point))
	{
		// Determine the actual button
		TCHITTESTINFO thti;
		ScreenToClient(&point);
		thti.pt = point;
		int iIndex = HitTest(&thti);
		if (iIndex == -1)
			return 0;

		return iIndex;
	}

	// If there are more when 1 tabs already the test area should cover the entire dock window
	// and the new tab should be inserted at the end
//	int iTabCount = GetItemCount();
//	if (iTabCount > 0)
//	{
//		CRect rectDockBar;
//		m_pSizeDockBar->GetWindowRect(rectDockBar);
//		rectDockBar.InflateRect(20, 20);
//		if (rectDockBar.PtInRect(point))
//			return iTabCount;
//	}

	return -1;
}

// Moves the tab from the old position to the new position
void COXDockTabCtrl::RepositionTabs(int iOldIndex, int iNewIndex, CPoint point)
{
	// To avoid flashing only reposition if the mouse cursor is behind the midpoint
	// of the next or previous item
	
	CRect rectNew;
	GetItemRect(iNewIndex, rectNew);
	ScreenToClient(&point);
	if (iOldIndex < iNewIndex)
	{
		// The new tab is to the right
		if (point.x < rectNew.CenterPoint().x)
			return; // the mouse cursor is not far enough to the right
	}
	else
	{
		// The new tab is to the left
		if (point.x > rectNew.CenterPoint().x)
			return; // the mouse cursor is not far enough to the left
	}

	TCHAR szBuffer[256];

	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = szBuffer;
	tci.cchTextMax = 256;
	GetItem(iOldIndex, &tci);

	DeleteItem(iOldIndex);
	InsertItem(iNewIndex, &tci);
}

CControlBar* COXDockTabCtrl::GetBar(int iIndex)
{
	TCITEM tci;
	tci.mask = TCIF_PARAM;
 	GetItem(iIndex, &tci);

	return (CControlBar*) tci.lParam;
}

int COXDockTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COXSkinnedTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_font.CreatePointFont(80, _T("MS Sans Serif"));

	SetFont(&m_font);

	SetPadding(CSize(12, 4));
	
	return 0;
}
