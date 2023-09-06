// stdafx.cpp : source file that includes just the standard includes
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// v9.3 - update 04 fixes for unicode builds in automatically links correct comsuppw library in VC2005 and later - AAW 2009-03-29
#if (_MSC_VER >= 1400)
#ifdef _DEBUG
#pragma comment(lib, "comsuppwd.lib")
#else
#pragma comment(lib, "comsuppw.lib")
#endif

#endif
