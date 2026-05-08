// ==========================================================================
//				Class Implementation : COXMetaFile
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
			  
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXMetaFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

COXMetaFile::COXMetaFile()
{
	m_hDCMeta = NULL;
	m_hEMF = NULL;
	m_lpEnhMetaProc = NULL;
	m_rectBounds.SetRectEmpty();
	m_rectNormalized.SetRectEmpty();
	m_metafileType=OXMETAFILE_NOTDEFINED;
}

COXMetaFile::~COXMetaFile()
{ 
	if(m_hEMF)
		DeleteEnhMetaFile(m_hEMF);
} 
/////////////////////////////////////////
// fileaccess
/////////////////////////////////////////

// ==================   Load File   ==================================
HENHMETAFILE COXMetaFile::LoadFile(int nID, CString strResType)
{   
	LPVOID lpvData;
	HANDLE hResource;
	HRSRC hRsrc = FindResource(AfxGetApp()->m_hInstance, 
				MAKEINTRESOURCE(nID), strResType);
	
	// Load MetaFile metafile from resource
	hResource = LoadResource(AfxGetApp()->m_hInstance, hRsrc);
	if(!hResource)
	{	
		TRACE(_T("MetaFile %d not found in resource\n"), nID);
		return NULL;
	}
	lpvData = LockResource(hResource) ;
	LoadFile(lpvData);
    UnlockResource(hResource) ;
	return m_hEMF;
}

HENHMETAFILE COXMetaFile::LoadFile(CString strFileName)
{	
    HANDLE hFile, hMapFile;
    LPVOID pMapFile;
    BOOL bSuccess = TRUE;

	// ask for filename, if not given as parameter
	if(! strFileName.GetLength())
	{	
		static TCHAR BASED_CODE szFilter[] = 
			_T("Windows MetaFile(*.wmf)|*.wmf|Enhanced MetaFile(*.emf)|*.emf||");
		CFileDialog LoadMetaFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
		if(LoadMetaFile.DoModal() != IDOK) 
			return 0L;
		m_strFileName = LoadMetaFile.GetPathName(); 
	}
	else
		m_strFileName = strFileName;

	// open file
    if((hFile = ::CreateFile((LPCTSTR) m_strFileName, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) ==(HANDLE)-1) 
	{	
		TRACE(_T("Fileopen Error\n"));
		return 0L;
	}
    // Create a map file of the opened file
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, _T("MapF"));
    if(hMapFile == NULL) 
	{   
		TRACE(_T("Error in CreateFileMapping\n"));
       	CloseHandle(hFile);
		return 0L;
	}
    // Map a view of the whole file
	pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
    if(pMapFile == NULL) 
	{
		TRACE(_T("Error in MapViewOfFile\n"));
        bSuccess = FALSE;
        CloseHandle(hMapFile);
		CloseHandle(hFile);
		return 0L;
	}
	// LoadFile does the hard work
	m_hEMF = LoadFile(pMapFile);
	UnmapViewOfFile(pMapFile);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	return m_hEMF;
}


// =============== CreateFile & CloseFile
// ----------------------------------------------
HDC COXMetaFile::CreateFile(CString strFileName, CString strDescript, 
							CString sDefExt/* = _T("emf")*/, 
							int nIDFilter/*"EMF Files(*.emf)|*.emf|All Files(*.*)|*.*|"*/)
{	
	m_hDCMeta = NULL;
	CString sFilter;
	VERIFY(sFilter.LoadString(nIDFilter));
	// Dlg 
	if(! strFileName.GetLength())
	{	
		CString sFileName=_T("*.");
		sFileName+=sDefExt;
		CFileDialog NewMetaFile(FALSE, sDefExt, sFileName, 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter);
		if(NewMetaFile.DoModal() != IDOK) 
			return NULL; 

		m_strFileName = NewMetaFile.GetPathName(); 
	}
	else
		m_strFileName = strFileName;
	m_hDCMeta = CreateEnhMetaFile(NULL, m_strFileName, NULL, strDescript) ;
#ifdef _DEBUG
	if(!m_hDCMeta) 
		AfxMessageBox(_T("Error in CreateEnhMetaFile\n")+strDescript); 
#endif
 	return m_hDCMeta;
}

void COXMetaFile::CloseFile()
{	
	if(m_hDCMeta)
		m_hEMF=CloseEnhMetaFile(m_hDCMeta);
	if(m_hEMF)
		DeleteEnhMetaFile(m_hEMF);
	m_hDCMeta = NULL;
	m_hEMF = NULL;
	m_rectBounds.SetRectEmpty();
	m_rectNormalized.SetRectEmpty();
	m_metafileType=OXMETAFILE_NOTDEFINED;
}

// ================ PlayFile - PlayRecords =======================
//
BOOL COXMetaFile::PlayFile(CDC* pDC)
{	
	CRect cRect(0,0,m_rectBounds.right-m_rectBounds.left,
		m_rectBounds.bottom-m_rectBounds.top);
	return PlayEnhMetaFile(pDC->m_hDC,m_hEMF,&cRect); 
}

BOOL COXMetaFile::PlayFile(CDC* pDC, CRect* pClientRect)
{	
	BOOL bReturn;
	CRect rectBounds;
	int nOldMapMode = pDC->GetMapMode();
	CSize OldWndExt, OldViewExt;

	rectBounds.SetRectEmpty();
	if(pDC->IsPrinting())
	{	
		rectBounds.right = pDC->GetDeviceCaps(HORZRES);
		rectBounds.bottom= pDC->GetDeviceCaps(VERTRES);
	}
	else
		rectBounds = * pClientRect;
	ASSERT(rectBounds!=(0,0,0,0));
	if(!m_hEMF) 
		return FALSE;
	pDC->SetMapMode(MM_ISOTROPIC);
	// draw the MetaFile
	// Ajust size of Window to MetaFile
	OldWndExt = pDC->SetWindowExt(GetSize());
	OldViewExt = pDC->SetViewportExt(rectBounds.right, rectBounds.bottom);
	bReturn = PlayFile(pDC);
	pDC->SetWindowExt(OldWndExt);
	pDC->SetViewportExt(OldViewExt);
	pDC->SetMapMode(nOldMapMode);
	return bReturn;
}


BOOL COXMetaFile::PlayRecords(CDC* pDC, CRect crect, 
							   ENHMFENUMPROC lpEnhMetaProc, LPVOID plData)
{	
	if((!m_lpEnhMetaProc) &&(lpEnhMetaProc))
		m_lpEnhMetaProc = lpEnhMetaProc;
	ASSERT(m_lpEnhMetaProc);
	return EnumEnhMetaFile(pDC->m_hDC, m_hEMF, m_lpEnhMetaProc, plData, crect); 
}


// =========   private helper functions
// ------------------------------------------
HENHMETAFILE COXMetaFile::LoadFile(LPVOID lpvData)
{	
	unsigned long lSize;
	ALDUSHEADER* pAldusMF;
    LPENHMETAHEADER pemh =(LPENHMETAHEADER) lpvData;
	ENHMETAHEADER EnhMetaHeader;

	if(m_hEMF)
	{
		DeleteEnhMetaFile(m_hEMF);
		m_hEMF=NULL;
	}

	// get size, if it makes sense
	// Obtain a handle to a reference device context  
	HDC hdcRef=::GetDC(NULL);  
	// Determine the picture frame dimensions. 
	// iWidthMM is the display width in millimeters. 
	// iHeightMM is the display height in millimeters. 
	// iWidthPels is the display width in pixels. 
	// iHeightPels is the display height in pixels  
	int iWidthMM=GetDeviceCaps(hdcRef,HORZSIZE); 
	int iHeightMM=GetDeviceCaps(hdcRef,VERTSIZE); 
	int iWidthPels=GetDeviceCaps(hdcRef,HORZRES); 
	int iHeightPels=GetDeviceCaps(hdcRef,VERTRES);  
	::ReleaseDC(NULL,hdcRef);


    // ============== 1) is it an enhanced MetaFile ==================================
	if(pemh->dSignature == META32_SIGNATURE) 
	{
		TRACE(_T("it's an Enhanced MetaFile\n"));
		lSize = pemh->nBytes;
		m_hEMF = SetEnhMetaFileBits(lSize,(CONST BYTE*) lpvData) ;
		m_strDescription.Empty();
		// MetaFileHeader lesen
		if(m_hEMF)
		{	
			GetEnhMetaFileHeader(m_hEMF,sizeof(EnhMetaHeader),&EnhMetaHeader);
			m_rectNormalized.SetRect(EnhMetaHeader.rclFrame.left, 
				EnhMetaHeader.rclFrame.top,EnhMetaHeader.rclFrame.right, 
				EnhMetaHeader.rclFrame.bottom);
			m_rectBounds.SetRect((m_rectNormalized.left*iWidthPels)/(iWidthMM*100),
				(m_rectNormalized.top*iHeightPels)/(iHeightMM*100),
				(m_rectNormalized.right*iWidthPels)/(iWidthMM*100),
				(m_rectNormalized.bottom*iHeightPels)/(iHeightMM*100));
			if(EnhMetaHeader.nDescription>0)
			{
				GetEnhMetaFileDescription(m_hEMF, EnhMetaHeader.nDescription, 
				m_strDescription.GetBufferSetLength(EnhMetaHeader.nDescription));
				m_strDescription.ReleaseBuffer();
			}
			else
				m_strDescription.Empty();

		}
		m_metafileType=OXMETAFILE_ENHANCED;
		return m_hEMF;
	}

	// ===============2) is is an Aldus MetaFile ==================================
	// If it has an ALDUS header skip it
    // Notice: APMSIZE is used instead of sizeof(ALDUSHEAD) because the 
	//	HANDLE and RECT of the structure
    //         depends on the environment and the align value
    if(*((LPDWORD)lpvData) == ALDUS_ID) 
	{	
		TRACE(_T("It's an Aldus MetaFile\n"));
		pAldusMF =(ALDUSHEADER*) lpvData;
        lSize = *((LPDWORD)((PBYTE)lpvData + APMSIZE + 6));
        // Notice: mtSize is size of the file in word.
        // if LPMETAFILEPICT is NULL
        //    MM_ANISOTROPIC mode and default device size will be used.
        m_hEMF=::SetWinMetaFileBits(lSize*2L,(PBYTE)lpvData+APMSIZE,NULL,NULL);
		VERIFY(m_strDescription.LoadString(IDS_OX_METAFILEALDUS)); //"WMF - Aldus Format"
		if(!m_hEMF) 
		{ 
			TRACE(_T("Error in SetWinMetaFileBits in Aldus MetaFile\n"));
			return 0L;
		}
		m_rectNormalized.SetRect(0,0,
			((pAldusMF->right-pAldusMF->left)*2540)/((int)pAldusMF->inch),
			((pAldusMF->bottom-pAldusMF->top)*2540)/((int)pAldusMF->inch));
		m_rectBounds.SetRect((m_rectNormalized.left*iWidthPels)/(iWidthMM*100),
			(m_rectNormalized.top*iHeightPels)/(iHeightMM*100),
			(m_rectNormalized.right*iWidthPels)/(iWidthMM*100),
			(m_rectNormalized.bottom*iHeightPels)/(iHeightMM*100));

		m_metafileType=OXMETAFILE_ALDUS;
		return m_hEMF;
    }
	// ================== 3) old Windows MetaFile ========================
    TRACE(_T("I try old Windows 3x MetaFile\n"));
	lSize = *((LPDWORD)((PBYTE)lpvData + 6));
    m_hEMF=::SetWinMetaFileBits(lSize*2L,(PBYTE)lpvData , NULL, NULL);
	VERIFY(m_strDescription.LoadString(IDS_OX_METAFILEWINDOWS)) ; //"Windows 3.x MetaFile"
	m_rectBounds.SetRect(0,0,::GetSystemMetrics(SM_CXSCREEN), 
		::GetSystemMetrics(SM_CYSCREEN));
	m_rectNormalized.SetRect(0,0,
		(m_rectBounds.Width()*iWidthMM*100)/iWidthPels,
		(m_rectBounds.Height()*iHeightMM*100)/iHeightPels);
	if(!m_hEMF) 
	{
	   TRACE(_T("Error on SetWinMetaFileBits in old windows MetaFile\n"));
	}
	else
		m_metafileType=OXMETAFILE_WIN16;
	return m_hEMF;
}


