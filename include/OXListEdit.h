#if !defined(AFX_OXLISTEDIT_H__28D7469D_27EB_4B0A_A426_31C2A795DCDB__INCLUDED_)
#define AFX_OXLISTEDIT_H__28D7469D_27EB_4B0A_A426_31C2A795DCDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OXListEdit.h : header file
//

#include "OXDllExt.h"
#include "OXDropEdit.h"
#include "OXEdit.h"

#pragma warning(disable : 4706)

// v9.3 - update 05 - shouldn't need to include this for VS2010 - problematic for static MFC linkage (?)
#if _MSC_VER < 1600
#include <multimon.h>
#endif

class OX_CLASS_DECL COXListPopup : public CListBox
{
// Construction
public:
	COXListPopup();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXListPopup)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	int Pick(CRect rect, CRect rectParent);
	virtual ~COXListPopup();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXListPopup)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void AdjustDisplayRectangle(CRect& rect, CRect rectParent);
};

/////////////////////////////////////////////////////////////////////////////
// COXListEdit window

class COXPropertiesWnd;

class OX_CLASS_DECL COXListEdit : public COXDropEdit<COXEdit>
{
	friend class COXListPopup;

// Construction
public:
	COXListEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXListEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetListItems(CStringArray* parItems, COXPropertiesWnd* pPropertiesWnd);
	virtual ~COXListEdit();

	// Generated message map functions
protected:
	virtual BOOL InitializeDropEdit();
	virtual void OnDropButton();
	DECLARE_DYNCREATE(COXListEdit)
	
		//{{AFX_MSG(COXListEdit)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
	COXListPopup m_DropList;
	COXPropertiesWnd* m_pPropertiesWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OXLISTEDIT_H__28D7469D_27EB_4B0A_A426_31C2A795DCDB__INCLUDED_)
