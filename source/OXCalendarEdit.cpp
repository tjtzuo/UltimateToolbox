// ==========================================================================
// 			Class Implementation : COXCalendarEdit, COXMonthCalPopup
// ==========================================================================

// Source file : OXCalendarEdit.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.   

#include "stdafx.h"
#include "OXCalendarEdit.h"

#pragma warning(disable : 4706)

// v9.3 - update 05 - shouldn't need to include this for VS2010 - problematic for static MFC linkage (?)
#if _MSC_VER < 1600
#include <multimon.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COXMonthCalPopup

COXMonthCalPopup::COXMonthCalPopup()
{
}

COXMonthCalPopup::~COXMonthCalPopup()
{
}


BEGIN_MESSAGE_MAP(COXMonthCalPopup, CMonthCalCtrl)
	//{{AFX_MSG_MAP(COXMonthCalPopup)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COXMonthCalPopup::Pick(CRect rect, CRect rectParent, COleDateTime dtInitialDate, COleDateTime& dtPickedDate)
{
	AdjustDisplayRectangle(rect, rectParent);

	SetCurSel(dtInitialDate);

	MoveWindow(rect);
	ShowWindow(SW_SHOW);
	SetCapture();

	// init message loop
	bool bBreak = false;
	BOOL bValidDatePicked = FALSE;
	while (!bBreak)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));


		if (msg.message == WM_LBUTTONDOWN)
		{
			DispatchMessage(&msg);
			SetCapture();
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			DispatchMessage(&msg);

			MCHITTESTINFO hti;
			::memset(&hti, 0, sizeof(hti));
			hti.pt.x = GET_X_LPARAM(msg.lParam);
			hti.pt.y = GET_Y_LPARAM(msg.lParam);
			hti.cbSize = sizeof(hti);
			HitTest(&hti);
			DWORD dwResult = HitTest(&hti);
			if (dwResult == MCHT_CALENDARDATE)
			{
				// The user clicked on a date, so save and close
				SYSTEMTIME stOut;
				GetCurSel(&stOut);
				stOut.wHour = 0;
				stOut.wMinute = 0;
				stOut.wSecond = 0;
				stOut.wMilliseconds = 0;
		
				dtPickedDate = COleDateTime(stOut);
				bValidDatePicked = TRUE;
				bBreak = true;
			}
			else if (dwResult == MCHT_NOWHERE)
			{
				// The user clicked outside
				bBreak = true;
			}

			SetCapture();
		}
		else if (msg.message == WM_KEYDOWN)
		{
			// Handle ESCAPE and ENTER
			if (msg.wParam == VK_ESCAPE)
				bBreak = true;
			else if (msg.wParam == VK_RETURN)
			{
				DispatchMessage(&msg);

				SYSTEMTIME stOut;
				GetCurSel(&stOut);
				stOut.wHour = 0;
				stOut.wMinute = 0;
				stOut.wSecond = 0;
				stOut.wMilliseconds = 0;
		
				dtPickedDate = COleDateTime(stOut);
				bValidDatePicked = TRUE;
				bBreak = true;
			}
			else
				DispatchMessage(&msg);
		}
		else
		{
			DispatchMessage(&msg);
		}
	}

	ReleaseCapture();
	ShowWindow(SW_HIDE);

	return bValidDatePicked;
}


/////////////////////////////////////////////////////////////////////////////
// COXCalendarEdit

IMPLEMENT_DYNCREATE(COXCalendarEdit, COXMaskedEdit)

COXCalendarEdit::COXCalendarEdit()
{
	m_strDateFormat = _T("");
}

COXCalendarEdit::COXCalendarEdit(LPCTSTR lpszDateFormat)
{
	SetDateFormat(lpszDateFormat);
}

COXCalendarEdit::~COXCalendarEdit()
{
}


BEGIN_MESSAGE_MAP(COXCalendarEdit, COXMaskedEdit)
	//{{AFX_MSG_MAP(COXCalendarEdit)
		ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXCalendarEdit message handlers

BOOL COXCalendarEdit::InitializeDropEdit()
{
	if(!m_Calendar.CreateEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, _T("SysMonthCal32"),_T(""),
		WS_POPUP | WS_BORDER, CRect(0, 0, 0, 0), this, 0, NULL))
	{
		TRACE(_T("Unable to create drop list.\n"));
		AfxThrowResourceException();
	}
	m_Calendar.SetOwner(this);

	return TRUE;
}

void COXCalendarEdit::OnDropButton()
{
	CRect rect;
	GetClientRect(rect);
	ClientToScreen(rect);
	rect.right += GetButtonWidth();
	CRect rectParent = rect;
	rect.top = rect.bottom;
	rect.left = rect.right;

	// Get the size of the calendar control
	RECT rectSize;
	m_Calendar.GetMinReqRect(&rectSize);
	rect.left -= rectSize.right;
	rect.bottom += rectSize.bottom;
	
	// Read the initial date
	CString strText;
	GetWindowText(strText);
	COleDateTime dtInitialDate = GetDateFromString(strText);
	if (dtInitialDate.GetStatus() != COleDateTime::valid)
		dtInitialDate = COleDateTime::GetCurrentTime();

	// Display the calendar popup window
	COleDateTime dtPickedDate;
	BOOL bDatePicked = m_Calendar.Pick(rect, rectParent, dtInitialDate, dtPickedDate);
	if (bDatePicked)
		SetWindowText(GetStringFromDate(dtPickedDate));
}

CString COXCalendarEdit::GetErrorString()
{
	return _T("<bad date>");
}

void COXCalendarEdit::SetDateFormat(LPCTSTR lpszDateFormat)
{
	m_strDateFormat = lpszDateFormat;
	m_strDateFormat.MakeUpper();

	// Set the masked edit format
	CString strMaskedEditFormat(m_strDateFormat);
	strMaskedEditFormat.Replace('D', '#');
	strMaskedEditFormat.Replace('M', '#');
	strMaskedEditFormat.Replace('Y', '#');
	SetMask(strMaskedEditFormat);
}

COleDateTime COXCalendarEdit::GetDateFromString(LPCTSTR lpszDateString)
{
	CString strDate = lpszDateString;

	// First get the year

	// 1. Find where the year starts and where it ends
	int iYearStartIdx = m_strDateFormat.Find('Y');
	int iYearEndIdx = m_strDateFormat.ReverseFind('Y');
	CString strYearOnly = strDate.Mid(iYearStartIdx, iYearEndIdx - iYearStartIdx + 1);
	int iYear = ::_ttoi(strYearOnly);

	// 2. Find where the month starts and where it ends
	int iMonthStartIdx = m_strDateFormat.Find('M');
	int iMonthEndIdx = m_strDateFormat.ReverseFind('M');
	CString strMonthOnly = strDate.Mid(iMonthStartIdx, iMonthEndIdx - iMonthStartIdx + 1);
	int iMonth = ::_ttoi(strMonthOnly);

	// 3. Find where the day starts and where it ends
	int iDayStartIdx = m_strDateFormat.Find('D');
	int iDayEndIdx = m_strDateFormat.ReverseFind('D');
	CString strDayOnly = strDate.Mid(iDayStartIdx, iDayEndIdx - iDayStartIdx + 1);
	int iDay = ::_ttoi(strDayOnly);

	return COleDateTime(iYear, iMonth, iDay, 0, 0, 0);
}

void COXCalendarEdit::OnKillFocus(CWnd* pNewWnd) 
{
	COXMaskedEdit::OnKillFocus(pNewWnd);
}

CString COXCalendarEdit::GetStringFromDate(COleDateTime dtDate)
{
	// Format the picked date and put it into the edit box

	// We need to convert the COXCalendarEdit date format string into
	// and strftime() style format string.
	CString strNewFormat(m_strDateFormat);
	strNewFormat.Replace(_T("YYYY"), _T("%Y"));
	strNewFormat.Replace(_T("YY"), _T("%y"));
	strNewFormat.Replace(_T("MM"), _T("%m"));
	strNewFormat.Replace(_T("DD"), _T("%d"));

	return dtDate.Format(strNewFormat);
}

CString COXCalendarEdit::GetText()
{
	CString strText = COXMaskedEdit::GetText();
	
	if (IsEmptyDate(strText))
		return _T("");
	else if (IsValidDate(strText))
		return strText;
	else
		return GetErrorString();
}

BOOL COXCalendarEdit::IsValidDate(LPCTSTR lpszTestDate)
{
	ASSERT(lpszTestDate != NULL);
	
	if (IsEmptyDate(lpszTestDate))
		return TRUE;

	COleDateTime dtDate = GetDateFromString(lpszTestDate);
	if (dtDate.GetStatus() == COleDateTime::valid)
		return TRUE;
	else
		return FALSE;
}

void COXMonthCalPopup::AdjustDisplayRectangle(CRect &rect, CRect rectParent)
{
	// Get the rectangle of the monitor closest to the menu rectangle
	HMONITOR hMonitor = ::MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	::GetMonitorInfo(hMonitor, &mi);

	const int iMixScreenX = mi.rcMonitor.left;
	const int iMaxScreenX = mi.rcMonitor.right;
	const int iMaxScreenY = mi.rcMonitor.bottom;

	CSize sizeParent(rectParent.Width(), rectParent.Height());
	CPoint ptTopLeft(rect.left, rect.top);

	if (ptTopLeft.x < iMixScreenX)
		ptTopLeft.x = iMixScreenX;
	if (ptTopLeft.x + rect.Width() > iMaxScreenX)
		ptTopLeft.x = iMaxScreenX - rect.Width();

	// Make sure the popup is not clipped at the bottom
	if (rect.bottom > iMaxScreenY)
	{
		// The popup should be above the item
		ptTopLeft.y = rectParent.top - rect.Height();
	}

	CSize sizeTemp = rect.Size();
	rect.SetRect(ptTopLeft.x, ptTopLeft.y,
		ptTopLeft.x + sizeTemp.cx, ptTopLeft.y + sizeTemp.cy);
}

BOOL COXCalendarEdit::IsEmptyDate(LPCTSTR lpszTestDate)
{
	CString strTestDate = lpszTestDate;
	if (strTestDate.IsEmpty())
		return TRUE;

	// Produce the empty date string
	CString strEmptyDate = m_strDateFormat;
	strEmptyDate.Replace('Y', ' ');
	strEmptyDate.Replace('M', ' ');
	strEmptyDate.Replace('D', ' ');

	if (strTestDate == strEmptyDate)
		return TRUE;
	else
		return FALSE;
}

void COXCalendarEdit::SetText(LPCTSTR lpszDate)
{
	if (IsValidDate(lpszDate))
		COXMaskedEdit::SetText(lpszDate);
	else
		COXMaskedEdit::SetText(_T(""));
}
