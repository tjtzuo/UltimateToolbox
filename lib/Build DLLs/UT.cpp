// UT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static int __cdecl crtReportHookW(int nReportType, wchar_t* wszMsg, int* pnRet)
{
	UNREFERENCED_PARAMETER(pnRet);
	const wchar_t wszTrace[] = L"atlTraceGeneral - ";
	const int ccTrace = _countof(wszTrace) - 1;	// exclude L'\0'
	if (nReportType == _CRT_WARN)
	{
		wchar_t* pwsz = wcsstr(wszMsg, wszTrace);
		if (pwsz != nullptr)
		{
			size_t ccBuf = wcslen(pwsz) + 1;	// remaining buffer size (include L'\0')
			wmemmove_s(pwsz, ccBuf, &pwsz[ccTrace], ccBuf - ccTrace);
			OutputDebugStringW(pwsz);
			return TRUE;
		}
	}
	return FALSE;	// always keep processing
}
static int __cdecl crtReportHook(int nReportType, char* szMsg, int* pnRet)
{
	UNREFERENCED_PARAMETER(pnRet);
	const char szTrace[] = "atlTraceGeneral - ";
	const int ccTrace = _countof(szTrace) - 1;	// exclude '\0'
	if (nReportType == _CRT_WARN)
	{
		char* psz = strstr(szMsg, szTrace);
		if (psz != nullptr)
		{
			size_t ccBuf = strlen(psz) + 1;		// remaining buffer size (include '\0')
			memmove_s(psz, ccBuf, &psz[ccTrace], ccBuf - ccTrace);
			OutputDebugStringA(psz);
			return TRUE;
		}
	}
	return FALSE;	// always keep processing
}
#endif


static AFX_EXTENSION_MODULE UTDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
#ifdef _DEBUG
		_CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, crtReportHookW);
		_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, crtReportHook);
#endif

		TRACE0("UT.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(UTDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(UTDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("UT.DLL Terminating!\n");

#ifdef _DEBUG
	_CrtSetReportHookW2(_CRT_RPTHOOK_REMOVE, crtReportHookW);
	_CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, crtReportHook);
#endif

		// Terminate the library before destructors are called
		AfxTermExtensionModule(UTDLL);
	}
	return 1;   // ok
}
