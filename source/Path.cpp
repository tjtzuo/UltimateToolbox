// ==========================================================================
// 							Class Implementation : COXPathSpec
// ==========================================================================

// Source file : path.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "path.h"		// class specification
#include "xstring.h"	// for string-int conversion
#ifndef WIN32
#include "toolhelp.h"	// To determine the module handle
#endif

#include <direct.h>		// For directory functions (_fullpath, ...)
#include <dos.h>		// For _dos_setfileattr, ...
#include <io.h>			// For _chsize()

#include "UTBStrOp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXPathSpec, COXDirSpec)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

// private:

// Member functions ---------------------------------------------------------
// public:

COXPathSpec::COXPathSpec()
	{
	}
	
COXPathSpec::COXPathSpec(LPCTSTR pszPath)
	{
	if (!SetPath(pszPath))
		{
		TRACE(_T("COXPathSpec::COXPathSpec : An invalid path (%s) was specified, clearing object\n"),
			pszPath);
		SetPath(_T(""));
		}
	}
	
COXPathSpec::COXPathSpec(const COXPathSpec& pathSrc)
	:
	COXDirSpec(pathSrc),
	COXFileSpec(pathSrc)
	{
	}
	
COXPathSpec& COXPathSpec::operator=(const COXPathSpec& pathSrc)
	{
	COXDirSpec::operator=(pathSrc);
	COXFileSpec::operator=(pathSrc);
	return *this;
	}
	
CString COXPathSpec::GetPath() const
	{
	CString sDir = GetDirectory();
	CString sFile = GetFileName();
	CString sPath;
	if (!sFile.IsEmpty())
		if (!sDir.IsEmpty())
			if (sDir.Right(1) != CString(_T("\\")))
				// \\DIR and AAA.BBB
				sPath = sDir + _T("\\") + sFile;
			else
				// \\ and AAA.BBB
				sPath = sDir + sFile;
		else
			// empty and AAA.BBB
			sPath = sFile;
	else
		// \\DIR and empty
		sPath = sDir;
	return sPath;;
	}
	
BOOL COXPathSpec::SetPath(LPCTSTR pszPath)
	{            
	TCHAR pszDrive[_MAX_DRIVE];
	TCHAR pszSubdir[_MAX_DIR];
	TCHAR pszBaseName[_MAX_FNAME];
	TCHAR pszExtender[_MAX_EXT];
	
	UTBStr::tsplitpath(pszPath, pszDrive, _MAX_DRIVE, pszSubdir, _MAX_DIR, pszBaseName, _MAX_FNAME, pszExtender, _MAX_EXT);
//	_tsplitpath(pszPath, pszDrive, pszSubdir, pszBaseName, pszExtender);

	size_t nSubDir = _tcsclen(pszSubdir);
	if (1 < nSubDir)
		// ... When not empty and not root, remove trailing back slash
		pszSubdir[nSubDir - 1] = _T('\0');
	size_t nExt = _tcsclen(pszExtender);
	if (1 <= nExt)
		// ... Remove leading full stop
		UTBStr::tcscpy(pszExtender, _MAX_EXT, pszExtender + 1);
	
	return SetDrive(pszDrive) && SetSubdirectory(pszSubdir) &&
		   SetExtender(pszExtender) && SetBaseName(pszBaseName);
	}
	
void COXPathSpec::ForceSetPath(LPCTSTR pszPath)
	{            
	TCHAR pszDrive[_MAX_DRIVE];
	TCHAR pszSubdir[_MAX_DIR];
	TCHAR pszBaseName[_MAX_FNAME];
	TCHAR pszExtender[_MAX_EXT];
	
	UTBStr::tsplitpath(pszPath, pszDrive, _MAX_DRIVE, pszSubdir, _MAX_DIR, pszBaseName, _MAX_FNAME, pszExtender, _MAX_EXT);

	size_t nSubDir = _tcsclen(pszSubdir);
	if (1 < nSubDir)
		// ... When not empty and not root, remove trailing back slash
		pszSubdir[nSubDir - 1] = '\0';
	size_t nExt = _tcsclen(pszExtender);
	if (1 <= nExt)
		// ... Remove leading full stop
		UTBStr::tcscpy(pszExtender, _MAX_EXT, pszExtender + 1);
	
	ForceSetDrive(pszDrive);
	ForceSetSubdirectory(pszSubdir);

	ForceSetExtender(pszExtender);
	ForceSetBaseName(pszBaseName);
	}
	
BOOL COXPathSpec::SetPath(const COXDirSpec& dirSpec, const COXFileSpec& fileSpec)
	{
	COXDirSpec::operator=(dirSpec);
	COXFileSpec::operator=(fileSpec);
	return TRUE;
	}
	
CString COXPathSpec::GetShortDescription()
	{
	// ... If path spec is empty, just return
	if (GetPath().IsEmpty())
		return _T("");

	COXPathSpec tempPath(*this);
	CString sLastSubdir;
	CString sEliminatedDirs = _T("\\..\\");

	// First try to make absolute path
	if (!tempPath.MakeAbsolute())
		{
		TRACE(_T("COXPathSpec::GetShortDescription : Could not make absolute path, returning full path spec\n"));
		return GetPath();
		}

	// Get the last subdir
	sLastSubdir = tempPath.GetLastSubdirectory().GetSubdirectory();
	// ... Last subdir should never contain back slashes
	ASSERT(sLastSubdir.Find(_T('\\')) == -1);
	// ... If last subdir is empty (root) or equals dir itself, 
	//     no subdirs have been eliminated
	if (sLastSubdir.IsEmpty())
		sEliminatedDirs.Empty();
		else if ((_T("\\") + sLastSubdir) == tempPath.GetSubdirectory())
			sEliminatedDirs = _T("\\");

	// Return the composed short description
	return tempPath.GetDrive() + sEliminatedDirs + sLastSubdir + _T("\\") + tempPath.GetFileName();
	}

BOOL COXPathSpec::GetShortPathName(CString& sShortPath)
{
	sShortPath=GetPath();
	// ... If path spec is empty, just return
	if (sShortPath.IsEmpty())
		return FALSE;

#ifdef WIN32
	COXPathSpec tempPath(*this);
	// First try to make absolute path
	tempPath.MakeAbsolute();

	CString sPath=tempPath.GetPath();
	BOOL bResult=(::GetShortPathName(sPath,sShortPath.GetBuffer(MAX_PATH),
		MAX_PATH)>0);
	sShortPath.ReleaseBuffer();

	return bResult;
#else
	return TRUE;
#endif
}

BOOL COXPathSpec::MakeTemp(BOOL bCreateEmpty /* = TRUE */,  LPCTSTR pszPrefix /* = _T("TMP") */,
						   LPCTSTR pszTempDir /* = _T("") */)
	{
	TCHAR path_buffer[_MAX_PATH + 1];
	*path_buffer = _T('\0'); 

#ifdef WIN32
	// Get temp path
	BOOL bSucces = TRUE;
	CString sTempPath(pszTempDir);
	if (sTempPath.IsEmpty())
		{
		bSucces = ::GetTempPath(_MAX_PATH, sTempPath.GetBuffer(_MAX_PATH));
		sTempPath.ReleaseBuffer();
		}

	if (!bSucces)
		return FALSE;

	if (!::GetTempFileName(sTempPath,	// Use the default drive
		    		       pszPrefix,    // Temporary file name prefix
					       0, 			// Generate number and create file
				    	   path_buffer)) // Result
		return FALSE;

#else
	// Get temp path
	pszTempDir;
	::GetTempFileName(0,			// Use the default drive
				      pszPrefix,    // Temporary file name prefix
				      0, 			// Generate number and create file
				      path_buffer); // Result
#endif

	if (*path_buffer != _T('\0'))
		{
		BOOL bResult = SetPath(path_buffer);
		if (bCreateEmpty)
			{
			// Empty file should have been created by ::GetTempFileName
			ASSERT(Exists());
			}
		else
			{
			// Delete empty file created by ::GetTempFileName
			VERIFY(DoRemove());
			ASSERT(!Exists());
			}
		return bResult;
		}
	else
		{
		TRACE(_T("COXPathSpec::MakeTemp : Could not make temporary path spec\n"));
		return FALSE;
		}
	}

BOOL COXPathSpec::MakeAbsolute()
	{
	// If no file name was specified, just return
	// Apparently in WIN32 _fullpath returns the apllications full path
	//  in this case (??)
	if (GetFileName().IsEmpty())
		return TRUE;

	TCHAR pszFullPath[_MAX_PATH];

	if (_tfullpath(pszFullPath, (LPCTSTR)GetPath(), _MAX_PATH) != NULL)
		return SetPath(pszFullPath);
	else
		return FALSE;
	}
	
BOOL COXPathSpec::MakeUnique()
	{
	if (GetBaseName().IsEmpty())
		SetBaseName(_T("unique"));
		
	if (!Exists())
		return TRUE;  

	// Change the name by first adding underscores, until 8 characters are used.
	// Then the last character(s) are replaced by a number starting from 2,
	// until a unique name is found.
	COXString sNumber = _T("2");
	CString sBaseName = GetBaseName().Left(8);
	while (sBaseName.GetLength() < 8)
		sBaseName +=  _T("_");          
		
	ASSERT(sBaseName.GetLength() == 8);
	ASSERT(Exists());
	do
		{
		sBaseName = sBaseName.Left(8 - sNumber.GetLength());
		sBaseName += sNumber;
		VERIFY(SetBaseName(sBaseName));
		sNumber = sNumber.GetInt() + 1;		// Implicit conversion to int
		}
		while(Exists());
		
	return TRUE;
	}
	
BOOL COXPathSpec::Exists() const
	{
	CFileStatus fileStatus;
	return CFile::GetStatus(GetPath(), fileStatus);
	}
	
BOOL COXPathSpec::IsEmpty() const
	{
	return COXDirSpec::IsEmpty() && COXFileSpec::IsEmpty();
	}
	
void COXPathSpec::Empty()
	{
	COXDirSpec::Empty();
	COXFileSpec::Empty();
	}
	
BOOL COXPathSpec::DoSearch(COXFileSpec fileName, COXDirSpec startingDir /* = COXDirSpec() */,
		 BOOL bRecursively /* = FALSE */)
	{
	COXPathSpec resultPath;
	COXDirSpec currentDir;
	
	// First store the current dir
	VERIFY(currentDir.DoGetCurrentDir());
	
	// 1. Check the specified directory
	if (!startingDir.IsEmpty())
		{
		resultPath.SetPath(startingDir, fileName);
		if (resultPath.Exists())
			{
			*this = resultPath;
			return TRUE;
			}
		}
		
	// 2. Check the subdirectories of the specified directory
	
	// ... Recursive search is not yet implemented
	ASSERT(!bRecursively);
	bRecursively;

	// 3  Check the EXE-directory
	TCHAR pszModulePath[_MAX_PATH];
	if (::GetModuleFileName(GetThisModule(), pszModulePath, _MAX_PATH) != 0)
		{
		VERIFY(resultPath.SetPath(pszModulePath));
		resultPath.COXFileSpec::operator=(fileName);
		if (resultPath.Exists())
			{
			*this = resultPath;
			return TRUE;
			}
		}
	else
		{
		TRACE(_T("COXPathSpec::DoSearch : Could not retrieve the path of the running application\n"));
		}
	
	// 4. Check the current directory
	if (!currentDir.IsEmpty())
		{
		resultPath.SetPath(currentDir, fileName);
		if (resultPath.Exists())
			{
			*this = resultPath;
			return TRUE;
			}
		}

	// 5. Check the Windows directory
	TCHAR pszWinDir[_MAX_DIR];
	if (::GetWindowsDirectory(pszWinDir, _MAX_DIR) != 0)
		{
		VERIFY(resultPath.SetDirectory(pszWinDir));
		resultPath.COXFileSpec::operator=(fileName);
		if (resultPath.Exists())
			{
			*this = resultPath;
			return TRUE;
			}
		}
	else
		{
		TRACE(_T("COXPathSpec::DoSearch : Could not retrieve the Windows directory\n"));
		}

	// 6. Check the System directory
	TCHAR pszSystemDir[_MAX_DIR];
	if (::GetSystemDirectory(pszSystemDir, _MAX_DIR) != 0)
		{
		VERIFY(resultPath.SetDirectory(pszSystemDir));
		resultPath.COXFileSpec::operator=(fileName);
		if (resultPath.Exists())
			{
			*this = resultPath;
			return TRUE;
			}
		}
	else
		{
		TRACE(_T("COXPathSpec::DoSearch : Could not retrieve the Windows directory\n"));
		}

	// 7. Check the directories of PATH-environment variable
	TCHAR pszResultPath[_MAX_PATH];
	if (SearchEnvironment(fileName.GetFileName(), _T("PATH"), pszResultPath))
		{
		ASSERT(*pszResultPath != '\0');
		VERIFY(SetPath(pszResultPath));
		return TRUE;
		}

	// If still not returned, the file was not found
	return FALSE;
	}
	
BOOL COXPathSpec::DoCopy(COXPathSpec destinationPath)
	{
	ASSERT(!GetFileName().IsEmpty());		// Source file must be specified
	CString sSourcePath;
	CString sDestPath;
	if (destinationPath.GetFileName().IsEmpty())
		// ... Make destination file name equal to source
		destinationPath.SetFileName(GetFileName());      
	sSourcePath = GetPath();
	sDestPath = destinationPath.GetPath();
	
#ifdef _DEBUG
	if (!Exists())
		{
		TRACE(_T("COXPathSpec::DoCopy : Source file %s does not exist\n"), sSourcePath);
		return FALSE;
		}
	else
		if (destinationPath.Exists())
			TRACE(_T("COXPathSpec::DoCopy : Destination file %s already exists, truncating ...\n"), sDestPath);
#endif	

	const int nBufferLength = 2048;
	BYTE pBuffer[nBufferLength + 1];
	int nLengthRead;
	CFile source;
	CFile dest;
	MSG msg;

	BOOL bSuccess = TRUE;
	TRY
		{
		if ( (source.Open(sSourcePath,CFile::modeRead | CFile::shareCompat) != 0) &&
		     (dest.Open(sDestPath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive) != 0) )
		     {
			do
				{
				if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
					{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					}

				nLengthRead = source.Read(pBuffer, nBufferLength);
				dest.Write(pBuffer, nLengthRead);
				}
				while (nLengthRead == nBufferLength);	// So while not EOF
			source.Close();
			dest.Close();
			}
		else
			{
			TRACE(_T("COXPathSpec::DoCopy : Could not open files\n"));
			bSuccess = FALSE;
			}
		}
	CATCH(CFileException, pxFile)
		{
		TRACE(_T("COXPathSpec::DoCopy : Catching FileException (%XH)\n"), pxFile->m_cause);
	
		source.Close();
		dest.Close();
		destinationPath.DoRemove();
	
		bSuccess = FALSE;
		}
	END_CATCH      

	// copy original time/date of creation and file attributes	
	DoGetInfo();
	destinationPath.SetTime(GetTime());
	destinationPath.DoSetTime();
	destinationPath.SetAttributes((CFile::Attribute)GetAttributes());
	destinationPath.DoSetAttributes();

	return bSuccess;			
	}
	
BOOL COXPathSpec::DoMove(COXPathSpec destinationPath) const
	{
	if (destinationPath.GetDrive() == _T(""))
		VERIFY(destinationPath.SetDrive(GetDrive()));
	if (destinationPath.GetSubdirectory() == _T(""))
		VERIFY(destinationPath.SetSubdirectory(GetSubdirectory()));
	if (destinationPath.GetFileName() == _T(""))
		VERIFY(destinationPath.SetFileName(GetFileName()));
	if (*this == destinationPath)
		{
		TRACE(_T("COXPathSpec::DoMove : Source and destination files are the same %s\n"), 
			(LPCTSTR)GetPath());
		// ... Nothing to do
		return TRUE;
		}
		
#ifdef _DEBUG
	if (!Exists())
		TRACE(_T("COXPathSpec::DoMove : Source file %s does not exist\n"), 
			(LPCTSTR)GetPath());
	if (destinationPath.Exists())
		TRACE(_T("COXPathSpec::DoMove : Destination file %s does not exist\n"), 
			(LPCTSTR)destinationPath.GetPath());
#endif
	
	// ... Assume success
	BOOL bSuccess = TRUE;
	TRY
		{
		CFile::Rename(GetPath(), destinationPath.GetPath());
		}
	CATCH(CFileException, px)
		{
		TRACE(_T("COXPathSpec::DoMove : CFile::Rename(%s, %s) failed with CFileException cause %i\n"), 
			(LPCTSTR)GetPath(), (LPCTSTR)destinationPath.GetPath(), px->m_cause);
		bSuccess = FALSE;
		}
	END_CATCH
	return bSuccess;
	}
	
BOOL COXPathSpec::DoRemove(BOOL bIgnoreReadOnly /* = FALSE */) const
	{
	CString sPath = GetPath();
	
	// ... This function may only be used to remove a file,
	//	   not a directory. So the file name must not be empty
	ASSERT(!GetFileName().IsEmpty());

#ifdef _DEBUG
	if (!Exists())
		TRACE(_T("COXPathSpec::DoRemove : File %s does not exist\n"), sPath);
#endif	
	if (bIgnoreReadOnly)
#ifdef WIN32
		if (!SetFileAttributes(sPath, CFile::normal))
#else
		if(_dos_setfileattr(sPath, CFile::normal))
#endif
			{
			TRACE(_T("COXPathSpec::DoRemove : File not found or cannot remove R/O attribute of %s\n"), sPath);
			return FALSE;
			}

	if (_tremove(sPath))
		{
		TRACE(_T("COXPathSpec::DoRemove : Cannot remove file %s\n"), sPath);
		return FALSE;
		}            
	return TRUE;
	}
	
BOOL COXPathSpec::DoGetInfo()
	{
	CFileStatus fileStatus;
	if (CFile::GetStatus(GetPath(), fileStatus))
		{
		m_time = 		fileStatus.m_mtime;
		m_lnLength = 	(LONG) fileStatus.m_size;
		m_eAttributes = fileStatus.m_attribute;
		return TRUE;
		}
	else
		{
		TRACE(_T("COXPathSpec::DoGetInfo : Could not get file status of %s\n"),
			GetPath());
		return FALSE;
		}
	}
	
BOOL COXPathSpec::DoSetTime()
	{
	CFileStatus fileStatus;
	
	if (!CFile::GetStatus(GetPath(), fileStatus))
		{
		TRACE(_T("COXPathSpec::DoSetTime : Could not even get the present status, failing\n"));
		return FALSE;
		}

	// Set new time, (DOS only knows one type of time)		
	fileStatus.m_ctime = m_time;;
	fileStatus.m_mtime = fileStatus.m_ctime;
	fileStatus.m_atime = fileStatus.m_ctime;
	
	BOOL bSuccess = TRUE;
	TRY
		{
		CFile::SetStatus(GetPath(), fileStatus);
		}
	CATCH(CFileException, pxFile)
		{
		TRACE(_T("COXPathSpec::DoSetTime : Catching file exception (cause %XH)"),
				((CFileException*)pxFile)->m_cause);
		bSuccess = FALSE;
		}
	END_CATCH
	return bSuccess;
	}
	
BOOL COXPathSpec::DoSetLength()
	{
	// SetStatus does not set the file length, so we use direct dos access
#ifdef WIN32
	HANDLE hFile;
	hFile = CreateFile(GetPath(), GENERIC_READ | GENERIC_WRITE, 
			0,NULL, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could not open file %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	SetFilePointer(hFile,m_lnLength, NULL,FILE_BEGIN);
	if(GetLastError() != ERROR_SUCCESS)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could set new length of %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	SetEndOfFile(hFile);
	if(GetLastError() != ERROR_SUCCESS)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could not set end of %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	if (!CloseHandle(hFile))
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could not close file %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	
#else
	UINT nErr;
	int handle;
	if ((nErr = _dos_open(GetPath(), CFile::modeReadWrite, &handle)) != 0)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could not open file %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	if ((nErr = _chsize(handle, m_lnLength)) != 0)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could set new length of %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
	if ((nErr = _dos_close(handle)) != 0)
		{
		TRACE(_T("COXPathSpec::DoSetLength : Could not close file %s, failing\n"),
			(LPCTSTR)GetPath());
		return FALSE;
		}
#endif
	
	return TRUE;
	}
	
BOOL COXPathSpec::DoSetAttributes()
	{
	CFileStatus fileStatus;
	
	if (!CFile::GetStatus(GetPath(), fileStatus))
		{
		TRACE(_T("COXPathSpec::DoSetAttributes : Could not even get the present status, failing\n"));
		return FALSE;
		}

	// Set new attributes
	fileStatus.m_attribute = m_eAttributes;;
	
	BOOL bSuccess = TRUE;
	TRY
		{
		CFile::SetStatus(GetPath(), fileStatus);
		}
	CATCH(CFileException, pxFile)
		{
		TRACE(_T("COXPathSpec::DoSetAttributes : Catching file exception (cause %XH)"),
				((CFileException*)pxFile)->m_cause);
		bSuccess = FALSE;
		}
	END_CATCH
	return bSuccess;
	}

void COXPathSpec::Serialize(CArchive& archive)
	{
	COXDirSpec::Serialize(archive);
	COXFileSpec::Serialize(archive);
	}

// To avoid conflict between '#define new DEBUG_NEW' and 'operator new'
#undef new 

#ifndef _DEBUG
void* COXPathSpec::operator new(size_t nSize)
	{
	return COXDirSpec::operator new(nSize);
	}
#else	
void* COXPathSpec::operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
	{
	return COXDirSpec::operator new(nSize, lpszFileName, nLine);
	}
#endif	

void COXPathSpec::operator delete(void* p)
	{
	COXDirSpec::operator delete(p);
	}
	
BOOL COXPathSpec::operator==(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator==(pathSpec) &&
		COXFileSpec::operator==(pathSpec);
	}

BOOL COXPathSpec::operator!=(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator!=(pathSpec) ||
		COXFileSpec::operator!=(pathSpec);
	}

BOOL COXPathSpec::operator<=(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator<=(pathSpec) &&
		COXFileSpec::operator<=(pathSpec);
	}
BOOL COXPathSpec::operator<(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator<(pathSpec) &&
		COXFileSpec::operator<(pathSpec);
	}
BOOL COXPathSpec::operator>=(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator>=(pathSpec) &&
		COXFileSpec::operator>=(pathSpec);
	}
BOOL COXPathSpec::operator>(const COXPathSpec& pathSpec) const
	{
	return COXDirSpec::operator>(pathSpec) &&
		COXFileSpec::operator>(pathSpec);
	}

#ifdef _DEBUG
void COXPathSpec::Dump(CDumpContext& dc) const
	{
	COXDirSpec::Dump(dc);
	COXFileSpec::Dump(dc);
	}

void COXPathSpec::AssertValid() const
	{
	COXDirSpec::AssertValid();
	COXFileSpec::AssertValid();
	}
#endif

COXPathSpec::~COXPathSpec()
	{
	}
	
// protected:
HMODULE COXPathSpec::GetThisModule()
	// --- In  : 
	// --- Out : 
	// --- Returns : The module handle of the running task
	// --- Effect : 
	{
	HMODULE hThisModule = NULL;

#ifdef WIN32
	hThisModule = GetModuleHandle(NULL);
#else	
	// WIN16 does not have a direct function which returns the module handle of the running application
	// Therefor TOOLHELP is used to calculate it from the running task
    HTASK hCurrentTask;
    TASKENTRY taskEntry;
    taskEntry.dwSize = sizeof(taskEntry);
    hCurrentTask = GetCurrentTask();
    if (hCurrentTask == NULL)
            return NULL;
    TaskFindHandle(&taskEntry, hCurrentTask);
	hThisModule = taskEntry.hModule;

	// An alternative (but undocumented) ways is the following
	// HMODULE hThisModule2 = (HMODULE)*(LPWORD)MAKELP(GetCurrentTask(), 0x1E);
#endif
	
	return hThisModule;
	}

BOOL COXPathSpec::SearchEnvironment(LPCTSTR pszFileName, LPCTSTR pszVarName, LPTSTR pszPathName)
	// --- In  : pszFileName : The name of the file to search for
	//			 pszVarName : The name of the environment variable
	// --- Out : pszPathName : The full path of the file if found, otherwise empty
	// --- Returns : Whether the specified file was found
	// --- Effect : Searches all the directories of the specified environment variable
	//				for the specified file
	//				An environment variable is case-sensitive
	// --- Remark : This function is an implementation of the C-RunTime function _searchenv
	//				 This function also works when called from a DLL, which _searchenv does not
	{
	const int nMaxEntriesLength(300);
	TCHAR szEntries[nMaxEntriesLength + 1];
	LPTSTR pszToken;
	DWORD dwEntriesLength;
	
	// ... Assume failure, so initialize out-parameter
	ASSERT(pszPathName != NULL);
	ASSERT(AfxIsValidAddress(pszPathName, 1));
	*pszPathName = _T('\0');	
	
	// ... First get the environment value
	dwEntriesLength = GetEnvironmentVar(pszVarName, szEntries, nMaxEntriesLength);
	if (dwEntriesLength == 0)
		// ... Failure : Environment variable does not exist
		return FALSE;
#ifdef _DEBUG	
	if (nMaxEntriesLength < dwEntriesLength)
		TRACE(_T("COXPathSpec::SearchEnv : Environment variable value length (%u) exceeds maximum length (%u) and will be truncated\n"),
			dwEntriesLength, nMaxEntriesLength);
#endif	
	
	// Iterate all the directory entries, which are seperated by a semi-colon
	BOOL bFound = FALSE;
	CFileStatus fileStatus;
	TCHAR szPath[_MAX_PATH];
	LPTSTR pszConcat;
	TCHAR * nextToken;
	pszToken = UTBStr::tcstok(szEntries, _T(";"), &nextToken);
	while (!bFound && (pszToken != NULL))
		{
			UTBStr::tcscpy(szPath, _MAX_PATH, pszToken);
		// ... String cannot be empty because (pszToken != '\0')
		ASSERT(1 <= _tcsclen(szPath));
		// ... Examine the last char of the directory spec, 
		//     if it is not back slash and not a colon (relative path), add a back slash
		pszConcat = &szPath[_tcsclen(szPath) - 1];
		if ( (*pszConcat != _T('\\')) && (*pszConcat != _T(':')) )
			*(++pszConcat) = _T('\\');
		// ... Position after last char of directory spec
		pszConcat++;
		// ... Add file name
		UTBStr::tcscpy(pszConcat, _tcsclen(szPath) - 1, pszFileName);
		bFound = CFile::GetStatus(szPath, fileStatus);

		TCHAR * nextToken;
		pszToken = UTBStr::tcstok(NULL, _T(";"), &nextToken);
		}
	if (bFound)
	{
		ASSERT(AfxIsValidAddress(pszPathName, _tcsclen(szPath) + 1));
		UTBStr::tcscpy(pszPathName, _MAX_PATH, szPath);
	}
	return bFound;
	}

DWORD COXPathSpec::GetEnvironmentVar(LPCTSTR pszVarName, LPTSTR pszValue, DWORD nLength)
	// --- In  : pszVarName : The name of the environment variable 
	//			 pszValue : Buffer into which the result will be stored
	//			 nLength : The length of the buffer
	// --- Out : pszValue : The result (the environment table entry containing 
	//				 the current string value of pszVarName)
	// --- Returns : The number of characters stored into the buffer pointed to by pszValue, 
	//				  not including the terminating null character. 
	//				 When the variable name was not found the return value is zero. 
	//				 If the buffer pointed to by pszValue is not large enough, 
	//				  the return value is the buffer size, in characters, 
	//				  required to hold the value string and its terminating null character. 
	// --- Effect : Searches the list of environment variables for the specified entry
	//				An environment variable is case-sensitive
	// --- Remark : This function is an implementation of the C-RunTime function getenv
	//				 This function also works when called from a DLL, which in WIN16 getenv does not
	//				See Also MS Developers Network Q78542 : 
	//				 Retreiving MS-DOS Environment Vars from a Windows DLL
	{
#ifdef WIN32
	// Functionality exists in WIN32, so just call Windows API function
	return ::GetEnvironmentVariable(pszVarName, pszValue, nLength);
#else	
	// ... Assume failure, so initialize out-parameter
	ASSERT(pszValue != NULL);
	ASSERT(AfxIsValidAddress(pszValue, (UINT)nLength));
	*pszValue = _T('\0');	

	LPTSTR lpEnvSearch;
	LPCTSTR lpszVarSearch;
	
	// ... Check for empty var
	if (*pszVarName == _T('\0'))
		{
		TRACE(_T("COXPathSpec::GetEnvironmentVar : Empty environment variable, returning 0\n"));
    	return 0;
    	}
    	
	//  ... Get a pointer to the MS-DOS environment block
	lpEnvSearch = GetDOSEnvironment();
	// ... Iterat all strings in the environment table
	while (*lpEnvSearch != _T('\0'))
		{
        //  ... Check to see if the variable names match
    	lpszVarSearch = pszVarName;
    	while ( (*lpEnvSearch != _T('\0')) && (*lpszVarSearch != _T('\0')) &&
                (*lpEnvSearch == *lpszVarSearch) )
	    	{
	      	lpEnvSearch++;
	      	lpszVarSearch++;
	      	}
        // ... If the names match, the lpEnvSearch pointer is on the "="
        //     character and lpszVarSearch is on a null terminator.
        //     Increment and return lpszEnvSearch, which will point to the
        //     environment variable's contents.
		if ((*lpEnvSearch == _T('=')) && (*lpszVarSearch == _T('\0')))
			{
			lpEnvSearch++;
			strncpy(pszValue, lpEnvSearch, (size_t)nLength);
			pszValue[nLength] = _T('\0');
			return _tcsclen(lpEnvSearch);
			}
        // ... If the names do not match, increment lpEnvSearch until it
        //     reaches the end of the current variable string.
	    else
      		while (*lpEnvSearch != _T('\0'))
        		lpEnvSearch++;
		// ... At this point the end of the environment variable's string
        // 	   has been reached. Increment lpEnvSearch to move to the
        //     next variable in the environment block. If it is NULL,
        //     the end of the environment block has been reached.
	    lpEnvSearch++;
	    }
	    
	// If this section of code is reached, the variable was not found.
	TRACE(_T("COXPathSpec::GetEnvironmentVar : Environment variable (%s) not found, returning NULL\n"), pszVarName);
  	return 0; 
#endif // WIN32  	
	}
	
BOOL COXPathSpec::GetFirstFile(COXPathIterator& FIterator) const
	{
	COXPathSpec searchPath(*this);
	if (searchPath.GetFileName().IsEmpty())
		VERIFY(searchPath.SetFileName(_T("*.*")));

	BOOL bGoodFileFound = FALSE;	
	BOOL bJustFileFound = TRUE;	
    FIterator.m_bValid = TRUE;

#ifdef WIN32
    FIterator.m_hFindFile = FindFirstFile(searchPath.GetPath(), &FIterator.m_FindFileData);

	if (FIterator.m_hFindFile == INVALID_HANDLE_VALUE)
    	{
    	FindClose(FIterator.m_hFindFile);
    	FIterator.m_hFindFile = NULL;
    	FIterator.m_bValid = FALSE;
    	}
	else
		{
		while (!bGoodFileFound && bJustFileFound)
			{
			if (!IsChildDir(&FIterator.m_FindFileData) &&
			   (FIterator.m_FindFileData.cFileName[0] != _T('.')))
				bGoodFileFound = TRUE;
			
			if (!bGoodFileFound)
				bJustFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
			}
		}
#else
	
    bGoodFileFound = !_dos_findfirst(searchPath.GetPath(), _A_NORMAL | _A_ARCH, &FIterator.m_FileInfo);
    	
#endif

   	FIterator.m_bValid = bGoodFileFound;
    return FIterator.m_bValid;
	}
	
COXFileSpec COXPathSpec::GetNextFile(COXPathIterator& FIterator) const
	{
	ASSERT_VALID(&FIterator);

	COXFileSpec ActualFile;

	BOOL bDirFound(TRUE);
#ifdef WIN32
	BOOL bFileFound;
	ActualFile.SetFileName(FIterator.m_FindFileData.cFileName);
	ActualFile.SetTime(CTime(FIterator.m_FindFileData.ftLastWriteTime));
	ASSERT(FIterator.m_FindFileData.nFileSizeHigh == 0);
	ActualFile.SetLength(FIterator.m_FindFileData.nFileSizeLow);
	ActualFile.SetAttributes((CFile::Attribute)FIterator.m_FindFileData.dwFileAttributes);

	bFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
	while (bFileFound && bDirFound)
		{
		if (!IsChildDir(&FIterator.m_FindFileData) &&
		   (FIterator.m_FindFileData.cFileName[0] != _T('.')))
			bDirFound = FALSE;
		
		if (bDirFound)
			bFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
			}
		
	if (!bFileFound)	
		{
    	FindClose(FIterator.m_hFindFile);
    	FIterator.m_hFindFile = NULL;
    	FIterator.m_bValid = FALSE;
		}

#else
	ActualFile.SetFileName(FIterator.m_FileInfo.name);
	ActualFile.SetTime(CTime((WORD)FIterator.m_FileInfo.wr_date, (WORD)FIterator.m_FileInfo.wr_time));
	ActualFile.SetLength(FIterator.m_FileInfo.size);
	ActualFile.SetAttributes((CFile::Attribute)FIterator.m_FileInfo.attrib);
	
	if (_dos_findnext(&FIterator.m_FileInfo) != 0)
		{
	   	FIterator.m_bValid = FALSE;
		}
#endif                

	return ActualFile;
	}
	
BOOL COXPathSpec::GetFirstDir(COXPathIterator& FIterator) const                 
	{
	COXPathSpec searchPath(*this);
	if (searchPath.GetFileName().IsEmpty())
		VERIFY(searchPath.SetFileName(_T("*.*")));

    FIterator.m_bValid = TRUE;
	BOOL bFileFound(TRUE);	
	BOOL bDirFound(FALSE);	

#ifdef WIN32
    FIterator.m_hFindFile = FindFirstFile(searchPath.GetPath(), &FIterator.m_FindFileData);

	if (FIterator.m_hFindFile == INVALID_HANDLE_VALUE)
    	{
    	FindClose(FIterator.m_hFindFile);
    	FIterator.m_hFindFile = NULL;
    	FIterator.m_bValid = FALSE;
    	}
	else
		{
		while (!bDirFound && bFileFound)
			{
			if (IsChildDir(&FIterator.m_FindFileData))
				bDirFound = TRUE;
			
			if (!bDirFound)
				bFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
			}
		}
    
#else
    bFileFound = !_dos_findfirst(searchPath.GetPath(), _A_NORMAL | _A_ARCH | _A_SUBDIR, &FIterator.m_FileInfo);
   	while (bFileFound && !bDirFound)
		{       
		if (((FIterator.m_FileInfo.attrib & _A_SUBDIR) == _A_SUBDIR) && (strcmp(FIterator.m_FileInfo.name, _T(".")) != 0) &&
			(strcmp(FIterator.m_FileInfo.name, _T("..")) != 0))
			bDirFound = TRUE;
		
		if (!bDirFound)	
			bFileFound = (_dos_findnext(&FIterator.m_FileInfo) == 0);
		}		
   	
#endif

   	FIterator.m_bValid = bDirFound;
    return FIterator.m_bValid;	
	}

COXDirSpec COXPathSpec::GetNextDir(COXPathIterator& FIterator) const
	{
	ASSERT_VALID(&FIterator);
	 
	COXDirSpec ActualDir;

	BOOL bFileFound;	
	BOOL bDirFound(FALSE);	

#ifdef WIN32
	ActualDir.SetDrive(m_sDrive);
	ActualDir.SetSubdirectory(m_sSubdirectory);
	ActualDir.AppendDirectory(COXDirSpec(FIterator.m_FindFileData.cFileName));

	bFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
	while (bFileFound && !bDirFound)
		{
		if (IsChildDir(&FIterator.m_FindFileData))
			bDirFound = TRUE;

		if (!bDirFound)
			bFileFound = FindNextFile(FIterator.m_hFindFile, &FIterator.m_FindFileData);
		}
		
	if (!bFileFound)	
		{
    	FindClose(FIterator.m_hFindFile);
    	FIterator.m_hFindFile = NULL;
    	FIterator.m_bValid = FALSE;
		}

#else

	ActualDir.SetDrive(m_sDrive);
	ActualDir.SetSubdirectory(m_sSubdirectory);
	ActualDir.AppendDirectory(COXDirSpec(FIterator.m_FileInfo.name));

	bFileFound = !_dos_findnext(&FIterator.m_FileInfo);
   	while (bFileFound && !bDirFound)
		{
		if (((FIterator.m_FileInfo.attrib & _A_SUBDIR) == _A_SUBDIR) && (strcmp(FIterator.m_FileInfo.name, _T(".")) != 0) &&
			(strcmp(FIterator.m_FileInfo.name, _T("..")) != 0))
			bDirFound = TRUE;
		
		if (!bDirFound)	
			bFileFound = (_dos_findnext(&FIterator.m_FileInfo) == 0);
		}		

   	FIterator.m_bValid = bDirFound;
#endif                

	return ActualDir;	
	}
	
// private:

// ==========================================================================
