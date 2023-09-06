
// Version: 9.3

#if !defined(AFX_OXSHADOWEDWND_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_)
#define AFX_OXSHADOWEDWND_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// v9.3 - update 05 - removed
//#include "StdAfx.h"
#include "OXShdWnd.h"

// OXSkinnedWnd.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// COXShadowedWnd window

/*
Description:

COXSkinnedWnd is a template class that implements a window shadow.

Usage:

If you want a window to have a shadow just derive it from COXSkinnedWnd.

Example: Creating a dialog with a shadow:

class CMyDialog : public COXShadowedWnd<CDialog>
*/


template<class BASE_CLASS>
class COXShadowedWnd : public BASE_CLASS
{

// Construction
public:
	COXShadowedWnd()
	{
	};

	COXShadowedWnd(UINT nIDTemplate) : BASE_CLASS(nIDTemplate)
	{
	};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~COXShadowedWnd() { };

private:
	COXShdWnd m_wndShadow;
};

/////////////////////////////////////////////////////////////////////////////
// COXShadowedWnd

template<class BASE_CLASS>
LRESULT COXShadowedWnd<BASE_CLASS>::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	// TODO: Add your specialized code here and/or call the base class
	ASSERT_VALID(this);	
	
	switch(message) 
	{
		case WM_CREATE:
		{
			BOOL bRes = PtrToInt(BASE_CLASS::WindowProc(message, wParam, lParam));

			CRect rect;
			GetWindowRect(rect);
			rect.OffsetRect(4, 4);

			// Create the shadow window
			m_wndShadow.CreateEx(/*WS_EX_TRANSPARENT*/0, AfxRegisterWndClass(CS_DBLCLKS), _T(""),
				WS_POPUP | WS_VISIBLE | WS_DISABLED, rect, AfxGetMainWnd(), NULL);

			m_wndShadow.SetCastingWindow(this);
			//ModifyStyle(DS_3DLOOK, 0); //Commented out by Nish : Jun/06/05
			ModifyStyleEx(WS_EX_WINDOWEDGE, 0);

			return bRes;
		}
		
		case WM_DESTROY:
		{
			m_wndShadow.DestroyWindow();
			break;
		}

		case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* pWP = (WINDOWPOS*) lParam;

			if (::IsWindow(m_wndShadow.m_hWnd))
			{
				m_wndShadow.SetWindowPos(this, pWP->x + 4, pWP->y + 4, pWP->cx, pWP->cy,
					/*SWP_NOZORDER |*/ SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
			}

			break;
		}

		case WM_NCPAINT:
		{
			LRESULT lRes = BASE_CLASS::WindowProc(message, wParam, lParam);

			CWindowDC dc(this);
			CRect rectWindow;
			GetWindowRect(rectWindow);
			rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);

			dc.DrawEdge(rectWindow, BDR_SUNKENOUTER, BF_FLAT | BF_RECT);
			rectWindow.DeflateRect(1, 1, 1, 1);
			dc.DrawEdge(rectWindow, BDR_RAISEDINNER, BF_FLAT | BF_RECT);

			return lRes;
		}

		case WM_SHOWWINDOW:
		{
			if (wParam)
				m_wndShadow.ShowWindow(SW_SHOW);
			else
				m_wndShadow.ShowWindow(SW_HIDE);

			break;
		}

		case WM_WINDOWPOSCHANGING:
		{
			if (::GetKeyState( VK_LBUTTON ) < 0)
			{
				m_wndShadow.ModifyStyleEx(0, WS_EX_TRANSPARENT);
			}
			break;
		}

		case WM_NCMOUSEMOVE:
		{
			// Remove the transparent style
			if (::GetKeyState( VK_LBUTTON ) >= 0)
				m_wndShadow.ModifyStyleEx(WS_EX_TRANSPARENT, 0);

			break;
		}

	}

	return BASE_CLASS::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OXSHADOWEDWND_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_)
