// =============================================================================
// 							Class Implementation : COXService
// =============================================================================
//
// Source file : 		OXService.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXService.h"

#include "UTBStrOp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Data members -------------------------------------------------------------
// protected:
static const TCHAR szBar[] = _T("|");
const TCHAR COXService::m_cNull = _T('\0');
const TCHAR COXService::m_cBar = _T('|');
const LPCTSTR COXService::m_pszBar = szBar;
SC_HANDLE COXService::c_hSCM = NULL;	// handle of the service manager
SC_LOCK COXService::c_Sclock = NULL;	// handle of the service lock
DWORD COXService::c_dwSCMAccess = 0;	// last access when openning service manager
//	SC_HANDLE m_hSC;					// handle of the service
//	DWORD m_dwErrCode;					// last error code
//	CString m_sKeyName;					// current service's key name

// Member functions ---------------------------------------------------------

COXService::COXService(LPCTSTR pszKeyName /* = NULL */)
	{
	m_hSC = NULL;
	m_dwErrCode = 0;
	if (pszKeyName)
		m_sKeyName = pszKeyName;
	}

COXService::COXService(const COXService& service)
	{
	m_hSC = NULL;
	m_dwErrCode = 0;
	m_sKeyName = service.m_sKeyName;
	}

COXService::~COXService()
	{
	Close();
	}

COXService& COXService::operator=(const COXService& service)
	{
	if(this==&service)
		return *this;
		
	m_hSC = NULL;
	m_dwErrCode = 0;
	m_sKeyName = service.m_sKeyName;
	return *this;
	}

DWORD COXService::GetLastError() const
	{
	return m_dwErrCode;
	}

void COXService::Empty()
	{
	Close();
	ASSERT(m_hSC == NULL);

	m_dwErrCode = 0;
	m_sKeyName.Empty();
	}

BOOL COXService::IsEmpty() const
	{
	return m_sKeyName.IsEmpty();
	}

CString COXService::GetKeyName() const
	{
	return m_sKeyName;
	}

void COXService::SetKeyName(LPCTSTR pszKeyName)
	{
	if (!IsEmpty())
		{
		TRACE0("COXService::SetKeyName(): service was not empty.\r\n");
		Empty();
		}

	m_sKeyName = pszKeyName;
	}

SC_HANDLE COXService::GetHandle()
	{
	if (m_hSC == NULL)
		Open();

	return m_hSC;
	}

BOOL COXService::Open(DWORD dwDesiredAccess /* = SERVICE_ALL_ACCESS */,
					  LPCTSTR pszKeyName /* = NULL */)
	{
	if (m_hSC != NULL && !Close())
		return FALSE;

	if (pszKeyName)
		m_sKeyName = pszKeyName;

	if (PrepareSCManager() && SetErrCode((m_hSC = ::OpenService(c_hSCM, 
			m_sKeyName, dwDesiredAccess)) != NULL))
		{
		TRACE1("COXService::Open(): service '%s' opened.\r\n", m_sKeyName);
		return TRUE;
		}
	return FALSE;
	}

BOOL COXService::Create(LPCTSTR pszBinaryPathName,
						LPCTSTR pszDisplayName, 
						LPCTSTR pszKeyName /* = NULL */, 
						DWORD dwDesiredAccess /* = SERVICE_ALL_ACCESS */, 
						DWORD dwServiceType /* = SERVICE_WIN32_OWN_PROCESS */, 
						DWORD dwStartType /* = SERVICE_DEMAND_START */, 
						DWORD dwErrorControl /* = SERVICE_ERROR_NORMAL */, 
						LPCTSTR pszLoadOrderGroup /* = NULL */, 
						LPDWORD pdwTagId /* = NULL */, 
						LPCTSTR pszDependencies /* = NULL */, 
						LPCTSTR pszServiceStartName /* = NULL */, 
						LPCTSTR pszPassword /* = NULL */)
	{
	if (m_hSC != NULL && !Close())
		return FALSE;

	if (pszKeyName)
		m_sKeyName = pszKeyName;

	if (PrepareSCManager() &&
		SetErrCode((m_hSC = ::CreateService(c_hSCM, m_sKeyName,
			pszDisplayName, dwDesiredAccess, dwServiceType, dwStartType,
			dwErrorControl, pszBinaryPathName, pszLoadOrderGroup, pdwTagId, 
			pszDependencies, pszServiceStartName, pszPassword)) != NULL))
		{
		TRACE1("COXService::Create(): service '%s' created.\r\n", m_sKeyName);
		return TRUE;
		}
	return FALSE;
	}

BOOL COXService::Close()
	{
	if (m_hSC == NULL)
		return TRUE;

	if (SetErrCode(::CloseServiceHandle(m_hSC)))
		{
		m_hSC = NULL;
		TRACE1("COXService::Close(): service '%s' closed.\r\n", m_sKeyName);
		return TRUE;
		}
	return FALSE;
	}

BOOL COXService::Delete()
	{
	return PrepareSCHandle(DELETE) && SetErrCode(::DeleteService(m_hSC));
	}

BOOL COXService::Start(DWORD dwNumServiceArgs /* = 0 */, 
					   LPCTSTR* rgpszServiceArgVectors /* = NULL */)
	{
	return PrepareSCHandle(SERVICE_START) &&
		SetErrCode(::StartService(m_hSC, dwNumServiceArgs, 
		rgpszServiceArgVectors));
	}

BOOL COXService::Pause()
	{
	SERVICE_STATUS srvStatus;
	return PrepareSCHandle(SERVICE_PAUSE_CONTINUE) &&
		SetErrCode(::ControlService(m_hSC, SERVICE_CONTROL_PAUSE, &srvStatus));
	}

BOOL COXService::Continue()
	{
	SERVICE_STATUS srvStatus;
	return PrepareSCHandle(SERVICE_PAUSE_CONTINUE) &&
		SetErrCode(::ControlService(m_hSC, SERVICE_CONTROL_CONTINUE, &srvStatus));
	}

BOOL COXService::Stop()
	{
	SERVICE_STATUS srvStatus;
	return PrepareSCHandle(SERVICE_STOP) &&
		SetErrCode(::ControlService(m_hSC, SERVICE_CONTROL_STOP, &srvStatus));
	}

BOOL COXService::Control(DWORD dwControl, 
						 LPSERVICE_STATUS pszServiceStatus /* = NULL */)
	{
	DWORD dwMinimumAccess;
	switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
			dwMinimumAccess = SERVICE_STOP;
			break;
		case SERVICE_CONTROL_PAUSE:
		case SERVICE_CONTROL_CONTINUE:
			dwMinimumAccess = SERVICE_PAUSE_CONTINUE;
			break;
		case SERVICE_CONTROL_INTERROGATE:
			dwMinimumAccess = SERVICE_INTERROGATE;
			break;
		case SERVICE_CONTROL_SHUTDOWN:
			return FALSE;
		default:
			if (dwControl >= 128 && dwControl <= 255)
				dwMinimumAccess = SERVICE_USER_DEFINED_CONTROL;
			else
				dwMinimumAccess = SERVICE_ALL_ACCESS;
		}

	
	// We noticed that if ::ControlService is called with an invalid controlcode
	// for that service, it tries to fill the SERVICE_STATUS structure parameter.
	// If this parameter is NULL, two exceptions are thrown and the true errorcode
	// ERROR_INVALID_SERVICE_CONTROL is never seen. So even if the user isn't 
	// interested in the SERVICE_STATUS structure, we have to supply it.
	if (pszServiceStatus == NULL)
		{
		SERVICE_STATUS srvStatus;
		return PrepareSCHandle(dwMinimumAccess) &&
			SetErrCode(::ControlService(m_hSC, dwControl, &srvStatus));
		}
	else
		return PrepareSCHandle(dwMinimumAccess) &&
			SetErrCode(::ControlService(m_hSC, dwControl, pszServiceStatus));
	}

/////////////////////////////////////////////////////////////////////////////
// BOOL ChangeServiceConfig(SC_HANDLE hService, 
//							DWORD dwServiceType, 
//							DWORD dwStartType, 
//							DWORD dwErrorControl, 
//							LPCTSTR lpBinaryPathName, 
//							LPCTSTR lpLoadOrderGroup, 
//							LPDWORD lpdwTagId, 
//							LPCTSTR lpDependencies, 
//							LPCTSTR lpServiceStartName, 
//							LPCTSTR lpPassword, 
//							LPCTSTR lpDisplayName)
BOOL COXService::ChangeConfig(LPCTSTR pszDisplayName /* = NULL */, 
							  DWORD dwServiceType /* = SERVICE_NO_CHANGE */, 
							  DWORD dwStartType /* = SERVICE_NO_CHANGE */, 
							  DWORD dwErrorControl /* = SERVICE_NO_CHANGE */, 
							  LPCTSTR pszBinaryPathName /* = NULL */, 
							  LPCTSTR pszLoadOrderGroup /* = NULL */, 
							  LPDWORD pdwTagId /* = NULL */, 
							  LPCTSTR pszDependencies /* = NULL */, 
							  LPCTSTR pszServiceStartName /* = NULL */, 
							  LPCTSTR pszPassword /* = NULL */)
	{
	return PrepareSCHandle(SERVICE_CHANGE_CONFIG) &&
		SetErrCode(::ChangeServiceConfig(m_hSC, 
		dwServiceType, dwStartType, dwErrorControl,
		pszBinaryPathName, pszLoadOrderGroup, pdwTagId, pszDependencies,
		pszServiceStartName, pszPassword, pszDisplayName));
	}

BOOL COXService::ChangeDisplayName(LPCTSTR pszDisplayName)
	{
	return ChangeConfig(pszDisplayName);
	}

BOOL COXService::ChangeServiceType(DWORD dwServiceType)
	{
	return ChangeConfig(NULL, dwServiceType);
	}

BOOL COXService::ChangeStartType(DWORD dwStartType)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, dwStartType);
	}

BOOL COXService::ChangeErrorControl(DWORD dwErrorControl)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		dwErrorControl);
	}

BOOL COXService::ChangeBinaryPathName(LPCTSTR pszBinaryPathName)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		SERVICE_NO_CHANGE, pszBinaryPathName);
	}

BOOL COXService::ChangeLoadOrderGroup(LPCTSTR pszLoadOrderGroup)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		SERVICE_NO_CHANGE, NULL, pszLoadOrderGroup);
	}

BOOL COXService::ChangeTagId(DWORD dwTagId)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		SERVICE_NO_CHANGE, NULL, NULL, &dwTagId);
	}

BOOL COXService::ChangeDependencies(LPCTSTR pszDependencies)
	{
	CString sDependencies = pszDependencies;
	int nMinBufLength = sDependencies.GetLength() + 1;
	BarToNullSeparator(sDependencies.GetBuffer(nMinBufLength));
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		SERVICE_NO_CHANGE, NULL, NULL, NULL, sDependencies);
	}

BOOL COXService::ChangeStartName(LPCTSTR pszServiceStartName, 
								 LPCTSTR pszPassword /* = NULL */)
	{
	return ChangeConfig(NULL, SERVICE_NO_CHANGE, SERVICE_NO_CHANGE, 
		SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL,
		pszServiceStartName, pszPassword);
	}

/////////////////////////////////////////////////////////////////////////////
// BOOL QueryServiceConfig( SC_HANDLE hService, 
//							LPQUERY_SERVICE_CONFIG lpServiceConfig, 
//							DWORD cbBufSize, 
//							LPDWORD pcbBytesNeeded) 
// typedef struct _QUERY_SERVICE_CONFIG { // qsc 
//    DWORD dwServiceType; 
//    DWORD dwStartType; 
//    DWORD dwErrorControl; 
//    LPTSTR lpBinaryPathName; 
//    LPTSTR lpLoadOrderGroup; 
//    DWORD dwTagId; 
//    LPTSTR lpDependencies; 
//    LPTSTR lpServiceStartName; 
//    LPTSTR lpDisplayName; 
//	} QUERY_SERVICE_CONFIG, LPQUERY_SERVICE_CONFIG; 
 
BOOL COXService::QueryConfig(LPQUERY_SERVICE_CONFIG lpServiceConfig, 
							 DWORD cbBufSize, 
							 LPDWORD pcbBytesNeeded)
	{
	return PrepareSCHandle(SERVICE_QUERY_CONFIG) &&
		SetErrCode(::QueryServiceConfig(m_hSC, lpServiceConfig, 
		cbBufSize, pcbBytesNeeded));
	}

#define OXSERVICE_QSC_BINARYPATHNAME		1
#define OXSERVICE_QSC_LOADORDERGROUP		2
#define OXSERVICE_QSC_DEPENDENCIES			3
#define OXSERVICE_QSC_SERVICESTARTNAME		4
#define OXSERVICE_QSC_DISPLAYNAME			5
#define OXSERVICE_QSC_SERVICETYPE			6
#define OXSERVICE_QSC_STARTTYPE				7
#define OXSERVICE_QSC_ERRORCONTROL			8
#define OXSERVICE_QSC_TAGID					9

CString COXService::QueryDisplayName()
	{
	return GetQSCData(OXSERVICE_QSC_DISPLAYNAME);
	}

CString COXService::QueryBinaryPathName()
	{
	return GetQSCData(OXSERVICE_QSC_BINARYPATHNAME);
	}

CString COXService::QueryLoadOrderGroup()
	{
	return GetQSCData(OXSERVICE_QSC_LOADORDERGROUP);
	}

CString COXService::QueryDependencies()
	{
	return GetQSCData(OXSERVICE_QSC_DEPENDENCIES);
	}

CString COXService::QueryStartName()
	{
	return GetQSCData(OXSERVICE_QSC_SERVICESTARTNAME);
	}

DWORD COXService::QueryServiceType()
	{
	DWORD dwResult;
	GetQSCData(OXSERVICE_QSC_SERVICETYPE, &dwResult);
	return dwResult;
	}

DWORD COXService::QueryStartType()
	{
	DWORD dwResult;
	GetQSCData(OXSERVICE_QSC_STARTTYPE, &dwResult);
	return dwResult;
	}

DWORD COXService::QueryErrorControl()
	{
	DWORD dwResult;
	GetQSCData(OXSERVICE_QSC_ERRORCONTROL, &dwResult);
	return dwResult;
	}

DWORD COXService::QueryTagId()
	{
	DWORD dwResult;
	GetQSCData(OXSERVICE_QSC_TAGID, &dwResult);
	return dwResult;
	}

/////////////////////////////////////////////////////////////////////////////
BOOL COXService::QuerySecurity(SECURITY_INFORMATION dwSecurityInformation, 
							   PSECURITY_DESCRIPTOR lpSecurityDescriptor, 
							   DWORD cbBufSize, 
							   LPDWORD pcbBytesNeeded)
	{
	return PrepareSCHandle(READ_CONTROL) &&
		SetErrCode(::QueryServiceObjectSecurity(m_hSC, dwSecurityInformation,
		lpSecurityDescriptor, cbBufSize, pcbBytesNeeded));
	}

BOOL COXService::ChangeSecurity(SECURITY_INFORMATION dwSecurityInformation, 
								PSECURITY_DESCRIPTOR lpSecurityDescriptor)
	{
	DWORD dwMinimumAccess = 0;
	if (dwSecurityInformation & OWNER_SECURITY_INFORMATION ||
		dwSecurityInformation & GROUP_SECURITY_INFORMATION)
		dwMinimumAccess |= WRITE_OWNER;
	if (dwSecurityInformation & DACL_SECURITY_INFORMATION)
		dwMinimumAccess |= WRITE_DAC;

	return PrepareSCHandle(dwMinimumAccess) &&
		SetErrCode(::SetServiceObjectSecurity(m_hSC, dwSecurityInformation,
		lpSecurityDescriptor));
	}

BOOL COXService::QueryStatus(LPSERVICE_STATUS lpServiceStatus,
							 BOOL bInterrogateNow /* = FALSE */)
	{
	if (bInterrogateNow)
		return Control(SERVICE_CONTROL_INTERROGATE, lpServiceStatus);
	
	return PrepareSCHandle(SERVICE_QUERY_STATUS) && 
		SetErrCode(::QueryServiceStatus(m_hSC, lpServiceStatus));
	}

DWORD COXService::QueryCurrentState(BOOL bInterrogateNow /* = FALSE*/)
	{
	SERVICE_STATUS ss;
	VERIFY(QueryStatus(&ss, bInterrogateNow));
	return ss.dwCurrentState;
	}

CString COXService::QueryDisplayName(LPCTSTR pszKeyName)
	{
	if (!PrepareSCManager())
		return _T("");

	CString sResult;
	DWORD dwCharNeeded = 256;
	BOOL bResult = ::GetServiceDisplayName(c_hSCM, pszKeyName,
		sResult.GetBuffer(256), &dwCharNeeded);
	sResult.ReleaseBuffer(bResult ? -1 : 0);
	return sResult;
	}

CString COXService::QueryKeyName(LPCTSTR pszDisplayName)
	{
	if (!PrepareSCManager())
		return _T("");

	CString sResult;
	DWORD dwCharNeeded = 256;
	BOOL bResult = ::GetServiceKeyName(c_hSCM, pszDisplayName,
		sResult.GetBuffer(256), &dwCharNeeded);
	sResult.ReleaseBuffer(bResult ? -1 : 0);
	return sResult;
	}

/////////////////////////////////////////////////////////////////////////////
// Static SCManager Functions

SC_HANDLE COXService::GetSCManagerHandle()
	{
	if (c_hSCM == NULL)
		OpenSCManager();

	return c_hSCM;
	}

BOOL COXService::OpenSCManager(LPCTSTR pszMachineName /* = NULL */, 
							   LPCTSTR pszDatabaseName /* = NULL */, 
							   DWORD dwDesiredAccess /* = SC_MANAGER_ALL_ACCESS */)
	{
	if (c_hSCM != NULL)
		{
		if (c_dwSCMAccess == dwDesiredAccess)
			{
			TRACE0("COXService::OpenSCManager(): desired SCManager was already opened previously.\r\n");
			return TRUE;
			}

		if (!CloseSCManager())
			return FALSE;
		
		TRACE0("COXService::OpenSCManager(): previously opened SCManager was closed by this function.\r\n");
		}
	
	c_dwSCMAccess = dwDesiredAccess;
	return (c_hSCM = ::OpenSCManager(pszMachineName, pszDatabaseName, 
		dwDesiredAccess)) != NULL;
	}

BOOL COXService::LockDatabase()
	{
	if (c_hSCM != NULL && c_Sclock != NULL)
		{
		TRACE0("COXService::LockDatabase(): SCM database was already locked.\r\n");
		return TRUE;
		}

	return PrepareSCManager(SC_MANAGER_LOCK) &&
		((c_Sclock = ::LockServiceDatabase(c_hSCM)) != NULL);
	}

BOOL COXService::UnlockDatabase()
	{
	// ... SCM has to be opened expicitly or implicitly already
	ASSERT(c_hSCM);
	if (c_hSCM != NULL)
		{
		if (c_Sclock == NULL)
			{
			TRACE0("COXService::UnlockDatabase(): SCM database was not locked.\r\n");
			return TRUE;
			}

		if (::UnlockServiceDatabase(c_Sclock))
			{
			c_Sclock = NULL;
			return TRUE;
			}
		}
	return FALSE;
	}

BOOL COXService::QueryDatabaseLockStatus(DWORD& /* bIsLocked */, 
										 CString& sLockOwner, 
										 DWORD& /* dwLockDuration */)
	{
	if (!PrepareSCManager(SC_MANAGER_QUERY_LOCK_STATUS))
		return FALSE;

	QUERY_SERVICE_LOCK_STATUS qsls;
	DWORD dwBytesNeeded;

	qsls.lpLockOwner = sLockOwner.GetBuffer(256);
	BOOL bResult = ::QueryServiceLockStatus(c_hSCM, &qsls, 
			256, &dwBytesNeeded);

	if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
		sLockOwner.ReleaseBuffer(0);
		qsls.lpLockOwner = sLockOwner.GetBuffer(dwBytesNeeded);
		bResult = ::QueryServiceLockStatus(c_hSCM, &qsls, 
				dwBytesNeeded, &dwBytesNeeded);
		}

	sLockOwner.ReleaseBuffer(bResult ? -1 : 0);
	return bResult;
	}

BOOL COXService::CloseSCManager()
	{
	if (c_hSCM == NULL)
		return TRUE;

	if (c_Sclock != NULL && !UnlockDatabase())
		return FALSE;

	if (::CloseServiceHandle(c_hSCM))
		{
		c_hSCM = NULL;
		return TRUE;
		}
	return FALSE;
	}

// protected:
BOOL COXService::SetErrCode(BOOL bFunctionSucceeded)
	// --- In      : bFunctionSucceeded, function return BOOL value
	// --- Out     : 
	// --- Returns : same as bFunctionSucceeded (passing it through)
	// --- Effect  : if last called function is failed, write down last error
	{
	m_dwErrCode = bFunctionSucceeded ? 0 : ::GetLastError();
#ifdef _DEBUG
	if (!bFunctionSucceeded)
		{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, m_dwErrCode, 0, (LPTSTR) &lpMsgBuf, 0, NULL);
		TRACE1("COXService Error: %s", lpMsgBuf);
		LocalFree(lpMsgBuf);
		}
#endif
	return bFunctionSucceeded;
	}

BOOL COXService::PrepareSCHandle(DWORD dwMinimumAccess /* = SERVICE_ALL_ACCESS */)
	// --- In      : dwMinimumAccess, the minimum access right of the service handle
	// --- Out     : 
	// --- Returns : TRUE if successful; FALSE otherwise
	// --- Effect  : if service handle is already opened, use it; if not, try to open
	//				 it with full access right (SERVICE_ALL_ACCESS); if failed,
	//				 try to open it with minimum access (dwMinimumAccess).
	{
	return m_hSC != NULL || Open(SERVICE_ALL_ACCESS) ||
		(dwMinimumAccess != SERVICE_ALL_ACCESS && Open(dwMinimumAccess));
	}

CString COXService::GetQSCData(int QSC_Code, DWORD* pdwResult /* = NULL */)
	// --- In      : QSC_Code, the code as above
	// --- Out     : pdwResult, the DWORD field result
	// --- Returns : the string field result
	// --- Effect  : obtain one field of the query service status
	{
	CString sResult;
	if (pdwResult != NULL)
		*pdwResult = 0;

	if (!PrepareSCHandle(SERVICE_QUERY_CONFIG))
		return _T("");

	DWORD dwBytesNeeded;
	if (::QueryServiceConfig(m_hSC, NULL, 0, &dwBytesNeeded) ||
		::GetLastError() != ERROR_INSUFFICIENT_BUFFER ||
		dwBytesNeeded == 0)
		{
		SetLastError(FALSE);
		return _T("");
		}

	LPQUERY_SERVICE_CONFIG pQSC = (LPQUERY_SERVICE_CONFIG)malloc(dwBytesNeeded);
	if (SetErrCode(::QueryServiceConfig(m_hSC, pQSC,
		dwBytesNeeded, &dwBytesNeeded)))
		{
		switch (QSC_Code)
			{
			case OXSERVICE_QSC_BINARYPATHNAME:
				sResult = pQSC->lpBinaryPathName; break;
			case OXSERVICE_QSC_LOADORDERGROUP:
				sResult = pQSC->lpLoadOrderGroup; break;
			case OXSERVICE_QSC_DEPENDENCIES:
				NullToBarSeparator(pQSC->lpDependencies);
				sResult = pQSC->lpDependencies; break;
			case OXSERVICE_QSC_SERVICESTARTNAME:
				sResult = pQSC->lpServiceStartName; break;
			case OXSERVICE_QSC_DISPLAYNAME:
				sResult = pQSC->lpDisplayName; break;
			case OXSERVICE_QSC_SERVICETYPE:
				*pdwResult = pQSC->dwServiceType; break;
			case OXSERVICE_QSC_STARTTYPE:
				*pdwResult = pQSC->dwStartType; break;
			case OXSERVICE_QSC_ERRORCONTROL:
				*pdwResult = pQSC->dwErrorControl; break;
			case OXSERVICE_QSC_TAGID:
				*pdwResult = pQSC->dwTagId; break;
			}
		}

	free(pQSC);
	return sResult;
	}

BOOL COXService::PrepareSCManager(DWORD dwMinimumAccess /* = SC_MANAGER_ALL_ACCESS */)
	// --- In      : dwMinimumAccess, the minimum access right of the SCManager
	// --- Out     : 
	// --- Returns : TRUE if successful; FALSE otherwise
	// --- Effect  : if SCManager is already opened, use it; if not, try to open
	//				 it with full access right (SC_MANAGER_ALL_ACCESS); if failed,
	//				 try to open it with minimum access (dwMinimumAccess).
	{
	return c_hSCM != NULL || OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) ||
		(dwMinimumAccess != SC_MANAGER_ALL_ACCESS && 
		OpenSCManager(NULL, NULL, dwMinimumAccess));
	}
 
void COXService::NullToBarSeparator(LPTSTR pStringData)
	// --- In  : pStringData : Pointer to string with parts seperated by null chars
	//							and concluded by two null chars
	// --- Out : pStringData : The same string but bars '|' used to seperate the parts
	//						   The string still end in a null char (without a bar)
	// --- Returns :
	// --- Effect : Converts the string
	{
	BOOL bPreviousWasNull = FALSE;
	BOOL bEnd = FALSE;
	BOOL bFirstSymbol=TRUE;

	while (!bEnd)
		{
		// ... String pointer should point to a valid string at all times
		ASSERT(AfxIsValidString(pStringData));
		if (*pStringData == m_cNull)
			{
			if(bFirstSymbol)
				return;

			if (bPreviousWasNull)
				{
				// ... This char is NULL and so was the previous :
				//     zero-terminate the string and end the loop
				*(pStringData - 1) = m_cNull;
				bEnd = TRUE;
				}
			else
				{
				// ... This char is NULL but the previous was not:
				//     replace the zero-char by a bar
				*pStringData = m_cBar;
				bPreviousWasNull = TRUE;
				}
			}
		else
			// ... Encountered a normal (non-NULL-char)
			bPreviousWasNull = FALSE;

		pStringData++;
		bFirstSymbol=FALSE;
		}
	}

void COXService::BarToNullSeparator(LPTSTR pStringData)
	// --- In  : pStringData : A string with bars '|' used to seperate the parts
	//						   The string end in a null char (without a bar)
	// --- Out : pStringData : The same string but with parts seperated by null chars
	//						   and concluded by two null chars
	// --- Returns :
	// --- Effect : Converts the string
	//				Note that the string will grow with one character.
	//				Enough space should have been allocated before callinbg this function
{
	// ... The pStringData should have enough space one additional (null) characters
	ASSERT(AfxIsValidAddress(pStringData, (_tcslen(pStringData) + 2) * sizeof(TCHAR)));

	// First add an extra NULL character at the end
	*(pStringData + _tcslen(pStringData) + 1) = m_cNull;

	// Now replace all bars by a NULL character (use _tcstok)
	TCHAR * p;
	LPCTSTR pNewStringData = UTBStr::tcstok(pStringData, m_pszBar, &p);
	while(pNewStringData != NULL)
	{
		pNewStringData = UTBStr::tcstok(NULL, m_pszBar, &p);
	}
}

// end of OXService.cpp