// ==========================================================================
//				Class Implementation : COXSplitterColRowState
// ==========================================================================

// Source file : OXSplitterColRowState.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#if (_MFC_VER < 0x0420)
// This file uses classes that were introduced in MFC Version 4.2
// These classes are now officially documented by Microsoft, but did not exist in previous versions
// Therefore this file will be completely excluded for older versions of MFC
#pragma message("Warning : OXSplitterColRowState.cpp not included because MFC Version < 4.2")
#else
// The entire file


#include "OXSplitterColRowState.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int COXSplitterColRowState::m_nSerializeSchemaVersion = 1;

IMPLEMENT_SERIAL(COXSplitterColRowState, CObject, COXSplitterColRowState::m_nSerializeSchemaVersion | VERSIONABLE_SCHEMA)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:
	// BOOL m_bRow;
	// --- Whether this is the property of a row (TRUE) or column (FALSE)

	// int m_nColRowIndex;
	// --- The index row (or column)

	// int m_nIdealSize;
	// --- The size set by the user (ideal size upon restore)

	// int m_nMinSize;
	// --- Size below which the pane should not be shown

	// int m_nSerializeSchemaVersionLoad;
	// --- The schema version number that is read from archive

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXSplitterColRowState::COXSplitterColRowState()
	{
	Initialize();

	ASSERT_VALID(this);
	}

BOOL COXSplitterColRowState::ComputeProperties(CSplitterWnd* pSplitterWnd, int nColRowIndex, BOOL bRow)
	{
	ASSERT_VALID(this);

	if (pSplitterWnd == NULL)
		{
		TRACE0("COXSplitterColRowState::ComputeProperties : No splitter window, failing\n");
		return FALSE;
		}

	m_bRow = bRow;
	m_nColRowIndex = nColRowIndex;
	if (bRow)
		pSplitterWnd->GetRowInfo(nColRowIndex, m_nIdealSize, m_nMinSize);
	else
		pSplitterWnd->GetColumnInfo(nColRowIndex, m_nIdealSize, m_nMinSize);


	ASSERT_VALID(this);
	return TRUE;
	}

BOOL COXSplitterColRowState::ApplyProperties(CSplitterWnd* pSplitterWnd) const
	{
	ASSERT_VALID(this);

	if (pSplitterWnd == NULL)
		{
		TRACE0("COXSplitterColRowState::ApplyProperties : No splitter window, failing\n");
		return FALSE;
		}

	if (m_bRow)
		pSplitterWnd->SetRowInfo(m_nColRowIndex, m_nIdealSize, m_nMinSize);
	else
		pSplitterWnd->SetColumnInfo(m_nColRowIndex, m_nIdealSize, m_nMinSize);

	ASSERT_VALID(this);
	return TRUE;
	}

void COXSplitterColRowState::Serialize(CArchive& ar)
	{
	ASSERT_VALID(this);

	// Check the version 
	// (If version == -1, the version is unknown, this occurs when Serialize() is called directly)
	if (ar.IsLoading())
		{
		m_nSerializeSchemaVersionLoad = (int)ar.GetObjectSchema();
		if (m_nSerializeSchemaVersion < m_nSerializeSchemaVersionLoad)
			{
			TRACE1("COXSplitterColRowState::Serialize : Unexpected schema version : %i, throwing CArchiveException\n", 
				m_nSerializeSchemaVersionLoad);
			AfxThrowArchiveException(CArchiveException::badSchema);
			}
		}

	// Call base class implementation
	CObject::Serialize(ar);

	// Serialize all data
	if (ar.IsStoring())
		StoreProperties(ar);
	else
		LoadProperties(ar);

	ASSERT_VALID(this);
	}

#ifdef _DEBUG
void COXSplitterColRowState::AssertValid() const
	{
	CObject::AssertValid();
	}

void COXSplitterColRowState::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	}
#endif //_DEBUG

COXSplitterColRowState::~COXSplitterColRowState()
	{
	ASSERT_VALID(this);
	}

// protected:
void COXSplitterColRowState::Initialize()
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Initialized the data members of this object
	{
	m_bRow = TRUE;
	m_nColRowIndex = 0;
	m_nIdealSize = 0;
	m_nMinSize = 0;

	m_nSerializeSchemaVersionLoad = -1;
	}

void COXSplitterColRowState::StoreProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : Stores the properties of this object to archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsStoring());

	ar << m_bRow;
	ar << m_nColRowIndex;
	ar << m_nIdealSize;
	ar << m_nMinSize;

	// Some sanity checks
	ASSERT((m_bRow == FALSE) || (m_bRow == TRUE));
	ASSERT(0 <= m_nColRowIndex);
	ASSERT(0 <= m_nIdealSize);
	ASSERT(0 <= m_nMinSize);

	ASSERT_VALID(this);
	}

void COXSplitterColRowState::LoadProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : loads the properties of this object from archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsLoading());

	// Some sanity checks
	ASSERT((m_bRow == FALSE) || (m_bRow == TRUE));
	ASSERT(0 <= m_nColRowIndex);
	ASSERT(0 <= m_nIdealSize);
	ASSERT(0 <= m_nMinSize);

	ar >> m_bRow;
	ar >> m_nColRowIndex;
	ar >> m_nIdealSize;
	ar >> m_nMinSize;

	ASSERT_VALID(this);
	}

// ==========================================================================
void AFXAPI SerializeElements(CArchive& ar, COXSplitterColRowState** pSplitterColRowState, int nCount)
	{
	ASSERT(AfxIsValidAddress(pSplitterColRowState, sizeof(COXSplitterColRowState*) * nCount, ar.IsStoring()));

	if (ar.IsStoring())
		{
		for (int i = 0; i < nCount; i++)
			{
			ASSERT_VALID(pSplitterColRowState[i]);
			ar << pSplitterColRowState[i];
			}
		}
	else
		{
		for (int i = 0; i < nCount; i++)
			{
			ar >> pSplitterColRowState[i];
			ASSERT_VALID(pSplitterColRowState[i]);
			}
		}
	}

#endif // _MFC_VER
// ==========================================================================
