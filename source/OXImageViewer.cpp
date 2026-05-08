// ==========================================================================
// 						Class Implementation : COXImageViewer
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
#include "OXImageViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(COXImageViewer, COXScrollWnd)


int COXImageViewer::m_nDisplayColors=-1;

/////////////////////////////////////////////////////////////////////////////
// COXImageViewer

COXImageViewer::COXImageViewer() : m_clrBackground(::GetSysColor(COLOR_WINDOW))
{
	m_bUseTrackZoom=TRUE;
	SetSmoothScrolling(TRUE);
}

COXImageViewer::~COXImageViewer()
{
}


BEGIN_MESSAGE_MAP(COXImageViewer, COXScrollWnd)
	//{{AFX_MSG_MAP(COXImageViewer)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDM_OX_IMGVIEVER_ROTATE90,IDM_OX_IMGVIEVER_ROTATE270,OnRotate)
	ON_COMMAND_RANGE(IDM_OX_IMGVIEVER_FLIPVERT,IDM_OX_IMGVIEVER_FLIPHORZ,OnFlip)
	ON_COMMAND_RANGE(IDM_OX_IMGVIEVER_ALIGNTOPLEFT,IDM_OX_IMGVIEVER_ALIGNBOTTOMRIGHT,OnAlign)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COXImageViewer message handlers



void COXImageViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_dib.IsEmpty())
		return;
	
	// TODO: Add your message handler code here
	OnPrepareDC(&dc);

	// get the size of image
	CSize sizeDIB=GetDIBSize();
	CRect rect(0,0,sizeDIB.cx,sizeDIB.cy);
	CRect rectPaint=rect;
	// transform coordinates of boundary rectangle
	// taking into account current zoom level
	NormalToScaled(&rectPaint);

	///
	// we have to revert Y-coordinates
	// to get right print output
	UINT diff=rect.bottom-rect.top;
	rect.bottom=sizeDIB.cy-rect.top;
	rect.top=rect.bottom-diff;
	///
	DrawDIB(&dc,rectPaint,rect);

	// Do not call COXScrollWnd::OnPaint() for painting messages
}

BOOL COXImageViewer::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	ASSERT_VALID(pDC);

	CBrush brush(m_clrBackground);

	//Bug-fix - by Nish - Mar 9th, 2005
 	if(IsEmpty())
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect,&brush);
	}
 	else
		FillOutsideRect(pDC,&brush);	

	return TRUE;
}

void COXImageViewer::OnSetFocus(CWnd* pOldWnd) 
{
	COXScrollWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	
	DoRealizePalette(TRUE,TRUE);
}

void COXImageViewer::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	COXScrollWnd::OnSettingChange(uFlags, lpszSection);
	
	// TODO: Add your message handler code here
	
	m_nDisplayColors=-1;
}

void COXImageViewer::OnPaletteChanged(CWnd* pFocusWnd) 
{
	// TODO: Add your message handler code here
    if(pFocusWnd!=this)
	{
		DoRealizePalette(TRUE,FALSE);
    }
}

BOOL COXImageViewer::OnQueryNewPalette() 
{
	// TODO: Add your message handler code here and/or call default
	return (DoRealizePalette(TRUE,FALSE)>0 ? TRUE : FALSE);
}

//////////////////////////////////////////////////////

BOOL COXImageViewer::LoadFile(LPCTSTR lpszPathName) 
{
	// try to open file on read
	CFile file;
	BOOL bSuccess=FALSE;
	TRY
	{
		bSuccess=file.Open(lpszPathName,CFile::modeRead);
	}
	CATCH(CFileException, e)
	{
		TCHAR szCause[255];
        e->GetErrorMessage(szCause, 255);
        TRACE(_T("COXImageViewer::LoadFile:exception: %s"),szCause);

		bSuccess=FALSE;
	}
	END_CATCH

	if(!bSuccess)
		return FALSE;

	return LoadFile(&file);
}


BOOL COXImageViewer::LoadFile(CFile* pFile) 
{
	pFile->SeekToBegin();
	// just try to read DIB file
	if(!m_dib.Read(pFile))
	{
		TRACE(_T("COXImageViewer::LoadFile: Unknown file format!"));
		return FALSE;
	}

	return InitializeImage();
}


#ifdef OXDIB_SUPPORTJPEG
BOOL COXImageViewer::LoadJPEGFile(LPCTSTR lpszPathName)
{
	// just try to read JPEG file
	if(!m_dib.ReadJPEG(lpszPathName))
	{
		TRACE(_T("COXImageViewer::LoadJPEGFile: Unknown file format!\n"));
		return FALSE;
	}

	return InitializeImage();
}

BOOL COXImageViewer::LoadJPEGFile(CFile* pFile)
{
	// just try to read JPEG file
	if(!m_dib.ReadJPEG(pFile))
	{
		TRACE(_T("COXImageViewer::LoadJPEGFile: Unknown file format!\n"));
		return FALSE;
	}

	return InitializeImage();
}
#endif


BOOL COXImageViewer::LoadResource(LPCTSTR lpszResource) 
{
	// just try to load DIB resource
	if(!m_dib.LoadResource(lpszResource))
	{
		TRACE(_T("COXImageViewer::LoadResource: Unknown file format!\n"));
		return FALSE;
	}

	return InitializeImage();
}


BOOL COXImageViewer::Empty(BOOL bRedraw/*=TRUE*/)
{
	if(m_dib.IsEmpty())
	{
		return TRUE;
	}

	m_dib.Empty();
	m_dibDither.Empty();

	if(bRedraw)
	{
		RedrawWindow();
	}

	return TRUE;
}


void COXImageViewer::SetBackgroundColor(COLORREF clrBackground, BOOL bRedraw/*=TRUE*/)
{
	if(m_clrBackground!=clrBackground)
	{
		m_clrBackground=clrBackground;
		if(bRedraw)
		{
			if(!IsEmpty() && CheckUsePalette())
			{
				DoRealizePalette(TRUE,TRUE);
			}
			else
			{
				RedrawWindow();
			}
		}
	}
}


BOOL COXImageViewer::UpdateImage(BOOL bRedraw/*=TRUE*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));
	if(InitializeImage(m_pageDev,m_lineDev))
	{
		if(bRedraw)
		{
			if(CheckUsePalette())
			{
				DoRealizePalette(TRUE,TRUE);
			}
			else
			{
				RedrawWindow();
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL COXImageViewer::InitializeImage(const CSize& sizePage/*=CSize(10,10)*/,
									 const CSize& sizeLine/*=CSize(1,1)*/)
{
	ASSERT(!m_dib.IsEmpty());

	m_dibDither=NULL;
	// if current video color palette is no more than 256 colors and
	// DIB file uses no less than 256 color then create dithered copy
	// of the file to render it on the display (this way we will get
	// better output quality)
	if(CheckUseDithered())
	{
		m_dibDither=m_dib;
		m_dibDither.HalfToneDitherDIB();
	}

	CSize sizeDIB=GetDIBSize();
	SetScrollSizes(MM_TEXT,sizeDIB,sizePage,sizeLine);
	SetZoomAlign(ZA_CENTER);
	
	if(IsAlwaysFitWindow())
		ZoomToWindow();
	else
		RedrawWindow();
	
	return TRUE;
}


BOOL COXImageViewer::CheckUseDithered()
{
	// if current video color palette is no more than 256 colors and
	// DIB file uses more than 256 color then create dithered copy
	// of the file to render it on the display (this way we will get
	// better output quality)
	if(m_nDisplayColors==-1)
	{
		CWindowDC dc(this);
		m_nDisplayColors=dc.GetDeviceCaps(BITSPIXEL);
	}
	return (m_nDisplayColors==8 && m_dib.GetNumColors()==0);
}


BOOL COXImageViewer::CheckUsePalette()
{
	// if current video color mode is able to display no more than 256 colors and
	// DIB file uses no less than 256 color then palette must be used
	return (CheckUseDithered() || (m_nDisplayColors==8 && m_dib.GetNumColors()==256));
}


BOOL COXImageViewer::DrawDIB(CDC* pDC, const CRect& rectDest, 
							 const CRect& rectSrc)
{
	// set neccessary map mode
	int oldMapMode;
	oldMapMode=pDC->SetMapMode(MM_TEXT);

	// depending on current palette use original or dithered image 
	// to render image on display
	int bSuccess;
	if(!CheckUseDithered())
		bSuccess=m_dib.Paint(pDC,rectDest,rectSrc);
	else
		bSuccess=m_dibDither.Paint(pDC,rectDest,rectSrc);

	pDC->SetMapMode(oldMapMode);

	return bSuccess;
}


UINT COXImageViewer::DoRealizePalette(BOOL bRedraw/*=TRUE*/, 
									  BOOL bForeground/*=TRUE*/)
{
	UINT nCount=0;
	// applicable only when palette is used
	if(CheckUsePalette())
	{
		// retrieve image palette
		CPalette* pPalette=GetDIBPalette();
		if(pPalette!=NULL)
		{
			CClientDC dc(this);
			CPalette* pOldPalette=dc.SelectPalette(pPalette,!bForeground);
			nCount=dc.RealizePalette();
			if(nCount>0)
			{
				// if image pallete is selected then
				// redraw image if needed
				if(bRedraw)
				{
					RedrawWindow(NULL,NULL,
						RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_FRAME|
						RDW_ERASENOW|RDW_INTERNALPAINT|RDW_ALLCHILDREN);
				}
				dc.SelectPalette(pOldPalette,!bForeground);
			}
		}
    }

	return nCount;
}


CPalette* COXImageViewer::GetDIBPalette()
{
	// depending on the current color palette 
	// return original palette or palette of dithered file
	if(!CheckUseDithered())
		return m_dib.GetPalette();
	else
		return m_dibDither.GetPalette();
}


CSize COXImageViewer::GetDIBSize()
{
	// depending on the current color palette 
	// return original size or size of dithered file
	if(!CheckUseDithered())
		return m_dib.GetSize();
	else
		return m_dibDither.GetSize();
}


BOOL COXImageViewer::OnPopulateContextMenu(CMenu* pMenu, CPoint& point)
{
	UNREFERENCED_PARAMETER(point);

	if(pMenu==NULL)
		return TRUE;

	// create "Align" popup menu
	//
	CMenu menuAlign;
	if(menuAlign.CreatePopupMenu())
	{
		CString sItem;
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ALIGNTOPLEFT));
		menuAlign.AppendMenu(MF_STRING|MF_CHECKED,IDM_OX_IMGVIEVER_ALIGNTOPLEFT,sItem);
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ALIGNCENTER));
		menuAlign.AppendMenu(MF_STRING|MF_CHECKED,IDM_OX_IMGVIEVER_ALIGNCENTER,sItem);
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ALIGNBOTTOMRIGHT));
		menuAlign.AppendMenu(MF_STRING|MF_CHECKED,IDM_OX_IMGVIEVER_ALIGNBOTTOMRIGHT,
			sItem);

		menuAlign.CheckMenuItem(IDM_OX_IMGVIEVER_ALIGNTOPLEFT,MF_BYCOMMAND|
			((GetContentsAlign()==CA_TOPLEFT) ? MF_CHECKED : MF_UNCHECKED));
		menuAlign.CheckMenuItem(IDM_OX_IMGVIEVER_ALIGNCENTER,MF_BYCOMMAND|
			((GetContentsAlign()==CA_CENTER) ? MF_CHECKED : MF_UNCHECKED));
		menuAlign.CheckMenuItem(IDM_OX_IMGVIEVER_ALIGNBOTTOMRIGHT,MF_BYCOMMAND|
			((GetContentsAlign()==CA_BOTTOMRIGHT) ? MF_CHECKED : MF_UNCHECKED));

		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ALIGNPOPUPMENU));
		pMenu->InsertMenu(0,MF_BYPOSITION|MF_STRING|MF_POPUP,
			(UINT_PTR)menuAlign.Detach(),sItem);
		pMenu->InsertMenu(1,MF_BYPOSITION|MF_SEPARATOR);
	}

	// create "Rotate" popup menu
	//
	CMenu menuRotate;
	if(menuRotate.CreatePopupMenu())
	{
		CString sItem;
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ROTATE90));
		menuRotate.AppendMenu(MF_STRING,IDM_OX_IMGVIEVER_ROTATE90,sItem);
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ROTATE180));
		menuRotate.AppendMenu(MF_STRING,IDM_OX_IMGVIEVER_ROTATE180,sItem);
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ROTATE270));
		menuRotate.AppendMenu(MF_STRING,IDM_OX_IMGVIEVER_ROTATE270,sItem);

		if(pMenu->GetMenuItemCount()>0)
			pMenu->AppendMenu(MF_SEPARATOR);
		VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_ROTATEPOPUPMENU));
		pMenu->AppendMenu(MF_STRING|MF_POPUP,
			(UINT_PTR)menuRotate.Detach(),sItem);

		// create "Flip" popup menu
		//
		CMenu menuFlip;
		if(menuFlip.CreatePopupMenu())
		{
			VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_FLIPVERT));
			menuFlip.AppendMenu(MF_STRING,IDM_OX_IMGVIEVER_FLIPVERT,sItem);
			VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_FLIPHORZ));
			menuFlip.AppendMenu(MF_STRING,IDM_OX_IMGVIEVER_FLIPHORZ,sItem);
			VERIFY(sItem.LoadString(IDS_OX_IMGVIEVER_FLIPPOPUPMENU));
			pMenu->AppendMenu(MF_STRING|MF_POPUP,
				(UINT_PTR)menuFlip.Detach(),sItem);
		}
	}

	return TRUE;
}


void COXImageViewer::OnRotate(UINT nID)
{
	int nRotateAngle=0;
	switch(nID)
	{
	case IDM_OX_IMGVIEVER_ROTATE90:
		nRotateAngle=90;
		break;
	case IDM_OX_IMGVIEVER_ROTATE180:
		nRotateAngle=180;
		break;
	case IDM_OX_IMGVIEVER_ROTATE270:
		nRotateAngle=270;
		break;
	default:
		ASSERT(FALSE);
	}

	CWaitCursor wait;
	m_dib.Rotate(nRotateAngle);
	UpdateImage();
}


void COXImageViewer::OnFlip(UINT nID)
{
	BOOL bFlipVert=FALSE;
	BOOL bFlipHorz=FALSE;
	switch(nID)
	{
	case IDM_OX_IMGVIEVER_FLIPVERT:
		bFlipVert=TRUE;
		break;
	case IDM_OX_IMGVIEVER_FLIPHORZ:
		bFlipHorz=TRUE;
		break;
	default:
		ASSERT(FALSE);
	}

	CWaitCursor wait;
	m_dib.Rotate(0,bFlipHorz,bFlipVert);
	UpdateImage();
}


void COXImageViewer::OnAlign(UINT nID)
{
	ContentsAlignment align=CA_TOPLEFT;
	switch(nID)
	{
	case IDM_OX_IMGVIEVER_ALIGNTOPLEFT:
		align=CA_TOPLEFT;
		break;
	case IDM_OX_IMGVIEVER_ALIGNCENTER:
		align=CA_CENTER;
		break;
	case IDM_OX_IMGVIEVER_ALIGNBOTTOMRIGHT:
		align=CA_BOTTOMRIGHT;
		break;
	default:
		ASSERT(FALSE);
	}

	SetContentsAlign(align);
}



