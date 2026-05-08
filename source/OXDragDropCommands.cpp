// ==========================================================================
// 					Class Implementation : COXDragDropCommands
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

#include "OXDragDropCommands.h"
#include "OXBitmapMenuOrganizer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////

// format for drag'n'drop item
CLIPFORMAT COXDragDropCommands::m_cfCommandButton=
	(CLIPFORMAT)::RegisterClipboardFormat(_T("UTB_Command_Button"));


COleDataSource* COXDragDropCommands::PrepareDragDropData(LPCTSTR lpszText, 
														 int nImageIndex, 
														 int nCommandID, 
														 BYTE fsStyle/*=0*/,
														 HMENU hSubmenu/*=NULL*/)
{
	ASSERT(lpszText!=NULL);

	// allocate our data object
	COleDataSource* pDataSource=new COleDataSource;
	
	//////////////////////
	// internal m_cfCommandButton format
	//
	// command ID + (text) + (NULL) + image index + button style + 
	// if finished  -	0
	// otherwise	-	1	if next item in menu 
	//					2	if previous one is the last in hierarchy + 
	// command ID + (text) + (NULL) + image index + button style + ...
	//////////////////////

	int nEntryMemSize=(lstrlen(lpszText)+1)*sizeof(TCHAR)+3*sizeof(int)+sizeof(BYTE);
	if(hSubmenu!=NULL)
	{
		nEntryMemSize+=CalcMenuDataSize(hSubmenu);
	}
	// allocate the memory
	HGLOBAL hgItemData=::GlobalAlloc(GPTR,nEntryMemSize);   
	BYTE* lpItemData=(BYTE*)GlobalLock(hgItemData);
     
	::ZeroMemory(lpItemData,nEntryMemSize);

	// if image hasn't been specified and item is not a popup menu
	// the we wil  try to find out if there is image assigned to the
	// item by COXBitmapMenuOrganizer object
	if(nCommandID!=-1 && nImageIndex==-1)
	{
		COXBitmapMenuOrganizer* pBMOrganizer=
			COXBitmapMenuOrganizer::FindOrganizer(AfxGetMainWnd()->GetSafeHwnd());
		if(pBMOrganizer!=NULL)
		{
			COXImageInfo* pImageInfo=pBMOrganizer->GetMenuItemImageInfo(nCommandID);
			if(pImageInfo!=NULL)
			{
				nImageIndex=pImageInfo->GetIndex();
			}
		}
	}

	WriteItemDragDropData(lpItemData,lpszText,nImageIndex,nCommandID,fsStyle);
	if(hSubmenu!=NULL)
	{
		lpItemData-=sizeof(int);
		(*(int*)lpItemData)=2;
		lpItemData+=sizeof(int);
		WriteMenuDragDropData(lpItemData,hSubmenu);
	}
	lpItemData-=sizeof(int);
	(*(int*)lpItemData)=0;
	lpItemData+=sizeof(int);

	::GlobalUnlock(hgItemData); 

	// save the item data
	pDataSource->CacheGlobalData(m_cfCommandButton,hgItemData);

	return pDataSource;
}


DROPEFFECT COXDragDropCommands::DoDragDrop(COleDataSource* pDataSource,
										   COleDropSource* pOleDropSource)
{
	ASSERT(pDataSource!=NULL);
	ASSERT(pOleDropSource!=NULL);

	// start drag'n'drop operation
	return pDataSource->DoDragDrop(DROPEFFECT_COPY|DROPEFFECT_MOVE,
		NULL,pOleDropSource);
}


int COXDragDropCommands::CalcMenuDataSize(HMENU hMenu)
{
	ASSERT(hMenu!=NULL);
	ASSERT(::IsMenu(hMenu));

	int nSize=0;

	CMenu* pMenu=CMenu::FromHandle(hMenu);
	COXBitmapMenu* pBitmapMenu=DYNAMIC_DOWNCAST(COXBitmapMenu,pMenu);

	int nItemCount=pMenu->GetMenuItemCount();
	for(int nIndex=0; nIndex<nItemCount; nIndex++)
	{
		CString sText;
		if(pBitmapMenu!=NULL)
		{
			MENUITEMINFO mii={ sizeof(MENUITEMINFO) };
			mii.fMask=MIIM_DATA;
#if _MFC_VER > 0x0421
			VERIFY(pBitmapMenu->GetMenuItemInfo(nIndex,&mii,TRUE));
#else
			VERIFY(::GetMenuItemInfo(pBitmapMenu->m_hMenu,nIndex,TRUE,&mii));
#endif
			COXItemInfo* pItemInfo=(COXItemInfo*)mii.dwItemData;
			ASSERT(pItemInfo!=NULL);
			sText=pItemInfo->GetText();
		}
		else
		{
			pMenu->GetMenuString(nIndex,sText,MF_BYPOSITION);
		}

		nSize+=sText.GetLength()+sizeof(TCHAR)+3*sizeof(int)+sizeof(BYTE);
		if(pMenu->GetMenuItemID(nIndex)==(UINT)-1)
		{
			nSize+=CalcMenuDataSize(pMenu->GetSubMenu(nIndex)->GetSafeHmenu());
		}
	}

	return nSize;
}


void COXDragDropCommands::WriteItemDragDropData(BYTE*& lpData, LPCTSTR lpszText, 
												int nImageIndex, int nCommandID, 
												BYTE fsStyle)
{
	ASSERT(lpData!=NULL);

	// write button command ID into global memory
	(*(int*)lpData)=nCommandID;
	lpData+=sizeof(int);

	// write button text into global memory
	lstrcpy((LPTSTR)lpData,lpszText);
	lpData+=(lstrlen(lpszText)+1)*sizeof(TCHAR);

	// write button image index into global memory
	(*(int*)lpData)=nImageIndex;
	lpData+=sizeof(int);

	// write button style into global memory
	(*(BYTE*)lpData)=fsStyle;
	lpData+=sizeof(BYTE);

	// finish flag set to FALSE
	(*(int*)lpData)=1;
	lpData+=sizeof(int);
}


void COXDragDropCommands::WriteMenuDragDropData(BYTE*& lpData, HMENU hMenu)
{
	ASSERT(lpData!=NULL);
	ASSERT(hMenu!=NULL);
	ASSERT(::IsMenu(hMenu));

	CMenu* pMenu=CMenu::FromHandle(hMenu);
	COXBitmapMenu* pBitmapMenu=DYNAMIC_DOWNCAST(COXBitmapMenu,pMenu);

	int nItemCount=pMenu->GetMenuItemCount();
	for(int nIndex=0; nIndex<nItemCount; nIndex++)
	{
		CString sText;
		if(pBitmapMenu!=NULL)
		{
			MENUITEMINFO mii={ sizeof(MENUITEMINFO) };
			mii.fMask=MIIM_DATA;
#if _MFC_VER > 0x0421
			VERIFY(pBitmapMenu->GetMenuItemInfo(nIndex,&mii,TRUE));
#else
			VERIFY(::GetMenuItemInfo(pBitmapMenu->m_hMenu,nIndex,TRUE,&mii));
#endif
			COXItemInfo* pItemInfo=(COXItemInfo*)mii.dwItemData;
			ASSERT(pItemInfo!=NULL);
			sText=pItemInfo->GetText();
		}
		else
		{
			pMenu->GetMenuString(nIndex,sText,MF_BYPOSITION);
		}

		WriteItemDragDropData(lpData,sText,-1,pMenu->GetMenuItemID(nIndex),0);
		if(nIndex==nItemCount-1)
		{
			lpData-=sizeof(int);
			(*(int*)lpData)=2;
			lpData+=sizeof(int);
		}
		if(pMenu->GetMenuItemID(nIndex)==(UINT)-1)
		{
			WriteMenuDragDropData(lpData,pMenu->GetSubMenu(nIndex)->GetSafeHmenu());
		}
	}
}
