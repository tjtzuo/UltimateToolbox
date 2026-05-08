// ==========================================================================
// 							Class Implementation : COXPreviewDialog
// ==========================================================================

// Source file : OXPreviewDialog.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"					// standard MFC include
#include <dlgs.h>
#include "OXPreviewDialog.h"		// class specification
#include "OXMainRes.h"

#include "WTypes.h"

#ifndef __OXMFCIMPL_H__
#if _MFC_VER >= 0x0700
#if _MFC_VER >= 1400
#include <afxtempl.h>
#endif
#include <..\src\mfc\afximpl.h>
#else
#include <..\src\afximpl.h>
#endif
#define __OXMFCIMPL_H__
#endif

#ifndef MORE_THAN_ONE
#define MORE_THAN_ONE						0xffff
#endif

#ifndef OX_MIN_PREVIEWWND_WIDTH
#define OX_MIN_PREVIEWWND_WIDTH				50
#endif

#ifndef OX_MIN_SHELLITEMSLISTCTRL_WIDTH
#define OX_MIN_SHELLITEMSLISTCTRL_WIDTH		50
#endif

#ifndef OX_IDC_SET_PREVIEW
#define OX_IDC_SET_PREVIEW					0xa100
#endif

#ifndef WM_OX_LOAD_PREVIEWWND_OFFSET
#define WM_OX_LOAD_PREVIEWWND_OFFSET		WM_APP+20
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(COXPreviewWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////

void COXPreviewWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	ASSERT(m_pPreviewDlg!=NULL);
	m_pPreviewDlg->OnPaintPreview(&dc,rect);
}


BOOL COXPreviewWnd::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}


BOOL COXPreviewWnd::Create(CWnd* pParent, CRect rect, UINT nID)
{
	return CWnd::CreateEx(WS_EX_STATICEDGE,
		AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW),
		(HBRUSH)(COLOR_BTNFACE+1),0),_T(""),WS_CHILD|WS_VISIBLE,rect,pParent,nID);
}

//////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_GRAPHICS
BOOL COXGraphicsFileViewer::OnNewFileSelect(const CString& sFilename, 
											BOOL bViewerFound/*=FALSE*/)
{
	COXFilePath pathPreview(sFilename);

	BOOL bSuccess=FALSE;
	if(!bViewerFound && !sFilename.IsEmpty() && pathPreview.Exists())
	{
		CWaitCursor wait;

#ifdef OX_PREVIEW_ICON
		if(!bSuccess && ::IsWindow(m_imageListBox.GetSafeHwnd()))
		{
			m_imageListBox.EmptyImageList();
			CString sExtension=pathPreview.GetExtender();
			if(sExtension.CompareNoCase(_T("ico"))==0 || 
				sExtension.CompareNoCase(_T("cur"))==0 ||
				sExtension.CompareNoCase(_T("ani"))==0 ||
				sExtension.CompareNoCase(_T("icl"))==0)
			{
				if(m_imageListBox.LoadIconsFromFile(sFilename,FALSE))
				{
					bSuccess=(m_imageListBox.GetCount()>0);
					if(bSuccess)
					{
						m_imageListBox.ShowWindow(SW_SHOWNA);
					}
				}
			}
		}
		if(!bSuccess && ::IsWindow(m_imageListBox.GetSafeHwnd()))
		{
			m_imageListBox.ShowWindow(SW_HIDE);
		}
#endif

#ifdef OX_PREVIEW_BMP
		if(!bSuccess)
		{
#ifdef OXDIB_SUPPORTJPEG
			bSuccess=m_dib.ReadJPEG(pathPreview.GetPath());
#endif //  OXDIB_SUPPORTJPEG
			if(!bSuccess)
			{
				bSuccess=m_dib.Read(pathPreview.GetPath());
			}
#endif
#ifdef OX_PREVIEW_METAFILE
			if(!bSuccess)
			{
				bSuccess=(m_MetaFile.LoadFile(pathPreview.GetPath())!=NULL);
			}
			else
			{
				m_MetaFile.CloseFile();
			}
#endif
		}
#ifdef OX_PREVIEW_BMP
		else
		{
			m_dib.Empty();
		}
	}
#endif

	if(!bSuccess)
	{
#ifdef OX_PREVIEW_BMP
		m_dib.Empty();
#endif
#ifdef OX_PREVIEW_METAFILE
		m_MetaFile.CloseFile();
#endif
#ifdef OX_PREVIEW_ICON
		if(::IsWindow(m_imageListBox.GetSafeHwnd()))
		{
			m_imageListBox.EmptyImageList();
			m_imageListBox.ShowWindow(SW_HIDE);
		}
#endif
	}

	m_bCanPreview=bSuccess;

	return bSuccess;
}

BOOL COXGraphicsFileViewer::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	if(!CanPreview())
	{
		return FALSE;
	}

	BOOL bSuccess=FALSE;

#ifdef OX_PREVIEW_ICON
	if(::IsWindow(m_imageListBox.GetSafeHwnd()) && m_imageListBox.GetCount()>0)
	{
		bSuccess=TRUE;
	}
#endif

#ifdef OX_PREVIEW_BMP
	if(!bSuccess && !m_dib.IsEmpty())
	{
		CRect dibRect;
		CSize dibSize;
		dibSize = m_dib.GetSize();
		dibRect.SetRect(0,0, dibSize.cx, dibSize.cy);

		CPalette* pPalette = m_dib.GetPalette();
		if (pPalette != NULL)
		{
			CPalette* pOldPalette = pDC->SelectPalette(pPalette,FALSE);
			pDC->RealizePalette();                                    
			pDC->SelectPalette(pOldPalette, FALSE);
		}

		CRect rect=COXPreviewDialog::AdjustRectToFit(dibRect,paintRect);
		m_dib.Paint(pDC,rect,dibRect);
		pDC->ExcludeClipRect(rect);

		// fill the background
		pDC->FillSolidRect(paintRect,::GetSysColor(COLOR_BTNFACE));

		bSuccess=TRUE;
	}
#endif		

#ifdef OX_PREVIEW_METAFILE
	if(!bSuccess)
	{
		if(m_MetaFile.GethEMF()!=NULL)
		{
			// fill the background
			pDC->FillSolidRect(paintRect,::GetSysColor(COLOR_BTNFACE));
			// paint the metafile
			CRect rect=paintRect;
			if(m_MetaFile.PlayFile(pDC,&rect))
			{
				pDC->ExcludeClipRect(rect);
				bSuccess=TRUE;
			}
		}
	}
#endif	

	return bSuccess;
}

BOOL COXGraphicsFileViewer::OnDoRealizePalette(CWnd* pFocusWnd)
{
#ifndef OX_PREVIEW_BMP
	UNREFERENCED_PARAMETER(pFocusWnd);
#endif

	if(!CanPreview())
	{
		return FALSE;
	}

#ifdef OX_PREVIEW_ICON
	if(::IsWindow(m_imageListBox.GetSafeHwnd()) && m_imageListBox.GetCount()>0)
	{
		return TRUE;
	}
#endif

#ifdef OX_PREVIEW_BMP
	if(m_dib.IsEmpty())
	{
		return FALSE;
	}

	UINT nColorsChanged = 0;

	CPalette* pPalette = m_dib.GetPalette();
	if (pPalette != NULL)
	{
		CClientDC appDC(m_pPreviewDialog->GetPreviewWnd());

		// bForceBackground flag is FALSE only if pFocusWnd != this (this dialog)
		CPalette* oldPalette=appDC.SelectPalette(pPalette, 
			pFocusWnd!=m_pPreviewDialog->GetPreviewWnd());
		if(oldPalette!=NULL)
		{
			nColorsChanged=appDC.RealizePalette();
			appDC.SelectPalette(oldPalette,TRUE);
		}
		else
		{
			TRACE(_T("SelectPalette failed in COXGraphicsPreviewDlg::OnDoRealizePalette\n"));
		}
	}

	if(nColorsChanged != 0)
	{
		m_pPreviewDialog->GetPreviewWnd()->Invalidate(FALSE);
	}
	return TRUE;
#else
	return FALSE;
#endif
}


void COXGraphicsFileViewer::OnChangeSize()
{
#ifdef OX_PREVIEW_ICON
	ASSERT(m_pPreviewDialog!=NULL);
	ASSERT(m_pPreviewDialog->GetPreviewWnd()!=NULL);

	if(!::IsWindow(m_imageListBox.GetSafeHwnd()))
	{
		return;
	}
	
	CRect rect;
	m_pPreviewDialog->GetPreviewWnd()->GetClientRect(rect);
	CSize szWindow(rect.Width(),rect.Height());

	m_imageListBox.MoveWindow(0,0,szWindow.cx,szWindow.cy);
#endif
}

void COXGraphicsFileViewer::OnPreview(BOOL bPreview)
{
#ifdef OX_PREVIEW_ICON
	if(CanPreview())
	{
		ASSERT(::IsWindow(m_imageListBox.GetSafeHwnd()));
		if(m_imageListBox.GetCount()>0)
		{
			m_imageListBox.ShowWindow((bPreview ? SW_SHOWNA : SW_HIDE));
		}
	}
#else
	UNREFERENCED_PARAMETER(bPreview);
#endif
}

void COXGraphicsFileViewer::OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd)
{
#ifdef OX_PREVIEW_ICON
	ASSERT(pPreviewWnd!=NULL);
	ASSERT(::IsWindow(pPreviewWnd->GetSafeHwnd()));
	CRect rect;
	pPreviewWnd->GetClientRect(rect);

	ASSERT(!::IsWindow(m_imageListBox.GetSafeHwnd()));
	VERIFY(m_imageListBox.CreateEx(0,_T("ListBox"),_T(""),
		WS_CHILD|WS_VSCROLL|WS_HSCROLL|LBS_NOTIFY|LBS_OWNERDRAWFIXED|
		LBS_NOINTEGRALHEIGHT|LBS_MULTICOLUMN,rect,pPreviewWnd,AFX_IDW_PANE_FIRST));
	m_imageListBox.SetHighlightColor(m_imageListBox.GetBkColor());
#else
	UNREFERENCED_PARAMETER(pPreviewWnd);
#endif
}

void COXGraphicsFileViewer::OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	UNREFERENCED_PARAMETER(pPreviewWnd);

#ifdef OX_PREVIEW_ICON
	ASSERT(::IsWindow(m_imageListBox.GetSafeHwnd()));
	m_imageListBox.DestroyWindow();

	m_bCanPreview=FALSE;
#endif
}
#endif	//	OX_PREVIEW_GRAPHICS

////////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_PLAINTEXT
#ifdef _UNICODE
BOOL COXTextFileViewer::OnNewFileSelect(const CString& /*sFilename*/, 
										BOOL /*bViewerFound*/)
{
	return FALSE;
#else
BOOL COXTextFileViewer::OnNewFileSelect(const CString& sFilename, 
										BOOL bViewerFound)
{
	BOOL bViewerCtrlCreated=TRUE;
	if(!::IsWindow(m_edit.GetSafeHwnd()))
	{
		bViewerCtrlCreated=FALSE;
	}
	
	COXFilePath pathPreview(sFilename);

	BOOL bSuccess=FALSE;
	if(bViewerCtrlCreated && !bViewerFound && !sFilename.IsEmpty() && 
		pathPreview.Exists())
	{
		CWaitCursor wait;

		CFile file;
		if(file.Open(sFilename,CFile::modeRead) && 
			file.m_hFile != CFile::hFileNull)
		{
			file.SeekToBegin();
			int nSize=(int)file.SeekToEnd();
			file.SeekToBegin();
			// max size of the displayed file is 64Kb
			if(nSize<0x0000ffff)
			{
				CString sText;
				VERIFY(file.Read((void*)sText.GetBuffer(nSize),nSize)==(UINT)nSize);
				m_edit.SetWindowText(sText);
				bSuccess=TRUE;
			}
		}
	}

	if(!bSuccess && bViewerCtrlCreated)
	{
		m_edit.ShowWindow(SW_HIDE);
	}
	else if(bSuccess && bViewerCtrlCreated)
	{
		m_edit.ShowWindow(SW_SHOWNA);
	}

	m_bCanPreview=bSuccess;

	return bSuccess;
#endif
}


BOOL COXTextFileViewer::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(paintRect);

	return (CanPreview());
}


BOOL COXTextFileViewer::OnDoRealizePalette(CWnd* pFocusWnd)
{
	UNREFERENCED_PARAMETER(pFocusWnd);

	return (CanPreview());
}


void COXTextFileViewer::OnChangeSize()
{
	ASSERT(m_pPreviewDialog!=NULL);
	ASSERT(m_pPreviewDialog->GetPreviewWnd()!=NULL);

	if(!::IsWindow(m_edit.GetSafeHwnd()))
	{
		return;
	}
	
	CRect rect;
	m_pPreviewDialog->GetPreviewWnd()->GetClientRect(rect);
	CSize szWindow(rect.Width(),rect.Height());

	m_edit.MoveWindow(0,0,szWindow.cx,szWindow.cy);
}

void COXTextFileViewer::OnPreview(BOOL bPreview)
{
	if(CanPreview())
	{
		ASSERT(::IsWindow(m_edit.GetSafeHwnd()));
		m_edit.ShowWindow((bPreview ? SW_SHOWNA : SW_HIDE));
	}
}

void COXTextFileViewer::OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	ASSERT(pPreviewWnd!=NULL);
	ASSERT(::IsWindow(pPreviewWnd->GetSafeHwnd()));
	CRect rect;
	pPreviewWnd->GetClientRect(rect);

	ASSERT(!::IsWindow(m_edit.GetSafeHwnd()));
	VERIFY(m_edit.CreateEx(0,_T("Edit"),_T(""),
		WS_CHILD|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,
		rect,pPreviewWnd,AFX_IDW_PANE_FIRST));
	// change the font to default GUI one
	CFont* pFont=CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_edit.SetFont(pFont);
}

void COXTextFileViewer::OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	UNREFERENCED_PARAMETER(pPreviewWnd);

	ASSERT(::IsWindow(m_edit.GetSafeHwnd()));
	m_edit.DestroyWindow();

	m_bCanPreview=FALSE;
}
#endif	//	OX_PREVIEW_PLAINTEXT

////////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_RTF
#ifdef _UNICODE
BOOL COXRTFFileViewer::OnNewFileSelect(const CString& /*sFilename*/, 
										BOOL /*bViewerFound*/)
{
	return FALSE;
#else
BOOL COXRTFFileViewer::OnNewFileSelect(const CString& sFilename, 
										BOOL bViewerFound)
{
	BOOL bViewerCtrlCreated=TRUE;
	if(!::IsWindow(m_richEdit.GetSafeHwnd()))
	{
		bViewerCtrlCreated=FALSE;
	}
	
	COXFilePath pathPreview(sFilename);

	BOOL bSuccess=FALSE;
	if(bViewerCtrlCreated && !bViewerFound && !sFilename.IsEmpty() && 
		pathPreview.Exists())
	{
		CWaitCursor wait;

		bSuccess=FALSE;
		if(m_file.m_hFile != CFile::hFileNull)
		{
			m_file.Close();
		}
		m_file.Open(sFilename,CFile::modeRead);
		if(m_file.m_hFile != CFile::hFileNull)
		{
			if(IsRTF(&m_file))
			{
				LoadRTF();
				bSuccess=TRUE;
			}
		}
	}

	if(!bSuccess && bViewerCtrlCreated)
	{
		m_richEdit.ShowWindow(SW_HIDE);
		if(m_file.m_hFile != CFile::hFileNull)
		{
			m_file.Close();
		}
	}
	else if(bSuccess && bViewerCtrlCreated)
	{
		m_richEdit.ShowWindow(SW_SHOWNA);
	}

	m_bCanPreview=bSuccess;

	return bSuccess;
#endif
}


BOOL COXRTFFileViewer::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(paintRect);

	return (CanPreview());
}


BOOL COXRTFFileViewer::OnDoRealizePalette(CWnd* pFocusWnd)
{
	UNREFERENCED_PARAMETER(pFocusWnd);

	return (CanPreview());
}


void COXRTFFileViewer::OnChangeSize()
{
	ASSERT(m_pPreviewDialog!=NULL);
	ASSERT(m_pPreviewDialog->GetPreviewWnd()!=NULL);

	if(!::IsWindow(m_richEdit.GetSafeHwnd()))
	{
		return;
	}
	
	CRect rect;
	m_pPreviewDialog->GetPreviewWnd()->GetClientRect(rect);
	CSize szWindow(rect.Width(),rect.Height());

	m_richEdit.MoveWindow(0,0,szWindow.cx,szWindow.cy);
}

void COXRTFFileViewer::OnPreview(BOOL bPreview)
{
	if(CanPreview())
	{
		ASSERT(::IsWindow(m_richEdit.GetSafeHwnd()));
		m_richEdit.ShowWindow((bPreview ? SW_SHOWNA : SW_HIDE));
	}
}

void COXRTFFileViewer::OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	ASSERT(pPreviewWnd!=NULL);
	ASSERT(::IsWindow(pPreviewWnd->GetSafeHwnd()));
	CRect rect;
	pPreviewWnd->GetClientRect(rect);

	ASSERT(!::IsWindow(m_richEdit.GetSafeHwnd()));

#if _MFC_VER >= 0x0700
	VERIFY(m_richEdit.Create(WS_CHILD|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,
		rect,pPreviewWnd,AFX_IDW_PANE_FIRST));
#else
	VERIFY(m_richEdit.CreateEx(0,_T("RICHEDIT"),_T(""),
		WS_CHILD|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,
		rect,pPreviewWnd,AFX_IDW_PANE_FIRST));
#endif
}

void COXRTFFileViewer::OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	UNREFERENCED_PARAMETER(pPreviewWnd);

	ASSERT(::IsWindow(m_richEdit.GetSafeHwnd()));
	m_richEdit.DestroyWindow();

	m_bCanPreview=FALSE;
}


void COXRTFFileViewer::LoadRTF()
{
	ASSERT(::IsWindow(m_richEdit.GetSafeHwnd()));
	ASSERT(m_file.m_hFile != CFile::hFileNull);
	ASSERT(IsRTF(&m_file));

	m_file.SeekToBegin();

	EDITSTREAM es;
	es.dwCookie=(DWORD_PTR)this;
    es.dwError=0; 
    es.pfnCallback=(EDITSTREAMCALLBACK)RichEditStreamInCallback;
	m_richEdit.StreamIn(SF_RTF,es);
}

BOOL COXRTFFileViewer::IsRTF(CFile* pFile)
{
	ASSERT(pFile!=NULL);
	ASSERT(pFile->m_hFile != CFile::hFileNull);

	int nPos=(int)pFile->Seek(0,CFile::current);

	// check if specified file is of RTF format
	pFile->SeekToBegin();
	int nSignatureLength=5;
	char* pBuffer=new char[nSignatureLength+1];
	BOOL bRTFFile=
		(pFile->Read((void*)pBuffer,nSignatureLength)==(UINT)nSignatureLength);
	pBuffer[nSignatureLength]='\0';
	if(bRTFFile)
	{
		CString sSignature;
#ifdef _UNICODE
		_mbstowcsz((LPTSTR)sSignature.GetBuffer(nSignatureLength),
			(LPSTR)pBuffer,nSignatureLength);
		sSignature.ReleaseBuffer();
#else
		sSignature=pBuffer;
#endif
		bRTFFile=(sSignature.CompareNoCase(_T("{\\rtf"))==0);
	}
	delete[] pBuffer;

	pFile->Seek(nPos,CFile::begin);

	return bRTFFile;
}


DWORD CALLBACK COXRTFFileViewer::RichEditStreamInCallback(DWORD_PTR dwCookie, 
														   LPBYTE pbBuff, 
														   LONG cb, 
														   LONG FAR *pcb)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	COXRTFFileViewer* pViewer=(COXRTFFileViewer*)dwCookie;
	ASSERT(pViewer!=NULL);

	int nPos=(int)pViewer->m_file.Seek(0,CFile::current);
	int nSize=(int)pViewer->m_file.SeekToEnd();
	pViewer->m_file.Seek(nPos,CFile::begin);
	if(nPos>=nSize)
	{
		*pcb=0;
		return (DWORD)E_FAIL;
	}

	if(nSize>cb)
	{
		nSize=cb-cb%sizeof(TCHAR);
	}
	TCHAR* pBuffer=new TCHAR[nSize/sizeof(TCHAR)];
	VERIFY(pViewer->m_file.Read((void*)pBuffer,nSize)==(UINT)nSize);

#ifdef _UNICODE
	_wcstombsz((LPSTR)pbBuff,(LPCTSTR)pBuffer,nSize);
#else
	::memcpy((void*)pbBuff,(const void*)pBuffer,nSize);
#endif

	delete[] pBuffer;

	*pcb=nSize;

	return NOERROR;
}
#endif	//	OX_PREVIEW_RTF

//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_HTML
BOOL COXHTMLFileViewer::OnNewFileSelect(const CString& sFilename, 
										BOOL bViewerFound/*=FALSE*/)
{
	BOOL bViewerCtrlCreated=TRUE;
	if(!::IsWindow(m_HTMLBrowserWnd.GetSafeHwnd()))
	{
		bViewerCtrlCreated=FALSE;
	}
	
	COXFilePath pathPreview(sFilename);

	BOOL bSuccess=FALSE;
	if(bViewerCtrlCreated && !bViewerFound && !sFilename.IsEmpty() && 
		pathPreview.Exists())
	{
		CWaitCursor wait;

		CString sExtension=pathPreview.GetExtender();
		// search registry for information on the content type
		if(sExtension.IsEmpty())
		{
			sExtension=_T("*");
		}
		else
		{
			sExtension=_T(".")+sExtension;
		}
		HKEY hExtensionContentTypeKey=NULL;
		if(::RegOpenKeyEx(HKEY_CLASSES_ROOT,sExtension,0,
			KEY_QUERY_VALUE,&hExtensionContentTypeKey)==ERROR_SUCCESS)
		{
			ASSERT(hExtensionContentTypeKey!=NULL);

			DWORD dwType=REG_SZ;
			CString sContentType;
			DWORD dwContentTypeStringLength=MAX_PATH;
			if(::RegQueryValueEx(hExtensionContentTypeKey,_T("Content Type"),NULL,
				&dwType,(LPBYTE)sContentType.GetBuffer(dwContentTypeStringLength),
				&dwContentTypeStringLength)==ERROR_SUCCESS)
			{
				HKEY hContentTypeKey=NULL;
				CString sSubkey;
				sSubkey.Format(_T("MIME\\Database\\Content Type\\%s"),sContentType);
				if(::RegOpenKeyEx(HKEY_CLASSES_ROOT,sSubkey,0,
					KEY_QUERY_VALUE,&hContentTypeKey)==ERROR_SUCCESS)
				{
					ASSERT(hContentTypeKey!=NULL);
		
					CString sCLSID;
					dwContentTypeStringLength=MAX_PATH;
					if(sExtension.CompareNoCase(_T(".doc"))==0 || 
						sExtension.CompareNoCase(_T(".rtf"))==0 || 
						sExtension.CompareNoCase(_T(".dot"))==0 || 
						sExtension.CompareNoCase(_T(".xls"))==0 || 
						::RegQueryValueEx(hContentTypeKey,_T("CLSID"),NULL,
						&dwType,(LPBYTE)sCLSID.GetBuffer(dwContentTypeStringLength),
						&dwContentTypeStringLength)==ERROR_SUCCESS)
					{
						ASSERT(m_pIWebBrowser!=NULL);

						COleVariant vURL(sFilename,VT_BSTR);
						COleVariant vFlags((long)(navNoHistory), VT_I4);
						COleVariant vHeaders((LPCTSTR)NULL,VT_BSTR);
						COleVariant vTargetFrameName((LPCTSTR)NULL,VT_BSTR);
						COleSafeArray vPostData;
						HRESULT hResult=m_pIWebBrowser->
							Navigate2(vURL,vFlags,vTargetFrameName,vPostData,vHeaders);

						bSuccess=(hResult==S_OK);

						::RegCloseKey(hContentTypeKey);
					}
				}
			}

			::RegCloseKey(hExtensionContentTypeKey);
		}
	}

	m_bCanPreview=bSuccess;

	if(!bSuccess && bViewerCtrlCreated)
	{
		m_HTMLBrowserWnd.MoveWindow(0,0,0,0);
	}
	else if(bSuccess && bViewerCtrlCreated)
	{
		OnChangeSize();
	}

	return bSuccess;
}


BOOL COXHTMLFileViewer::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(paintRect);

	return (CanPreview());
}


BOOL COXHTMLFileViewer::OnDoRealizePalette(CWnd* pFocusWnd)
{
	UNREFERENCED_PARAMETER(pFocusWnd);

	return (CanPreview());
}


void COXHTMLFileViewer::OnChangeSize()
{
	ASSERT(m_pPreviewDialog!=NULL);
	ASSERT(m_pPreviewDialog->GetPreviewWnd()!=NULL);

	if(!::IsWindow(m_HTMLBrowserWnd.GetSafeHwnd()) || !CanPreview())
	{
		return;
	}
	
	CRect rect;
	m_pPreviewDialog->GetPreviewWnd()->GetClientRect(rect);
	CSize szWindow(rect.Width(),rect.Height());

	m_HTMLBrowserWnd.MoveWindow(0,0,szWindow.cx,szWindow.cy);
}

void COXHTMLFileViewer::OnPreview(BOOL bPreview)
{
	if(CanPreview())
	{
		ASSERT(::IsWindow(m_HTMLBrowserWnd.GetSafeHwnd()));
		if(bPreview)
		{
			OnChangeSize();
		}
		else
		{
			m_HTMLBrowserWnd.MoveWindow(0,0,0,0);
		}
	}
}

void COXHTMLFileViewer::OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	ASSERT(pPreviewWnd!=NULL);
	ASSERT(::IsWindow(pPreviewWnd->GetSafeHwnd()));

	ASSERT(!::IsWindow(m_HTMLBrowserWnd.GetSafeHwnd()) && m_pIWebBrowser==NULL);
	if(m_HTMLBrowserWnd.CreateControl(CLSID_WebBrowser,_T(""),
		WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),pPreviewWnd,AFX_IDW_PANE_FIRST))
	{
		LPUNKNOWN lpIUnknown=m_HTMLBrowserWnd.GetControlUnknown();
		ASSERT(lpIUnknown!=NULL);
		HRESULT hResult=
			lpIUnknown->QueryInterface(IID_IWebBrowser2,(void**)&m_pIWebBrowser);
		if(!SUCCEEDED(hResult))
		{
			m_pIWebBrowser=NULL;
			m_HTMLBrowserWnd.DestroyWindow();
		}
	}
}

void COXHTMLFileViewer::OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	UNREFERENCED_PARAMETER(pPreviewWnd);

	if(m_pIWebBrowser!=NULL)
	{
		m_pIWebBrowser->Release();
		m_pIWebBrowser=NULL;
	}
	if(::IsWindow(m_HTMLBrowserWnd.GetSafeHwnd()))
	{
		m_HTMLBrowserWnd.DestroyWindow();
	}

	m_bCanPreview=FALSE;
}


#endif	//	OX_PREVIEW_HTML

//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_SOUND

BEGIN_MESSAGE_MAP(COXSoundFileViewer::COXSoundFileViewerHelper, CWnd)
	//{{AFX_MSG_MAP(COXSoundFileViewer::COXSoundFileViewerHelper)
	ON_MESSAGE(WM_PAINT, 
		COXSoundFileViewer::COXSoundFileViewerHelper::OnPaint)
	ON_BN_CLICKED(IDC_OXSOUNDPREVIEW_BUTTON_START_PLAYING, 
		COXSoundFileViewer::COXSoundFileViewerHelper::OnStartPlaying)
	ON_BN_CLICKED(IDC_OXSOUNDPREVIEW_BUTTON_STOP_PLAYING, 
		COXSoundFileViewer::COXSoundFileViewerHelper::OnStopPlaying)
	ON_MESSAGE(WM_OX_SOUNDPLAYBACKCOMPLETE, 
		COXSoundFileViewer::COXSoundFileViewerHelper::OnPlayComplete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LRESULT COXSoundFileViewer::COXSoundFileViewerHelper::OnPaint(WPARAM wParam, 
															  LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	CPaintDC dc(this);

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect,::GetSysColor(COLOR_BTNFACE));

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	
	static CFont fontText;
	if((HFONT)fontText==NULL)
	{
		VERIFY(fontText.CreatePointFont(80,_T("MS Sans Serif")));
	}
	dc.SelectObject(&fontText);
	dc.DrawText(m_sSoundInfo,m_rectSoundInfo,DT_LEFT|DT_WORDBREAK);

	return 0;
}


void COXSoundFileViewer::COXSoundFileViewerHelper::OnStartPlaying()
{
	m_sound.Play(FALSE,TRUE);

	m_btnStartPlaying.EnableWindow(FALSE);
	m_btnStopPlaying.EnableWindow(TRUE);
}


void COXSoundFileViewer::COXSoundFileViewerHelper::OnStopPlaying()
{
	if(m_sound.IsWaveLoaded())
	{
		WAVEFORMATEX wfx;

		if(m_sound.GetWaveFormat(&wfx))
		{
			CString sStereo, sMono;
			VERIFY(sMono.LoadString(IDS_OX_SOUNDFILEVIEWERMONO));//"Mono"
			VERIFY(sStereo.LoadString(IDS_OX_SOUNDFILEVIEWERSTEREO));//"Stereo"
			m_sSoundInfo.Format(IDS_OX_SOUNDFILEVIEWERRATE,  //"%s\nSample rate: %d kHz"
				(wfx.nChannels==1 ? sMono :	sStereo), 
				wfx.nSamplesPerSec/1000);
			RedrawWindow(m_rectSoundInfo);
		}
	}

	m_sound.Stop();

	m_btnStartPlaying.EnableWindow(TRUE);
	m_btnStopPlaying.EnableWindow(FALSE);
}


LRESULT COXSoundFileViewer::COXSoundFileViewerHelper::OnPlayComplete(WPARAM wParam, 
																	 LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	OnStopPlaying();
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////


BOOL COXSoundFileViewer::OnNewFileSelect(const CString& sFilename, 
										 BOOL bViewerFound/*=FALSE*/)
{
	COXFilePath pathPreview(sFilename);

	m_wndHelper.m_sound.Stop();

	BOOL bSuccess=FALSE;
	if(!bViewerFound && !sFilename.IsEmpty() && pathPreview.Exists())
	{
		if(m_wndHelper.m_sound.CanPlay())
		{
			bSuccess=m_wndHelper.m_sound.Open(sFilename);
		}
	}

	if(::IsWindow(m_wndHelper.GetSafeHwnd()))
	{
		m_wndHelper.m_btnStartPlaying.EnableWindow(TRUE);
		m_wndHelper.m_btnStopPlaying.EnableWindow(FALSE);

		m_wndHelper.ShowWindow((bSuccess ? SW_SHOWNA : SW_HIDE));
	}

	if(!bSuccess)
	{
		m_wndHelper.m_sSoundInfo.Empty();
	}

	m_bCanPreview=bSuccess;

	return bSuccess;
}


void COXSoundFileViewer::OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	ASSERT(pPreviewWnd!=NULL);
	ASSERT(::IsWindow(pPreviewWnd->GetSafeHwnd()));

	CRect rect;
	pPreviewWnd->GetClientRect(rect);

	// create helper window
	ASSERT(!::IsWindow(m_wndHelper.GetSafeHwnd()));
	VERIFY(m_wndHelper.Create(AfxRegisterWndClass(CS_DBLCLKS),_T(""),
		WS_CHILD|WS_CLIPCHILDREN,rect,pPreviewWnd,AFX_IDW_PANE_FIRST));
	m_wndHelper.m_sound.SetCallbackWnd(&m_wndHelper);

	rect.DeflateRect(OXSOUNDPREVIEW_BORDER_OFFSET,OXSOUNDPREVIEW_BORDER_OFFSET);
	// create buttons for playing the sound
	CRect rectButtonStart=rect;
	rectButtonStart.bottom=rectButtonStart.top+OXSOUNDPREVIEW_BUTTON_HEIGHT;
	rectButtonStart.right=rectButtonStart.left+OXSOUNDPREVIEW_BUTTON_WIDTH;
	VERIFY(m_wndHelper.m_btnStartPlaying.Create(
		_T("4"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,rectButtonStart,
		&m_wndHelper,IDC_OXSOUNDPREVIEW_BUTTON_START_PLAYING));

	CRect rectButtonStop=rectButtonStart;
	rectButtonStop.left=rectButtonStart.right+OXSOUNDPREVIEW_BORDER_OFFSET;
	rectButtonStop.right=rectButtonStop.left+OXSOUNDPREVIEW_BUTTON_WIDTH;
	VERIFY(m_wndHelper.m_btnStopPlaying.Create(
		_T("<"),WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,rectButtonStop,
		&m_wndHelper,IDC_OXSOUNDPREVIEW_BUTTON_START_PLAYING));

	static CFont fontButton;
	if((HFONT)fontButton==NULL)
	{
		VERIFY(fontButton.CreatePointFont(100,_T("Marlett")));
	}
	m_wndHelper.m_btnStartPlaying.SetFont(&fontButton);
	m_wndHelper.m_btnStopPlaying.SetFont(&fontButton);

	OnChangeSize();
}


void COXSoundFileViewer::OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd)
{
	UNREFERENCED_PARAMETER(pPreviewWnd);

	ASSERT(::IsWindow(m_wndHelper.GetSafeHwnd()));
	m_wndHelper.DestroyWindow();

	m_wndHelper.m_rectSoundInfo.SetRectEmpty();
	m_wndHelper.m_sSoundInfo.Empty();
	m_wndHelper.m_sound.Stop();

	m_bCanPreview=FALSE;
}


void COXSoundFileViewer::OnPreview(BOOL bPreview)
{
	if(CanPreview())
	{
		ASSERT(::IsWindow(m_wndHelper.GetSafeHwnd()));
		m_wndHelper.ShowWindow((bPreview ? SW_SHOWNA : SW_HIDE));
	}
}


void COXSoundFileViewer::OnChangeSize()
{
	ASSERT(m_pPreviewDialog!=NULL);
	ASSERT(m_pPreviewDialog->GetPreviewWnd()!=NULL);

	CRect rect;
	m_pPreviewDialog->GetPreviewWnd()->GetClientRect(rect);

	if(::IsWindow(m_wndHelper.GetSafeHwnd()))
	{
		CSize szWindow(rect.Width(),rect.Height());
		m_wndHelper.MoveWindow(0,0,szWindow.cx,szWindow.cy);

		rect.DeflateRect(OXSOUNDPREVIEW_BORDER_OFFSET,OXSOUNDPREVIEW_BORDER_OFFSET);
		// calculate the rectangle for the area that can be taken
		// by sound info text
		m_wndHelper.m_rectSoundInfo=rect;

		CRect rectButton;
		m_wndHelper.m_btnStartPlaying.GetWindowRect(rectButton);
		m_wndHelper.m_rectSoundInfo.top+=rectButton.Height()+
			OXSOUNDPREVIEW_BORDER_OFFSET;
	}
}


BOOL COXSoundFileViewer::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(paintRect);

	return (CanPreview());
}


BOOL COXSoundFileViewer::OnDoRealizePalette(CWnd* pFocusWnd)
{
	UNREFERENCED_PARAMETER(pFocusWnd);

	return (CanPreview());
}

#endif	//	OX_PREVIEW_SOUND

//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// global array of all preview dialog objects
CMap<HWND,HWND,COXPreviewDialog*,COXPreviewDialog*> 
COXPreviewDialog::m_allPreviewDialogs;

IMPLEMENT_DYNAMIC(COXPreviewDialog, CFileDialog)

COXPreviewDialog::COXPreviewDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, 
								   LPCTSTR lpszFileName, DWORD dwFlags, 
								   LPCTSTR lpszFilter, CWnd* pParentWnd) : 
	CFileDialog(bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags,
				lpszFilter, pParentWnd),
	m_bFlatToolbar(TRUE),
	m_rectPreviewArea(0,0,0,0),
	m_rectSplitterArea(0,0,0,0),
	m_rectResizableArea(0,0,0,0),
	m_bIsSplitterPressed(FALSE),
	m_ptOldSplitterCapture(0,0),
	m_nOldListCtrlWidth(-1),
	m_nOldListCtrlHeight(-1),
	m_pfnSuper(NULL),
	m_nPreviewWndOffset(0),
	m_bPreview(TRUE),
	m_bPrevPreview(TRUE),
	m_sSelectedFile(_T("")),
	m_sPrevSelectedFile(_T("")),
	m_bInitialized(FALSE)
{
	if(IsOldStyle())
	{
		TRACE(_T("COXPreviewDialog: failed to instantiate the object. OFN_EXPLORER style must be specified"));
		AfxThrowNotSupportedException();
		return;
	}

	m_preview.m_pPreviewDlg=this;

#ifdef OX_PREVIEW_RTF
	VERIFY(AddFileViewer(&m_rtfFileViewer)!=-1);
#endif
#ifdef OX_PREVIEW_GRAPHICS
	VERIFY(AddFileViewer(&m_graphicsFileViewer)!=-1);
#endif
#ifdef OX_PREVIEW_SOUND
	VERIFY(AddFileViewer(&m_soundFileViewer)!=-1);
#endif
#ifdef OX_PREVIEW_HTML
	VERIFY(AddFileViewer(&m_htmlFileViewer)!=-1);
#endif
#ifdef OX_PREVIEW_PLAINTEXT
	VERIFY(AddFileViewer(&m_textFileViewer)!=-1);
#endif
}


COXPreviewDialog::~COXPreviewDialog()
{
}


BEGIN_MESSAGE_MAP(COXPreviewDialog, CFileDialog)
	ON_WM_PALETTECHANGED()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


LRESULT CALLBACK COXPreviewDialog::GlobalPreviewDialogProc(HWND hWnd, 
														   UINT uMsg, 
														   WPARAM wParam, 
														   LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	COXPreviewDialog* pPreviewDlg=NULL;
	if(m_allPreviewDialogs.Lookup(hWnd,pPreviewDlg))
	{
		ASSERT_VALID(pPreviewDlg);
		return pPreviewDlg->PreviewDialogProc(hWnd,uMsg,wParam,lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
}

LRESULT COXPreviewDialog::PreviewDialogProc(HWND hWnd, UINT uMsg, 
											WPARAM wParam, LPARAM lParam)
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	ASSERT(!IsOldStyle());

	LRESULT result = CallWindowProc(m_pfnSuper, hWnd, uMsg, wParam, lParam);

	// initialize internal variables
	if(!m_bInitialized)
	{
		InitializePreviewDialog();
	}
	
	switch(uMsg)
	{
    case WM_OX_LOAD_PREVIEWWND_OFFSET:
        {
            LoadPreviewWndOffsetState();
			//changed 11/12/1999
            //GetParent()->UnlockWindowUpdate();
            break;
        }
	case WM_SIZE:
		{
			AdjustControls();

			// notify all file viewers
			for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
			{
				COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
				ASSERT(pFileViewer!=NULL);
				pFileViewer->OnChangeSize();
			}
			break;
		}

	case WM_SETCURSOR:
		{
			if(IsInPreviewMode())
			{
				ASSERT(::IsWindow(m_preview.GetSafeHwnd()));

				CPoint point;
				::GetCursorPos(&point);
				GetParent()->ScreenToClient(&point);

				if(m_rectSplitterArea.PtInRect(point))
				{
					HCURSOR hCursor=AfxGetApp()->
						LoadCursor(MAKEINTRESOURCE(AFX_IDC_HSPLITBAR));
					if(hCursor==NULL)
						hCursor=::LoadCursor(NULL,IDC_SIZEWE);
					SetCursor(hCursor);
					return TRUE;
				}
			}
			break;
		}

	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpmmi=(LPMINMAXINFO)lParam;
			CRect rect;
			GetParent()->GetWindowRect(rect);
			// list control
			CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
			ASSERT(pListCtrl!=NULL);
			CRect rectListCtrl;
			pListCtrl->GetWindowRect(rectListCtrl);
			int nNewMinWidth=rect.Width()-rectListCtrl.Width()+
				OX_MIN_SHELLITEMSLISTCTRL_WIDTH;
			if(nNewMinWidth>lpmmi->ptMinTrackSize.x)
			{
				lpmmi->ptMinTrackSize.x=nNewMinWidth;
			}
			break;
		}

	case WM_LBUTTONDOWN:
		{
			if(IsInPreviewMode())
			{
				ASSERT(::IsWindow(m_preview.GetSafeHwnd()));

				CPoint point(LOWORD(lParam),HIWORD(lParam));
				if(m_rectSplitterArea.PtInRect(point))
				{
					GetParent()->SetCapture();
					m_ptOldSplitterCapture=point;
					m_bIsSplitterPressed=TRUE;
					return 0;
				}
			}
			break;
		}

	case WM_LBUTTONUP:
		{
			if(m_bIsSplitterPressed)
			{
				if(::GetCapture()==GetParent()->GetSafeHwnd())
				{
					::ReleaseCapture();
				}
				m_bIsSplitterPressed=FALSE;
				return 0;
			}
			break;
		}

	case WM_CANCELMODE:
		{
			if(m_bIsSplitterPressed)
			{
				if(::GetCapture()==GetParent()->GetSafeHwnd())
				{
					::ReleaseCapture();
				}
				m_bIsSplitterPressed=FALSE;
				return 0;
			}
			break;
		}

	case WM_MOUSEMOVE:
		{
			if(m_bIsSplitterPressed)
			{
				CPoint point;
				::GetCursorPos(&point);
				GetParent()->ScreenToClient(&point);
				if(!m_rectResizableArea.PtInRect(point))
				{
					if(point.x<m_rectResizableArea.left)
					{
						point.x=m_rectResizableArea.left;
					}
					else
					{
						point.x=m_rectResizableArea.right;
					}
				}
				if(m_ptOldSplitterCapture.x!=point.x)
				{
					SetPreviewWndOffset(GetPreviewWndOffset()+
						m_ptOldSplitterCapture.x-point.x);
				}
				m_ptOldSplitterCapture=point;
				return 0;
			}
			break;
		}
	}

	if(!::IsWindow(GetSafeHwnd()))
	{
		// remove dialog from the global list of active dialogs
		HWND hWnd=NULL;
		COXPreviewDialog* pDialog=NULL;
		POSITION pos=m_allPreviewDialogs.GetStartPosition();
		while(pos!=NULL)
		{
			m_allPreviewDialogs.GetNextAssoc(pos,hWnd,pDialog);
			if(pDialog==this)
			{
				m_allPreviewDialogs.RemoveKey(hWnd);
				m_pfnSuper=NULL;
			}
		}
		ASSERT(m_pfnSuper==NULL);
		m_bInitialized=FALSE;
	}

	return result;
}


CRect COXPreviewDialog::AdjustRectToFit(CRect rect, CRect rectBound)
{
	// adjust the rect to fit the size of preview area
	int nWidth=rect.Width();
	int nHeight=rect.Height();
	BOOL bAlignHorz=((rect.Width()*rectBound.Height()) > 
		(rect.Height()*rectBound.Width()));
	if(bAlignHorz)
	{
		if(rectBound.Width()<rect.Width())
		{
			nHeight=(rect.Height()*rectBound.Width())/rect.Width();
			nWidth=rectBound.Width();
		}
	}
	else
	{
		if(rectBound.Height()<rect.Height())
		{
			nWidth=(rect.Width()*rectBound.Height())/rect.Height();
			nHeight=rectBound.Height();
		}
	}

	ASSERT(rectBound.Height()>=nHeight);
	ASSERT(rectBound.Width()>=nWidth);
	
	rect.top+=(rectBound.Height()-nHeight)/2;
	rect.bottom=rect.top+nHeight;
	rect.left+=(rectBound.Width()-nWidth)/2;
	rect.right=rect.left+nWidth;

	return rect;
}


void COXPreviewDialog::InitializePreviewDialog()
{
	ASSERT(!m_bInitialized);

	CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
	if(pListCtrl!=NULL)
	{
		m_bInitialized=TRUE;

		CRect rectListCtrl;
		pListCtrl->GetWindowRect(rectListCtrl);
		m_nOldListCtrlWidth=rectListCtrl.Width();
		m_nOldListCtrlHeight=rectListCtrl.Height();

        LoadPreviewState();

        GetParent()->PostMessage(WM_OX_LOAD_PREVIEWWND_OFFSET);
    }
}


void COXPreviewDialog::OnNewFileSelect(const CString& sFilename)
{
	BOOL bViewerFound=FALSE;
	for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
	{
		COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
		ASSERT(pFileViewer!=NULL);
		if(pFileViewer->OnNewFileSelect(sFilename,bViewerFound))
		{
			bViewerFound=TRUE;
		}
	}
}

void COXPreviewDialog::OnPaintPreview(CDC* pDC, const CRect& paintRect)
{
	for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
	{
		COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
		ASSERT(pFileViewer!=NULL);
		if(pFileViewer->OnPaintPreview(pDC,paintRect))
		{
			return;
		}
	}


	DrawPreviewNotAvailable(pDC,paintRect);
}


void COXPreviewDialog::OnDoRealizePalette(CWnd* pFocusWnd)
{
	for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
	{
		COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
		ASSERT(pFileViewer!=NULL);
		if(pFileViewer->OnDoRealizePalette(pFocusWnd))
		{
			return;
		}
	}
}


void COXPreviewDialog::DrawPreviewNotAvailable(CDC* pDC, const CRect& paintRect)
{
	ASSERT(pDC!=NULL);

	// Paint space of preview 
	pDC->FillSolidRect(paintRect,::GetSysColor(COLOR_BTNFACE));

	CFont* pFont=CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont* pOldFont=pDC->SelectObject(pFont);
	CRect rect=paintRect;
	pDC->DrawText(_T("Preview not available."),rect,
		DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
}


void COXPreviewDialog::CheckFileSelection()
{
	CString sFile;
	
	if((m_sPrevSelectedFile.CompareNoCase(m_sSelectedFile)!=0) ||
		(m_bPrevPreview!=m_bPreview))
	{
		if(m_bPreview)
		{
			sFile=m_sSelectedFile;
		}

		OnNewFileSelect(sFile);

		if(m_bPreview)
		{
			ASSERT(::IsWindow(m_preview.GetSafeHwnd()));
			m_preview.Invalidate(FALSE);
		}
		
		m_sPrevSelectedFile=m_sSelectedFile;
		m_bPrevPreview=m_bPreview;
	}
}	


BOOL COXPreviewDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	//changed 11/12/1999
	//GetParent()->LockWindowUpdate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void COXPreviewDialog::OnPaletteChanged(CWnd* pFocusWnd)
{
	CFileDialog::OnPaletteChanged(pFocusWnd);
	OnDoRealizePalette(pFocusWnd);
}

void COXPreviewDialog::OnActivate(UINT nState, CWnd* pWndOther, 
								  BOOL bMinimized)
{
	CFileDialog::OnActivate(nState,pWndOther,bMinimized);

	if(nState==WA_CLICKACTIVE || nState==WA_ACTIVE)
	{
		OnDoRealizePalette(this);
	}
}


void COXPreviewDialog::SetPreview(BOOL bPreview)
{
	BOOL bOldPreview=IsInPreviewMode();

	if(!::IsWindow(GetSafeHwnd()))
	{
		m_bPreview=bPreview;
	}
	else
	{
		if(bPreview)
		{
    		if(!::IsWindow(m_preview.GetSafeHwnd()))
            {
				// resize list control
				CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
				ASSERT(pListCtrl!=NULL);
				CRect rectListCtrl;
				pListCtrl->GetWindowRect(rectListCtrl);
				m_rectPreviewArea=rectListCtrl;
				// align right side of the list control with right side of 
				// the extended combo box
				CWnd* pExtComboBox=GetParent()->GetDlgItem(cmb2);
				ASSERT(pExtComboBox!=NULL);
				CRect rectExtCombo;
				pExtComboBox->GetWindowRect(rectExtCombo);
				rectListCtrl.right=rectExtCombo.right-GetPreviewWndOffset();
				GetParent()->ScreenToClient(rectListCtrl);
				pListCtrl->MoveWindow(rectListCtrl);
				m_nOldListCtrlWidth=rectListCtrl.Width();
				m_nOldListCtrlHeight=rectListCtrl.Height();

				// set preview flag to TRUE
				m_bPreview=TRUE;
				// calculate preview rectangle. Its width will be the same as 
				// the width of the toolbar and height as list control's height
				CWnd* pToolBar=CWnd::FromHandle(FindToolBar());
				ASSERT(pToolBar!=NULL);
				CRect rectToolBar;
				pToolBar->GetWindowRect(rectToolBar);
				m_rectPreviewArea.left=rectToolBar.left-GetPreviewWndOffset();
				GetParent()->ScreenToClient(m_rectPreviewArea);

				// check/uncheck "Preview" button
                TBBUTTON button;
				pToolBar->SendMessage(TB_GETBUTTON,
                    pToolBar->SendMessage(TB_BUTTONCOUNT)-1,(LPARAM)&button);
                if((button.fsState&TBSTATE_CHECKED)!=(bPreview ? TBSTATE_CHECKED : 0))
                {
				    pToolBar->SendMessage(TB_CHECKBUTTON,OX_IDC_SET_PREVIEW,
					    MAKELPARAM(bPreview,0));
                }

				// create preview window
				VERIFY(m_preview.
					Create(GetParent(),m_rectPreviewArea,AFX_IDW_PANE_FIRST));

				// save "splitter" area coordinates
				m_rectSplitterArea=m_rectPreviewArea;
				int nOldLeftCoord=m_rectSplitterArea.left;
				m_rectSplitterArea.left=rectListCtrl.right;
				m_rectSplitterArea.right=nOldLeftCoord;

				// save "resizable" area coordinates
				m_rectResizableArea=rectListCtrl;
				m_rectResizableArea.left+=OX_MIN_SHELLITEMSLISTCTRL_WIDTH;
				m_rectResizableArea.right=m_rectPreviewArea.right-
					OX_MIN_PREVIEWWND_WIDTH;

				// notify all file viewers
				for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
				{
					COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
					ASSERT(pFileViewer!=NULL);
					pFileViewer->OnCreatePreviewWnd(&m_preview);
				}
            }
		}
		else
		{
			if(::IsWindow(m_preview.GetSafeHwnd()))
			{
				// notify all file viewers
				for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
				{
					COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
					ASSERT(pFileViewer!=NULL);
					pFileViewer->OnDestroyPreviewWnd(&m_preview);
				}

				// destroy preview window
				VERIFY(m_preview.DestroyWindow());

				// resize list control
				CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
				ASSERT(pListCtrl!=NULL);
				CRect rectListCtrl;
				pListCtrl->GetWindowRect(rectListCtrl);
				GetParent()->ClientToScreen(m_rectPreviewArea);
				rectListCtrl.right=m_rectPreviewArea.right;
				GetParent()->ScreenToClient(rectListCtrl);
				pListCtrl->MoveWindow(rectListCtrl);
				m_nOldListCtrlWidth=rectListCtrl.Width();
				m_nOldListCtrlHeight=rectListCtrl.Height();
			}

			// reset preview area size
			m_rectPreviewArea.SetRectEmpty();
		}

		// set preview flag
		m_bPreview=bPreview;

		CheckFileSelection();
	}

	if(bOldPreview!=IsInPreviewMode())
	{
		CheckFileSelection();
		// notify all file viewers
		for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
		{
			COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
			ASSERT(pFileViewer!=NULL);
			pFileViewer->OnPreview(IsInPreviewMode());
		}
	}
}


void COXPreviewDialog::OnInitDone() 
{
	CFileDialog::OnInitDone();
	
	UpdateToolBar();
	SetFlatToolBar(IsFlatToolBar());
	// subclass parent window
	CWnd* pParentWnd=GetParent();
	ASSERT(pParentWnd!=NULL);
	m_pfnSuper=(WNDPROC)
		(LONG_PTR)
		::SetWindowLongPtr(
		pParentWnd->GetSafeHwnd(),	
		GWL_WNDPROC, 
		(LONG_PTR)GlobalPreviewDialogProc);
	m_allPreviewDialogs.SetAt(pParentWnd->GetSafeHwnd(),this);
}


void COXPreviewDialog::OnFolderChange()
{
	CFileDialog::OnFolderChange();

	if(m_bPreview)
	{
		// resize list control
		CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
		ASSERT(pListCtrl!=NULL);
		CRect rectListCtrl;
		pListCtrl->GetWindowRect(rectListCtrl);
		// align right side of the list control with right side of 
		// the extended combo box
		CWnd* pExtComboBox=GetParent()->GetDlgItem(cmb2);
		ASSERT(pExtComboBox!=NULL);
		CRect rectExtCombo;
		pExtComboBox->GetWindowRect(rectExtCombo);
		rectListCtrl.right=rectExtCombo.right-GetPreviewWndOffset();
		GetParent()->ScreenToClient(rectListCtrl);
		pListCtrl->MoveWindow(rectListCtrl);
		m_nOldListCtrlWidth=rectListCtrl.Width();
		m_nOldListCtrlHeight=rectListCtrl.Height();

		OnFileNameChange();
	}
}


HWND COXPreviewDialog::FindListCtrl()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// find toolbar window
	CWnd* pParentWnd=GetParent();
	ASSERT(pParentWnd!=NULL);
	CWnd* pChildWnd=pParentWnd->GetWindow(GW_CHILD);
	while(pChildWnd!=NULL)
	{
		CString sClassName;
		::GetClassName(pChildWnd->GetSafeHwnd(),
			sClassName.GetBuffer(MAX_PATH),MAX_PATH);
		if(sClassName.CompareNoCase(_T("SHELLDLL_DefView"))==0)
		{
			return pChildWnd->GetSafeHwnd();
		}
		else
		{
			pChildWnd=pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}

	return NULL;
}


HWND COXPreviewDialog::FindToolBar()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// find toolbar window
	CWnd* pParentWnd=GetParent();
	ASSERT(pParentWnd!=NULL);
	CWnd* pChildWnd=pParentWnd->GetWindow(GW_CHILD);
	while(pChildWnd!=NULL)
	{
		CString sClassName;
		::GetClassName(pChildWnd->GetSafeHwnd(),
			sClassName.GetBuffer(MAX_PATH),MAX_PATH);
		if(sClassName.CompareNoCase(_T("ToolbarWindow32")) == 0)
		{
			// Make sure we get the horizontal toolbar
			CRect rectClient;
			pChildWnd->GetClientRect(rectClient);
			if (rectClient.Width() > rectClient.Height())
				return pChildWnd->GetSafeHwnd(); // This is the one we want!
			else
				pChildWnd=pChildWnd->GetWindow(GW_HWNDNEXT);
		}
		else
		{
			pChildWnd=pChildWnd->GetWindow(GW_HWNDNEXT);
		}
	}

	return NULL;
}


void COXPreviewDialog::SetFlatToolBar(BOOL bFlat)
{
	ASSERT(!IsOldStyle());

	m_bFlatToolbar=bFlat;

	if(::IsWindow(GetSafeHwnd()))
	{
		CWnd* pToolBarWnd=CWnd::FromHandle(FindToolBar());
		if(pToolBarWnd!=NULL)
		{
			pToolBarWnd->ModifyStyle((m_bFlatToolbar ? 0 : TBSTYLE_FLAT),
				(m_bFlatToolbar ? TBSTYLE_FLAT : 0));
		}
	}
}


void COXPreviewDialog::UpdateToolBar()
{
	ASSERT(::IsWindow(GetSafeHwnd()));
	ASSERT(!IsOldStyle());
    ASSERT(!m_bInitialized);

	// adjust toolbar
	CWnd* pToolBarWnd=CWnd::FromHandle(FindToolBar());
	ASSERT(pToolBarWnd!=NULL);
	// remove the first button separator 
	pToolBarWnd->SendMessage(TB_DELETEBUTTON,0,0);

#if _MFC_VER>0x0421
	BOOL bSizable=((GetParent()->GetStyle()&WS_THICKFRAME)==WS_THICKFRAME);
	if(bSizable)
	{
		// set list style in order to display preview button text
		pToolBarWnd->ModifyStyle(0,TBSTYLE_LIST);
		for(int nIndex=0; nIndex<pToolBarWnd->SendMessage(TB_BUTTONCOUNT); nIndex++)
		{
			TBBUTTON button;
			VERIFY(pToolBarWnd->SendMessage(TB_GETBUTTON,nIndex,(LPARAM)&button));
			int nID=button.idCommand;
			if(nID!=ID_SEPARATOR)
			{
				TBBUTTONINFO buttonInfo={ sizeof(TBBUTTONINFO) };
				buttonInfo.dwMask=TBIF_STYLE;
				VERIFY(pToolBarWnd->
					SendMessage(TB_GETBUTTONINFO,nID,(LPARAM)&buttonInfo)!=-1);
				if((buttonInfo.fsStyle & TBSTYLE_AUTOSIZE)==0)
				{
					buttonInfo.fsStyle|=TBSTYLE_AUTOSIZE;
					VERIFY(pToolBarWnd->
						SendMessage(TB_SETBUTTONINFO,nID,(LPARAM)&buttonInfo));
				}
			}
		}
	}
#endif

	// add button for preview mode
	TBBUTTON button;
#if _MFC_VER>=0x0420
	// update toolbar image list with new button
	CImageList* pImageList=
		CImageList::FromHandle((HIMAGELIST)pToolBarWnd->SendMessage(TB_GETIMAGELIST));
	ASSERT(pImageList!=NULL);
	CBitmap bitmap;
	VERIFY(bitmap.LoadBitmap(IDB_OX_PREVIEWBUTTON));
	button.iBitmap=pImageList->Add(&bitmap,RGB(192,192,192));
	ASSERT(button.iBitmap!=-1);
	pToolBarWnd->SendMessage(TB_SETIMAGELIST,0,(LPARAM)pImageList->GetSafeHandle());
#else
	// some number
	button.iBitmap=6;
#endif
	button.idCommand=OX_IDC_SET_PREVIEW;
	button.fsState=(BYTE)(TBSTATE_ENABLED);
	button.fsStyle=TBSTYLE_CHECK;
#if _MFC_VER>0x0421
	if(bSizable)
	{
		button.fsStyle|=TBSTYLE_AUTOSIZE;
	}
#endif
	button.dwData=0;
	CString sItem;
	VERIFY(sItem.LoadString(IDS_OX_PREVIEWDIALOGPRV));//"Preview"
	button.iString=pToolBarWnd->
		SendMessage(TB_ADDSTRING,0,(LPARAM)(LPCTSTR) sItem);
	pToolBarWnd->SendMessage(TB_INSERTBUTTON,
		pToolBarWnd->SendMessage(TB_BUTTONCOUNT,0,0),(LPARAM)&button);
	pToolBarWnd->SendMessage(TB_AUTOSIZE);
	pToolBarWnd->SendMessage(TB_SETBUTTONSIZE,0,MAKELPARAM(24,22));

	// adjust toolbar and extended combo box positions
	CRect rectToolBar;
	pToolBarWnd->GetWindowRect(rectToolBar);
	rectToolBar.right+=8;
#if _MFC_VER>0x0421
	if(bSizable)
	{
		rectToolBar.left-=72;
	}
	else
#endif
	{
		rectToolBar.left-=6;
	}

	// extended combo for drives and folders
	CWnd* pExtComboBox=GetParent()->GetDlgItem(cmb2);
	ASSERT(pExtComboBox!=NULL);
	CRect rectExtCombo;
	pExtComboBox->GetWindowRect(rectExtCombo);
	rectExtCombo.right=rectToolBar.left-2;

	GetParent()->ScreenToClient(rectToolBar);
	pToolBarWnd->MoveWindow(rectToolBar);
	GetParent()->ScreenToClient(rectExtCombo);
	pExtComboBox->MoveWindow(rectExtCombo);
	///////////////////////////////////////////////////

	// force the toolbar to send command messages to this window
	pToolBarWnd->SendMessage(TB_SETPARENT,(WPARAM)GetSafeHwnd());
}


void COXPreviewDialog::AdjustControls()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// parent dialog window
	CWnd* pParentWnd=GetParent();
	ASSERT(pParentWnd!=NULL);

	// toolbar
	CWnd* pToolBarWnd=CWnd::FromHandle(FindToolBar());
	ASSERT(pToolBarWnd!=NULL);
	CRect rectToolBar;
	pToolBarWnd->GetWindowRect(rectToolBar);

	// list control
	CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
	ASSERT(pListCtrl!=NULL);
	CRect rectListCtrl;
	pListCtrl->GetWindowRect(rectListCtrl);
	ASSERT(rectListCtrl.Width()>=OX_MIN_SHELLITEMSLISTCTRL_WIDTH);

	// the change margin 
	int nChangedWidth=rectListCtrl.Width()-m_nOldListCtrlWidth;
	int nChangedHeight=rectListCtrl.Height()-m_nOldListCtrlHeight;
	m_nOldListCtrlWidth=rectListCtrl.Width();
	m_nOldListCtrlHeight=rectListCtrl.Height();


	if (!IsWindows2K())
	{
		// OK button
		CWnd* pOKButtonWnd=pParentWnd->GetDlgItem(IDOK);
		ASSERT(pOKButtonWnd!=NULL);
		CRect rectOKButton;
		pOKButtonWnd->GetWindowRect(rectOKButton);
		rectOKButton.OffsetRect(nChangedWidth,0);
		pParentWnd->ScreenToClient(rectOKButton);
		pOKButtonWnd->MoveWindow(rectOKButton);

		// Cancel button
		CWnd* pCancelButtonWnd=pParentWnd->GetDlgItem(IDCANCEL);
		ASSERT(pCancelButtonWnd!=NULL);
		CRect rectCancelButton;
		pCancelButtonWnd->GetWindowRect(rectCancelButton);
		rectCancelButton.OffsetRect(nChangedWidth,0);
		pParentWnd->ScreenToClient(rectCancelButton);
		pCancelButtonWnd->MoveWindow(rectCancelButton);

		// filename edit box
		CWnd* pFilenameEditWnd=pParentWnd->GetDlgItem(edt1);
		ASSERT(pFilenameEditWnd!=NULL);
		CRect rectFilenameEdit;
		pFilenameEditWnd->GetWindowRect(rectFilenameEdit);
		rectFilenameEdit.right+=nChangedWidth;
		pParentWnd->ScreenToClient(rectFilenameEdit);
		pFilenameEditWnd->MoveWindow(rectFilenameEdit);

		// filetype combo box
		CWnd* pFiletypeComboWnd=pParentWnd->GetDlgItem(cmb1);
		ASSERT(pFiletypeComboWnd!=NULL);
		CRect rectFiletypeCombo;
		pFiletypeComboWnd->GetWindowRect(rectFiletypeCombo);
		rectFiletypeCombo.right+=nChangedWidth;
		pParentWnd->ScreenToClient(rectFiletypeCombo);
		pFiletypeComboWnd->MoveWindow(rectFiletypeCombo);

	}
	// align toolbar with right side of the parent window
	pParentWnd->ScreenToClient(rectToolBar);
	CRect rectClient;
	pParentWnd->GetClientRect(rectClient);
	if(rectToolBar.right!=rectClient.right-2)
	{
		rectToolBar.OffsetRect(rectClient.right-2-rectToolBar.right,0);
		pToolBarWnd->MoveWindow(rectToolBar);
	}
	pParentWnd->ClientToScreen(rectToolBar);

	// resize extended combo box
	CWnd* pExtComboBox=pParentWnd->GetDlgItem(cmb2);
	ASSERT(pExtComboBox!=NULL);
	CRect rectExtCombo;
	pExtComboBox->GetWindowRect(rectExtCombo);
	rectExtCombo.right=rectToolBar.left-2;
	pParentWnd->ScreenToClient(rectExtCombo);
	pExtComboBox->MoveWindow(rectExtCombo);

	// update preview area
	if(IsInPreviewMode() && ::IsWindow(m_preview.GetSafeHwnd()))
	{
		m_rectPreviewArea.OffsetRect(nChangedWidth,0);
		m_rectPreviewArea.bottom+=nChangedHeight;
		m_preview.MoveWindow(m_rectPreviewArea);
		m_preview.Invalidate(FALSE);

		// update "splitter" area coordinates
		m_rectSplitterArea.OffsetRect(nChangedWidth,0);
		m_rectSplitterArea.bottom+=nChangedHeight;

		// update "resizable" area coordinates
		m_rectResizableArea.InflateRect(0,0,nChangedWidth,nChangedHeight);
	}
}


void COXPreviewDialog::OnFileNameChange()
	// Notification for Windows 95 and Windows NT 4.0
{
	int nFileCount;
	CString sPreviewFile;
	
	sPreviewFile=GetPathName();
	COXFilePath pathPreview(sPreviewFile);	
	if(sPreviewFile.IsEmpty())
	{
		nFileCount=0;
	}
	else if(pathPreview.Exists())
	{
		nFileCount = 1;
	}
	else
	{
		nFileCount=MORE_THAN_ONE;
	}

	if(nFileCount==1)
	{
		m_sSelectedFile=pathPreview.GetPath();
	}
	else
	{
		m_sSelectedFile.Empty();
	}
	
	CheckFileSelection();
}


BOOL COXPreviewDialog::OnWndMsg(UINT message,WPARAM wParam,
								LPARAM lParam,LRESULT* pResult)
{
	if(message==WM_NOTIFY)
	{
		NMHDR* pNMHDR=(NMHDR*)lParam;
		if(pNMHDR->hwndFrom==FindToolBar())
		{
			*pResult=GetParent()->SendMessage(message,wParam,lParam);
			return TRUE;
		}
		// tooltip support
		if(pNMHDR->code==TTN_NEEDTEXTA || pNMHDR->code==TTN_NEEDTEXTW)
		{
			// need to handle both ANSI and UNICODE versions of the message
			TOOLTIPTEXTA* pTTTA=(TOOLTIPTEXTA*)pNMHDR;
			TOOLTIPTEXTW* pTTTW=(TOOLTIPTEXTW*)pNMHDR;
			CString sToolTipText;
			VERIFY(sToolTipText.LoadString(IDS_OX_PREVIEWDIALOGTOOLTIP));//"Preview selected file"
			UINT_PTR nID=pNMHDR->idFrom;
			if(pNMHDR->code==TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
				pNMHDR->code==TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
			{
				// idFrom is actually the HWND of the tool
				nID=::GetDlgCtrlID((HWND)nID);
			}

			// tooltip for preview button
			if(nID==OX_IDC_SET_PREVIEW) 
			{
#ifndef _UNICODE
				if (pNMHDR->code==TTN_NEEDTEXTA)
				{
					lstrcpyn(pTTTA->szText,sToolTipText,
						sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0]));
				}
				else
				{
					_mbstowcsz(pTTTW->szText,sToolTipText,
						sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0]));
				}
#else
				if (pNMHDR->code==TTN_NEEDTEXTA)
				{
					_wcstombsz(pTTTA->szText,sToolTipText,
						sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0]));
				}
				else
				{
					lstrcpyn(pTTTW->szText,sToolTipText,
						sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0]));
				}
#endif
				*pResult=0;

				// bring the tooltip window above other popup windows
				::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
					SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

				return TRUE;    // message was handled
			}

			*pResult=GetParent()->SendMessage(message,wParam,lParam);
			return TRUE;
		}
	}
	else if(message==WM_DESTROY)
	{
		SaveState();
	}

	return CFileDialog::OnWndMsg(message,wParam,lParam,pResult);
}


BOOL COXPreviewDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(wParam==OX_IDC_SET_PREVIEW)
	{
		SetPreview(!IsInPreviewMode());
		return TRUE;
	}
	return (BOOL)GetParent()->SendMessage(WM_COMMAND,wParam,lParam);
}


BOOL COXPreviewDialog::InsertFileViewer(COXFileViewer* pFileViewer, int nIndex)
{
	ASSERT(pFileViewer!=NULL);
	int nFileViewerCount=GetFileViewerCount();
	ASSERT(nIndex>=0 && nIndex<=nFileViewerCount);

	m_arrFileViewers.Add(NULL);
	for(int nViewerIndex=nFileViewerCount-1; nViewerIndex>=nIndex; nViewerIndex--)
	{
		m_arrFileViewers.SetAt(nViewerIndex+1,m_arrFileViewers[nViewerIndex]);
	}
	m_arrFileViewers.SetAt(nIndex,pFileViewer);
	pFileViewer->m_pPreviewDialog=this;

	if(::IsWindow(m_preview.GetSafeHwnd()) && m_preview.IsWindowVisible())
	{
		m_preview.RedrawWindow();
	}

	return TRUE;
}


BOOL COXPreviewDialog::RemoveFileViewer(int nIndex)
{
	int nFileViewerCount=GetFileViewerCount();
	ASSERT(nIndex>=0 && nIndex<=nFileViewerCount);

	for(int nViewerIndex=nIndex+1; nViewerIndex<nFileViewerCount; nViewerIndex++)
	{
		m_arrFileViewers.SetAt(nViewerIndex-1,m_arrFileViewers[nViewerIndex]);
	}
	m_arrFileViewers.RemoveAt(nFileViewerCount-1);

	if(::IsWindow(m_preview.GetSafeHwnd()) && m_preview.IsWindowVisible())
	{
		m_preview.RedrawWindow();
	}

	return TRUE;
}


COXFileViewer* COXPreviewDialog::GetFileViewer(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<=GetFileViewerCount());
	return m_arrFileViewers[nIndex];
}


int COXPreviewDialog::SetPreviewWndOffset(int nPreviewWndOffset)
{
	int nDifference=nPreviewWndOffset-GetPreviewWndOffset();
	if(::IsWindow(m_preview.GetSafeHwnd()) && 
		m_preview.IsWindowVisible() && nDifference!=0)
	{
		// resize preview window and list control
		//
		CWnd* pListCtrl=CWnd::FromHandle(FindListCtrl());
		ASSERT(pListCtrl!=NULL);
		CRect rectListCtrl;
		pListCtrl->GetWindowRect(rectListCtrl);
		GetParent()->ScreenToClient(rectListCtrl);
		rectListCtrl.right-=nDifference;
		if(rectListCtrl.right<rectListCtrl.left+OX_MIN_SHELLITEMSLISTCTRL_WIDTH)
		{
			nPreviewWndOffset-=rectListCtrl.left-rectListCtrl.right+
				OX_MIN_SHELLITEMSLISTCTRL_WIDTH;
			nDifference=nPreviewWndOffset-GetPreviewWndOffset();
			rectListCtrl.right=rectListCtrl.left+OX_MIN_SHELLITEMSLISTCTRL_WIDTH;
		}
		else if(rectListCtrl.right>m_rectPreviewArea.right-OX_MIN_PREVIEWWND_WIDTH)
		{
			nPreviewWndOffset+=rectListCtrl.right-m_rectPreviewArea.right+
				OX_MIN_PREVIEWWND_WIDTH;
			nDifference=nPreviewWndOffset-GetPreviewWndOffset();
			rectListCtrl.right=m_rectPreviewArea.right-OX_MIN_PREVIEWWND_WIDTH;
		}

		if(nDifference!=0)
		{
			pListCtrl->MoveWindow(rectListCtrl);
			m_nOldListCtrlWidth=rectListCtrl.Width();

			m_rectPreviewArea.left-=nDifference;
			m_preview.MoveWindow(m_rectPreviewArea);
			m_preview.Invalidate(FALSE);

			m_rectSplitterArea.OffsetRect(-nDifference,0);

			// notify all file viewers
			for(int nIndex=0; nIndex<GetFileViewerCount(); nIndex++)
			{
				COXFileViewer* pFileViewer=m_arrFileViewers[nIndex];
				ASSERT(pFileViewer!=NULL);
				pFileViewer->OnChangeSize();
			}
		}
	}

	m_nPreviewWndOffset=nPreviewWndOffset;

	return GetPreviewWndOffset();
}


void COXPreviewDialog::SaveState(LPCTSTR lpszProfileName/*=_T("COXPreviewDialog Settings")*/)
{
	CString sProfileName(lpszProfileName);
	ASSERT(!sProfileName.IsEmpty());
	CWinApp* pApp=AfxGetApp();
	ASSERT(pApp!=NULL);
	pApp->WriteProfileInt(lpszProfileName,_T("Preview"),IsInPreviewMode());
	pApp->WriteProfileInt(lpszProfileName,_T("Preview Window Offset"),
		GetPreviewWndOffset());
}


void COXPreviewDialog::LoadPreviewState(LPCTSTR lpszProfileName/*=_T("COXPreviewDialog Settings")*/)
{
	CString sProfileName(lpszProfileName);
	ASSERT(!sProfileName.IsEmpty());
	CWinApp* pApp=AfxGetApp();
	ASSERT(pApp!=NULL);
	BOOL bPreview=(BOOL)pApp->GetProfileInt(lpszProfileName,
		_T("Preview"),IsInPreviewMode());
    SetPreview(bPreview);
}

void COXPreviewDialog::LoadPreviewWndOffsetState(LPCTSTR lpszProfileName/*=_T("COXPreviewDialog Settings")*/)
{
	CString sProfileName(lpszProfileName);
	ASSERT(!sProfileName.IsEmpty());
	CWinApp* pApp=AfxGetApp();
	ASSERT(pApp!=NULL);
	int nOffset=(int)pApp->GetProfileInt(lpszProfileName,
		_T("Preview Window Offset"),GetPreviewWndOffset());
	SetPreviewWndOffset(nOffset);
}



BOOL COXPreviewDialog::IsWindows2K()
{
	OSVERSIONINFO VersionInfo;

	::ZeroMemory(&VersionInfo, sizeof(VersionInfo));
	VersionInfo.dwOSVersionInfoSize=sizeof(VersionInfo);
	::GetVersionEx(&VersionInfo);
	if (VersionInfo.dwMajorVersion>4)
		return TRUE;
	else
		return FALSE;
}
