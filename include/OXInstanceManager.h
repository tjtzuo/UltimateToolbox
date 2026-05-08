// ==========================================================================
// 					Class Specification : COXInstanceManager
// ==========================================================================

// Header file : OXInstanceManager.h

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
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class organizes several instances of the same program.
//	A list is kept with all the instances IDs (which are actual process IDs)
//	This way you can check the number of running instances, send messages to
//	 their main window, close isntances etc.

//	A framework to exchange (share) data between these instances is also implemented
//	The size of this additional data must be supplied to the constructor of this class
//	The data can be access through : GetAdditionalDataSize() and GetAdditionalData()
//	You may use a supplied mutex to synchronize access to this data (GetAdditionalDataMutex())

//	To allow only one instance of your application :
//	- Create a global object of this class (supply a unique application name!)
//	- In CYourWinApp::InitInstance() add :
//		if (CheckMaxAllowedInstances())
//			return FALSE;


// Remark:
//	Shared instance data (through linker) is not used, because when the program is loaded
//	 twice, but from a different location, the both have their own (seperate)
//	 instance data. 
//	 See also MSDN "Problem with Shared Data Sections" (PSS ID Number: Q147136)
//	Using the text in the main window to distinguish between different applications
//	 is also not waterproof
//	This class uses a method bases on memory mapped files, which is the prefered way
//	 of sharing data between processes

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXINSTANCEMANAGER_H__
#define __OXINSTANCEMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"
#include "OXMainRes.h"

#include <afxmt.h>

#include "UTB64Bit.h"

#ifndef OX_MAX_NUM_INSTANCES_IN_LIST
// ... The maximum number of instances that can be tracked
//     Extra instances will be discarded
// ... The size of an entire CInstanceList object will rounded to the nearest
//	   allocation granularity
#define OX_MAX_NUM_INSTANCES_IN_LIST	1000
#endif




class OX_CLASS_DECL COXInstanceManager : public CObject
{
DECLARE_DYNAMIC(COXInstanceManager);

// Data members -------------------------------------------------------------
public:
	static const LPCTSTR m_pszInstanceListMutexPrefix;
	// --- The prefix of the name that will be used to create a mutex
	//     to synchronize access to the list of instances
	//	   The complete name adds the application name

	static const LPCTSTR m_pszAdditionalDataMutexPrefix;
	// --- The prefix of the name that will be used to create a mutex
	//     to synchronize access to the additional data
	//	   The complete name adds the application name

	static const LPCTSTR m_pszFileMapPrefix;
	// --- The prefix of the name that will be used to create a file
	//     mapping object
	//	   The complete name adds the application name

protected:
	CString m_sApplicationName;
	DWORD	m_nInstanceListSize;
	DWORD	m_nAdditionalDataSize;
	CMutex	m_instanceListMutex;
	CMutex	m_additionalDataMutex;
	HANDLE	m_hFileMap;
	LPVOID	m_pInstanceListView;
	LPVOID	m_pAdditionalDataView;

	struct CInstanceWindow
		{
		DWORD_PTR	m_nPID;
		HWND		m_hMainWnd;
		};

	struct CInstanceList
		{
		DWORD		m_nMaxAllowedInstances;
		DWORD		m_nCurrentNumInstances;
		DWORD		m_rgPID[OX_MAX_NUM_INSTANCES_IN_LIST];
		};

private:
	
// Member functions ---------------------------------------------------------
public:
	COXInstanceManager(LPCTSTR pszApplicationName, DWORD nMinAdditionalDataSize = 0);
	// --- In  : pszApplicationName : Name identifying this application
	//							   This must be the same for all instances of a program
	//							   and must not be empty
	//			 nAdditionalDataSize : The minimum number of additional bytes that must be shared
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	BOOL HasPreviousInstance();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether there are other instances (apart from this one) running
	// --- Effect : 

	DWORD GetNumberOfInstances();
	// --- In  : 
	// --- Out : 
	// --- Returns : The total number of running instances (this one included)
	// --- Effect : 

	DWORD GetMaxAllowedInstances();
	// --- In  : 
	// --- Out : 
	// --- Returns : The maximum allowable number of running instances (this one included)
	// --- Effect : 

	BOOL SetMaxAllowedInstances(DWORD nMaxAllowedInstances, BOOL bCloseExtra = TRUE);
	// --- In  : nMaxAllowedInstances : New maximum allowable number of running instances 
	//			 bCloseExtra : Whether to close extra instances when the max number is exceeded
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : When setting this number to less than the present number of
	//				running instances and bCloseExtra is TRUE, some instances will be closed

	BOOL GetInstanceCollection(CDWordArray& instanceColl);
	// --- In  : 
	// --- Out : instanceColl : A collection with all the instance IDs
	// --- Returns : Whether it succeeded or not
	// --- Effect : 

	BOOL CheckMaxAllowedInstances(BOOL bActivatePrevious = TRUE);
	// --- In  : bActivatePrevious : Whether to activate another instance when the 
	//								 max number has been exceeded
	// --- Out : 
	// --- Returns : Whether the maximum number of allowable instances has been exceeded
	//				 You should close this application when TRUE is returned.
	// --- Effect : 

	static HWND GetMainWindow(DWORD_PTR nPID);
	// --- In  : nPID : The instance (process) ID
	// --- Out : 
	// --- Returns : The main window of the specified instance when found
	//				 or NULL otherwise
	// --- Effect : The main window is a top-level, not-owned window of the specified instance

	static BOOL CloseInstance(DWORD_PTR nPID);
	// --- In  : nPID : The instance (process) ID
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : This function tries to terminate to specified instance by 
	//				closing its mlan window
	//				The function will fail if the main window is disabled

	static DWORD GetCurrentInstanceID();
	// --- In  : 
	// --- Out : 
	// --- Returns : The current instance (process) ID
	// --- Effect : 

	DWORD GetAdditionalDataSize() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The size in bytes that is avaliable for additioanl sharable data
	// --- Effect : This size may be larger than the size specified in the constructor
	//				because space is allocated according to the allocation granularity

	LPVOID GetAdditionalData();
	// --- In  : 
	// --- Out : 
	// --- Returns : A pointer to the additioanl data if successful, NULL otherwise
	// --- Effect : This pointer is valid until the next call to ReleaseAdditionalData()

	void ReleaseAdditionalData();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Releases the pointer to the additioanl data.
	//				A previously pointer returned from GetAdditionalData() is no longer valid
	//				This function will also be called when the object is destroyed

	CMutex& GetAdditionalDataMutex();
	// --- In  : 
	// --- Out : 
	// --- Returns : A mutex that can be used to synchronize access to the additional data 
	// --- Effect : 

#ifdef _DEBUG
	virtual void AssertValid() const;
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.
	
	virtual void Dump(CDumpContext& dc) const;
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 
#endif

	virtual ~COXInstanceManager();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of the object

protected:
	BOOL GetInstanceList();
	void ReleaseInstanceList();
	BOOL InitializeInstanceList();
	BOOL AddInstanceToList(DWORD nPID);
	BOOL RemoveInstanceFromList(DWORD nPID);
	static BOOL CALLBACK EnumMainWindows(HWND hWnd, LPARAM lParam);

private:
                   
};

#endif // __OXINSTANCEMANAGER_H__
// ==========================================================================
