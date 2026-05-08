// =============================================================================
// 							Class Implementation : COXResourceLibrary
// =============================================================================
//
// Source file : 		OXResourceLibrary.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXResourceLibrary.h"
#include "OXResourceFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXResourceLibrary, CObject)

// Data members -------------------------------------------------------------
// protected:
//	CString		m_sFileName;	the library file's name
//	DWORD		m_dwFlags;		the action to take when loading a library
//	HINSTANCE	m_hLib;			the handle of a loaded library; NULL if current module
//	HANDLE		m_hUpdateRes;	the update handle from BeginUpdateResource() 
//	CObList		m_ResFiles;		the list of all registered COXResoruceFile objects

// Member functions ---------------------------------------------------------
// public:
COXResourceLibrary::COXResourceLibrary()
	{
	m_hLib = NULL;
	m_hUpdateRes = NULL;
	}

COXResourceLibrary::~COXResourceLibrary()
	{
	Close();
	}

BOOL COXResourceLibrary::Open(LPCTSTR pszLibFileName, DWORD dwFlags /* = LOAD_LIBRARY_AS_DATAFILE */)
	{
	Close();

	if (pszLibFileName)
		{
		m_hLib = ::LoadLibraryEx(pszLibFileName, NULL, dwFlags);
		if (m_hLib)
			{
			m_sFileName = pszLibFileName;
			m_dwFlags = dwFlags;
			}
		else
			{
			TRACE1("COXResourceLibrary::Open(): failed to load \"%s\".\n",
					pszLibFileName);
			return FALSE;
			}
		}
	return TRUE;
	}

void COXResourceLibrary::Close()
	{
	if (m_hUpdateRes)
		{
		TRACE0("COXResourceLibrary::Close(): missing EndUpdate().\n");
		EndUpdate(FALSE, TRUE);
		}
	ASSERT(m_hUpdateRes == NULL);
	
#ifdef _DEBUG
	int nCount = PtrToInt(m_ResFiles.GetCount());
#endif
	while (!m_ResFiles.IsEmpty())
		{
		COXResourceFile* pResFile = (COXResourceFile*)m_ResFiles.GetHead();
		BOOL bAutoDelete = pResFile->IsAutoDeleteByLib();
		pResFile->SetResourceLibrary(NULL);
		if (bAutoDelete)
			delete pResFile;

		// we don't want infinite loop occurs
		ASSERT(m_ResFiles.GetCount() == --nCount);
		}

	ASSERT(nCount == 0);

	if (m_hLib)
		{
		VERIFY(::FreeLibrary(m_hLib));
		m_hLib = NULL;
		m_sFileName.Empty();
		}

	ASSERT(m_sFileName.IsEmpty());
	}

BOOL COXResourceLibrary::IsModifiable()
	{
	if (m_hUpdateRes)
		return TRUE;
	if (BeginUpdate())
		{
		// even though this is not an error msg, when BeginUpdate() was called, some
		// memory may be consumed and not released until FreeLibrary() is called
		// this is a strange behavior of Win32's EndUpdateResource() (even when you
		// specify fDiscard as TRUE). Therefore do NOT repeatedly call this function
		// (e.g. in your UpdateCmdUI() handlers)
		TRACE0("COXResourceLibrary::IsModifiable(): used BeginUpdate() and EndUpdate() to check.\n");
		return EndUpdate(FALSE, TRUE);
		}
	return FALSE;
	}

HRSRC COXResourceLibrary::FindResource(LPCTSTR pszType, LPCTSTR pszName,
	WORD nLanguage /* = OXRESOURCE_DEFLANGID */) const
	{
	// FindResourceEx will accept CString format
	HRSRC hRes = ::FindResourceEx(m_hLib, pszType, pszName, nLanguage);

#ifdef _AFXDLL
	// try once again with AfxFindResourceHandle to find resource in chain of 
	// extension DLLs
	if (hRes == NULL && m_hLib == NULL && 
		nLanguage == MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL))
		hRes = (HRSRC)AfxFindResourceHandle(pszName, pszType);
#endif

	return hRes;
	}

COXResourceFile* COXResourceLibrary::GetOpenedResFile(LPCTSTR pszType, 
	LPCTSTR pszName, WORD nLanguage /* = OXRESOURCE_DEFLANGID */) const
	{
	CString sType = COXResourceFile::OXResToCString(pszType);
	CString sName = COXResourceFile::OXResToCString(pszName);
	POSITION pos = m_ResFiles.GetHeadPosition();
	while (pos)
		{
		COXResourceFile* pResFile = (COXResourceFile*)m_ResFiles.GetNext(pos);
		if (pResFile->GetResType() == sType &&
			pResFile->GetResName() == sName &&
			pResFile->GetResLanguage() == nLanguage )
			return pResFile;
		}
	return NULL;
	}

BOOL COXResourceLibrary::DeleteResource(LPCTSTR pszType, LPCTSTR pszName,
		WORD nLanguage /* = OXRESOURCE_DEFLANGID */, BOOL bInstant /* = FALSE */)
	{
	if (m_hUpdateRes) // BeginUpdate() has been called already, neglect bInstant
		return Update(NULL, 0, pszType, pszName, nLanguage);
	else if (bInstant) // single resource instant update
		return (BeginUpdate() && 
				Update(NULL, 0, pszType, pszName, nLanguage) &&
				EndUpdate());
	else
		{
		COXResourceFile* pResFile = new COXResourceFile;
		pResFile->SetResType(pszType);
		pResFile->SetResName(pszName);
		pResFile->SetResLanguage(nLanguage);
		pResFile->SetResourceLibrary(this, TRUE);
		}
	return TRUE;
	}

BOOL COXResourceLibrary::BeginUpdate(BOOL bDeleteExistingResources /* = FALSE */)
	{
	if (m_hUpdateRes)
		{
		TRACE0("COXResourceLibrary::BeginUpdate(): missing EndUpdate().\n");
		return FALSE;
		}

	if (m_hLib == NULL)
		{
		TRACE0("COXResourceLibrary::BeginUpdate(): current module is not modifiable.\n");
		return FALSE;
		}

	m_hUpdateRes = ::BeginUpdateResource(m_sFileName, bDeleteExistingResources);
	return (m_hUpdateRes != NULL);
	}

BOOL COXResourceLibrary::Update(BYTE* lpBuffer, DWORD dwSize, LPCTSTR pszType, 
								LPCTSTR pszName, WORD nLanguage /* = OXRESOURCE_DEFLANGID */)
	{
	ASSERT(lpBuffer == NULL || AfxIsValidAddress(lpBuffer, dwSize));

	if (m_hUpdateRes == NULL)
		{
		TRACE0("COXResourceLibrary::Update(): missing BeginUpdate().\n");
		return FALSE;
		}

	// UpdateResource() only accepts item format
	WORD nTypeID = COXResourceFile::OXResToInt(pszType);
	LPCTSTR pszItemType = nTypeID ? MAKEINTRESOURCE(nTypeID) : pszType;
	WORD nNameID = COXResourceFile::OXResToInt(pszName);
	LPCTSTR pszItemName = nNameID ? MAKEINTRESOURCE(nNameID) : pszName;

	if (lpBuffer == NULL || dwSize == 0)
		{
		// It is better to trace only in case of error
		// TRACE0("COXResourceLibrary::Update(): action is \"delete\".\n");
#ifdef _DEBUG
		if (!FindResource(pszItemType, pszItemName, nLanguage))
			TRACE0("COXResourceLibrary::Update(): the resource to delete was not found.\n");
#endif
		return ::UpdateResource(m_hUpdateRes, pszItemType, pszItemName, 
			nLanguage, NULL, 0);
		}

#ifdef _DEBUG
	// It is better to trace only in case of error
#endif

	return ::UpdateResource(m_hUpdateRes, pszItemType, pszItemName, 
		nLanguage, lpBuffer, dwSize);
	}

BOOL COXResourceLibrary::EndUpdate(BOOL bFlushAll /* = FALSE */, BOOL bDiscard /* = FALSE */)
	{
	if (m_hUpdateRes == NULL)
		{
		TRACE0("COXResourceLibrary::EndUpdate(): missing BeginUpdate().\n");
		return FALSE;
		}

	BOOL bSuccess;
	if (bDiscard)
		bSuccess = ::EndUpdateResource(m_hUpdateRes, TRUE);
	else
		{
		if (bFlushAll) FlushAll();
		// we need to free the library when writing back, otherwise a .tmp file
		// will be created to store the updated resources instead of writing onto
		// the original file
		ASSERT(m_hLib);
		VERIFY(::FreeLibrary(m_hLib));
		bSuccess = ::EndUpdateResource(m_hUpdateRes, FALSE);
		m_hLib = ::LoadLibraryEx(m_sFileName, NULL, m_dwFlags);
		ASSERT(m_hLib);
		}

	m_hUpdateRes = NULL;
	return bSuccess;
	}

struct OXResLib_EnumParam
	{
	CStringArray*	pResTypes;
	CStringArray*	pResNames;
	CWordArray*		pResLangs;
	};

BOOL CALLBACK OXResLib_EnumResLangProc(HANDLE /* hModule */, LPCTSTR lpszType, 
									   LPCTSTR lpszName, WORD wIDLanguage, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	OXResLib_EnumParam* pEnumParam = (OXResLib_EnumParam*)lParam;
	ASSERT(pEnumParam->pResLangs);

	if (pEnumParam->pResTypes)
		pEnumParam->pResTypes->Add(COXResourceFile::OXResItemToCString(lpszType));
	if (pEnumParam->pResNames)
		pEnumParam->pResNames->Add(COXResourceFile::OXResItemToCString(lpszName));
	pEnumParam->pResLangs->Add(wIDLanguage);
	return TRUE;
}

BOOL CALLBACK OXResLib_EnumResNameProc(HANDLE hModule, LPCTSTR lpszType, 
									   LPTSTR lpszName, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	OXResLib_EnumParam* pEnumParam = (OXResLib_EnumParam*)lParam;
	ASSERT(pEnumParam->pResNames);

	// current Win32 doesn't really remove a resource when deleting, instead it
	// sets its name to #0, therefore, we need to skip this kind of resources;
	// however, be aware of a fact: MS Developer Studio's resource editor doesn't
	// enumerate well when this occurs
	if ((DWORD_PTR)lpszName == 0)
		return TRUE;

	if (pEnumParam->pResLangs)
		return ::EnumResourceLanguages((HINSTANCE)hModule, lpszType, lpszName, 
		(ENUMRESLANGPROC)OXResLib_EnumResLangProc, lParam);
	else
	{
		if (pEnumParam->pResTypes)
			pEnumParam->pResTypes->Add(COXResourceFile::OXResItemToCString(lpszType));
		pEnumParam->pResNames->Add(COXResourceFile::OXResItemToCString(lpszName));
	}
	
	return TRUE;
}

BOOL CALLBACK OXResLib_EnumResTypeProc(HANDLE hModule, LPTSTR lpszType, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	OXResLib_EnumParam* pEnumParam = (OXResLib_EnumParam*)lParam;
	ASSERT(pEnumParam->pResTypes);

	if (pEnumParam->pResNames)
		return ::EnumResourceNames((HINSTANCE)hModule, lpszType, 
		(ENUMRESNAMEPROC)OXResLib_EnumResNameProc, lParam);
	else
		pEnumParam->pResTypes->Add(COXResourceFile::OXResItemToCString(lpszType));

	return TRUE;
}

BOOL COXResourceLibrary::EnumResources(CStringArray* pResTypes, CStringArray* pResNames,
									  CWordArray* pResLangs /* = NULL */, 
									  int nGrowBy /* = -1 */)
	{
	ASSERT(pResTypes && !(!pResNames && pResLangs));
	if (pResTypes == NULL)
		return FALSE;

	pResTypes->SetSize(0, nGrowBy);
	if (pResNames)
		pResNames->SetSize(0, nGrowBy);
	if (pResLangs)
		pResLangs->SetSize(0, nGrowBy);
	
	OXResLib_EnumParam enumParam = {pResTypes, pResNames, pResLangs};
	return ::EnumResourceTypes(m_hLib, 
		(ENUMRESTYPEPROC)OXResLib_EnumResTypeProc, (LONG_PTR)&enumParam);
	}

BOOL COXResourceLibrary::EnumResources(LPCTSTR lpszType, CStringArray* pResNames,
									  CWordArray* pResLangs /* = NULL */, 
									  int nGrowBy /* = -1 */)
	{
	ASSERT(lpszType && pResNames);
	if (pResNames == NULL)
		return FALSE;

	pResNames->SetSize(0, nGrowBy);
	if (pResLangs)
		pResLangs->SetSize(0, nGrowBy);

	OXResLib_EnumParam enumParam = {NULL, pResNames, pResLangs};

	// EnumResourceNames() only accepts item format
	WORD nTypeID = COXResourceFile::OXResToInt(lpszType);
	LPCTSTR pszItemType = nTypeID ? MAKEINTRESOURCE(nTypeID) : lpszType;

	return ::EnumResourceNames(m_hLib, pszItemType, 
		(ENUMRESNAMEPROC)OXResLib_EnumResNameProc, (LONG_PTR)&enumParam);
	}

BOOL COXResourceLibrary::EnumResources(LPCTSTR lpszType, LPCTSTR lpszName,
									   CWordArray* pResLangs, int nGrowBy /* = -1 */)
	{
	ASSERT(lpszType && lpszName && pResLangs);
	if (pResLangs == NULL)
		return FALSE;

	pResLangs->SetSize(0, nGrowBy);

	OXResLib_EnumParam enumParam = {NULL, NULL, pResLangs};

	// EnumResourceLanguages() only accepts item format
	WORD nTypeID = COXResourceFile::OXResToInt(lpszType);
	LPCTSTR pszItemType = nTypeID ? MAKEINTRESOURCE(nTypeID) : lpszType;
	WORD nNameID = COXResourceFile::OXResToInt(lpszName);
	LPCTSTR pszItemName = nNameID ? MAKEINTRESOURCE(nNameID) : lpszName;

	return ::EnumResourceLanguages(m_hLib, pszItemType, pszItemName, 
		(ENUMRESLANGPROC)OXResLib_EnumResLangProc, (LONG_PTR)&enumParam);
	}

// protected:
void COXResourceLibrary::Register(COXResourceFile* pResFile)
// --- In      : pResFile, pointer to the COXResourceFile object to register
// --- Out     : 
// --- Returns :
// --- Effect  : add a COXResourceFile to the registree list
//				 (called by COXResourceFile::SetResourceLibrary() only)
	{
	ASSERT(pResFile);

	if (pResFile && pResFile->GetResourceLibrary() != this)
		{
		ASSERT(m_ResFiles.Find(pResFile) == NULL);
		m_ResFiles.AddTail(pResFile);
		}
	}

void COXResourceLibrary::Unregister(COXResourceFile* pResFile)
// --- In      : pResFile, pointer to the COXResourceFile object to unregister
// --- Out     : 
// --- Returns :
// --- Effect  : delete a COXResourceFile to the registree list
//				 (called by COXResourceFile::SetResourceLibrary() only)
	{
	ASSERT(pResFile);

	if (pResFile && pResFile->GetResourceLibrary() == this)
		{
		POSITION pos = m_ResFiles.Find(pResFile);
		ASSERT(pos);
		m_ResFiles.RemoveAt(pos);
		}
	}

void COXResourceLibrary::FlushAll()
// --- In      :
// --- Out     : 
// --- Returns :
// --- Effect  : flush all COXResourceFile in the registree list
	{
	POSITION pos = m_ResFiles.GetHeadPosition();
	while (pos)
		((COXResourceFile*)m_ResFiles.GetNext(pos))->Flush();
	}

// end of OXResourceLibrary.cpp
