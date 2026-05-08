// =============================================================================
// 							Class Implementation : COXResourceFile
// =============================================================================
//
// Source file : 		OXResourceFile.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXResourceFile.h"
#include "OXResourceLibrary.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define OXRES_MUN_STRING_ATTEMPT _T("Resource");

IMPLEMENT_DYNAMIC(COXResourceFile, CSharedFile)

// Data members -------------------------------------------------------------
// protected:
//	COXResourceLibrary* m_pResLib;	the target library if updating
//	BOOL	m_bAutoDeleteByLib;		whether deleted by COXResourceLibrary when
//									it is closed
//	BOOL	m_bFlushOnClose;		whether flush on close
//	CString m_sResType;				the type of the resource
//	CString m_sResName;				the name of the resource
//	WORD	m_nResLanguage;			the language of the resource

// Member functions ---------------------------------------------------------
// public:
COXResourceFile::COXResourceFile(UINT nAllocFlags /* = GMEM_DDESHARE | GMEM_MOVEABLE */, 
								 UINT nGrowBytes /* = 4096 */) 
	: CSharedFile(nAllocFlags, nGrowBytes)
	{
	m_pResLib = NULL;
	m_bAutoDeleteByLib = FALSE;
	m_bFlushOnClose = TRUE;

	// set default values for an empty resource
	SetResType(RT_RCDATA);
	SetResLanguage(OXRESOURCE_DEFLANGID);
	}

COXResourceFile::~COXResourceFile()
	{
	if (m_pResLib || m_lpBuffer)
		Close();
	}

BOOL COXResourceFile::Open(UINT nOpenFlags, COXResourceLibrary* pResLib, BOOL bAutoDeleteByLib, 
	LPCTSTR pszType, LPCTSTR pszName, WORD nLanguage /* = OXRESOURCE_DEFLANGID */, 
	BOOL bMakeUniqueName /* = FALSE */)
	{
	// ... Cannot open for write only (use either modeRead or modeReadWrite).
	//     (Note that modeRead = 0x0000)
	ASSERT((nOpenFlags & modeWrite) != modeWrite);
	// ... Can only create a resource if opened for read/write
	ASSERT( ((nOpenFlags & modeCreate) != modeCreate) ||
		    ((nOpenFlags & modeReadWrite) == modeReadWrite) );
	// ... Can only disable truncating if the resource is to be created
	ASSERT( ((nOpenFlags & modeNoTruncate) != modeNoTruncate) ||
		    ((nOpenFlags & modeCreate) == modeCreate) );

	ASSERT(pResLib && pszType && pszName);

	// open once only
	ASSERT(m_pResLib == NULL && m_lpBuffer == NULL);
	if (m_pResLib || m_lpBuffer) 
		return FALSE;

	BOOL bSuccess = TRUE;
	BOOL bFoundExisting = FALSE;

	// We search for the resource if (!modeCreate) or (modeCreate & modeNoTruncate)
	if (((nOpenFlags & modeCreate) != modeCreate) ||
	    ((nOpenFlags & modeNoTruncate) == modeNoTruncate) )
		{
		HRSRC hRes = pResLib->FindResource(pszType, pszName, nLanguage);
		if (hRes)
			{
			bFoundExisting = TRUE;

			HGLOBAL hResData = pResLib->LoadResource(hRes);
			DWORD nSize = pResLib->GetResourceSize(hRes);
			if (hResData && nSize)
				{
				BYTE* lpBuffer = (BYTE*)::LockResource(hResData);
				if (lpBuffer)
					{
					Write(lpBuffer, nSize);
					SeekToBegin();
					}
				else
					{
					bSuccess = FALSE;
					TRACE1("COXResourceFile::Open(): failed to lock the resource \"%s\".\n", pszName);
					}
				}
			else
				{
				bSuccess = FALSE;
				TRACE1("COXResourceFile::Open(): failed to load the resource \"%s\".\n", pszName);
				}
			}
		else 
			{
			// The resource was not found, fail if no modeCreate is specified
			bSuccess = ((nOpenFlags & modeCreate) == modeCreate);
			if (!bSuccess)
				TRACE1("COXResourceFile::Open(): failed to find the resource \"%s\".\n", pszName);
			}
		}

	if (bSuccess)
		{
		SetResType(pszType);
		SetResName(pszName);
		SetResLanguage(nLanguage);
		SetResourceLibrary(pResLib, bAutoDeleteByLib, 
			(bFoundExisting ? FALSE : bMakeUniqueName));
		m_bFlushOnClose = ((nOpenFlags & modeReadWrite) == modeReadWrite);
		}

	return bSuccess;
	}

void COXResourceFile::Close()
	{
	if (m_bFlushOnClose) 
		Flush();
	Abort();
	}

void COXResourceFile::Abort()
	{
	SetResourceLibrary(NULL);

	if (m_lpBuffer)
		CSharedFile::Close();
	}

void COXResourceFile::Flush()
	{
	if (m_pResLib == NULL || !m_bFlushOnClose)
		return;

	if(m_pResLib->GetUpdateHandle())
		{ // BeginUpdate() has been called already
		VERIFY(m_pResLib->Update(m_lpBuffer, PtrToUint(m_nFileSize), m_sResType, 
			m_sResName, m_nResLanguage));
		}
	else
		{ // single resource instant update
		if (m_pResLib->BeginUpdate())
			{
			VERIFY(m_pResLib->Update(m_lpBuffer, PtrToUint(m_nFileSize), m_sResType, 
				m_sResName, m_nResLanguage));
			m_pResLib->EndUpdate();
			}
		else
			TRACE0("COXResourceFile::Flush(): failed to begin an update.\n");
		}
	}

HGLOBAL COXResourceFile::DetachEx()
// Even though COXResourceFile::Detach() can return an HGLOBAL that is ready
// for clipboard operations, it does NOT contain the type, name and language
// info we need (it only contains the raw binary data block). To solve
// this problem, we modify the binary block a little bit:
//
//	Original data: XX XX XX XX xx xx xx xx
//	<1> we duplicate the first 4 bytes (1 DWORD size) at the end:
//				   XX XX XX XX xx xx xx xx XX XX XX XX
//	<2> we write down the file size into the first 1 DWORD:
//				   00 00 00 08 xx xx xx xx XX XX XX XX
//	<3> we append type, name, lang info at the end
//				   00 00 00 08 xx xx xx xx XX XX XX XX (type)(name)(lang)
//	Now, when we decode it using SetHandleEx(), we know where to fetch the type,
//	name and lang, and we'll put the original 4 bytes back, then set file size back.
	{
	ASSERT(m_hGlobalMemory && m_bAllowGrow);
	if (m_hGlobalMemory == NULL || !m_bAllowGrow)
		return NULL;

	if (m_nBufferSize < sizeof(DWORD))
		GrowFile(sizeof(DWORD));

	DWORD_PTR nFileSize0 = m_nFileSize;
	DWORD nHeadBlock0 = *((DWORD*)m_lpBuffer);

	SeekToBegin();
	Write((BYTE*)&nFileSize0, sizeof(DWORD));

	SeekToEnd();
	CArchive ar(this, CArchive::store);
	ar << nHeadBlock0 << m_sResType << m_sResName << m_nResLanguage;
	ar.Close();

	m_bFlushOnClose = FALSE;
	return CSharedFile::Detach();
	}

BOOL COXResourceFile::SetHandleEx(HGLOBAL hTaggedResData, BOOL bAllowGrow /* = TRUE */)
	{
	ASSERT(hTaggedResData);
	ASSERT(m_hGlobalMemory == NULL);

	if (m_hGlobalMemory)
		return FALSE;

	SetHandle(hTaggedResData, bAllowGrow);
	if (m_nBufferSize < sizeof(DWORD))
		{
		TRACE0("COXResourceFile::SetHandleEx(): 1st parameter invalid.\n");
		return FALSE;
		}

	DWORD nFileSize0, nHeadBlock0;

	SeekToBegin();
	Read((BYTE*)&nFileSize0, sizeof(DWORD));

	if (nFileSize0 >= m_nFileSize)
		{
		TRACE0("COXResourceFile::SetHandleEx(): invalid memory block.\n");
		return FALSE;
		}

	Seek(nFileSize0, begin);

	CArchive ar(this, CArchive::load);
	try
		{
		ar >> nHeadBlock0 >> m_sResType >> m_sResName >> m_nResLanguage;
		}
	catch (CArchiveException* e)
		{
		e->Delete();
		TRACE0("COXResourceFile::SetHandleEx(): wrong tag format.\n");
		return FALSE;
		}
	catch (CFileException* e)
		{
		e->Delete();
		AfxThrowMemoryException();
		}
	
	ar.Close();

	SeekToBegin();
	Write((BYTE*)&nHeadBlock0, sizeof(DWORD));

	SetLength(nFileSize0);
	SeekToBegin();
	
	ASSERT_VALID(this);

	return TRUE;
	}

BOOL COXResourceFile::SetResourceLibrary(COXResourceLibrary* pResLib,
		BOOL bAutoDeleteByLib /* = FALSE */, BOOL bMakeUniqueName /* = FALSE */)
	{
	if (m_pResLib) 
		m_pResLib->Unregister(this);
	
	if (pResLib)
		{
		if (bMakeUniqueName)
			VERIFY(MakeUniqueName(pResLib));
		pResLib->Register(this);
		}

	m_pResLib = pResLib;
	m_bAutoDeleteByLib = bAutoDeleteByLib;
	return TRUE;
	}

// protected:
BOOL COXResourceFile::MakeUniqueName(COXResourceLibrary* pSearchLibrary /* = NULL */)
// --- In      : pSearchLibrary, the library in which to search for duplicate
// --- Out     : 
// --- Returns : TRUE if successful; FALSE otherwise
// --- Effect  : rename this resource to make sure it has a unique type + name + language
//				 identifiers
	{
	if (pSearchLibrary == NULL)
		pSearchLibrary = m_pResLib;

	ASSERT(pSearchLibrary);
	if (pSearchLibrary == NULL)
		return FALSE;

	WORD nAttempt;

	// number attack
	nAttempt = OXResCStringToInt(m_sResName);
	if (nAttempt)
		{
		for (WORD nCount = 0; nCount < 0xFFFF; nCount++)
			{
			if (pSearchLibrary->FindResource(m_sResType, m_sResName, m_nResLanguage) == NULL &&
				pSearchLibrary->GetOpenedResFile(m_sResType, m_sResName, m_nResLanguage) == NULL )
				return TRUE;

			if (nAttempt == 0xFFFF)
				nAttempt = 1;
			else
				nAttempt++;
			
			m_sResName = OXResIntToCString(nAttempt);
			}
		}

	// when number attack fails, try string attack
	CString sAttempt = OXResCStringToString(m_sResName);
	if (sAttempt.IsEmpty())
		sAttempt = OXRES_MUN_STRING_ATTEMPT;

	for (int i = sAttempt.GetLength(); i > 0; i--)
		{
		sAttempt = sAttempt.Left(i);
		nAttempt = 1;
		CString sNumber;
		for (DWORD dwCount = 0; dwCount < 0xFFFFFFFF; dwCount++)
			{
			sNumber.Format(_T("%d"), ++nAttempt);
			m_sResName = sAttempt + sNumber;
			if (pSearchLibrary->FindResource(m_sResType, m_sResName, m_nResLanguage) == NULL &&
				pSearchLibrary->GetOpenedResFile(m_sResType, m_sResName, m_nResLanguage) == NULL )
				return TRUE;
			}
		}

	TRACE0("COXResourceFile::MakeUniqueName(): all attempts failed.\n");
	return FALSE;
	}
	
CString COXResourceFile::ValidateResString(LPCTSTR pszTypeOrName)
// --- In      : pszTypeOrName, the type or name string that needs to be validated
// --- Out     : 
// --- Returns : the result type or name in CString format
// --- Effect  : this function will check the validity of a type or name parameter,
//				 and format it to make sure the resulted string will work with resource
//				 functions. Specifically:
//					(1) all lower letters will be made upper;
//					(2) all leading '#' will be replaced by '_', if no numbers follow it;
//					(3) all white space will be replaced by '_'.
	{
	WORD nID = OXResToInt(pszTypeOrName);
	if (nID)
		return OXResIntToCString(nID);

	CString sResult = pszTypeOrName;
	int nLen = sResult.GetLength();

	sResult.MakeUpper();

	int i = 0;
	while (i < nLen && sResult[i] == _T('#'))
		sResult.SetAt(i++, _T('_'));

	while (i < nLen)
		{
		switch (sResult[i])
			{
			case _T(' '):
			case _T('\t'):
			case _T('\n'):
				sResult.SetAt(i, _T('_'));
			}
		i++;
		}
	return sResult;
	}

// type/name format conversion functions
CString COXResourceFile::OXResIntToCString(WORD nID)
	{
	CString sResult;
	sResult.Format(_T("#%u"), nID);
	return sResult;
	}

WORD COXResourceFile::OXResToInt(LPCTSTR lpszTypeOrName)
	{
	WORD nID = OXResItemToInt(lpszTypeOrName);
	if (nID) 
		return nID;
	return OXResCStringToInt(lpszTypeOrName);
	}

CString COXResourceFile::OXResToCString(LPCTSTR lpszTypeOrName)
	{
	WORD nID = OXResToInt(lpszTypeOrName);

	if (nID)
		return OXResIntToCString(nID);
	else
		return lpszTypeOrName;
	}

// end of OXResourceFile.cpp

