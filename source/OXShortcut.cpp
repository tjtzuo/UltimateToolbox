// =============================================================================
// 							Class Implementation : COXShortcut
// =============================================================================
//
// Source file : 		OXShortcut.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXShortcut.h"
#include "OXMainRes.h"

#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (_MFC_VER < 0x0420)
#ifndef T2COLE 
// MFC Version < 4.2 AND T2COLE is not defined, this is probably due to
// a wrong order of file inclusion
#error Make sure <afxdisp.h> is included before <afxpriv.h> (see stdafx.h)
#endif // T2COLE 
#endif // _MFC_VER

#ifdef _DEBUG
#define TRACE_ERROR_IF_ANY(TXT) TraceFailure(TXT)
#else
#define TRACE_ERROR_IF_ANY(TXT) 
#endif // _DEBUG

IMPLEMENT_DYNCREATE(COXShortcut, CObject)

// Data members -------------------------------------------------------------
// protected:
//	CString m_sCurFileName;
//		The currently opening shortcut file.
//
//	IShellLink*	m_psl; 
//		The IShellLink interface pointer of the current shortcut file.
//
//	IPersistFile* m_ppf;
//		The IPersistFile interface pointer of the IShellLink.
//
//	HRESULT	m_hres;
//		The HRESULT from the last COM operation.
//
//	BOOL m_bThrowException;
//		indicates whether to throw a COleException while failure occurs
//
// Member functions ---------------------------------------------------------

COXShortcut::COXShortcut()
	{
	m_psl = NULL;
	m_ppf = NULL;
	m_hres = NOERROR;
	m_bThrowException = FALSE;

	ASSERT_VALID(this);
	}

COXShortcut::~COXShortcut()
	{
	ASSERT_VALID(this);
	if (m_psl != NULL || m_ppf != NULL)
		{
		TRACE0("COXShortcut::~COXShortcut(): no Close() was called after last Open().\n");
		Close(FALSE);
		}
	}

#ifdef _DEBUG
void COXShortcut::AssertValid() const
	{
	// OLE must have been initialized for this thread (AfxOleInit)
#if (0x0420 <= _MFC_VER)
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	ASSERT((pState != NULL) && (pState->m_bNeedTerm));
#endif // _MFC_VER

	if ((m_psl == NULL) && (m_ppf == NULL))
		// ... Not yet associated : valid
		return;

	// Check minimum pointer length
	ASSERT_POINTER(m_psl, IUnknown);
	ASSERT_POINTER(m_ppf, IUnknown);

	// Check interface type
	LPUNKNOWN pObject;
	if (SUCCEEDED(m_psl->QueryInterface(IID_IShellLink, (void**)&pObject)))
		pObject->Release();
	else
		// ... m_psl is of incorrect type
		ASSERT(FALSE);

	if (SUCCEEDED(m_ppf->QueryInterface(IID_IPersistFile, (void**)&pObject)))
		pObject->Release();
	else
		// ... m_ppf is of incorrect type
		ASSERT(FALSE);
	}
#endif

BOOL COXShortcut::Open(LPCTSTR pszPathLink, LPCTSTR pszPathObj /* = NULL */,
					   BOOL bThrowException /* = FALSE */)
	{
	ASSERT_VALID(this);
	ASSERT(pszPathLink);

	USES_CONVERSION;

	if (m_psl != NULL || m_ppf != NULL)
		{
		TRACE0("COXShortcut::Open(): no Close() was called after last Open().\n");
		Close(FALSE);
		}
	
	// ... turn off exception throwing
	m_bThrowException = FALSE;

	// Add default extender if necessary
	CString sPathLink = pszPathLink;
	if (!sPathLink.IsEmpty())
		{
		// Check whether the file name (part after last backslash) has en extender
		// If not, add ".lnk"
		int nBackslashPos = sPathLink.ReverseFind(_T('\\'));
		int nDotPos = sPathLink.ReverseFind(_T('.'));
		if (nDotPos <= nBackslashPos)
			sPathLink += _T(".lnk");
		}

    m_hres = CoCreateInstance(CLSID_ShellLink, NULL,
        CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&m_psl);
    if (m_hres == NOERROR)
		{ 
        m_hres = m_psl->QueryInterface(IID_IPersistFile, (LPVOID*)&m_ppf); 
        if (m_hres == NOERROR)
			{
			if (pszPathObj != NULL)
				{
				// ... SetPath() is absolutely neccessary for not crashing Explorer
				if (SetPath(pszPathObj))
					m_hres = m_ppf->Save(T2COLE((LPCTSTR)sPathLink), TRUE);
				}
			else
				m_hres = m_ppf->Load(T2COLE((LPCTSTR)sPathLink), STGM_READ);

			if (m_hres == NOERROR)
				{
				m_sCurFileName = sPathLink;
				ASSERT(!m_sCurFileName.IsEmpty());
				m_bThrowException = bThrowException;
				return TRUE;
				}
			}
		else
			{
			// ... Clean up shell link
			if (m_psl != NULL) 
				m_psl->Release();
			m_psl = NULL;
			}
		}

	// ... if anything goes wrong, return to initial state
	ASSERT(m_sCurFileName.IsEmpty());
	if (m_ppf != NULL)
		{
		m_ppf->Release();
		m_ppf = NULL;
		}
	if (m_psl != NULL)
		{
		m_psl->Release();
		m_psl = NULL;
		}

	// ... throw an exception even when m_hres == S_FALSE
	TRACE_ERROR_IF_ANY(_T("COXShortcut::Open"));
	if (bThrowException)
		AfxThrowOleException(SUCCEEDED(m_hres) ? E_FAIL : m_hres);

	ASSERT_VALID(this);
    return FALSE;
	}

void COXShortcut::Close(BOOL bSave /* = TRUE */)
	{
	ASSERT_VALID(this);

	if (m_ppf != NULL && bSave) Save();

#ifdef _DEBUG
	if (m_ppf != NULL && IsDirty())
		TRACE0("COXShortcut::Close(): closed a dirty shortcut file without saving.\n");
#endif

	m_sCurFileName.Empty();
	if (m_ppf != NULL)
		{
		m_ppf->Release();
		m_ppf = NULL;
		}
	if (m_psl != NULL)
		{
		m_psl->Release();
		m_psl = NULL;
		}

	ASSERT_VALID(this);
	}

BOOL COXShortcut::Save(LPCTSTR pszFileName, BOOL fRemember)
	{
	ASSERT(m_ppf);
	ASSERT_VALID(this);

	if (Validate(m_ppf))
		{
		USES_CONVERSION;

		// Add default extender if necessary
		CString sFileName = pszFileName;
		if (!sFileName.IsEmpty())
			{
			// Check whether the file name (part after last backslash) has en extender
			// If not, add ".lnk"
			int nBackslashPos = sFileName.ReverseFind(_T('\\'));
			int nDotPos = sFileName.ReverseFind(_T('.'));
			if (nDotPos <= nBackslashPos)
				sFileName += _T(".lnk");

			//changed 11/17/99
			m_hres = m_ppf->Save(T2COLE((LPCTSTR)sFileName), fRemember);
			}
		else
			m_hres = m_ppf->Save(NULL, fRemember);

		if (m_hres == NOERROR && !sFileName.IsEmpty() && fRemember)
			m_sCurFileName = sFileName;
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::Save"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::IsDirty()
	{
	ASSERT(m_ppf);
	ASSERT_VALID(this);

	if (Validate(m_ppf))
		{
		m_hres = m_ppf->IsDirty();
		}

#ifdef _DEBUG
	// in this case, S_FALSE is an absolutely normal HRESULT (meaning not dirty)
	// therefore, we can neglect it in the trace message
	if (!SUCCEEDED(m_hres))
		TRACE_ERROR_IF_ANY(_T("COXShortcut::IsDirty"));
#endif

	ThrowExceptionIfNeccessary();

	// ... we treat any error as dirty
	return !(m_hres == S_FALSE);
	}

BOOL COXShortcut::GetArguments(CString& rString)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetArguments(rString.GetBuffer(_MAX_PATH), _MAX_PATH);
		rString.ReleaseBuffer();
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetArguments"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetDescription(CString& rString)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetDescription(rString.GetBuffer(_MAX_FNAME), _MAX_FNAME);
		rString.ReleaseBuffer();
		}
	
	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetDescription"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetHotkey(WORD& wHotKey)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetHotkey(&wHotKey);
		}
	
	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetHotkey"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetIconPath(CString& rString)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		int iIcon;
		m_hres = m_psl->GetIconLocation(rString.GetBuffer(_MAX_PATH), _MAX_PATH, &iIcon);
		rString.ReleaseBuffer();
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetIconPath"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetIconIndex(int& iIcon)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		CString sIconPath;
		m_hres = m_psl->GetIconLocation(sIconPath.GetBuffer(_MAX_PATH), _MAX_PATH, &iIcon);
		sIconPath.ReleaseBuffer();
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetIconIndex"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetIDList(LPITEMIDLIST& ridl)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetIDList(&ridl);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetIDList"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetPath(CString& rString, BOOL bUNC)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		WIN32_FIND_DATA wfd;
		m_hres = m_psl->GetPath(rString.GetBuffer(_MAX_PATH), _MAX_PATH, &wfd, 
								bUNC ? SLGP_UNCPRIORITY : SLGP_SHORTPATH);
		rString.ReleaseBuffer();
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetPath"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetShowCmd(int& iShowCmd)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetShowCmd(&iShowCmd);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetShowCmd"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::GetWorkingDirectory(CString& rString)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->GetWorkingDirectory(rString.GetBuffer(_MAX_PATH), _MAX_PATH);
		rString.ReleaseBuffer();
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::GetWorkingDirectory"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::Resolve(CWnd* pWnd, DWORD fFlags)
	{
	ASSERT(m_psl);
	ASSERT(pWnd == NULL || ::IsWindow(pWnd->m_hWnd));
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->Resolve(pWnd ? pWnd->m_hWnd : NULL, fFlags);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::Resolve"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetArguments(LPCTSTR pszArguments)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetArguments(pszArguments);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetArguments"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetDescription(LPCTSTR pszDescription)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetDescription(pszDescription);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetDescription"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetHotkey(WORD nHotkey)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetHotkey(nHotkey);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetHotkey"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetIconPath(LPCTSTR pszPath)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		int iIcon;
		if (!GetIconIndex(iIcon)) return FALSE;
		m_hres = m_psl->SetIconLocation(pszPath, iIcon);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetIconPath"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetIconIndex(int nIconIndex)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		CString sIconPath;
		if (!GetIconPath(sIconPath)) return FALSE;
		m_hres = m_psl->SetIconLocation(sIconPath, nIconIndex);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetIconIndex"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetIDList(LPCITEMIDLIST pidl)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetIDList(pidl);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetIDList"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetPath(LPCTSTR pszPath)
	{
	ASSERT(m_psl);
	ASSERT(pszPath);
	ASSERT_VALID(this);

	// ... setting a NULL path is very dangerous (it crashes Explorer), therefore
	// we disallow it
	if (Validate(m_psl) && Validate((void *)pszPath))
		{
		m_hres = m_psl->SetPath(pszPath);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetPath"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetRelativePath(LPCTSTR pszRelPath)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetRelativePath(pszRelPath, 0);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetRelativePath"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetShowCmd(int nShowCmd)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetShowCmd(nShowCmd);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetShowCmd"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

BOOL COXShortcut::SetWorkingDirectory(LPCTSTR pszWorkingDirectory)
	{
	ASSERT(m_psl);
	ASSERT_VALID(this);

	if (Validate(m_psl))
		{
		m_hres = m_psl->SetWorkingDirectory(pszWorkingDirectory);
		}

	TRACE_ERROR_IF_ANY(_T("COXShortcut::SetWorkingDirectory"));
	ThrowExceptionIfNeccessary();
	return (m_hres == NOERROR);
	}

// protected:
void COXShortcut::ThrowExceptionIfNeccessary()
// --- In      :
// --- Out     : 
// --- Returns :
// --- Effect  : throw exception according to the current setting (m_bThrowException)
//				 and m_hres (the HRESULT from last method called).
	{
	if (m_bThrowException && !SUCCEEDED(m_hres))
		AfxThrowOleException(m_hres);
	}

#ifdef _DEBUG
void COXShortcut::TraceFailure(LPCTSTR sFunctionName) const
// --- In      :
// --- Out     : 
// --- Returns :
// --- Effect  : display a trace message according to m_hres (if it is not NOERROR)
	{
	if (m_hres != NOERROR)
		{
		// Get a meaningful text for the error code
		CString sResultMessage = GetResultMessage(m_hres);
		TRACE(_T("%s : Failed (%u == 0x%X, Code : %u) :\n\t%s\n"),
			sFunctionName, m_hres, m_hres, HRESULT_CODE(m_hres), sResultMessage);
		}
	}

static TCHAR szUnknownError[] = _T("*** Unknown Error ***");
static TCHAR szBooleanFalse[] = _T("Function returned (Boolean) FALSE");
static DWORD dwLangID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT); 

CString COXShortcut::GetResultMessage(HRESULT hResult)
	// --- In  : hResult : The result code
	// --- Out : 
	// --- Returns : A string containg a message of the specified code
	// --- Effect : 
	{
	CString sResultMessage;
	if (hResult == S_FALSE)
		{
		//sResultMessage = szBooleanFalse;
		VERIFY(sResultMessage.LoadString(IDS_OX_SHORTCUTBOOLFALSE));//"Function returned (Boolean) FALSE"
		return sResultMessage;
		}

	LPTSTR pszMsgBuf = NULL;
	BOOL bUnknown = FALSE;
	DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;

	// ... Remove the facility part if FACILITY_WIN32
	if (HRESULT_FACILITY(hResult) == FACILITY_WIN32)
		hResult = HRESULT_CODE(hResult);

	// ... Get the actual message 
	if (::FormatMessage(dwFlags, NULL, hResult, dwLangID,
	      (LPTSTR)&pszMsgBuf, 0, NULL) == 0)
		{
		TRACE2("COXShortcut::GetResultMessage : No message was found for result code %i == 0x%8.8X\n",
			hResult, hResult);
	  	//pszMsgBuf = szUnknownError;
		VERIFY(sResultMessage.LoadString(IDS_OX_SHORTCUTUNKERROR));//"*** Unknown Error ***"
		bUnknown = TRUE;
		}
	else
		sResultMessage = pszMsgBuf;

	// ... Clean up
	if (!bUnknown)
		LocalFree(pszMsgBuf);

	return sResultMessage;
	}
#endif // _DEBUG

// end of OXShortcut.cpp
