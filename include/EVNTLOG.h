// ==========================================================================
// 					Class Specification : COXEventLog
// ==========================================================================

// Header file : evntlog.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// Creation Date : 	   27 November 1995
// Last Modification : 27 November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO 	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//   This class makes the reading and writing to the eventlog of
//   Windows NT very easy

// Remark:

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////

#ifndef __EVENT_LOG_H__
#define __EVENT_LOG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


enum EventType
	{
	 eventError       = EVENTLOG_ERROR_TYPE,
	 eventWarning     = EVENTLOG_WARNING_TYPE,
	 eventInformation = EVENTLOG_INFORMATION_TYPE,
	 eventSuccess     = EVENTLOG_AUDIT_SUCCESS,
	 eventFailure     = EVENTLOG_AUDIT_FAILURE
	};

class OX_CLASS_DECL COXEventLog : public CObject
{
DECLARE_DYNAMIC(COXEventLog)
// Data members -------------------------------------------------------------
public:

	CString m_sComputerName;
	CString m_sLogName;

protected:

	HANDLE m_LogHandle;
	HANDLE m_EventSourceHandle;

	DWORD m_ErrorCode;
	DWORD m_NumberOfBytesRead;
	DWORD m_NumberOfBytesInNextRecord;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXEventLog();
 	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXEventLog(LPCTSTR pszSourceName);
 	// --- In  : pszSourceName : Points to a null-terminated string that
 	// 			 	specifies the name of the source referenced by the returned
 	// 				handle. The source name must be a subkey of a logfile entry
 	// 				under the EventLog key in the registry. For example, the source
	//				name WinApp would be valid if the registry had the following form:
	//				HKEY_LOCAL_MACHINE
	//  			System
	//  			  CurrentControlSet
	//  			    Services
	//  			      EventLog
	//  			        Application
	//  			          WinApp
	//  			        Security
	//  			        System
	//				If the source name cannot be found, the event logging service uses
	// 				the Application logfile with no message files for the event
	// 				identifier or category. 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	virtual BOOL Backup(LPCTSTR pszNameOfBackupFile);
 	// --- In  :  pszNameOfBackupFile : Points to a null-terminated string that names
 	// 					the backup file. The backup filename may contain a server
 	// 					name to save the backup file on a remote server. 
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : saves the specified event log to a backup file. The function
	// 				does not clear the event log. 

	virtual BOOL Clear(LPCTSTR pszNameOfBackupFile);
 	// --- In  : pszNameOfBackupFile : Points to the null-terminated string specifying
 	// 					the name of a file in which a current copy of the event logfile
 	// 					will be placed. If this file already exists, the function fails.
 	//					The backup filename may contain a server name to save the
 	// 					backup file on a remote server. If the lpBackupFileName parameter
 	// 					is NULL, the current event logfile is not backed up. 
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : The ClearEventLog function clears the event log,
	// 				and if name_of_backup_file is not NULL, saves the current copy
	// 				of the logfile to a backup file. 

	virtual BOOL Close();
 	// --- In  :
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Closes the event log

	virtual BOOL CreateApplicationLog(LPCTSTR pszApplicationName,
	                                  LPCTSTR pszFileContainingMessageTableResource,
	                                  DWORD   dwSupportedTypes);
 	// --- In  : pszApplicationName : name of the subkey toi create in the registry
	//			 pszFileContainingMessageTableResource : value of the subkey
	//			 dwSupportedTypes : types supported
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Creates a log entry in the registry for an application with
	//				name application_name and value the name file_containing_message_table_resource

	virtual BOOL  DeleteApplicationLog(LPCTSTR pszApplicationName);
 	// --- In  : application_name : subkey to delete
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Removes a log entry out of the registry

	virtual BOOL  DeregisterSource();
 	// --- In  : 
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : closes registrysource registered by the RegisterSource function. 

	virtual DWORD GetErrorCode() const;
 	// --- In  :
	// --- Out : 
	// --- Returns : the errorcode
	// --- Effect : Returns the last error that occurred when executing the last command

	virtual BOOL GetNumberOfRecords(DWORD& dwNumberOfRecords);
 	// --- In  :
	// --- Out : dwNumberOfRecords : number of records
	// --- Returns : succeeded or not
	// --- Effect : retrieves the number of records in the current event log. 

	virtual BOOL GetNumberOfBytesInNextRecord(DWORD& dwNumberOfBytesInNextRecord);
 	// --- In  :
	// --- Out : dwNumberOfRecords : number of bytes in next record
	// --- Returns : succeeded or not
	// --- Effect : retrieves the number of bytes in the next record of the current event log. 

	virtual BOOL NotifyChange(HANDLE hEventHandle, HANDLE hLogHandle = NULL);
 	// --- In  : hEventHandle :	A handle to a Win32 event. This is the event that becomes
 	// 					signaled when an event is written to the event log file
 	//					specified by log_handle.
	//			 hLogHandle : Handle to an event log file
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : lets an application receive notification when an event is written
	// 				to the event log file specified by log_handle. When the event is
	// 				written to the event log file, the function causes the event object
	// 				specified by event_handle to become signaled.

	virtual BOOL OpenBackup(LPCTSTR pszNameOfBackupFile, LPCTSTR pszNameOfComputer = NULL);
 	// --- In  : pszNameOfBackupFile : Points to a null-terminated string that specifies
 	// 				the name of the backup file. The backup filename may contain a server
 	// 				name to open a backup file on a remote server (in this case, the 
 	//				lpszUNCServerName parameter must be NULL). 
	//			 pszNameOfComputer : Points to a null-terminated string that specifies
	// 				the Universal Naming Convention (UNC) name of the server on which
	// 				this operation is to be performed. If this parameter is NULL, the 
	//				operation is performed on the local computer.  
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : opens a backup event log.

	virtual BOOL  Open(LPCTSTR pszLogName, LPCTSTR pszNameOfComputer = NULL);
 	// --- In  : pszLogName : Points to a null-terminated string that specifies the name
 	// 				of the source that will be opened. The source
 	// 				name must be a subkey of a logfile entry under the EventLog key in
 	// 				the registry. For example, the source name WinApp would be valid if
 	// 				the registry had the following form:
	// 						HKEY_LOCAL_MACHINE
  	// 							System
    //								CurrentControlSet
    //								  Services
    //								    EventLog
    //								      Application
    //								        WinApp
    //								      Security
    //								      System
	//
	//				If the source name cannot be found, the event logging service uses
	// 				the Application logfile with no message files for the event identifier
	// 				or category. 
	//			 pszNameOfComputer : Points to a null-terminated string that specifies
	// 				the Universal Naming Convention (UNC) name of the server on which
	// 				this operation is to be performed. If this parameter is NULL, the 
	//				operation is performed on the local computer.  
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : opens an event log. 

	virtual BOOL Read(DWORD  dwRecordNumber, 
	                  LPVOID pBuffer, 
	                  DWORD& dwNumberOfBytesToRead, 
	                  DWORD  dwHowToRead = EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ);
 	// --- In  : dwRecordNumber : see ::ReadEventLog parameter dwRecordOffset
	//			 dwHowToRead : see ::ReadEventLog parameter dwReadFlags
	// --- Out : pBuffer : see ::ReadEventLog parameter lpBuffer
	//			 dwNumberOfBytesToRead : see ::ReadEventLog parameter nNumberOfBytesToRead  
	// --- Returns : succeeded or not
	// --- Effect : reads a whole number of entries from the specified event log. The 
	//				function can be used to read log entries in forward or reverse
	// 				chronological order.

	virtual BOOL RegisterSource(LPCTSTR pszSourceName, LPCTSTR pszNameOfComputer = NULL);
 	// --- In  : pszSourceName : Points to a null-terminated string that specifies the name
 	// 				of the source that will be opened. The source
 	// 				name must be a subkey of a logfile entry under the EventLog key in
 	// 				the registry. For example, the source name WinApp would be valid if
 	// 				the registry had the following form:
	// 						HKEY_LOCAL_MACHINE
  	// 							System
    //								CurrentControlSet
    //								  Services
    //								    EventLog
    //								      Application
    //								        WinApp
    //								      Security
    //								      System
	//
	//				If the source name cannot be found, the event logging service uses
	// 				the Application logfile with no message files for the event identifier
	// 				or category. 
	//			 pszNameOfComputer : Points to a null-terminated string that specifies
	// 				the Universal Naming Convention (UNC) name of the server on which
	// 				this operation is to be performed. If this parameter is NULL, the 
	//				operation is performed on the local computer.  
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : register source_name with name_of_computer at the eventlog

	virtual BOOL Report(EventType eEventType,
	                    DWORD     dwEventIdentifier,
	                    WORD      wCategory				   = 0,
	                    WORD      wNumberOfStrings         = 0,
	                    LPCTSTR*  pszStringArray           = NULL,
	                    DWORD     dwNumberOfRawDataBytes   = 0,
	                    LPVOID    pRawDataBuffer           = NULL,
	                    PSID      pUserSecurityIdentifier  = NULL);
 	// --- In  : see ::ReportEvent
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : writes an entry at the end of the current event log.

	virtual BOOL Report(LPCTSTR   pszLogName, 
	                    DWORD     dwMessageStringResourceID,
	                    WORD      wNumberOfStrings 		   = 0,
	                    LPCTSTR*  pszStringArray           = NULL);
 	// --- In  : see ::ReportEvent
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : opens the eventlog, writes an entry at the end of the current
	// 				event logand closes it again 

	virtual void ReportError(LPCTSTR pszStringToReport);
 	// --- In  : pszStringToReport : data to write
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : writes an error entry at the end of the current event log. 

	virtual void ReportInformation(LPCTSTR pszStringToReport);
 	// --- In  : pszStringToReport : data to write
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : writes an info entry at the end of the current event log.

#if defined(_DEBUG)
    virtual void Dump(CDumpContext& dump_context) const;
	virtual void AssertValid() const;
#endif // _DEBUG

	virtual ~COXEventLog();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
	void Initialize();
	
};

#endif // __EVENT_LOG_H__
