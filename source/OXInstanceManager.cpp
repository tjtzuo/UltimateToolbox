// ==========================================================================
//				Class Implementation : COXInstanceManager
// ==========================================================================

// Source file : OXInstanceManager.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXInstanceManager.h"
#include "UTB64bit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXInstanceManager, CObject)

#define new DEBUG_NEW

static const TCHAR szInstanceListMutexPrefix[] = _T("COXInstanceManager_InstanceList_Mutex_");
static const TCHAR szAdditionalDataMutexPrefix[] = _T("COXInstanceManager_Additional_Mutex_");
static const TCHAR szFileMapPrefix[] = _T("COXInstanceManager_FileMap_");

/////////////////////////////////////////////////////////////////////////////
// Definition of static members 
const LPCTSTR COXInstanceManager::m_pszInstanceListMutexPrefix = szInstanceListMutexPrefix;
const LPCTSTR COXInstanceManager::m_pszAdditionalDataMutexPrefix = szAdditionalDataMutexPrefix;
const LPCTSTR COXInstanceManager::m_pszFileMapPrefix = szFileMapPrefix;

// Data members -------------------------------------------------------------
// protected:
// CString m_sApplicationName;
// --- A unique name identifying all the instances of an application

// DWORD m_nInstanceListSize;
// --- The complete size used for the CInstanceList object.
//     This number is rounded to the allocation granularity

// DWORD m_nAdditionalDataSize;
// --- The complete size used for the additional data
//     This number is rounded to the allocation granularity

// CMutex m_instanceListMutex;
// --- The mutexused to synchronize access to the list of instances

// CMutex m_additionalDataMutex;
// --- The mutex used to synchronize access to the additional data

// HANDLE m_hFileMap;
// --- The file mapping object

// LPVOID m_pInstanceListView;
// --- The view on the file mapping used to access the shared instance list

// LPVOID m_pAdditionalDataView;
// --- The view on the file mapping used to access the additional data

// private:

// Member functions ---------------------------------------------------------
// public:

// ... To make sure inly one object  is created per instance
static BOOL bInstanceManagerCreated = FALSE;

COXInstanceManager::COXInstanceManager(LPCTSTR pszApplicationName, DWORD nMinAdditionalDataSize /* = 0 */)
:
m_sApplicationName(pszApplicationName),
m_nAdditionalDataSize(nMinAdditionalDataSize),
m_instanceListMutex(FALSE, m_pszInstanceListMutexPrefix + m_sApplicationName),
m_additionalDataMutex(FALSE, m_pszAdditionalDataMutexPrefix + m_sApplicationName),
m_hFileMap(NULL),
m_pInstanceListView(NULL),
m_pAdditionalDataView(NULL)
{
	// ... You should supply a unique name for the application
	ASSERT((pszApplicationName != NULL) && (*pszApplicationName != _T('\0')));
#ifdef _DEBUG
	// ... Only one object of the COXInstanceManager class may be created per instance
	ASSERT(bInstanceManagerCreated == FALSE);
	bInstanceManagerCreated = TRUE;
#endif // _DEBUG
	ASSERT_VALID(this);

	// Compute the size of the instance list 
	m_nInstanceListSize  = sizeof(CInstanceList);

	// ... Round the size to a multiple of the page size
	SYSTEM_INFO systemInfo;
	::ZeroMemory(&systemInfo, sizeof(systemInfo));
	::GetSystemInfo(&systemInfo);

	if (m_nInstanceListSize % systemInfo.dwAllocationGranularity != 0)
		m_nInstanceListSize = m_nInstanceListSize / systemInfo.dwAllocationGranularity + 
		systemInfo.dwAllocationGranularity;
	else
		m_nInstanceListSize = m_nInstanceListSize / systemInfo.dwAllocationGranularity;

	if (m_nAdditionalDataSize % systemInfo.dwAllocationGranularity != 0)
		m_nAdditionalDataSize = m_nAdditionalDataSize / systemInfo.dwAllocationGranularity + 
		systemInfo.dwAllocationGranularity;
	else
		m_nAdditionalDataSize = m_nAdditionalDataSize / systemInfo.dwAllocationGranularity;

	// Create file map in memory
	DWORD nTotalSize = m_nInstanceListSize + m_nAdditionalDataSize;
	// ... Total size must also be a multiple of the page size
	ASSERT(nTotalSize % systemInfo.dwAllocationGranularity == 0);
	CString sFileMapName = m_pszFileMapPrefix + m_sApplicationName;
	m_hFileMap = ::CreateFileMapping((HANDLE)(INT_PTR)0xFFFFFFFF, NULL, 
		PAGE_READWRITE | SEC_COMMIT, 0, nTotalSize, sFileMapName);
	BOOL bAlreadyExists = (::GetLastError() == ERROR_ALREADY_EXISTS);

#ifdef _DEBUG
	if (m_hFileMap == NULL)
		TRACE1("COXInstanceManager::COXInstanceManager : No file map could be created (error %i)\n",
		::GetLastError());
#endif // _DEBUG

	if (!bAlreadyExists)
		InitializeInstanceList();

	// Add this instance to the list
	AddInstanceToList(GetCurrentInstanceID());
}

BOOL COXInstanceManager::HasPreviousInstance()
{
	return (1 < GetNumberOfInstances());
}

DWORD COXInstanceManager::GetNumberOfInstances()
{
	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Get the total number of instances in the list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	DWORD nCurrentNumInstances = pInstanceList->m_nCurrentNumInstances;

#ifdef _DEBUG
	if (pInstanceList->m_nCurrentNumInstances == OX_MAX_NUM_INSTANCES_IN_LIST)
		TRACE1("COXInstanceManager::GetNumberOfInstances : Max number reached (%i), extra may be ignored\n",
		OX_MAX_NUM_INSTANCES_IN_LIST);
#endif

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return nCurrentNumInstances;
}

DWORD COXInstanceManager::GetMaxAllowedInstances()
{
	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return 0;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Get the total number of instances in the list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	DWORD nMaxAllowedInstances = pInstanceList->m_nMaxAllowedInstances;

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return nMaxAllowedInstances;
}

BOOL COXInstanceManager::SetMaxAllowedInstances(DWORD nMaxAllowedInstances, 
												BOOL bCloseExtra /* = TRUE */)
{
	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

#ifdef _DEBUG
	if (OX_MAX_NUM_INSTANCES_IN_LIST < nMaxAllowedInstances)
		TRACE2("COXInstanceManager::SetMaxAllowedInstances : Trying to set max allowable number (%i)higher than process list size (%i), extra may be ignored\n",
		nMaxAllowedInstances, OX_MAX_NUM_INSTANCES_IN_LIST);
#endif

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Get the total number of instances in the list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	pInstanceList->m_nMaxAllowedInstances = nMaxAllowedInstances;

	// Get a copy off the data before we release the instancelist
	CDWordArray instanceColl;
	DWORD nCurrentNumInstances = pInstanceList->m_nCurrentNumInstances;
	instanceColl.SetSize(pInstanceList->m_nCurrentNumInstances);
	::CopyMemory(instanceColl.GetData(), pInstanceList->m_rgPID, pInstanceList->m_nCurrentNumInstances * sizeof(DWORD));

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();
	listLock.Unlock();

	// If their are currently more instances running than allowed, we will close some
	int nInstancesToKill = nCurrentNumInstances - nMaxAllowedInstances;
	if (bCloseExtra && (0 < nInstancesToKill))
	{
		// ... Iterate the collection backwards (because we are deleting items)
		DWORD nThisPID = GetCurrentInstanceID();
		DWORD nInstanceID = 0;
		int nInstanceIndex = PtrToInt(instanceColl.GetSize() - 1);
		while ((0 <= nInstanceIndex) && (0 < nInstancesToKill))
		{
			nInstanceID = instanceColl.GetAt(nInstanceIndex);
			// ... Kill the instance if it is not this instance
			if ((nInstanceID != nThisPID) && (CloseInstance(nInstanceID)) )
				nInstancesToKill--;
			nInstanceIndex--;
		}

		if (0 < nInstancesToKill)
		{
			// ... We we still have not killed enough instances, try to kill this instance
			if (CloseInstance(nThisPID))
				nInstancesToKill--;
		}

#ifdef _DEBUG
		if (0 < nInstancesToKill)
			TRACE0("COXInstanceManager::SetMaxAllowedInstances : Not enough instnaces could be closed\n");
#endif
	}

	return TRUE;
}

BOOL COXInstanceManager::GetInstanceCollection(CDWordArray& instanceColl)
{
	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Get the total number of instances in the list
	// ... First DWORD in list is the number of items, followed by the PIDs
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);

	// Fill the result collection
	instanceColl.SetSize(pInstanceList->m_nCurrentNumInstances);
	::CopyMemory(instanceColl.GetData(), pInstanceList->m_rgPID, pInstanceList->m_nCurrentNumInstances * sizeof(DWORD));

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return TRUE;
}

BOOL COXInstanceManager::CheckMaxAllowedInstances(BOOL bActivatePrevious /* = TRUE */)
{
	BOOL bMaxExceeded = FALSE;

	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Get the total number of instances in the list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	bMaxExceeded = (pInstanceList->m_nMaxAllowedInstances < pInstanceList->m_nCurrentNumInstances);

	if (bMaxExceeded && bActivatePrevious)
	{
		// Activate a previous instance
		DWORD nThisPID = GetCurrentInstanceID();
		DWORD nPID = 0;
		if (0 < pInstanceList->m_nCurrentNumInstances)
			nPID = pInstanceList->m_rgPID[0];
		if (nPID == nThisPID)
		{
			nPID = 0;
			if (1 < pInstanceList->m_nCurrentNumInstances)
				nPID = pInstanceList->m_rgPID[1];
			ASSERT(nPID != nThisPID);
		}

		if (nPID != 0)
		{
			// Another instance was found : activate it
			HWND hTopLevelWnd = GetMainWindow(nPID);
			if (hTopLevelWnd != NULL)
			{
				::SetForegroundWindow(hTopLevelWnd);
				if(CWnd::FromHandle(hTopLevelWnd)->GetStyle() & WS_MINIMIZE)
				{
					::ShowWindow(hTopLevelWnd,SW_RESTORE);
				}
			}
		}
	}

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return bMaxExceeded;
}

HWND COXInstanceManager::GetMainWindow(DWORD_PTR nPID)
{
	// First find a top level window of the specified instance
	CInstanceWindow instanceWindow;
	instanceWindow.m_nPID = nPID;
	instanceWindow.m_hMainWnd = NULL;

	::EnumWindows(&EnumMainWindows, (LPARAM)&instanceWindow);	

#ifdef _DEBUG
	if (instanceWindow.m_hMainWnd == NULL)
		TRACE1("COXInstanceManager::GetMainWindow : No main window found for instance %i\n",	nPID);
#endif // _DEBUG

	return instanceWindow.m_hMainWnd;
}

BOOL COXInstanceManager::CloseInstance(DWORD_PTR nPID)
{
	// ... Assume failure
	BOOL bSuccess = FALSE;

	HWND hMainWnd = GetMainWindow(nPID);
	if (hMainWnd != NULL)
	{
		// ... Only try to close enabled windows
		//     A window will be disabled e.g. when it owns an open modal dialog
		//	   Closing the main window at this point may be dangerous
		//	   For more info see MSDN : 
		//		* Terminating Windows-Based Application from Another App (PSS ID Number: Q92528)
		//		* Dialog Box Default Message Instanceing : WM_CLOSE
		if (::IsWindowEnabled(hMainWnd))
		{
			::PostMessage(hMainWnd, WM_CLOSE, 0, 0);
			bSuccess = TRUE;
		}
	}

#ifdef _DEBUG
	if (::IsWindow(hMainWnd) && !::IsWindowEnabled(hMainWnd)) 
		TRACE1("COXInstanceManager::CloseInstance : Main window of instance %i is disabled, failing\n", nPID);
#endif // _DEBUG

	return bSuccess;
}

DWORD COXInstanceManager::GetCurrentInstanceID()
{
	return ::GetCurrentProcessId();
}

DWORD COXInstanceManager::GetAdditionalDataSize() const
{
	return m_nAdditionalDataSize;
}

LPVOID COXInstanceManager::GetAdditionalData()
{
	if (m_pAdditionalDataView != NULL)
		return m_pAdditionalDataView;

	if (m_hFileMap == NULL)
	{
		TRACE0("COXInstanceManager::GetAdditionalData : No file mapping could be created, returning NULL\n");
		return NULL;
	}

	// Create new view of file mapping (additional data is located after the instance list)
	m_pAdditionalDataView = ::MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 
		0, m_nInstanceListSize, m_nAdditionalDataSize);

#ifdef _DEBUG
	if (m_pAdditionalDataView != NULL)
		ASSERT(AfxIsValidAddress(m_pAdditionalDataView, m_nAdditionalDataSize));
	else
		TRACE1("COXInstanceManager::GetAdditionalData : No view of file map could be created (error %i), returning NULL\n",
		::GetLastError());
#endif // _DEBUG

	return m_pAdditionalDataView;
}

void COXInstanceManager::ReleaseAdditionalData()
{
	if (m_pAdditionalDataView != NULL)
	{
		VERIFY(::UnmapViewOfFile(m_pAdditionalDataView));
		m_pAdditionalDataView = NULL;
	}
}

CMutex& COXInstanceManager::GetAdditionalDataMutex()
{
	return m_additionalDataMutex;
}

#ifdef _DEBUG
void COXInstanceManager::AssertValid() const
{
	CObject::AssertValid();
}

void COXInstanceManager::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

COXInstanceManager::~COXInstanceManager()
{
	// Remove this instance from the list
	RemoveInstanceFromList(GetCurrentInstanceID());

	// ... View of instance list data should have been closed after use
	ASSERT(m_pInstanceListView == NULL);

	// Close view of additional data if not done yet
	ReleaseAdditionalData();

	// Close the file map
	if (m_hFileMap != NULL)
	{
		VERIFY(::CloseHandle(m_hFileMap));
		m_hFileMap = NULL;
	}

	bInstanceManagerCreated = FALSE;
}

// protected:
BOOL COXInstanceManager::GetInstanceList()
// --- In  : 
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : This functions gets a pointer to the instance list and stores it 
//				in the data member m_pInstanceListView
//				The pointer is valid until the next call to ReleaseInstanceList()
{
	// ... Should have been unmapped after its last use
	ASSERT(m_pInstanceListView == NULL);

	if (m_hFileMap == NULL)
	{
		TRACE0("COXInstanceManager::GetInstanceListData : No file mapping could be created, returning NULL\n");
		return NULL;
	}

	// Create new view of file mapping (additional data is located after the instance list)
	m_pInstanceListView = ::MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 
		0, 0, m_nInstanceListSize);

#ifdef _DEBUG
	if (m_pInstanceListView != NULL)
		ASSERT(AfxIsValidAddress(m_pInstanceListView, m_nInstanceListSize));
	else
		TRACE1("COXInstanceManager::GetInstanceListData : No view of file map could be created (error %i), returning NULL\n",
		::GetLastError());
#endif // _DEBUG

	return (m_pInstanceListView != NULL);
}

void COXInstanceManager::ReleaseInstanceList()
// --- In  : 
// --- Out : 
// --- Returns : 
// --- Effect : Releases the pointer to the instance list
//				The value of the data member m_pInstanceListView is no longer valid
{
	if (m_pInstanceListView != NULL)
	{
		VERIFY(::UnmapViewOfFile(m_pInstanceListView));
		m_pInstanceListView = NULL;
	}
}

BOOL COXInstanceManager::InitializeInstanceList()
// --- In  : 
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : Initializes the shared data of the instance list
{
	BOOL bSuccess = FALSE;

	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Initailze the instance list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	// ... Default max allowed instances to 1
	pInstanceList->m_nMaxAllowedInstances = 1;
	pInstanceList->m_nCurrentNumInstances = 0;
	::ZeroMemory(pInstanceList->m_rgPID, sizeof(pInstanceList->m_rgPID));

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return bSuccess;
}

BOOL COXInstanceManager::AddInstanceToList(DWORD nPID)
// --- In  : nPID : The instance (process) ID
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : Adds the specified instance ID to the list
{
	// ... Assume failure
	BOOL bSuccess = FALSE;

	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Add PID to the end of the list
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	// ... Only add when there is space left
	if (pInstanceList->m_nCurrentNumInstances < OX_MAX_NUM_INSTANCES_IN_LIST)
	{
		for(int nIndex=0; nIndex<(int)pInstanceList->m_nCurrentNumInstances; nIndex++)
		{
			::PostMessage(GetMainWindow(pInstanceList->m_rgPID[nIndex]),
				WM_OX_INSTANCE_CREATED,(WPARAM)NULL,(LPARAM)nPID);
		}
		pInstanceList->m_rgPID[pInstanceList->m_nCurrentNumInstances] = nPID;
		pInstanceList->m_nCurrentNumInstances++;
		bSuccess = TRUE;
	}
	else
	{
		TRACE1("COXInstanceManager::AddInstanceToList : Max number of trackable instances (%i) reached, extra ignored\n",
			OX_MAX_NUM_INSTANCES_IN_LIST);
	}
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return bSuccess;
}

BOOL COXInstanceManager::RemoveInstanceFromList(DWORD nPID)
// --- In  : nPID : The instance (process) ID
// --- Out : 
// --- Returns : Whether it succeeded or not
// --- Effect : Removes the specified instance ID from the list
{
	BOOL bSuccess = FALSE;

	// Get a pointer to the list of instances
	if (!GetInstanceList())
		return FALSE;

	// ... Lock the list
	CSingleLock listLock(&m_instanceListMutex, TRUE);

	// Search for the specified instance ID
	CInstanceList* pInstanceList = (CInstanceList*)m_pInstanceListView;
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);
	DWORD nInstanceIndex = 0;
	while ((nInstanceIndex < pInstanceList->m_nCurrentNumInstances) && 
		(pInstanceList->m_rgPID[nInstanceIndex] != nPID))
	{
		nInstanceIndex++;
	}

	if (nInstanceIndex < pInstanceList->m_nCurrentNumInstances)
	{
		DWORD dwRemovedPID=pInstanceList->m_rgPID[nInstanceIndex];
		// Move all the following ID one position to the beginning and set 
		// the last one to zero
		DWORD* pFoundInstance = &pInstanceList->m_rgPID[nInstanceIndex];
		// ... Move 1 to the top
		::MoveMemory(pFoundInstance, pFoundInstance+1, 
			(pInstanceList->m_nCurrentNumInstances-nInstanceIndex-1)*sizeof(DWORD));
		// .. Add a new 0
		pInstanceList->m_rgPID[pInstanceList->m_nCurrentNumInstances]=0;
		ASSERT(1 <= pInstanceList->m_nCurrentNumInstances);
		// ...Decrement the current number
		pInstanceList->m_nCurrentNumInstances--;
		bSuccess = TRUE;

		for(int nIndex=0; nIndex<(int)pInstanceList->m_nCurrentNumInstances; nIndex++)
		{
			::PostMessage(GetMainWindow(pInstanceList->m_rgPID[nIndex]),
				WM_OX_INSTANCE_DESTROYED,(WPARAM)NULL,(LPARAM)dwRemovedPID);
		}
	}
	else
		TRACE1("COXInstanceManager::RemoveInstanceFromList : Instances (%i) not found, ignored\n", nPID);
	ASSERT(pInstanceList->m_nCurrentNumInstances <= OX_MAX_NUM_INSTANCES_IN_LIST);

	// Immediately unmap the view to protect against dangerous pointers
	ReleaseInstanceList();

	return bSuccess;
}

BOOL CALLBACK COXInstanceManager::EnumMainWindows(HWND hWnd, LPARAM lParam)
// --- In  : hWnd : A top-level window
//			 lParam : Additioanl data (pointer to a CInstanceWindow object)
// --- Out : 
// --- Returns : Whether to continue iterating
// --- Effect : This function is called for every top-level window
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	CInstanceWindow* pInstanceWindow = (CInstanceWindow*)lParam;
	ASSERT(AfxIsValidAddress(pInstanceWindow, sizeof(CInstanceWindow)));

	// We will search for a toplevel window of the specified instance
	// Thiw window should be not-owned (to exclude dialogs) 
	DWORD nWindowPID = 0;
	::GetWindowThreadProcessId(hWnd, &nWindowPID);
	if ((nWindowPID==pInstanceWindow->m_nPID) && 
		(::GetWindow(hWnd,GW_OWNER)==NULL) &&
		(::GetWindowLongPtr(hWnd,GWL_EXSTYLE) & WS_EX_TOOLWINDOW)==0 &&
		(::GetWindowLongPtr(hWnd,GWL_STYLE) & WS_VISIBLE)==WS_VISIBLE)
	{
		pInstanceWindow->m_hMainWnd = hWnd;
		// ... Found, stop looking 
		return FALSE;
	}
	// ... Not found, keep on looking
	return TRUE;
}


// private:

// ==========================================================================
