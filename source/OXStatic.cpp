// ==========================================================================
// 					Class Implementation : 
//						COXStatic
// ==========================================================================

// Implementation file : OXStatic.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

////////////////
// COXStatic implements a static control which could be set to use
// user-defined color and font to draw text. Also you can show tooltip
// and you can make COXStatic automatically adjust its size to fit drawn text
//
#include "StdAfx.h"
#include "OXStatic.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////	
//////////////////////////////////////////////////	
	
IMPLEMENT_DYNAMIC(COXStatic, CStatic)

BEGIN_MESSAGE_MAP(COXStatic, CStatic)
	//{{AFX_MSG_MAP(COXStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_SETTEXT,OnSetText)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////
// Constructor
//
// --- In  :
// --- Out : 
// --- Returns :
// --- Effect : Constructs the object
COXStatic::COXStatic()
{
	// background color
	m_clrBk=::GetSysColor(COLOR_BTNFACE);
	// type of filling
	m_typeFillBackground=NOGRADIENT;
	// text color
	m_clr=::GetSysColor(COLOR_WINDOWTEXT); 

	m_bShowToolTip=FALSE;
	m_bToolTipUserDefined=FALSE;

	m_bFitToText=FALSE;

	m_bCreated=FALSE;
	m_bIsDefaultFontSet=FALSE;
}


void COXStatic::SetBkColor(COLORREF clr, BOOL bRedraw) 
{ 
	m_clrBk=clr; 
	if(bRedraw)
		::InvalidateRect(GetSafeHwnd(),NULL,TRUE);
}


void COXStatic::SetBackgroundFillType(BackgroundFillType typeFillBackground, 
									  BOOL bRedraw) 
{ 
	m_typeFillBackground=typeFillBackground; 
	if(bRedraw)
		::InvalidateRect(GetSafeHwnd(),NULL,TRUE);
}


void COXStatic::SetTextColor(COLORREF clr, BOOL bRedraw) 
{ 
	m_clr=clr; 
	if(bRedraw)
		::InvalidateRect(GetSafeHwnd(),NULL,TRUE);
}

// --- In  :	plf	-	pointer to LOGFONT structure
// --- Out :	plf -	filled the structure with the LOGFONT
//						of the font that COXStatic object uses to draw text
// --- Returns:	TRUE if plf was successfully populated with 
//				LOGINFO of the font used by COXStatic object, 
//				otherwise returns FALSE and plf is undefined
// --- Effect : 
BOOL COXStatic::GetTextLogFont(LOGFONT* plf) const 
{
	if((HFONT)m_font==NULL) 
	{
		return FALSE;
	}
	return m_font.GetObject(sizeof(*plf),plf);
}

// --- In  :	
// --- Out :	
//				
// --- Returns:	pointer to the font used by COXStatic object if succeed, 
//				otherwise returns NULL
// --- Effect : 
CFont* COXStatic::GetTextFont()
{
	if((HFONT)m_font==NULL) 
	{
		return NULL;
	}
	return &m_font;
}

// --- In  :	plf	-	pointer to LOGFONT structure
// --- Out : 
// --- Returns:	TRUE if font was successfully set
// --- Effect : sets the font that COXStatic object uses to draw text;
//				if you call this function as SetTextLogFont(NULL) then
//				text will be drawn using the font which is associated 
//				with CStatic window by default 
BOOL COXStatic::SetTextLogFont(LOGFONT* plf, BOOL bRedraw)
{
	if ((HFONT)m_font!=NULL) 
	{
		m_font.DeleteObject();
	}

	BOOL bResult=TRUE;
	if(plf!=NULL)
	{
		bResult=m_font.CreateFontIndirect(plf);
	}

	if(bResult)
	{
		if(m_bFitToText)
		{
			AdjustToFitText();
		}
		else
		{
			if(bRedraw)
			{
				::InvalidateRect(GetSafeHwnd(),NULL,TRUE);
			}
		}
	}

	return bResult;
}

// --- In  :	pFont	-	pointer to CFont
// --- Out : 
// --- Returns:	TRUE if font was successfully set
// --- Effect : sets the font that COXStatic object uses to draw text
BOOL COXStatic::SetTextFont(CFont* pFont, BOOL bRedraw)
{
	ASSERT_VALID(pFont);

	BOOL bResult=TRUE;
	if(pFont!=&m_font)
	{
		if ((HFONT)m_font!=NULL) 
			m_font.DeleteObject();

		LOGFONT lf;
		bResult=pFont->GetLogFont(&lf);
		if(bResult)
			bResult=m_font.CreateFontIndirect(&lf);
	}

	if(bResult)
	{
		if(m_bFitToText)
		{
			AdjustToFitText();
		}
		else
		{
			if(bRedraw)
			{
				::InvalidateRect(GetSafeHwnd(),NULL,TRUE);
			}
		}
	}

	return bResult;
}

// --- In  :
// --- Out : 
// --- Returns:	TRUE if default font was successfully set to the 
//				COXStatic object, otherwise returns FALSE
// --- Effect :	make COXStatic object to use default font to draw text.
//				Note that function declared as virtual, so you can put
//				in any derivation of COXStatic class your own definition
//				of "default font". COXStatic class uses the font which is 
//				associated with CStatic window as default.
BOOL COXStatic::SetDefaultTextFont()
{
	if(!IsStaticText(this)) 
		return TRUE;

	CFont* pFont=GetFont();
	if(pFont==NULL)
		return FALSE;

	ASSERT_VALID(pFont);

	if((HFONT)m_font!=NULL) 
		m_font.DeleteObject();

	LOGFONT lf;
	BOOL bResult=pFont->GetObject(sizeof(lf), &lf);
	if(bResult)
	{
		bResult=m_font.CreateFontIndirect(&lf);

		if (bResult && m_bFitToText)
			AdjustToFitText(); 
	}

	return bResult;
}

// if TRUE then we will adjust size of static control window 
// to match the size of the text otherwise we will use original
// size of static control's window
void COXStatic::SetFitToText(BOOL bFitText) 
{ 
	m_bFitToText=bFitText; 
	AdjustToFitText(); 
}

// Sets the tooltip text. If tooltip text wasn't set and tootip control is still
// to be shown then COXStatic would use window's text as the tooltip text
void COXStatic::SetToolTipText(LPCTSTR sText) 
{ 
	m_sToolTipText=sText; 	

	if(m_sToolTipText.IsEmpty())
	{
		// mark tooltip text as not userdefined to use 
		// window text as tooltip
		m_bToolTipUserDefined=FALSE; 
	}
	else
	{
		// mark tooltip text as userdefined
		m_bToolTipUserDefined=TRUE; 
	}
	
	FormatToolTipText();
}

// Returns tooltip text if it was set previously by SetToolTipText function.
// If bForse is TRUE then returns the tooltip text even if it wasn't set by 
// SetToolTipText function, otherwise returns empty string
CString COXStatic::GetToolTipText(BOOL bForce) 
{ 
	if(!bForce && !m_bToolTipUserDefined)
	{
		return _T("");
	}
	return m_sToolTipText; 
}

// if set to TRUE, the tooltip will be shown (by default TRUE)
void COXStatic::SetShowToolTip(BOOL bShowToolTip) 
{ 
	m_bShowToolTip=bShowToolTip; 

	if(::IsWindow(m_ttc.GetSafeHwnd()))
		// activate/deactivate tooltip control
		m_ttc.Activate(m_bShowToolTip); 
	
	FormatToolTipText();
}

//////////////////
// Handle reflected WM_CTLCOLOR to set custom control color and font.
// For non-text controls, do nothing.
// Note that function is declared as virtual, so any COXStatic derived 
// class can define its own function to draw itself.
// 
HBRUSH COXStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	UNREFERENCED_PARAMETER(nCtlColor);

	ASSERT(nCtlColor==CTLCOLOR_STATIC);
	DWORD dwStyle=GetStyle();
	if (!(dwStyle & SS_NOTIFY)) 
	{
		// Turn on notify flag to get mouse messages and STN_CLICKED.
		// Otherwise, I'll never get any mouse clicks!
		::SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	}
	
	HBRUSH hbr = NULL;
	if(IsStaticText(this)) 
	{
		// this is a text control: set up font and colors
		//

		if((HFONT)m_font!=NULL) 
		{
			// use our font
			pDC->SelectObject(&m_font);
		}

		// use our visited/unvisited colors
		pDC->SetTextColor(m_clr);
		pDC->SetBkMode(TRANSPARENT);

		// return hollow brush to preserve parent background color
		hbr=(HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	return hbr;
}

// handles WM_SETTEXT message in order to possibly adjust size of 
// control's window or update tooltip text
LRESULT COXStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	BOOL bResult=(BOOL)Default();

	FormatToolTipText();
	if(m_bFitToText)
		AdjustToFitText();
	RedrawWindow();

	return bResult;
}

// if m_bFitToText is TRUE then adjusts the size of control's window 
// to the size of the text, otherwise set size of window to its original size
void COXStatic::AdjustToFitText()
{
	if(!IsStaticText(this)) 
	{
		// just return if static control is not text control
		return;
	}

	// get the parent window
	// all operations with coordinates use parent as origin
	CWnd* pParent = GetParent();
	if(pParent==NULL)
	{
		// just return if we didn't find any parent
		return;
	}

	CRect rect;
	if (!m_bFitToText)
	{
		// if m_bFitToText is FALSE then reset the size of control's window
		// to its original size
		rect=m_rectOriginal;
	}
	else
	{
		// Get the current window position
		GetWindowRect(rect);

		pParent->ScreenToClient(rect);

		// Get the size of the window text
		CString strWndText;
		GetWindowText(strWndText);

		CDC* pDC = GetDC();
		CFont* pOldFont=NULL;
		if ((HFONT)m_font!=NULL) 
		{
			// use our font
			pOldFont=pDC->SelectObject(&m_font);
		}
		CSize sizeText = pDC->GetTextExtent(strWndText);
		if(pOldFont!=NULL)
		{
			pDC->SelectObject(pOldFont);
		}
		ReleaseDC(pDC);

		// make sure the size of text is no more than the size of 
		// the client part of parent window
		CRect rectClient;
		pParent->GetClientRect(&rectClient);
		if(sizeText.cx>rectClient.Width())
		{
			sizeText.cx=rectClient.Width();
		}
		if(sizeText.cy>rectClient.Height())
		{
			sizeText.cy=rectClient.Height();
		}

		// Get the text justification via the window style
		DWORD dwStyle = GetStyle();

		// Recalc the window size and position based on the text justification
		if(dwStyle & SS_CENTERIMAGE)
		{
			rect.DeflateRect(0, (rect.Height() - sizeText.cy)/2);
		}
		else
		{
			rect.bottom = rect.top + sizeText.cy;
		}

		DWORD dwStaticStyle=GetStyle()&0x0000000f;
		switch(dwStaticStyle)
		{
		// SS_CENTER
		case 1:
			rect.DeflateRect((rect.Width() - sizeText.cx)/2, 0);
			break;
		// SS_RIGHT
		case 2:
			rect.left=rect.right - sizeText.cx;
			break;
		// SS_LEFT
		default:
			rect.right=rect.left + sizeText.cx;
			break;
		}
	}

	// Change information about window's rect in tooltip control
	if(::IsWindow(m_ttc.GetSafeHwnd()))
	{
		CToolInfo toolInfo;
		if(m_ttc.GetToolInfo(toolInfo,this,ID_HLTOOLTIP))
		{
			toolInfo.rect.left=0;
			toolInfo.rect.top=0;
			toolInfo.rect.right=rect.Width();
			toolInfo.rect.bottom=rect.Height();
			m_ttc.SetToolInfo(&toolInfo);
		}
	}

	// Move the window
	SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

// builds tooltip text
void COXStatic::FormatToolTipText()
{
	// set only if tooltip is shown
	if(m_bShowToolTip)
	{
		// if tooltip text wasn't set then set it to the window text 
		if(!m_bToolTipUserDefined)
		{
			if(IsStaticText(this))
			{
				GetWindowText(m_sToolTipText);
			}
			else
			{
				m_sToolTipText.Empty();
			}
		}

		if(::IsWindow(m_ttc.GetSafeHwnd()))
			m_ttc.UpdateTipText(m_sToolTipText, this, ID_HLTOOLTIP);
	}
}

// handle it to pass a mouse message to a tool tip control for processing 
BOOL COXStatic::PreTranslateMessage(MSG* pMsg) 
{
	if(::IsWindow(m_ttc.GetSafeHwnd()))
	{
		m_ttc.Activate(m_bShowToolTip); 
		m_ttc.RelayEvent(pMsg);
	}
	return CStatic::PreTranslateMessage(pMsg);
}

// handle it to initialize tooltip control and save the original size 
// of static control window
void COXStatic::PreSubclassWindow() 
{
	if(!m_bCreated && !InitStatic())
	{
		TRACE(_T("COXStatic::PreSubclassWindow: failed to initialize static control\n"));
	}

	CStatic::PreSubclassWindow();
}

// handle it to update original size of window and if m_bFitToText is TRUE then
// resize it to fit to its text
void COXStatic::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(m_bFitToText)
	{
		AdjustToFitText();
	}
	else
	{
		// update the original size of the static control window
		GetWindowRect(&m_rectOriginal);
		CWnd* pParent = GetParent();
		if(pParent!=NULL)
		{
			pParent->ScreenToClient(&m_rectOriginal);
		}

		// Change information about window's rect in tooltip control
		if(::IsWindow(m_ttc.GetSafeHwnd()))
		{
			CToolInfo toolInfo;
			if(m_ttc.GetToolInfo(toolInfo,this,ID_HLTOOLTIP))
			{
				toolInfo.rect.left=0;
				toolInfo.rect.top=0;
				toolInfo.rect.right=m_rectOriginal.Width();
				toolInfo.rect.bottom=m_rectOriginal.Height();
				m_ttc.SetToolInfo(&toolInfo);
			}
		}

		RedrawWindow();
	}
}

// helper function, defines if static control displays text or graphics
BOOL COXStatic::IsStaticText(CStatic* pStatic) const
{
	DWORD dwStyle=pStatic->GetStyle();
	return ((dwStyle&0xFF)>SS_RIGHT && (dwStyle&0xFF)!=SS_SIMPLE && 
		(dwStyle&0xFF)!=SS_LEFTNOWORDWRAP) ? FALSE : TRUE; 
}

BOOL COXStatic::InitStatic() 
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// save the original size of the static control window
	GetWindowRect(&m_rectOriginal);
	CWnd* pParent = GetParent();
	if(pParent!=NULL)
	{
		pParent->ScreenToClient(&m_rectOriginal);
	}

	// Create the tooltip
	CRect rect; 
	GetClientRect(rect);
	m_ttc.Create(this);
	m_ttc.AddTool(this, m_sToolTipText, rect, ID_HLTOOLTIP);

	FormatToolTipText();

	// set up default font once!
	if(!m_bIsDefaultFontSet && SetDefaultTextFont())
		m_bIsDefaultFontSet=TRUE;

	return TRUE;
}

BOOL COXStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, 
					   CWnd* pParentWnd, UINT nID/*=0xffff*/)
{
	// mark control as being created 
	m_bCreated=TRUE;

	BOOL bResult=CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	if(bResult)
	{
		// Initialize static control
		if(!InitStatic())
		{
			TRACE(_T("COXStatic::Create: failed to initialize static control"));
			return FALSE;
		}
	}

	return bResult;
}

void COXStatic::OnDestroy()
{
	if(::IsWindow(m_ttc.GetSafeHwnd()))
	{
		m_ttc.DelTool(this, ID_HLTOOLTIP);
		m_ttc.DestroyWindow();
	}

	CStatic::OnDestroy();
}

BOOL COXStatic::OnEraseBkgnd(CDC* pDC)
{
	// redraw the background
	if(m_clrBk!=CLR_NONE)
	{
		DrawBackground(pDC);
	}

	return TRUE;
}


void COXStatic::DrawBackground(CDC* pDC)
{
	ASSERT(pDC!=NULL);

	CRect rect;
	GetClientRect(rect);
	pDC->IntersectClipRect(rect);

	switch(m_typeFillBackground)
	{
	case NOGRADIENT:
		{
			CBrush brush(m_clrBk);
			ASSERT((HBRUSH)brush!=NULL);
			pDC->FillRect(rect,&brush);
			break;
		}
	case GRADIENT_LEFT:
	case GRADIENT_CENTER:
	case GRADIENT_RIGHT:
		{
			FillGradient(pDC,m_typeFillBackground,rect,GetBkColor());
			break;
		}
	default:
		ASSERT(FALSE);
	}
}


void COXStatic::FillGradient(CDC* pDC, BackgroundFillType typeFillBackground,
							 CRect rect, COLORREF clr)
{
	ASSERT(typeFillBackground!=NOGRADIENT);

	// red, green and blue color vals
	COLORREF clrBackground=clr;
	int red=GetRValue(clrBackground);				
	int green=GetGValue(clrBackground);				
	int blue=GetBValue(clrBackground);		
	
	int cxCap=rect.Width();
	int cyCap=rect.Height();

	int nCurBlock;
	// width of area to shade and width squared
	int nWidth, nWidth_x_2;				
	// width of one shade band
	int nDelta;	
	UINT nNumberShade=128;

	switch(typeFillBackground)
	{
	case GRADIENT_LEFT:
		{
			nCurBlock=cxCap;
			nWidth=cxCap;
			nWidth_x_2=cxCap*cxCap;
			nDelta=__max(nWidth/nNumberShade,1);

			while(nCurBlock>0) 
			{
				// paint bands right to left
				int nRest_x_2=(nWidth-nCurBlock)*(nWidth-nCurBlock);
				PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
					RGB(red-(red*nRest_x_2)/nWidth_x_2, 
					green-(green*nRest_x_2)/nWidth_x_2, 
					blue-(blue*nRest_x_2)/nWidth_x_2));
				// next band
				nCurBlock-=nDelta;							
			}
			// whatever's left ==> black
			PaintRect(pDC,0,0,nCurBlock+nDelta,cyCap,RGB(0,0,0));  

			break;
		}

	case GRADIENT_CENTER:
		{
			nCurBlock=cxCap/2;
			nWidth=cxCap/2;
			nWidth_x_2=cxCap*cxCap/4;
			nDelta=__max(nWidth/(2*nNumberShade),1);

			while(nCurBlock>0) 
			{
				// paint bands right to left
				int nRest_x_2=(nWidth-nCurBlock)*(nWidth-nCurBlock);
				PaintRect(pDC, nWidth+nCurBlock, 0, nDelta, cyCap,	
					RGB(red-(red*nRest_x_2)/nWidth_x_2, 
					green-(green*nRest_x_2)/nWidth_x_2, 
					blue-(blue*nRest_x_2)/nWidth_x_2));
				// next band
				nCurBlock-=nDelta;							
			}
			// whatever's left ==> black
			PaintRect(pDC,nWidth,0,nCurBlock+nDelta,cyCap,RGB(0,0,0));  

			nCurBlock=0;
			while(nCurBlock<=nWidth) 
			{
				// paint bands left to right
				int nRest_x_2=nCurBlock*nCurBlock;
				PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
					RGB(red-(red*nRest_x_2)/nWidth_x_2, 
					green-(green*nRest_x_2)/nWidth_x_2, 
					blue-(blue*nRest_x_2)/nWidth_x_2));
				// next band
				nCurBlock+=nDelta;							
			}
			// whatever's left ==> black
			PaintRect(pDC,nCurBlock-nDelta,0,nWidth-nCurBlock+nDelta,cyCap,RGB(0,0,0));  

			break;
		}

	case GRADIENT_RIGHT:
		{
			nCurBlock=0;
			nWidth=cxCap;
			nWidth_x_2=cxCap*cxCap;
			nDelta=__max(nWidth/nNumberShade,1);

			while(nCurBlock<nWidth) 
			{
				// paint bands left to right
				int nRest_x_2=nCurBlock*nCurBlock;
				PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
					RGB(red-(red*nRest_x_2)/nWidth_x_2, 
					green-(green*nRest_x_2)/nWidth_x_2, 
					blue-(blue*nRest_x_2)/nWidth_x_2));
				// next band
				nCurBlock+=nDelta;							
			}
			// whatever's left ==> black
			PaintRect(pDC,nCurBlock-nDelta,0,nWidth-nCurBlock+nDelta-1,cyCap,RGB(0,0,0));  

			break;
		}
	}
}


void COXStatic::PaintRect(CDC* pDC, int x, int y, int w, int h, COLORREF color)
{
	CBrush brush(color);
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(x, y, w, h, PATCOPY);
	pDC->SelectObject(pOldBrush);
}

