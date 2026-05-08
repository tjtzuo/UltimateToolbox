// ==========================================================================
// 							Class Specification : COXDialogBar
// ==========================================================================

// Header file : OXDlgBar.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CControlBar

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class encapsulates a controlbar with a dialog created from a memory template
//	on top of it.

/////////////////////////////////////////////////////////////////////////////
#ifndef __DIALOGBAR_H__
#define __DIALOGBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXDialogBar : public CControlBar
{
// Data Members

public:
	//{{AFX_DATA(COXDockPropertySheet)
	//}}AFX_DATA

protected:
	CSize m_sizeDefault;

private:

// Member Functions
public:
	COXDialogBar();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object

	BOOL Create(CWnd* pParentWnd, UINT nStyle, UINT nID);
	// --- In  : pParentWnd : parent window
	//			 nDlgStyle : styles for the 'memory' dialog
	//			 nID : ID of the dialog
	// --- Out : 
	// --- Returns : successful or not
	// --- Effect : Creates Control bar with a dialog from a mem template on top of it

	virtual ~COXDialogBar();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
	virtual BOOL CreateMemDialog(CWnd* pParentWnd);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXDialogBar)
	public:
	protected:
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(COXDialogBar)
	//}}AFX_MSG
protected:
#ifndef _AFX_NO_OCC_SUPPORT
	// data and functions necessary for OLE control containment
	_AFX_OCC_DIALOG_INFO* m_pOccDialogInfo;
	LPCTSTR m_lpszTemplateName;
	virtual BOOL SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
#endif

private:

};

#endif //__DIALOGBAR_H__
