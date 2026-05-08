// ==========================================================================
//				Class Implementation : COXWorkspaceState
// ==========================================================================

// Source file : OXWorkspaceState.cpp

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
#pragma message("Warning : OXWorkspaceState.cpp not included because MFC Version < 4.2")
#else
// The entire file

// need this file for CControlBarInfo class
#include <afxpriv.h>       		// MFC extensions for help constants


#include "OXWorkspaceState.h"
#include "OXSplitterColRowState.h"
#include "OXSizeCtrlBar.h"
#include "OXSizeDockBar.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int COXWorkspaceState::m_nSerializeSchemaVersion = 1;

IMPLEMENT_SERIAL(COXWorkspaceState, CObject, COXWorkspaceState::m_nSerializeSchemaVersion | VERSIONABLE_SCHEMA)
#define new DEBUG_NEW

static const TCHAR szSoftware[] =		_T("Software");
static const TCHAR szWorkspaceState[] =	_T("WorkspaceState");
static const TCHAR szWorkspaceStateDefaultValue[] =	_T("Main");

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
const int COXWorkspaceState::m_nMagicNumber = 0x1234FEDC;

// Data members -------------------------------------------------------------
// protected:
	// CSize m_screenSize;
	// --- The size of the entire video screen in pixels (computed during runtilme)

	// BOOL m_bSaveMainWndProps;
	// --- WHether the properties of the main window should be saved and restored

	// WINDOWPLACEMENT m_mainWndPlacement;
	// --- The placement of the main window

	// BOOL m_bSaveBarProps;
	// ---- Whether the properties of the control bars should be saved and restored

	// CDockState* m_pDockState;
	// --- The state of the control bars

	// BOOL m_bSaveChildFrames;
	// ---- Whether the properties of child frame windows should be saved and restored

	// BOOL m_bSaveSplitterPanes;
	// ---- Whether the properties of splitter panes should be saved and restored

	// CObArray* m_pChildFrameStates;
	// --- The properties of the child frame windows

	// int m_nSerializeSchemaVersionLoad;
	// --- The schema version number that is read from archive

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXWorkspaceState::COXWorkspaceState()
	:
	m_pDockState(NULL),
	m_pChildFrameStates(NULL),
	m_pSplitterPanes(NULL)
	{
	Initialize();

	ASSERT_VALID(this);
	}

BOOL COXWorkspaceState::IsMainWindowIncluded() const
	{
	ASSERT_VALID(this);

	return m_bSaveMainWndProps;
	}

BOOL COXWorkspaceState::IsBarPropertyIncluded() const
	{
	ASSERT_VALID(this);

	return m_bSaveBarProps;
	}

BOOL COXWorkspaceState::IsChildFrameIncluded() const
	{
	ASSERT_VALID(this);

	return m_bSaveChildFrames;
	}

BOOL COXWorkspaceState::IsSplitterPaneIncluded() const
	{
	ASSERT_VALID(this);

	return m_bSaveSplitterPanes;
	}

void COXWorkspaceState::IncludeMainWindow(BOOL bInclude /* = TRUE */)
	{
	ASSERT_VALID(this);

	m_bSaveMainWndProps = bInclude;

	ASSERT_VALID(this);
	}

void COXWorkspaceState::IncludeBarProperty(BOOL bInclude /* = TRUE */)
	{
	ASSERT_VALID(this);

	m_bSaveBarProps = bInclude;

	ASSERT_VALID(this);
	}

void COXWorkspaceState::IncludeChildFrame(BOOL bInclude /* = TRUE */)
	{
	ASSERT_VALID(this);

	m_bSaveChildFrames = bInclude;

	ASSERT_VALID(this);
	}

void COXWorkspaceState::IncludeSplitterPane(BOOL bInclude /* = TRUE */)
	{
	ASSERT_VALID(this);

	m_bSaveSplitterPanes = bInclude;

	ASSERT_VALID(this);
	}

BOOL COXWorkspaceState::ComputeProperties()
	{
	ASSERT_VALID(this);

	CWnd* pMainWnd = NULL;
	CFrameWnd* pMainFrame = NULL;

	// Get the main window and the main frame
	pMainWnd = AfxGetMainWnd();
	if (pMainWnd != NULL)
		pMainFrame = pMainWnd->GetTopLevelFrame();

	// Get the position of the main window
	::ZeroMemory(&m_mainWndPlacement, sizeof(WINDOWPLACEMENT));
	if (m_bSaveMainWndProps && (pMainWnd != NULL))
		{
		m_mainWndPlacement.length = sizeof(WINDOWPLACEMENT);
		VERIFY(pMainWnd->GetWindowPlacement(&m_mainWndPlacement));
		}
	
	// Get the current state of the control bars 
	if (m_bSaveBarProps && (pMainFrame != NULL))
	{
		pMainFrame->GetDockState(*m_pDockState);
		ComputeDockingWindowsState(m_pDockState->m_arrBarInfo);
	}

	EmptySplitterPanes(m_pSplitterPanes);
	if (m_bSaveSplitterPanes)
		ComputeSplitterPanes(pMainFrame);

	EmptyChildFrames(m_pChildFrameStates);
	if (m_bSaveChildFrames)
		ComputeChildFrameState();

	return TRUE;
	}

BOOL COXWorkspaceState::ApplyProperties() const
{
	ASSERT_VALID(this);

	CWnd* pMainWnd = NULL;
	CFrameWnd* pMainFrame = NULL;

	// Get the main window and the main frame
	pMainWnd = AfxGetMainWnd();
	if (pMainWnd != NULL)
		pMainFrame = pMainWnd->GetTopLevelFrame();

	BOOL bIsLocked=FALSE;
	// Set the position of the main window
	TRY
	{
		if (m_bSaveMainWndProps && (pMainWnd != NULL))
		{
			WINDOWPLACEMENT	mainWndPlacementCopy;
			::CopyMemory(&mainWndPlacementCopy, &m_mainWndPlacement, 
				sizeof(WINDOWPLACEMENT));
			int nCmdShow=mainWndPlacementCopy.showCmd;
			// Check whether the last state of the main window was minimized
			if(nCmdShow==SW_SHOWMINIMIZED)
			{
				// ... Remove the minimize state and set the restore state 
				nCmdShow=SW_SHOWNORMAL;
			}
			if(!pMainWnd->IsWindowVisible())
				mainWndPlacementCopy.showCmd=SW_HIDE;
			VERIFY(pMainWnd->SetWindowPlacement(&mainWndPlacementCopy));
			CWinApp* pApp=AfxGetApp();
			if(pApp!=NULL)
				pApp->m_nCmdShow=nCmdShow;
		}

		// Set the current state of of the control bars 
		// (Even though the main window has its update locked,
		//  toolbars will be shown directly because they are not childs of the mainframe)
		if (m_bSaveBarProps && (pMainFrame != NULL))
		{
			////////
			// remove TBSTYLE_FLAT style out of CToolBar
			CObArray arrFlatBars;
			int i=0;
			for (i=0; i<m_pDockState->m_arrBarInfo.GetSize(); i++)
			{
				CControlBarInfo* pInfo=(CControlBarInfo*)m_pDockState->m_arrBarInfo[i];
				ASSERT(pInfo!=NULL);
				pInfo->m_pBar=pMainFrame->GetControlBar(pInfo->m_nBarID);
				if (pInfo->m_pBar!=NULL)
				{
					if(pInfo->m_pBar->IsKindOf(RUNTIME_CLASS(CToolBar)) && 
						pInfo->m_pBar->GetStyle()&TBSTYLE_FLAT)
					{
						arrFlatBars.Add((CObject*)pInfo->m_pBar);
						pInfo->m_pBar->ModifyStyle(TBSTYLE_FLAT,0);
					}

					COXSizeControlBar* pSzControlBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pInfo->m_pBar);
					if (pSzControlBar != NULL)
					{



					}
				}
			}
			////////

			pMainFrame->SetDockState(*m_pDockState);
			ApplyDockingWindowsState(m_pDockState->m_arrBarInfo);

			// Refresh all floating windows
			for (i = 0; i < m_pDockState->m_arrBarInfo.GetSize(); i++)
			{
				CControlBarInfo* pInfo=(CControlBarInfo*)m_pDockState->m_arrBarInfo[i];
				ASSERT(pInfo!=NULL);
				pInfo->m_pBar=pMainFrame->GetControlBar(pInfo->m_nBarID);
				if (pInfo->m_pBar != NULL && pInfo->m_pBar->IsFloating())
				{
					CRect rect;
					pInfo->m_pBar->GetParentFrame()->GetWindowRect(rect);
					rect.bottom += 1;
					pInfo->m_pBar->GetParentFrame()->MoveWindow(rect);
				}
 			}

			////////
			// set TBSTYLE_FLAT style for CToolBar
			for (i=0; i<arrFlatBars.GetSize(); i++)
			{
				((CToolBar*)arrFlatBars.GetAt(i))->ModifyStyle(0,TBSTYLE_FLAT);
			}
			////////
		}

		if (m_bSaveSplitterPanes)
			ApplySplitterPanes(pMainFrame);

		// Set the current state of the frame windows
		if (m_bSaveChildFrames)
			ApplyChildFrameState();
	}
	CATCH_ALL(px)
	{
		TRACE0("COXWorkspaceState::ApplyProperties : Catching exception, re-throwing\n");

		// Re-enable main window
		if (pMainWnd != NULL && bIsLocked)
			pMainWnd->UnlockWindowUpdate();

		// Throw the exception again
		THROW_LAST();
	}
	END_CATCH_ALL

	return TRUE;
}

void COXWorkspaceState::Serialize(CArchive& ar)
	{
	ASSERT_VALID(this);

	// Check the version 
	// (If version == -1, the version is unknown, this occurs when Serialize() is called directly)
	if (ar.IsLoading())
		{
		m_nSerializeSchemaVersionLoad = (int)ar.GetObjectSchema();
		if (m_nSerializeSchemaVersion < m_nSerializeSchemaVersionLoad)
			{
			TRACE1("COXWorkspaceState::Serialize : Unexpected schema version : %i, throwing CArchiveException\n", 
				m_nSerializeSchemaVersionLoad);
			AfxThrowArchiveException(CArchiveException::badSchema);
			}
		}

	// Call base class implementation
	CObject::Serialize(ar);

	// Serialize magic number and version number so that when Serialize 
	// is called directly versioning can still  be used
	if (ar.IsStoring())
		{
		ar << m_nMagicNumber;
		ar << m_nSerializeSchemaVersion;
		}
	else
		{
		int nInternalMagicNumber = -1;
		int nInternalVersion = -1;

		ar >> nInternalMagicNumber;
		if (nInternalMagicNumber != m_nMagicNumber)
			{
			TRACE1("COXWorkspaceState::Serialize : Unexpected magic number : 0x%X, throwing CArchiveException\n", 
				nInternalMagicNumber);
			AfxThrowArchiveException(CArchiveException::badSchema);
			}

		ar >> nInternalVersion;
		if (m_nSerializeSchemaVersion < nInternalVersion)
			{
			TRACE1("COXWorkspaceState::Serialize : Unexpected version number : %i, throwing CArchiveException\n", 
				nInternalVersion);
			AfxThrowArchiveException(CArchiveException::badSchema);
			}

		// ... Internal version number take priority over MFC
		m_nSerializeSchemaVersionLoad = nInternalVersion;
		}

	// Serialize all data
	if (ar.IsStoring())
		StoreProperties(ar);
	else
		LoadProperties(ar);

	ASSERT_VALID(this);
	}

BOOL COXWorkspaceState::StoreToFile(LPCTSTR pszPath, 
									BOOL bSaveOpenDocs /* = TRUE */, 
									BOOL bComputeProperties /* = TRUE */, 
									BOOL bCloseApplication /* = TRUE */)
{
	ASSERT_VALID(this);

	// ... When storing : first let all the documents save themselves
	//     Because this may change their name
	if(bSaveOpenDocs)
	{
		AfxGetApp()->SaveAllModified();

		// if this function has been called when application is about to be closed 
		// we don't want it to call SaveAllModified() function on the files that
		// user didn't want to save. in order to do that we have to go through 
		// all application's document and clear modified flag
		if(bCloseApplication && AfxGetApp()->m_pDocManager!=NULL)
		{
			POSITION pos=AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
			while(pos!=NULL)
			{
				CDocTemplate* pTemplate=
					AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
				ASSERT_KINDOF(CDocTemplate,pTemplate);
				POSITION posDoc=pTemplate->GetFirstDocPosition();
				while(posDoc!=NULL)
				{
					CDocument* pDoc=pTemplate->GetNextDoc(posDoc);
					pDoc->SetModifiedFlag(FALSE);
				}
			}
		}
	}

	// ... When storing first establish the values off all properties
	if (bComputeProperties)
		ComputeProperties();

	BOOL bSuccess = TRUE;
	TRY
	{
		StoreLoadFile(pszPath, TRUE);
	}
	CATCH_ALL(px)
	{
#ifdef _DEBUG
		const int nMaxErrorMsgLength = 1024;
		CString sErrorMsg;
		px->GetErrorMessage(sErrorMsg.GetBuffer(nMaxErrorMsgLength), nMaxErrorMsgLength);
		sErrorMsg.ReleaseBuffer();
		TRACE1("COXWorkspaceState::StoreToFile : Catching CException (%s)\n", sErrorMsg);
#endif // _DEBUG

		bSuccess = FALSE;
	}
	END_CATCH_ALL

	ASSERT_VALID(this);
	return bSuccess;
}

BOOL COXWorkspaceState::LoadFromFile(LPCTSTR pszPath, BOOL bApplyProperties /* = TRUE */)
{
	ASSERT_VALID(this);

	BOOL bSuccess = TRUE;
	TRY
	{
		StoreLoadFile(pszPath, FALSE);
	}
	CATCH_ALL(px)
	{
#ifdef _DEBUG
		const int nMaxErrorMsgLength = 1024;
		CString sErrorMsg;
		px->GetErrorMessage(sErrorMsg.GetBuffer(nMaxErrorMsgLength), nMaxErrorMsgLength);
		sErrorMsg.ReleaseBuffer();
		TRACE1("COXWorkspaceState::LoadFromFile : Catching CException (%s)\n", sErrorMsg);
#endif // _DEBUG

		bSuccess = FALSE;
	}
	END_CATCH_ALL


	// ... When loading apply the just read properties
	if (bApplyProperties && bSuccess)
		ApplyProperties();

	ASSERT_VALID(this);
	return bSuccess;
}

BOOL COXWorkspaceState::StoreToRegistry(LPCTSTR pszValueName /* = NULL */, 
										LPCTSTR pszCompany /* = NULL */, 
										LPCTSTR pszApplication /* = NULL*/ , 
										HKEY hKeyRoot /* = HKEY_CURRENT_USER */, 
										BOOL bSaveOpenDocs /* = TRUE */, 
										BOOL bComputeProperties /* = TRUE */, 
										BOOL bCloseApplication /* = TRUE */)
	{
	ASSERT_VALID(this);

	// ... When storing : first let all the documents save themselves
	//     Because this may change their name
	if (bSaveOpenDocs)
	{
		AfxGetApp()->SaveAllModified();

		// if this function has been called when application is about to be closed 
		// we don't want it to call SaveAllModified() function on the files that
		// user didn't want to save. in order to do that we have to go through 
		// all application's document and clear modified flag
		if(bCloseApplication && AfxGetApp()->m_pDocManager!=NULL)
		{
			POSITION pos=AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
			while(pos!=NULL)
			{
				CDocTemplate* pTemplate=
					AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
				ASSERT_KINDOF(CDocTemplate,pTemplate);
				POSITION posDoc=pTemplate->GetFirstDocPosition();
				while(posDoc!=NULL)
				{
					CDocument* pDoc=pTemplate->GetNextDoc(posDoc);
					pDoc->SetModifiedFlag(FALSE);
				}
			}
		}
	}

	// ... When storing first establish the values off all properties
	if (bComputeProperties)
		ComputeProperties();

	CString sValueName(pszValueName);
	CString sCompany(pszCompany);
	CString sApplication(pszApplication);
	CString sContents;

	if (sValueName.IsEmpty())
		sValueName = szWorkspaceStateDefaultValue;

	if (sCompany.IsEmpty())
		sCompany = AfxGetApp()->m_pszRegistryKey;
	if (sCompany.IsEmpty())
		{
		TRACE0("COXWorkspaceState::StoreToRegistry : No valid company name is provided, failing\n");
		return FALSE;
		}

	if (sApplication.IsEmpty())
		sApplication = AfxGetApp()->m_pszProfileName;

	// Get the contents from the list
	CByteArray binaryState;

	BOOL bSuccess = GetBinaryWorkspaceState(&binaryState);
	if (bSuccess)
		bSuccess = StoreLoadRegistry(TRUE, sValueName, sCompany, sApplication, hKeyRoot, &binaryState);

#ifdef _DEBUG
	if (!bSuccess)
		TRACE0("COXWorkspaceState::StoreToRegistry : Failed to store to registry\n");
#endif // _DEBUG

	ASSERT_VALID(this);
	return bSuccess;
	}

BOOL COXWorkspaceState::LoadFromRegistry(LPCTSTR pszValueName /* = NULL */, 
	LPCTSTR pszCompany /* = NULL */, LPCTSTR pszApplication /* = NULL*/ , 
	HKEY hKeyRoot /* = HKEY_CURRENT_USER */, BOOL bApplyProperties /* = TRUE */)
	{
	ASSERT_VALID(this);

	CString sValueName(pszValueName);
	CString sCompany(pszCompany);
	CString sApplication(pszApplication);
	CString sContents;

	if (sValueName.IsEmpty())
		sValueName = szWorkspaceStateDefaultValue;

	if (sCompany.IsEmpty())
		sCompany = AfxGetApp()->m_pszRegistryKey;
	if (sCompany.IsEmpty())
		{
		TRACE0("COXWorkspaceState::LoadFromRegistry : No valid company name is provided, failing\n");
		return FALSE;
		}

	if (sApplication.IsEmpty())
		sApplication = AfxGetApp()->m_pszProfileName;

	// Get the contents from the list
	CByteArray binaryState;

	BOOL bSuccess = StoreLoadRegistry(FALSE, sValueName, sCompany, sApplication, hKeyRoot, &binaryState);
	if (bSuccess)
		bSuccess = SetBinaryWorkspaceState(&binaryState);

	// ... When loading apply the just read properties
	if (bApplyProperties && bSuccess)
		ApplyProperties();

#ifdef _DEBUG
	if (!bSuccess)
		TRACE0("COXWorkspaceState::LoadFromRegistry : Failed to load from registry\n");
#endif // _DEBUG

	ASSERT_VALID(this);
	return bSuccess;
	}


#ifdef _DEBUG
void COXWorkspaceState::AssertValid() const
	{
	CObject::AssertValid();

	// The pointer object must always be valid and may never be NULL
	ASSERT_VALID(m_pDockState);
	ASSERT_VALID(m_pChildFrameStates);
	ASSERT_VALID(m_pSplitterPanes);
	}

void COXWorkspaceState::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	}
#endif //_DEBUG

COXWorkspaceState::~COXWorkspaceState()
	{
	ASSERT_VALID(this);

	// Empty all lists
	EmptyChildFrames(m_pChildFrameStates);
	EmptySplitterPanes(m_pSplitterPanes);

	// Delete dynamically created members
	delete m_pDockState;
	m_pDockState = NULL;
	delete m_pChildFrameStates;
	m_pChildFrameStates = NULL;
	delete m_pSplitterPanes;
	m_pSplitterPanes = NULL;
	}

// protected:
void COXWorkspaceState::Initialize()
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Initialized the data members of this object
	{
	m_screenSize.cx = ::GetSystemMetrics(SM_CXSCREEN);
	m_screenSize.cy = ::GetSystemMetrics(SM_CYSCREEN);

	m_bSaveMainWndProps = TRUE;
	::ZeroMemory(&m_mainWndPlacement, sizeof(WINDOWPLACEMENT));

	m_bSaveBarProps = TRUE;
	if (m_pDockState == NULL)
		m_pDockState = new CDockState();
	m_pDockState->Clear();

	if (m_pChildFrameStates == NULL)
		m_pChildFrameStates = new CObArray();
	m_bSaveChildFrames = TRUE;
	m_bSaveSplitterPanes = TRUE;
	EmptyChildFrames(m_pChildFrameStates);

	m_bSaveSplitterPanes = TRUE;
	if (m_pSplitterPanes == NULL)
		m_pSplitterPanes = new CObArray();
	EmptySplitterPanes(m_pSplitterPanes);

	m_nSerializeSchemaVersionLoad = -1;

	ASSERT_VALID(this);
	}

BOOL COXWorkspaceState::ComputeChildFrameState()
	// --- In  :
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Computes all the states of the child frame windows and adds them
	//				to an internal collection
	{
	// Iterate all views
	POSITION MDIChildWndPos = NULL;
	CFrameWnd* pFrameWnd = NULL;

	BOOL bAllSucceeded = TRUE;

	MDIChildWndPos = GetFirstMDIChildWndPosition();
	while(MDIChildWndPos != NULL)
		{
		pFrameWnd = GetNextMDIChildWnd(MDIChildWndPos);
		ASSERT(pFrameWnd != NULL);
		
		COXChildFrameState* pChildFrameState = new COXChildFrameState;
		pChildFrameState->IncludeSplitterPane(m_bSaveSplitterPanes);
		if (pChildFrameState->ComputeProperties(pFrameWnd))
			m_pChildFrameStates->Add(pChildFrameState);
		else
			{
			TRACE0("COXWorkspaceState::ComputeChildFrameState : Failed to compute properties\n");
			bAllSucceeded = FALSE;
			delete pChildFrameState;
			}
		}

	return bAllSucceeded;
	}

BOOL COXWorkspaceState::ApplyChildFrameState() const
	// --- In  :
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Applies all the states of the child frame windows that are in the
	//				internal collection
	{
	ASSERT_VALID(this);

	BOOL bAllSucceeded = TRUE;

	// Delegate the apply to all the child frame states in our list
	// Iterate the child frame sates in backward direction, this way
	// the last handles child will become the active one
	int nChildFrameIndex = 0;
	COXChildFrameState* pChildFrameState = NULL;
	for (nChildFrameIndex = PtrToInt(m_pChildFrameStates->GetSize() - 1); 0 <= nChildFrameIndex ; nChildFrameIndex--)
		{
		pChildFrameState = DYNAMIC_DOWNCAST(COXChildFrameState, m_pChildFrameStates->GetAt(nChildFrameIndex));
		if (pChildFrameState != NULL)
			if (!pChildFrameState->ApplyProperties())
				{
				TRACE0("COXChildFrameState::ApplyChildFrameState : Failed to apply properties\n");
				bAllSucceeded = FALSE;
				}
		}

	ASSERT_VALID(this);
	return bAllSucceeded;
	}

void COXWorkspaceState::StoreProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : Stores the properties of this object to archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsStoring());

	// Some sanity checks
	ASSERT((m_bSaveMainWndProps == FALSE) || (m_bSaveMainWndProps == TRUE));
	ASSERT((m_bSaveBarProps == FALSE) || (m_bSaveBarProps == TRUE));
	ASSERT((m_bSaveChildFrames == FALSE) || (m_bSaveChildFrames == TRUE));
	ASSERT((m_bSaveSplitterPanes == FALSE) || (m_bSaveSplitterPanes == TRUE));

	// ... General
	ar << m_screenSize;

	// ... Main window
	ar << m_bSaveMainWndProps;
	ar << m_mainWndPlacement.flags;
	ar << m_mainWndPlacement.showCmd;
	ar << m_mainWndPlacement.ptMinPosition;
	ar << m_mainWndPlacement.ptMaxPosition;
	ar << m_mainWndPlacement.rcNormalPosition;

	// ... Bars
	ar << m_bSaveBarProps;
	ar << m_pDockState;

	// ... Child frames
	ar << m_bSaveChildFrames;
	ar << m_bSaveSplitterPanes;
	ar << m_pChildFrameStates;

	m_mapBarID2TabIdx.Serialize(ar);

	// ... Splitter panes
	ar << m_bSaveSplitterPanes;
	ar << m_pSplitterPanes;

	ASSERT_VALID(this);
	}

void COXWorkspaceState::LoadProperties(CArchive& ar)
	// --- In  : ar : Archive used in serialization
	// --- Out : 
	// --- Returns :
	// --- Effect : loads the properties of this object from archive
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);
	ASSERT(ar.IsLoading());

	// ... General
	ar >> m_screenSize;

	// ... Main window
	ar >> m_bSaveMainWndProps;
	ar >> m_mainWndPlacement.flags;
	ar >> m_mainWndPlacement.showCmd;
	ar >> m_mainWndPlacement.ptMinPosition;
	ar >> m_mainWndPlacement.ptMaxPosition;
	ar >> m_mainWndPlacement.rcNormalPosition;

	// ... Bars
	CDockState* pOldDockState = NULL;
	ar >> m_bSaveBarProps;
	pOldDockState = m_pDockState;
	ar >> m_pDockState;
	delete pOldDockState;

	// ... Child frames
	ar >> m_bSaveChildFrames;
	ar >> m_bSaveSplitterPanes;
	CObArray* pOldChildFrameStates = NULL;
	pOldChildFrameStates = m_pChildFrameStates;
	ar >> m_pChildFrameStates;
	EmptyChildFrames(pOldChildFrameStates);
	delete pOldChildFrameStates;

	m_mapBarID2TabIdx.RemoveAll();
	m_mapBarID2TabIdx.Serialize(ar);

	// ... Splitter panes
	ar >> m_bSaveSplitterPanes;
	CObArray* pOldSplitterPanes= NULL;
	pOldSplitterPanes = m_pSplitterPanes;
	ar >> m_pSplitterPanes;
	EmptySplitterPanes(pOldSplitterPanes);
	delete pOldSplitterPanes;

	// Some sanity checks
	ASSERT((m_bSaveMainWndProps == FALSE) || (m_bSaveMainWndProps == TRUE));
	ASSERT((m_bSaveBarProps == FALSE) || (m_bSaveBarProps == TRUE));
	ASSERT((m_bSaveChildFrames == FALSE) || (m_bSaveChildFrames == TRUE));
	ASSERT((m_bSaveSplitterPanes == FALSE) || (m_bSaveSplitterPanes == TRUE));

	ASSERT_VALID(this);
	}

void COXWorkspaceState::StoreLoadFile(LPCTSTR pszPath, BOOL bStoring)
	// --- In  : pszPath : The file path to use
	//			 bStoring : Whether the properties should be stored or loaded
	// --- Out : 
	// --- Returns :
	// --- Effect : Stores or loads the properties of this object by using a file
	//				This action may throw an exception on failure
	{
	ASSERT_VALID(this);

	// Open the file
	CFile file;
	CFileException* pxFile = NULL;
	UINT nOpenFlags = 0;

	if (bStoring)
		nOpenFlags = CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive;
	else
		nOpenFlags = CFile::modeRead | CFile::shareDenyWrite;
	pxFile = new CFileException;
	if (!file.Open(pszPath, nOpenFlags, pxFile))
		THROW(pxFile);
	else
		pxFile->Delete();

	// Wrap a CArchive around it
	CArchive ar(&file, bStoring ? CArchive::store : CArchive::load);

	// Serialize the properties
	Serialize(ar);

	// Clean everything up
	ar.Close();
	file.Close();

	ASSERT_VALID(this);
	}


BOOL COXWorkspaceState::SetBinaryWorkspaceState(CByteArray* pBinaryState)
	// --- In  : pBinaryState : Valid CByteArray object
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Serializes a binary represntation to this object (load)
	{
	ASSERT(pBinaryState != NULL);
	ASSERT_VALID(this);

	BOOL bSuccess = TRUE;
	TRY
		{
		// Use a memory file to serialize the data in memory
		CMemFile memFile;

		// Copy CByteArray to a memFile
		BYTE* pBinData = NULL;
		int nDataSize = 0;

		nDataSize = PtrToInt(pBinaryState->GetSize());
		pBinData = pBinaryState->GetData();
		ASSERT((nDataSize == 0) || AfxIsValidAddress(pBinData, nDataSize, FALSE));
		memFile.Write(pBinData, nDataSize);
		memFile.Seek(0, CFile::begin);

		// ... Wrap a CArchive around it
		CArchive ar(&memFile, CArchive::load);
		// ... Serialize the properties
		Serialize(ar);

		// Clean everything up
		ar.Close();
		memFile.Close();
		}
	CATCH_ALL(px)
		{
#ifdef _DEBUG
		const int nMaxErrorMsgLength = 1024;
		CString sErrorMsg;
		px->GetErrorMessage(sErrorMsg.GetBuffer(nMaxErrorMsgLength), nMaxErrorMsgLength);
		sErrorMsg.ReleaseBuffer();
		TRACE1("COXWorkspaceState::SetBinaryWorkspaceState : Catching CException (%s)\n", sErrorMsg);
#endif // _DEBUG

		bSuccess = FALSE;
		}
	END_CATCH_ALL

	ASSERT_VALID(this);
	return bSuccess;
	}

BOOL COXWorkspaceState::GetBinaryWorkspaceState(CByteArray* pBinaryState)
	// --- In  : pBinaryState : Valid CByteArray object
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Serializes this object to a binary represntation (store)
	{
	ASSERT(pBinaryState != NULL);
	ASSERT_VALID(this);

	BOOL bSuccess = TRUE;
	TRY
		{
		// Use a memory file to serialize the data in memory
		CMemFile memFile;

		// ... Wrap a CArchive around it
		CArchive ar(&memFile, CArchive::store);
		// ... Serialize the properties
		Serialize(ar);
		// ... Flush and close
		ar.Close();

		// Copy memfile contents to CByteArray
		BYTE* pBinData = NULL;
		BYTE* pMemData = NULL;
		int nDataSize = 0;

		nDataSize = (int) memFile.GetLength();
		pBinaryState->SetSize(nDataSize);
		pBinData = pBinaryState->GetData();
		ASSERT(AfxIsValidAddress(pBinData, nDataSize, TRUE));
		pMemData  = memFile.Detach();
		ASSERT(AfxIsValidAddress(pMemData, nDataSize, FALSE));
		::CopyMemory(pBinData, pMemData, nDataSize);

		// Clean everything up
		memFile.Close();
		free(pMemData);
		}
	CATCH_ALL(px)
		{
#ifdef _DEBUG
		const int nMaxErrorMsgLength = 1024;
		CString sErrorMsg;
		px->GetErrorMessage(sErrorMsg.GetBuffer(nMaxErrorMsgLength), nMaxErrorMsgLength);
		sErrorMsg.ReleaseBuffer();
		TRACE1("COXWorkspaceState::GetBinaryWorkspaceState : Catching CException (%s)\n", sErrorMsg);
#endif // _DEBUG

		bSuccess = FALSE;
		}
	END_CATCH_ALL

	ASSERT_VALID(this);
	return bSuccess;
	}

BOOL COXWorkspaceState::StoreLoadRegistry(BOOL bStoring, LPCTSTR pszValueName, 
	LPCTSTR pszCompany, LPCTSTR pszApplication, HKEY hKeyRoot, CByteArray* pBinaryState)
	// --- In  : bStoring : Whether the value should be stored or loaded
	//			 pszValueName   The name of the value to set. 
	//				(A default name will be used when this is NULL)
	//			 pszCompany : The name of the subkey with which a value is associated. 
	//				If the parameter is NULL, AfxGetApp()->m_pszRegistryKey is used 
	//				(use AfxGetApp()->SetRegistryKey() to set this to your company name)
	//			 pszApplication : Name of this application
	//				If the paramater is NULL, AfxGetApp()->m_pszProfileName is used
	//			 hKeyRoot : An open key in the registry or any of the following predefined handle values: 
	//				HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS
	//			 bSaveOpenDocs : Whether open document that have been changed should be
	//				saved first (unnamed docs will then receive a name)
	//			 bComputeProperties :  Whether the current workspace should be recomputed
	//				(Same as ComputeProperties())
	// --- Out : 
	// --- Returns :
	// --- Effect : Stores or loads the properties of this object using the registry
	{
	ASSERT(hKeyRoot != NULL);
	ASSERT((pszCompany != NULL) && (*pszCompany != _T('\0')));
	ASSERT((pszApplication != NULL) && (*pszApplication != _T('\0')));
	ASSERT((pszValueName != NULL) && (*pszValueName != _T('\0')));
	ASSERT(pBinaryState != NULL);

	REGSAM samDesired = 0;
	if (bStoring)
		samDesired = KEY_WRITE | KEY_READ;
	else
		samDesired = KEY_READ;

	// Open key for hKeyRoot\<szSoftware>\<pszCompany>\<pszApplication>\<szWorkspaceState>
	HKEY hSoftwareKey = NULL;
	HKEY hCompanyKey = NULL;
	HKEY hApplicationKey = NULL;
	HKEY hWorkspaceKey = NULL;
	if (::RegOpenKeyEx(hKeyRoot, szSoftware, 0, samDesired,
		&hSoftwareKey) == ERROR_SUCCESS)
		{
		DWORD dw;
		if (::RegCreateKeyEx(hSoftwareKey, pszCompany, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, samDesired, NULL, &hCompanyKey, &dw) == ERROR_SUCCESS)
			{
			if (::RegCreateKeyEx(hCompanyKey, pszApplication, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, samDesired, NULL, &hApplicationKey, &dw) == ERROR_SUCCESS)
				{
				::RegCreateKeyEx(hApplicationKey, szWorkspaceState, 0, REG_NONE,
					REG_OPTION_NON_VOLATILE, samDesired, NULL, &hWorkspaceKey, &dw);
				}
			}
		}
	if (hSoftwareKey != NULL)
		::RegCloseKey(hSoftwareKey);
	if (hCompanyKey != NULL)
		::RegCloseKey(hCompanyKey);
	if (hApplicationKey != NULL)
		::RegCloseKey(hApplicationKey);

	if (hWorkspaceKey == NULL)
		{
		TRACE0("COXWorkspaceState::StoreLoadRegistry : Failed to open workspace key, failing\n");
		if (!bStoring)
			pBinaryState->RemoveAll();
		return FALSE;
		}

	LONG nResult = 0;
	if (bStoring)
		{
		// Store the value
		nResult = ::RegSetValueEx(hWorkspaceKey, pszValueName, NULL, REG_BINARY,
			(LPBYTE)pBinaryState->GetData(), PtrToLong(pBinaryState->GetSize()));
		}
	else
		{
		// Load the value
		DWORD dwType, dwCount;
		nResult = ::RegQueryValueEx(hWorkspaceKey, (LPTSTR)pszValueName, NULL, &dwType,
			NULL, &dwCount);
		if (nResult == ERROR_SUCCESS)
			{
			// ... Workspace must be save in binary format
			ASSERT(dwType == REG_BINARY);
			pBinaryState->SetSize(dwCount);
			nResult = ::RegQueryValueEx(hWorkspaceKey, (LPTSTR)pszValueName, NULL, &dwType,
				(LPBYTE)pBinaryState->GetData(), &dwCount);
			ASSERT(dwType == REG_BINARY);
			}
		}
	::RegCloseKey(hWorkspaceKey);

	if (nResult != ERROR_SUCCESS)
		{
		TRACE1("COXWorkspaceState::StoreLoadRegistry : Failed to access workspace value (%s), failing\n",
			pszValueName);
		if (!bStoring)
			pBinaryState->RemoveAll();
		return FALSE;
		}

	return TRUE;
	}

void COXWorkspaceState::EmptyChildFrames(CObArray* pChildFrameStates)
	// --- In  : pChildFrameStates: Collection to use
	// --- Out : 
	// --- Returns :
	// --- Effect : Clears the collection by deleting all its members and 
	//				clearing the collection itself
	{
	ASSERT_VALID(pChildFrameStates);

	int nChildFrameIndex = 0;
	CObject* pChildFrameState = NULL;
	for (nChildFrameIndex = 0; nChildFrameIndex < pChildFrameStates->GetSize(); nChildFrameIndex++)
		{
		pChildFrameState = pChildFrameStates->GetAt(nChildFrameIndex);
		delete pChildFrameState;
		pChildFrameStates->SetAt(nChildFrameIndex, NULL);
		}
	pChildFrameStates->RemoveAll();
	}

POSITION COXWorkspaceState::GetFirstMDIChildWndPosition()
	// --- In  :
	// --- Out : 
	// --- Returns : The first position of the child windows
	//				 (NULL if the end has been reached)
	// --- Effect : Used to iterate all the MDI child windows
	{
	CMDIFrameWnd* pMainMDIFrameWnd = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
	if (pMainMDIFrameWnd == NULL)
		{
		TRACE0("COXWorkspaceState::GetFirstMDIChildWndPosition : Unable to get the main frame window\n");
		return NULL;
		}

	HWND hChildWnd = ::GetWindow(pMainMDIFrameWnd->m_hWndMDIClient, GW_CHILD);
	CWnd* pChildWnd = CWnd::FromHandlePermanent(hChildWnd);
	CMDIChildWnd* pMDIChildWnd = DYNAMIC_DOWNCAST(CMDIChildWnd, pChildWnd);
	if ((pMDIChildWnd != NULL) && (pMDIChildWnd->GetWindow(GW_OWNER) == NULL))
		return (POSITION)hChildWnd;
	else
		return NULL;
	}

CMDIChildWnd* COXWorkspaceState::GetNextMDIChildWnd(POSITION& pos)
	// --- In  : pos : The current position in the iteration
	// --- Out : The next position in the iteration (NULL if the end has been reached)
	// --- Returns : The current child frame window
	// --- Effect : Used to iterate all the MDI child windows
	{
	CMDIChildWnd* pPrevMDIChildWnd = DYNAMIC_DOWNCAST(CMDIChildWnd, 
		CWnd::FromHandlePermanent((HWND)pos));
	ASSERT(pPrevMDIChildWnd != NULL);

	HWND hChildWnd = ::GetWindow(pPrevMDIChildWnd->GetSafeHwnd(), GW_HWNDNEXT);
	CWnd* pChildWnd = CWnd::FromHandlePermanent(hChildWnd);
	CMDIChildWnd* pMDIChildWnd = DYNAMIC_DOWNCAST(CMDIChildWnd, pChildWnd);
	if ((pMDIChildWnd != NULL) && (pMDIChildWnd->GetWindow(GW_OWNER) == NULL))
		pos = (POSITION)hChildWnd;
	else
		pos = NULL;

	return pPrevMDIChildWnd;
	}

BOOL COXWorkspaceState::ComputeSplitterPanes(CFrameWnd* pFrameWnd)
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

BOOL COXWorkspaceState::ApplySplitterPanes(CFrameWnd* pFrameWnd) const
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

void COXWorkspaceState::EmptySplitterPanes(CObArray* pSplitterPanes)
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

CSplitterWnd* COXWorkspaceState::GetSplitterWindow(CFrameWnd* pFrameWnd)
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

void COXWorkspaceState::ComputeDockingWindowsState(CPtrArray& aryBarInfo)
{
	// Get the main window and the main frame
	CFrameWnd* pMainFrame = NULL;
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd != NULL)
		pMainFrame = pMainWnd->GetTopLevelFrame();

	// Loop through all control bars and populate the m_mapBarID2TabIdx map
	m_mapBarID2TabIdx.RemoveAll();
	int iSize = PtrToInt(aryBarInfo.GetSize());
	for (int i = 0; i < iSize; i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*) aryBarInfo[i];
		ASSERT(pInfo != NULL);
		pInfo->m_pBar = pMainFrame->GetControlBar(pInfo->m_nBarID);
		if (pInfo->m_pBar != NULL)
		{
			COXSizeControlBar* pSzBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pInfo->m_pBar);
			if (pSzBar != NULL)
			{
				// Get the tab index
				COXSizeDockBar* pSzDockBar = (COXSizeDockBar*) pSzBar->m_pDockBar;
				if (pSzDockBar != NULL && ::IsWindow(pSzDockBar->m_wndDockTabCtrl))
				{
					OXDOCKTABPOSITION oxdtp;
					oxdtp.nControlBarID = pInfo->m_nBarID;
					oxdtp.nDockBarID = pSzDockBar->GetDlgCtrlID();
					oxdtp.iTabIndex = pSzDockBar->m_wndDockTabCtrl.FindTab(pSzBar);
					if (oxdtp.iTabIndex == pSzDockBar->m_wndDockTabCtrl.GetCurSel())
						oxdtp.bSelected = TRUE;
					else
						oxdtp.bSelected = FALSE;

					m_mapBarID2TabIdx.SetAt(pInfo->m_nBarID, oxdtp);
				}
			}
		}
	}
}

void COXWorkspaceState::ApplyDockingWindowsState(CPtrArray& aryBarInfo) const
{
	// Get the main window and the main frame
	CFrameWnd* pMainFrame = NULL;
	CWnd* pMainWnd = AfxGetMainWnd();
	if (pMainWnd != NULL)
		pMainFrame = pMainWnd->GetTopLevelFrame();

	// Loop through all control bars and add then to their appropriate tab
	int iSize = PtrToInt(aryBarInfo.GetSize());
	for (int i = 0; i < iSize; i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*) aryBarInfo[i];
		ASSERT(pInfo != NULL);
		pInfo->m_pBar = pMainFrame->GetControlBar(pInfo->m_nBarID);
		if (pInfo->m_pBar != NULL)
		{
			COXSizeControlBar* pSzBar = DYNAMIC_DOWNCAST(COXSizeControlBar, pInfo->m_pBar);
			if (pSzBar != NULL)
			{
				// Lookup this bar in m_mapBarID2TabIdx
				OXDOCKTABPOSITION oxdtp;
				BOOL bFound = m_mapBarID2TabIdx.Lookup(pInfo->m_nBarID, oxdtp);

				if (bFound && oxdtp.iTabIndex != -1)
				{
					// Add this bar to its tab control
					DWORD arDockPos[4] = {AFX_IDW_DOCKBAR_LEFT, AFX_IDW_DOCKBAR_RIGHT,
						AFX_IDW_DOCKBAR_TOP, AFX_IDW_DOCKBAR_BOTTOM};
					for (int i = 0; i < 4; i++)
					{
						COXSizeDockBar* pDockBar = DYNAMIC_DOWNCAST(COXSizeDockBar,
							pMainFrame->GetControlBar(arDockPos[i]));
						if (pDockBar != NULL && oxdtp.nDockBarID == arDockPos[i])
						{
							// This is the one
							int iIndex = pDockBar->m_wndDockTabCtrl.FindTab(pSzBar);
							if (iIndex == -1)
							{
								// Remove if tabbed to another dockbar
								/*
								COXSizeDockBar* pOtherDockBar = DYNAMIC_DOWNCAST(COXSizeDockBar, pSzBar->m_pDockBar);
								if (pOtherDockBar != pDockBar)
								{
								}
								*/
								pDockBar->m_wndDockTabCtrl.InsertTab(pSzBar, oxdtp.iTabIndex, TRUE);



							}
							/*
							if (oxdtp.bSelected)
							{
								pDockBar->m_wndDockTabCtrl.SetCurSel(oxdtp.iTabIndex);
							}
							*/
						}
					}
				}
				else
				{
					// Remove this from its tab control

					// Check the left, top, right and bottom dock bars
					DWORD arDockPos[4] = {AFX_IDW_DOCKBAR_LEFT, AFX_IDW_DOCKBAR_RIGHT,
						AFX_IDW_DOCKBAR_TOP, AFX_IDW_DOCKBAR_BOTTOM};
					for (int i = 0; i < 4; i++)
					{
						COXSizeDockBar* pDockBar = DYNAMIC_DOWNCAST(COXSizeDockBar,
							pMainFrame->GetControlBar(arDockPos[i]));
						if (pDockBar != NULL)
						{
							int iIndex = pDockBar->m_wndDockTabCtrl.FindTab(pSzBar);
							if (iIndex != -1)
								pDockBar->m_wndDockTabCtrl.RemoveTab(pSzBar);
						}
					}
				}
			}
		}
	}
}

// private:

#endif // _MFC_VER
// ==========================================================================
