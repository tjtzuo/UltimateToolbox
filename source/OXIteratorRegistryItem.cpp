// ==========================================================================
//				Class Implementation : COXIteratorRegistryItem
// ==========================================================================

// Source file : OXIteratorRegistryItem.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXIteratorRegistryItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXIteratorRegistryItem, COXRegistryItem)

#define new DEBUG_NEW

#ifdef _DEBUG
// Trace a message when the RESULT specifies failure
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)							\
	{ if (FAILED(RESULT)) {												\
		TRACE(_T("%s (%s): Failed (%u == 0x%X, Code : %u) :\n\t%s\n"),	\
		_T(TEXT), (LPCTSTR)GetFullRegistryItem(), RESULT, RESULT,		\
		HRESULT_CODE(RESULT), GetResultMessage(RESULT));				\
	} }
#else
// Do not trace in Release build
#define CONDITIONAL_TRACE_RESULT(TEXT, RESULT)
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Definition of static variables

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:
	// struct COXSearchData
	//	{
	//	COXRegistryItem m_regItem;
	//  --- The current registry item used in the (recursive) search
	//	int m_nValueIndex;
	//  ---  The index of the value that needs processing (-1 means all done)
	//	int m_nSubkeyIndex;
	//  ---  The index of the subkey that needs processing (-1 means all done)
	//	int m_nRecursiveSubkeyIndex;
	//  ---  The index of the subkey that must be used for recursive enumeration (-1 means all done)
	//	};
	// CList<COXSearchData*, COXSearchData*> m_searchStack;
	// --- The complete stack of COXSearchData.  The top (head) is the current item in use.

	// BOOL m_bIncludeValues;
	// --- Whether values should be included in the result

	// BOOL m_bIncludeSubkeys;
	// --- Whether the subkeys should be included in the result

	// BOOL m_bReversed;
	// --- Whether the values and subkeys should be visited in 
	//     reversed order (high index to 0) (TRUE) or in normal order (0 to number - 1) (FALSE)

	// BOOL m_bRecursive;
	// --- Whether subkeys should be visited recursively

	// BOOL m_bDepthFirst;
	// --- Whether recursive traversal should be done depth-first (TRUE) or breadth-first (FALSE)

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXIteratorRegistryItem::COXIteratorRegistryItem(LPCTSTR pszFullRegistryItem /* = NULL */)
	:
	COXRegistryItem(pszFullRegistryItem),
	m_searchStack(),
	m_bIncludeValues(FALSE),
	m_bIncludeSubkeys(FALSE),
	m_bReversed(FALSE),
	m_bRecursive(FALSE),
	m_bDepthFirst(FALSE)
	{
	ASSERT_VALID(this);
	}

COXIteratorRegistryItem::COXIteratorRegistryItem(const COXRegistryItem& registryItem)
	:
	COXRegistryItem(registryItem),
	m_searchStack(),
	m_bIncludeValues(FALSE),
	m_bIncludeSubkeys(FALSE),
	m_bReversed(FALSE),
	m_bRecursive(FALSE),
	m_bDepthFirst(FALSE)
	{
	ASSERT_VALID(&registryItem);

	// The registry key handles are set to NULL, so they will have to be opened
	// again before use

	ASSERT_VALID(this);
	}

COXIteratorRegistryItem::COXIteratorRegistryItem(const COXIteratorRegistryItem& registryItem)
	:
	COXRegistryItem(registryItem),
	m_searchStack(),
	m_bIncludeValues(	registryItem.m_bIncludeValues),
	m_bIncludeSubkeys(	registryItem.m_bIncludeSubkeys),
	m_bReversed(		registryItem.m_bReversed),
	m_bRecursive(		registryItem.m_bRecursive),
	m_bDepthFirst(		registryItem.m_bDepthFirst)
	{
	ASSERT_VALID(&registryItem);
	ASSERT_VALID(this);
	}

COXIteratorRegistryItem& COXIteratorRegistryItem::operator=(const COXRegistryItem& registryItem)
	{
	ASSERT_VALID(&registryItem);

	// ... Base class will call virtaul Empty()
	COXRegistryItem::operator=(registryItem);

	// The registry key handles are set to NULL, so they will have to be opened
	// again before use
	
	ASSERT_VALID(this);
	return *this;
	}

COXIteratorRegistryItem& COXIteratorRegistryItem::operator=(const COXIteratorRegistryItem& registryItem)
	{
	ASSERT_VALID(&registryItem);

	if(this==&registryItem)
		return *this;
		
	// ... Base class will call virtaul Empty()
	COXRegistryItem::operator=(registryItem);

	m_bIncludeValues = 	registryItem.m_bIncludeValues;
	m_bIncludeSubkeys =	registryItem.m_bIncludeSubkeys;
	m_bReversed =		registryItem.m_bReversed;
	m_bRecursive =		registryItem.m_bRecursive;
	m_bDepthFirst =		registryItem.m_bDepthFirst;
	
	ASSERT_VALID(this);
	return *this;
	}


void COXIteratorRegistryItem::Empty()
	{
	ASSERT_VALID(this);

	// ... First call base class implementation
	COXRegistryItem::Empty();

	// Then initialize data members
	m_bIncludeValues = FALSE;
	m_bIncludeSubkeys = FALSE;
	m_bReversed = FALSE;
	m_bRecursive = FALSE;
	m_bDepthFirst = FALSE;

	// And empty the stack
	POSITION pos = m_searchStack.GetHeadPosition();
	while (pos != NULL)
		{
		ASSERT(AfxIsValidAddress(m_searchStack.GetAt(pos), sizeof(COXSearchData)));
		delete m_searchStack.GetNext(pos);
		}
	m_searchStack.RemoveAll();

	ASSERT_VALID(this);
	}

BOOL COXIteratorRegistryItem::Start(BOOL bIncludeValues /* = TRUE */, 
	BOOL bIncludeSubkeys /* = FALSE */, BOOL bReversed /* = FALSE */, 
	BOOL bRecursive /* = FALSE */, BOOL bDepthFirst /* = FALSE */)
{
	// Store required attributes
	m_bIncludeValues = bIncludeValues;
	m_bIncludeSubkeys = bIncludeSubkeys;
	m_bReversed = bReversed;
	m_bRecursive = bRecursive;
	m_bDepthFirst = bDepthFirst;

	// If an iteration is still running, end it
	End();

	// Initialize the first search level
	// (Do not try to get the first item if this already fails)
	if (AddNewSearchData(*this) != NULL)
		Next();
	else
		COXRegistryItem::Empty();

	// ... Do not trace for end of iteration (expected condition)
	if (HRESULT_CODE(m_nLastError) != ERROR_NO_MORE_ITEMS)
		CONDITIONAL_TRACE_RESULT("COXRegistryItem::Start", m_nLastError);
	ASSERT_VALID(this);
	return SUCCEEDED(m_nLastError);
}

BOOL COXIteratorRegistryItem::Next()
{
	// For non-recursive search, the stack can contain at most one search data object
	ASSERT(m_bRecursive || m_searchStack.IsEmpty() || 
		(m_searchStack.GetTail() == m_searchStack.GetHead()));

	BOOL bItemFound = FALSE;
	COXSearchData* pSearchData = NULL;
	if (!m_searchStack.IsEmpty())
		pSearchData = m_searchStack.GetHead();

	// ... While we have not found anything, and there are still possibilities to examine
	while (!bItemFound && (pSearchData != NULL))
	{
		// The following conditions will be tested and acted upon accordingly
		//   1. Remove top level of the stack if it is finished
		//   2. Get the next subkey name if searching recursively (depth first)
		//   3. Get the next value name if it is requested and available
		//   4. Get the next subkey name if it is requested and available
		//   5. Get one level deeper if we are searching recursively (breadth first)

		// 1. Remove top level of the stack if it is finished
		ASSERT(pSearchData != NULL);
		if ( ((pSearchData->m_nValueIndex < 0) || !m_bIncludeValues) &&
			 ((pSearchData->m_nSubkeyIndex < 0) || !m_bIncludeSubkeys) &&
			 ((pSearchData->m_nRecursiveSubkeyIndex < 0) || !m_bRecursive) )
		{
			delete pSearchData;
			pSearchData = NULL;
			m_searchStack.RemoveHead();
			// ... Reset current
			if (!m_searchStack.IsEmpty())
				pSearchData = m_searchStack.GetHead();
			// Loop again
			continue;
		}

		// 2. Get the next subkey name if searching recursively (depth first)
		ASSERT(pSearchData != NULL);
		if((0 <= pSearchData->m_nRecursiveSubkeyIndex) && m_bRecursive && m_bDepthFirst)
		{
			ASSERT(pSearchData->m_regItem.IsOpen());

			// ... Get the subkey name
			CString sSubkeyName = pSearchData->m_regItem.
				EnumerateSubkey(pSearchData->m_nRecursiveSubkeyIndex);
			m_nLastError = pSearchData->m_regItem.GetLastError();

			// Handled this value, get the index of the next one
			if (!m_bReversed)
				pSearchData->m_nRecursiveSubkeyIndex++;
			else
				pSearchData->m_nRecursiveSubkeyIndex--;
			if (FAILED(m_nLastError))
				pSearchData->m_nRecursiveSubkeyIndex = -1;

			if (SUCCEEDED(m_nLastError))
			{
				// .... Empty subkey name is not allowed
				ASSERT(!sSubkeyName.IsEmpty());
				// ... Add new data to the stack
				AddNewSearchData(pSearchData->m_regItem, sSubkeyName, TRUE);
				// ... Keep on looking with the new top of the stack
				//     (whether the new level succeeded or not)
				ASSERT(!m_searchStack.IsEmpty());
				pSearchData = m_searchStack.GetHead();
				continue;
			}
		}

		// 3. Get the next value name if it is requested and available
		ASSERT(pSearchData != NULL);
		if((0 <= pSearchData->m_nValueIndex) && m_bIncludeValues)
		{
			ASSERT(pSearchData->m_regItem.IsOpen());

			// ... Get the value name
			CString sValueName = pSearchData->m_regItem.EnumerateValue(
				pSearchData->m_nValueIndex);
			m_nLastError = pSearchData->m_regItem.GetLastError();
			
			// Handled this value, get the index of the next one
			if (!m_bReversed)
				pSearchData->m_nValueIndex++;
			else
				pSearchData->m_nValueIndex--;
			if (FAILED(m_nLastError))
				pSearchData->m_nValueIndex = -1;

			if (SUCCEEDED(m_nLastError))
			{
				if (sValueName.IsEmpty())
					sValueName = _T("NoName");
				// ... Asign data to this
				*this = pSearchData->m_regItem;
				SetValueName(sValueName);
				// ... Stop looking
				bItemFound = TRUE;
				continue;
			}
		}

		// 4. Get the next subkey name if it is requested and available
		ASSERT(pSearchData != NULL);
		if((0 <= pSearchData->m_nSubkeyIndex) && m_bIncludeSubkeys)
		{
			ASSERT(pSearchData->m_regItem.IsOpen());

			// ... Get the subkey name
			CString sSubkeyName = pSearchData->m_regItem.EnumerateSubkey(
				pSearchData->m_nSubkeyIndex);
			m_nLastError = pSearchData->m_regItem.GetLastError();

			// Handled this value, get the index of the next one
			if (!m_bReversed)
				pSearchData->m_nSubkeyIndex++;
			else
				pSearchData->m_nSubkeyIndex--;
			if (FAILED(m_nLastError))
				pSearchData->m_nSubkeyIndex = -1;

			if (SUCCEEDED(m_nLastError))
			{
				// .... Empty subkey name is not allowed
				ASSERT(!sSubkeyName.IsEmpty());
				// ... Asign data to this
				*this = pSearchData->m_regItem;
				SetKeyNames(GetKeyNames() + sSubkeyName + m_pszBackslash);
				// ... Stop looking
				bItemFound = TRUE;
				continue;
			}
		}

		// 5. Get one level deeper if we are searching recursively (breadth first)
		ASSERT(pSearchData != NULL);
		if((0 <= pSearchData->m_nRecursiveSubkeyIndex) && m_bRecursive && !m_bDepthFirst)
		{
			ASSERT(pSearchData->m_regItem.IsOpen());

			// ... Get the subkey name
			CString sSubkeyName = pSearchData->m_regItem.EnumerateSubkey(
				pSearchData->m_nRecursiveSubkeyIndex);
			m_nLastError = pSearchData->m_regItem.GetLastError();

			// Handled this value, get the index of the next one
			if (!m_bReversed)
				pSearchData->m_nRecursiveSubkeyIndex++;
			else
				pSearchData->m_nRecursiveSubkeyIndex--;
			if (FAILED(m_nLastError))
				pSearchData->m_nRecursiveSubkeyIndex = -1;

			if (SUCCEEDED(m_nLastError))
			{
				// .... Empty subkey name is not allowed
				ASSERT(!sSubkeyName.IsEmpty());
				// ... Add new data to the stack
				AddNewSearchData(pSearchData->m_regItem, sSubkeyName, FALSE);
				// ... Keep on looking with the current level of the stack
				//     (whether the new level succeeded or not)
				continue;
			}
		}
	}
	
	if (!bItemFound)
	{
		if (SUCCEEDED(m_nLastError))
			// ... Mark that there are no more items left to iterate
			m_nLastError = ERROR_NO_MORE_ITEMS;
		COXRegistryItem::Empty();
	}

	// ... Do not trace for end of iteration (expected condition)
	if (HRESULT_CODE(m_nLastError) != ERROR_NO_MORE_ITEMS)
		CONDITIONAL_TRACE_RESULT("COXRegistryItem::Next", m_nLastError);
	return SUCCEEDED(m_nLastError);
}

void COXIteratorRegistryItem::End()
{
	// Empty the search stack
	POSITION pos = m_searchStack.GetHeadPosition();
	while (pos != NULL)
	{
		ASSERT(AfxIsValidAddress(m_searchStack.GetAt(pos), sizeof(COXSearchData)));
		delete m_searchStack.GetNext(pos);
	}
	m_searchStack.RemoveAll();
}

COXIteratorRegistryItem& COXIteratorRegistryItem::operator++()
	// Prefix increment operator.
{
	Next();
	return *this;
}

COXIteratorRegistryItem COXIteratorRegistryItem::operator++(int)
	// Postfix increment operator.
	{
	COXIteratorRegistryItem tempIterRegItem = *this;
	Next();
	return tempIterRegItem;
	}

#ifdef _DEBUG
void COXIteratorRegistryItem::AssertValid() const
	{
	// ... First call base class implementation
	COXRegistryItem::AssertValid();
	}

void COXIteratorRegistryItem::Dump(CDumpContext& dc) const
	{
	// ... First call base class implementation
	COXRegistryItem::Dump(dc);

	// Then dump our data members
	dc << "\nm_searchStack : " << m_searchStack;
	dc << "\nm_bIncludeValues : " << m_bIncludeValues;
	dc << "\nm_bIncludeSubkeys : " << m_bIncludeSubkeys;
	dc << "\nm_bReversed : " << m_bReversed;
	dc << "\nm_bRecursive : " << m_bRecursive;
	dc << "\nm_bDepthFirst : " << m_bDepthFirst;
	dc << "\n";
	}
#endif //_DEBUG

COXIteratorRegistryItem::~COXIteratorRegistryItem()
	{
	}

// protected:
COXIteratorRegistryItem::COXSearchData* COXIteratorRegistryItem::AddNewSearchData(
	const COXRegistryItem& registryItem, LPCTSTR pszSubkeyName /* = NULL */,
	BOOL bDepthFirst /* = FALSE */)
	// --- In  : registryItem : The registry item to use as base for the new search item
	//			 pszSubkeyName : The subkey name of the key that should be used as search item
	//			 bDepthFirst : Whether the item will be used in a depth-first search or not
	// --- Out : 
	// --- Returns : The newly created search item (or NULL when failed)
	// --- Effect : Creates a new search item and adds it to the stack
	//				When !bDepthFirst the item is added at the bottom of the stack (tail)
	//				otheriwse at the top (head)
	{
	//  Initialize the new registry item in the search data
	COXSearchData* pSearchData = new COXSearchData;
	pSearchData->m_regItem = registryItem;
	CString sSubkeyName = pszSubkeyName;
	if (!sSubkeyName.IsEmpty())
		pSearchData->m_regItem.SetKeyNames(pSearchData->m_regItem.GetKeyNames() + sSubkeyName + m_pszBackslash);

	// Open the key for enumerating
	ASSERT(!pSearchData->m_regItem.IsOpen());
	REGSAM samDesired = KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE;
//	if (m_bReversed)
		// ... KEY_QUERY_VALUE is needed when iterating in reversed order 
		//     to get the number of values and subkeys
//		samDesired |= KEY_QUERY_VALUE;
	// ... First try to open with maximum access, 
	//     then with KEY_ENUMERATE_SUB_KEYS (and KEY_QUERY_VALUE) to enumerate)
	if (pSearchData->m_regItem.Open(FALSE) || 
		pSearchData->m_regItem.Open(FALSE, samDesired))
		{
		if (!bDepthFirst)
			m_searchStack.AddTail(pSearchData);
		else
			m_searchStack.AddHead(pSearchData);
		if (!m_bReversed)
			{
			// Iterate from 0 to (number - 1)
			pSearchData->m_nValueIndex = 0;
			pSearchData->m_nSubkeyIndex = 0;
			pSearchData->m_nRecursiveSubkeyIndex = 0;
			}
		else
			{
			// Iterate from (number - 1) to 0
			pSearchData->m_nValueIndex = pSearchData->m_regItem.GetNumberOfValues() - 1;
			pSearchData->m_nSubkeyIndex = pSearchData->m_regItem.GetNumberOfSubkeys() - 1;
			pSearchData->m_nRecursiveSubkeyIndex = pSearchData->m_nSubkeyIndex;
			}
		}
	else
		{
		delete pSearchData;
		pSearchData = NULL;
		}

	return pSearchData;
	}

// private:

// ==========================================================================
