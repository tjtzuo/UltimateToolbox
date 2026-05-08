// ==========================================================================
// 				Class Implementation : 	  COXOptionTreeCtrl
// ==========================================================================

// Header file : OXOptionTreeCtrl.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "OXOptionTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef TVM_SETBKCOLOR
#define TVM_SETBKCOLOR              (TV_FIRST + 29)
#endif

/////////////////////////////////////////////////////////////////////////////
// COXOptionTreeCtrl

COXOptionTreeCtrl::COXOptionTreeCtrl() :
	m_bUseHighContrast(FALSE)
{
}

COXOptionTreeCtrl::~COXOptionTreeCtrl()
{
	ASSERT(m_mapItems.GetCount()==0);
	ASSERT(m_mapReadOnlyItems.GetCount()==0);
}


BEGIN_MESSAGE_MAP(COXOptionTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(COXOptionTreeCtrl)
	ON_NOTIFY_REFLECT_EX(TVN_KEYDOWN, OnKeydown)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT_EX(TVN_DELETEITEM,OnDeleteItem)
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(TVM_SETBKCOLOR,OnSetBkColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXOptionTreeCtrl message handlers

BOOL COXOptionTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
    TV_KEYDOWN* pTVKeyDown=(TV_KEYDOWN*)pNMHDR;
    *pResult=0;

    if(pTVKeyDown->wVKey==VK_SPACE)
    {
        HTREEITEM hItem=GetSelectedItem();
        if(hItem!=NULL && !IsReadOnly(hItem))
        {
            ToggleItem(hItem);
        }
    }

	return FALSE;
}

void COXOptionTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	// prevents beep
    if(nChar!=VK_SPACE)
    {
        CTreeCtrl::OnChar(nChar,nRepCnt,nFlags);
    }
}

void COXOptionTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    HTREEITEM hItem=GetClickedOptionItem();

    if(hItem!=NULL && !IsReadOnly(hItem))
    {
        if(::GetFocus()!=GetSafeHwnd())
            SetFocus();

        ToggleItem(hItem);
        SelectItem(hItem);
    }
    else
    {
        CTreeCtrl::OnLButtonDown(nFlags,point);
    }
}

void COXOptionTreeCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CheckHightContrast();
	SetImageList(GetImageList(),TVSIL_NORMAL);

	CWnd::OnSettingChange(uFlags, lpszSection);
}


LRESULT COXOptionTreeCtrl::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	LRESULT lResult=Default();

	CheckHightContrast();
	SetImageList(GetImageList(),TVSIL_NORMAL);

	return lResult;
}


void COXOptionTreeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
	// hook not already in progress
	if(pThreadState->m_pWndInit==NULL)
	{
		if(!Initialize())
		{
			TRACE(_T("COXOptionTreeCtrl::PreSubclassWindow: failed to initialize the control\n"));
		}
	}
	
	CTreeCtrl::PreSubclassWindow();
}

int COXOptionTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!Initialize())
	{
		TRACE(_T("COXOptionTreeCtrl::OnCreate: failed to initialize the control\n"));
		return -1;
	}
	
	return 0;
}


BOOL COXOptionTreeCtrl::OnDeleteItem(NMHDR* pNotifyStruct, LRESULT* result)
{
	LPNMTREEVIEW lpNMTV=(LPNMTREEVIEW)pNotifyStruct;
	ASSERT(lpNMTV!=NULL);
	UINT nID=GetIDFromItem(lpNMTV->itemOld.hItem);
	m_mapItems.RemoveKey(nID);
	m_mapReadOnlyItems.RemoveKey(lpNMTV->itemOld.hItem);
	*result=0;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////


void COXOptionTreeCtrl::ToggleItem(HTREEITEM hItem)
{
    ASSERT(hItem!=NULL);

    UpdateWindow();

    if(IsCheckBox(hItem))
    {
        if(GetCheck(hItem)==OTITEM_CHECKED)
        {
            SetCheckBox(hItem,OTITEM_UNCHECKED);
        }
        else
        {
            SetCheckBox(hItem,OTITEM_CHECKED);
        }
        // Send MODIFIED message to parent if needed.
    }
    else if (IsRadioButton(hItem))
    {
        if(GetCheck(hItem)==OTITEM_UNCHECKED)
        {
            SetRadioButton(hItem,OTITEM_CHECKED);
            // Send MODIFIED message to parent if needed.
        }
    }

    // avoid unnecessary flicker
    RECT rect;
    GetItemRect(hItem,&rect,TRUE);
    ValidateRect(&rect);
}


HTREEITEM COXOptionTreeCtrl::GetClickedOptionItem() const
{
	ASSERT(::IsWindow(GetSafeHwnd()));

    // Get the current cursor coordinate
    CPoint pt;
    ::GetCursorPos(&pt);
    ScreenToClient(&pt);

	UINT nFlags=NULL;
	HTREEITEM hItem=HitTest(pt,&nFlags);
	if(nFlags&TVHT_ONITEM && (IsCheckBox(hItem) || IsRadioButton(hItem)))
	{
		if((GetStyle()&TVS_EDITLABELS)==TVS_EDITLABELS && (nFlags&TVHT_ONITEMLABEL))
			return NULL;
		return hItem;
	}
	else
		return NULL;
}


HTREEITEM COXOptionTreeCtrl::AddControlGroup(LPCTSTR pszText, 
											 HTREEITEM hParent/*=NULL*/, 
											 BOOL bExpanded/*=TRUE*/,
											 int nImageIndex/*=-1*/,
											 int nSelectedImageIndex/*=-1*/,
											 HTREEITEM hInsertAfter/*=TVI_LAST*/)
{
    TV_INSERTSTRUCT	tvs;

    tvs.hParent=hParent;
    tvs.hInsertAfter=hInsertAfter;
    tvs.item.mask=TVIF_CHILDREN|TVIF_STATE|TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;

    tvs.item.state=(bExpanded ? TVIS_EXPANDED : 0);
	tvs.item.stateMask=tvs.item.state;
    tvs.item.cChildren=1;

    tvs.item.pszText = (LPTSTR)pszText;

	tvs.item.iImage=nImageIndex;
	tvs.item.iSelectedImage=nSelectedImageIndex;

    return InsertItem(&tvs);
}


HTREEITEM COXOptionTreeCtrl::AddCheckBox(UINT uID, LPCTSTR pszText, 
										 HTREEITEM hParent,
										 int nCheck/*=OTITEM_UNCHECKED*/,
										 HTREEITEM hInsertAfter/*=TVI_LAST*/, 
										 BOOL bReadOnly/*=FALSE*/)
{
    if(uID==NULL || GetItemFromID(uID)!=NULL)
    {
        TRACE(_T("COXOptionTreeCtrl::AddCheckBox: invalid/duplicated ID\n"));
        ASSERT(FALSE);// Give a chance to debug.
        return NULL;
    }

    TV_INSERTSTRUCT	tvs;

    tvs.hParent=hParent;
    tvs.hInsertAfter=hInsertAfter;
    tvs.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;

    tvs.item.pszText=(LPTSTR)pszText;
    tvs.item.iImage=((nCheck==OTITEM_CHECKED) ? 
		CHECKED_IMAGE_INDEX : UNCHECKED_IMAGE_INDEX);
	tvs.item.iSelectedImage=tvs.item.iImage;

    return AddOptionItem(uID,&tvs,bReadOnly);
}


HTREEITEM COXOptionTreeCtrl::AddRadioButton(UINT uID, LPCTSTR pszText, 
											HTREEITEM hParent,
											int nCheck/*=OTITEM_UNCHECKED*/,
											HTREEITEM hInsertAfter/*=TVI_LAST*/, 
											BOOL bReadOnly/*=FALSE*/)
{
    if(uID==NULL || GetItemFromID(uID)!=NULL)
    {
        TRACE(_T("COXOptionTreeCtrl::AddRadioButt: invalid/duplicated ID\n"));
        ASSERT(FALSE);// Give a chance to debug.
        return NULL;
    }

    TV_INSERTSTRUCT	tvs;

    tvs.hParent=hParent;
    tvs.hInsertAfter=hInsertAfter;
    tvs.item.mask=TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;

    tvs.item.pszText=(LPTSTR)pszText;
    tvs.item.iImage=((nCheck==OTITEM_CHECKED) ? 
		RADIO_IMAGE_INDEX : UNRADIOED_IMAGE_INDEX);
	tvs.item.iSelectedImage=tvs.item.iImage;

    return AddOptionItem(uID,&tvs,bReadOnly);
}


HTREEITEM COXOptionTreeCtrl::AddOptionItem(UINT uID, LPTV_INSERTSTRUCT ptvs, 
										   BOOL bReadOnly/*=FALSE*/)
{
    HTREEITEM hItem=InsertItem(ptvs);
    if(hItem!=NULL)
	{
		m_mapItems.SetAt(uID,hItem);
		m_mapReadOnlyItems.SetAt(hItem,bReadOnly);
	}
    return hItem;
}


HTREEITEM COXOptionTreeCtrl::GetItemFromID(UINT uID) const
{
    HTREEITEM hItem=NULL;
    m_mapItems.Lookup(uID, (void*&)hItem);
    return hItem;
}


UINT COXOptionTreeCtrl::GetIDFromItem(HTREEITEM hItemFrom) const
{
    UINT uID;
    HTREEITEM hItem;

    for(POSITION pos=m_mapItems.GetStartPosition(); pos!=NULL; )
    {
        m_mapItems.GetNextAssoc(pos,uID,(void*&)hItem);
        if(hItem==hItemFrom)
            return uID;
    }

    return NULL;
}


void COXOptionTreeCtrl::SetCheck(UINT uID, int nCheck)
{
    ASSERT(nCheck==OTITEM_UNCHECKED || nCheck==OTITEM_CHECKED);

    HTREEITEM hItem=GetItemFromID(uID);
	if(hItem!=NULL)
	{
		SetCheck(hItem,nCheck);
	}
	else
	{
		TRACE(_T("COXOptionTreeCtrl::SetCheck: item with specified ID wasn't found\n"));
	}
}


void COXOptionTreeCtrl::SetCheck(HTREEITEM hItem, int nCheck)
{
    ASSERT(nCheck==OTITEM_UNCHECKED || nCheck==OTITEM_CHECKED);
    ASSERT(hItem!=NULL);

    if(GetCheck(hItem)==nCheck)
    {
        // Already Set. Just return.
        return;
    }

    if(IsCheckBox(hItem))
    {
        SetCheckBox(hItem,nCheck);
    }
    else if(IsRadioButton(hItem))
    {
        SetRadioButton(hItem,nCheck);
    }
}


void COXOptionTreeCtrl::SetCheckBox(HTREEITEM hItem, int nCheck)
{
    ASSERT(hItem!=NULL);
    ASSERT(IsCheckBox(hItem));

    int nImage=-1;
	switch(nCheck)
	{
	case OTITEM_UNCHECKED:
		nImage=UNCHECKED_IMAGE_INDEX;
		break;
	case OTITEM_CHECKED:
		nImage=CHECKED_IMAGE_INDEX;
		break;
	default:
        ASSERT(FALSE);
	}

	int nOldCheck=GetCheck(hItem);

    VERIFY(SetItemImage(hItem,nImage,nImage));

	// notify parent
	NotifyOptionChanged(hItem,nOldCheck,nCheck);
}


void COXOptionTreeCtrl::SetRadioButton(HTREEITEM hItem, int nCheck)
{
    ASSERT(hItem!=NULL);
    ASSERT(IsRadioButton(hItem));

    int nImage=-1;

	switch(nCheck)
	{
	case OTITEM_UNCHECKED:
		{
			nImage=UNRADIOED_IMAGE_INDEX;
			break;
		}
	case OTITEM_CHECKED:
		{
			nImage=RADIO_IMAGE_INDEX;
			HTREEITEM hsibling=hItem;

			// Set all other radio buttons ABOVE to OFF
			while((hsibling=GetPrevSiblingItem(hsibling))!=NULL)
			{
				if(!IsRadioButton(hsibling))
					break;
				if(GetCheck(hsibling)!=OTITEM_UNCHECKED)
					SetRadioButton(hsibling,OTITEM_UNCHECKED);
			}

			hsibling=hItem;
			// Set all other radio buttons BELOW to OFF
			while((hsibling=GetNextSiblingItem(hsibling))!=NULL)
			{
				if(!IsRadioButton(hsibling))
					break;
				if(GetCheck(hsibling)!=OTITEM_UNCHECKED)
					SetRadioButton(hsibling,OTITEM_UNCHECKED);
			}
			break;
		}
	default:
        ASSERT(FALSE);
	}

	int nOldCheck=GetCheck(hItem);

    VERIFY(SetItemImage(hItem,nImage,nImage));

	// notify parent
	NotifyOptionChanged(hItem,nOldCheck,nCheck);
}


int COXOptionTreeCtrl::GetCheck(UINT uID) const
{
    HTREEITEM hItem=GetItemFromID(uID);
    return GetCheck(hItem);
}


int COXOptionTreeCtrl::GetCheck(HTREEITEM hItem) const
{
    int nImage=-1;
	int nSelectedImage=-1;
    VERIFY(GetItemImage(hItem,nImage,nSelectedImage));

    if(nImage==UNCHECKED_IMAGE_INDEX || nImage==UNRADIOED_IMAGE_INDEX)
    {
        return OTITEM_UNCHECKED;
    }

    if(nImage==CHECKED_IMAGE_INDEX || nImage==RADIO_IMAGE_INDEX)
    {
        return OTITEM_CHECKED;
    }

    ASSERT(FALSE);
    return -1;
}


BOOL COXOptionTreeCtrl::IsCheckBox(HTREEITEM hItem) const
{
    ASSERT(hItem!=NULL);

    int nImage=-1;
	int nSelectedImage=-1;
    VERIFY(GetItemImage(hItem,nImage,nSelectedImage));

    if(nImage==CHECKED_IMAGE_INDEX || nImage==UNCHECKED_IMAGE_INDEX)
    {
        return TRUE;
    }

    return FALSE;
}


BOOL COXOptionTreeCtrl::IsRadioButton(HTREEITEM hItem) const
{
    ASSERT(hItem!=NULL);

    int nImage=-1;
	int nSelectedImage=-1;
    VERIFY(GetItemImage(hItem,nImage,nSelectedImage));

    if(nImage==RADIO_IMAGE_INDEX || nImage==UNRADIOED_IMAGE_INDEX)
    {
        return TRUE;
    }

    return FALSE;
}


void COXOptionTreeCtrl::SetReadOnly(UINT uID, BOOL bReadOnly)
{
    HTREEITEM hItem=GetItemFromID(uID);
	if(hItem!=NULL)
	{
		SetReadOnly(hItem,bReadOnly);
	}
	else
	{
		TRACE(_T("COXOptionTreeCtrl::SetReadOnly: item with specified ID wasn't found\n"));
	}
}


void COXOptionTreeCtrl::SetReadOnly(HTREEITEM hItem, BOOL bReadOnly)
{
    ASSERT(hItem!=NULL);

    if(IsCheckBox(hItem) || IsRadioButton(hItem))
    {
		m_mapReadOnlyItems.SetAt(hItem,bReadOnly);
    }
}


BOOL COXOptionTreeCtrl::IsReadOnly(HTREEITEM hItem) const
{
    ASSERT(hItem!=NULL);

	BOOL bReadOnly=FALSE;
    if(m_mapReadOnlyItems.Lookup(hItem,bReadOnly))
    {
		return bReadOnly;
    }
	else
	{
		ASSERT(!(IsCheckBox(hItem) || IsRadioButton(hItem)));
		return FALSE;
	}
}


UINT COXOptionTreeCtrl::GetCheckedRadioButton(UINT uIDFirstButton, 
											  UINT uIDLastButton) const
{
    HTREEITEM hItemFirst=GetItemFromID(uIDFirstButton);
    HTREEITEM hItemLast=GetItemFromID(uIDLastButton);
	ASSERT(hItemFirst!=NULL && hItemLast!=NULL);

    // Just checking....
    HTREEITEM hsibling = hItemFirst;
    do
    {
        if(GetCheck(hsibling)!=OTITEM_UNCHECKED)
        {
            return GetIDFromItem(hsibling);
        }

        if (hsibling == hItemLast)
        {
            break;
        }

        hsibling = GetNextSiblingItem(hsibling);

    } while(hsibling!=NULL);

    return NULL;
}


void COXOptionTreeCtrl::CheckRadioButton(UINT uIDCheckButton)
{
    SetCheck(uIDCheckButton,OTITEM_CHECKED);
}


BOOL COXOptionTreeCtrl::Initialize()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	if((HIMAGELIST)m_imageList==NULL)
	{
		if(!m_imageList.Create(IDB_OX_OPTIONTREE_IMAGELIST,16,0,RGB(192,192,192)))
		{
			TRACE(_T("COXOptionTreeCtrl::Initialize: failed to create image list\n"));
			ASSERT(FALSE);
			return FALSE;
		}
	}

	if((HIMAGELIST)m_imageListHighContrast==NULL)
	{
		if(!m_imageListHighContrast.
			Create(IDB_OX_OPTIONTREE_HIGHCONTRAST_IMAGELIST,16,0,RGB(192,192,192)))
		{
			TRACE(_T("COXOptionTreeCtrl::Initialize: failed to create image list\n"));
			ASSERT(FALSE);
			return FALSE;
		}
	}

	CheckHightContrast();
	SetImageList(GetImageList(),TVSIL_NORMAL);

	return TRUE;
}


int COXOptionTreeCtrl::AddImage(LPCTSTR lpszImageResourceID, 
								COLORREF clrMask/*=RGB(192,192,192)*/,
								LPCTSTR lpszHighContrastImageResourceID/*=NULL*/, 
								COLORREF clrHighContrastMask/*=RGB(192,192,192)*/)
{
	CBitmap bitmap;
	bitmap.LoadBitmap(lpszImageResourceID);
	int nImageIndex=m_imageList.Add(&bitmap,clrMask);
	if(lpszHighContrastImageResourceID==NULL)
	{
		VERIFY(m_imageListHighContrast.Add(&bitmap,clrMask)==nImageIndex);
	}
	else
	{
		CBitmap bitmapHighContrast;
		bitmapHighContrast.LoadBitmap(lpszHighContrastImageResourceID);
		VERIFY(m_imageListHighContrast.
			Add(&bitmapHighContrast,clrHighContrastMask)==nImageIndex);
		bitmapHighContrast.DeleteObject();
	}
	bitmap.DeleteObject();

	return nImageIndex;
}


// Save to registry state of check boxes and radio buttons
BOOL COXOptionTreeCtrl::SaveState(LPCTSTR lpszSubKey, LPCTSTR lpszValueName) const
{
#ifndef _MAC
	CWinApp* pApp=AfxGetApp();
	CString sProfileName;
	sProfileName.Format(_T("%s_%s"),lpszSubKey,lpszValueName);

	UINT uID=NULL;
	HTREEITEM hItem=NULL;
	CString sItemID(_T(""));
    for(POSITION pos=m_mapItems.GetStartPosition(); pos!=NULL; )
    {
        m_mapItems.GetNextAssoc(pos,uID,(void*&)hItem);
		ASSERT(hItem!=NULL && uID!=NULL && (IsCheckBox(hItem) || IsRadioButton(hItem)));
		sItemID.Format(_T("%d"),uID);
		pApp->WriteProfileInt(sProfileName,sItemID,GetCheck(hItem));
	}

	return TRUE;
#else
	return FALSE;
#endif
}

// Load from registry state of check boxes and radio buttons
BOOL COXOptionTreeCtrl::LoadState(LPCTSTR lpszSubKey, LPCTSTR lpszValueName)
{
#ifndef _MAC
	CWinApp* pApp=AfxGetApp();
	CString sProfileName;
	sProfileName.Format(_T("%s_%s"),lpszSubKey,lpszValueName);

	UINT uID=NULL;
	HTREEITEM hItem=NULL;
	CString sItemID(_T(""));
    for(POSITION pos=m_mapItems.GetStartPosition(); pos!=NULL; )
    {
        m_mapItems.GetNextAssoc(pos,uID,(void*&)hItem);
		ASSERT(hItem!=NULL && uID!=NULL && (IsCheckBox(hItem) || IsRadioButton(hItem)));
		sItemID.Format(_T("%d"),uID);
		int nCheck=pApp->GetProfileInt(sProfileName,sItemID,-1);
		if(nCheck!=-1)
			SetCheck(hItem,nCheck);
	}

	return TRUE;
#else
	return FALSE;
#endif
}


LRESULT COXOptionTreeCtrl::NotifyOptionChanged(HTREEITEM hItem, int nOldCheck, 
											   int nNewCheck)
{
    ASSERT(nOldCheck==OTITEM_UNCHECKED || nOldCheck==OTITEM_CHECKED);
    ASSERT(nNewCheck==OTITEM_UNCHECKED || nNewCheck==OTITEM_CHECKED);
	ASSERT(hItem!=NULL);
	ASSERT(IsCheckBox(hItem) || IsRadioButton(hItem));

	UINT uID=GetIDFromItem(hItem);
	ASSERT(uID!=NULL);

	// notify parent
	CWnd* pParentWnd=GetParent();
	if(pParentWnd!=NULL)
	{
		NMOPTIONTREE nmot;
		nmot.hdr.hwndFrom=GetSafeHwnd();
		nmot.hdr.idFrom=GetDlgCtrlID();
		nmot.hdr.code=OTN_OPTIONCHANGED;
		nmot.hItem=hItem;
		nmot.uItemID=uID;
		nmot.nOldCheck=nOldCheck;
		nmot.nNewCheck=nNewCheck;

		return (pParentWnd->SendMessage(WM_NOTIFY,(WPARAM)nmot.hdr.idFrom,
			(LPARAM)&nmot));
	}
	else
		return (LRESULT)0;
}


void COXOptionTreeCtrl::CheckHightContrast()
{
	COLORREF clrBack=(COLORREF)-1;
#if _MFC_VER>0x0421
	if(::IsWindow(GetSafeHwnd()))
	{
		clrBack=GetBkColor();
	}
#endif
	if(clrBack==(COLORREF)-1)
	{
		clrBack=::GetSysColor(COLOR_WINDOW);
	}

	m_bUseHighContrast=(GetRValue(clrBack)+GetGValue(clrBack)+GetBValue(clrBack)<300);
}



BOOL COXOptionTreeCtrl::DeleteAllItems()
{
	m_mapItems.RemoveAll();
	return(CTreeCtrl::DeleteAllItems());

}
