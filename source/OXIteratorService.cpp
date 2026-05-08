// =============================================================================
// 							Class Implementation : COXIteratorService
// =============================================================================
//
// Source file : 		OXIteratorService.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXIteratorService.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OXITERATORSERVICE_POS_NULL -2

// Data members -------------------------------------------------------------
// protected:
//	CStringArray m_SrvKeyNames;	//	enumerated service key names
//	CStringArray m_SrvDspNames;	//  enumerated service display names
//	int m_nPos;					//  current index

// Member functions ---------------------------------------------------------
COXIteratorService::COXIteratorService(LPCTSTR pszKeyName /* = NULL */) :
COXService(pszKeyName)
{
	m_nPos = OXITERATORSERVICE_POS_NULL;
}

COXIteratorService::COXIteratorService(const COXService& service) :
COXService(service)
{
	m_nPos = OXITERATORSERVICE_POS_NULL;
}

COXIteratorService::COXIteratorService(const COXIteratorService& iteratorService) :
COXService(iteratorService)
{
	if ((m_nPos = iteratorService.m_nPos) != OXITERATORSERVICE_POS_NULL)
	{
		m_SrvKeyNames.Copy(iteratorService.m_SrvKeyNames);
		m_SrvDspNames.Copy(iteratorService.m_SrvDspNames);
	}
}

COXIteratorService& COXIteratorService::operator=(const COXService& service)
{
	End();
	COXService::operator=(service);
	return *this;
}

COXIteratorService& COXIteratorService::operator=(const COXIteratorService& iteratorService)
{
	End();
	COXService::operator=(iteratorService);
	if ((m_nPos = iteratorService.m_nPos) != OXITERATORSERVICE_POS_NULL)
	{
		m_SrvKeyNames.Copy(iteratorService.m_SrvKeyNames);
		m_SrvDspNames.Copy(iteratorService.m_SrvDspNames);
	}
	return *this;
}

BOOL COXIteratorService::StartIteration(
										DWORD dwServiceType /* = SERVICE_WIN32 | SERVICE_DRIVER */,
										DWORD dwServiceState /* = SERVICE_STATE_ALL */)
{
	// we terminate a potential previous iteration
	End();

	if (!PrepareSCManager(SC_MANAGER_ENUMERATE_SERVICE))
		return FALSE;

	// get number of services first
	DWORD dwBytesNeeded, dwServicesReturned, dwResumeHandle = 0;
	BOOL bResult = ::EnumServicesStatus(c_hSCM,
		dwServiceType, dwServiceState, NULL, 0, 
		&dwBytesNeeded, &dwServicesReturned, &dwResumeHandle);

	// no services
	if (bResult) return TRUE;

	// abnormal failures
	if (::GetLastError() != ERROR_MORE_DATA || dwBytesNeeded == 0)
	{
		SetLastError(FALSE);
		return FALSE;
	}

	// get services again
	LPENUM_SERVICE_STATUS pESSArray = 
		(LPENUM_SERVICE_STATUS)malloc(dwBytesNeeded);
	if (pESSArray == NULL)
		AfxThrowMemoryException();

	dwResumeHandle = 0;
	bResult = SetErrCode(::EnumServicesStatus(c_hSCM,
		dwServiceType, dwServiceState, pESSArray, dwBytesNeeded, 
		&dwBytesNeeded, &dwServicesReturned, &dwResumeHandle));

	if (bResult)
	{
		GetNamesFromESSArray(pESSArray, dwServicesReturned);
		m_sKeyName = m_SrvKeyNames[m_nPos = 0];
	}

	free(pESSArray);
	return bResult;
}

BOOL COXIteratorService::StartDependentIteration(
	DWORD dwServiceState /* = SERVICE_STATE_ALL */)
{
	// we terminate a potential previous iteration
	CString sKeyName = GetKeyName();
	End();
	SetKeyName(sKeyName);

	if (!PrepareSCHandle(SERVICE_ENUMERATE_DEPENDENTS))
		return FALSE;

	// get number of dependent services first
	DWORD dwBytesNeeded, dwServicesReturned;
	BOOL bResult = ::EnumDependentServices(m_hSC, dwServiceState, NULL, 0,
		&dwBytesNeeded, &dwServicesReturned);

	// no services
	if (bResult) return TRUE;

	// abnormal failures
	if (::GetLastError() != ERROR_MORE_DATA || dwBytesNeeded == 0)
	{
		SetLastError(FALSE);
		return FALSE;
	}

	// get services again
	LPENUM_SERVICE_STATUS pESSArray = 
		(LPENUM_SERVICE_STATUS)malloc(dwBytesNeeded);
	if (pESSArray == NULL)
		AfxThrowMemoryException();

	bResult = SetErrCode(::EnumDependentServices(m_hSC,
		dwServiceState, pESSArray, dwBytesNeeded, 
		&dwBytesNeeded, &dwServicesReturned));

	if (bResult)
	{
		GetNamesFromESSArray(pESSArray, dwServicesReturned);
		m_sKeyName = m_SrvKeyNames[m_nPos = 0];
	}

	free(pESSArray);
	return bResult;
}

BOOL COXIteratorService::Next()
{
	ASSERT(m_nPos != OXITERATORSERVICE_POS_NULL);
	if (m_nPos == OXITERATORSERVICE_POS_NULL || 
		m_nPos == m_SrvKeyNames.GetSize())
		return FALSE;

	Empty();
	m_nPos++;
	if (m_nPos == m_SrvKeyNames.GetSize())
		return FALSE;

	m_sKeyName = m_SrvKeyNames[m_nPos];
	return TRUE;
}

COXIteratorService& COXIteratorService::operator++()
{
	Next();
	return *this;
}

COXIteratorService COXIteratorService::operator+(int nOffset)
{
	ASSERT(m_nPos != OXITERATORSERVICE_POS_NULL);
	if (m_nPos == OXITERATORSERVICE_POS_NULL)
		return *this;

	Empty();
	m_nPos += nOffset;
	if (m_nPos <= -1)
		m_nPos = -1;
	else if (m_nPos >= m_SrvKeyNames.GetSize())
		m_nPos = PtrToInt(m_SrvKeyNames.GetSize());
	else
		m_sKeyName = m_SrvKeyNames[m_nPos];
	return *this;
}

BOOL COXIteratorService::Prev()
{
	ASSERT(m_nPos != OXITERATORSERVICE_POS_NULL);
	if (m_nPos == OXITERATORSERVICE_POS_NULL || m_nPos == -1)
		return FALSE;

	Empty();
	m_nPos--;
	if (m_nPos == -1) return FALSE;

	m_sKeyName = m_SrvKeyNames[m_nPos];
	return TRUE;
}

COXIteratorService& COXIteratorService::operator--()
{
	Prev();
	return *this;
}

COXIteratorService COXIteratorService::operator-(int nOffset)
{
	return operator+(-nOffset);
}

void COXIteratorService::GetKeyNames(CStringArray& sBuffer)
{
	ASSERT(m_nPos != OXITERATORSERVICE_POS_NULL);
	sBuffer.Copy(m_SrvKeyNames);
}

void COXIteratorService::GetDisplayNames(CStringArray& sBuffer)
{
	ASSERT(m_nPos != OXITERATORSERVICE_POS_NULL);
	sBuffer.Copy(m_SrvDspNames);
}

int COXIteratorService::GetServiceCount()
{
	return PtrToInt(m_SrvKeyNames.GetSize());
}

void COXIteratorService::End()
{
	Empty();
	m_SrvKeyNames.RemoveAll();
	m_SrvDspNames.RemoveAll();
	m_nPos = -1;
}

// protected:
void COXIteratorService::GetNamesFromESSArray(LPENUM_SERVICE_STATUS pESSArray,
											  DWORD dwServicesReturned)
// --- In      : pESSArray, the service status array
//				 dwServicesReturned, service status number
// --- Out     :
// --- Returns : 
// --- Effect  : copy service key names and display names from a 
//				 service status array
{
	m_SrvKeyNames.SetSize(dwServicesReturned);
	m_SrvDspNames.SetSize(dwServicesReturned);
	BYTE* pCurESS = (BYTE*)pESSArray;

	for (DWORD i = 0; i < dwServicesReturned; i++)
	{
		m_SrvKeyNames[i] = ((LPENUM_SERVICE_STATUS)pCurESS)->lpServiceName;
		m_SrvDspNames[i] = ((LPENUM_SERVICE_STATUS)pCurESS)->lpDisplayName;
		pCurESS += sizeof(ENUM_SERVICE_STATUS);
	}
}

// end of OXIteratorService.cpp