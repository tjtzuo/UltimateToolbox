// ==========================================================================
//							Class Implementation : 
//							COXAdvancedAssertMail
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef __AdvancedAssert_h__
#error In order to use Advanced Asserts "OXAdvancedAssert.h" must be included in stdafx.h
#endif

#include "OXAdvancedAssertMail.h"  // SendMail()

// If this is not a debug or a beta version, then don't use any of this code. 
#if defined(_DEBUG) || defined(BETA)   // entire file

#include <afxres.h>              // AFX_IDP_...
#include <mapi.h>                // MapiMessage
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__ ;
#endif


/////////////////////////////////////////////////////////////////////////////
// COXAdvancedAssertMail

COXAdvancedAssertMail::COXAdvancedAssertMail() :
	m_hInstMail         (NULL),
	m_lhSession         (NULL),
	m_lpfnMAPILogon     (NULL),
	m_lpfnMAPILogoff    (NULL),
	m_lpfnMAPISendMail  (NULL)
	{
	}

COXAdvancedAssertMail::~COXAdvancedAssertMail()
	{
	if ( IsLoaded() )
		{
		FreeMapiLibrary() ;
		}
	}

#ifdef _WIN32
static TCHAR szMapiModule[] = _T("MAPI32.DLL") ;
#else
static TCHAR szMapiModule[] = _T("MAPI.DLL"  ) ;
#endif


BOOL COXAdvancedAssertMail::LoadMapiLibrary()
	{
	if ( !IsLoaded() )
		{
		m_hInstMail = ::LoadLibrary ( szMapiModule ) ;
		(FARPROC&)m_lpfnMAPILogon    = ::GetProcAddress ( m_hInstMail, "MAPILogon"    ) ;
		(FARPROC&)m_lpfnMAPILogoff   = ::GetProcAddress ( m_hInstMail, "MAPILogoff"   ) ;
		(FARPROC&)m_lpfnMAPISendMail = ::GetProcAddress ( m_hInstMail, "MAPISendMail" ) ;
		if ( !IsLoaded() )
			{
			CString csFailedMapiLoad ;
			if ( !csFailedMapiLoad.LoadString(AFX_IDP_FAILED_MAPI_LOAD) )
				csFailedMapiLoad = _T("Unable to load mail system support.") ;
			AfxMessageBox ( csFailedMapiLoad, MB_ICONEXCLAMATION ) ;
			}
		}
	return IsLoaded() ;
	}

void COXAdvancedAssertMail::FreeMapiLibrary()
	{
	if ( IsLoaded() )
		{
		::FreeLibrary ( m_hInstMail ) ;
		m_hInstMail            = NULL ;
		m_lhSession            = NULL ;
		m_lpfnMAPILogon        = NULL ;
		m_lpfnMAPILogoff       = NULL ;
		m_lpfnMAPISendMail     = NULL ;
		}
	}

ULONG COXAdvancedAssertMail::Logon(ULONG ulUIParam, LPSTR lpszProfileName, LPSTR lpszPassword, FLAGS flFlags, ULONG ulReserved)
	{
	if ( !IsLoaded() )
		LoadMapiLibrary() ;
	
	ASSERT( IsLoaded() );  // Careful, we might already be inside an assert. 
	if ( m_lpfnMAPILogon == NULL )
		{
		(FARPROC&)m_lpfnMAPILogon = ::GetProcAddress ( m_hInstMail, "MAPILogon" ) ;
		ASSERT( m_lpfnMAPILogon != NULL );  // Careful, we might already be inside an assert. 
		if ( m_lpfnMAPILogon == NULL )
			{
			CString csInvalidMapiDll ;
			if ( !csInvalidMapiDll.LoadString(AFX_IDP_INVALID_MAPI_DLL) )
				csInvalidMapiDll = _T("Mail system DLL is invalid.") ;
			AfxMessageBox ( csInvalidMapiDll, MB_ICONEXCLAMATION ) ;
			return MAPI_E_NOT_SUPPORTED ;
			}
		}
	return m_lpfnMAPILogon ( ulUIParam, lpszProfileName, lpszPassword, flFlags, ulReserved, &m_lhSession ) ;
	}

ULONG COXAdvancedAssertMail::Logoff(ULONG ulUIParam, FLAGS flFlags, ULONG ulReserved)
	{
	ASSERT( IsLoaded() );  // Careful, we might already be inside an assert. 
	if ( m_lpfnMAPILogoff == NULL )
		{
		(FARPROC&)m_lpfnMAPILogoff = ::GetProcAddress ( m_hInstMail, "MAPILogoff" ) ;
		ASSERT( m_lpfnMAPILogoff != NULL );  // Careful, we might already be inside an assert. 
		if ( m_lpfnMAPILogoff == NULL )
			{
			CString csInvalidMapiDll ;
			if ( !csInvalidMapiDll.LoadString(AFX_IDP_INVALID_MAPI_DLL) )
				csInvalidMapiDll = _T("Mail system DLL is invalid.") ;
			AfxMessageBox ( csInvalidMapiDll, MB_ICONEXCLAMATION ) ;
			return MAPI_E_NOT_SUPPORTED ;
			}
		}
	ULONG ulStatus = m_lpfnMAPILogoff ( m_lhSession, ulUIParam, flFlags, ulReserved ) ;
	m_lhSession = NULL ;
	return ulStatus ;
	}

ULONG COXAdvancedAssertMail::SendMail(ULONG_PTR ulUIParam, lpMapiMessage lpMessage, FLAGS flFlags, ULONG ulReserved)
	{
	if ( !IsLoaded() )
		{
		if ( !LoadMapiLibrary() )
			return MAPI_E_FAILURE ;
		}
	
	ASSERT( IsLoaded() );  // Careful, we might already be inside an assert. 
	if ( m_lpfnMAPISendMail == NULL )
		{
		(FARPROC&)m_lpfnMAPISendMail = ::GetProcAddress ( m_hInstMail, "MAPISendMail" ) ;
		ASSERT( m_lpfnMAPISendMail != NULL );  // Careful, we might already be inside an assert. 
		if ( m_lpfnMAPISendMail == NULL )
			{
			CString csInvalidMapiDll ;
			if ( !csInvalidMapiDll.LoadString(AFX_IDP_INVALID_MAPI_DLL) )
				csInvalidMapiDll = _T("Mail system DLL is invalid.") ;
			AfxMessageBox ( csInvalidMapiDll, MB_ICONEXCLAMATION ) ;
			return MAPI_E_FAILURE ;
			}
		}
	return m_lpfnMAPISendMail ( m_lhSession, PtrToUlong(ulUIParam), lpMessage, flFlags, ulReserved ) ;
	}

BOOL COXAdvancedAssertMail::IsLoaded() const
	{
	return m_hInstMail != NULL ;
	}

BOOL COXAdvancedAssertMail::IsValidSendMail() const
	{
	if ( m_lpfnMAPISendMail == NULL )
		(FARPROC&)m_lpfnMAPISendMail = ::GetProcAddress ( m_hInstMail, "MAPISendMail" ) ;
	return m_lpfnMAPISendMail != NULL ;
	}

/////////////////////////////////////////////////////////////////////////////
// SendMail()

#ifndef _WIN32
class CWaitCursor
	{
	// Construction/Destruction
	public:
		CWaitCursor()  { AfxGetApp()->BeginWaitCursor() ; }
		~CWaitCursor() { AfxGetApp()->EndWaitCursor()   ; }
	
	// Operations
	public:
		void Restore() { AfxGetApp()->RestoreWaitCursor() ; }
	} ;

HWND PASCAL _AfxGetSafeOwner(CWnd* pParent) ;
#endif

int SendMail(LPCSTR pszTo, LPCSTR pszSubject, LPCSTR pszBody, LPCSTR pszAttachment/*=NULL*/)
	{
	CWaitCursor wait ;
	
	MapiMessage   message   ;
	MapiRecipDesc recipFrom ;
	MapiRecipDesc recipTo   ;
	MapiFileDesc  fileDesc  ;
	memset ( &recipFrom, 0, sizeof(recipFrom) ) ;
	memset ( &recipTo,   0, sizeof(recipTo  ) ) ;
	memset ( &fileDesc,  0, sizeof(fileDesc ) ) ;
	memset ( &message,   0, sizeof(message  ) ) ;

	message.lpszSubject  = (char*) pszSubject ;
	message.lpszNoteText = (char*) pszBody    ;
	if ( pszTo && strlen(pszTo) )
		{
		recipTo.ulRecipClass = MAPI_TO     ;
		//recipTo.lpszName     = _T("Support") ;
		//recipTo.lpszAddress  = _T("SMTP:beta@yourcompany.com") ;
		recipTo.lpszName     = (char*) pszTo ;
		
		message.nRecipCount  = 1        ;
		message.lpRecips     = &recipTo ;
		}
	if ( pszAttachment && strlen(pszAttachment) )
		{
		fileDesc.nPosition    = (ULONG) -1            ;
		fileDesc.lpszPathName = (char*) pszAttachment ;
		message.nFileCount = 1         ;
		message.lpFiles    = &fileDesc ;
		}
	
#ifndef _WIN32
	COXAdvancedAssertMail* appMailState = &_appMailState ;
#endif

#ifdef _WIN32
	// prepare for modal dialog box
	AfxGetApp()->EnableModeless ( FALSE ) ;
	HWND hwndTop ;
	CWnd* pwndParent  = CWnd::GetSafeOwner ( NULL, &hwndTop ) ;
	ULONG_PTR ulParentWnd = (ULONG_PTR)pwndParent->GetSafeHwnd() ;
	
	// Parent window will be NULL if assert is fired before 
	// main window is setup. 
	if ( pwndParent )
		{
		// some extra precautions are required to use MAPISendMail as it
		// tends to enable the parent window in between dialogs (after
		// the login dialog, but before the send note dialog).
		pwndParent->SetCapture() ;
		::SetFocus ( NULL ) ;
		pwndParent->m_nFlags |= WF_STAYDISABLED ;
		}
	
	int nError = appMailState->SendMail ( PtrToUlong(ulParentWnd), &message, MAPI_LOGON_UI | MAPI_DIALOG, 0 ) ;
	
	// Parent window will be NULL if assert is fired before 
	// main window is setup. 
	if ( pwndParent )
		{
		// after returning from the MAPISendMail call, the window must
		// be re-enabled and focus returned to the frame to undo the workaround
		// done before the MAPI call.
		::ReleaseCapture() ;
		pwndParent->m_nFlags &= ~WF_STAYDISABLED ;
		
		pwndParent->EnableWindow ( TRUE ) ;
		::SetActiveWindow ( NULL ) ;
		pwndParent->SetActiveWindow() ;
		pwndParent->SetFocus() ;
		if ( hwndTop != NULL )
			::EnableWindow ( hwndTop, TRUE ) ;
		}
	AfxGetApp()->EnableModeless ( TRUE ) ;
#else
	// allow MAPI to send the mail message
	HWND  hwndParent  = _AfxGetSafeOwner(NULL);
	ULONG ulParentWnd = (ULONG)hwndParent ;

	// Parent window will be NULL if assert is fired before 
	// main window is setup. 
	if ( hwndParent != NULL )
		{
		// some extra precautions are required to use MAPISendMail as it
		// tends to enable the parent window in between dialogs (after
		// the login dialog, but before the send note dialog).
		::SetCapture(hwndParent);
		::SetFocus(NULL);
		::SetProp(hwndParent, _T("StayDisabled"), (HANDLE)1);
		}

	FLAGS flFlags = MAPI_LOGON_UI | MAPI_DIALOG ;
#ifdef _UNICODE
	flFlags |= ;
#endif
	int nError = (int) appMailState->SendMail ( ulParentWnd, &message, flFlags, 0 ) ;

	// Parent window will be NULL if assert is fired before 
	// main window is setup. 
	if ( hwndParent != NULL )
		{
		// after returning from the MAPISendMail call, the window must
		// be re-enabled and focus returned to the frame to undo the workaround
		// done before the MAPI call.
		::ReleaseCapture();
		::RemoveProp   ( hwndParent, _T("StayDisabled") ) ;
		::EnableWindow ( hwndParent, TRUE               ) ;
		::SetFocus ( NULL       ) ;
		::SetFocus ( hwndParent ) ;
		}

#endif
	if ( (nError != SUCCESS_SUCCESS) && (nError != MAPI_USER_ABORT) && (nError != MAPI_E_LOGIN_FAILURE) )
		{
		CString csFailedMapiSend ;
		if ( !csFailedMapiSend.LoadString(AFX_IDP_FAILED_MAPI_SEND) )  // Don't require a resource entry. 
			csFailedMapiSend = _T("Send Mail failed to send message.") ;
		AfxMessageBox ( csFailedMapiSend, MB_ICONEXCLAMATION ) ;
		}
	return nError ;
	}

/////////////////////////////////////////////////////////////////////////////
// SendMailToBeta()

int SendMailToBeta(LPCSTR pszSubject, LPCSTR pszBody, LPCSTR pszAttachment/*=NULL*/)
	{
	return SendMail ( AppGetAssertEmailAddress(), pszSubject, pszBody, pszAttachment ) ;
	}


#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

#ifdef _WIN32
PROCESS_LOCAL(COXAdvancedAssertMail, appMailState)
#else
COXAdvancedAssertMail _appMailState ;
#endif

#endif // _DEBUG || BETA
/////////////////////////////////////////////////////////////////////////////
