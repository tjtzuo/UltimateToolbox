// ==========================================================================
// 					Class Implementation : COXJPEGException
// ==========================================================================

// Source file : OXJPGExp.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXJPGExp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( COXJPEGException, CException );

#define new DEBUG_NEW


// Data members -------------------------------------------------------------
// protected:


//	CString		m_sErrMsg;
//	---			A string associated with the error code (if available)

// private:

// Member functions ---------------------------------------------------------
// public:

COXJPEGException::COXJPEGException(DWORD dwErrorCode, LPCTSTR pszErrMsg)
	: m_dwErrorCode(dwErrorCode),
	m_sErrMsg(pszErrMsg)
	{
	}

COXJPEGException::~COXJPEGException()
	{
	}

BOOL COXJPEGException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
									   PUINT pnHelpContext /* = NULL */)
	{
	UNREFERENCED_PARAMETER(pnHelpContext);
	ASSERT(lpszError != NULL && AfxIsValidString(lpszError, nMaxError));

	lstrcpyn(lpszError, m_sErrMsg, nMaxError);

	return TRUE;
	}


///////////////////////////////////////////////////////////////////////////
