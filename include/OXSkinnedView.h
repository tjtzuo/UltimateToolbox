
// Version: 9.3

#if !defined(AFX_OXSKINNEDVIEW_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_)
#define AFX_OXSKINNEDVIEW_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// v9.3 - update 05 - removed
//#include "StdAfx.h"

// OXSkinnedView.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// COXSkinnedView window

template<class BASE_CLASS>
class COXSkinnedView : public BASE_CLASS
{

// Construction
public:
	COXSkinnedView()
	{
		m_pFrameSkin = NULL;
	};

	COXSkinnedView(UINT nIDTemplate) : BASE_CLASS(nIDTemplate)
	{
		m_pFrameSkin = NULL;
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
	virtual ~COXSkinnedView() { };

private:
	COXFrameSkin* m_pFrameSkin;
	COXFrameSkin* GetFrameSkin();
};

/////////////////////////////////////////////////////////////////////////////
// COXSkinnedView

template<class BASE_CLASS>
COXFrameSkin* COXSkinnedView<BASE_CLASS>::GetFrameSkin()
{
	// Check if the app is derived from COXSkinnedApp
	COXSkinnedApp* pSkinnedApp = DYNAMIC_DOWNCAST(COXSkinnedApp, AfxGetApp());
	if (pSkinnedApp != NULL && pSkinnedApp->GetCurrentSkin() != NULL)
		return pSkinnedApp->GetCurrentSkin()->GetFrameSkin();
	else
	{
		// Create a classic skin for this class if not created already
		if (m_pFrameSkin == NULL)
			m_pFrameSkin = new COXFrameSkinClassic();

		return m_pFrameSkin;
	}
};

template<class BASE_CLASS>
LRESULT COXSkinnedView<BASE_CLASS>::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_NCPAINT:
		{
			LRESULT lResult = BASE_CLASS::WindowProc(message, wParam, lParam);
			GetFrameSkin()->DrawViewBorder(this);
			
			return lResult;
		}

	case WM_NCCALCSIZE:
		{
			LRESULT lResult = BASE_CLASS::WindowProc(message, wParam, lParam);
			
			if (wParam == 0)
				return FALSE;

			GetFrameSkin()->OnNcCalcSizeView(lParam, this);

			return lResult;
		}


	default:
		{
			break;
		}
	}

	return BASE_CLASS::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OXSKINNEDVIEW_H__6BAD3A54_4046_453C_A79B_4A9BB99FF063__INCLUDED_)
