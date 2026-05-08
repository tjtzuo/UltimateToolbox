// ==========================================================================
//						   Class Implementation 
// 							  COXHistoryCtrl
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
#include "OXImageListBox.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COXImageListBox

COXImageListBox::COXImageListBox(int cx/*=::GetSystemMetrics(SM_CXSMICON)*/,
								 int cy/*=::GetSystemMetrics(SM_CYSMICON)*/, 
								 UINT nFlags/*=ILC_COLOR24|ILC_MASK*/) :
	m_nOrigWidth(cx),
	m_nOrigHeight(cy),
	m_clrBackground(::GetSysColor(COLOR_WINDOW)),
	m_clrHighlight(::GetSysColor(COLOR_HIGHLIGHT))

{
	VERIFY(m_imageList.Create(cx,cy,nFlags,0,0));
}


COXImageListBox::~COXImageListBox()
{
}


BEGIN_MESSAGE_MAP(COXImageListBox, CListBox)
	//{{AFX_MSG_MAP(COXImageListBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_DELETESTRING,OnDeleteString)
	ON_MESSAGE(LB_RESETCONTENT,OnResetContent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXImageListBox message handlers

int COXImageListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!InitializeImageListBox())
	{
		TRACE(_T("COXImageListBox::OnCreate: failed to initialize the control\n"));
		return -1;
	}
	
	return 0;
}


void COXImageListBox::OnDestroy() 
{
	EmptyImageList();
	CListBox::OnDestroy();
}


void COXImageListBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	DWORD dwStyle=GetStyle();
	// make sure LBS_OWNERDRAWFIXED and LBS_MULTICOLUMN styles are specified
	ASSERT(dwStyle&LBS_OWNERDRAWFIXED);
	ASSERT(dwStyle&LBS_MULTICOLUMN);
	// make sure LBS_OWNERDRAWVARIABLE and LBS_MULTIPLESEL styles are not specified
	ASSERT(!(dwStyle&(LBS_OWNERDRAWVARIABLE|LBS_MULTIPLESEL)));

	// make sure the control is empty
	ASSERT(GetCount()==0);
	
	_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
	// hook not already in progress
	if(pThreadState->m_pWndInit==NULL)
	{
		if(!InitializeImageListBox())
		{
			TRACE(_T("COXImageListBox::PreSubclassWindow: failed to initialize the control\n"));
		}
	}

	CListBox::PreSubclassWindow();
}


BOOL COXImageListBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	// make sure LBS_OWNERDRAWVARIABLE and LBS_MULTIPLESEL styles are not specified
	cs.style&=~(LBS_OWNERDRAWVARIABLE|LBS_MULTIPLESEL);
	// make sure LBS_OWNERDRAWFIXED and LBS_MULTICOLUMN styles are specified
	cs.style|=LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN;

	return CListBox::PreCreateWindow(cs);
}


BOOL COXImageListBox::InitializeImageListBox()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	if(m_imageList.GetImageCount()>0)
	{
		IMAGEINFO imageInfo;
		VERIFY(m_imageList.GetImageInfo(0,&imageInfo));
		CRect rect(imageInfo.rcImage);
		SetItemHeight(0,rect.Height()+rect.Height()/4);
		SetColumnWidth(rect.Width()+rect.Width()/4);
	}
	else
	{
		SetItemHeight(0,m_nOrigWidth+m_nOrigWidth/4);
		SetColumnWidth(m_nOrigHeight+m_nOrigHeight/4);
	}

	return TRUE;
}


LRESULT COXImageListBox::OnDeleteString(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	LRESULT lResult=Default();
	if(lResult!=LB_ERR)
	{
		m_imageList.Remove(PtrToInt(wParam));
		// update internal data
		for(int nIndex=(int)wParam; nIndex<GetCount(); nIndex++)
		{
			SetItemData(nIndex,GetItemData(nIndex)-1);
		}
	}

	return lResult;
}


LRESULT COXImageListBox::OnResetContent(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	LRESULT lResult=Default();
#if _MFC_VER>0x0421
	VERIFY(m_imageList.SetImageCount(0));
#else
	while(m_imageList.Remove(0));
#endif

	return lResult;
}


void COXImageListBox::EmptyImageList()
{
	ResetContent();
}


int COXImageListBox::SetImageList(CImageList* pImageList)
{
	EmptyImageList();

	if(pImageList==NULL || pImageList->GetImageCount()==0)
		return 0;

	VERIFY(m_imageList.DeleteImageList());

	IMAGEINFO imageInfo;
	VERIFY(pImageList->GetImageInfo(0,&imageInfo));
	CRect rect(imageInfo.rcImage);
#if _MFC_VER > 0x0421
	VERIFY(m_imageList.Create(pImageList));
#else
	VERIFY(m_imageList.Create(rect.Width(),rect.Height(),ILC_COLOR24|ILC_MASK,0,0));
	for(int nButtonIndex=0; nButtonIndex<pImageList->GetImageCount(); nButtonIndex++)
	{
		HICON hIcon=pImageList->ExtractIcon(nButtonIndex);
		ASSERT(hIcon!=NULL);
		VERIFY(m_imageList.Add(hIcon)!=-1);
		VERIFY(::DestroyIcon(hIcon));
	}
#endif
	SetItemHeight(0,rect.Height()+rect.Height()/4);
	SetColumnWidth(rect.Width()+rect.Width()/4);

	for(int nIndex=0; nIndex<m_imageList.GetImageCount(); nIndex++)
	{
		AddString((LPCTSTR) (INT_PTR)nIndex);
	}

	return m_imageList.GetImageCount();
}


int COXImageListBox::AddImageList(CImageList* pImageList)
{
	if(m_imageList.GetImageCount()==0)
		return -1;

	ASSERT(pImageList!=NULL);
	if(pImageList==NULL)
		return -1;

	if(pImageList->GetImageCount()==0)
		return -1;

	IMAGEINFO imageInfo;
	VERIFY(pImageList->GetImageInfo(0,&imageInfo));
	CRect rect(imageInfo.rcImage);
	VERIFY(m_imageList.GetImageInfo(0,&imageInfo));
	if(rect.Width()!=imageInfo.rcImage.right-imageInfo.rcImage.left ||
		rect.Height()!=imageInfo.rcImage.bottom-imageInfo.rcImage.top)
	{
		return -1;
	}

	int nLastImage=GetCount();
	int nIndex=0;
	for(nIndex=0; nIndex<pImageList->GetImageCount(); nIndex++)
	{
		HICON hIcon=pImageList->ExtractIcon(nIndex);
		ASSERT(hIcon!=NULL);
		VERIFY(m_imageList.Add(hIcon)!=-1);
		VERIFY(::DestroyIcon(hIcon));
	}

	for(nIndex=nLastImage; nIndex<m_imageList.GetImageCount(); nIndex++)
	{
		VERIFY(AddString((LPCTSTR)(INT_PTR)nIndex)>=0);
	}

	return nLastImage;
}
	
int COXImageListBox::AddImage(CBitmap* pbmImage, CBitmap* pbmMask)
{
	if(m_imageList.Add(pbmImage,pbmMask)==-1)
		return -1;

	return AddString((LPCTSTR)(INT_PTR)GetCount());
}

int COXImageListBox::AddImage(CBitmap* pbmImage, COLORREF crMask)
{
	if(m_imageList.Add(pbmImage,crMask)==-1)
		return -1;

	return AddString((LPCTSTR)(INT_PTR)GetCount());
}

int COXImageListBox::AddImage(HICON hIcon)
{
	if(m_imageList.Add(hIcon)==-1)
		return -1;

	return AddString((LPCTSTR)(INT_PTR)GetCount());
}


BOOL COXImageListBox::ReplaceImage(int nImage, CBitmap* pbmImage, 
								   CBitmap* pbmMask/*=NULL*/)
{
	if(nImage<0 || nImage>=GetCount())
		return FALSE;

	return m_imageList.Replace(nImage,pbmImage,pbmMask);
}

BOOL COXImageListBox::ReplaceImage(int nImage, HICON hIcon)
{
	if(nImage<0 || nImage>=GetCount())
		return FALSE;

	return m_imageList.Replace(nImage,hIcon);
}


BOOL COXImageListBox::GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
{
	ASSERT(pImageInfo!=NULL);
	if(nImage<0 || nImage>=GetCount())
		return FALSE;

	return m_imageList.GetImageInfo(nImage,pImageInfo);
}

HICON COXImageListBox::GetIcon(int nImage)
{
	if(nImage<0 || nImage>=GetCount())
		return NULL;

	return m_imageList.ExtractIcon(nImage);
}


HICON COXImageListBox::GetSelectedIcon()
{
	int nImage=GetCurSel();
	if(nImage==LB_ERR)
		return NULL;

	return m_imageList.ExtractIcon(nImage);
}


BOOL COXImageListBox::LoadIconsFromFile(LPCTSTR lpszFileName/*=NULL*/, 
										BOOL bSmallIcon/*=TRUE*/)
{
	// it might take time to load icons from big file
	CWaitCursor waitCursor;

	// make sure the specified file exists 
	if(lpszFileName!=NULL && lstrlen(lpszFileName)>0)
	{
		CFileStatus fileStatus;
		if(!CFile::GetStatus(lpszFileName,fileStatus))
		{
			TRACE(_T("COXImageListBox::LoadIconsFromFile: failed to find file %s\n"),lpszFileName);
			return FALSE;
		}
	}

	CString sFileName;
	// if NULL then retrieve the name of the running executable
	if(lpszFileName==NULL || lstrlen(lpszFileName)==0)
	{
		//get the application path
		::GetModuleFileName(NULL,sFileName.GetBuffer(MAX_PATH),MAX_PATH);
		sFileName.ReleaseBuffer();
	}
	else
	{
		sFileName=lpszFileName;
	}

	if(sFileName.IsEmpty())
	{
		TRACE(_T("COXImageListBox::LoadIconsFromFile: empty file name has been specified\n"));
		return FALSE;
	}

	CImageList imageList;
	// create new image list to accomodate icon size
	if(bSmallIcon)
	{
		VERIFY(imageList.Create(::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON),ILC_COLOR24|ILC_MASK,0,0));
	}
	else
	{
		VERIFY(imageList.Create(::GetSystemMetrics(SM_CXICON),
			::GetSystemMetrics(SM_CYICON),ILC_COLOR24|ILC_MASK,0,0));
	}

	// try to extract icons from the file
	UINT nIconCount=::ExtractIconEx(sFileName,-1,NULL,NULL,0);
	if(nIconCount==0)
		return TRUE;

	HICON* arrIcons=new HICON[nIconCount];
	ASSERT(arrIcons!=NULL);
	if(bSmallIcon)
	{
		VERIFY(::ExtractIconEx(sFileName,0,NULL,arrIcons,nIconCount)==nIconCount);
	}
	else
	{
		VERIFY(::ExtractIconEx(sFileName,0,arrIcons,NULL,nIconCount)==nIconCount);
	}

	for(UINT nIndex=0; nIndex<nIconCount; nIndex++)
	{
		VERIFY(imageList.Add(arrIcons[nIndex])!=-1);
		::DestroyIcon(arrIcons[nIndex]);
	}

	delete[] arrIcons;

	return (SetImageList(&imageList)==(int)nIconCount);
}


void COXImageListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	if(m_imageList.GetImageCount()==0)
		return;
	
	int nImageIndex=(int)lpDrawItemStruct->itemData;
	if(nImageIndex<0 || nImageIndex>=GetCount())
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect(lpDrawItemStruct->rcItem);
	CRect rectIcon=rect;
	rectIcon.DeflateRect(rect.Width()/10,rect.Height()/10);

	if((lpDrawItemStruct->itemAction & (ODA_SELECT|ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		CBrush brush(lpDrawItemStruct->itemState & ODS_SELECTED ?
			m_clrHighlight : m_clrBackground);
		pDC->FillRect(&rect,&brush);
	}
	else if((lpDrawItemStruct->itemAction & (ODA_FOCUS|ODA_DRAWENTIRE)) &&
		!(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		CBrush brush((lpDrawItemStruct->itemState & ODS_SELECTED) ?
			m_clrHighlight : m_clrBackground);
		pDC->FrameRect(rect,&brush);
	}

	if (!(lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & ODA_SELECT))
	{
		// Item has been unselected - remove frame
		pDC->IntersectClipRect(rect);
		SendMessage(WM_ERASEBKGND,(WPARAM)lpDrawItemStruct->hDC);
	}

	if(lpDrawItemStruct->itemAction&(ODA_SELECT|ODA_DRAWENTIRE))
	{
		UINT nStyle=ILD_TRANSPARENT;
		m_imageList.Draw(pDC,nImageIndex,rectIcon.TopLeft(),nStyle);
	}

}



BOOL COXImageListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rect;
	GetClientRect(rect);
	if(GetCount()==0)
	{
		CBrush brush(m_clrBackground);
		pDC->FillRect(rect,&brush);
	}
	else 
	{
		CRect rectItem;
		VERIFY(GetItemRect(GetCount()-1,rectItem)!=LB_ERR);
		if(rectItem.bottom<rect.bottom)
		{
			CRect rectUpdate=rect;
			rectUpdate.top=rectItem.bottom;
			CBrush brush(m_clrBackground);
			pDC->FillRect(rectUpdate,&brush);
		}

		if(rectItem.right<rect.right)
		{
			CRect rectUpdate=rect;
			rectUpdate.left=rectItem.right;
			CBrush brush(m_clrBackground);
			pDC->FillRect(rectUpdate,&brush);
		}
	}

	return TRUE;
}
