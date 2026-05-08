// ==========================================================================
//					Class Implementation : COXSpinCtrl
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXSpinCtrl.h"
#include "OXMainRes.h"

/////////////////////////////////////////////////////////////////////////////
// Data members -------------------------------------------------------------
// public:
// protected:

//	BOOL	m_bEnable;
//	--- Flag that indicates whether the spin thumb is currently enabled (TRUE) or disabled (FALSE).

//	BOOL	m_bHoriz;
//	--- Flag that indicates whether the spin control has horizontal (TRUE) or vertical (FALSE) orientation.

//	BYTE	m_ucWasPressed;
//	--- Flag that indicates which spin button - up (1), down (2) or thumb (3) - was pressed.

//	int		m_nComputationMethod;
//	--- Current value of change computation method.

//	int		m_nSpinWidth;
//	int		m_nSpinHeight;
//	--- The spin control size.

//	int		m_nThumbTop;
//	int		m_nThumbBottom;
//	--- The thumb upper and lower borders.

//	int		m_nOriginalPixel;
//	--- Mouse screen coordinate immediately before start of thumb dragging.

//	int		m_nMaxPixel;
//	--- Screen size.

//	int		m_nOriginalValue;
//	--- Spin value immediately before start of thumb dragging.

//	int		m_nMinValue;
//	int		m_nMaxValue;
//	--- Upper and lower limits of the spin value.

//	DWORD	m_dwClickTicks;
//	--- Number of milliseconds that have elapsed since mouse clicked.

//	HCURSOR	m_hThumbHorCursor;
//	HCURSOR	m_hThumbVerCursor;
//	--- Cursor handlers for horizontal and vertical spin.

//	HCURSOR	m_hThumbDefCursor;
//	--- Default cursor handler.

// private:

/////////////////////////////////////////////////////////////////////////////
// Member functions ---------------------------------------------------------
// public:

COXSpinCtrl::COXSpinCtrl(BOOL bEnable /*= TRUE*/)
	{
	m_bEnable = bEnable;
	m_bHoriz = FALSE;
	m_ucWasPressed = 0;
	m_nSpinWidth = m_nSpinHeight = 0;
	m_nThumbBottom = m_nThumbTop = 0;
	m_nMinValue = m_nMaxValue = m_nOriginalValue = 0;
	m_nMaxPixel = m_nOriginalPixel = 0;
	m_nComputationMethod = OX_SPIN_DELTA_PIXEL_IS_DELTA_VALUE;
	m_dwClickTicks = 0;
	m_hThumbDefCursor = m_hThumbVerCursor = AfxGetApp()->LoadCursor(IDC_OX_SPINVERCUR);
	m_hThumbHorCursor = AfxGetApp()->LoadCursor(IDC_OX_SPINHORCUR);
#ifdef _DEBUG
	if ((m_hThumbDefCursor == NULL) || (m_hThumbVerCursor == NULL) || (m_hThumbHorCursor == NULL))
		{
		TRACE0("COXSpinCtrl::COXSpinCtrl : At least one cursor could not be loaded\n");
		TRACE0("Make sure OXSpinCtrl.rc is included in your project\n");
		ASSERT(FALSE);
		}
#endif // _DEBUG
	}

COXSpinCtrl::~COXSpinCtrl()
	{
	::ReleaseCapture();
	}

void COXSpinCtrl::EnableThumb(BOOL bEnable /*= TRUE*/)
	{
	m_bEnable = bEnable;
	RedrawWindow();
	}

int COXSpinCtrl::ComputeValueFromDeltaPixel(int nDeltaPixel) const
	{
	int	nValue = 0;

	// Use the right computation method.
	switch ( m_nComputationMethod )
		{
		case OX_SPIN_DELTA_PIXEL_IS_DELTA_VALUE :	// Pixel change equals value change.
			if ( m_nMaxValue > m_nMinValue )
				nValue = m_nOriginalValue + nDeltaPixel;
			else
				nValue = m_nOriginalValue - nDeltaPixel;
			break;
		case OX_SPIN_SCREEN_AREA :					// Screen area used as outer limits.
			{
			int	nOriginalPixel = m_nOriginalPixel,
				nScreenUpperRange, nScreenLowerRange,				// Screen ranges above and under the original pixel.
				nValueUpperRange = m_nMaxValue - m_nOriginalValue,	// \ Spin ranges above and
				nValueLowerRange = m_nOriginalValue - m_nMinValue,	// / under the original value.
				nUpperDeltaValue, nLowerDeltaValue,					// Spin value changes upward and downward.
				nDeltaValue;										// Desired spin value change.

			// Correct original mouse coordinate.
			if ( nOriginalPixel == 0 )
				nOriginalPixel++;
			else if ( m_nMaxPixel - nOriginalPixel == 1 )
				nOriginalPixel--;

			// Compute the screen range.
			if ( m_bHoriz )
				{
				nScreenUpperRange = m_nMaxPixel - nOriginalPixel - 1;
				nScreenLowerRange = nOriginalPixel;
				}
			else
				{
				nScreenUpperRange = nOriginalPixel;
				nScreenLowerRange = m_nMaxPixel - nOriginalPixel - 1;
				}

			// Compute the spin value increasing or decreasing.
			nUpperDeltaValue = nDeltaPixel * nValueUpperRange / nScreenUpperRange;
			nLowerDeltaValue = nDeltaPixel * nValueLowerRange / nScreenLowerRange;
			if ( abs(nUpperDeltaValue) > abs(nLowerDeltaValue) )
				nDeltaValue = nUpperDeltaValue;
			else
				nDeltaValue = nLowerDeltaValue;

			nValue = m_nOriginalValue + nDeltaValue;
			}
			break;
		default :
			TRACE1("COXSpinCtrl::ComputeValueFromDeltaPixel() : Unexpected computation method %i\n", m_nComputationMethod);
			ASSERT(FALSE);
			break;
		}

	// Check for out of bound conditions
	if ( m_nMaxValue > m_nMinValue )
	{
		if ( nValue < m_nMinValue )
			nValue = m_nMinValue;
		if ( nValue > m_nMaxValue )
			nValue = m_nMaxValue;
	}
	else
	{
		if ( nValue < m_nMaxValue )
			nValue = m_nMaxValue;
		if ( nValue > m_nMinValue )
			nValue = m_nMaxValue;
	}

	return nValue;
	}

BEGIN_MESSAGE_MAP(COXSpinCtrl, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(COXSpinCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// protected:

// Message handlers

void COXSpinCtrl::OnPaint()
	// --- In :
	// --- Out :
	// --- Returns :
	// --- Effect :		The framework calls this member function when Windows or an application makes
	//					a request to repaint a portion of an application's window.
	//					Function draws the spin buttons: up, down and (if it's specified) the thumb.
	{
	CPaintDC	dc(this);						// device context for painting
	DWORD		dwStateUp = DFCS_BUTTONPUSH,
				dwStateDown = DFCS_BUTTONPUSH,	// button controls state
				dwStateThumb = DFCS_BUTTONPUSH;

	// Define which button is pressed; change its state.
	if ( m_ucWasPressed == 3 )
		dwStateThumb |= DFCS_PUSHED;
	if ( m_ucWasPressed == 1 || m_ucWasPressed == 3 )
		dwStateUp |= DFCS_PUSHED;
	if ( m_ucWasPressed == 2 || m_ucWasPressed == 3 )
		dwStateDown |= DFCS_PUSHED;

	if (!IsWindowEnabled())
	{
		dwStateThumb|=DFCS_INACTIVE;
		dwStateUp|=DFCS_INACTIVE;
		dwStateDown|=DFCS_INACTIVE;
	}

	// Draw the thumb and make it darker if it's too small.
	ThumbDraw(&dc, dwStateThumb);

	// Draw up and down buttons with arrows.
	ArrowsDraw(&dc, dwStateUp, dwStateDown);
	}

void COXSpinCtrl::OnLButtonDown(UINT nFlags, CPoint point)
	// --- In :			nFlags :	Indicates whether various virtual keys are down.
	//					point :		Specifies the x- and y-coordinate of the cursor.
	// --- Out :
	// --- Returns :
	// --- Effect :		The framework calls this member function when the user presses the left mouse button.
	//					Function provides default CSpinButtonCtrl implementation for up and down buttons pressing,
	//					and special COXSpinCtrl implementation for pressing of the thumb.
	{
	CPoint screenPoint = point;
	ClientToScreen(&screenPoint);

	int	nCoord = ( m_bHoriz ) ? point.x : point.y,
		nScreenCoord = ( m_bHoriz ) ? screenPoint.x : screenPoint.y;

	// Which spin button is pressed?
	if ( nCoord < m_nThumbTop )			// Up button is pressed.
		m_ucWasPressed = 1;
	else if ( nCoord > m_nThumbBottom )	// Down button is pressed.
		m_ucWasPressed = 2;
	else if ( m_bEnable )
		{
		// Thumb is pressed: special COXSpinCtrl implementation.
		m_ucWasPressed = 3;
		if ( m_hThumbDefCursor )
			::SetCursor(m_hThumbDefCursor);
		GetRange(m_nMinValue, m_nMaxValue);
		m_nOriginalValue = LOWORD(GetPos());
		m_nOriginalPixel = nScreenCoord;
		m_nMaxPixel = ( m_bHoriz ) ? ::GetSystemMetrics(SM_CXSCREEN) : ::GetSystemMetrics(SM_CYSCREEN);
		SetCapture();
		}

	RedrawWindow();
	if ( m_ucWasPressed != 3 )
		CSpinButtonCtrl::OnLButtonDown(nFlags, point);	// Up and down buttons: default CSpinButtonCtrl implementation.
	}

void COXSpinCtrl::OnLButtonUp(UINT nFlags, CPoint point)
	// --- In :			nFlags :	Indicates whether various virtual keys are down.
	//					point :		Specifies the x- and y-coordinate of the cursor.
	// --- Out :
	// --- Returns :
	// --- Effect :		The framework calls this member function when the user releases the left mouse button.
	//					Function processes the thumb releasing: finally sets the spin value and implements
	//					the value swaping, in case it was caused by mouse double-clicking.
	{
	if ( m_ucWasPressed == 3 )	// Thumb releasing implementation.
		{
		if ( IsDoubleClick() )
			{
			// Double-click implementation (spin value swaping).
			int	nCurrentPos = LOWORD(GetPos()),
				nLowerInterval = nCurrentPos - m_nMinValue,
				nUpperInterval = m_nMaxValue - nCurrentPos;

			if ( !nLowerInterval )
				SetPos(m_nMaxValue);
			else if ( !nUpperInterval )
				SetPos(m_nMinValue);
			else
				{
				if ( nLowerInterval < nUpperInterval )
					SetPos(m_nMinValue);
				else
					SetPos(m_nMaxValue);
				}
			}
		else
			{
			// Final spin value setting.
			CPoint screenPoint = point;
			ClientToScreen(&screenPoint);
			int	nScreenCoord = ( m_bHoriz ) ? screenPoint.x : screenPoint.y,
				nDirect = ( m_bHoriz ) ? 1 : -1;
			SetPos(ComputeValueFromDeltaPixel(nDirect * (nScreenCoord - m_nOriginalPixel)));
			m_nMinValue = m_nMaxValue = m_nOriginalValue = 0;
			m_nMaxPixel = m_nOriginalPixel = 0;
			::ReleaseCapture();
			}
		}

	m_ucWasPressed = 0;			// Set all spin buttons to non-pushed state.
	RedrawWindow();
	CSpinButtonCtrl::OnLButtonUp(nFlags, point);
	}

void COXSpinCtrl::OnMouseMove(UINT nFlags, CPoint point)
	// --- In :			nFlags :	Indicates whether various virtual keys are down.
	//					point :		Specifies the x- and y-coordinate of the cursor.
	// --- Out :
	// --- Returns :
	// --- Effect :		The framework calls this member function when the mouse cursor moves.
	//					Function sets the spin value when user drags the mouse with pressed
	//					left button (in case this button was pressed on the thumb).
	{
	if ( m_ucWasPressed == 3 )	// Thumb is pressed - change spin value.
		{
		CPoint screenPoint = point;
		ClientToScreen(&screenPoint);
		int	nScreenCoord = ( m_bHoriz ) ? screenPoint.x : screenPoint.y,
			nDirect = ( m_bHoriz ) ? 1 : -1;
		SetPos(ComputeValueFromDeltaPixel(nDirect * (nScreenCoord - m_nOriginalPixel)));
		}

	int nCoord = ( m_bHoriz ) ? point.x : point.y;

	if ( m_hThumbDefCursor && (m_ucWasPressed == 3 ||
		 (m_bEnable && nCoord >= m_nThumbTop && nCoord <= m_nThumbBottom)) )
		::SetCursor(m_hThumbDefCursor);

	CSpinButtonCtrl::OnMouseMove(nFlags, point);
	}

// Member functions

void COXSpinCtrl::GetSizeAndOrientation()
	// --- In :
	// --- Out :
	// --- Returns :
	// --- Effect :		This function checks the spin size and orientation, and sets necessary member variables.
	{
	RECT ClientRect;						// size
	GetClientRect(&ClientRect);
	m_nSpinWidth = ClientRect.right;
	m_nSpinHeight = ClientRect.bottom;

	DWORD dwStyle = GetStyle();				// orientation
	if ( dwStyle & UDS_HORZ )
		{
		m_bHoriz = TRUE;
		m_hThumbDefCursor = m_hThumbHorCursor;
		}
	else
		{
		m_bHoriz = FALSE;
		m_hThumbDefCursor = m_hThumbVerCursor;
		}

	ThumbUpDownDefine();					// thumb up and down borders
	}

void COXSpinCtrl::ThumbUpDownDefine()
	// --- In :
	// --- Out :
	// --- Returns :
	// --- Effect :		This function defines the thumb upper and lower borders.
	{
	int	nWidth = ( m_bEnable ) ? 5 : 1;

	// Place thumb borders relatively to the spin control borders.
	m_nThumbTop = nWidth;
	if ( m_bHoriz )
		m_nThumbBottom = m_nSpinWidth - nWidth;
	else
		m_nThumbBottom = m_nSpinHeight - nWidth;

	// Move one thumb border to another: set suitable thumb size.
	while ( m_nThumbBottom - m_nThumbTop > nWidth )
		{
		m_nThumbTop++;
		m_nThumbBottom--;
		}
	}

void COXSpinCtrl::ThumbDraw(CPaintDC* pdc, DWORD dwStateThumb)
	// --- In :			pdc :			Points to CPaintDC object represented device context for painting.
	//					dwStateThumb :	Specifies the initial state of the thumb frame control.
	// --- Out :
	// --- Returns :
	// --- Effect :		This function draws the thumb and makes it darker if it's too small.
	{
	// Set necessary variables for spin size and orientation.
	GetSizeAndOrientation();

	// Define thumb coordinates and draw it.
	RECT rectButton;
	if ( m_bHoriz )
		{
		rectButton.left = m_nThumbTop;
		rectButton.right = m_nThumbBottom;
		rectButton.top = 0;
		rectButton.bottom = m_nSpinHeight;
		}
	else
		{
		rectButton.left = 0;
		rectButton.right = m_nSpinWidth;
		rectButton.top = m_nThumbTop;
		rectButton.bottom = m_nThumbBottom;
		}
	pdc->DrawFrameControl(&rectButton, DFC_BUTTON, dwStateThumb);

	// Make thumb darker, if it's needed.
	if ( m_nThumbBottom - m_nThumbTop < 4 )
		{
		COLORREF dwColor = ::GetSysColor(COLOR_3DFACE);
		pdc->FillSolidRect(&rectButton, RGB(GetRValue(dwColor)/2, GetGValue(dwColor)/2, GetBValue(dwColor)/2));
		}
	}

void COXSpinCtrl::ArrowsDraw(CPaintDC* pdc, DWORD dwStateUp, DWORD dwStateDown)
	// --- In :			pdc :			Points to CPaintDC object represented device context for painting.
	//					dwStateUp :		Specifies the initial state of the up button frame control.
	//					dwStateDown :	Specifies the initial state of the down button frame control.
	// --- Out :
	// --- Returns :
	// --- Effect :		This function draws the spin up and down buttons with its arrows.
	{
	RECT rectButton;

	// Draw up button.
	rectButton.left = 0;
	rectButton.right = ( m_bHoriz ) ? m_nThumbTop : m_nSpinWidth;
	rectButton.top = 0;
	rectButton.bottom = ( m_bHoriz ) ? m_nSpinHeight : m_nThumbTop;
	pdc->DrawFrameControl(&rectButton, DFC_BUTTON, dwStateUp);

	// Draw arrows on the up button.
	if ( m_bHoriz )
		ArrowTriangle(pdc, rectButton.bottom, rectButton.right, -1, 1);
	else
		ArrowTriangle(pdc, rectButton.right, rectButton.bottom, -1, 1);

	// Draw down button.
	rectButton.left = ( m_bHoriz ) ? m_nThumbBottom : 0;
	rectButton.right = m_nSpinWidth;
	rectButton.top = ( m_bHoriz ) ? 0 : m_nThumbBottom;
	rectButton.bottom = m_nSpinHeight;
	pdc->DrawFrameControl(&rectButton, DFC_BUTTON, dwStateDown);

	// Draw arrows on the down button.
	if ( m_bHoriz )
		ArrowTriangle(pdc, rectButton.bottom, rectButton.right - rectButton.left, rectButton.left, -1);
	else
		ArrowTriangle(pdc, rectButton.right, rectButton.bottom - rectButton.top, rectButton.top, -1);
	}

void COXSpinCtrl::ArrowTriangle(CPaintDC* pdc, int nWid, int nHei, int nShift, int nUL)
	// --- In :			pdc :		Points to CPaintDC object represented device context for painting.
	//					nWid :		Width of the thumb.
	//					nHei :		Height of the thumb.
	//					nShift :	Shift of the triangle (for its better look).
	//					nUL :		"Upper/Lower" flag. Determines which triangle (upper or lower) is drawn.
	// --- Out :
	// --- Returns :
	// --- Effect :		This function draws triangular arrows on up and down buttons.
	{

	CPen pen;
	CPen* pOldPen=NULL;
	COLORREF clr=::GetSysColor(COLOR_GRAYTEXT);
	if (!IsWindowEnabled())
	{
		VERIFY(pen.CreatePen(PS_SOLID, 1, clr));
		pOldPen=pdc->SelectObject(&pen);
	}

	int	nWidDyn = nWid, nMidLineCoord, nMidLineLen, nMidLine1, nMidLine2, nLinesNum;

	// Coordinate of arrow's "middle line".
	if ( nUL > 0 )
		nMidLineCoord = nShift + nHei / 2;
	else
		nMidLineCoord = nShift + nHei - nHei / 2 - 1;

	// Number of lines on each hand of the "middle line".
	do
		{
		nMidLineLen = (3 * nWidDyn / 4 + 1) / 2;	// Length of the "middle line".
		if ( (nWid - nMidLineLen) % 2 )
			nMidLineLen--;
		nLinesNum = (nMidLineLen - 1) / 2;
		nWidDyn--;
		}
	while ( nMidLineCoord - nLinesNum <= nShift || nMidLineCoord + nLinesNum >= nShift + nHei - 1 );

	// Coordinates of the "middle line" begin & end.
	nMidLine1 = (nWid - nMidLineLen) / 2;
	nMidLine2 = nWid - nMidLine1;

	// Draw the "middle line".
	if ( m_bHoriz )
		{
		pdc->MoveTo(nMidLineCoord, nMidLine1);
		pdc->LineTo(nMidLineCoord, nMidLine2);
		}
	else
		{
		pdc->MoveTo(nMidLine1, nMidLineCoord);
		pdc->LineTo(nMidLine2, nMidLineCoord);
		}

	// Draw lines on both hands of the "middle line".
	for ( int nCount = 1; nCount <= nLinesNum; nCount++ )
		{
		if ( m_bHoriz )
			{
			if ( nMidLineCoord - nCount > nShift + 1 )
				{
				pdc->MoveTo(nMidLineCoord - nCount, nMidLine1 + nUL * nCount);
				pdc->LineTo(nMidLineCoord - nCount, nMidLine2 - nUL * nCount);
				}
			if ( nMidLineCoord + nCount < nShift + nHei - 2 )
				{
				pdc->MoveTo(nMidLineCoord + nCount, nMidLine1 - nUL * nCount);
				pdc->LineTo(nMidLineCoord + nCount, nMidLine2 + nUL * nCount);
				}
			}
		else
			{
			if ( nMidLineCoord - nCount > nShift + 1 )
				{
				pdc->MoveTo(nMidLine1 + nUL * nCount, nMidLineCoord - nCount);
				pdc->LineTo(nMidLine2 - nUL * nCount, nMidLineCoord - nCount);
				}
			if ( nMidLineCoord + nCount < nShift + nHei - 2 )
				{
				pdc->MoveTo(nMidLine1 - nUL * nCount, nMidLineCoord + nCount);
				pdc->LineTo(nMidLine2 + nUL * nCount, nMidLineCoord + nCount);
				}
			}
		}

	if (pen.m_hObject)
	{
		pdc->SelectObject(pOldPen);
		pen.DeleteObject();
	}
	}

BOOL COXSpinCtrl::IsDoubleClick()
	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if mouse click on the spin thumb is double-click, FALSE - otherwise.
	// --- Effect :		This function returns whether mouse click on the spin thumb is double-click.
	{
	BOOL	bDoubleClick = FALSE;
	DWORD	dwTickCount = ::GetTickCount();

	if ( dwTickCount - m_dwClickTicks <= ::GetDoubleClickTime() )
		{
		bDoubleClick = TRUE;
		m_dwClickTicks = 0;
		}
	else
		m_dwClickTicks = dwTickCount;

	return bDoubleClick;
	}
