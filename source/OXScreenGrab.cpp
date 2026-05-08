// ==========================================================================
// 					Class Implementation : COXScreenGrabber
// ==========================================================================

// Source file : oxscreengrab.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"			// standard MFC include
#include "oxscreengrab.h"	// file header


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXScreenGrabber, CObject)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:

// private:

// Member functions ---------------------------------------------------------
// public:

COXScreenGrabber::COXScreenGrabber()
{
}

BOOL COXScreenGrabber::GrabFullWindow(CWnd* pWnd /* = NULL */)
{
	CRect WndRect(0, 0, 0, 0);

	if (!PrepareWindow(FALSE, WndRect, pWnd))
		return FALSE;

	return GrabRectangle(WndRect);
}

BOOL COXScreenGrabber::GrabClientWindow(CWnd* pWnd /* = NULL */)
{
	CRect ClientRect(0, 0, 0, 0);

	if (!PrepareWindow(TRUE, ClientRect, pWnd))
		return FALSE;

	return GrabRectangle(ClientRect);
}

BOOL COXScreenGrabber::GrabRectangle(CRect SrcRect)
{
	CDC SrcDC, MemDC;

	// Check For an empty rectangle
	if(SrcRect.IsRectEmpty())
		return FALSE;

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// create a DC for the screen and create
	// a memory DC compatible to screen DC
	if (!SrcDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL))
		return FALSE;
	if (!MemDC.CreateCompatibleDC(&SrcDC))
	{
		SrcDC.DeleteDC();
		return FALSE;
	}

	// get screen resolution
	int nXScrn = SrcDC.GetDeviceCaps(HORZRES);
	int nYScrn = SrcDC.GetDeviceCaps(VERTRES);

#if(WINVER >= 0x0500)

	DISPLAY_DEVICE display;
	display.cb = sizeof(DISPLAY_DEVICE);
	if (EnumDisplayDevices(NULL, 1, &display, 0))
	{
		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		if (EnumDisplaySettings((LPCTSTR)&display.DeviceName[0], ENUM_CURRENT_SETTINGS, &devMode))
		{
			// This assumes that the second monitor is set to draw to the 'right' of the first one.
			nXScrn += devMode.dmPelsWidth;
		}
	}

#endif

	// make sure window rectangle is visible
	if (SrcRect.left < 0)
		SrcRect.left = 0;
	if (SrcRect.top < 0)
		SrcRect.top = 0;
	if (SrcRect.right > nXScrn)
		SrcRect.right = nXScrn;
	if (SrcRect.bottom > nYScrn)
		SrcRect.bottom = nYScrn;

	// create a bitmap compatible with the screen DC
	CBitmap ScreenBMP;
	if(!ScreenBMP.CreateCompatibleBitmap(&SrcDC, SrcRect.Width(), SrcRect.Height()))
	{
		SrcDC.DeleteDC();
		MemDC.DeleteDC();

		return FALSE;
	}

	// select new bitmap into memory DC
	CBitmap* pOldBitmap = MemDC.SelectObject(&ScreenBMP);

	// bitblt screen DC to memory DC
	MemDC.BitBlt(0, 0, SrcRect.Width(), SrcRect.Height(), &SrcDC, SrcRect.left, SrcRect.top, SRCCOPY);

	// select old bitmap back into memory DC 
	MemDC.SelectObject(pOldBitmap);

	// Clear previous contents of DIB 
	GrabDIB.Empty();

	CPalette SysPalette;
	COXDIB::GetSystemPalette(&SysPalette);
	GrabDIB.BitmapToDIB(HBITMAP(ScreenBMP), &SysPalette);

	// clean up
	SrcDC.DeleteDC();
	MemDC.DeleteDC();

	return TRUE;
}

COXDIB* COXScreenGrabber::GetGrabDIB()
{
	return &GrabDIB;
}

BOOL COXScreenGrabber::GrabTracker(CWnd* pWndFromStartPoint, CPoint StartPoint, CWnd* pWndForRect)
{
	// some simple validations
	if (pWndFromStartPoint == NULL)
		pWndFromStartPoint = CWnd::GetDesktopWindow();

	if (pWndForRect == NULL)
		return FALSE;

	// the startpoint is defined in the client coordinate system of pWndFromStartPoint
	// but the tracking rect will be defined within pWndForRect, so convert
	pWndFromStartPoint->MapWindowPoints(pWndForRect, &StartPoint, 1);

	CRectTracker tracker;

	// do the tracking
	if (tracker.TrackRubberBand(pWndForRect, StartPoint, TRUE))
	{
		CRect rect = tracker.m_rect;

		// because we allowed inversion in the tracking, normalize the rect
		rect.NormalizeRect();

		// the screengrabber needs screen coordinates
		pWndForRect->ClientToScreen(&rect);

		return GrabRectangle(rect);
	}
	else
		return FALSE;
}

#ifdef _DEBUG
void COXScreenGrabber::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}

void COXScreenGrabber::AssertValid() const
{
	CObject::AssertValid();
}
#endif

// Protected member functions
BOOL COXScreenGrabber::PrepareWindow(BOOL bClient, CRect& ScrRect, CWnd* pWnd)
{
	if (pWnd == NULL)
		pWnd = CWnd::GetForegroundWindow();
	if (pWnd == NULL)
		pWnd = CWnd::GetDesktopWindow();

	ASSERT(pWnd != NULL);
	if (!pWnd->IsWindowVisible())
		return FALSE;

	if (pWnd == CWnd::GetDesktopWindow())
	{
		// create a DC for the screen and create
		CDC SreenDC;
		if (!SreenDC.CreateDC(_T("DISPLAY"), NULL, NULL, NULL))
			return FALSE;

		// get screen resolution and set Rect
		ScrRect.left = 0;
		ScrRect.top = 0;
		ScrRect.right = SreenDC.GetDeviceCaps(HORZRES);
		ScrRect.bottom = SreenDC.GetDeviceCaps(VERTRES);

		SreenDC.DeleteDC();
	}
	else
	{
		// Move window which was selected to top of Z-order for
		// the capture, and make it redraw itself
		pWnd->SetWindowPos(NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE);
		pWnd->UpdateWindow();

		if (bClient)
		{
			// get the client area dimensions
			pWnd->GetClientRect(&ScrRect);

			// convert client coords to screen coords
			CPoint pt1(ScrRect.left, ScrRect.top); 
			CPoint pt2(ScrRect.right, ScrRect.bottom); 

			pWnd->ClientToScreen(&pt1);
			pWnd->ClientToScreen(&pt2);

			ScrRect.left = pt1.x;
			ScrRect.top = pt1.y;
			ScrRect.right = pt2.x;
			ScrRect.bottom = pt2.y;
		}
		else
			pWnd->GetWindowRect(&ScrRect);
	}

	return TRUE;
}

COXScreenGrabber::~COXScreenGrabber()
{
}

