// ==========================================================================
//				Class Implementation : COXUNCStandardActor
// ==========================================================================

// Version: 9.3

// Source file : OXUNCStandardActor.cpp

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXUNCStandardActor.h"
#include "OXMainRes.h"

#pragma warning(disable : 4201)

#include <IO.h>			// for _access
#include <WinIOCtl.h>	// for FSCTL_GET_COMPRESSION and FSCTL_SET_COMPRESSION

#include "UTBStrOp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXUNCStandardActor, CObject)

#define new DEBUG_NEW

#ifdef _DEBUG
// Trace a message when the RESULT specifies failure
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)	\
	{ if (FAILED(RESULT)) {						\
		TRACE(_T("%s : Failed (%u == 0x%X, Code : %u) :\n\t%s\n"),					\
		_T(TEXT), RESULT, RESULT, HRESULT_CODE(RESULT), GetResultMessage(RESULT));	\
	} }
#else
// Do not trace in Release build
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)
#endif // _DEBUG

// Assign the LastError of Win32 as HRESULT code to RESULT
#define GET_LAST_HRESULT(RESULT) RESULT = (RESULT = ::GetLastError(), HRESULT_FROM_WIN32(RESULT))

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
COXUNCStandardActor COXUNCStandardActor::m_theOneAndOnly;

// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXUNCStandardActor::COXUNCStandardActor()
	{
	ASSERT_VALID(this);
	}

HRESULT COXUNCStandardActor::MakeAbsolute(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

    LPCTSTR pszPathName = UNC.m_sUNC;
	if (*pszPathName == COXUNC::m_cNull)
		// ... Empty path spec stays empty
		return hResult;

	CString sAbsolutePath;
    DWORD nMaxAbsolutePathLength = 0;
    DWORD nPathLength = 0;
    LPTSTR pszAbsolutePath = NULL;
    LPTSTR pszFilePart = NULL;

	// ... First get the necessary length for the result string
	nMaxAbsolutePathLength = ::GetFullPathName(pszPathName, nMaxAbsolutePathLength, pszAbsolutePath, &pszFilePart);
	if (nMaxAbsolutePathLength != 0)
		{
		nMaxAbsolutePathLength++;
		// ... The alloc a buffer and get the absolute path
		pszAbsolutePath = sAbsolutePath.GetBuffer(nMaxAbsolutePathLength);
		nPathLength = ::GetFullPathName(pszPathName, nMaxAbsolutePathLength, pszAbsolutePath, &pszFilePart);
		// ... Check for failure
		if ((nPathLength == 0) || (nMaxAbsolutePathLength < nPathLength))
			GET_LAST_HRESULT(hResult);
		sAbsolutePath.ReleaseBuffer();
		}
	else
		{
		// ... First call to GetFullPathName already failed
		GET_LAST_HRESULT(hResult);
		}

	if (SUCCEEDED(hResult))
		{
		// ... We should have created a buffer that is large enough
		ASSERT(nPathLength <= nMaxAbsolutePathLength);
		UNC = sAbsolutePath;
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::MakeAbsolute", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::Exists(COXUNC UNC, BOOL& bExists)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	bExists = FALSE;

	if (UNC.File().IsEmpty())
		{
		// Search for device or directory
		int nAccess = _taccess(UNC, 0);
		bExists = (nAccess == 0);
		if (!bExists)
			hResult = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
	else
		{
		// Search for file
		WIN32_FIND_DATA findFileData;
		HANDLE hFind = ::FindFirstFile(UNC, &findFileData);
		bExists = (hFind != INVALID_HANDLE_VALUE);
		if (!bExists)
			GET_LAST_HRESULT(hResult);
		else
			{
			VERIFY(::FindClose(hFind));
			// Check that we have not found a directory, because we are searching for a file
			if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
				// ... Searching for file but found dir
				bExists = FALSE;
				hResult = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
				}
			}
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::Exists", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::Create(COXUNC UNC)
	{
	// ... We will change the parameter UNC in this function

	HRESULT hResult = ERROR_SUCCESS;

	// First create all the directories specified (some may not yet exist)
	CString sOriginalDir = UNC.Directory();
	CString sOriginalFile = UNC.File();
	LPCTSTR pszDirBegin = sOriginalDir;
	LPCTSTR pszDirEnd = NULL;

	// ... Get a pointer to the end of the first subdirectory
	if (*pszDirBegin != COXUNC::m_cNull)
		pszDirEnd = _tcspbrk(pszDirBegin + 1, COXUNC::m_pszSlashes);
	else
		pszDirEnd = NULL;

	// ... Remove the file part from the URL
	UNC.File().Empty();

	// Iterate all the subdirectories and create them one by one
	CString sDir;
	int nDirLength = 0;
	while (pszDirEnd != NULL)
		{
		nDirLength = (int)(pszDirEnd - pszDirBegin);
		UTBStr::tcsncpy(sDir.GetBufferSetLength(nDirLength), nDirLength, pszDirBegin, nDirLength);
		UNC.Directory() = sDir;
		if (!::CreateDirectory(UNC, NULL))
			{
			GET_LAST_HRESULT(hResult);
			if (hResult == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS))
				// ... Dirctory already exists : OK, continue
				hResult = ERROR_SUCCESS;
			else
				// ... Failed to create the directory
				break;
			}
		
		// ... Get pointer to the end of the next subdirectory
		pszDirEnd = _tcspbrk(pszDirEnd + 1, COXUNC::m_pszSlashes);
		}
	UNC.Directory() = sOriginalDir;

	// Then create the file (if one was specified)
	if (!sOriginalFile.IsEmpty())
		{
		UNC.File() = sOriginalFile;
		HANDLE hFile = ::CreateFile(UNC, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			VERIFY(::CloseHandle(hFile));
		else
			GET_LAST_HRESULT(hResult);
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::Create", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::CreateTemporaryFile(COXUNC tempDir, LPCTSTR pszPrefix, COXUNC& file)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	file.Empty();

	CString sPrefix(pszPrefix);
	CString sTempFile;

	if (!::GetTempFileName(tempDir.FileForm(), sPrefix, 0, sTempFile.GetBuffer(_MAX_PATH)))
		GET_LAST_HRESULT(hResult);
	sTempFile.ReleaseBuffer();

	if (SUCCEEDED(hResult))
		file = sTempFile;

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::CreateTemporaryFile", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::Copy(COXUNC sourceUNC, COXUNC destinationUNC, BOOL bReplaceExisting)
	{
	HRESULT hResult = ERROR_SUCCESS;

	if (!destinationUNC.IsEmpty())
		{
		// ... Use the parts of the source that are not filled out in the destination
		if (destinationUNC.Server().IsEmpty() && destinationUNC.Share().IsEmpty())
			{
			destinationUNC.Server() = sourceUNC.Server();
			destinationUNC.Share() = sourceUNC.Share();
			}
		if (destinationUNC.Directory().IsEmpty())
			destinationUNC.Directory() = sourceUNC.Directory();
		if (destinationUNC.File().IsEmpty())
			destinationUNC.File() = sourceUNC.File();
		}

	if (!::CopyFile(sourceUNC.FileForm(), destinationUNC.FileForm(), !bReplaceExisting))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::Copy", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::MoveRename(COXUNC sourceUNC, COXUNC destinationUNC, DWORD dwFlags)
	{
	// ... We will change the parameters sourceUNC and destinationUNC in this function

	HRESULT hResult = ERROR_SUCCESS;

	if (!destinationUNC.IsEmpty())
		{
		// ... Use the parts of the source that are not filled out in the destination
		if (destinationUNC.Server().IsEmpty() && destinationUNC.Share().IsEmpty())
			{
			destinationUNC.Server() = sourceUNC.Server();
			destinationUNC.Share() = sourceUNC.Share();
			}
		if (destinationUNC.Directory().IsEmpty())
			destinationUNC.Directory() = sourceUNC.Directory();
		if (destinationUNC.File().IsEmpty())
			destinationUNC.File() = sourceUNC.File();
		}

	// ... When destination is NULL, we are deleting the file
	CString sFileForm = destinationUNC.FileForm();
	LPCTSTR pszDestUNC = sFileForm;
	if (sFileForm.GetLength() == 0)
		pszDestUNC = NULL;

	if (!::MoveFileEx(sourceUNC.FileForm(), pszDestUNC, dwFlags))
		GET_LAST_HRESULT(hResult);

	if (hResult == HRESULT_FROM_WIN32(ERROR_CALL_NOT_IMPLEMENTED))
		{
		// Function call failed because it is not supported on this platform
		// (probably Win95)

		// We will try to emulate this function through other function calls

		// Comapre absolute paths of source and destination
		sourceUNC.MakeAbsolute();
		destinationUNC.MakeAbsolute();
		// ... If source and destination are equal we do not have to copy it
		if (sourceUNC.StandardForm() == destinationUNC.StandardForm())
			hResult = ERROR_SUCCESS;
		else
			{
			switch(dwFlags)
				{
				case 0:
					// Use plain ::MoveFile()
					hResult = ERROR_SUCCESS;
					if (!::MoveFile(sourceUNC.FileForm(), pszDestUNC))
						GET_LAST_HRESULT(hResult);
					break;
				case MOVEFILE_COPY_ALLOWED:
					// COPY the file and delete the original (do not allow overwrite)
					hResult = ERROR_SUCCESS;
					if (!::CopyFile(sourceUNC.FileForm(), pszDestUNC, TRUE))
						GET_LAST_HRESULT(hResult);
					if (SUCCEEDED(hResult) && !::DeleteFile(sourceUNC.FileForm()))
						GET_LAST_HRESULT(hResult);
					break;
				case MOVEFILE_REPLACE_EXISTING:
					// We cannot disallow moves accross disks, so specifying MOVEFILE_REPLACE_EXISTING
					// also implies MOVEFILE_COPY_ALLOWED, even when not specified.
					// ... Fall through
				case MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED:
					// First delete destination and then move the file
					// ... Delete may fail if it does not exist, so we discard the result
					hResult = ERROR_SUCCESS;
					if (pszDestUNC != NULL)
						::DeleteFile(pszDestUNC);
					if (!::MoveFile(sourceUNC.FileForm(), pszDestUNC))
						GET_LAST_HRESULT(hResult);
					break;
				default:
					// Other combinations (e.g. MOVEFILE_DELAY_UNTIL_REBOOT or 
					// MOVEFILE_WRITE_THROUGH) cannot be emulated and will fail)
					ASSERT(FAILED(hResult));
					break;
				}
			}
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::MoveRename", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::Delete(COXUNC UNC, BOOL bRemoveReadOnly)
	{
	HRESULT hResult = ERROR_SUCCESS;

	if (!UNC.File().IsEmpty())
		{
		// Delete file
		if (!::DeleteFile(UNC))
			{
			GET_LAST_HRESULT(hResult);
			if ((hResult == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED)) && bRemoveReadOnly)
				{
				// File may be read/only : Remove this attribute
				if (::SetFileAttributes(UNC, FILE_ATTRIBUTE_NORMAL) && ::DeleteFile(UNC))
					hResult = ERROR_SUCCESS;
				else
					GET_LAST_HRESULT(hResult);
				}
			}
		}
	else
		{
		// Delete directory
		if (!::RemoveDirectory(UNC))
			GET_LAST_HRESULT(hResult);
		}


	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::Delete", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetCurrentDirectory(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	UNC.Empty();

	CString sDir;
	int nMaxDirLength = 0;
	nMaxDirLength = ::GetCurrentDirectory(nMaxDirLength, NULL);
	if (nMaxDirLength != 0)
		{
		nMaxDirLength = ::GetCurrentDirectory(nMaxDirLength, sDir.GetBuffer(nMaxDirLength));
		if (nMaxDirLength == 0)
			GET_LAST_HRESULT(hResult);
		sDir.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		{
		// ... Add terminating slash if necessary
		TCHAR cLastChar = *(sDir.Right(1));
		if (_tcschr(COXUNC::m_pszSlashes, cLastChar) == NULL)
			sDir += COXUNC::m_cBackslash;
		UNC = sDir;
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetCurrentDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetCurrentDirectory(COXUNC UNC)
	{
	// ... We will change the parameter UNC in this function

	HRESULT hResult = ERROR_SUCCESS;

	// ... Remove file spacification
	UNC.File().Empty();

	if (!::SetCurrentDirectory(UNC))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetCurrentDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetWindowsDirectory(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	UNC.Empty();

	CString sDir;
	int nMaxDirLength = 0;
	nMaxDirLength = ::GetWindowsDirectory(NULL, nMaxDirLength);
	if (nMaxDirLength != 0)
		{
		nMaxDirLength = ::GetWindowsDirectory(sDir.GetBuffer(nMaxDirLength), nMaxDirLength);
		if (nMaxDirLength == 0)
			GET_LAST_HRESULT(hResult);
		sDir.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		{
		// ... Add terminating slash if necessary
		TCHAR cLastChar = *(sDir.Right(1));
		if (_tcschr(COXUNC::m_pszSlashes, cLastChar) == NULL)
			sDir += COXUNC::m_cBackslash;
		UNC = sDir;
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetWindowsDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetSystemDirectory(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	UNC.Empty();

	CString sDir;
	int nMaxDirLength = 0;
	nMaxDirLength = ::GetSystemDirectory(NULL, nMaxDirLength);
	if (nMaxDirLength != 0)
		{
		nMaxDirLength = ::GetSystemDirectory(sDir.GetBuffer(nMaxDirLength), nMaxDirLength);
		if (nMaxDirLength == 0)
			GET_LAST_HRESULT(hResult);
		sDir.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		{
		// ... Add terminating slash if necessary
		TCHAR cLastChar = *(sDir.Right(1));
		if (_tcschr(COXUNC::m_pszSlashes, cLastChar) == NULL)
			sDir += COXUNC::m_cBackslash;
		UNC = sDir;
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetSystemDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetApplicationDirectory(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	UNC.Empty();

	CString sAppPath;
	int nMaxPathLength = _MAX_PATH;
	nMaxPathLength = ::GetModuleFileName(NULL, sAppPath.GetBuffer(nMaxPathLength), nMaxPathLength);
	if (nMaxPathLength == 0)
		GET_LAST_HRESULT(hResult);
	sAppPath.ReleaseBuffer();

	if (SUCCEEDED(hResult))
		{
		// ... Use path, but remove the file name
		UNC = sAppPath;
		UNC.File().Empty();
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetApplicationDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetTemporaryDirectory(COXUNC& UNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	UNC.Empty();

	CString sDir;
	int nMaxDirLength = 0;
	nMaxDirLength = ::GetTempPath(nMaxDirLength, NULL);
	if (nMaxDirLength != 0)
		{
		nMaxDirLength = ::GetTempPath(nMaxDirLength, sDir.GetBuffer(nMaxDirLength));
		if (nMaxDirLength == 0)
			GET_LAST_HRESULT(hResult);
		sDir.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		{
		// ... Add terminating slash if necessary
		TCHAR cLastChar = *(sDir.Right(1));
		if (_tcschr(COXUNC::m_pszSlashes, cLastChar) == NULL)
			sDir += COXUNC::m_cBackslash;
		UNC = sDir;
		}

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetTemporaryDirectory", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetSize(COXUNC UNC, DWORDLONG& nSize)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	nSize = 0;

	ULARGE_INTEGER nFileSize;
	nFileSize.LowPart = 0;
	nFileSize.HighPart = 0;
	HANDLE hFile = NULL;

	// ... Open the (existing) file
	hFile = ::CreateFile(UNC, 0 /* Query */, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		{
		// ... Get the file size
		nFileSize.LowPart = ::GetFileSize(hFile, &nFileSize.HighPart);
		if (nFileSize.LowPart == MAXDWORD)
			{
			GET_LAST_HRESULT(hResult);
			// If (nFileSize.LowPart == MAXDWORD) AND (GetLastError != NO_ERROR))
			// Then an error has occurred
			}
		VERIFY(::CloseHandle(hFile));
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		nSize = nFileSize.QuadPart;

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetSize", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetCompressedSize(COXUNC UNC, DWORDLONG& nSize)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	nSize = 0;

	ULARGE_INTEGER nFileSize;
	nFileSize.LowPart = 0;
	nFileSize.HighPart = 0;

	// ... Get the compressed file size
	nFileSize.LowPart = ::GetCompressedFileSize(UNC, &nFileSize.HighPart);
	if (nFileSize.LowPart == MAXDWORD)
		{
		GET_LAST_HRESULT(hResult);
		// If (nFileSize.LowPart == MAXDWORD) AND (GetLastError != NO_ERROR))
		// Then an error has occurred
		}

	if (SUCCEEDED(hResult))
		nSize = nFileSize.QuadPart;

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetCompressedSize", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetSize(COXUNC UNC, DWORDLONG nSize)
	{
	HRESULT hResult = ERROR_SUCCESS;

	ULARGE_INTEGER nWantedFileSize;
	nWantedFileSize.QuadPart = nSize;

	// ... Convert from unsigned to signed (needed for SetFilePointer)
	LONG nWantedFileSizeLow = 0;
	LONG nWantedFileSizeHigh = 0;
	if (nWantedFileSize.LowPart < MAXLONG)
		{
		// ... High order bit of low part is not used
		nWantedFileSizeLow = (LONG)nWantedFileSize.LowPart;
		nWantedFileSizeHigh = (LONG)(nWantedFileSize.HighPart << 1);
		}
	else
		{
		// ... Transfer high order bit from low part to high
		nWantedFileSizeLow = (LONG)(nWantedFileSize.LowPart & MAXLONG);
		nWantedFileSizeHigh = (LONG)((nWantedFileSize.HighPart << 1 ) + 1);
		}


	DWORD nNewLowSize;
	HANDLE hFile = NULL;

	// ... Open the (existing) file
	hFile = ::CreateFile(UNC, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		{
		// ... Set the file size
		nNewLowSize = ::SetFilePointer(hFile, nWantedFileSizeLow, &nWantedFileSizeHigh, FILE_BEGIN);
		if (nNewLowSize == MAXDWORD)
			{
			GET_LAST_HRESULT(hResult);
			// If (nNewLowSize == MAXDWORD) AND (GetLastError != NO_ERROR))
			// Then an error has occurred
			}

		if (SUCCEEDED(hResult))
			if (!::SetEndOfFile(hFile))
				GET_LAST_HRESULT(hResult);

		VERIFY(::CloseHandle(hFile));
		}
	else
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetSize", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetShortName(COXUNC UNC, COXUNC& shortUNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	shortUNC.Empty();

	CString sShortPath;
	int nMaxPathLength = 0;
	nMaxPathLength = ::GetShortPathName(UNC, NULL, nMaxPathLength);
	if (nMaxPathLength != 0)
		{
		nMaxPathLength = ::GetShortPathName(UNC, sShortPath.GetBuffer(nMaxPathLength), nMaxPathLength);
		if (nMaxPathLength == 0)
			GET_LAST_HRESULT(hResult);
		sShortPath.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))
		shortUNC = sShortPath;

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetShortName", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetLongName(COXUNC UNC, COXUNC& longUNC)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	longUNC.Empty();

	// Change every subdirectory and file name into its long name
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = NULL;
	longUNC = UNC;

	// First change all the subdirectories
	longUNC.File().Empty();
	CString sShortDir = longUNC.Directory();

	// ... Copy leading slash to result (if present)
	CString sLongDir = sShortDir.Left(1);
	if (_tcschr(COXUNC::m_pszSlashes, *LPCTSTR(sShortDir)) != NULL)
		sLongDir = sShortDir.Left(1);

	CString sDirPart;
	LPCTSTR pszDirPartEnd = NULL;
	if (0 < sShortDir.GetLength())
		pszDirPartEnd = _tcspbrk((LPCTSTR)sShortDir + 1, COXUNC::m_pszSlashes); 
	int nDirLength = 0;

	while(SUCCEEDED(hResult) && (pszDirPartEnd != NULL))
		{
		nDirLength  = (int)(pszDirPartEnd - (LPCTSTR)sShortDir + 1);
		UTBStr::tcsncpy(sDirPart.GetBufferSetLength(nDirLength), nDirLength, sShortDir, nDirLength);

		// ... Get the long directory name
		longUNC.Directory() = sDirPart;
		hFind = ::FindFirstFile(longUNC.FileForm(), &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
			{
			VERIFY(::FindClose(hFind));
			sLongDir += findFileData.cFileName + CString(UNC.PreferedSlash());
			}
		else
			GET_LAST_HRESULT(hResult);

		// ... Get next subirectory
		pszDirPartEnd = _tcspbrk(pszDirPartEnd + 1, COXUNC::m_pszSlashes); 
		}
	longUNC.Directory() = sLongDir;

	// Then change the file name itself
	longUNC.File() = UNC.File();
	hFind = ::FindFirstFile(UNC, &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		{
		VERIFY(::FindClose(hFind));
		longUNC.File() = findFileData.cFileName;
		}
	else
		GET_LAST_HRESULT(hResult);

	if (FAILED(hResult))
		longUNC.Empty();

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetLongName", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetLogicalDrives(CString& sDrives)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	sDrives.Empty();

	int nMaxDrivesLength = 0;
	nMaxDrivesLength = ::GetLogicalDriveStrings(nMaxDrivesLength, NULL);
	if (nMaxDrivesLength != 0)
		{
		LPTSTR pszDrives = sDrives.GetBuffer(nMaxDrivesLength);
		nMaxDrivesLength = ::GetLogicalDriveStrings(nMaxDrivesLength, pszDrives);
		if (nMaxDrivesLength != 0)
			{
			// Change all NULL-characters to a '|' which is easier to use in a CString
			// ... Stop when we find two NULLs after each other
			pszDrives += _tcslen(pszDrives);
			while (*(pszDrives + 1) != _T('\0'))
				{
				*pszDrives = _T('|');
				pszDrives += _tcslen(pszDrives);
				}
			}
		else
			GET_LAST_HRESULT(hResult);

		sDrives.ReleaseBuffer();
		}
	else
		GET_LAST_HRESULT(hResult);

	if (SUCCEEDED(hResult))

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetLogicalDrives", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetTotalDiskSpace(COXUNC UNC, DWORDLONG& nTotalSpace)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	nTotalSpace = 0;

	DWORDLONG nFreeSpace = 0;
	if (!GetDiskSpace(UNC.GetRoot(), nTotalSpace, nFreeSpace))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetTotalDiskSpace", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetFreeDiskSpace(COXUNC UNC, DWORDLONG& nFreeSpace)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	nFreeSpace = 0;

	DWORDLONG nTotalSpace = 0;
	if (!GetDiskSpace(UNC.GetRoot(), nTotalSpace, nFreeSpace))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetFreeDiskSpace", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetDriveType(COXUNC UNC, UINT& nDriveType)
	{
	HRESULT hResult = ERROR_SUCCESS;

	nDriveType = ::GetDriveType(UNC.GetRoot());

	// ... GetDriveType does not use GetLastError so we substitute
	//	   the result for more appropriate error codes
	if (nDriveType == 0)
		hResult = HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE);
	else if (nDriveType == 1)
		hResult = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetDriveType", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetDiskInformation(COXUNC UNC, CString& sName, DWORD& nSerialNumber, 
		DWORD& nMaximumComponentLength, DWORD& nFileSystemFlags, CString& sFileSystemName)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	sName.Empty();
	nSerialNumber = 0;
	nMaximumComponentLength = 0;
	nFileSystemFlags = 0;
	sFileSystemName.Empty();

	const int nMaxVolumneNameLength = 256;
	const int nMaxFileSystemNameLength = 256;

	if (!::GetVolumeInformation(UNC.GetRoot(), sName.GetBuffer(nMaxVolumneNameLength), nMaxVolumneNameLength,
		&nSerialNumber, &nMaximumComponentLength, &nFileSystemFlags, 
		sFileSystemName.GetBuffer(nMaxFileSystemNameLength), nMaxFileSystemNameLength))
		{
		GET_LAST_HRESULT(hResult);
		}

	sName.ReleaseBuffer();
	sFileSystemName.ReleaseBuffer();

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetDiskInformation", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetDiskName(COXUNC UNC, LPCTSTR pszDiskName)
	{
	HRESULT hResult = ERROR_SUCCESS;

	if (!::SetVolumeLabel(UNC.GetRoot(), pszDiskName))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetDiskName", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetAttributes(COXUNC UNC, DWORD& nAttributes)
	{
	HRESULT hResult = ERROR_SUCCESS;

	nAttributes = ::GetFileAttributes(UNC.FileForm());
	if (nAttributes == MAXDWORD)
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetAttributes", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetAttributes(COXUNC UNC, DWORD nAttributes)
	{
	HRESULT hResult = ERROR_SUCCESS;

	if (!::SetFileAttributes(UNC.FileForm(), nAttributes))
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetAttributes", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetCompression(COXUNC UNC, USHORT nCompressionState)
	{
	HRESULT hResult = ERROR_SUCCESS;

	DWORD nBytesReturned = 0;
	HANDLE hFile = NULL;

	// Get handle to file or directory
	BOOL bDirectory = UNC.File().IsEmpty();
	hFile = ::CreateFile(UNC.FileForm(), GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, 
		bDirectory ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		{
		if (!DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, &nCompressionState, 
			sizeof(nCompressionState),  NULL, 0, &nBytesReturned, NULL))
			{
			GET_LAST_HRESULT(hResult);
			}
		VERIFY(::CloseHandle(hFile));
		}
	else
		GET_LAST_HRESULT(hResult);


	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetAttributes", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetCompression(COXUNC UNC, USHORT& nCompressionState)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	nCompressionState = 0;

	DWORD nBytesReturned = 0;
	HANDLE hFile;

	// Get handle to file or directory
	BOOL bDirectory = UNC.File().IsEmpty();
	hFile = ::CreateFile(UNC.FileForm(), GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, 
		bDirectory ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		{
		if (!DeviceIoControl(hFile, FSCTL_GET_COMPRESSION, NULL, 0, &nCompressionState, 
			sizeof(nCompressionState),  &nBytesReturned, NULL))
			{
			GET_LAST_HRESULT(hResult);
			}
		VERIFY(::CloseHandle(hFile));
		}
	else
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetCompression", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::GetTime(COXUNC UNC, CTime& creationTime, 
		CTime& lastAccessTime, CTime& lastWriteTime)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// ... Initialize result
	creationTime = 0;
	lastAccessTime = 0;
	lastWriteTime = 0;

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = ::FindFirstFile(UNC.FileForm(), &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
		{
		VERIFY(::FindClose(hFind));

		// Convert FILETIME to CTime (UTC -> local -> system)
		creationTime = CTime(findFileData.ftCreationTime);
		lastAccessTime = CTime(findFileData.ftLastAccessTime);
		lastWriteTime = CTime(findFileData.ftLastWriteTime);

		// When creation and last access is not supported, use last write time
		if (creationTime.GetTime() == 0)
			creationTime = lastWriteTime;
		if (lastAccessTime.GetTime() == 0)
			lastAccessTime = lastWriteTime;
		}
	else
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::GetTime", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::SetTime(COXUNC UNC, CTime creationTime, CTime lastAccessTime, 
	CTime lastWriteTime)
	{
	HRESULT hResult = ERROR_SUCCESS;

	BOOL bDirectory = UNC.File().IsEmpty();
	HANDLE hFile = ::CreateFile(UNC.FileForm(), GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		bDirectory ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
		{
		FILETIME newCreationTime;
		FILETIME newLastAccessTime;
		FILETIME newLastWriteTime;

		LPFILETIME pNewCreationTime = NULL;
		LPFILETIME pNewLastAccessTime = NULL;
		LPFILETIME pNewLastWriteTime = NULL;

		// Convert CTime to FILETIME (sytem -> local -> UTC)
		if ((creationTime.GetTime() != 0) && TimeToFileTime(creationTime, &newCreationTime))
			pNewCreationTime = &newCreationTime;
		if ((lastAccessTime.GetTime() != 0) && TimeToFileTime(lastAccessTime, &newLastAccessTime))
			pNewLastAccessTime = &newLastAccessTime;
		if ((lastWriteTime.GetTime() != 0) && TimeToFileTime(lastWriteTime, &newLastWriteTime))
			pNewLastWriteTime = &newLastWriteTime;

		// Set the new times
		if (!::SetFileTime(hFile, pNewCreationTime, pNewLastAccessTime, pNewLastWriteTime))
			GET_LAST_HRESULT(hResult);

		VERIFY(::CloseHandle(hFile));
		}
	else
		GET_LAST_HRESULT(hResult);


	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::SetTime", hResult);
	return hResult;
	}

HRESULT COXUNCStandardActor::FindFirstFile(COXUNC UNC, HANDLE &hFindFile, WIN32_FIND_DATA &findFileData,
	BOOL /* bOnlyDirectories */ /* = FALSE */)
	{
	HRESULT hResult = ERROR_SUCCESS;

	// bOnlyDirectories is ignored for now
	hFindFile = ::FindFirstFile(UNC.FileForm(), &findFileData);
	if (hFindFile == INVALID_HANDLE_VALUE)
		{
		// ... Use a NULL handle instead of INVALID_HANDLE_VALUE
		hFindFile = NULL;
		GET_LAST_HRESULT(hResult);
		}

#ifdef _DEBUG
	// The ERROR_FILE_NOT_FOUND condition is a situation that is likely to occur
	// (viz. at the end of each iteration)
	// We do not trace a message then
	if (HRESULT_CODE(hResult) != ERROR_FILE_NOT_FOUND)
		CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::FindFirstFile", hResult);
#endif

	return hResult;
	}


HRESULT COXUNCStandardActor::FindNextFile(HANDLE hFindFile, WIN32_FIND_DATA &findFileData)
	{
	HRESULT hResult = ERROR_SUCCESS;
	BOOL bSuccess = TRUE;

    bSuccess = ::FindNextFile(hFindFile, &findFileData);
	if (!bSuccess)
		GET_LAST_HRESULT(hResult);

#ifdef _DEBUG
	// The ERROR_NO_MORE_FILES condition is a situation that is likely to occur
	// (viz. at the end of each iteration)
	// We do not trace a message then
	if (HRESULT_CODE(hResult) != ERROR_NO_MORE_FILES)
		CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::FindNextFile", hResult);
#endif

	return hResult;
	}

HRESULT COXUNCStandardActor::FindClose(HANDLE hFindFile)
	{
	HRESULT hResult = ERROR_SUCCESS;
	BOOL bSuccess = TRUE;

    bSuccess = ::FindClose(hFindFile);
	if (!bSuccess)
		GET_LAST_HRESULT(hResult);

	CONDITIONAL_TRACE_RESULT("COXUNCStandardActor::FindClose", hResult);
	return hResult;
	}


#ifdef _DEBUG
void COXUNCStandardActor::AssertValid() const
	{
	CObject::AssertValid();
	}

void COXUNCStandardActor::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	}
#endif //_DEBUG

COXUNCStandardActor::~COXUNCStandardActor()
	{
	}

// protected:
#ifdef _DEBUG

static TCHAR szUnknownError[] = _T("*** Unknown Error ***");
static DWORD dwLangID = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT); 

CString COXUNCStandardActor::GetResultMessage(HRESULT hResult)
	// --- In  : hResult : The result code
	// --- Out : 
	// --- Returns : A string containg a message of the specified code
	// --- Effect : 
	{
	CString sResultMessage;
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
		TRACE2("COXUNCStandardActor::GetResultMessage : No message was found for result code %i == 0x%8.8X\n",
			hResult, hResult);
	  	//pszMsgBuf = szUnknownError;
		VERIFY(sResultMessage.LoadString(IDS_OX_UNCUNKNOWNERROR));
		bUnknown = TRUE;
		}
	else
		sResultMessage = pszMsgBuf;

	// ... Clean up
	if (!bUnknown)
		LocalFree(pszMsgBuf);

	return sResultMessage;
	}
#endif //_DEBUG

BOOL COXUNCStandardActor::GetDiskSpace(LPCTSTR pszRoot, DWORDLONG& nTotalSpace, DWORDLONG& nFreeSpace)
	// --- In  : pszRoot : The root of wich info is requested
	// --- Out : nTotalSpace : The total space in bytes
	//			 nFreeSpace : The free space in bytes
	// --- Returns : Whether it was successful or not
	// --- Effect : Computes the total and free space of a disk
	{
	BOOL bSuccess = TRUE;

	// Check whether we can use the GetDiskFreeSpaceEx function 
	// This is available on Windows 95 systems beginning with OEM Service Release 2 (OSR 2). 

	// ... Can we use GetDiskFreeSpaceEx
	BOOL bExFunctionAvailable = FALSE;

	// Check the OS Version number
	OSVERSIONINFO OSVersionInfo;
	::ZeroMemory(&OSVersionInfo, sizeof(OSVersionInfo));
	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);
	VERIFY(::GetVersionEx(&OSVersionInfo));
	if ((OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		(1000 < LOWORD(OSVersionInfo.dwBuildNumber)) )
		bExFunctionAvailable = TRUE;

	if (bExFunctionAvailable)
		{
		// GetDiskFreeSpaceEx function is available
		ULARGE_INTEGER nFreeBytesAvailableToCaller;
		ULARGE_INTEGER nTotalNumberOfBytes;
		ULARGE_INTEGER nTotalNumberOfFreeBytes;
		nFreeBytesAvailableToCaller.QuadPart = 0;
		nTotalNumberOfBytes.QuadPart = 0;
		nTotalNumberOfFreeBytes.QuadPart = 0;
	
		HINSTANCE hKernelInstance = NULL;
		BOOL (*pfnGetDiskFreeSpaceEx)(LPCTSTR lpDirectoryName, 
			PULARGE_INTEGER lpFreeBytesAvailableToCaller,
			PULARGE_INTEGER lpTotalNumberOfBytes, 
			PULARGE_INTEGER lpTotalNumberOfFreeBytes) = NULL;

		hKernelInstance = ::LoadLibrary(_T("KERNEL32.DLL"));
		if (hKernelInstance != NULL)
			(FARPROC&)pfnGetDiskFreeSpaceEx = ::GetProcAddress(hKernelInstance, "GetDiskFreeSpaceEx");
		if (pfnGetDiskFreeSpaceEx == NULL)
			{
			TRACE0("COXUNCStandardActor::GetDiskSpace : Dynamic load of GetDiskFreeSpaceEx function failed\n");
			// ... Try GetDiskFreeSpace function
			bExFunctionAvailable = FALSE;
			}

		bSuccess = bExFunctionAvailable && (*pfnGetDiskFreeSpaceEx)(pszRoot, &nFreeBytesAvailableToCaller, &nTotalNumberOfBytes, &nTotalNumberOfFreeBytes);
		if (bSuccess)
			{
			nTotalSpace = nTotalNumberOfBytes.QuadPart;
			nFreeSpace = nTotalNumberOfFreeBytes.QuadPart;
			}

		// ... No need to call FreeLibrary because KERNEL32 will stay loaded anyway
		}

	if (!bExFunctionAvailable)
		{
		// GetDiskFreeSpaceEx function is not available, use GetDiskFreeSpace
		DWORD nSectorsPerCluster = 0;
		DWORD nBytesPerSector = 0;
		DWORD nNumberOfFreeClusters = 0;
		DWORD nTotalNumberOfClusters = 0;

		bSuccess = ::GetDiskFreeSpace(pszRoot, &nSectorsPerCluster, &nBytesPerSector,
			&nNumberOfFreeClusters, &nTotalNumberOfClusters);
		if (bSuccess)
			{
			nTotalSpace = UInt32x32To64(nTotalNumberOfClusters, nSectorsPerCluster) * (DWORDLONG)nBytesPerSector;
			nFreeSpace = UInt32x32To64(nNumberOfFreeClusters, nSectorsPerCluster) * (DWORDLONG)nBytesPerSector;
			}
		}

	return bSuccess;
	}

BOOL COXUNCStandardActor::TimeToFileTime(const CTime& time, LPFILETIME pFileTime)
	// --- In  : time
	//			 pFileTime : Points to a FILETIME object
	// --- Out : pFileTime : The filled out object
	// --- Returns : Whether it was successful or not
	// --- Effect : Converts a CTime object to a FILETIME object
	{
	SYSTEMTIME sysTime;
	sysTime.wYear = (WORD)time.GetYear();
	sysTime.wMonth = (WORD)time.GetMonth();
	sysTime.wDay = (WORD)time.GetDay();
	sysTime.wHour = (WORD)time.GetHour();
	sysTime.wMinute = (WORD)time.GetMinute();
	sysTime.wSecond = (WORD)time.GetSecond();
	sysTime.wMilliseconds = 0;

	// Convert system time to local file time
	FILETIME localTime;
	if (!::SystemTimeToFileTime((LPSYSTEMTIME)&sysTime, &localTime))
		{
		TRACE0("COXUNCStandardActor::TimeToFileTime : Illegal system time encountered, ignoring\n");
		return FALSE;
		}

	// Convert local file time to UTC file time
	if (!::LocalFileTimeToFileTime(&localTime, pFileTime))
		{
		TRACE0("COXUNCStandardActor::TimeToFileTime : Illegal logal file time encountered, ignoring\n");
		return FALSE;
		}

	return TRUE;
	}


// private:

// ==========================================================================
