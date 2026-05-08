// ==========================================================================
// 					Class Specification : 
//						COXTreeEdit
// ==========================================================================

// Header file : COXTreeEdit.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
#ifndef _OXTREE_EDIT_H
#define _OXTREE_EDIT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CEdit

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

//Description:
// This is a small helper class designed for
//in-place label edit in COXTreeCtrl


class OX_CLASS_DECL COXTreeEdit : public CEdit
{
	friend class COXPropertiesWnd;
// Construction
public:
	void KeepBounds(CSize sz);
	void KeepColumnSize(CSize sz);
	//In: sz: specified bounded edit control

	void Init();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : performs initialization
	void KeepPos(CPoint pt, DWORD dwAlign);
	//In: pt: point to be aligned
	//		dwAlign: one of following ES_LEFT,ES_RIGHT,ES_CENTER
	//		specified edit control alignment

	COXTreeEdit();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : constructor

	void SetBorder(BOOL bHasBorder);
	// --- In  :	bHasBorder - pass TRUE if you want a border, FALSE otherwise
	// --- Out : 
	// --- Returns:
	// --- Effect : Call this function to set or clear the border of the edit box

// Attributes
public:
	int m_nDeltaY;
	int m_nDeltaX;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXTreeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COXTreeEdit();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : destructor

	// Generated message map functions
protected:
	BOOL m_bHasBorder;
	CSize m_szBounds;
	BOOL m_bKeepBounds;
	BOOL m_bKeepColumnSize;
	DWORD m_dwAlign;
	CPoint m_pos;
	BOOL m_bKeepPos;
	//{{AFX_MSG(COXTreeEdit)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
