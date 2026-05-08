// ==========================================================================
//					Class Implementation : COXStaticText
// ==========================================================================

// Source file :		OXStaticText.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXStaticText.h"

#include "UTBStrOp.h"

double __cdecl absolute(double a)
{
	return ((a<0)?(-a):a)+0.5;
}

/////////////////////////////////////////////////////////////////////////////
// Data	members	-------------------------------------------------------------
// public:

// protected:

//	int					m_nHorzAlignment;
//	---	Text horizontal	alignment: left, center, right

//	int					m_nVertAlignment;
//	---	Text vertical alignment: top, center, bottom

//	BOOL		m_bDottedEdge;
//	---	Indicates whether the window edge is dotted.

//	DWORD		m_dwBkColor;
//	---	Text background	color.

//	DWORD		m_dwMinTimeOut;
//	---	Minimum	time out interval for text scrolling.

//	DWORD		m_dwOffset;
//	---	Text 3D	offset.

//	DWORD		m_dwScrollSpeed;
//	---	Text scrolling speed (pixels/second).

//	DWORD		m_dwScrollTimeOut;
//	---	Text scrolling time	out	period (ms).

//	DWORD		m_dwTextColor;
//	---	Text color.

//	double		m_dXDelta;
//	---	Exact text scrolling x-increment.

//	double		m_dXExactDisplacement;
//	---	Exact text scrolling x-displacement.

//	double		m_dYDelta;
//	---	Exact text scrolling y-increment.

//	double		m_dYExactDisplacement;
//	---	Exact text scrolling y-displacement.

//	LOGFONT		m_LogFont;
//	---	Structure defines the attributes of	a font.

//	int			m_nEllipseMode;
//	---	Ellipses replacing mode.

//	int			m_nGraphicsMode;
//	---	Current	graphics mode.

//	int			m_nScrollAmount;
//	---	Text scrolling amount.

//	int			m_nScrollDirection;
//	---	Text scrolling direction (degrees).

//	int			m_nXCastDisplacement;
//	---	Text scrolling cast	x-displacement.

//	int			m_nXDisplacement;
//	---	Text scrolling x-displacement.

//	int			m_nYCastDisplacement;
//	---	Text scrolling cast	y-displacement.

//	int			m_nYDisplacement;
//	---	Text scrolling y-displacement.

//	CEvent*		m_pEventLoop;
//	---	When pointed event is signaled,	text scrolling thread terminates.

//	CCriticalSection*		m_pCritSecRedrawWait;
//	---	Pointed	critical section is	locked during window redrawing;

//	CFont*		m_pObjFont;
//	---	Pointer	to the font	object.

//	CSingleLock*	m_pRedrawThreadLock;
//	---	This object	locks window redrawing in the special redraw thread.

//	CWinThread*		m_pScrollingThread;
//	---	Points to CWinThread object	that represents	text scrolling thread.

//	CString		m_sText;
//	---	Text string.

//	CString		m_sTextNarrow;
//	---	Narrow text	string (with ellipses so that the result fits in the specified rectangle).

// private:

UINT COXStaticText::m_nPrepareBitmap=
RegisterWindowMessage(_T("_OXPREPARE_BITMAP_"));

/////////////////////////////////////////////////////////////////////////////
// Member functions	---------------------------------------------------------
// public:

COXStaticText::COXStaticText(DWORD dwOffset, int nGraphicsMode,	
							 int nHorzAlignment, int nVertAlignment) : m_BMP(NULL)
{
	m_bAllowRefresh = FALSE;
	m_dwCurrentTickDelta = 0;
	m_dwLastTickDelta = 0;

	m_nHorzAlignment = nHorzAlignment;
	m_nVertAlignment = nVertAlignment;

	m_bDottedEdge =	FALSE;

	m_bEmbossText =	FALSE;
	m_bEmbossRaised	= FALSE;
	m_clrEmbossHighLight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrEmbossShadow =	::GetSysColor(COLOR_BTNSHADOW);

	m_dwBkColor	= ::GetSysColor(COLOR_WINDOW);
	m_dwMinTimeOut = 10;
	m_dwOffset = dwOffset;
	m_dwScrollSpeed	= m_dwScrollTimeOut	= 0;
	m_dwTextColor =	::GetSysColor(COLOR_WINDOWTEXT);

	m_dXDelta =	m_dYDelta =	0.0;

	::GetObject(::GetStockObject(ANSI_VAR_FONT), sizeof(m_LogFont),	&m_LogFont);

	m_nEllipseMode = OX_NO_ELLIPSES;
	m_nGraphicsMode	= nGraphicsMode;
	m_nScrollAmount	= m_nScrollDirection = 0;
	m_nXDisplacement = m_nYDisplacement	= m_nXCastDisplacement = m_nYCastDisplacement =	0;

	m_pEventLoop = new CEvent(FALSE, TRUE);
	m_pCritSecRedrawWait = new CCriticalSection();

	m_pObjFont = new CFont;
	m_pObjFont->CreateFontIndirect(&m_LogFont);

	m_pScrollingThread = NULL;

	m_sText	= m_sTextNarrow	= _T("");

	m_szGapSize=CSize(0,0);

	m_rectViewMargins=CRect(2,0,2,0);
}

BOOL COXStaticText::Create(LPCTSTR lpszText, DWORD dwStyle,	const RECT&	rect,
						   CWnd* pParentWnd, UINT nID /* = 0xffff */)
{
	// Look	for	special	CStatic	control	styles and trace if	any.
#ifdef _DEBUG	
	if ((dwStyle & ~(WS_CHILD |	WS_VISIBLE | WS_DISABLED | WS_BORDER)) != 0)
		TRACE0("in COXStaticText::Create : dwStyle contains	more styles	than permitted.	They will be ignored\n");
#endif

	dwStyle&=WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_BORDER;
	return CStatic::Create(lpszText, dwStyle, rect,	pParentWnd,	nID);
}

COXStaticText::~COXStaticText()
{
	m_pObjFont->DeleteObject();
	delete m_pObjFont;

	delete m_pCritSecRedrawWait;
	delete m_pEventLoop;

	delete m_BMP;
}

BOOL COXStaticText::SetHorzAlignment(int nAlignment, BOOL bPrepareNow /* = FALSE */)
{
	if(nAlignment!=OX_ALIGNHORZ_LEFT &&	nAlignment!=OX_ALIGNHORZ_CENTER	&& 
		nAlignment!=OX_ALIGNHORZ_RIGHT)
	{
		return FALSE;
	}
	m_nHorzAlignment = nAlignment;
	return RestoreTextPos(bPrepareNow);
}

BOOL COXStaticText::SetVertAlignment(int nAlignment, BOOL bPrepareNow /* = FALSE */)
{
	if(nAlignment!=OX_ALIGNVERT_TOP	&& nAlignment!=OX_ALIGNVERT_CENTER && 
		nAlignment!=OX_ALIGNVERT_BOTTOM)
	{
		return FALSE;
	}
	m_nVertAlignment = nAlignment;
	return RestoreTextPos(bPrepareNow);
}

BOOL COXStaticText::SetBkColor(COLORREF	dwBkColor, BOOL	bPrepareNow	/* = FALSE */)
{
	m_dwBkColor	= dwBkColor;
	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::SetTextColor(COLORREF dwTextColor, BOOL	bPrepareNow	/* = FALSE */)
{
	m_dwTextColor =	dwTextColor;
	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::SetEmboss(BOOL bEmboss /* =	TRUE */, BOOL bRaised /* = FALSE */, BOOL bPrepareNow /* = FALSE */,
							  COLORREF	clrHLight /* = ::GetSysColor(COLOR_BTNHIGHLIGHT) */,
							  COLORREF	clrShadow /* = ::GetSysColor(COLOR_BTNSHADOW) */)
{
	m_bEmbossText =	bEmboss;
	m_bEmbossRaised	= bRaised;

	m_clrEmbossHighLight = clrHLight;
	m_clrEmbossShadow =	clrShadow;

	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::SetWindowText(LPCTSTR psText, BOOL bPrepareNow /* =	FALSE */)
{
	m_sText	= m_sTextNarrow	= psText;
	m_bAllowRefresh = FALSE;
	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::SetGraphicsMode(int	nGraphicsMode, BOOL	bPrepareNow	/* = FALSE */)
{
	m_nGraphicsMode	= nGraphicsMode;
	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::SetEllipseMode(int nEllipseMode, BOOL bPrepareNow /* = FALSE */)
{
	if ( GetEllipseMode() == nEllipseMode )
		m_nEllipseMode = OX_NO_ELLIPSES;
	else
		m_nEllipseMode = nEllipseMode;

	return PrepareBitmap(bPrepareNow);
}

BOOL COXStaticText::Set3Doffset(DWORD dwOffset,	BOOL bPrepareNow /*	= FALSE	*/)
{
	BOOL bSuccess =	FALSE;

	if ( dwOffset <= OX_MAX_3DOFFSET )
	{
		m_dwOffset = dwOffset;
		bSuccess = PrepareBitmap(bPrepareNow);
	}

	return bSuccess;
}

BOOL COXStaticText::SetFontAttr(int	nAttr, BOOL	bSet /*	= TRUE */, BOOL	bPrepareNow	/* = FALSE */)
{
	switch ( nAttr )
	{
	case OX_BOLD_FONT:
		m_LogFont.lfWeight = bSet ?	FW_BOLD	: FW_NORMAL;
		break;
	case OX_ITALIC_FONT:
		m_LogFont.lfItalic = (BYTE)bSet;
		break;
	case OX_UNDERLINED_FONT:
		m_LogFont.lfUnderline =	(BYTE)bSet;
		break;
	case OX_STRIKED_OUT_FONT:
		m_LogFont.lfStrikeOut =	(BYTE)bSet;
		break;
	}

	BOOL bSuccess =	RebuildFont();
	if (bSuccess)
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::IsBold() const
{
	if ( m_LogFont.lfWeight	== FW_BOLD )
		return TRUE;
	else
		return FALSE;
}

BOOL COXStaticText::SetStringAngle(int nAngle, BOOL	bPrepareNow	/* = FALSE */)
{
	m_LogFont.lfEscapement = nAngle;

	BOOL bSuccess =	RebuildFont();
	if ( bSuccess )
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::SetCharAngle(int nAngle, BOOL bPrepareNow /* = FALSE */)
{
	m_LogFont.lfOrientation	= nAngle;

	BOOL bSuccess =	RebuildFont();
	if ( bSuccess )
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::SetCharSet(int nCharSet, BOOL bPrepareNow /* = FALSE */)
{
	m_LogFont.lfCharSet	= (BYTE)nCharSet;

	BOOL bSuccess =	RebuildFont();
	if ( bSuccess )
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::SetFontHeight(int nHeight, BOOL	bPrepareNow	/* = FALSE */)
{
	m_LogFont.lfHeight = nHeight;

	BOOL bSuccess =	RebuildFont();
	if ( bSuccess )
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::SetFontWidth(int nWidth, BOOL	bPrepareNow	/* = FALSE */)
{
	m_LogFont.lfWidth = nWidth;

	BOOL bSuccess =	RebuildFont();
	if ( bSuccess )
		bSuccess = PrepareBitmap(bPrepareNow);
	return bSuccess;
}

BOOL COXStaticText::SetFontName(LPCTSTR	sName, BOOL	bPrepareNow	/* = FALSE */)
{
	BOOL bSuccess =	FALSE;

	if ( _tcslen(sName)	<= LF_FACESIZE )
	{
		UTBStr::tcscpy(m_LogFont.lfFaceName, LF_FACESIZE, sName);

		bSuccess = RebuildFont();
		if ( bSuccess )
			bSuccess = PrepareBitmap(bPrepareNow);
	}

	return bSuccess;
}

// returns TRUE	if succeed and sets	plf	to the LOGINFO of the font used	to draw	text,
// otherwise FALSE and plf is undefined
BOOL COXStaticText::GetLogFont(LOGFONT*	plf) const
{
	if (m_pObjFont==NULL) 
	{
		return FALSE;
	}
	return m_pObjFont->GetObject(sizeof(*plf),plf);
}

// returns TRUE	if succeed and creates font	used to	draw text 
// from	plf, otherwise FALSE
BOOL COXStaticText::SetLogFont(LOGFONT*	plf, BOOL bPrepareNow/*	= FALSE*/)
{
	CFont font;
	BOOL bSuccess=font.CreateFontIndirect(plf);
	if(bSuccess)
	{
		bSuccess = font.GetObject(sizeof(m_LogFont),&m_LogFont);
		if(bSuccess)
		{
			bSuccess = RebuildFont();
			if(bSuccess)
			{
				bSuccess = PrepareBitmap(bPrepareNow);
			}
		}	
	}

	return bSuccess;
}

BOOL COXStaticText::SetPlainBorder(BOOL	bSet /*	= TRUE */)
{
	if (bSet)
	{
		m_bDottedEdge =	FALSE;
		// This	odd	line is	to force the NC	area to	be invalidated
		ModifyStyleEx(0, WS_EX_STATICEDGE |	WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 0);

		ModifyStyle(0, WS_BORDER, 0);
		ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_CLIENTEDGE |	WS_EX_WINDOWEDGE, 0, SWP_FRAMECHANGED);
	}
	else
		ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);

	return TRUE;
}

BOOL COXStaticText::SetStaticEdge(BOOL bSet	/* = TRUE */)
{
	if (bSet)
	{
		m_bDottedEdge =	FALSE;
		ModifyStyle(0, WS_BORDER, 0);
		ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
	}
	else
		ModifyStyleEx(WS_EX_STATICEDGE,	0, SWP_FRAMECHANGED);

	return TRUE;
}

BOOL COXStaticText::SetClientEdge(BOOL bSet	/* = TRUE */)
{
	if (bSet)
	{
		m_bDottedEdge =	FALSE;
		ModifyStyle(0, WS_BORDER, 0);
		ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_WINDOWEDGE, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	}
	else
		ModifyStyleEx(WS_EX_CLIENTEDGE,	0, SWP_FRAMECHANGED);

	return TRUE;
}

BOOL COXStaticText::SetRaisedEdge(BOOL bSet	/* = TRUE */)
{
	if (bSet)
	{
		m_bDottedEdge =	FALSE;
		ModifyStyle(0, WS_BORDER, 0);
		ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_CLIENTEDGE, WS_EX_WINDOWEDGE, SWP_FRAMECHANGED);
	}
	else
		ModifyStyleEx(WS_EX_WINDOWEDGE,	0, SWP_FRAMECHANGED);

	return TRUE;
}

BOOL COXStaticText::SetDottedEdge(BOOL bSet	/* = TRUE */)
{
	m_bDottedEdge =	bSet;
	if (bSet)
	{
		// This	odd	line is	to force the NC	area to	be invalidated
		ModifyStyleEx(0, WS_EX_STATICEDGE |	WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE, 0);

		ModifyStyle(0, WS_BORDER, 0);
		ModifyStyleEx(WS_EX_STATICEDGE | WS_EX_CLIENTEDGE |	WS_EX_WINDOWEDGE, 0, SWP_FRAMECHANGED);
	}

	return TRUE;
}

void COXStaticText::SetScrollDirection(int nDirection, BOOL	bPrepareNow	/* = FALSE */)
{
	CSingleLock	RedrawLock(m_pCritSecRedrawWait);
	RedrawLock.Lock();

	// Correct direction angle to keep it in [0	- 360] interval.
	nDirection %= 360;
	if ( nDirection	< 0	)
		nDirection += 360;

	m_nScrollDirection = nDirection;
	RestoreTextPos(bPrepareNow);
	ScrollAmountRecalc();

	RedrawLock.Unlock();
}

void COXStaticText::SetScrollSpeed(DWORD dwScrollSpeed)
{
	CSingleLock	RedrawLock(m_pCritSecRedrawWait);
	RedrawLock.Lock();

	DWORD dwTimeOut, dwAmount;
	SpeedCalc(dwScrollSpeed, &dwTimeOut, &dwAmount);

	m_dwScrollSpeed	= dwScrollSpeed;
	m_dwScrollTimeOut =	dwTimeOut;
	m_nScrollAmount	= dwAmount;
	ScrollAmountRecalc();

	RedrawLock.Unlock();
}

void COXStaticText::StartScrolling(BOOL	bStart)
{
	if(GetSafeHwnd()==NULL)
		return;

	if ( IsScrollingStarted() )
	{
		m_pEventLoop->SetEvent();

		DWORD	dwWaitResult;
		BOOL	bEnd = FALSE;
		BOOL	bPostQuit =	FALSE;

		while (!bEnd)
		{
			dwWaitResult = MsgWaitForMultipleObjects(1,	&m_pScrollingThread->m_hThread,	FALSE, INFINITE, QS_ALLINPUT);

			if (dwWaitResult ==	(WAIT_OBJECT_0 + 1))
			{
				MSG	msg;
				// making sure there is	a msg in the queue
				// we don't	want PumpMessage() to hang
				if (PeekMessage(&msg, NULL,	0, 0, PM_NOREMOVE))
					bPostQuit =	(!AfxGetThread()->PumpMessage()) ||	bPostQuit;
			}
			else
			{
				//if (dwWaitResult != WAIT_OBJECT_0)
				//	  TerminateThread(h_thread,	0);
				bEnd = TRUE;
			}
		}
		if (bPostQuit)
		{
			AfxPostQuitMessage(0);
			Sleep(200);	// Give	the	terminating	thread the time	to terminate
		}

		m_pEventLoop->ResetEvent();
		delete m_pScrollingThread;
		m_pScrollingThread = NULL;
	}

	if ( bStart	)
	{
		m_pScrollingThread = AfxBeginThread((AFX_THREADPROC)TextScrollingThreadFunction, this);
		m_pScrollingThread->m_bAutoDelete =	FALSE;
	}
}

BOOL COXStaticText::IsScrollingStarted() const
{
	if ( m_pScrollingThread	&&
		::WaitForSingleObject(m_pScrollingThread->m_hThread, 0) ==	WAIT_TIMEOUT )
		return TRUE;
	else
		return FALSE;
}

void COXStaticText::SetMinTimeOut(DWORD	dwMinTimeOut)
{
	if ( dwMinTimeOut != m_dwMinTimeOut	&& dwMinTimeOut	>= 1 &&	dwMinTimeOut <=	500	)
	{
		DWORD dwScrollSpeed	= GetScrollSpeed();
		m_dwMinTimeOut = dwMinTimeOut;
		SetScrollSpeed(dwScrollSpeed);
	}
}

void COXStaticText::SetGapSize(CSize& szGapSize, BOOL bPrepareNow /* = FALSE */)
{
	m_szGapSize	= szGapSize;

	PrepareBitmap(bPrepareNow);
}

void COXStaticText::SetViewMargins(CRect& rectViewMargins, BOOL	bPrepareNow	/* = FALSE */)
{
	m_rectViewMargins =	rectViewMargins;

	PrepareBitmap(bPrepareNow);
}


BOOL COXStaticText::RestoreTextPos(BOOL	bPrepareNow	/* = TRUE */)
{
	CRect textRect;
	GetClientRect(&textRect);


	GetInitialDisplacement(textRect);

	return PrepareBitmap(bPrepareNow);
}

BEGIN_MESSAGE_MAP(COXStaticText, CStatic)
	//{{AFX_MSG_MAP(COXStaticText)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_REGISTERED_MESSAGE(m_nPrepareBitmap, OnPrepareBitmap)
END_MESSAGE_MAP()

// protected:
void COXStaticText::OnNcPaint()
{
	if (!m_bDottedEdge)
	{
		Default();
		return;
	}

	// Prepare for drawing the non-client area with	a dotted border
	CWindowDC dc(this);
	CRect rect;
	GetWindowRect(&rect);

	rect.OffsetRect(-rect.left,	-rect.top);

	CRect Temp;
	GetClientRect(&Temp);

	CPen DottedPen(PS_DOT, 1, RGB(0, 0,	0));
	CBrush HollowBrush;
	HollowBrush.CreateStockObject(HOLLOW_BRUSH);

	CPen* pOldPen =	dc.SelectObject(&DottedPen);
	CBrush*	pOldBrush =	dc.SelectObject(&HollowBrush);
	dc.Rectangle(rect);

	if (pOldBrush != NULL)
		dc.SelectObject(pOldBrush);

	if (pOldPen	!= NULL)
		dc.SelectObject(pOldPen);
}

void COXStaticText::OnSize(UINT	nType, int cx, int cy)
{
	CStatic::OnSize(nType,cx,cy);
	if(IsScrollingStarted())
		StartScrolling(TRUE);
	PrepareBitmap(TRUE);
}

UINT COXStaticText::TextScrollingThreadFunction(LPVOID pParam)
// --- In :			pParam :	Pointer	to this	COXStaticText object. It makes possible	to
//								work with non-static class members in this static function.
// --- Out :
// --- Returns :	Exit code =	0.
// --- Effect :		Controlling	function for text scrolling	worker thread.
{
	COXStaticText*		pStaText = (COXStaticText*)pParam;
	CSingleLock		LoopLock(pStaText->m_pEventLoop), RedrawLock(pStaText->m_pCritSecRedrawWait);
	BOOL			bRedraw;

	pStaText->PrepareBitmap(TRUE);

	CRect textRect;
	pStaText->GetClientRect(&textRect);
	///
	textRect+=pStaText->m_szGapSize;
	///

	while (TRUE)
	{
		if(LoopLock.Lock(pStaText->m_dwScrollTimeOut))
			return 0;

		bRedraw	= FALSE;

		RedrawLock.Lock();

		double	nXDisplacement = pStaText->m_nXDisplacement,
			nYDisplacement = pStaText->m_nYDisplacement;

		pStaText->m_nXDisplacement = (pStaText->m_dXDelta +	pStaText->m_nXDisplacement);
		pStaText->m_nYDisplacement = (pStaText->m_dYDelta +	pStaText->m_nYDisplacement);

		if ( pStaText->m_nXDisplacement	!= nXDisplacement || 
			pStaText->m_nYDisplacement != nYDisplacement )
		{
			if(pStaText->m_nXDisplacement-pStaText->m_szTextSize.cx/2>=textRect.right || 
				pStaText->m_nXDisplacement+pStaText->m_szTextSize.cx/2<0 ||	
				pStaText->m_nYDisplacement-pStaText->m_szTextSize.cy/2>=textRect.bottom	|| 
				pStaText->m_nYDisplacement+pStaText->m_szTextSize.cy/2<0) 
			{
				pStaText->m_nXDisplacement += pStaText->m_nXCastDisplacement;
				pStaText->m_nYDisplacement += pStaText->m_nYCastDisplacement;

				if(pStaText->m_nXDisplacement-pStaText->m_szTextSize.cx/2>=textRect.right || 
					pStaText->m_nXDisplacement+pStaText->m_szTextSize.cx/2<0 ||	
					pStaText->m_nYDisplacement-pStaText->m_szTextSize.cy/2>=textRect.bottom	|| 
					pStaText->m_nYDisplacement+pStaText->m_szTextSize.cy/2<0) 
				{
					pStaText->m_nXDisplacement -= pStaText->m_nXCastDisplacement;
					pStaText->m_nYDisplacement -= pStaText->m_nYCastDisplacement;
				}
			}

			bRedraw	= TRUE;
		}


		RedrawLock.Unlock();

		if (bRedraw)
			pStaText->RedrawWindow();
	}

	return 0;
}

void COXStaticText::EmbossText(CDC*	pMemDC,	RECT BmpRect)
{
	const DWORD	PSDPxax	= 0x00B8074A;

	COLORREF clrShadow = m_clrEmbossShadow;
	COLORREF clrHighlight =	m_clrEmbossHighLight;
	if(!m_bEmbossRaised)
	{
		// Swap	the	highlight and shadow color
		clrShadow =	m_clrEmbossHighLight;
		clrHighlight = m_clrEmbossShadow;
	}

	// We create two monochrome	bitmaps. One of	them will contain the
	// highlighted edge	and	the	other will contain the shadow. These
	// bitmaps are then	used to	paint the highlight	and	shadow on the
	// background image.
	CDC* pDC = GetDC();

	// Create compatible DCs
	CDC	MonoDC;
	MonoDC.CreateCompatibleDC(pDC);
	CDC* pMonoDC = &MonoDC;

	int	BmpWidth = BmpRect.right - BmpRect.left;
	int	BmpHeight =	BmpRect.bottom - BmpRect.top;
	// Create the monochrome and compatible	color bitmaps 
	CBitmap	bmShadow;
	bmShadow.CreateBitmap(BmpWidth,	BmpHeight, 1, 1, NULL);
	CBitmap	bmHighlight;
	bmHighlight.CreateBitmap(BmpWidth, BmpHeight, 1, 1,	NULL);

	// Set background color	of bitmap for mono conversion
	// We assume that the pixel	in the top left	corner has the background color
	COLORREF oldBkColor=CLR_NONE;
	if(m_dwBkColor!=CLR_NONE)
		oldBkColor = pMemDC->SetBkColor(pMemDC->GetNearestColor(m_dwBkColor));

	// Create the highlight	bitmap.
	CBitmap* pbmOldHighlight = pMonoDC->SelectObject(&bmHighlight);
	pMonoDC->PatBlt(0, 0, BmpWidth,	BmpHeight, WHITENESS );
	pMonoDC->BitBlt(0, 0, BmpWidth - 1,	BmpHeight -	1, pMemDC, 1, 1, SRCCOPY );
	pMonoDC->BitBlt(0, 0, BmpWidth,	BmpHeight, pMemDC, BmpRect.left, BmpRect.top, MERGEPAINT );
	pbmOldHighlight	= pMonoDC->SelectObject(pbmOldHighlight);

	// create the shadow bitmap
	CBitmap* pbmOldShadow =	pMonoDC->SelectObject(&bmShadow);
	pMonoDC->PatBlt(0, 0, BmpWidth,	BmpHeight, WHITENESS );
	pMonoDC->BitBlt(0, 0, BmpWidth - 1,	BmpHeight -	1, pMemDC, 1, 1, SRCCOPY );
	pMonoDC->BitBlt(0, 0, BmpWidth,	BmpHeight, pMemDC, BmpRect.left, BmpRect.top, MERGEPAINT );
	pbmOldShadow = pMonoDC->SelectObject(pbmOldShadow);

	// Now let's start working on the final	image
	// Set the background and foreground color for the raster operations
	pMemDC->SetBkColor(pMemDC->GetNearestColor(RGB(255,255,255)));
	pMemDC->SetTextColor(pMemDC->GetNearestColor(RGB(0,0,0)));

	// blt the highlight edge
	CBrush brHighPat(pMemDC->GetNearestColor(clrHighlight));
	CBrush*	pOldbrHighPat =	pMemDC->SelectObject(&brHighPat);
	pbmOldHighlight	= pMonoDC->SelectObject(pbmOldHighlight);
	pMemDC->BitBlt(BmpRect.left, BmpRect.top, BmpWidth,	BmpHeight, pMonoDC,	0, 0, PSDPxax);
	pMemDC->SelectObject(pOldbrHighPat);
	pMonoDC->SelectObject(pbmOldHighlight);

	// blt the shadow edge
	CBrush brShwPat(clrShadow);
	CBrush*	pOldbrShwPat = pMemDC->SelectObject(&brShwPat);
	pbmOldShadow = pMonoDC->SelectObject(pbmOldShadow);
	pMemDC->BitBlt(BmpRect.left, BmpRect.top, BmpWidth,	BmpHeight, pMonoDC,	0, 0, PSDPxax);
	pMemDC->SelectObject(pOldbrShwPat);
	pMonoDC->SelectObject(pbmOldShadow);

	bmShadow.DeleteObject();
	bmHighlight.DeleteObject();

	ReleaseDC(pDC);

	pMemDC->SetBkColor(oldBkColor);
}

void COXStaticText::TextOutput(CDC*	pMemDC,	RECT rect)
// --- In :			pMemDC :	Pointer	to compatible memory device	context	to text	output.
//					rect :		Text output	rectangle.
// --- Out :
// --- Returns :
// --- Effect :		Performs text output to	the	compatible memory device context.
{
	rect.left+=m_rectViewMargins.left;
	rect.top+=m_rectViewMargins.top;
	rect.right-=m_rectViewMargins.right;
	rect.bottom-=m_rectViewMargins.bottom;

	TEXTMETRIC tm;
	pMemDC->GetTextMetrics(&tm);
	double alpha = ((GetGraphicsMode()==GM_ADVANCED) ? 
		GetCharAngle() : GetStringAngle())/10;
	alpha =	alpha *	3.1415926/180.0;
	double alphaAdvanced = GetStringAngle()/10;
	alphaAdvanced =	alphaAdvanced *	3.1415926/180.0;

	CSize sizeCorr(0,0);
	sizeCorr.cx	= (tm.tmAscent - tm.tmDescent)/2;
	sizeCorr.cy	= (tm.tmAscent - tm.tmDescent)/2;
	sizeCorr.cx	= (int)(((double)sizeCorr.cx)*sin(alpha)+(sin(alpha)>0 ? -0.5 :	0.5));
	sizeCorr.cy	= (int)(((double)sizeCorr.cy)*cos(alpha)+(cos(alpha)>0 ? -0.5 :	0.5));
	if(GetGraphicsMode()==GM_ADVANCED)
	{
	}

	pMemDC->SetTextAlign(TA_CENTER | TA_BASELINE | TA_NOUPDATECP);
	pMemDC->SetTextColor(pMemDC->GetNearestColor(RGB(GetRValue(m_dwTextColor)/2, GetGValue(m_dwTextColor)/2, GetBValue(m_dwTextColor)/2)));
	for	( DWORD	dwCount	= 0; dwCount < m_dwOffset; dwCount++ )
	{
		pMemDC->TextOut( (rect.left+rect.right)/2+sizeCorr.cx, 
			(rect.top+rect.bottom)/2+sizeCorr.cy, m_sTextNarrow);
		CRect rectOutput((rect.left+rect.right)/2+sizeCorr.cx,
			(rect.top+rect.bottom)/2+sizeCorr.cy,
			(rect.left+rect.right)/2+sizeCorr.cx+rect.right-rect.left,
			(rect.top+rect.bottom)/2+sizeCorr.cy+rect.bottom-rect.top);
		ScreenToClient(rectOutput);
		pMemDC->DrawText(m_sTextNarrow,	&rectOutput,DT_SINGLELINE);
		OffsetRect(	&rect, 1,1);
	}

	pMemDC->SetTextColor(pMemDC->GetNearestColor(m_dwTextColor));
	pMemDC->TextOut( (rect.left+rect.right)/2+sizeCorr.cx, 
		(rect.top+rect.bottom)/2+sizeCorr.cy, m_sTextNarrow);
	if (m_bEmbossText)
		EmbossText(pMemDC, rect);
}

void COXStaticText::EllipsesReplace(CDC* pMemDC, LPRECT	lpRect)
// --- In :			pMemDC :	Pointer	to compatible memory device	context	to text	output.
//					lpRect :	Pointer	to the text	output rectangle.
// --- Out :
// --- Returns :
// --- Effect :		If it's	necessary, replaces	text with ellipses so that
//					the	result fits	in the specified rectangle
{
	BOOL		bContinue =	TRUE;
	int			nRemoved = 0, nRem1	= 0, nRem2 = 0;
	CString		sEllipses =	_T("...");
	CSize		textSize;

	m_sTextNarrow =	m_sText;

	int	nLimit=lpRect->right-(int)(lpRect->right*1/4*
		sin((double)(GetStringAngle()/10)*OX_PI/180.0));

	while (	bContinue && m_sText.GetLength() > nRemoved	&&
		::GetTextExtentPoint32(pMemDC->m_hDC, (LPCTSTR)m_sTextNarrow, 
		m_sTextNarrow.GetLength(), &textSize) )
	{
		if(textSize.cx>=nLimit)
		{
			nRemoved++;

			switch(m_nEllipseMode)
			{
			case OX_BEGIN_ELLIPSES:
				m_sTextNarrow =	sEllipses +	m_sText.Right(m_sText.GetLength() -	
					nRemoved);
				break;
			case OX_MIDDLE_ELLIPSES:
				nRem1 =	m_sText.GetLength()	- nRemoved;
				if ( nRem1 % 2 )
				{
					nRem1 /= 2;
					nRem2 =	nRem1 +	1;
				}
				else
					nRem2 =	nRem1 /= 2;
				m_sTextNarrow =	m_sText.Left(nRem1)	+ sEllipses	+ 
					m_sText.Right(nRem2);
				break;
			case OX_END_ELLIPSES:
				m_sTextNarrow =	m_sText.Left(m_sText.GetLength() - nRemoved) + 
					sEllipses;
				break;
			default:
				bContinue =	FALSE;
				break;
			}
		}
		else
			bContinue =	FALSE;
	}
}

void COXStaticText::ScrollAmountRecalc()
// --- In :
// --- Out :
// --- Returns :
// --- Effect :		Calculates the text	scrolling x- and y-	offsets.
{
	double dRadDirection = (double)m_nScrollDirection *	OX_PI /	180.0;

	m_dXDelta =	(double)m_nScrollAmount	* cos(dRadDirection);
	m_dYDelta =	-(double)m_nScrollAmount * sin(dRadDirection);

	RECT textRect;
	GetClientRect(&textRect);

	textRect.right = __max(	textRect.right,	m_szTextSize.cx)+m_szGapSize.cx;
	textRect.bottom	= __max( textRect.bottom, m_szTextSize.cy)+m_szGapSize.cy;

	double angle = atan( ((double)textRect.bottom)/((double)textRect.right));
	angle =	(angle*180.0)/OX_PI;

	if ( m_nScrollDirection	<= angle )
	{
		m_nXCastDisplacement = -textRect.right;
		m_nYCastDisplacement = ((double)textRect.right * tan(dRadDirection));
	}
	else if	( m_nScrollDirection > angle &&	m_nScrollDirection <= 90 )
	{
		m_nXCastDisplacement = -((double)textRect.bottom / tan(dRadDirection));
		m_nYCastDisplacement = textRect.bottom;
	}
	else if	( m_nScrollDirection > 90 && m_nScrollDirection	<= 180 - angle )
	{
		m_nXCastDisplacement = ((double)textRect.bottom	/ tan(OX_PI	- dRadDirection));
		m_nYCastDisplacement = textRect.bottom;
	}
	else if	( m_nScrollDirection > 180-angle &&	m_nScrollDirection <= 180 )
	{
		m_nXCastDisplacement = textRect.right;
		m_nYCastDisplacement = ((double)textRect.right * tan(OX_PI - dRadDirection));
	}
	else if	( m_nScrollDirection > 180 && m_nScrollDirection <=	180	+ angle	)
	{
		m_nXCastDisplacement = textRect.right;
		m_nYCastDisplacement = -((double)textRect.right	* tan(dRadDirection));
	}
	else if	( m_nScrollDirection > 180+angle &&	m_nScrollDirection <= 270 )
	{
		m_nXCastDisplacement = ((double)textRect.bottom	/ tan(dRadDirection));
		m_nYCastDisplacement = -textRect.bottom;
	}
	else if	( m_nScrollDirection > 270 && m_nScrollDirection <=	360-angle )
	{
		m_nXCastDisplacement = -((double)textRect.bottom / tan(OX_PI - dRadDirection));
		m_nYCastDisplacement = -textRect.bottom;
	}
	else
	{
		m_nXCastDisplacement = -textRect.right;
		m_nYCastDisplacement = -((double)textRect.right	* tan(OX_PI	- dRadDirection));
	}
}

BOOL COXStaticText::RebuildFont()
// --- In :
// --- Out :
// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
// --- Effect :		Rebuilds the font object.
{
	if((HFONT)*m_pObjFont!=NULL)
		m_pObjFont->DeleteObject();
	return m_pObjFont->CreateFontIndirect(&m_LogFont);
}

BOOL COXStaticText::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void COXStaticText::OnPaint()
// --- In :
// --- Out :
// --- Returns :
// --- Effect :		The	framework calls	this member	function when Windows or an	application
//					makes a	request	to repaint a portion of	an application's window.
{
	CSingleLock	RedrawLock(m_pCritSecRedrawWait);
	RedrawLock.Lock();

	m_dwBeginTickCount = ::GetTickCount();

	CPaintDC	dc(this);	// Device context for painting.

	CDC			memDC;		// Compatible memory device	context.
	CRect		textRect;	// Client coordinates of the window	client area.
	CDC			drawDC;
	CBitmap		drawBMP;

	GetClientRect(&textRect);

	if (!memDC.CreateCompatibleDC(&dc)) // Create compatible memory	device context
	{
		TRACE(_T("OnPaint() - memDC.CreateCompatibleDC() failed\n"));
		RedrawLock.Unlock();
		return;
	}
	int	nSavedDC=memDC.SaveDC();
	memDC.SelectObject(m_BMP);

	if (!drawDC.CreateCompatibleDC(&dc))	// Create compatible memory	device context
	{
		RedrawLock.Unlock();
		return;
	}
	int	nSavedDC2=drawDC.SaveDC();
	if (drawBMP.CreateCompatibleBitmap(	&dc, textRect.Width(), textRect.Height()))
	{
		CBitmap* pOldDrawBitmap	= drawDC.SelectObject(&drawBMP);

		if(m_dwBkColor!=CLR_NONE)
			drawDC.FillSolidRect(&textRect,	drawDC.GetNearestColor(m_dwBkColor));
		else
			SendMessage(WM_ERASEBKGND,(WPARAM)((HDC)drawDC),NULL);

		drawDC.BitBlt( (int)(m_nXDisplacement-m_szTextSize.cx/2+0.5), 
			(int)(m_nYDisplacement-m_szTextSize.cy/2+0.5), m_szTextSize.cx,	
			m_szTextSize.cy, &memDC, 0,	0, SRCCOPY);

		CPoint secPt;
		if(	(m_nXDisplacement +	m_szTextSize.cx/2 >= textRect.right+m_szGapSize.cx)	|| 
			(m_nXDisplacement -	m_szTextSize.cx/2 <	textRect.left-m_szGapSize.cx) ||
			(m_nYDisplacement +	m_szTextSize.cy/2 >= textRect.bottom+m_szGapSize.cy) ||	
			(m_nYDisplacement -	m_szTextSize.cy/2 <	textRect.top-m_szGapSize.cy))
		{
			secPt.x	= (long)(m_nXDisplacement +	m_nXCastDisplacement + 0.5);
			secPt.y	= (long)(m_nYDisplacement +	m_nYCastDisplacement + 0.5);
			drawDC.BitBlt( secPt.x-m_szTextSize.cx/2, secPt.y-m_szTextSize.cy/2, 
				m_szTextSize.cx, m_szTextSize.cy, &memDC, 0, 0,	SRCCOPY);
		}

		dc.BitBlt( 0, 0, textRect.Width(), textRect.Height(), &drawDC, 0, 0, SRCCOPY);

		// Clean Up

		if (pOldDrawBitmap != NULL)
			drawDC.SelectObject(pOldDrawBitmap);
	}
	else
	{
		TRACE(_T("OnPaint() - drawDC.CreateCompatibleBitmap() failed\n"));
	}

	// Clean Up
	if(nSavedDC2)
		drawDC.RestoreDC(nSavedDC2);

	drawBMP.DeleteObject();

	if(nSavedDC)
		memDC.RestoreDC(nSavedDC);

	m_dwEndTickCount = ::GetTickCount();
	m_dwLastTickDelta = m_dwCurrentTickDelta;
	m_dwCurrentTickDelta = m_dwEndTickCount - m_dwBeginTickCount;

	if (m_bAllowRefresh 
		&& !(m_dwCurrentTickDelta == 0 && m_dwLastTickDelta == 0)
		&& (m_dwCurrentTickDelta >= (2 * m_dwLastTickDelta)
		|| m_dwLastTickDelta >= (2 * m_dwCurrentTickDelta)))
	{
		RefreshBitmap();
		m_bAllowRefresh = FALSE;
	}
	else
		m_bAllowRefresh = TRUE;

	RedrawLock.Unlock();
}

void COXStaticText::OnDestroy()
// --- In :
// --- Out :
// --- Returns :
// --- Effect :		The	framework calls	this member	function to	inform the CWnd	object that	it is
//					being destroyed. OnDestroy is called after the CWnd	object is removed from the screen.
//					This function stops	text scrolling and terminates the window redrawing thread.
{
	CStatic::OnDestroy();
	StartScrolling(FALSE);
}

LRESULT COXStaticText::OnPrepareBitmap(WPARAM wParam, LPARAM lParam)
{
	// --- In :
	// --- Out :
	// --- Returns :	 TRUE is successful
	// --- Effect :		Prepares the text bitmap for drawing
	UNREFERENCED_PARAMETER(lParam);

	BOOL bNow = (BOOL)wParam;
	if (!bNow)
		return TRUE;

	CSingleLock	RedrawLock(m_pCritSecRedrawWait);
	RedrawLock.Lock();

	CClientDC dc(this);
	if(m_dwBkColor==CLR_NONE)
		SendMessage(WM_ERASEBKGND,(WPARAM)((HDC)dc),NULL);

	CRect textRect;
	CDC	memDC;

	GetClientRect(&textRect);

	if (!memDC.CreateCompatibleDC(&dc)) // Create compatible memory	device context
	{
		TRACE(_T("OnPrepareBitmap() - memDC.CreateCompatibleDC() failed\n"));
		RedrawLock.Unlock();
		return FALSE;
	}
	int	nSavedDC=memDC.SaveDC();
	memDC.OffsetViewportOrg(0, 0);
	::SetGraphicsMode(memDC.m_hDC, m_nGraphicsMode);
	CFont* pOldFont	= memDC.SelectObject(m_pObjFont);
	memDC.SetBkMode(TRANSPARENT);

	if (m_sTextNarrow.IsEmpty())
		GetWindowText(m_sText);


	EllipsesReplace(&memDC,	&textRect);
	m_szTextSize = CalcRectSizes(&memDC);

	CRect rc(CPoint(0,0),m_szTextSize);
	m_szTextSize.cx	+= m_dwOffset;
	m_szTextSize.cy	+= m_dwOffset;
	CRect rcText( CPoint( 0, 0), m_szTextSize);

	if(m_BMP !=	NULL)
	{
		delete m_BMP;
		m_BMP=NULL;
	}
	try
	{
		m_BMP =	new	CBitmap();
	}
	catch(CMemoryException*	e)
	{
		delete e;
		m_BMP=NULL;
		TRACE(_T("OnPrepareBitmap() - m_BMP = new	CBitmap() failed\n"));
		RedrawLock.Unlock();
		return FALSE;
	}

	if(!m_BMP->CreateCompatibleBitmap( &dc,	m_szTextSize.cx, m_szTextSize.cy))		
	{
		delete m_BMP;
		m_BMP =	NULL;
		TRACE(_T("OnPrepareBitmap() - m_BMP->CreateCompatibleBitmap failed\n"));
		RedrawLock.Unlock();
		return FALSE;
	}

	CBitmap* pOldBMP= memDC.SelectObject(m_BMP);

	if(m_dwBkColor!=CLR_NONE)
		memDC.FillSolidRect(&rcText, memDC.GetNearestColor(m_dwBkColor));
	else
		SendMessage(WM_ERASEBKGND,(WPARAM)((HDC)memDC),NULL);

	TextOutput(&memDC, rc);

	if (pOldBMP	!= NULL)
		memDC.SelectObject(pOldBMP);

	if (pOldFont !=	NULL)
		memDC.SelectObject(pOldFont);

	if(nSavedDC)
		memDC.RestoreDC(nSavedDC);

	GetInitialDisplacement(textRect);

	ScrollAmountRecalc();

	RedrawLock.Unlock();
	RedrawWindow();

	return TRUE;
}

BOOL COXStaticText::RefreshBitmap()
{
	// --- In :
	// --- Out :
	// --- Returns :	 TRUE is successful
	// --- Effect :		Prepares the text bitmap for drawing


	CClientDC dc(this);
	if(m_dwBkColor==CLR_NONE)
		SendMessage(WM_ERASEBKGND,(WPARAM)((HDC)dc),NULL);

	CRect textRect;
	CDC	memDC;

	GetClientRect(&textRect);

	if (!memDC.CreateCompatibleDC(&dc)) // Create compatible memory	device context
	{
		TRACE(_T("RefreshBitmap() - memDC.CreateCompatibleDC() failed\n"));
		return FALSE;
	}
	int	nSavedDC=memDC.SaveDC();
	memDC.OffsetViewportOrg(0, 0);
	::SetGraphicsMode(memDC.m_hDC, m_nGraphicsMode);
	CFont* pOldFont	= memDC.SelectObject(m_pObjFont);
	memDC.SetBkMode(TRANSPARENT);

	if (m_sTextNarrow.IsEmpty())
		GetWindowText(m_sText);


	EllipsesReplace(&memDC,	&textRect);
	m_szTextSize = CalcRectSizes(&memDC);

	CRect rc(CPoint(0,0),m_szTextSize);
	m_szTextSize.cx	+= m_dwOffset;
	m_szTextSize.cy	+= m_dwOffset;
	CRect rcText( CPoint( 0, 0), m_szTextSize);

	if(m_BMP !=	NULL)
	{
		delete m_BMP;
		m_BMP=NULL;
	}
	try
	{
		m_BMP =	new	CBitmap();
	}
	catch(CMemoryException*	e)
	{
		delete e;
		m_BMP=NULL;
		TRACE(_T("RefreshBitmap() - m_BMP = new	CBitmap() failed\n"));
		return FALSE;
	}

	if(!m_BMP->CreateCompatibleBitmap( &dc,	m_szTextSize.cx, m_szTextSize.cy))		
	{
		delete m_BMP;
		m_BMP =	NULL;
		TRACE(_T("RefreshBitmap() - m_BMP->CreateCompatibleBitmap failed\n"));
		return FALSE;
	}

	CBitmap* pOldBMP= memDC.SelectObject(m_BMP);

	if(m_dwBkColor!=CLR_NONE)
		memDC.FillSolidRect(&rcText, memDC.GetNearestColor(m_dwBkColor));
	else
		SendMessage(WM_ERASEBKGND,(WPARAM)((HDC)memDC),NULL);

	TextOutput(&memDC, rc);

	if (pOldBMP	!= NULL)
		memDC.SelectObject(pOldBMP);

	if (pOldFont !=	NULL)
		memDC.SelectObject(pOldFont);

	if(nSavedDC)
		memDC.RestoreDC(nSavedDC);

	return TRUE;
}

LRESULT COXStaticText::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return PrepareBitmap(TRUE);
}

void COXStaticText::SpeedCalc(DWORD	dwScrollSpeed, DWORD* pdwTimeOut, DWORD* pdwAmount)
// --- In :			dwScrollSpeed -	text scrolling speed (pixels per second).
// --- Out :		pdwTimeOut -	pointer	to the scroll time out interval	(ms).
//					pdwAmount -		pointer	to the scroll amount (pixels).
// --- Returns :
// --- Effect :		Calculates optimal time	out	interval and scroll	amount values.
//					These values give amount/time_out ratio	that is	nearest	to specified speed.
{
	DWORD	dwSpeed, dwSpeedBest = dwScrollSpeed,
		dwTimeOut, dwTimeOutBest = 0,
		dwAmount, dwAmountBest = 0;

	for	( dwAmount = 1;	dwAmount < dwScrollSpeed; dwAmount++ )
	{
		for	( dwTimeOut	= m_dwMinTimeOut; dwTimeOut	< 1000 ; dwTimeOut++ )
		{
			dwSpeed	= dwScrollSpeed	- 1000 * dwAmount /	dwTimeOut;
			if ( dwSpeed < dwSpeedBest )
			{
				dwSpeedBest	= dwSpeed;
				dwTimeOutBest =	dwTimeOut;
				dwAmountBest = dwAmount;
			}
		}
	}

	if ( !dwTimeOutBest	|| !dwAmountBest ||	dwSpeedBest	> dwScrollSpeed	/ 6	)
	{
		dwTimeOutBest =	1000;
		dwAmountBest = dwScrollSpeed;
	}

	*pdwTimeOut	= dwTimeOutBest;
	*pdwAmount = dwAmountBest;
}

BOOL COXStaticText::PrepareBitmap(BOOL bNow)
// --- In :
// --- Out :
// --- Returns :	 TRUE is successful
// --- Effect :		Sends message to CWnd object to prepare the text bitmap for drawing
{
	return ((BOOL)SendMessage(m_nPrepareBitmap,(WPARAM)(bNow),NULL));
}

CSize COXStaticText::CalcRectSizes(CDC*	pDC)
{
	CSize size;
	::GetTextExtentPoint32(pDC->m_hDC, (LPCTSTR)m_sTextNarrow, 
		m_sTextNarrow.GetLength(), &size);

	double x1, x2, y1, y2;
	double alpha = ((GetGraphicsMode()==GM_ADVANCED) ? 
		GetCharAngle() : GetStringAngle())/10;
	alpha =	alpha *	3.1415926/180.0;

	x1=	size.cx	* cos(alpha);
	x2=	size.cy	* sin(alpha);

	y1=	size.cx	* sin(alpha);
	y2=	size.cy	* cos(alpha);

	CSize szRet;
	szRet.cx = (int)(absolute(x1) +	absolute(x2) + 0.5)	+ 
		m_rectViewMargins.left + m_rectViewMargins.right;
	szRet.cy = (int)(absolute(y1) +	absolute(y2) + 0.5)	+ 
		m_rectViewMargins.top +	m_rectViewMargins.bottom;

	return szRet;
}

void COXStaticText::GetInitialDisplacement(CRect& textRect)
{
	switch(m_nHorzAlignment)
	{
	case OX_ALIGNHORZ_LEFT:
		{
			m_nXDisplacement = m_szTextSize.cx/2;
			break;
		}
	case OX_ALIGNHORZ_CENTER:
		{
			m_nXDisplacement = textRect.Width()/2;
			break;
		}
	case OX_ALIGNHORZ_RIGHT:
		{
			m_nXDisplacement = textRect.Width()	- m_szTextSize.cx/2	- 1;
			break;
		}
	}
	switch(m_nVertAlignment)
	{
	case OX_ALIGNVERT_TOP:
		{
			m_nYDisplacement = m_szTextSize.cy/2;
			break;
		}
	case OX_ALIGNVERT_CENTER:
		{
			m_nYDisplacement = textRect.Height()/2;
			break;
		}
	case OX_ALIGNVERT_BOTTOM:
		{
			m_nYDisplacement = textRect.Height() - m_szTextSize.cy/2 - 1;
			break;
		}
	}
}
