// ==========================================================================
//				Class Implementation : COXChildFrameState
// ==========================================================================

// Source file : OXChildFrameState.cpp

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
#pragma message("Warning : OXChildFrameState.cpp not included because MFC Version < 4.2")
#else
// The entire file


#include "OXChildFrameState.h"
#include "OXSplitterColRowState.h"
#include "OXDocTemplateSpy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int COXChildFrameState::m_nSerializeSchemaVersion = 1;

IMPLEMENT_SERIAL(COXChildFrameState, CObject, COXChildFrameState::m_nSerializeSchemaVersion | VERSIONABLE_SCHEMA)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:
	// WINDOWPLACEMENT m_framePlacement;
	// --- The placement of the frame window

	// CString m_sDocPath;
	// --- The full path name of the file associated with the document

	// CString m_sDocClassName;
	// CString m_sFrameClassName;
	// CString m_sViewClassName;
	// --- Name of the class of the document, frame window, view
	//	   The template containing this classes will be used to
	//     reconstruct the previous state

	// BOOL m_bSaveSplitterPanes;
	// ---- Whether the properties of the splitter panes should be saved and restored

	// CObArray* m_pSplitterPanes;
	// --- The collection of the properties of the splitter panes

	// int m_nSerializeSchemaVersionLoad;
	// --- The schema version number that is read from archive

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXChildFrameState::COXChildFrameState()
	:
	m_pSplitterPanes(NULL)
	{
	Initialize();

	ASSERT_VALID(this);
	}

BOOL COXChildFrameState::IsSplitterPaneIncluded() const
	{
	ASSERT_VALID(this);

	return m_bSaveSplitterPanes;
	}

void COXChildFrameState::IncludeSplitterPane(BOOL bInclude /* = TRUE */)
	{
	ASSERT_VALID(this);

	m_bSaveSplitterPanes = bInclude;

	ASSERT_VALID(this);
	}

BOOL COXChildFrameState::ComputeProperties(CFrameWnd* pFrameWnd)
	{
	ASSERT_VALID(this);

	// Get the position of the frame window
	::ZeroMemory(&m_framePlacement, sizeof(WINDOWPLACEMENT));
	m_framePlacement.length = sizeof(WINDOWPLACEMENT);
	if (pFrameWnd != NULL)
	{
		VERIFY(pFrameWnd->GetWindowPlacement(&m_framePlacement));
		if(!pFrameWnd->IsWindowVisible())
		{
			m_framePlacement.showCmd=SW_HIDE;
		}
	}

	// Get the document and view of this frame
	CDocument* pDoc = NULL;
	CView* pView = NULL;
	if (!GetDocView(pFrameWnd, pDoc, pView))
		{
		TRACE0("COXChildFrameState::ComputeProperties : Failed to get the doc-view for this frame, failing\n");
		return FALSE;
		}

	// Get the file path of the document
	if (pDoc != NULL)
		m_sDocPath = pDoc->GetPathName();

	// Store the name of the classes of doc, frame and view
	if (pDoc != NULL)
		m_sDocClassName = pDoc->GetRuntimeClass()->m_lpszClassName;
	if (pFrameWnd != NULL)
		m_sFrameClassName = pFrameWnd->GetRuntimeClass()->m_lpszClassName;
	if (pView != NULL)
		m_sViewClassName = pView->GetRuntimeClass()->m_lpszClassName;

	EmptySplitterPanes(m_pSplitterPanes);
	if (m_bSaveSplitterPanes)
		ComputeSplitterPanes(pFrameWnd);

	ASSERT_VALID(this);
	return TRUE;
	}

BOOL COXChildFrameState::ApplyProperties() const
	{
	ASSERT_VALID(this);

	CDocTemplate* pDocTemplate = NULL;
	CDocument* pDoc = NULL;
	CView* pView = NULL;
	CFrameWnd* pFrameWnd = NULL;

	// Create a new document with the info we have
	pDocTemplate = GetDocTemplate(m_sDocClassName, m_sFrameClassName, m_sViewClassName);
	if (pDocTemplate == NULL)
		{
		TRACE0("COXChildFrameState::ApplyProperties : Could not find the necessary doc template\n");
		// Did you change the class name of the document ?
		// Did you remove the doc template from your program ?
		return FALSE;
		}

	// Open the document
	LPCTSTR pszDocPath = m_sDocPath;
	if (m_sDocPath.IsEmpty())
		pszDocPath = NULL;
	if (!OpenDocument(pDocTemplate, pszDocPath, pDoc, pFrameWnd, pView))
		{
		TRACE0("COXChildFrameState::ApplyProperties : Failed to open document, failing\n");
		return FALSE;
		}

	// Restore the position of the frame window and show it
	ASSERT(pFrameWnd != NULL);
	VERIFY(pFrameWnd->SetWindowPlacement(&m_framePlacement));

	if (m_bSaveSplitterPanes)
		ApplySplitterPanes(pFrameWnd);

	ASSERT_VALID(this);
	return TRUE;
	}

void COXChildFrameState::Serialize(CArchive& ar)
	{
	ASSERT_VALID(this);

	// Check the version 
	// (If version == -1, the version is unknown, this occurs when Serialize() is called directly)
	if (ar.IsLoading())
		{
		m_nSerializeSchemaVersionLoad = (int)ar.GetObjectSchema();
		if (m_nSerializeSchemaVersion < m_nSerializeSchemaVersionLoad)
			{
			TRACE1("COXChildFrameState::Serialize : Unexpected schema version : %i, throwing CArchiveException\n", 
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
void COXChildFrameState::AssertValid() const
	{
	CObject::AssertValid();

	// The pointer object must always be valid and may never be NULL
	ASSERT_VALID(m_pSplitterPanes);
	}

void COXChildFrameState::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	}
#endif //_DEBUG

COXChildFrameState::~COXChildFrameState()
	{
	ASSERT_VALID(this);

	// Empty all lists
	EmptySplitterPanes(m_pSplitterPanes);

	// Delete dynamically created members
	delete m_pSplitterPanes;
	m_pSplitterPanes = NULL;
	}

// protected:
void COXChildFrameState::Initialize()
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Initialized the data members of this object
	{
	::ZeroMemory(&m_framePlacement, sizeof(WINDOWPLACEMENT));
	m_bSaveSplitterPanes = TRUE;
	if (m_pSplitterPanes == NULL)
		m_pSplitterPanes = new CObArray();
	EmptySplitterPanes(m_pSplitterPanes);

	m_nSerializeSchemaVersionLoad = -1;

	ASSERT_VALID(this);
	}

BOOL COXChildFrameState::ComputeSplitterPanes(CFrameWnd* pFrameWnd)
	// --- In  :
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Computes all the states of the splitter panes and adds them
	//				to an internal collection
	{
	ASSERT_VALID(this);

	// Check for a CSplitterWnd child window
	CSplitterWnd* pSplitterWnd = GetSplitterWindow(pFrameWnd);
	if (pSplitterWnd  == NULL)
		// ... No splitter window found : nothing to do
		return TRUE;

	BOOL bAllSucceeded = TRUE;
	int nRowColIndex = 0;

	// Calculate the row properties
	for (nRowColIndex = 0; nRowColIndex < pSplitterWnd->GetRowCount(); nRowColIndex++)
		{
		COXSplitterColRowState* pSplitterColRowState = new COXSplitterColRowState;
		if (pSplitterColRowState->ComputeProperties(pSplitterWnd, nRowColIndex, TRUE))
			m_pSplitterPanes->Add(pSplitterColRowState);
		else
			{
			TRACE0("COXChildFrameState::ComputeSplitterPanes : Failed to compute properties (row)\n");
			bAllSucceeded = FALSE;
			delete pSplitterColRowState;
			}
		}

	// Calculate the column properties
	for (nRowColIndex = 0; nRowColIndex < pSplitterWnd->GetColumnCount(); nRowColIndex++)
		{
		COXSplitterColRowState* pSplitterColRowState = new COXSplitterColRowState;
		if (pSplitterColRowState->ComputeProperties(pSplitterWnd, nRowColIndex, FALSE))
			m_pSplitterPanes->Add(pSplitterColRowState);
		else
			{
			TRACE0("COXChildFrameState::ComputeSplitterPanes : Failed to compute properties (column)\n");
			bAllSucceeded = FALSE;
			delete pSplitterColRowState;
			}
		}

	ASSERT_VALID(this);
	return bAllSucceeded;
	}

BOOL COXChildFrameState::ApplySplitterPanes(CFrameWnd* pFrameWnd) const
	// --- In  :
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Applies all the states of the splitter panes that are in the
	//				internal collection
	{
	ASSERT_VALID(this);

	// Check for a CSplitterWnd child window
	CSplitterWnd* pSplitterWnd = GetSplitterWindow(pFrameWnd);
	if (pSplitterWnd  == NULL)
		// ... No splitter window found : nothing to do
		return TRUE;

	BOOL bAllSucceeded = TRUE;
	int nSplitterColRowStateIndex = 0;
	COXSplitterColRowState* pSplitterColRowState = NULL;
	for(nSplitterColRowStateIndex = 0; 
		nSplitterColRowStateIndex < m_pSplitterPanes->GetSize(); 
		nSplitterColRowStateIndex++)
		{
		pSplitterColRowState = DYNAMIC_DOWNCAST(COXSplitterColRowState, 
			m_pSplitterPanes->GetAt(nSplitterColRowStateIndex));
		if (pSplitterColRowState != NULL)
			{
			if (!pSplitterColRowState->ApplyProperties(pSplitterWnd))
				{
				TRACE0("COXChildFrameState::ApplySplitterPanes : Failed to apply properties\n");
				bAllSucceeded = FALSE;
				}
			}
		}

	// Make the changes visible
	pSplitterWnd->RecalcLayout();

	ASSERT_VALID(this);
	return bAllSucceeded;
	}

void COXChildFrameState::StoreProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : Stores the properties of this object to archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsStoring());

	// Some sanity checks
	ASSERT((m_bSaveSplitterPanes == FALSE) || (m_bSaveSplitterPanes == TRUE));

	// ... Frame position
	ar << m_framePlacement.flags;
	ar << m_framePlacement.showCmd;
	ar << m_framePlacement.ptMinPosition;
	ar << m_framePlacement.ptMaxPosition;
	ar << m_framePlacement.rcNormalPosition;

	// ... Doc file path
	ar << m_sDocPath;

	// ... Class names
	ar << m_sDocClassName;
	ar << m_sFrameClassName;
	ar << m_sViewClassName;

	// ... Splitter panes
	ar << m_bSaveSplitterPanes;
	ar << m_pSplitterPanes;

	ASSERT_VALID(this);
	}

void COXChildFrameState::LoadProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : loads the properties of this object from archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsLoading());

	// ... Frame position
	::ZeroMemory(&m_framePlacement, sizeof(WINDOWPLACEMENT));
	m_framePlacement.length = sizeof(WINDOWPLACEMENT);
	ar >> m_framePlacement.flags;
	ar >> m_framePlacement.showCmd;
	ar >> m_framePlacement.ptMinPosition;
	ar >> m_framePlacement.ptMaxPosition;
	ar >> m_framePlacement.rcNormalPosition;

	// ... Doc file path
	ar >> m_sDocPath;

	// ... Class names
	ar >> m_sDocClassName;
	ar >> m_sFrameClassName;
	ar >> m_sViewClassName;

	// ... Splitter panes
	ar >> m_bSaveSplitterPanes;
	CObArray* pOldSplitterPanes= NULL;
	pOldSplitterPanes = m_pSplitterPanes;
	ar >> m_pSplitterPanes;
	EmptySplitterPanes(pOldSplitterPanes);
	delete pOldSplitterPanes;

	// Some sanity checks
	ASSERT((m_bSaveSplitterPanes == FALSE) || (m_bSaveSplitterPanes == TRUE));

	ASSERT_VALID(this);
	}

void COXChildFrameState::EmptySplitterPanes(CObArray* pSplitterPanes)
	// --- In  : pSplitterPanes: Collection to use
	// --- Out : 
	// --- Returns :
	// --- Effect : Clears the collection by deleting all its members and 
	//				clearing the collection itself
	{
	ASSERT_VALID(pSplitterPanes);

	int nSplitterColRowStateIndex = 0;
	CObject* pSplitterColRowState = NULL;
	for (nSplitterColRowStateIndex = 0; nSplitterColRowStateIndex < pSplitterPanes->GetSize(); nSplitterColRowStateIndex++)
		{
		pSplitterColRowState = pSplitterPanes->GetAt(nSplitterColRowStateIndex);
		delete pSplitterColRowState;
		pSplitterPanes->SetAt(nSplitterColRowStateIndex, NULL);
		}
	pSplitterPanes->RemoveAll();
	}

CView* COXChildFrameState::GetFirstView(CDocument* pDoc)
	// --- In  : pDoc : A valid CDocument object
	// --- Out : 
	// --- Returns : The first view attached to the specified document
	// --- Effect : 
	{
	ASSERT(pDoc != NULL);

	CView * pView = NULL;
	POSITION viewPos = NULL;

	// Get the first view in the collection
	viewPos = pDoc->GetFirstViewPosition();
	if (viewPos != NULL)
		{
		pView = pDoc->GetNextView(viewPos);
		ASSERT(pView != NULL);
		}

#ifdef _DEBUG
	if (pView == NULL)
		TRACE0("COXChildFrameState::GetFirstView : No view found\n");
#endif // _DEBUG

	return pView;
	}


BOOL COXChildFrameState::OpenDocument(CDocTemplate* pDocTemplate, LPCTSTR pszDocPath, 
	CDocument*& pDoc, CFrameWnd*& pFrameWnd, CView*& pView)
	// --- In  : pDocTemplate : The document template to use
	//			 pszDocPath : The file path of the document to open
	// --- Out : pDoc : The document that is opened
	//			 pFrameWnd : The associated MDI child frame window
	//			 pView : The associated view (the first one)
	// --- Returns : Whether it succeeded or not
	// --- Effect : Uses the specified doc template the open a document with the
	//				spcified file path
	{
	ASSERT(pDocTemplate != NULL);

	// ... Initialize output parameters
	pDoc = NULL;
	pFrameWnd = NULL;
	pView = NULL;

	// ... Check whether the document has already been opened
	pDoc = SearchDocument(pszDocPath);
	if (pDoc != NULL)
		{
		// Document has already been opened, attach a new view to it
		pFrameWnd = pDocTemplate->CreateNewFrame(pDoc, NULL);
		if (pFrameWnd == NULL)
			{
			TRACE0("COXChildFrameState::OpenDocument : Failed to create new frame for existing document.\n");
			pDoc = NULL;
			return FALSE;
			}
		
		// ... Frame is initially visible, this is necessary to send
		//     WM_INITIALUPDATE to the view
		pDocTemplate->InitialUpdateFrame(pFrameWnd, pDoc, TRUE);
		pView = GetFirstView(pFrameWnd);
		ASSERT(pView != NULL);
		}
	else
		{
		// Document has not yet been opened, open it now
		// ... Frame is initially visible, this is necessary to send
		//     WM_INITIALUPDATE to the view
		pDoc = pDocTemplate->OpenDocumentFile(pszDocPath, TRUE);
		if (pDoc == NULL)
			{
			TRACE0("COXChildFrameState::OpenDocument : Failed to open document, failing\n");
			return FALSE;
			}

		pView = GetFirstView(pDoc);
		ASSERT(pView != NULL);
		pFrameWnd = pView->GetParentFrame();
		ASSERT(pFrameWnd != NULL);
		}


	return TRUE;
	}

CDocument* COXChildFrameState::SearchDocument(LPCTSTR pszDocPath)
	// --- In  : pszDocPath : File path of a document
	// --- Out : 
	// --- Returns : The open document with that file path or NULL otherwise
	// --- Effect : 
	{
	if (pszDocPath == NULL)
		// ... Can only search for documents with a non-empty name
		return NULL;

	CDocument* pDoc = NULL;
	POSITION templatePos = NULL;
	CDocTemplate* pDocTemplate = NULL;
	templatePos = AfxGetApp()->GetFirstDocTemplatePosition();
	while((pDoc == NULL) && (templatePos != NULL))
		{
		pDocTemplate = AfxGetApp()->GetNextDocTemplate(templatePos);
		ASSERT(pDocTemplate != NULL);
		if (pDocTemplate->MatchDocType(pszDocPath, pDoc) != CDocTemplate::yesAlreadyOpen)
			pDoc = NULL;
		}

	return pDoc;
	}

CView* COXChildFrameState::GetFirstView(CFrameWnd* pFrameWnd)
	// --- In  : pFrameWnd : The frame window to use
	// --- Out : 
	// --- Returns : The first view that is child of the specified MDI child frame window
	// --- Effect : 
	{
	CView* pView = NULL;
	CWnd* pWnd = pFrameWnd->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	pView = DYNAMIC_DOWNCAST(CView, pWnd);
	if (pView == NULL)
		{
		pView = pFrameWnd->GetActiveView();
		}
	return pView;
	}

CSplitterWnd* COXChildFrameState::GetSplitterWindow(CFrameWnd* pFrameWnd)
	// --- In  : pFrameWnd : The MDI child frame to use
	// --- Out : 
	// --- Returns : The splitter window of the specified frame or NULL otherwise
	// --- Effect : When spliiter windows are nested, only the top level window is used
	{
	if (pFrameWnd == NULL)
		return NULL;

	// Search all children for a CSplitterWnd derived object
	HWND hChildWnd = NULL;
	CWnd* pChildWnd = NULL;
	CSplitterWnd* pSplitterWnd = NULL;

	hChildWnd = ::GetWindow(pFrameWnd->GetSafeHwnd(), GW_CHILD);
	pChildWnd = CWnd::FromHandlePermanent(hChildWnd);
	pSplitterWnd = DYNAMIC_DOWNCAST(CSplitterWnd, pChildWnd);
	while ((hChildWnd != NULL) && (pSplitterWnd == NULL))
		{
		hChildWnd = ::GetWindow(hChildWnd, GW_HWNDNEXT);
		pChildWnd = CWnd::FromHandlePermanent(hChildWnd);
		pSplitterWnd = DYNAMIC_DOWNCAST(CSplitterWnd, pChildWnd);
		}

	return pSplitterWnd;
	}

CDocTemplate* COXChildFrameState::GetDocTemplate(LPCTSTR pszDocName, LPCTSTR pszFrameWndName, 
	LPCTSTR pszViewName)
	// --- In  : pszDocName : Class name of the document
	//			 pszFrameWndName : Class name of the frame window
	//			 pszViewName : Class name of the view 
	// --- Out : 
	// --- Returns : The document template with the specified runtime classes for
	//				 document (required), frame (if found) and view (if found)
	// --- Effect : When no template is found that matches all three names,
	//				one is searched that matches at least the docuemnt name
	//				When none is found NULL is returned
	{
	// Iterate all the templates until we find one that matches the requested
	// doc - frame - view combination
	CDocTemplate* pDocTemplate = NULL;
	// ... The requested doc template if found
	CDocTemplate* pRequestedDocTemplate = NULL;
	// ... A doc template with the same doc class name
	CDocTemplate* pSimilarDocTemplate = NULL;
	COXDocTemplateSpy* pDocTemplateSpy = NULL;

	POSITION templatePos = NULL;
	templatePos = AfxGetApp()->GetFirstDocTemplatePosition();
	while((pRequestedDocTemplate == NULL) && (templatePos != NULL))
		{
		pDocTemplate = AfxGetApp()->GetNextDocTemplate(templatePos);
		ASSERT(pDocTemplate != NULL);
		// ... We cast the template to a COXDocTemplateSpy object to be able to
		//     check the runtime classes
		pDocTemplateSpy = (COXDocTemplateSpy*)pDocTemplate;
		if (pDocTemplateSpy->CheckMatch(pszDocName, pszFrameWndName, pszViewName))
			{
			pRequestedDocTemplate = pDocTemplate;
			}
		else if ((pSimilarDocTemplate == NULL) && pDocTemplateSpy->CheckMatch(pszDocName))
			{
			pSimilarDocTemplate = pDocTemplate;
			}
		}

	if (pRequestedDocTemplate != NULL)
		return pRequestedDocTemplate;
	else
		{
#ifdef _DEBUG
		if (pSimilarDocTemplate != NULL)
			TRACE0("COXChildFrameState::GetTemplate : Requested template not found, using other template with the same document class\n");
		else
			TRACE0("COXChildFrameState::GetTemplate : No template found with the requested document class\n");
#endif // _DEBUG
		return pSimilarDocTemplate;
		}
	}

BOOL COXChildFrameState::GetDocView(CFrameWnd* pFrameWnd, CDocument*& pDoc, CView*& pView)
	// --- In  : pFrameWnd : The frame window to use
	// --- Out : pDoc : The document attached to this frame window
	//			 pView : The first view attached to this document
	// --- Returns :
	// --- Effect : 
	{
	// Initialize return values
	pDoc = NULL;
	pView = NULL;

	// Check input parameter
	if (pFrameWnd == NULL)
		return FALSE;

	// First get the view (first pane or active)
	pView = GetFirstView(pFrameWnd);

	// Then get the document
	if (pView != NULL)
		pDoc = pView->GetDocument();

	return ((pView != NULL) && (pDoc != NULL));
	}

// ==========================================================================
void AFXAPI SerializeElements(CArchive& ar, COXChildFrameState** pChildFrameState, int nCount)
	{
	ASSERT(AfxIsValidAddress(pChildFrameState, sizeof(COXChildFrameState*) * nCount, ar.IsStoring()));

	if (ar.IsStoring())
		{
		for (int i = 0; i < nCount; i++)
			{
			ASSERT_VALID(pChildFrameState[i]);
			ar << pChildFrameState[i];
			}
		}
	else
		{
		for (int i = 0; i < nCount; i++)
			{
			ar >> pChildFrameState[i];
			ASSERT_VALID(pChildFrameState[i]);
			}
		}
	}

#endif // _MFC_VER
// ==========================================================================
