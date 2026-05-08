// ==========================================================================
// 							Class Specification : COXJPEGException
// ==========================================================================

// Header file : OXJPGExp.h

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
//	YES	Derived from CException

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is the class that encapsulates the exception handling fro all
//  our JPEG classes.  Adds an error string to base CException class
//  method

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __JPEGEXCEPT_H__
#define __JPEGEXCEPT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXJPEGException : public CException
{
DECLARE_DYNAMIC( COXJPEGException )

// Data Members
public:
	DWORD m_dwErrorCode;	

protected:
	CString m_sErrMsg;

private:

// Member Functions
public:
	COXJPEGException(DWORD dwErrorCode, LPCTSTR pszErrMsg);  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object

	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL );
	// --- In  : see CException::GetErrorMessage()
	// --- Out : 
	// --- Returns : TRUE if string available, otherwise FALSE
	// --- Effect : retrieves the error string if available

	virtual ~COXJPEGException();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:

private:

};

#endif //__JPEGEXCEPT_H__
