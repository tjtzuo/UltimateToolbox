// =============================================================================
// 							Class Implementation : COXWebCtrl
// =============================================================================
//
// Version: 9.3
// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OXWebCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COXWebCtrl

IMPLEMENT_DYNCREATE(COXWebCtrl, CWnd)

/////////////////////////////////////////////////////////////////////////////
// COXWebCtrl properties

/////////////////////////////////////////////////////////////////////////////
// COXWebCtrl operations

void COXWebCtrl::GoBack()
	{
	InvokeHelper(0x64, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

void COXWebCtrl::GoForward()
	{
	InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

void COXWebCtrl::GoHome()
	{
	InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

void COXWebCtrl::GoSearch()
	{
	InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

void COXWebCtrl::Navigate(LPCTSTR pcszURL, long lFlags, LPCTSTR pcszTargetFrameName, 
						  VARIANT* PostData, VARIANT* Headers)
	{
	static BYTE parms[] = VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	VARIANT Flags, TargetFrameName;
	Flags.vt = VT_I4;
	Flags.lVal = lFlags;

	if (pcszTargetFrameName != NULL)
		{
		CString str(pcszTargetFrameName);
		TargetFrameName.vt = VT_BSTR;
		TargetFrameName.bstrVal = str.AllocSysString();
		}
	else
		{
		TargetFrameName.vt = VT_EMPTY;
		}

	InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pcszURL, &Flags, &TargetFrameName, PostData, Headers);

	if (TargetFrameName.vt != VT_EMPTY)
		SysFreeString(TargetFrameName.bstrVal);
	}

void COXWebCtrl::Refresh()
	{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

void COXWebCtrl::Refresh2(long lLevel)
	{
	static BYTE parms[] = VTS_PVARIANT;
	VARIANT Level;
	Level.vt = VT_I4;
	Level.lVal = lLevel;
	InvokeHelper(0x69, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Level);
	}

void COXWebCtrl::Stop()
	{
	InvokeHelper(0x6a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

LPDISPATCH COXWebCtrl::GetApplication()
	{
	LPDISPATCH result;
	InvokeHelper(0xc8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
	}

LPDISPATCH COXWebCtrl::GetParent()
	{
	LPDISPATCH result;
	InvokeHelper(0xc9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
	}

LPDISPATCH COXWebCtrl::GetContainer()
	{
	LPDISPATCH result;
	InvokeHelper(0xca, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
	}

LPDISPATCH COXWebCtrl::GetDocument()
	{
	LPDISPATCH result;
	InvokeHelper(0xcb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
	return result;
	}

BOOL COXWebCtrl::GetTopLevelContainer()
	{
	BOOL result;
	InvokeHelper(0xcc, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
	}

CString COXWebCtrl::GetType()
	{
	CString result;
	InvokeHelper(0xcd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
	}

long COXWebCtrl::GetLeft()
	{
	long result;
	InvokeHelper(0xce, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
	}

void COXWebCtrl::SetLeft(long nNewValue)
	{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xce, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
	}

long COXWebCtrl::GetTop()
	{
	long result;
	InvokeHelper(0xcf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
	}

void COXWebCtrl::SetTop(long nNewValue)
	{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xcf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
	}

long COXWebCtrl::GetWidth()
	{
	long result;
	InvokeHelper(0xd0, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
	}

void COXWebCtrl::SetWidth(long nNewValue)
	{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
	}

long COXWebCtrl::GetHeight()
	{
	long result;
	InvokeHelper(0xd1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
	}

void COXWebCtrl::SetHeight(long nNewValue)
	{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
	}

CString COXWebCtrl::GetLocationName()
	{
	CString result;
	InvokeHelper(0xd2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
	}

CString COXWebCtrl::GetLocationURL()
	{
	CString result;
	InvokeHelper(0xd3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
	}

BOOL COXWebCtrl::GetBusy()
	{
	BOOL result;
	InvokeHelper(0xd4, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
	}

void COXWebCtrl::Print(DWORD dwOptions)
	{
	IOleCommandTarget* pOLECmdTarget=NULL;
	LPDISPATCH pDispatch = GetDocument();
	if(pDispatch!=NULL)
		{
		HRESULT hr = pDispatch->QueryInterface(IID_IOleCommandTarget,
		   (void**)&pOLECmdTarget);
		ASSERT(hr==S_OK && pOLECmdTarget!=NULL);
		pOLECmdTarget->Exec(NULL, OLECMDID_PRINT, dwOptions, NULL, NULL);
		pOLECmdTarget->Release();
		pDispatch->Release();
		}
	}
