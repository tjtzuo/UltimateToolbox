// ==========================================================================
//						   Class Specification : 
//					COXCoolComboEdit & COXCoolComboBox
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

/*

  DESCRIPTION

COXCoolComboBox class implements standard combo box as cool control. Refer
to description of COXCoolCtrl class for details on cool controls. Briefly,
we call any control (generally speaking, any window) as cool one if it 
supports normal or hot state. By definition, window is in hot state when mouse 
cursor is over it or it has focus. Otherwise window is in normal state.

COXCoolComboBox is derived from COXCoolCtrl. COXCoolComboBox doesn't have 
it's own public functions and it relies heavily on COXCoolCtrl class. 
Refer to COXCoolCtrl function reference for details.


Basically, COXCoolComboBox provides drawing routines for normal and hot state.
Use COXCoolComboBox class the same way you would use standard CComboBox.

Note that COXCoolCtrl automatically support tooltip for control. You can 
set/retrieve tooltip text using the following functions:

	BOOL SetTooltipText(const CString& sTooltipText);
	BOOL GetTooltipText(CString& sTooltipText) const;



Refer to CoolCombo sample that can be found in .\Samples\gui\CoolCombo subdirectory 
of your Ultimate Toolbox directory for example that shows how to use cool combobox 
in dialog and toolbar.

Dependencies:

	#include "OXCoolComboBox.h"


Source code files:

	"OXCoolComboBox.cpp"


*/




#ifndef _OXCOOLCOMBOBOX_H__
#define _OXCOOLCOMBOBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#include "OXCoolCtrl.h"

// since combos generally tend to bo too large left to their own devices
#define EXTRA_BOTTOM_MARGIN		0   
#define HOT_MARGIN_SIZE			2

#define IDW_OXCOMBOLISTBOX		1000
#define IDW_OXCOMBOEDIT			1001

class COXCoolComboBox;

/////////////////////////////////////////////////////////////////////////////
// COXCoolComboEdit window

class OX_CLASS_DECL COXCoolComboEdit : public COXCoolCtrl<CEdit>
{
// Attributes
public:

protected:
	COXCoolComboBox* m_pCombo;

// Operations
public:

	// --- In  :	pCombo	-	pointer to COXCoolComboBox object to be 
	//							associated with this edit control 
	// --- Out : 
	// --- Returns:
	// --- Effect : Assigns parent combo control 
	void SetCombo(COXCoolComboBox* pCombo);

	// --- In  :
	// --- Out : 
	// --- Returns: pointer to parent combo control
	// --- Effect : Retrieves the pointer to parent combo control
	COXCoolComboBox* GetCombo();

protected:
	virtual void OnFocusChange(CWnd* pWndGainFocus, CWnd* pWndLooseFocus);

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(COXCoolComboEdit)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COXCoolComboBox window

class OX_CLASS_DECL COXCoolComboBox : public COXCoolCtrl<CComboBox>
{
// Construction
public:

	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXCoolComboBox();

// Attributes
public:

protected:
	COXCoolComboEdit m_edit;
	COXCoolCtrl<CListBox> m_listBox;

	int m_nDefaultFontHeight;
	int m_nDefaultFontHeightSansLeading;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXCoolComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

protected:
	// overriden virtual functions
	//
	// called when hot state is about to be changed
	virtual void OnChangeHotState(BOOL bInHotState);
	// called when new tooltip text is about to be set
	virtual BOOL OnChangeTooltipText(const CString& sTooltipText);

	// initialize mouse timer and tooltip control
	virtual BOOL InitializeCoolCtrl();

	// hooks child edit control
	BOOL SubclassEditCtrl();
	// hooks child list box control
	BOOL SubclassListBoxCtrl();

	// retrieves the rectangle taken by dropdown button
	CRect GetButtonRect() const;
	// retrieves the width of dropdown button
	int GetButtonWidth() const;

	// reteieve control margins that are used to create cool effect
	inline int GetLeftMargin() const { return HOT_MARGIN_SIZE; }
	inline int GetTopMargin() const { return HOT_MARGIN_SIZE; }
	inline int GetRightMargin() const { return HOT_MARGIN_SIZE; }
	inline int GetBtmMargin() const { return HOT_MARGIN_SIZE+EXTRA_BOTTOM_MARGIN; }
	/////////////////////////////////////////

	// helper function, defines if control is in dropdown state
	inline BOOL IsDroppedDown() { 
		return (SendMessage(CB_GETDROPPEDSTATE) ? TRUE : FALSE);
	}


// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXCoolComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXCoolComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	afx_msg BOOL OnUpdateCombo();
	afx_msg LRESULT OnSetFont(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	friend COXCoolComboEdit;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _OXCOOLCOMBOBOX_H__
