// ==========================================================================
//					Class Implementation : COXRegistryWatchNotifier
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
#include "OXRegistryWatchNotifier.h"

/////////////////////////////////////////////////////////////////////////////
// Data members -------------------------------------------------------------
// protected:
	//	BOOL m_bPost;
	//	--- If TRUE, notification message is posted to a window, if FALSE, message sent.

	//	BOOL m_bWatchSubtree;
	//	--- If TRUE, subtrees are being watched.

	//	DWORD m_dwNotifyFilter;
	//	--- Watch filter - combination of flags that control which changes should be reported.

	//	DWORD m_dwID;
	//	--- Watch notifier ID.

	//	HKEY m_hWatchKey;
	//	--- Original key specification that is being watch.

	//	HWND m_hWndDst;
	//	--- Window that receives the notification message.

	//	CEvent* m_phWatchEvent;
	//	--- Registry notification event.

	//	CTime m_tNotificationTime;
	//	--- Time when the notification was received.

/////////////////////////////////////////////////////////////////////////////
// Member functions ---------------------------------------------------------
// public:

COXRegistryWatchNotifier::COXRegistryWatchNotifier()
	:	
	m_bPost(TRUE),
	m_bWatchSubtree(FALSE),
	m_dwNotifyFilter(0),
	m_dwID(0),
	m_hWatchKey(NULL),
	m_hWndDst(NULL),
	m_phWatchEvent(NULL),
	m_tNotificationTime()
	{
	}

COXRegistryWatchNotifier& COXRegistryWatchNotifier::operator=(const COXRegistryWatchNotifier& RegistryWatchNotifier)
{
	if(this==&RegistryWatchNotifier)
		return *this;
		
	m_bPost =				RegistryWatchNotifier.m_bPost;
	m_bWatchSubtree =		RegistryWatchNotifier.m_bWatchSubtree;
	m_dwNotifyFilter =		RegistryWatchNotifier.m_dwNotifyFilter;
	m_dwID =				RegistryWatchNotifier.m_dwID;
	m_hWatchKey =			RegistryWatchNotifier.m_hWatchKey;
	m_hWndDst =				RegistryWatchNotifier.m_hWndDst;
	m_phWatchEvent =		RegistryWatchNotifier.m_phWatchEvent;
	m_tNotificationTime =	RegistryWatchNotifier.m_tNotificationTime;
	
	return *this;
}

// protected:
void COXRegistryWatchNotifier::SetMembers(HKEY hRegKey, BOOL bWatchSubtree, DWORD dwWatchFilter,
	CEvent* phWatchEvent, DWORD dwID)
	// --- In :			hRegKey :		Registry key to be watched.
	//					bWatchSubtree :	Flag that indicates whether to report changes in the specified key
	//									and all of its subkeys or only in the specified key. If this parameter
	//									is TRUE, the class reports changes in the key and its subkeys.
	//									If the parameter is FALSE, the class reports changes only in the key.
	//					dwWatchFilter :	Specifies a combination of flags that control which changes should be
	//									reported. Flags are described in the "Data members - public:" section
	//									of COXRegistryWatcher Class Specification.
	//					phWatchEvent :	Registry notification event.
	//					dwID :			Watch notifier ID.
	// --- Out :
	// --- Returns :
	// --- Effect :		Initializes some of notification parameters. This function is used in
	//					COXRegistryWatcher::AddWatch().
	{
	m_hWatchKey =		hRegKey;
	m_bWatchSubtree =	bWatchSubtree;
	m_dwNotifyFilter =	dwWatchFilter;
	m_phWatchEvent =	phWatchEvent;
	m_dwID =			dwID;
	m_hWndDst =			NULL;
	}

