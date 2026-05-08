// ==========================================================================
// 							   Class Implementation : 
//						COXCaptionInfo & COXCaptionPainter
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <shlwapi.h>
#include "OXCaptionPainter.h"
#include "UTBStrOp.h"

#include "UTB64Bit.h"

#include "WTypes.h"

#ifndef __OXMFCIMPL_H__
#if _MFC_VER >= 0x0700
#if _MFC_VER >= 1400
#include <afxtempl.h>
#endif
#include <..\src\mfc\afximpl.h>
#else
#include <..\src\afximpl.h>
#endif
#define __OXMFCIMPL_H__
#endif

#ifndef OXCP_NO_SAVESTATE
#include "OXRegistryValFile.h"
#endif // OXCP_NO_SAVESTATE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define IDT_UPDATECAPTION		421
#define ID_UPDATECAPTION_DELAY	300

static void PaintRect(CDC* pDC, int x, int y, int w, int h, COLORREF color);
static BOOL IsSmallFont(BOOL& bIsSmall);

UINT COXCaptionPainter::m_nSetCaptionPainter=
	RegisterWindowMessage(_T("_SET_CAPTION_PAINTER_"));
UINT COXCaptionPainter::m_nGetCaptionPainter=
	RegisterWindowMessage(_T("_GET_CAPTION_PAINTER_"));

//////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(COXCaptionInfo, CObject, 1)

// Constructor
COXCaptionInfo::COXCaptionInfo() 
{
	Reset();
}

// Sets COXCaptionInfo properties to their default value
void COXCaptionInfo::Reset()
{
	m_bGradient=TRUE;
	m_nGradientAlignment=ID_GRADIENT_LEFT;
	m_nGradientAlgorithm=ID_GRADIENT_SQUARE;
	m_nTextFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	m_nNumberShade=64;

	m_clrBackground=ID_OXCP_COLOR_NONE;
	m_clrText=ID_OXCP_COLOR_NONE;
	if((HFONT)m_font!=NULL) 
		m_font.DeleteObject();
	if((HFONT)m_fontSm!=NULL) 
		m_fontSm.DeleteObject();
}

// Returns color used to fill caption. If an user preffered not to set any 
// custom color then function returns standard color. 
COLORREF COXCaptionInfo::GetBackgroundColor(BOOL bActive) const 
{ 
	if(m_clrBackground==ID_OXCP_COLOR_NONE)
	{
		if(bActive)
		{
			return GetSysColor(COLOR_ACTIVECAPTION);
//			return RGB(255,0,0);
		}
		else
		{
			return GetSysColor(COLOR_INACTIVECAPTION);
//			return RGB(0,255,0);
		}
	}
	return m_clrBackground; 
}

// Returns color used to draw text in caption. If an user preffered not to set any 
// custom color then function returns standard color. 
COLORREF COXCaptionInfo::GetTextColor(BOOL bActive) const 
{ 
	if(m_clrText==ID_OXCP_COLOR_NONE)
	{
		if(bActive)
			return ::GetSysColor(COLOR_CAPTIONTEXT);
		else
			return ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
	}
	return m_clrText; 
}

// returns TRUE if succeeds and sets plf to the LOGINFO of the font used to draw text,
// otherwise returns FALSE and plf is undefined
BOOL COXCaptionInfo::GetCaptionLogFont(LOGFONT* plf) const
{
	if((HFONT)m_font==NULL) 
	{
		BOOL bIsSmall;
		IsSmallFont(bIsSmall);

		NONCLIENTMETRICS ncm;
		ncm.cbSize=sizeof(NONCLIENTMETRICS);
		if(SystemParametersInfo(SPI_GETNONCLIENTMETRICS,0,&ncm,0))
		{
			CFont font;
			if(font.CreateFontIndirect(&ncm.lfCaptionFont))
				return font.GetObject(sizeof(*plf),plf);
		}
		return FALSE;
	}
	return m_font.GetObject(sizeof(*plf),plf);
}

// returns TRUE if succeeds and sets plf to the LOGINFO of the font used to 
// draw text (Small caption), otherwise returns FALSE and plf is undefined
BOOL COXCaptionInfo::GetSmCaptionLogFont(LOGFONT* plf) const
{
	if((HFONT)m_fontSm==NULL) 
	{
		NONCLIENTMETRICS ncm;
		ncm.cbSize=sizeof(NONCLIENTMETRICS);
		if(SystemParametersInfo(SPI_GETNONCLIENTMETRICS,0,&ncm,0))
		{
			CFont font;
			if(font.CreateFontIndirect(&ncm.lfSmCaptionFont))
				return font.GetObject(sizeof(*plf),plf);
		}
		return FALSE;
	}
	return m_fontSm.GetObject(sizeof(*plf),plf);
}

// returns TRUE if succeeds and creates font used to draw text from plf, 
// otherwise returns FALSE. If plf is NULL then any custom font previosly 
// set by SetCaptionLogFont function will be deleted (i.e. standard font 
// will be used to draw text in caption)
BOOL COXCaptionInfo::SetCaptionLogFont(LOGFONT* plf)
{
	if ((HFONT)m_font!=NULL) 
		m_font.DeleteObject();
	if(plf!=NULL)
		return m_font.CreateFontIndirect(plf);
	return TRUE;
}

// returns TRUE if succeeds and creates font used to draw text from plf, 
// otherwise returns FALSE. If plf is NULL then any custom font previosly 
// set by SetCaptionLogFont function will be deleted (i.e. standard font 
// will be used to draw text in caption)
BOOL COXCaptionInfo::SetSmCaptionLogFont(LOGFONT* plf)
{
	if ((HFONT)m_fontSm!=NULL) 
		m_fontSm.DeleteObject();
	if(plf!=NULL)
		return m_fontSm.CreateFontIndirect(plf);
	return TRUE;
}

//////////////////////////////////////////////
//  copy constructor.
COXCaptionInfo& COXCaptionInfo::operator=(const COXCaptionInfo& ci)
{
    ASSERT_VALID(this);
    ASSERT_VALID(&ci);

	if(this==&ci)
		return *this;

	m_clrBackground=ci.m_clrBackground;
	m_clrText=ci.m_clrText;
	m_bGradient=ci.m_bGradient;
	m_nGradientAlignment=ci.m_nGradientAlignment;
	m_nGradientAlgorithm=ci.m_nGradientAlgorithm;
	m_nNumberShade=ci.m_nNumberShade;

	LOGFONT lf;
	if ((HFONT)m_font!=NULL) 
		m_font.DeleteObject();
	if(ci.GetCaptionLogFont(&lf))
		SetCaptionLogFont(&lf);
	if ((HFONT)m_fontSm!=NULL) 
		m_fontSm.DeleteObject();
	if(ci.GetSmCaptionLogFont(&lf))
		SetSmCaptionLogFont(&lf);

	m_nTextFormat=ci.m_nTextFormat;
	
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// serialization
void COXCaptionInfo::Serialize(CArchive& ar)
{
    // Only CObject-derived objects and six data-type
    // primitives are serializable. However, you
    // can cast any data type to a serializable data type,
    // and then you can serialize your data. The serializable
    // data types are

    // BYTE:    8 bits unsigned
    // WORD:    16 bits unsigned
    // LONG:    32 bits unsigned
    // DWORD:   32 bits unsigned
    // float    32 bits
    // double   64 bits, IEEE standard
    
    if (ar.IsStoring())
    {
        ar << m_clrBackground;
        ar << m_bGradient;
        ar << m_nGradientAlignment;
        ar << m_nGradientAlgorithm;
        ar << m_nTextFormat;
        ar << m_clrText;
        ar << m_nNumberShade;
    }
    else
    {
        ar >> m_clrBackground;
        ar >> m_bGradient;
        ar >> m_nGradientAlignment;
        ar >> m_nGradientAlgorithm;
        ar >> m_nTextFormat;
        ar >> m_clrText;
        ar >> m_nNumberShade;
    }                 
	SerializeFont(ar,&m_font);
	SerializeFont(ar,&m_fontSm);
} 

// helper function to serialize any font to opened archive
void COXCaptionInfo::SerializeFont(CArchive& ar, CFont* pFont)
{
    // Only CObject-derived objects and six data-type
    // primitives are serializable. However, you
    // can cast any data type to a serializable data type,
    // and then you can serialize your data. The serializable
    // data types are

    // BYTE:    8 bits unsigned
    // WORD:    16 bits unsigned
    // LONG:    32 bits unsigned
    // DWORD:   32 bits unsigned
    // float    32 bits
    // double   64 bits, IEEE standard

    LOGFONT lf;

	if (ar.IsStoring())
    {
		if((HFONT)*pFont==NULL)
		{
	        ar << FALSE;
		}
		else
		{
	        ar << TRUE;
			if(!pFont->GetLogFont(&lf))
			{
				// Throw the same exception as before on older compilers, for compatibility
#if _MSC_VER >= 1400
				AfxThrowArchiveException(CArchiveException::badClass);
#else
				AfxThrowArchiveException(CArchiveException::generic);
#endif
			}
			ar << lf.lfHeight;
			ar << lf.lfWidth;
			ar << lf.lfEscapement;
			ar << lf.lfOrientation;
			ar << lf.lfWeight;
			ar << lf.lfItalic;
			ar << lf.lfUnderline;
			ar << lf.lfStrikeOut;
			ar << lf.lfCharSet;
			ar << lf.lfOutPrecision;
			ar << lf.lfClipPrecision;
			ar << lf.lfQuality;
			ar << lf.lfPitchAndFamily;
			CString string=lf.lfFaceName;
			ar << string;
		}
    }
    else
    {
		if ((HFONT)*pFont!=NULL) 
		{
			pFont->DeleteObject();
		}

		BOOL bFontWasSaved;
        ar >> bFontWasSaved;

		if(bFontWasSaved)
		{
			ar >> lf.lfHeight;
			ar >> lf.lfWidth;
			ar >> lf.lfEscapement;
			ar >> lf.lfOrientation;
			ar >> lf.lfWeight;
			ar >> lf.lfItalic;
			ar >> lf.lfUnderline;
			ar >> lf.lfStrikeOut;
			ar >> lf.lfCharSet;
			ar >> lf.lfOutPrecision;
			ar >> lf.lfClipPrecision;
			ar >> lf.lfQuality;
			ar >> lf.lfPitchAndFamily;
			CString string;
			ar >> string;
			try 
			{
				UTBStr::tcscpy(lf.lfFaceName, string.GetLength() + 1, string.GetBuffer(LF_FACESIZE));
			}
			catch(CMemoryException* e)
			{
				UNREFERENCED_PARAMETER(e);
				// Throw the same exception as before on older compilers, for compatibility
#if _MSC_VER >= 1400
				AfxThrowArchiveException(CArchiveException::badClass);
#else
				AfxThrowArchiveException(CArchiveException::generic);
#endif
			}
			if(!pFont->CreateFontIndirect(&lf))
			{
				// Throw the same exception as before on older compilers, for compatibility
#if _MSC_VER >= 1400
				AfxThrowArchiveException(CArchiveException::badClass);
#else
				AfxThrowArchiveException(CArchiveException::generic);
#endif
			}
		}
    }                 
} 
//////////////////////////////////////


IMPLEMENT_DYNAMIC(COXCaptionPainter, COXHookWnd);

// Constructor
COXCaptionPainter::COXCaptionPainter()
{
	Reset();

	m_bHackStyleSet=FALSE;
	m_bHackStyleExSet=FALSE;
	m_bAnimation=FALSE;
	m_bActive=TRUE;
	m_bUpdate=FALSE;

	m_sWindowText.Empty();
	m_nIdleFlags=0;
}

// Destructor
COXCaptionPainter::~COXCaptionPainter()
{
}

#ifndef PACKVERSION
	#define PACKVERSION(major,minor) MAKELONG(minor,major)
#endif

DWORD COXCaptionPainter::GetDllVersion(LPCTSTR lpszDllName)
{

    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    hinstDll = LoadLibrary(lpszDllName);
	
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;

        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

		/*Because some DLLs may not implement this function, you
		 *must test for it explicitly. Depending on the particular 
		 *DLL, the lack of a DllGetVersion function may
		 *be a useful indicator of the version.
		*/
        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

BOOL COXCaptionPainter::IsAppSkinned()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);

	if (osvi.dwMajorVersion >= 5 && osvi.dwMinorVersion >= 1)
	{
		// We have XP or higher

		// Call ::IsAppThemed()
		HMODULE hLib = ::LoadLibrary(_T("Uxtheme.dll"));
		typedef int (WINAPI* LPISAPPTHEMED) (void);
		LPISAPPTHEMED pIsAppThemed = (LPISAPPTHEMED) GetProcAddress(hLib, "IsAppThemed");
		BOOL bThemed = pIsAppThemed();
		::FreeLibrary(hLib);			
		
		if (bThemed)
		{
			// Check is we are using common controls v.6.0
			if(GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(6,00))
				return TRUE; // we have the XP skins on
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}

// Attach caption handler.
BOOL COXCaptionPainter::Attach(CWnd* pWnd)
{
	if (IsAppSkinned())
		return FALSE; // we have Windows XP or higher and the skins are turned on,
					  // making the caption painter obsolete

	ASSERT_VALID(pWnd);
	ASSERT(::IsWindow(pWnd->m_hWnd));

	DWORD dwStyle=pWnd->GetStyle();
	if((dwStyle&WS_CAPTION)==WS_CAPTION)
	{
		HookWindow(pWnd);
		m_nColorBits=GetNumColorBits();
		if(pWnd->SetTimer(IDT_UPDATECAPTION,ID_UPDATECAPTION_DELAY,NULL)!=
			IDT_UPDATECAPTION)
		{
			UnhookWindow();
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

// Detach caption handler. Called by default when hooked window is about
// to be destroyed.
void COXCaptionPainter::Detach()
{
	UnhookWindow();
}

// sets caption properties
void COXCaptionPainter::SetCaptionInfo(COXCaptionInfo* pCI, BOOL bActive) 
{ 
	ASSERT_VALID(pCI);

	if(bActive)
		m_ciActive=*pCI;
	else
		m_ciInactive=*pCI;
}

// retrievs any COXCaptionPainter object associated with any CWnd
BOOL COXCaptionPainter::GetCaptionPainter(CWnd* pWnd, COXCaptionPainter* pCP)
{
	ASSERT_VALID(pWnd);
	ASSERT(::IsWindow(pWnd->m_hWnd));

	return (BOOL)pWnd->SendMessage(m_nGetCaptionPainter,0,(LPARAM)pCP);
}

// sets properties of existed COXCaptionPainter object to the COXCaptionPainter
// associated with any window
BOOL COXCaptionPainter::SetCaptionPainter(CWnd* pWnd, COXCaptionPainter* pCP)
{
	ASSERT_VALID(pWnd);
	ASSERT(::IsWindow(pWnd->m_hWnd));

	return (BOOL)pWnd->SendMessage(m_nSetCaptionPainter,0,(LPARAM)pCP);
}

// Sets/removes hooked window's style 
void COXCaptionPainter::SetHackStyle(LONG dwStyle, UINT nAction)
{
	ASSERT(nAction==ID_OXHACK_ADDSTYLE || nAction==ID_OXHACK_REMOVESTYLE);
	ASSERT(::IsWindow(m_hWndHooked));
	ASSERT_VALID(this);

	if(!m_bHackStyleSet)
	{
		ASSERT(GetHookedWnd()!=NULL);

		// save original size
		m_dwSavedStyle=GetHookedWnd()->GetStyle();
		switch(nAction)
		{
		case ID_OXHACK_ADDSTYLE:
			{
				::SetWindowLongPtr(m_hWndHooked,GWL_STYLE,m_dwSavedStyle|dwStyle);
				break;
			}
		case ID_OXHACK_REMOVESTYLE:
			{
				::SetWindowLongPtr(m_hWndHooked,GWL_STYLE,(m_dwSavedStyle & ~dwStyle));
				break;
			}
		}
		m_bHackStyleSet=TRUE;
	}
}

// Sets hooked window's original style
void COXCaptionPainter::RemoveHackStyle()
{
	ASSERT(::IsWindow(m_hWndHooked));
	ASSERT_VALID(this);

	if(m_bHackStyleSet)
	{
		ASSERT(m_hWndHooked!=NULL);

		// have to put it here
		m_bHackStyleSet=FALSE;
		::SetWindowLongPtr(m_hWndHooked, GWL_STYLE, m_dwSavedStyle);
	}
}

// Sets/removes hooked window's extended style 
void COXCaptionPainter::SetHackStyleEx(LONG dwStyleEx, UINT nAction)
{
	ASSERT(nAction==ID_OXHACK_ADDSTYLE || nAction==ID_OXHACK_REMOVESTYLE);
	ASSERT(::IsWindow(m_hWndHooked));
	ASSERT_VALID(this);

	if(!m_bHackStyleExSet)
	{
		ASSERT(m_hWndHooked!=NULL);

		// save original extended size
		m_dwSavedStyleEx=GetHookedWnd()->GetExStyle();
		switch(nAction)
		{
		case ID_OXHACK_ADDSTYLE:
			{
				::SetWindowLongPtr(m_hWndHooked, GWL_EXSTYLE, 
					m_dwSavedStyleEx|dwStyleEx);
				break;
			}
		case ID_OXHACK_REMOVESTYLE:
			{
				::SetWindowLongPtr(m_hWndHooked, GWL_EXSTYLE, 
					m_dwSavedStyleEx & ~ dwStyleEx);
				break;
			}
		}
		m_bHackStyleExSet=TRUE;
	}
}

// Sets hooked window's original extended style
void COXCaptionPainter::RemoveHackStyleEx()
{
	ASSERT(::IsWindow(m_hWndHooked));
	ASSERT_VALID(this);

	if(m_bHackStyleExSet)
	{
		ASSERT(m_hWndHooked!=NULL);

		// have to put it here
		m_bHackStyleExSet=FALSE;
		::SetWindowLongPtr(m_hWndHooked, GWL_EXSTYLE, m_dwSavedStyleEx);
	}
}

// Handle animation effects
LRESULT COXCaptionPainter::HackAnimation(UINT msg, WPARAM wp, LPARAM lp)
{
	ASSERT(::IsWindow(m_hWndHooked));

	LRESULT result;

	if(!m_bAnimation)
	{
		// animation structure
		ANIMATIONINFO aniInfo;
		aniInfo.cbSize=sizeof(ANIMATIONINFO);
		SystemParametersInfo(SPI_GETANIMATION,sizeof(ANIMATIONINFO),&aniInfo,0);
		// save original state
		BOOL bAnimated=aniInfo.iMinAnimate==0 ? FALSE : TRUE;
		if(bAnimated)
		{
			aniInfo.iMinAnimate=0;
			SystemParametersInfo(SPI_SETANIMATION,sizeof(ANIMATIONINFO),
				&aniInfo,0);
		}
		
		result=COXHookWnd::WindowProc(msg,wp,lp);

		// animation
		if(bAnimated)
		{
			// load original state
			aniInfo.iMinAnimate=1;
			SystemParametersInfo(SPI_SETANIMATION,sizeof(ANIMATIONINFO),&aniInfo,0);
		}
	}
	else
	{
		result=COXHookWnd::WindowProc(msg,wp,lp);
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////
// Message handler handles caption-related and other messages
//
LRESULT COXCaptionPainter::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(::IsWindow(m_hWndHooked));

	// The purpose of SetHackStyle() and SetHackStyleEx() functions is to 
	// force Windows to handle some messages in a special way. As soon as 
	// the message is handled we have to restore the original settings calling 
	// RemoveHackStyle() and RemoveHackStyleEx(). The best place for that is 
	// when next message for hooked window will be handled. But when SetHackStyle() 
	// or SetHackStyleEx() are called then WM_STYLECHANGING and WM_STYLECHANGED 
	// messages are sent. So we have to treat them a little bit differently.
	if(msg!=WM_STYLECHANGING && msg!=WM_STYLECHANGED)
	{
		UINT oldMsg=msg;
		RemoveHackStyle();
		RemoveHackStyleEx();
		msg=oldMsg;
	}

	// first of all handle or registered messages
	if(msg==m_nGetCaptionPainter)
	{
		ASSERT_VALID((COXCaptionPainter*)lp);

		((COXCaptionPainter*)lp)->Reset();
		((COXCaptionPainter*)lp)->SetCaptionInfo(GetCaptionInfo(TRUE),TRUE);
		((COXCaptionPainter*)lp)->SetCaptionInfo(GetCaptionInfo(FALSE),FALSE);
		return TRUE;
	}
	else if(msg==m_nSetCaptionPainter)
	{
		ASSERT_VALID((COXCaptionPainter*)lp);

		Reset();
		SetCaptionInfo(((COXCaptionPainter*)lp)->GetCaptionInfo(TRUE),TRUE);
		SetCaptionInfo(((COXCaptionPainter*)lp)->GetCaptionInfo(FALSE),FALSE);
		return TRUE;
	}

	// Handle all messages which standard handlers could draw in caption rect.
	// I am sure you'll be amased to know that there are so many places where
	// Windows draws different element of caption. It is mostly due to problem
	// with MDI window. Just look through the code and you will figure out
	// how Windows inconsistent internally.
	switch (msg) 
	{
	case WM_HELP:
	case WM_MENUSELECT:
	case WM_INITMENUPOPUP:
	case WM_CANCELMODE:
		{
 			LRESULT result=COXHookWnd::WindowProc(msg,wp,lp);
			DrawCaption();
			return result;

		}

	case WM_SETCURSOR:
		{
			if(LOWORD(lp)==HTBOTTOM || LOWORD(lp)==HTBOTTOMLEFT || 
				LOWORD(lp)==HTBOTTOMRIGHT || LOWORD(lp)==HTLEFT || 
				LOWORD(lp)==HTRIGHT || LOWORD(lp)==HTTOP || 
				LOWORD(lp)==HTTOPLEFT || LOWORD(lp)==HTTOPRIGHT)
			{
				SetHackStyle(WS_CAPTION);
				LRESULT result=COXHookWnd::WindowProc(msg,wp,lp);
				RemoveHackStyle();
				return result;
			}
			break;
		}

	case WM_NCLBUTTONDOWN: 
		{
			return OnNCLButtonDown(wp,lp);
		}

	case WM_NCLBUTTONDBLCLK:
		{
			if(wp==HTCAPTION && (GetHookedWnd()->GetStyle()&WS_MAXIMIZEBOX)!=0)
			{
				LRESULT result=::SendMessage(m_hWndHooked,WM_SYSCOMMAND,
					(GetHookedWnd()->IsZoomed() ? SC_RESTORE : SC_MAXIMIZE),0);
				DrawCaption();
				return result;
			}
			break;
		}

	case WM_SYSCOMMAND:
		{
			if(wp==SC_MINIMIZE || wp==SC_MAXIMIZE || 
				wp==SC_RESTORE || wp==SC_CLOSE)
			{
				DrawCaption();
				if(wp!=SC_CLOSE)
				{
					return HackAnimation(msg,wp,lp);
				}
			}
			break;
		}

	case WM_COMMAND:
		{
			if(LOWORD(wp)==ID_WINDOW_TILE_HORZ || 
				LOWORD(wp)==ID_WINDOW_TILE_VERT || 
				LOWORD(wp)==ID_WINDOW_CASCADE || 
				LOWORD(wp)==ID_WINDOW_ARRANGE)
			{
				return HackAnimation(msg,wp,lp);
			}
			break;
		}

	case WM_SIZE:
		{
			if(GetHookedWnd()->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				if(wp==SIZE_MAXIMIZED || wp==SIZE_MINIMIZED || wp==SIZE_RESTORED)
				{
	 				LRESULT result=COXHookWnd::WindowProc(msg,wp,lp);
					DrawCaption();
					return result;
				}
			}
			break;
		}

	case WM_NCPAINT:
		{
			OnNcPaint(HRGN(wp));
			return 0;
		}

	case WM_NCACTIVATE:
		{
			return OnNcActivate((BOOL)wp);
		}

	case WM_SETTEXT:
		{
			OnSetText((LPCTSTR)lp);
			return TRUE;
		}

	case WM_SYSCOLORCHANGE:
	case WM_SETTINGCHANGE:
		{
			m_nColorBits=GetNumColorBits();
			Reset();
			DrawCaption();
			return 0;
		}

	case WM_STYLECHANGED:
		{
			LRESULT result=COXHookWnd::WindowProc(msg,wp,lp);
			Reset();
			DrawCaption();
			return result;
		}

	case WM_TIMER:
		{
			if(wp==IDT_UPDATECAPTION)
			{
				// force new bitmap
				CString sText;
				GetHookedWnd()->GetWindowText(sText);
				if(sText!=m_sWindowText)
				{
					Reset();							
					DrawCaption();
				}
				else
				{
					// redraw the toolbar if necessary
					if(m_nIdleFlags & oxidleRedrawCaption)
					{
						if(GetHookedWnd()->IsWindowVisible())
						{
							Reset();							
							DrawCaption();
						}
					}
				}
				return 0L;
			}
		}

	case WM_DESTROY:
		{
			GetHookedWnd()->KillTimer(IDT_UPDATECAPTION);
			break;
		}
	}

	// I don't handle it: pass along
	return COXHookWnd::WindowProc(msg,wp,lp);
}

/////////////////
// Handle WM_NCPAINT for main window
//
void COXCaptionPainter::OnNcPaint(HRGN hRgn)
{
	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();
	
	// caption rectangle in window coordinates
	CRect rect;		
	GetCaptionRect(rect);
	// window rect
	CRect rectWindow;				
	// .. get window rect
	wnd.GetWindowRect(&rectWindow);	
	// convert caption rect to screen coords
	rect+=rectWindow.TopLeft();		

	// Don't bother painting if the caption doesn't lie within the region.
	//
	if ((WORD)hRgn>1 && !::RectInRegion(hRgn,&rect)) 
	{
		// just do default thing
		Default();					
		return;						
	}

	// Exclude caption from update region
	//
	HRGN hRgnCaption=::CreateRectRgnIndirect(&rect);
	HRGN hRgnNew=::CreateRectRgnIndirect(&rect);
	if((WORD)hRgn>1) 
	{
		// wParam is a valid region: subtract caption from it
		::CombineRgn(hRgnNew, hRgn, hRgnCaption, RGN_DIFF);
	}
	else 
	{
		// wParam is not a valid region: create one that's the whole
		// window minus the caption bar
		//
		HRGN hRgnAll = ::CreateRectRgnIndirect(&rectWindow);
		CombineRgn(hRgnNew, hRgnAll, hRgnCaption, RGN_DIFF);
		DeleteObject(hRgnAll);
	}

	// Call Windows to do WM_NCPAINT with altered update region
	//
	MSG& msg=AfxGetThreadState()->m_lastSentMsg;
	// save original wParam
	WPARAM oldWP=msg.wParam;	
	// set new region for DefWindowProc
	msg.wParam=(WPARAM)hRgnNew;	
	Default();
	// clean up
	DeleteObject(hRgnCaption);		
	DeleteObject(hRgnNew);			
	// restore original wParam
	msg.wParam=oldWP;				

	// Now paint the special caption
	DrawCaption();					
}

//////////////////
// Handle WM_NCACTIVATE for main window
//
BOOL COXCaptionPainter::OnNcActivate(BOOL bActive)
{
	ASSERT(::IsWindow(m_hWndHooked));


	// Handle it differently for MDIFrame window
	if(GetHookedWnd()->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
	{
		CFrameWnd& frame=*((CFrameWnd*)GetHookedWnd());
		ASSERT_KINDOF(CFrameWnd, &frame);

		// Mimic MFC kludge to stay active if WF_STAYACTIVE bit is on
		if (frame.m_nFlags & WF_STAYACTIVE)
		{
			bActive = TRUE;
		}
		if (!frame.IsWindowEnabled())			
		{
			// but not if disabled
			bActive = FALSE;
		}
		if (bActive==m_bActive && bActive==FALSE)
		{
			// nothing to do
			DrawCaption();
			return TRUE;					
		}

		// In case this is a MDI app, manually activate/paint active MDI child
		// window, because Windows won't do it if parent frame is invisible.
		// Must do this BEFORE calling Default, or it will not work.
		//

		if (!m_bUpdate)
		{
			m_bUpdate=TRUE;
			CFrameWnd* pActiveFrame = frame.GetActiveFrame();
			if (pActiveFrame) 
			{
				pActiveFrame->SendMessage(WM_NCACTIVATE,TRUE);
				pActiveFrame->SendMessage(WM_NCPAINT, 1);
			}
			CWnd* pWnd=frame.GetNextWindow();
			CWnd* pStartWnd=pWnd;
			while (pWnd)
			{
				CFrameWnd* pFrame=DYNAMIC_DOWNCAST(CFrameWnd, pWnd);
				if (pFrame && pFrame!=pActiveFrame)
				{
					pFrame->SendMessage(WM_NCACTIVATE,FALSE);

				}
				pWnd=pWnd->GetNextWindow();

			}
			pWnd=pStartWnd;
			if (pWnd)
				pWnd=pWnd->GetNextWindow(GW_HWNDPREV);
			while (pWnd)
			{
				
				CFrameWnd* pFrame=DYNAMIC_DOWNCAST(CFrameWnd, pWnd);
				if (pFrame /*&& pFrame!=pActiveFrame*/)
				{
					pFrame->SendMessage(WM_NCACTIVATE,FALSE);

				}
				pWnd=pWnd->GetNextWindow(GW_HWNDPREV);

			}
			m_bUpdate=FALSE;
		}
		// Turn WS_CAPTION off before calling DefWindowProc,
		// so DefWindowProc won't paint and thereby cause flicker.
		//

		MSG& msg=AfxGetThreadState()->m_lastSentMsg;
		msg.wParam=bActive;
		Default();

//		RemoveHackStyle();

		// At this point, nothing has happened (since WS_CAPTION was off).
		// Now it's time to paint.
		//
		// update state
		m_bActive = bActive;			
		// paint non-client area (frame too)
		frame.SendMessage(WM_NCPAINT);	
		return TRUE;						
	}
	else
	{
		CWnd& wnd = *GetHookedWnd();

		if (bActive==m_bActive)
		{
			// nothing to do
			return TRUE;					
		}

//		SetHackStyle(WS_CAPTION);

		MSG& msg=AfxGetThreadState()->m_lastSentMsg;
		msg.wParam=bActive;
		Default();

//		RemoveHackStyle();

		// Now it's time to paint.
		//
		// update state
		m_bActive = bActive;			
		// paint non-client area (frame too)
		wnd.SendMessage(WM_NCPAINT);	
		return TRUE;						
	}
}

//////////////////
// Handle WM_SETTEXT for main window
//
void COXCaptionPainter::OnSetText(LPCTSTR lpText)
{
	UNREFERENCED_PARAMETER(lpText);

	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();

	// Turn WS_CAPTION style off before calling Windows to set the text, 
	// then turn it back on again after.
	//
	// When MDIChild window is maximized then it draws the caption of 
	// its parent, MDIFrame window. if this is a case then we have to 
	// apply hack to parent window.
	CMDIFrameWnd* pFrameWnd=NULL;
	BOOL bMDIFrameChanged=FALSE;
	BOOL bMaximizedMDIChild=FALSE;
	BOOL bIsMDIChild=wnd.IsKindOf(RUNTIME_CLASS(CMDIChildWnd));
	BOOL bIsMDIFrame=wnd.IsKindOf(RUNTIME_CLASS(CMDIFrameWnd));
	ASSERT(!(bIsMDIChild&bIsMDIFrame));
	if(bIsMDIChild)
	{
		pFrameWnd=((CMDIChildWnd*)&wnd)->GetMDIFrame();
		if(pFrameWnd!=NULL)
		{
		    BOOL bMaximize=FALSE;
			CWnd* pActiveWnd=pFrameWnd->MDIGetActive(&bMaximize);

			if(pActiveWnd->GetSafeHwnd()==wnd.GetSafeHwnd() && bMaximize)
			{
				pFrameWnd->ModifyStyle(WS_VISIBLE,0);
				bMDIFrameChanged=TRUE;
			}
		}
	}
	else if(bIsMDIFrame)
	{
		pFrameWnd=(CMDIFrameWnd*)GetHookedWnd();
		BOOL bMaximize=FALSE;
		CWnd* pActiveWnd=pFrameWnd->MDIGetActive(&bMaximize);

		if(pActiveWnd!=NULL && bMaximize)
		{
			SetHackStyle(WS_VISIBLE);
			bMaximizedMDIChild=TRUE;
		}
	}

	if(!bMDIFrameChanged && !bMaximizedMDIChild)
	{
		SetHackStyle(WS_CAPTION);
	}

	Default();
	
	if(bMDIFrameChanged)
	{
		pFrameWnd->ModifyStyle(0,WS_VISIBLE);
	}
	else
	{
		RemoveHackStyle();
	}

	// force new bitmap
	Reset();							
	DrawCaption();
}

//////////////////
// Handle WM_NCLBUTTONDOWN for main window
//
LRESULT COXCaptionPainter::OnNCLButtonDown(WPARAM wp, LPARAM lp)
{
	// Treat caption area in a special way
	if(wp==HTCAPTION)
	{
		if(!m_bActive)
		{
			if(!GetHookedWnd()->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
			{
				GetHookedWnd()->SendMessage(WM_ACTIVATE,TRUE);
				DrawCaption();
			}
			else
			{
				CWnd* pWnd=GetHookedWnd()->GetParent();
				if(pWnd!=NULL)
				{
					pWnd->SendMessage(WM_ACTIVATE,TRUE);
					DrawCaption();
				}
			}
		}
		// It sounds crazy but we have to handle it this way.
		if(m_nColorBits<=8 && 
			!GetHookedWnd()->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
		{
			return GetHookedWnd()->SendMessage(WM_SYSCOMMAND,SC_MOVE);
		}
	}
	// area of caption's buttons is treated differently
	if(wp!=HTCLOSE && wp!=HTHELP && wp!=HTMAXBUTTON && 
		wp!=HTMINBUTTON && wp!=HTREDUCE && wp!=HTZOOM)
	{
		SetHackStyle(WS_CAPTION);
	}
	
	LRESULT result=COXHookWnd::WindowProc(WM_NCLBUTTONDOWN, wp, lp);

	// in result of operation the window might have been destroyed
	if(!IsHooked())
		return result;
	
	if(wp!=HTCLOSE && wp!=HTHELP && wp!=HTMAXBUTTON && 
		wp!=HTMINBUTTON && wp!=HTREDUCE && wp!=HTZOOM)
	{
		RemoveHackStyle();
	}
	if(wp==HTHELP)
	{
		DrawCaption();
	}
	return result;
}

//////////////////
// Paint custom caption. Flag tells whether active or not. Just blast the
// bitmap to the title bar, but not if minimized (iconic).
//
void COXCaptionPainter::DrawCaption()
{
	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();

	m_nIdleFlags&=~oxidleRedrawCaption;

	// Get caption DC and rectangle
	//
	// window DC
	CWindowDC dc(&wnd);					
	// memory DC
	CDC dcCompatible;										
	dcCompatible.CreateCompatibleDC(&dc);

	CRect rect;		
	// get caption rectangle
	GetCaptionRect(rect);

	// if the saved size of caption doesn't match the real one then mark 
	// bitmaps for both active/inactive state as "dirty"
	if(m_sizeCaption!=rect.Size())
	{
		m_sizeCaption=rect.Size();
		// invalidate bitmaps
		m_bmActive.DeleteObject();	
		m_bmInactive.DeleteObject();
	}

	// Get active/inactive bitmap & determine if needs to be regenerated
	CBitmap &bm=m_bActive ? m_bmActive : m_bmInactive;
	COXCaptionInfo &ci=m_bActive ? m_ciActive : m_ciInactive;
	BOOL bPaint=FALSE;
	if(bm.GetSafeHandle()==NULL) 
	{
		// no bitmap:
		bm.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		bPaint=TRUE;								
	}
	// select bitmap into memory DC
	CBitmap* pOldBitmap=dcCompatible.SelectObject(&bm);	

	// If bitmap needs painting then do it.
	if (bPaint) 
	{
		// first of all draw background
		DrawCaptionBackground(&dcCompatible,&ci,&rect);
		CSize sizeConstraints(0,0);
		CSize sizeCaption(rect.Width(),rect.Height());
		// then draw icon
		sizeConstraints.cx=DrawCaptionIcon(&dcCompatible,&ci,&sizeCaption);
		sizeConstraints.cy=DrawCaptionButtons(&dcCompatible,&ci,&sizeCaption);
		DrawCaptionText(&dcCompatible,&ci,&sizeCaption,&sizeConstraints);
	}

	// blast bits to screen
	dc.BitBlt(rect.left,rect.top,rect.Width(),rect.Height(),
		&dcCompatible,0,0,SRCCOPY);
	dcCompatible.SelectObject(pOldBitmap); // restore DC
}

////////////////
// Draw caption background.
//
// Compute new color brush for each band from x to x + xDelta.
// Excel uses a linear algorithm from black to normal, i.e.
//
//		color = CaptionColor * r
//
// where r is the ratio x/w, which ranges from 0 (x=0, left)
// to 1 (x=w, right). This results in a mostly black title bar,
// since we humans don't distinguish dark colors as well as light
// ones. So instead, I use the formula
//
//		color = CaptionColor * [1-(1-r)^2]
//
// which still equals black when r=0 and CaptionColor when r=1,
// but spends more time near CaptionColor. For example, when r=.5,
// the multiplier is [1-(1-.5)^2] = .75, closer to 1 than .5.
// I leave the algebra to the reader to verify that the above formula
// is equivalent to
//
//		color = CaptionColor - (CaptionColor*(w-x)*(w-x))/(w*w)
//
// The computation looks horrendous, but it's only done once each
// time the caption changes size; thereafter BitBlt'ed to the screen.
//
void COXCaptionPainter::DrawCaptionBackground(CDC* pDC, const COXCaptionInfo* pCI,
											  const CRect* pCaptionRect)
{
	ASSERT(::IsWindow(m_hWndHooked));


	ASSERT_VALID(pCI);
	ASSERT(pDC);

	int cxCap=pCaptionRect->Width();
	int cyCap=pCaptionRect->Height();

	// red, green and blue color vals
	COLORREF clrBackground=pCI->GetBackgroundColor(m_bActive);
	int red=GetRValue(clrBackground);				
	int green=GetGValue(clrBackground);				
	int blue=GetBValue(clrBackground);		
	
	if(!pCI->GetGradient())
	{
		PaintRect(pDC,0,0,cxCap,cyCap,clrBackground);
	}
	else
	{
		int nCurBlock;
		// width of area to shade and width squared
		int nWidth, nWidth_x_2;				
		// width of one shade band
		int nDelta;	
		UINT nAlignment=pCI->GetGradientAlignment();
		UINT nAlgorithm=pCI->GetGradientAlgorithm();
		UINT nNumberShade=pCI->GetNumberShade();

		switch(nAlignment)
		{
		case ID_GRADIENT_LEFT:
			{
				nCurBlock=cxCap;
				nWidth=cxCap;
				nWidth_x_2=cxCap*cxCap;
				nDelta=__max(nWidth/nNumberShade,1);

				while(nCurBlock>0) 
				{
					switch(nAlgorithm)
					{
					case ID_GRADIENT_LINEAR:
						{
							// paint bands right to left
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB((red*nCurBlock)/nWidth, 
								(green*nCurBlock)/nWidth, 
								(blue*nCurBlock)/nWidth));

							break;
						}
					case ID_GRADIENT_SQUARE:
						{
							// paint bands right to left
							int nRest_x_2=(nWidth-nCurBlock)*(nWidth-nCurBlock);
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nRest_x_2)/nWidth_x_2, 
								green-(green*nRest_x_2)/nWidth_x_2, 
								blue-(blue*nRest_x_2)/nWidth_x_2));

							break;
						}
					}
					// next band
					nCurBlock-=nDelta;							
				}
				// whatever's left ==> black
				PaintRect(pDC,0,0,nCurBlock+nDelta,cyCap,RGB(0,0,0));  

				break;
			}
		case ID_GRADIENT_CENTER:
			{
				nCurBlock=cxCap/2;
				nWidth=cxCap/2;
				nWidth_x_2=cxCap*cxCap/4;
				nDelta=__max(nWidth/(2*nNumberShade),1);

				while(nCurBlock>0) 
				{
					switch(nAlgorithm)
					{
					case ID_GRADIENT_LINEAR:
						{
							// paint bands right to left
							PaintRect(pDC, nWidth+nCurBlock, 0, nDelta, cyCap,	
								RGB((red*nCurBlock)/nWidth, 
								(green*nCurBlock)/nWidth, 
								(blue*nCurBlock)/nWidth));

							break;
						}
					case ID_GRADIENT_SQUARE:
						{
							// paint bands right to left
							int nRest_x_2=(nWidth-nCurBlock)*(nWidth-nCurBlock);
							PaintRect(pDC, nWidth+nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nRest_x_2)/nWidth_x_2, 
								green-(green*nRest_x_2)/nWidth_x_2, 
								blue-(blue*nRest_x_2)/nWidth_x_2));

							break;
						}
					}
					// next band
					nCurBlock-=nDelta;							
				}
				// whatever's left ==> black
				PaintRect(pDC,nWidth,0,nCurBlock+nDelta,cyCap,RGB(0,0,0));  

				nCurBlock=0;
				while(nCurBlock<=nWidth) 
				{
					switch(nAlgorithm)
					{
					case ID_GRADIENT_LINEAR:
						{
							// paint bands left to right
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nCurBlock)/nWidth, 
								green-(green*nCurBlock)/nWidth, 
								blue-(blue*nCurBlock)/nWidth));

							break;
						}
					case ID_GRADIENT_SQUARE:
						{
							// paint bands left to right
							int nRest_x_2=nCurBlock*nCurBlock;
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nRest_x_2)/nWidth_x_2, 
								green-(green*nRest_x_2)/nWidth_x_2, 
								blue-(blue*nRest_x_2)/nWidth_x_2));

							break;
						}
					}
					// next band
					nCurBlock+=nDelta;							
				}
				// whatever's left ==> black
				PaintRect(pDC,nCurBlock-nDelta,0,nWidth-nCurBlock+nDelta,
					cyCap,RGB(0,0,0));  

				break;
			}
		case ID_GRADIENT_RIGHT:
			{
				nCurBlock=0;
				nWidth=cxCap;
				nWidth_x_2=cxCap*cxCap;
				nDelta=__max(nWidth/nNumberShade,1);

				while(nCurBlock<nWidth) 
				{
					switch(nAlgorithm)
					{
					case ID_GRADIENT_LINEAR:
						{
							// paint bands left to right
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nCurBlock)/nWidth, 
								green-(green*nCurBlock)/nWidth, 
								blue-(blue*nCurBlock)/nWidth));

							break;
						}
					case ID_GRADIENT_SQUARE:
						{
							// paint bands left to right
							int nRest_x_2=nCurBlock*nCurBlock;
							PaintRect(pDC, nCurBlock, 0, nDelta, cyCap,	
								RGB(red-(red*nRest_x_2)/nWidth_x_2, 
								green-(green*nRest_x_2)/nWidth_x_2, 
								blue-(blue*nRest_x_2)/nWidth_x_2));

							break;
						}
					}
					// next band
					nCurBlock+=nDelta;							
				}
				// whatever's left ==> black
				PaintRect(pDC,nCurBlock-nDelta,0,nWidth-nCurBlock+nDelta-1,
					cyCap,RGB(0,0,0));  

				break;
			}
		}
	}

}

////////////////
// Draw caption icon
//
int COXCaptionPainter::DrawCaptionIcon(CDC* pDC, const COXCaptionInfo* pCI,
										const CSize* pCaptionSize)
{
	UNREFERENCED_PARAMETER(pCI);
	UNREFERENCED_PARAMETER(pCaptionSize);

	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd = *GetHookedWnd();

	ASSERT_VALID(pCI);
	ASSERT(pDC);

	if((wnd.GetStyle() & WS_SYSMENU)!=0)
	{
		HICON hIcon = (HICON)(UINT_PTR)GetClassLongPtr(wnd.m_hWnd, GCL_HICONSM);
		if(hIcon==NULL)
			hIcon=wnd.GetIcon(FALSE);
		if(hIcon)
		{
			// Within the basic button rectangle, Windows 95 uses a 1 or 2 pixel border
			// Icon has 2 pixel border on left, 1 pixel on top/bottom, 0 right
			//
			int cxIcon=GetSystemMetrics(SM_CXSIZE);
			CRect rect(0, 0, cxIcon, GetSystemMetrics(SM_CYSIZE));
			rect.DeflateRect(2,1,0,1);

			DrawIconEx(pDC->m_hDC, rect.left, rect.top,	hIcon, 
				rect.Width(), rect.Height(), 0, NULL, DI_NORMAL);
		
			return cxIcon+2;
		}
	}

	return 2;
}

////////////////
// Draw min, max/restore, close buttons.
//
int COXCaptionPainter::DrawCaptionButtons(CDC* pDC, const COXCaptionInfo* pCI,
										   const CSize* pCaptionSize)
{
	UNREFERENCED_PARAMETER(pCI);

	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();

	ASSERT_VALID(pCI);
	ASSERT(pDC);

	int nButtonsWidth=0;

	DWORD dwStyle=wnd.GetStyle();

	int cxIcon;
	int cyIcon;
	if((GetHookedWnd()->GetExStyle() & WS_EX_TOOLWINDOW)==WS_EX_TOOLWINDOW)
	{
		cxIcon=GetSystemMetrics(SM_CXSMSIZE);
		cyIcon=GetSystemMetrics(SM_CYSMSIZE);
	}
	else
	{
		cxIcon=GetSystemMetrics(SM_CXSIZE);
		cyIcon=GetSystemMetrics(SM_CYSIZE);
	}

	BOOL bCloseBox=(dwStyle & WS_SYSMENU)!=0;
	BOOL bMaxBox=bCloseBox & 
		((dwStyle&WS_MAXIMIZEBOX)!=0 || (dwStyle&WS_MINIMIZEBOX)!=0);
	BOOL bMinBox=bCloseBox & 
		((dwStyle&WS_MINIMIZEBOX)!=0 || (dwStyle&WS_MAXIMIZEBOX)!=0);
	BOOL bHelpBox=bCloseBox & !bMaxBox & !bMinBox & 
		((wnd.GetExStyle()&WS_EX_CONTEXTHELP)!=0);

	//changed 11/17/99
	BOOL bCloseBoxGrayed=FALSE;
	BOOL bMaxBoxGrayed=FALSE;
	BOOL bMinBoxGrayed=FALSE;
	BOOL bHelpBoxGrayed=FALSE;

	// Draw caption buttons. These are all drawn inside a rectangle
	// of dimensions SM_CXSIZE by SM_CYSIZE
	CRect rect(pCaptionSize->cx-cxIcon, 0, pCaptionSize->cx, cyIcon);
	if(bCloseBox)
	{
		// Close box has a 2 pixel border on all sides but left, which is zero
		rect.DeflateRect(0,2,2,2);
		UINT nState=DFCS_CAPTIONCLOSE;
		nState|=(bCloseBoxGrayed?DFCS_INACTIVE:NULL);
		pDC->DrawFrameControl(&rect, DFC_CAPTION, nState);
		nButtonsWidth+=cxIcon;
	}


	// Max/restore button is like close box; just shift rectangle left
	if(bMaxBox) 
	{
		ASSERT(bCloseBox);

		rect-=CPoint(cxIcon,0);
		UINT nState=wnd.IsZoomed() ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMAX;
		if((dwStyle&WS_MAXIMIZEBOX)==0)
		{
			ASSERT(bMinBox);
			nState|=DFCS_INACTIVE;
		}
		nState|=(bMaxBoxGrayed?DFCS_INACTIVE:NULL);
		pDC->DrawFrameControl(&rect, DFC_CAPTION, nState);
		nButtonsWidth+=cxIcon;
	}

	// Minimize button has 2 pixel border on all sides but right.
	if(bMinBox) 
	{
		ASSERT(bCloseBox);

		rect-=CPoint(cxIcon-2,0);
		UINT nState=(wnd.IsIconic() ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMIN);
		if((dwStyle&WS_MINIMIZEBOX)==0)
		{
			ASSERT(bMaxBox);
			nState|=DFCS_INACTIVE;
		}
		nState|=(bMinBoxGrayed?DFCS_INACTIVE:NULL);
		pDC->DrawFrameControl(&rect, DFC_CAPTION, nState);
		nButtonsWidth+=cxIcon;
	}

	// help button, if any.
	if(bHelpBox) 
	{
		ASSERT(bCloseBox);

		rect-=CPoint(cxIcon, 0);
		UINT nState=DFCS_CAPTIONHELP;
		nState|=(bHelpBoxGrayed?DFCS_INACTIVE:NULL);
		pDC->DrawFrameControl(&rect, DFC_CAPTION, nState);
		nButtonsWidth+=cxIcon;
	}

	return nButtonsWidth;
}

////////////////
// Draw caption background.
//
void COXCaptionPainter::DrawCaptionText(CDC* pDC, const COXCaptionInfo* pCI,
										const CSize* pCaptionSize, 
										const CSize* pIndentsSize)
{
	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();

	
	ASSERT_VALID(pCI);
	ASSERT(pDC);

	int cxCap=pCaptionSize->cx-(pIndentsSize->cx+pIndentsSize->cy);
	int cyCap=pCaptionSize->cy;

	wnd.GetWindowText(m_sWindowText);


	if(m_sWindowText.IsEmpty() || cxCap<=0)
		return;

	CFont font;
	CFont* pOldFont=NULL;
	BOOL bUseFont=FALSE;
	LOGFONT lf;
	BOOL bResult=FALSE;
	if((GetHookedWnd()->GetExStyle() & WS_EX_TOOLWINDOW)==WS_EX_TOOLWINDOW)
	{
		bResult=pCI->GetSmCaptionLogFont(&lf);
	}
	else
	{
		bResult=pCI->GetCaptionLogFont(&lf);
	}
	if(bResult && font.CreateFontIndirect(&lf))
	{
		pOldFont=pDC->SelectObject(&font);
		bUseFont=TRUE;
	}

	COLORREF clr=pCI->GetTextColor(m_bActive);
	pDC->SetTextColor(clr);					

	CRect rect(pIndentsSize->cx,0,pIndentsSize->cx+cxCap,cyCap);

	// draw on top of our shading
	pDC->SetBkMode(TRANSPARENT);					
	pDC->DrawText(m_sWindowText, &rect, pCI->GetTextFormat());

	if(bUseFont)
	{
		ASSERT(pOldFont!=NULL);
		// Restore DC
		pDC->SelectObject(pOldFont);
	}
}

void COXCaptionPainter::GetCaptionRect(CRect& rect)
{
	ASSERT(::IsWindow(m_hWndHooked));
	CWnd& wnd=*GetHookedWnd();

	DWORD dwStyle=wnd.GetStyle();
	// Get size of frame around window
	CSize szFrame=(dwStyle & WS_THICKFRAME) ?	
		CSize(GetSystemMetrics(SM_CXSIZEFRAME), GetSystemMetrics(SM_CYSIZEFRAME)) :
		CSize(GetSystemMetrics(SM_CXFIXEDFRAME), GetSystemMetrics(SM_CYFIXEDFRAME));

	// Compute rectangle
	//
	// window rect in screen coords
	wnd.GetWindowRect(rect);		
	// shift origin to (0,0)
	rect-=CPoint(rect.left,rect.top);	
	// frame
	rect.left+=szFrame.cx;				
	// frame
	rect.right-=szFrame.cx;				
	// top = end of frame
	rect.top+=szFrame.cy;				
	// height of caption minus gray shadow border
	rect.bottom=rect.top+
		(((GetHookedWnd()->GetExStyle() & WS_EX_TOOLWINDOW)==WS_EX_TOOLWINDOW) ? 
		GetSystemMetrics(SM_CYSMCAPTION) : GetSystemMetrics(SM_CYCAPTION))-
		GetSystemMetrics(SM_CYBORDER);				  

	if(wnd.IsIconic())
	{
		rect.InflateRect(1,1,1,-1);
	}
}

UINT COXCaptionPainter::GetNumColorBits() 
{
	ASSERT(::IsWindow(m_hWndHooked));

	// get the number of bits in current system pallete
	CWindowDC dc(GetHookedWnd());
	return dc.GetDeviceCaps(BITSPIXEL);
}

//////////////////////////////////////
#ifndef OXCP_NO_SAVESTATE
//////////////////////////////////////
BOOL COXCaptionPainter::SaveState(LPCTSTR lpszProfileName)
{
#ifndef _MAC
	CWinApp* pApp=AfxGetApp();

	// make sure you called CWinApp::SetRegistryKey() functions before
	if(pApp->m_pszRegistryKey==NULL || pApp->m_pszProfileName==NULL)
	{
		TRACE(_T("COXCaptionPainter::SaveState: haven't called	SetRegistryKey()\n"));
		return FALSE;
	}
	// we use default registry key assigned to your application by MFC
	HKEY hSecKey=pApp->GetSectionKey(_T(""));
	if (hSecKey==NULL)
	{
		TRACE(_T("COXCaptionPainter::SaveState: unable to get section key\n"));
		return FALSE;
	}

	BOOL bResult=TRUE;
	try
	{
		COXRegistryValFile regActive(hSecKey, lpszProfileName, _T("Active"));
		CArchive arActive(&regActive, CArchive::store);
		m_ciActive.Serialize(arActive);
		arActive.Close();

		COXRegistryValFile regInactive(hSecKey, lpszProfileName, _T("Inactive"));
		CArchive arInactive(&regInactive, CArchive::store);
		m_ciInactive.Serialize(arInactive);
		arInactive.Close();
	}
	catch(CException* e)
	{
		UNREFERENCED_PARAMETER(e);
		bResult=FALSE;
	}

	::RegCloseKey(hSecKey);

	return bResult;
#else
	return FALSE;
#endif
}

BOOL COXCaptionPainter::LoadState(LPCTSTR lpszProfileName, BOOL bApply/*=TRUE*/)
{
#ifndef _MAC
	CWinApp* pApp=AfxGetApp();

	// make sure you called CWinApp::SetRegistryKey() functions before
	if(pApp->m_pszRegistryKey==NULL || pApp->m_pszProfileName==NULL)
	{
		TRACE(_T("COXCaptionPainter::SaveState: haven't called	SetRegistryKey()\n"));
		return FALSE;
	}
	// we use default registry key assigned to your application by MFC
	HKEY hSecKey=pApp->GetSectionKey(_T(""));
	if (hSecKey==NULL)
	{
		TRACE(_T("COXCaptionPainter::SaveState: unable to get section key\n"));
		return FALSE;
	}
	
	BOOL bResult=TRUE;
	try
	{
		COXRegistryValFile regActive(hSecKey, lpszProfileName, _T("Active"));
		if(regActive.GetLength()>0)
		{
			CArchive arActive(&regActive, CArchive::load);
			m_ciActive.Serialize(arActive);
			arActive.Close();
		}

		COXRegistryValFile regInactive(hSecKey, lpszProfileName, _T("Inactive"));
		if(regInactive.GetLength()>0)
		{
			CArchive arInactive(&regInactive, CArchive::load);
			m_ciInactive.Serialize(arInactive);
			arInactive.Close();
		}
	}
	catch(CException* e)
	{
		UNREFERENCED_PARAMETER(e);
		bResult=FALSE;
	}

	::RegCloseKey(hSecKey);

	if(bResult && bApply)
	{
		Reset();
		CWnd* pWnd = GetHookedWnd();
		if (pWnd != NULL)
			pWnd->SendMessage(WM_NCPAINT);
	}

	return bResult;
#else
	return FALSE;
#endif
}
//////////////////////////////////////
#endif // OXCP_NO_SAVESTATE
//////////////////////////////////////


//////////////////////////////////////////////////////////////////

//////////////////
// Helper to paint rectangle with a color.
//
static void PaintRect(CDC* pDC, int x, int y, int w, int h, COLORREF color)
{
	CBrush brush(color);
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(x, y, w, h, PATCOPY);
	pDC->SelectObject(pOldBrush);
}

BOOL IsSmallFont(BOOL& bIsSmall)
{
	CWnd* pWnd=CWnd::GetDesktopWindow();
	if(pWnd)
	{
		CDC* pDC=pWnd->GetWindowDC();
		if(pDC)
		{
			TEXTMETRIC tm;
			if(pDC->GetTextMetrics(&tm))
			{
				if(tm.tmHeight>16)
				{
					bIsSmall=FALSE;
				}
				else
				{
					bIsSmall=TRUE;
				}
				pWnd->ReleaseDC(pDC);
				return TRUE;
			}
			pWnd->ReleaseDC(pDC);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

// static variables
CMap<DWORD,DWORD,COXCaptionPainterOrganizer*,COXCaptionPainterOrganizer*> 
COXCaptionPainterOrganizer::m_arrThreadOrganizers;

HHOOK COXCaptionPainterOrganizer::m_pfnOriginalCBTHookProc=NULL;
HHOOK COXCaptionPainterOrganizer::m_pfnOriginalGetMessageHookProc=NULL;
/////////////////

COXCaptionPainterOrganizer::~COXCaptionPainterOrganizer() 
{ 
	if(IsAttachedAllInThread())
	{
		DetachAllInThread();
	}
	else
	{
		Detach(NULL); 
	}

	int nCount= (int)m_arrUsedPainters.GetSize();
	for(int nIndex=0; nIndex<nCount; nIndex++)
	{
		COXCaptionPainter* pPainter=m_arrUsedPainters[nIndex];
		ASSERT(pPainter!=NULL);
		ASSERT(!pPainter->IsHooked());
		delete pPainter;
	}
	m_arrUsedPainters.RemoveAll();


	ASSERT(m_pfnOriginalCBTHookProc==NULL);
	ASSERT(m_pfnOldCBTHookProc==NULL);
	ASSERT(m_pfnOriginalGetMessageHookProc==NULL);
}


COXCaptionPainter* COXCaptionPainterOrganizer::Attach(CWnd* pWnd)
{
	ASSERT(pWnd!=NULL);
	if(pWnd==NULL)
		return NULL;

	HWND hWndAttached=pWnd->GetSafeHwnd();
	ASSERT(::IsWindow(hWndAttached));
	COXCaptionPainter* pPainter=NULL;
	if(m_arrAttachedWnd.Lookup(hWndAttached,pPainter))
	{
		ASSERT(pPainter!=NULL);
		TRACE(_T("COXCaptionPainterOrganizer::Attach: specified window already attached to a caption painter object\n"));
		return pPainter;
	}

	if(m_arrUsedPainters.GetSize()>0)
	{
		pPainter=m_arrUsedPainters[0];
		ASSERT(pPainter!=NULL);
		ASSERT(!pPainter->IsHooked());
		m_arrUsedPainters.RemoveAt(0);
	}
	else
	{
		pPainter=new COXCaptionPainter;
	}

	if(pPainter->Attach(pWnd))
	{
		m_arrAttachedWnd.SetAt(hWndAttached,pPainter);
		return pPainter;
	}
	else
	{
		// save the object in the array of COXCaptionPainter objects 
		// that can be used later
		m_arrUsedPainters.Add(pPainter);
		return NULL;
	}
}

BOOL COXCaptionPainterOrganizer::Detach(const CWnd* pWnd/*=NULL*/, 
										BOOL bRedraw/*=TRUE*/)
{
	if(pWnd==NULL)
	{
		POSITION pos=m_arrAttachedWnd.GetStartPosition();
		HWND hAttachedWnd=NULL;
		COXCaptionPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hAttachedWnd,pPainter);
			ASSERT(::IsWindow(hAttachedWnd));
			ASSERT(pPainter!=NULL);
			if(bRedraw)
			{
				WINDOWPOS wPos;
				::ZeroMemory(&wPos, sizeof(wPos));
				CRect rect;
				::GetWindowRect(hAttachedWnd,&rect);
				wPos.cx=rect.Width();
				wPos.cy=rect.Height();
				wPos.x=rect.left;
				wPos.y=rect.top;
				wPos.hwnd=hAttachedWnd;
				wPos.flags=SWP_DRAWFRAME | SWP_FRAMECHANGED;
				::SendMessage(hAttachedWnd,WM_WINDOWPOSCHANGED,
				NULL, (LPARAM) &wPos);
				HDC hDC=::GetWindowDC(hAttachedWnd);
				::SendMessage(hAttachedWnd,WM_ERASEBKGND,(WPARAM) hDC, NULL);
				pPainter->Reset();
				::ReleaseDC(hAttachedWnd,hDC);
			}
			if(pPainter!=NULL)
			{
				if(pPainter->IsHooked())
					pPainter->Detach();
				// save the object in the array of COXCaptionPainter objects 
				// that can be used later
				m_arrUsedPainters.Add(pPainter);
			}
		}
		m_arrAttachedWnd.RemoveAll();
	}
	else
	{
		COXCaptionPainter* pPainter=NULL;
		CWnd* pAttachedWnd=(CWnd*)pWnd;
		if(!m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
			return FALSE;
		ASSERT(pPainter!=NULL);
		m_arrAttachedWnd.RemoveKey(pAttachedWnd->GetSafeHwnd());
		if(bRedraw && ::IsWindow(pAttachedWnd->GetSafeHwnd()))
		{
			pPainter->Reset();
			pAttachedWnd->SendMessage(WM_NCPAINT);
		}
		if(pPainter!=NULL)
		{
			if(pPainter->IsHooked())
				pPainter->Detach();
			// save the object in the array of COXCaptionPainter objects 
			// that can be used later
			m_arrUsedPainters.Add(pPainter);
		}
	}

	return TRUE;
}

BOOL COXCaptionPainterOrganizer::SetCaptionInfo(const CWnd* pWnd, 
												COXCaptionInfo* pCI, 
												BOOL bActive, 
												BOOL bRedraw/*=TRUE*/) const
{
	if(pWnd==NULL)
	{
		POSITION pos=m_arrAttachedWnd.GetStartPosition();
		HWND hAttachedWnd=NULL;
		COXCaptionPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hAttachedWnd,pPainter);
			ASSERT(::IsWindow(hAttachedWnd));
			ASSERT(pPainter!=NULL);
			if(pPainter!=NULL)
				pPainter->SetCaptionInfo(pCI,bActive);
			if(bRedraw)
			{
				pPainter->Reset();
				::SendMessage(hAttachedWnd,WM_NCPAINT,NULL,NULL);
			}
		}
	}
	else
	{
		COXCaptionPainter* pPainter=NULL;
		CWnd* pAttachedWnd=(CWnd*)pWnd;
		if(!m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
			return FALSE;
		ASSERT(pPainter!=NULL);
		if(pPainter!=NULL)
			pPainter->SetCaptionInfo(pCI,bActive);
		if(bRedraw && ::IsWindow(pAttachedWnd->GetSafeHwnd()))
		{
			pPainter->Reset();
			pAttachedWnd->SendMessage(WM_NCPAINT);
		}
	}

	return TRUE;
}


BOOL COXCaptionPainterOrganizer::Reset(CWnd* pWnd, BOOL bRedraw/*=TRUE*/) const
{
	if(pWnd==NULL)
	{
		POSITION pos=m_arrAttachedWnd.GetStartPosition();
		HWND hAttachedWnd=NULL;
		COXCaptionPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hAttachedWnd,pPainter);
			ASSERT(::IsWindow(hAttachedWnd));
			ASSERT(pPainter!=NULL);
			if(pPainter!=NULL)
				pPainter->Reset();
			if(bRedraw)
			{
				::SendMessage(hAttachedWnd,WM_NCPAINT,NULL,NULL);
			}
		}
	}
	else
	{
		COXCaptionPainter* pPainter=NULL;
		CWnd* pAttachedWnd=(CWnd*)pWnd;
		if(!m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
			return FALSE;
		ASSERT(pPainter!=NULL);
		if(pPainter!=NULL)
			pPainter->Reset();
		if(bRedraw && ::IsWindow(pAttachedWnd->GetSafeHwnd()))
		{
			pAttachedWnd->SendMessage(WM_NCPAINT);
		}
	}

	return TRUE;
}


#ifndef OXCP_NO_SAVESTATE
BOOL COXCaptionPainterOrganizer::LoadState(const CWnd* pWnd, LPCTSTR lpszProfileName, 
										   BOOL bApply/*=TRUE*/)
{
	if(pWnd==NULL)
	{
		POSITION pos=m_arrAttachedWnd.GetStartPosition();
		HWND hAttachedWnd=NULL;
		COXCaptionPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hAttachedWnd,pPainter);
			ASSERT(::IsWindow(hAttachedWnd));
			ASSERT(pPainter!=NULL);
			if(pPainter!=NULL)
				pPainter->LoadState(lpszProfileName,bApply);
		}
	}
	else
	{
		COXCaptionPainter* pPainter=NULL;
		CWnd* pAttachedWnd=(CWnd*)pWnd;
		if(!m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
			return FALSE;
		ASSERT(pPainter!=NULL);
		if(pPainter!=NULL)
			pPainter->LoadState(lpszProfileName,bApply);
	}

	return TRUE;
}
#endif // OXCP_NO_SAVESTATE


COXCaptionPainter* COXCaptionPainterOrganizer::GetPainter(const CWnd* pWnd) const
{
	ASSERT(pWnd!=NULL);
	if(pWnd==NULL)
		return NULL;

	COXCaptionPainter* pPainter=NULL;
	CWnd* pAttachedWnd=(CWnd*)pWnd;
	if(!m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
		pPainter=NULL;

	return pPainter;
}

BOOL COXCaptionPainterOrganizer::IsAttached(const CWnd* pWnd) const
{
	ASSERT(pWnd!=NULL);
	if(pWnd==NULL)
		return FALSE;

	COXCaptionPainter* pPainter=NULL;
	CWnd* pAttachedWnd=(CWnd*)pWnd;
	if(m_arrAttachedWnd.Lookup(pAttachedWnd->GetSafeHwnd(),pPainter))
	{
		ASSERT(pPainter!=NULL);
		if(pPainter!=NULL)
			return TRUE;
	}

	return FALSE;
}


BOOL COXCaptionPainterOrganizer::
AttachAllInThread(DWORD dwThreadID/*=::GetCurrentThreadId()*/)
{
	if(IsAttachedAllInThread())
	{
		TRACE(_T("COXCaptionPainterOrganizer::AttachAllInThread: this object already attached to a thread\n"));
		return FALSE;
	}

	COXCaptionPainterOrganizer* pOrganizer=NULL;
	if(m_arrThreadOrganizers.Lookup(dwThreadID,pOrganizer))
	{
		ASSERT(pOrganizer!=NULL);
		TRACE(_T("COXCaptionPainterOrganizer::AttachAllInThread: specified thread already attached to a COXCaptionPainterOrganizer object\n"));
		return FALSE;
	}
	m_arrThreadOrganizers.SetAt(dwThreadID,this);

	m_dwThreadID=dwThreadID;
	// go through all windows and attach them
	::EnumWindows(&EnumThreadWindows,(LPARAM)this);

	// setup hooks for Computer Based Training
	if(m_pfnOriginalCBTHookProc==NULL)
	{
		m_pfnOriginalCBTHookProc=
			::SetWindowsHookEx(WH_CBT,CaptionPainterCBTHookProc,NULL,dwThreadID);
		m_pfnOldCBTHookProc=m_pfnOriginalCBTHookProc;
	}
	else
	{
		m_pfnOldCBTHookProc=
			::SetWindowsHookEx(WH_CBT,CaptionPainterCBTHookProc,NULL,dwThreadID);
	}

	// setup hooks for GetMessage
	if(m_pfnOriginalGetMessageHookProc==NULL)
	{
		m_pfnOriginalGetMessageHookProc=::SetWindowsHookEx(WH_GETMESSAGE,
			CaptionPainterGetMessageHookProc,NULL,dwThreadID);
		m_pfnOldGetMessageHookProc=m_pfnOriginalGetMessageHookProc;
	}
	else
	{
		m_pfnOldGetMessageHookProc=::SetWindowsHookEx(WH_GETMESSAGE,
			CaptionPainterGetMessageHookProc,NULL,dwThreadID);
	}


	return TRUE;
}


void COXCaptionPainterOrganizer::AttachAllWindows(HWND hWndStartFrom)
{
	ASSERT(hWndStartFrom!=NULL);

	HWND hWnd=hWndStartFrom;
	while(hWnd!=NULL)
	{
		CWnd* pWnd=CWnd::FromHandlePermanent(hWnd);
		if(pWnd!=NULL && !IsAttached(pWnd))
		{
			Attach(pWnd);
		}

		// loop through children
		HWND hWndChild=::GetWindow(hWnd,GW_CHILD);
		if(hWndChild!=NULL)
			AttachAllWindows(hWndChild);

		// loop through windows
		hWnd=::GetWindow(hWnd,GW_HWNDNEXT);
	}
}


LRESULT CALLBACK COXCaptionPainterOrganizer::
CaptionPainterCBTHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	if(nCode>=0 && ::IsWindow((HWND)wParam))
	{
		DWORD dwThreadID=::GetWindowThreadProcessId((HWND)wParam,NULL);
		COXCaptionPainterOrganizer* pOrganizer=NULL;
		if(COXCaptionPainterOrganizer::m_arrThreadOrganizers.
			Lookup(dwThreadID,pOrganizer))
		{
			ASSERT(pOrganizer!=NULL);
			ASSERT(pOrganizer->IsAttachedAllInThread());

			if(nCode==HCBT_DESTROYWND)
			{
				// check if the window that is about to be destroyed
				// had been added to caption organizer list
				CWnd* pWnd=CWnd::FromHandlePermanent((HWND)wParam);
				if(pWnd!=NULL && pOrganizer->IsAttached(pWnd))
					pOrganizer->Detach(pWnd,FALSE);
			}
			else
			{
				// check if new window is created and attach it.
				CWnd* pWnd=CWnd::FromHandlePermanent((HWND)wParam);
				if(pWnd!=NULL && !pOrganizer->IsAttached(pWnd))
				{
					POSITION pos=NULL;
					COXCaptionPainter* pPainter=pOrganizer->GetFirstPainter(pos);
					if(pOrganizer->Attach(pWnd)!=NULL && pPainter!=NULL)
					{
						COXCaptionPainter::SetCaptionPainter(pWnd,pPainter);
					}
				}
			}

			return ::CallNextHookEx(pOrganizer->GetSavedCBTHookProc(),
				nCode,wParam,lParam);
		}
	}
	return ::CallNextHookEx(COXCaptionPainterOrganizer::GetOriginalCBTHookProc(),
		nCode,wParam,lParam);
}


LRESULT CALLBACK COXCaptionPainterOrganizer::
CaptionPainterGetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	if(nCode>=0 && ::IsWindow(((MSG*)lParam)->hwnd))
	{
		DWORD dwThreadID=::GetWindowThreadProcessId(((MSG*)lParam)->hwnd,NULL);
		COXCaptionPainterOrganizer* pOrganizer=NULL;
		if(COXCaptionPainterOrganizer::
			m_arrThreadOrganizers.Lookup(dwThreadID,pOrganizer))
		{
			ASSERT(pOrganizer!=NULL);
			ASSERT(pOrganizer->IsAttachedAllInThread());

			// check if new window is created and attach it.
			CWnd* pWnd=CWnd::FromHandlePermanent(((MSG*)lParam)->hwnd);
			if(pWnd!=NULL && !pOrganizer->IsAttached(pWnd))
			{
				POSITION pos=NULL;
				COXCaptionPainter* pPainter=pOrganizer->GetFirstPainter(pos);
				if(pOrganizer->Attach(pWnd)!=NULL && pPainter!=NULL)
				{
					COXCaptionPainter::SetCaptionPainter(pWnd,pPainter);
				}
			}

			return ::CallNextHookEx(pOrganizer->GetSavedGetMessageHookProc(),
				nCode,wParam,lParam);
		}
	}

	return ::CallNextHookEx(COXCaptionPainterOrganizer::GetOriginalGetMessageHookProc(),
		nCode,wParam,lParam);
}


BOOL CALLBACK COXCaptionPainterOrganizer::EnumThreadWindows(HWND hWnd, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(lParam!=NULL);
	ASSERT(::IsWindow(hWnd));
	COXCaptionPainterOrganizer* pOrganizer=(COXCaptionPainterOrganizer*)lParam;
	ASSERT(pOrganizer->IsAttachedAllInThread());

	DWORD dwThreadID=::GetWindowThreadProcessId(hWnd,NULL);
	if(dwThreadID==pOrganizer->GetAttachedThread())
	{
		pOrganizer->AttachAllWindows(hWnd);
		return FALSE;
	}
	return TRUE;
}


BOOL COXCaptionPainterOrganizer::DetachAllInThread(BOOL bRedraw/*=TRUE*/)
{
	if(!IsAttachedAllInThread())
		return FALSE;

	ASSERT(m_dwThreadID!=NULL);
	ASSERT(m_pfnOldCBTHookProc!=NULL);
	ASSERT(m_pfnOriginalCBTHookProc!=NULL);
	ASSERT(m_pfnOldGetMessageHookProc!=NULL);
	ASSERT(m_pfnOriginalGetMessageHookProc!=NULL);

	// unhook CBT
	if(m_pfnOldCBTHookProc!=NULL)
	{
		VERIFY(::UnhookWindowsHookEx(m_pfnOldCBTHookProc));
		m_pfnOldCBTHookProc=NULL;
		m_pfnOriginalCBTHookProc=NULL;
	}

	// unhook GetMessage
	if(m_pfnOldGetMessageHookProc!=NULL)
	{
		VERIFY(::UnhookWindowsHookEx(m_pfnOldGetMessageHookProc));
		m_pfnOldGetMessageHookProc=NULL;
		m_pfnOriginalGetMessageHookProc=NULL;
	}

	m_arrThreadOrganizers.RemoveKey(m_dwThreadID);

	m_dwThreadID=NULL;

	return Detach(NULL,bRedraw);
}

