// ==========================================================================
// 							Class Implementation : COXDialogBar
// ==========================================================================

// Source file : OXDlgBar.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oxdlgbar.h"


#ifndef __OX_OCCIMPL_H__
#if _MFC_VER < 0x0700
#include <..\src\occimpl.h>
#else
#include <afxocc.h>
#endif

#define __OX_OCCIMPL_H__
#endif __OX_OCCIMPL_H__


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

//	CSize 		m_sizeDefault;
//	---			The size of the control bar

// private:

// Member functions ---------------------------------------------------------
// public:

COXDialogBar::COXDialogBar()
{
#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = NULL;
	m_pOccDialogInfo = NULL;
#endif
}

COXDialogBar::~COXDialogBar()
{
	DestroyWindow();    // avoid PostNcDestroy problems
}

BOOL COXDialogBar::Create(CWnd* pParentWnd, UINT nStyle, UINT nID)
{
	ASSERT(pParentWnd != NULL);

	// allow chance to modify styles
	m_dwStyle = nStyle;
	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(cs));
#ifdef _UNICODE
	cs.lpszClass = _T("uAfxControlBar40");
#else
	cs.lpszClass = _T("AfxControlBar40");
#endif
	// must have WS_CHILD style
	cs.style = (DWORD)nStyle | WS_CHILD;
	cs.hMenu = (HMENU)(UINT_PTR)nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.hwndParent = pParentWnd->GetSafeHwnd();
	if (!PreCreateWindow(cs))
		return FALSE;

	if (!CreateMemDialog(pParentWnd))
		return FALSE;

	// dialog template MUST specify that the dialog
	//  is an invisible child window
	SetDlgCtrlID(nID);
	CRect rect;
	GetWindowRect(&rect);
	m_sizeDefault = rect.Size();    // set fixed size

	// force WS_CLIPSIBLINGS
	ModifyStyle(0, WS_CLIPSIBLINGS);

	return TRUE;
}

BOOL COXDialogBar::CreateMemDialog(CWnd* pParentWnd)
{
	// create a modeless dialog out of an empty memory dialog template
	// load resource
	DWORD lStyle;
	WORD *p, *lpDialogTemplate;

	// allocate some memory to play with 
	lpDialogTemplate = p = (PWORD) LocalAlloc (LPTR, 100);

	// start to fill in the dlgtemplate information.  addressing by WORDs
	lStyle = WS_CHILD | WS_VISIBLE;

	*p++ = LOWORD (lStyle);
	*p++ = HIWORD (lStyle);
	*p++ = 0;			// LOWORD (lExtendedStyle)
	*p++ = 0;			// HIWORD (lExtendedStyle)
	*p++ = 0;			// NumberOfItems
	*p++ = 0;			// x
	*p++ = 0;			// y
	*p++ = 0;			// cx
	*p++ = 0;   		// cy
	*p++ = 0;			// Menu
	*p++ = 0;			// Class

	ASSERT(lpDialogTemplate != NULL);

#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = (LPCTSTR)lpDialogTemplate;
#endif

	// create a modeless dialog
	BOOL bSuccess = CreateDlgIndirect((LPDLGTEMPLATE)lpDialogTemplate, pParentWnd, NULL);

#ifndef _AFX_NO_OCC_SUPPORT
	m_lpszTemplateName = NULL;
#endif

	LocalFree (LocalHandle ((void NEAR *)lpDialogTemplate));

	return bSuccess;
}

CSize COXDialogBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if (bStretch) // if not docked stretch to fit
		return CSize(bHorz ? 32767 : m_sizeDefault.cx,
		bHorz ? m_sizeDefault.cy : 32767);
	else
		return m_sizeDefault;
}



void COXDialogBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

#ifndef _AFX_NO_OCC_SUPPORT

BEGIN_MESSAGE_MAP(COXDialogBar, CControlBar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
END_MESSAGE_MAP()

LRESULT COXDialogBar::HandleInitDialog(WPARAM, LPARAM)
{
	Default();  // allow default to initialize first (common dialogs/etc)

	// create OLE controls
	COccManager* pOccManager = afxOccManager;
	if ((pOccManager != NULL) && (m_pOccDialogInfo != NULL))
	{
		ASSERT(m_lpszTemplateName != NULL);
		if (!pOccManager->CreateDlgControls(this, m_lpszTemplateName, m_pOccDialogInfo))
		{
			TRACE0("Warning: CreateDlgControls failed during dialog bar init.\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL COXDialogBar::SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo)
{
	m_pOccDialogInfo = pOccDialogInfo;
	return TRUE;
}

#endif //!_AFX_NO_OCC_SUPPORT

///////////////////////////////////////////////////////////////////////////
