// ==========================================================================
// 							Class Specification : COXDockPropertyPage
// ==========================================================================

// Header file : OXDckPPg.h

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
//	YES	Derived from CPropertyPage

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is used by the COXDockPropertySheet class to be the base class
//  of sheet's pages.

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __DPROPPG_H__
#define __DPROPPG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class COXDockPropertySheet;

class OX_CLASS_DECL COXDockPropertyPage : public CPropertyPage
{
DECLARE_DYNAMIC(COXDockPropertyPage)

// Data Members
friend COXDockPropertySheet;

public:

protected:
	UINT		m_nID;
	CString		m_sCaption;
	BOOL		m_bDynamic;

private:

// Member Functions
public:
	COXDockPropertyPage(LPCTSTR pszCaption = NULL, BOOL bDynamic = FALSE);  
	// --- In  : pszCaption : title of the page
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXDockPropertyPage(UINT nIDCaption, BOOL bDynamic = FALSE);  
	// --- In  : nIDCaption : title-ID of the property page
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	BOOL IsDynamicPage()
		{ return m_bDynamic;}
	// --- In  : 
	// --- Out : 
	// --- Returns : whether the page was dynamically created or not
	// --- Effect : used to determine whether the sheet should destroy the page object or not

	virtual BOOL OnSetActive();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : see CPropertyPage::OnSetActive()

	virtual BOOL OnKillActive();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : see CPropertyPage::OnKillActive()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

private:


};

#endif //__DPROPPG_H__
