// ==========================================================================
// 							Class Specification : CMFCToolBar
// ==========================================================================

// Header file : MFCToolBar.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#pragma once

#include "OXDllExt.h"

#define TOOLBAR_BASE_CLASS CToolBar

// CMFCToolBar

// v9.3 - update 05 - changed to COXMFCToolBar to avoid conflict (VS2010)

class OX_CLASS_DECL COXMFCToolBar : public TOOLBAR_BASE_CLASS
{
	DECLARE_DYNAMIC(COXMFCToolBar)

public:
	void SetBarStyle(DWORD dwStyle);
	COXMFCToolBar();
	virtual ~COXMFCToolBar();
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
		CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR);

protected:
	DECLARE_MESSAGE_MAP()
};


