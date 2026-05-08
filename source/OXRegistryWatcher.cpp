// ==========================================================================
//					Class Implementation : COXRegistryWatcher
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXRegistryWatcher.h"

/////////////////////////////////////////////////////////////////////////////
// Data members -------------------------------------------------------------
// public:

const DWORD COXRegistryWatcher::OXRegistryWatchChangeName = REG_NOTIFY_CHANGE_NAME;
const DWORD COXRegistryWatcher::OXRegistryWatchChangeAttributes = REG_NOTIFY_CHANGE_ATTRIBUTES;
const DWORD COXRegistryWatcher::OXRegistryWatchChangeLastSet = REG_NOTIFY_CHANGE_LAST_SET;
const DWORD COXRegistryWatcher::OXRegistryWatchChangeSecurity = REG_NOTIFY_CHANGE_SECURITY;

// protected:

	//	CEvent m_EventWatchLoop;
	//	--- When this event is signaled, "watch queue" thread stops.

	//	CEvent m_EventWatchRestart;
	//	--- When this event is signaled, "watch queue" restarts.

	//	CEvent m_EventWatchBuildBegin;
	//	--- When this event is signaled, "watch queue" suspends and its rebuilding starts.

	//	CEvent m_EventWatchBuildEnd;
	//	--- When this event is signaled, "watch queue" rebuilding ends and watching continues.

	//	HRESULT m_hResultError;
	//	--- Last occured error code.

	//	DWORD m_dwWatchesNumber;
	//	--- Number of watched keys.

	//	CWinThread* m_pNotificationThread;
	//	--- Points to CWinThread object that represents watch thread.

	//	COXRegistryWatchNotifier* m_pRegistryWatchNotifier;
	//	--- Points to array of COXRegistryWatchNotifier objects that stores
	//		parameters of "watch queue".

/////////////////////////////////////////////////////////////////////////////
// Member functions ---------------------------------------------------------
// public:

COXRegistryWatcher::COXRegistryWatcher()
	{
	m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
	m_dwWatchesNumber = 0;
	m_pNotificationThread = NULL;
	m_pRegistryWatchNotifier = NULL;
	}

COXRegistryWatcher::~COXRegistryWatcher()
{
	RemoveAllWatches();
}

BOOL COXRegistryWatcher::IsWatchingSupported()
{
	BOOL			bSupported = FALSE;
	OSVERSIONINFO	verInfo;
	
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (::GetVersionEx(&verInfo))
	{
		m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
		if((verInfo.dwPlatformId==VER_PLATFORM_WIN32_NT && verInfo.dwMajorVersion>=4) || 
			(verInfo.dwMajorVersion>=4 && verInfo.dwMinorVersion>=10))
		{
			bSupported = TRUE;
		}
	}
	else
	{
		m_hResultError = OX_REGISTRY_WATCHER_VERSION_FAILURE;
	}
	
	return bSupported;
}

DWORD COXRegistryWatcher::AddWatch(HKEY hRegKey, BOOL bWatchSubtree, DWORD dwWatchFilter)
{
	DWORD dwID = 0;
	DWORD dwCount = 0;
	DWORD dwNewID = 0;
	
	if (hRegKey != NULL)
	{
		CEvent* phWatchEvent = new CEvent;
		if (::RegNotifyChangeKeyValue(hRegKey, bWatchSubtree, dwWatchFilter,
			(HANDLE)(*phWatchEvent), TRUE) == ERROR_SUCCESS)
		{
			BOOL bWatchStarted = IsWatchStarted();
			if (!bWatchStarted || m_EventWatchRestart.SetEvent())
			{
				if (bWatchStarted)
				{
					CSingleLock BuildBeginLock(&m_EventWatchBuildBegin);
					BuildBeginLock.Lock();
				}
				
				COXRegistryWatchNotifier *pRegistryWatchNotifier =
					new COXRegistryWatchNotifier[m_dwWatchesNumber+1];
				for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
					pRegistryWatchNotifier[dwCount] = m_pRegistryWatchNotifier[dwCount];
				dwNewID = FindNewID();
				pRegistryWatchNotifier[m_dwWatchesNumber].SetMembers(hRegKey,
					bWatchSubtree, dwWatchFilter, phWatchEvent, dwNewID);
				if (m_pRegistryWatchNotifier)
					delete [] m_pRegistryWatchNotifier;
				m_pRegistryWatchNotifier = pRegistryWatchNotifier;
				m_dwWatchesNumber++;
				
				if (bWatchStarted)
				{
					if (m_EventWatchBuildEnd.SetEvent())
					{
						m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
						dwID = dwNewID;
					}
					else
						m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
				}
				else
				{
					if (StartWatchThread())
						dwID = dwNewID;
				}
			}
			else
				m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
		}
		else
		{
			delete phWatchEvent;
			m_hResultError = OX_REGISTRY_WATCHER_EVENT_FAILURE;
		}
	}
	else
		m_hResultError = OX_REGISTRY_WATCHER_NO_HKEY;
	
	return dwID;
}

BOOL COXRegistryWatcher::OnNotify(COXRegistryWatchNotifier* /* pRegWatchNotifier */)
	{
	return FALSE;
	}

BOOL COXRegistryWatcher::GetWatchIDsFromKey(HKEY hRegKey, CDWordArray& IDs)
	{
	BOOL	bFilled = FALSE;
	DWORD	dwCount = 0;
	
	if (m_dwWatchesNumber > 0)
		{
		m_hResultError = OX_REGISTRY_WATCHER_NO_NOTIFIER;
		IDs.RemoveAll();
		for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
			{
			if (m_pRegistryWatchNotifier[dwCount].GetRegKey() == hRegKey &&
				IDs.Add(m_pRegistryWatchNotifier[dwCount].GetWatchID()) >= 0)
				{
				m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
				bFilled = TRUE;
				}
			}
		}
	else
		m_hResultError = OX_REGISTRY_WATCHER_EMPTY_WATCHER;
	
	return bFilled;
	}

DWORD COXRegistryWatcher::GetNotifCount()
	{
	m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
	return m_dwWatchesNumber;
	}

BOOL COXRegistryWatcher::RemoveWatch(DWORD dwID)
	{
	BOOL	bSuccess = FALSE;
	DWORD	dwCount = 0, dwIndex = 0;
	
	if (GetWatchNotifier(dwID, &dwIndex))
		{
		if (m_dwWatchesNumber == 1)
			bSuccess = RemoveAllWatches();
		else if (m_EventWatchRestart.SetEvent())
			{
			CSingleLock BuildBeginLock(&m_EventWatchBuildBegin);
			BuildBeginLock.Lock();
			
			COXRegistryWatchNotifier *pRegistryWatchNotifier =
				new COXRegistryWatchNotifier[--m_dwWatchesNumber];
			for (dwCount = 0; dwCount < dwIndex; dwCount++)
				pRegistryWatchNotifier[dwCount] = m_pRegistryWatchNotifier[dwCount];
			for (dwCount = dwIndex; dwCount < m_dwWatchesNumber; dwCount++)
				pRegistryWatchNotifier[dwCount] = m_pRegistryWatchNotifier[dwCount+1];
			if (m_pRegistryWatchNotifier)
				{
				if (m_pRegistryWatchNotifier[dwIndex].GetEvent())
					delete m_pRegistryWatchNotifier[dwIndex].GetEvent();
				delete [] m_pRegistryWatchNotifier;
				}
			m_pRegistryWatchNotifier = pRegistryWatchNotifier;
			
			if (m_EventWatchBuildEnd.SetEvent())
				{
				m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
				bSuccess = TRUE;
				}
			else
				m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
			}
		else
			m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
		}
	
	return bSuccess;
	}

BOOL COXRegistryWatcher::RemoveAllWatches()
	{
	BOOL	bSuccess = FALSE;
	DWORD	dwCount = 0;
	
	if (m_dwWatchesNumber > 0)
		{
		if (!IsWatchStarted() || StopWatchThread())
			{
			if (m_pRegistryWatchNotifier)
				{
				for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
					{
					if (m_pRegistryWatchNotifier[dwCount].GetEvent())
						delete m_pRegistryWatchNotifier[dwCount].GetEvent();
					}
				delete [] m_pRegistryWatchNotifier;
				m_pRegistryWatchNotifier = NULL;
				}
			m_dwWatchesNumber = 0;
			m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
			bSuccess = TRUE;
			}
		}
	else
		m_hResultError = OX_REGISTRY_WATCHER_EMPTY_WATCHER;
	
	return bSuccess;
	}

BOOL COXRegistryWatcher::EnableWindowNotification(DWORD dwID, CWnd* pWnd, BOOL bPost)
	{
	BOOL	bSuccess = FALSE;
	DWORD	dwIndex = 0;
	
	if (GetWatchNotifier(dwID, &dwIndex))
		{
		if (m_EventWatchRestart.SetEvent())
			{
			CSingleLock BuildBeginLock(&m_EventWatchBuildBegin);
			BuildBeginLock.Lock();
			
			m_pRegistryWatchNotifier[dwIndex].SetWndDst(pWnd);
			if (pWnd)
				m_pRegistryWatchNotifier[dwIndex].SetPost(bPost);
			
			if (m_EventWatchBuildEnd.SetEvent())
				{
				m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
				bSuccess = TRUE;
				}
			else
				m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
			}
		else
			m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
		}
	
	return bSuccess;
	}

BOOL COXRegistryWatcher::DisableWindowNotification(DWORD dwID)
	{
	return EnableWindowNotification(dwID, NULL);
	}

BOOL COXRegistryWatcher::DisableAllWindowNotifications()
	{
	BOOL	bSuccess = FALSE;
	DWORD	dwCount = 0;
	
	if (0 < m_dwWatchesNumber)
		{
		if (m_EventWatchRestart.SetEvent())
			{
			CSingleLock BuildBeginLock(&m_EventWatchBuildBegin);
			BuildBeginLock.Lock();
			
			for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
				m_pRegistryWatchNotifier[dwCount].SetWndDst(NULL);
			
			if (m_EventWatchBuildEnd.SetEvent())
				{
				m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
				bSuccess = TRUE;
				}
			else
				m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
			}
		else
			m_hResultError = OX_REGISTRY_WATCHER_SYNCHRO_FAILURE;
		}
	else
		m_hResultError = OX_REGISTRY_WATCHER_EMPTY_WATCHER;
	
	return bSuccess;
	}

COXRegistryWatchNotifier* COXRegistryWatcher::GetWatchNotifier(DWORD dwID, DWORD* pdwQueueIndex)
	{
	DWORD						dwCount = 0;
	COXRegistryWatchNotifier*	pNotifier = NULL;
	
	if (0 < m_dwWatchesNumber)
		{
		m_hResultError = OX_REGISTRY_WATCHER_INCORRECT_ID;
		for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
			{
			if (m_pRegistryWatchNotifier[dwCount].GetWatchID() == dwID)
				{
				pNotifier = &m_pRegistryWatchNotifier[dwCount];
				if (pdwQueueIndex)
					*pdwQueueIndex = dwCount;
				m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
				break;
				}
			}
		}
	else
		m_hResultError = OX_REGISTRY_WATCHER_EMPTY_WATCHER;
	
	return pNotifier;
	}

HRESULT COXRegistryWatcher::GetLastError() const
	{
	return m_hResultError;
	}

// protected:

void COXRegistryWatcher::AfterNotify(COXRegistryWatchNotifier* pRegWatchNotifier)
	// --- In :			pRegWatchNotifier : Copy of the received parameters in the form of a
	//										pointer to COXRegistryWatchNotifier object.
	// --- Out :
	// --- Returns :
	// --- Effect :		This function is called AFTER a Registry change notification is received by
	//					specified Registry watch object. Function posts/sends notification message
	//					to a specified window (about message see EnableWindowNotification() declaration).
	{
	if (pRegWatchNotifier->GetWndDst())
		{
		if (pRegWatchNotifier->GetPost())
			::PostMessage(pRegWatchNotifier->GetWndDst()->m_hWnd, WM_OX_REGISTRY_NOTIFY,
			(WPARAM)pRegWatchNotifier->GetWatchID(), (LPARAM)pRegWatchNotifier->GetRegKey());
		else
			::SendMessage(pRegWatchNotifier->GetWndDst()->m_hWnd, WM_OX_REGISTRY_NOTIFY,
			(WPARAM)pRegWatchNotifier->GetWatchID(), (LPARAM)pRegWatchNotifier->GetRegKey());
		}
	}

UINT COXRegistryWatcher::RegistryWatchThreadFunction(LPVOID pParam)
	// --- In :			pParam :	Pointer to this COXRegistryWatcher object. It makes possible to
	//								work with non-static class members in this static function.
	// --- Out :
	// --- Returns :	Exit code = 0.
	// --- Effect :		Controlling function for watch worker thread.
{
	COXRegistryWatcher*	pWatcher = (COXRegistryWatcher*)pParam;
	DWORD				dwCount = 0;
	DWORD				dwSignaled = 0;
	CMultiLock*			pRegistryEventLock = NULL;
	CSyncObject**		ppObjects = NULL;
	BOOL				bLoop = TRUE;
	
	while (bLoop)
	{
		ppObjects = new CSyncObject*[pWatcher->m_dwWatchesNumber+2];
		ppObjects[0] = &pWatcher->m_EventWatchLoop;
		ppObjects[1] = &pWatcher->m_EventWatchRestart;
		for (dwCount = 2; dwCount < pWatcher->m_dwWatchesNumber+2; dwCount++)
			ppObjects[dwCount] = pWatcher->m_pRegistryWatchNotifier[dwCount-2].GetEvent();
		pRegistryEventLock = new CMultiLock(ppObjects, pWatcher->m_dwWatchesNumber+2);
		
		dwSignaled = pRegistryEventLock->Lock(INFINITE, FALSE) - WAIT_OBJECT_0;
		delete pRegistryEventLock;
		delete [] ppObjects;
		
		if (dwSignaled == 0)
			bLoop = FALSE;
		else if (dwSignaled == 1)
		{
			pWatcher->m_EventWatchBuildBegin.SetEvent();
			CSingleLock BuildEndLock(&pWatcher->m_EventWatchBuildEnd);
			BuildEndLock.Lock();
		}
		else if (dwSignaled > 1 && dwSignaled < pWatcher->m_dwWatchesNumber+2)
		{
			pWatcher->m_pRegistryWatchNotifier[dwSignaled-2].SetNotificationTime();
			LONG result=::RegNotifyChangeKeyValue(
				pWatcher->m_pRegistryWatchNotifier[dwSignaled-2].GetRegKey(),
				pWatcher->m_pRegistryWatchNotifier[dwSignaled-2].GetWatchSubtree(),
				pWatcher->m_pRegistryWatchNotifier[dwSignaled-2].GetWatchFilter(),
				(HANDLE)(*pWatcher->m_pRegistryWatchNotifier[dwSignaled-2].GetEvent()),
				TRUE);
			if(result==ERROR_SUCCESS)
			{
				if(!pWatcher->
					OnNotify(&pWatcher->m_pRegistryWatchNotifier[dwSignaled-2]))
				{
					pWatcher->
						AfterNotify(&pWatcher->m_pRegistryWatchNotifier[dwSignaled-2]);
				}
			}
			else
			{
				CString sMessage;
				::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,result,0,
					sMessage.GetBuffer(500*sizeof(TCHAR)),500,NULL);
				sMessage.ReleaseBuffer();
				TRACE(_T("COXRegistryWatcher::RegistryWatchThreadFunction: failed - %s"),sMessage);
			}
		}
	}
	
	return 0;
}

BOOL COXRegistryWatcher::StartWatchThread()
	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds, FALSE - if the function fails.
	//					To get extended error information, call GetLastError() method of this class.
	// --- Effect :		Starts "watch queue" worker thread.
	{
	BOOL	bSuccess = FALSE;
	
	if (IsWatchStarted())
		m_hResultError = OX_REGISTRY_WATCHER_ALREADY_STARTED;
	else
		{
		m_pNotificationThread = AfxBeginThread((AFX_THREADPROC)RegistryWatchThreadFunction, this);
		if (m_pNotificationThread)
			{
			m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
			bSuccess = TRUE;
			}
		else
			m_hResultError = OX_REGISTRY_WATCHER_THREAD_FAILURE;
		}
	
	return bSuccess;
	}

BOOL COXRegistryWatcher::StopWatchThread()
	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds, FALSE - if the function fails.
	//					To get extended error information, call GetLastError() method of this class.
	// --- Effect :		Stops "watch queue" worker thread.
	{
	BOOL bSuccess = FALSE;
	
	if (m_pNotificationThread)
		{
		if (::WaitForSingleObject(m_pNotificationThread->m_hThread, 0) == WAIT_OBJECT_0)
			m_hResultError = OX_REGISTRY_WATCHER_ALREADY_STOPPED;
		else
			{
			m_EventWatchLoop.SetEvent();
			::WaitForSingleObject(m_pNotificationThread->m_hThread, INFINITE);
			m_pNotificationThread = NULL;
			m_hResultError = OX_REGISTRY_WATCHER_ERROR_SUCCESS;
			bSuccess = TRUE;
			}
		}
	else
		m_hResultError = OX_REGISTRY_WATCHER_THREAD_FAILURE;
	
	return bSuccess;
	}

BOOL COXRegistryWatcher::IsWatchStarted()
	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if watch thread is started, FALSE - otherwise.
	// --- Effect :		Checks whether watch worker thread is started.
	{
	if (m_pNotificationThread &&
		::WaitForSingleObject(m_pNotificationThread->m_hThread, 0) == WAIT_TIMEOUT)
		return TRUE;
	else
		return FALSE;
	}

DWORD COXRegistryWatcher::FindNewID()
	// --- In :
	// --- Out :
	// --- Returns :	ID for a new notifier.
	// --- Effect :		Finds ID for a new notifier. When AddWatch() adds Registry key
	//					watch and creates a new notifier, ID of this notifier must be set
	//					to the unique value. FindNewID() returns such value for ID.
	{
	DWORD	dwCount = 0, dwBuf = 0, dwID = 0;
	
	for (dwCount = 0; dwCount < m_dwWatchesNumber; dwCount++)
		{
		dwBuf = m_pRegistryWatchNotifier[dwCount].GetWatchID();
		if (dwID < dwBuf)
			dwID = dwBuf;
		}
	
	return dwID + 1;
	}
