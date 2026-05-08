// ==========================================================================
//				Class Implementation : OXCalendar
// ==========================================================================

// Source file :OXCalendar.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXCalendar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COXCalendar

static int nNumDays;

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

CStringArray COXCalendar::ms_straWeekDays;
CStringArray COXCalendar::ms_straMonthNames;


// Data members -------------------------------------------------------------
//protected:
//	COleDateTime m_CurrentDate;
// --- Current date value 

//	COleDateTime m_StartDate; COleDateTime m_EndDate;
// --- starting and ending dates of selectable date range

//	int m_nFirstDayOfWeek;
//--- number of first day of week
// this value stored in internal format
// 0 - Monday 1 - Tuesday, ect., 6 - Sunday

//	COXCalendarPopup m_wndDatePicker;
//--- calendar popup window

//	HCURSOR m_hCursor;
//--- window cursor 

//	BOOL m_bDrawButton;
//--- this flag show whether the down arrow will draw

//	int m_nButtonWidth;
//--- width in pixels of down arrow

//private
//int ms_nRefCount
//--- count of present COXCalendar objects (static)

// this callback routine is used to fill COXCalendar::ms_straWeekDays
BOOL CALLBACK COXCalendar::EnumDayOfWeek(LPTSTR lpCalendarInfoString)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	COXCalendar::ms_straWeekDays.Add(lpCalendarInfoString);
	ASSERT(COXCalendar::ms_straWeekDays.GetSize() <= 7);// we don't need long weeks
	return FALSE;
}

// this callback routine is used to fill COXCalendar::ms_straMonthNames
BOOL CALLBACK COXCalendar::EnumMonth(LPTSTR lpCalendarInfoString)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	COXCalendar::ms_straMonthNames.Add(lpCalendarInfoString);
	ASSERT(COXCalendar::ms_straMonthNames.GetSize() <= 12);
	return FALSE;
}


void COXCalendar::GetCalendarLocaleData()
{
	ms_straWeekDays.RemoveAll();
	ms_straMonthNames.RemoveAll();

	//Retrieving Win32 National Language Support Calendar Information
	// Windows provide weekdays as
	// 0-Monday

	// for 1st WeekDay, ect.
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME1);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME2);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME3);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME4);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME5);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME6);
	EnumCalendarInfo(EnumDayOfWeek,LOCALE_SYSTEM_DEFAULT,1,CAL_SABBREVDAYNAME7);

	// now retrieving months
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME1);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME2);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME3);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME4);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME5);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME6);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME7);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME8);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME9);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME10);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME11);
	EnumCalendarInfo(EnumMonth,LOCALE_SYSTEM_DEFAULT,1,CAL_SMONTHNAME12);
}


int COXCalendar::GetLocaleFirstDayOfWeek()
{
	// get space needed to take locale info
	int nCount =::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_IFIRSTDAYOFWEEK,NULL,0);
	ASSERT(nCount>0);
	CString sFirstDay;
	// retreiving default number of first day of week
	VERIFY(::GetLocaleInfo(
		LOCALE_SYSTEM_DEFAULT,LOCALE_IFIRSTDAYOFWEEK,
		sFirstDay.GetBufferSetLength(nCount),nCount));
	int nFirstDayOfWeek=_ttoi(sFirstDay.GetBuffer(sFirstDay.GetLength()));
	sFirstDay.ReleaseBuffer();

	return nFirstDayOfWeek;
}


COXCalendar::COXCalendar() :
	m_StartDate(100,1,1,0,0,0),
	m_EndDate(9999,12,31,0,0,0),
	m_bDrawButton(FALSE),
	m_sFormat(_T(""))
{
	m_CurrentDate.SetStatus(COleDateTime::null);
	m_nFirstDayOfWeek=COXCalendar::GetLocaleFirstDayOfWeek();

	if(ms_straWeekDays.GetSize()==0 || ms_straMonthNames.GetSize()==0)
	{
		COXCalendar::GetCalendarLocaleData();
	}
}

COXCalendar::~COXCalendar()
{
}

BEGIN_MESSAGE_MAP(COXCalendar, CWnd)
	//{{AFX_MSG_MAP(COXCalendar)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTINGCHANGE,OnSettingChanged)
	ON_EN_SETFOCUS(IDEDIT, OnSetFocusEdit)
	ON_EN_KILLFOCUS(IDEDIT, OnKillFocusEdit)
	ON_EN_CHANGE(IDEDIT,OnEnChange)
	ON_EN_UPDATE(IDEDIT,OnEnUpdate)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COXCalendar message handlers

BOOL COXCalendar::Create(DWORD dwStyle, const CRect& r, CWnd* pParentWnd, UINT nID)
{
	if(!CWnd::Create(NULL,_T(""),dwStyle,r,pParentWnd,nID))
	{
		 return FALSE;
	}

	return Init();// initialization
}

void COXCalendar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	COleDateTime targetDate;
	CRect rWnd;
	GetWindowRect(&rWnd);
	ASSERT(::IsWindow(m_wndDatePicker.m_hWnd));
	GetCurrentDate();//fill m_CurrentDate from text typed in edit control

	// perform bounds check and corrections
	ShrinkDateInBounds();

	if(m_wndDatePicker.Pick(m_CurrentDate,m_StartDate,m_EndDate,rWnd))
	{
		//Show selected date in edit window
		CString sText;
		SetOutputText(sText);
		m_wndEdit.SetWindowText(sText);
	}
	SetFocus();
	CWnd::OnLButtonDown(nFlags, point);
}

void COXCalendar::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	ASSERT(::IsWindow(m_wndEdit.m_hWnd));
	m_wndEdit.SetFocus();
}

void COXCalendar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if(m_bDrawButton)
	{
		CRect r;
		GetClientRect(&r);
		r.left = r.right - m_nButtonWidth;
		dc.DrawFrameControl(&r,DFC_SCROLL,DFCS_SCROLLDOWN);
	}

	// Do not call CWnd::OnPaint() for painting messages
}

LRESULT COXCalendar::OnSettingChanged(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	COXCalendar::GetCalendarLocaleData();

	return 0;
}


void COXCalendar::OnKillFocusEdit()
	{
	// now trying to parse edit text and correct mismatched day of week 
	GetCurrentDate();//fill m_CurrentDate from text typed in edit control
	// perform bounds check and corrections
	ShrinkDateInBounds();
	if(FALSE || m_CurrentDate.GetStatus() == COleDateTime::valid)
		{
		CString sText;
		SetOutputText(sText);
		m_wndEdit.SetWindowText(sText);
		}
	
	int id = GetDlgCtrlID();
	DWORD nCode = EN_KILLFOCUS;
	nCode <<= 16;
	DWORD wParam = nCode | id;
	GetParent()->SendMessage(WM_COMMAND,wParam,(LPARAM)GetSafeHwnd());

	ReInitLayout(FALSE); // hide dropdown button
	}

void COXCalendar::OnSetFocusEdit()
	{
	int id = GetDlgCtrlID();
	DWORD nCode = EN_SETFOCUS;
	nCode <<= 16;
	DWORD wParam = nCode | id;
	GetParent()->SendMessage(WM_COMMAND,wParam,(LPARAM)GetSafeHwnd());

	ReInitLayout(TRUE); // show dropdown button
	}

BOOL COXCalendar::Init()
	// --- In  :
	// --- Out : 
	// --- Returns : Whether the window initialized succesfully
	// --- Effect : perform initialization, create child windows
	{
	ASSERT(::IsWindow(m_hWnd));
	CRect r;
	GetClientRect(&r);
	// In this test class CEdit styles are fixed
	if(!m_wndEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP,
		r,this,IDEDIT))
		{
		TRACE0("COXCalendar:: failed to create child window!");
		return FALSE;
		}
	CWnd * pParent = GetParent();
	if(pParent)
		{
		CFont *pFont = pParent->GetFont();
		if(pFont)
			{
			LOGFONT lf;
			pParent->GetFont()->GetLogFont(&lf);
			m_font.CreateFontIndirect(&lf);
			SetFont(&m_font);
			m_wndEdit.SetFont(&m_font);
			}
		}
	m_nButtonWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	if(!m_wndDatePicker.Create(this))
		{
		TRACE0("COXCalendar:: failed to create child window!");
		return FALSE;
		}
	return TRUE;
	}

void COXCalendar::ReInitLayout(BOOL bShowBtn)
	// --- In  :Whether the down arrow show 
	// --- Out : 
	// --- Returns : 
	// --- Effect :manage edit box size, redraw window 
	{
	CRect r;
	GetClientRect(&r);
	m_bDrawButton = bShowBtn;// OnPaint() check it to draw button

	if(bShowBtn) // if set input focus, add space to down button
		r.right -= m_nButtonWidth;
	ASSERT(IsWindow(m_wndEdit.m_hWnd));
	m_wndEdit.MoveWindow(&r);
	InvalidateRect(NULL);
	}

BOOL COXCalendar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if(nHitTest == HTCLIENT && pWnd == this)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL COXCalendar::SetFirstDayOfWeek(int nDayOfWeek)
{
	//  use the following values: 0 Sunday, 1 Monday, 2 Tuesday,
	//3 Wednesday, 4 Thursday, 5 Friday, 6 Saturday
	if(nDayOfWeek < 0 || nDayOfWeek > 6)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	// convert it to internal schema:
	//  0 Monday, 1 Tuesday,2 Wednesday,
	// 3 Thursday, 4 Friday, 5 Saturday,6 Sunday

	m_nFirstDayOfWeek = (nDayOfWeek+6)%7;
	m_wndDatePicker.m_nFirstDayOfWeek = m_nFirstDayOfWeek;
	return TRUE;
}

int COXCalendar::GetFirstDayOfWeek()
{
	ASSERT(m_nFirstDayOfWeek >= 0 && m_nFirstDayOfWeek < 7);
	// convert from internal schema
	return (m_nFirstDayOfWeek + 1)%7;
}

BOOL COXCalendar::SetDateRange(COleDateTime startDate, COleDateTime endDate)
	{
	if(startDate.GetStatus() == COleDateTime::invalid ||
		endDate.GetStatus() == COleDateTime::invalid)
		{
		// dates must be valid
		ASSERT(FALSE);
		return FALSE;
		}
	if(startDate.GetStatus() == COleDateTime::valid &&
		endDate.GetStatus() == COleDateTime::valid &&
		startDate > endDate)
		{
		TRACE0("COXCalendar:::SetDateRange: incorrect range specified, function fail!\n");
		return FALSE;
		}
	m_StartDate = startDate;
	m_EndDate = endDate;
	return TRUE;
	}

void COXCalendar::GetDateRange(COleDateTime& startDate, COleDateTime& endDate)
	{
	startDate = m_StartDate;
	endDate = m_EndDate;
	}

// return current time from edit window
COleDateTime COXCalendar::GetCurrentDate()
	{
	if(::IsWindow(m_wndEdit.m_hWnd))
		{
		m_CurrentDate.m_dt = 0;
		m_CurrentDate.SetStatus(COleDateTime::null);
		CString sTypedDate;
		m_wndEdit.GetWindowText(sTypedDate);
		//preset contained date to "no selection"
		// COleDateTime don't parse date string if it
		//contained some locale's weekday names
		//We must exclude any leading non-digit characters  
		int pos=sTypedDate.FindOneOf(_T("0123456789"));
		if(pos != -1)
			{
			COleDateTime targetDate;
			CString sParseSrc = 
				sTypedDate.Right(sTypedDate.GetLength() - pos);
			if(targetDate.ParseDateTime(sParseSrc))
				m_CurrentDate = targetDate;
			}
		}
	else
		TRACE0("COXCalendar::GetCurrentDate() called with no active edit window\n");
	return m_CurrentDate;
	}
//Get text form edit window and convert it to date value, if possible
//After this, call CWnd::OnClose() to close window
void COXCalendar::OnClose() 
	{
	GetCurrentDate();
	CWnd::OnClose();
	}

BOOL COXCalendar::SetCurrentDate(const COleDateTime &currDate)
	{
	if(currDate.GetStatus() == COleDateTime::invalid
		|| currDate < m_StartDate || currDate > m_EndDate)
		{
		TRACE0("COXCalendar::SetCurrentDate: invalid value specified!");
		ASSERT(FALSE);
		return FALSE;
		}
	m_CurrentDate = currDate;
	if(::IsWindow(m_wndEdit.m_hWnd))
		{
		CString sText;
		SetOutputText(sText);
		m_wndEdit.SetWindowText(sText);
		}
	return TRUE;
	}

void COXCalendar::PreSubclassWindow() 
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : called by the framework
	// perform initialization
{
	// TODO: Add your specialized code here and/or call the base class
	_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
	// hook not already in progress
	if(pThreadState->m_pWndInit==NULL)
	{
		VERIFY(Init());
	}

	CWnd::PreSubclassWindow();
}

void COXCalendar::ShrinkDateInBounds()
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect :check current date for valid range, correct if needed
	{
	if(m_StartDate.GetStatus() == COleDateTime::valid &&
		m_CurrentDate.GetStatus() == COleDateTime::valid &&
		m_CurrentDate < m_StartDate)
		{
		m_CurrentDate = m_StartDate;
		return;
		}
	if(m_EndDate.GetStatus() == COleDateTime::valid &&
		m_CurrentDate.GetStatus() == COleDateTime::valid &&
		m_CurrentDate > m_EndDate)
		{
		m_CurrentDate = m_EndDate;
		return;
		}

	}

void COXCalendar::SetOutputText(CString& sOutputString)
	{
	if(m_CurrentDate.GetStatus() == COleDateTime::null)
		{
		sOutputString=m_wndDatePicker.m_sNone;
		}
	else
		{
		if(m_sFormat.IsEmpty())
			{
			sOutputString=ms_straWeekDays[(m_CurrentDate.GetDayOfWeek() + 5)%7];
			sOutputString+=_T(' ') + m_CurrentDate.Format();
			}
		else
			{
			sOutputString=m_CurrentDate.Format(m_sFormat);
			}
		}
	}

void COXCalendar::OnEnChange()
	{
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect notify parent about altered text in edit control
	int id = GetDlgCtrlID();
	DWORD nCode = EN_CHANGE;
	nCode <<= 16;
	DWORD wParam = nCode | id;
	GetParent()->SendMessage(WM_COMMAND,wParam,(LPARAM)GetSafeHwnd());
	}

void COXCalendar::OnEnUpdate()
	{
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : notify parent about altered text in edit control
	int id = GetDlgCtrlID();
	DWORD nCode = EN_UPDATE;
	nCode <<= 16;
	DWORD wParam = nCode | id;
	GetParent()->SendMessage(WM_COMMAND,wParam,(LPARAM)GetSafeHwnd());
	}

void COXCalendar::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_wndDatePicker.GetSafeHwnd())
		m_wndDatePicker.DestroyWindow();
	m_font.DeleteObject();
	CWnd::PostNcDestroy();
}
