// ==========================================================================
// 					Class Specification : COXAutoComplete
// ==========================================================================
// Header file : OXAutoComplete.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
//
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXAutoComplete.h"
#include "OXRegistryValFile.h"
#include "UTB64Bit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4355)

//////////////////////////////////////////////////////////////////////
// COXAutoComplete
//////////////////////////////////////////////////////////////////////
COXAutoComplete* COXAutoComplete::m_pThis=NULL;

COXAutoComplete::COXAutoComplete(HWND hParentWnd) :
m_lstBox(this), m_hParent(hParentWnd)
{
	m_pThis=this;
	m_hkMsg=::SetWindowsHookEx(WH_CALLWNDPROC,CallWndProc,NULL,::GetCurrentThreadId());
	ASSERT(m_hkMsg);
	m_hkKbrd=::SetWindowsHookEx(WH_KEYBOARD,KeyboardProc,NULL,::GetCurrentThreadId());
	ASSERT(m_hkKbrd);
	m_hAttached=NULL;
	m_bUpdate=FALSE;


}

COXAutoComplete::~COXAutoComplete()
{
	Detach();
	::UnhookWindowsHookEx(m_hkMsg);
	::UnhookWindowsHookEx(m_hkKbrd);
	if (::IsWindow(m_lstBox.GetSafeHwnd()))
		m_lstBox.DestroyWindow();
}

BOOL COXAutoComplete::Attach(CWnd *pWnd, LPCTSTR lpszStorageName, DWORD dwOptions)
{
	return Attach(pWnd->GetSafeHwnd(),lpszStorageName, dwOptions);
}

BOOL COXAutoComplete::Attach(HWND hWnd, LPCTSTR lpszStorageName, DWORD dwOptions)
{
	ASSERT(hWnd);

	if (!m_hkMsg || !m_hkKbrd)
		return FALSE;

	if (::IsWindow(hWnd))
	{
		if (!m_hParent)
			m_hParent=::GetParent(hWnd);
		COXAutoStorage* pStorage;
		if (m_mpStorage.Lookup(hWnd,pStorage))
			Detach(hWnd);

		pStorage=NULL;

		//try to find COXAutoStorage
		POSITION pos=m_mpStorage.GetStartPosition();
		while (pos)
		{
			HWND hwnd;
			COXAutoStorage* pMappedStorage=NULL;
			m_mpStorage.GetNextAssoc(pos, hwnd, pMappedStorage);
			if (pMappedStorage && pMappedStorage->GetName()==lpszStorageName)
			{
				pStorage=pMappedStorage;
				break;
			}
		}

		if (pStorage)
		{
			m_mpStorage.SetAt(hWnd,pStorage);
			m_mpOptions.SetAt(hWnd,dwOptions);
			return TRUE;
		}
		pStorage=new COXAutoStorage(lpszStorageName);
		m_mpStorage.SetAt(hWnd,pStorage);
		m_mpOptions.SetAt(hWnd,dwOptions);
		return TRUE;
	}
	return FALSE;

}

LRESULT CALLBACK COXAutoComplete::CallWndProc(int nCode,
											  WPARAM wParam, 
											  LPARAM lParam )
{
#if defined (_WINDLL)
#if defined (_AFXDLL)
	AFX_MANAGE_STATE(AfxGetAppModuleState());
#else
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
#endif

	if (nCode<0)
	{
		return ::CallNextHookEx(m_pThis->m_hkMsg,nCode,wParam,lParam);
	}
	
	CWPSTRUCT* pSt=(CWPSTRUCT*) lParam;
	COXAutoStorage* pStorage=NULL;
	
	ASSERT(m_pThis);

	if (!m_pThis->m_mpStorage.Lookup(pSt->hwnd,pStorage))
	{
		return ::CallNextHookEx(m_pThis->m_hkMsg,nCode,wParam,lParam);
	}


	switch (pSt->message)
	{
		case WM_SETTEXT:
		case WM_PASTE:
		case WM_CUT:
			{
				if (!m_pThis->m_bUpdate)
				{
					CString sText;
					CWnd* pWnd=CWnd::FromHandle(pSt->hwnd);
					pWnd->GetWindowText(sText);
					m_pThis->OnContentsChange(pWnd->m_hWnd,sText);
				}
			}
		break;
		case WM_KILLFOCUS:
			m_pThis->Hide();
	}
	return ::CallNextHookEx(m_pThis->m_hkMsg,nCode,wParam,lParam);
}

void COXAutoComplete::Detach(CWnd *pWnd)
{
	Detach(pWnd->GetSafeHwnd());
}

void COXAutoComplete::Detach(HWND hWnd)
{
	if (hWnd)
	{
		COXAutoStorage* pStorage;
		if (m_mpStorage.Lookup(hWnd,pStorage))
		{
			m_mpStorage.RemoveKey(hWnd);
			m_mpOptions.RemoveKey(hWnd);
		}
		else
			return;
		//try to find out who else is using this storage,
		//if no one is using, delete it
		COXAutoStorage* pTest=NULL;
		POSITION pos=m_mpStorage.GetStartPosition();

		while (pos)
		{
			HWND hwnd;
			m_mpStorage.GetNextAssoc(pos,hwnd,pTest);
			if (pTest==pStorage)
				return;
		}
		delete pStorage;
		return;
	}

	//detach all storages
	POSITION pos=m_mpStorage.GetStartPosition();
	
	//cannot delete directly because the same storage can be mapped 
	//to different windows
	CMap<COXAutoStorage*,COXAutoStorage*,DWORD,DWORD> mpToDelete;

	while (pos)
	{
		HWND hwnd;
		COXAutoStorage* pStorage=NULL;
		m_mpStorage.GetNextAssoc(pos,hwnd,pStorage);
		mpToDelete.SetAt(pStorage,NULL);	
	}

	//delete all objects
	pos=mpToDelete.GetStartPosition();
	while (pos)
	{
		COXAutoStorage* pStorage;
		DWORD dwNULL;
		mpToDelete.GetNextAssoc(pos,pStorage,dwNULL);
		delete pStorage;

	}

	mpToDelete.RemoveAll();
	m_mpStorage.RemoveAll();
	m_mpOptions.RemoveAll();
}


LRESULT COXAutoComplete::KeyboardProc(int code,
				WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pThis);

	if (code<0)
		return ::CallNextHookEx(m_pThis->m_hkKbrd,code,wParam,lParam);
	else
	{
		CString sText;
		CWnd* pWnd=CWnd::GetFocus();
		COXAutoStorage* pStorage;
		_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
		if(pThreadState->m_hTrackingWindow==NULL && pWnd!=NULL && 
			m_pThis->m_mpStorage.Lookup(pWnd->m_hWnd,pStorage))
		{
			if (lParam<0)//key is pressed
			{
				switch (wParam)
				{
				case VK_NEXT:
				case VK_PRIOR:
				case VK_UP:
				case VK_DOWN:
					m_pThis->ChangeSel(PtrToInt(wParam));
					return TRUE;
					break;
				default:
					if (wParam>VK_HELP)
					{
						pWnd->GetWindowText(sText);
						m_pThis->OnContentsChange(pWnd->m_hWnd,sText);
					}

				}
			}
			else
			{
				switch (wParam)
				{
					case VK_NEXT:
					case VK_PRIOR:
					case VK_UP:
					case VK_DOWN:
						return TRUE;
				}
			}
		}
	}
	return ::CallNextHookEx(m_pThis->m_hkKbrd,code,wParam,lParam);
}
 

BOOL COXAutoComplete::OnContentsChange(HWND hwnd, CString sNewText)
{
	COXAutoStorage* pStorage;
	if (!m_mpStorage.Lookup(hwnd,pStorage))
		return FALSE;

	DWORD dwOptions;
	VERIFY(m_mpOptions.Lookup(hwnd,dwOptions));

	if (!dwOptions)
		return FALSE;

	CStringArray arsStrings;
	ASSERT(pStorage);
	UINT nCount=pStorage->GetMatchedStrings(sNewText, arsStrings);
	
	if (nCount)
	{
		CWnd* pWnd=CWnd::FromHandle(hwnd);
		if (dwOptions & OX_AUTOCOMPLETE_APPEND)
		{
			CString sText=arsStrings.GetAt(0);
			int nLength=pWnd->GetWindowTextLength();
			if (nLength<=sText.GetLength())
			{
				m_bUpdate=TRUE;
				pWnd->SetWindowText(sText);
				pWnd->SendMessage(EM_SETSEL,nLength,-1);
				m_bUpdate=FALSE;
			}

		}
		if (dwOptions & OX_AUTOCOMPLETE_LIST)
		{
			m_lstBox.m_bDraw=FALSE;
			if (hwnd!=m_hAttached && m_lstBox.GetSafeHwnd())
				m_lstBox.DestroyWindow();


			//calculate height of the listbox
			CDC* pDC=pWnd->GetDC();
			SIZE sz;
			sz.cx=sz.cy=0;
			int nHeight=0;
			int n=0;
			for (n=0;n<arsStrings.GetSize();n++)
			{
				CString sText=arsStrings.GetAt(n);
				VERIFY(::GetTextExtentPoint32(pDC->m_hDC,
					(LPCTSTR) sText, sText.GetLength(),&sz));
				if ((sz.cy+nHeight)<(OX_AUTOCOMPLETE_HEIGHTDEFAULTMAX-1))
					nHeight+=sz.cy;
				else
					break;
			}
			nHeight+=2;
			CRect rctWnd, rctWArea;
			pWnd->GetWindowRect(&rctWnd);

			//calculate vertical position of the listbox
			VERIFY(::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rctWArea,NULL));
			if (rctWArea.bottom-rctWnd.bottom<nHeight)
			{
				//on the top of the window
				rctWnd.bottom=rctWnd.top;
				rctWnd.top-=nHeight;
			}
			else
			{
				rctWnd.top=rctWnd.bottom;
				rctWnd.bottom+=nHeight;
			}

			m_lstBox.m_nHeight=sz.cy;
			m_lstBox.m_nWidth=rctWnd.Width();
			
			CWnd* pParent=CWnd::FromHandle(m_hParent);
			pParent->ScreenToClient(rctWnd);
			if (!::IsWindow(m_lstBox.GetSafeHwnd()))
			{
				VERIFY(m_lstBox.Create(WS_CHILD | WS_BORDER
				| LBS_OWNERDRAWFIXED | WS_VSCROLL | LBS_HASSTRINGS,
				rctWnd,pParent,
				OX_AUTOCOMPLETE_IDC_LIST));
				m_lstBox.ModifyStyleEx(0, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
				m_lstBox.SetFont(pWnd->GetFont());
				m_hAttached=hwnd;
			}
			else
			{
				CRect rctAct;
				m_lstBox.GetWindowRect(&rctAct);
				if (rctAct!=rctWnd)
					m_lstBox.MoveWindow(&rctWnd);
				m_lstBox.ResetContent();
			}
	
			for (n=0;n<arsStrings.GetSize();n++)
			{
				m_lstBox.AddString(arsStrings.GetAt(n));
			}
			if (!m_lstBox.IsWindowVisible())
				m_lstBox.ShowWindow(SW_SHOW);
			m_lstBox.m_bDraw=TRUE;
			m_lstBox.SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE);

		}

	}
	else
		if (::IsWindow(m_lstBox.GetSafeHwnd()))
			m_lstBox.ShowWindow(SW_HIDE);

	
	return TRUE;
}


void COXAutoComplete::Complete(HWND hWnd)
{
	COXAutoStorage* pStorage;
	if (!m_mpStorage.Lookup(hWnd,pStorage))
		return;
	
	ASSERT(pStorage);

	CWnd* pWnd=CWnd::FromHandlePermanent(hWnd);
	CString sText;
	pWnd->GetWindowText(sText);
	if (!sText.IsEmpty())
	{
		pStorage->AddString(sText);	
	}
	::SendMessage(m_hAttached,EM_SETSEL,(WPARAM)-1,(LPARAM)-1);
}

void COXAutoComplete::Hide()
{

	if (::IsWindow(m_lstBox.GetSafeHwnd()))
		m_lstBox.ShowWindow(SW_HIDE);
	
	::SendMessage(m_hAttached,EM_SETSEL,(WPARAM)-1,(LPARAM)-1);
}

void COXAutoComplete::ChangeSel(int nKey)
{
	if (::IsWindow(m_lstBox.GetSafeHwnd()))
	{
		int nSel=m_lstBox.GetCurSel();
		if (nSel==-1)
		{
			m_lstBox.SetCurSel(0);
		}
		else
		{

			CRect rct;
			m_lstBox.GetClientRect(rct);
			int nCount=rct.Height()/m_lstBox.GetItemHeight(nSel);
			switch (nKey)
			{
			case VK_UP:
				if (nSel>0)
					m_lstBox.SetCurSel(nSel-1);
				break;
			case VK_DOWN:
				if (nSel<(m_lstBox.GetCount()-1))
					m_lstBox.SetCurSel(nSel+1);
				break;
			case VK_NEXT:
				if ((nSel+nCount)<m_lstBox.GetCount())
					m_lstBox.SetCurSel(nSel+nCount-1);
				else
					m_lstBox.SetCurSel(m_lstBox.GetCount()-1);
				break;
			case VK_PRIOR:
				if ((nSel-nCount+1)>0)
					m_lstBox.SetCurSel(nSel-nCount+1);
				else
					m_lstBox.SetCurSel(0);

			}
		}
		int nNewSel=m_lstBox.GetCurSel();
		if (nNewSel!=nSel)
		{
			int nStartSel;
			::SendMessage(m_hAttached,EM_GETSEL, (WPARAM) &nStartSel,NULL);
			CString sText;
			m_lstBox.GetText(nNewSel,sText);
			m_bUpdate=TRUE;
			::SendMessage(m_hAttached,WM_SETTEXT,NULL, (LPARAM) (LPCTSTR) sText);
			m_bUpdate=FALSE;
			::SendMessage(m_hAttached,EM_SETSEL, nStartSel, -1); 
		}
	}
}


void COXAutoComplete::SetDepth(UINT nDepth, HWND hWnd)
{
	COXAutoStorage* pStorage=NULL;
	if (m_mpStorage.Lookup(hWnd,pStorage))
	{
		ASSERT(pStorage);
		pStorage->SetDepth(nDepth);
	}
}

int COXAutoComplete::GetDepth(HWND hWnd)
{
	int nRet=-1;

	COXAutoStorage* pStorage=NULL;
	if (m_mpStorage.Lookup(hWnd,pStorage))
	{
		ASSERT(pStorage);
		nRet=pStorage->GetDepth();
	}
	return nRet;
}

COXAutoStorage* COXAutoComplete::GetStorage(HWND hWnd)
{
	COXAutoStorage* pStorage=NULL;
	if (m_mpStorage.Lookup(hWnd,pStorage))
		return pStorage;
	else
		return NULL;
}


//////////////////////////////////////////////////////////////////////
// COXAutoStorage
//////////////////////////////////////////////////////////////////////

COXAutoStorage::COXAutoStorage(LPCTSTR lpszName,UINT nDepth) :
				m_nDepth(nDepth)
{
	if (lpszName && *lpszName)
		m_sName=lpszName;
	else
		m_sName=OX_AUTOCOMPLETE_NAMEDEFAULT;
	VERIFY(Load());
}

COXAutoStorage::~COXAutoStorage()
{
	Save();
}

UINT COXAutoStorage::GetMatchedStrings(CString sText, CStringArray& arsStrings)
{
	UINT nRslt=0;
	for (int n=0; n<m_arsContents.GetSize();n++)
	{
		if (m_arsContents.GetAt(n).Find(sText)==0)
		{
			nRslt++;
			arsStrings.Add(m_arsContents.GetAt(n));
		}
	}
	return nRslt;
}

BOOL COXAutoStorage::AddString(CString sText)
{
	if (sText.IsEmpty())
		return FALSE;
	ASSERT((UINT) m_arsContents.GetSize()<=m_nDepth);

	//try to find the same string in the storage
	for (int n=0;n<m_arsContents.GetSize();n++)
	{
		if (sText==m_arsContents.GetAt(n))
		{

			m_arsContents.RemoveAt(n);
			m_arsContents.InsertAt(0,sText);
			return TRUE;
		}
	}

	m_arsContents.InsertAt(0,sText);
	if ((UINT) m_arsContents.GetSize()>=m_nDepth)
		m_arsContents.RemoveAt(m_nDepth);

	return TRUE;
}

BOOL COXAutoStorage::Load()
{
	CString sApp=AfxGetAppName();
	sApp+=_T("\\Autocomplete");
	COXRegistryValFile reg;
	
	long lErr;

	if (!reg.Open(HKEY_CURRENT_USER,sApp,m_sName,lErr))
		return FALSE;
	
	DWORD dwLength=(DWORD)reg.GetLength();
	if (!dwLength)
		return TRUE;

	BYTE* pBuffer=new BYTE[dwLength+2];
	::ZeroMemory(pBuffer,dwLength+2);

	if (reg.Read(pBuffer,dwLength)!=dwLength)
	{
		delete []pBuffer;
		reg.Close();
		return FALSE;
	}

	reg.Close();

	DWORD dwVersion=*((DWORD*) pBuffer);
	if (dwVersion!=OX_AUTOCOMPLETE_VERSION)
	{
		delete []pBuffer;
		return FALSE;
	}
	TCHAR* pChar=(TCHAR*) (pBuffer+4);

	CString sText=pChar;
	delete []pBuffer;

	int nFind=sText.Find(_T("\r\n"));
	while (nFind!=-1)
	{
		CString sString=sText.Left(nFind);
		m_arsContents.Add(sString);
		sText=sText.Right(sText.GetLength()-nFind-2);
		nFind=sText.Find(_T("\r\n"));
	}
	while((UINT) m_arsContents.GetSize()>m_nDepth)
		m_arsContents.RemoveAt(m_arsContents.GetSize()-1);
	
	return TRUE;
}

BOOL COXAutoStorage::Save()
{
	CString sApp=AfxGetAppName();
	sApp+=_T("\\Autocomplete");
	COXRegistryValFile reg;
	
	long lErr;

	if (!reg.Open(HKEY_CURRENT_USER,sApp,m_sName,lErr))
		return FALSE;

	reg.SetLength(0);
	DWORD dwVersion=OX_AUTOCOMPLETE_VERSION;
	reg.Write(&dwVersion, 4);
	for (int i=0; i<m_arsContents.GetSize(); i++)
	{
		CString sString=m_arsContents.GetAt(i)+_T("\r\n");
		reg.Write(sString, sString.GetLength());
	}
	reg.Close();
	return TRUE;
}

void COXAutoStorage::SetDepth(UINT nDepth)
{
	m_nDepth=nDepth;
}

UINT COXAutoStorage::GetDepth()
{
	return m_nDepth;
}

void COXAutoComplete::SetParent(HWND hParentWnd)
{
	m_hParent=hParentWnd;
}
