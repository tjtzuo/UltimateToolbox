// v9.3 update 03 - 64-bit
#if defined(_WIN64)
// OnToolHitTest handlers return INT_PTR - code in 32 bit gets away with 
// UINT, which allows VC6 compilation as well. In 64 bit INT_PTR needed,
// so we'll use this define - TD
#define OXINTRET INT_PTR

// OnTimer calls take a UINT_PTR parameter - code in 32 bit gets away with 
// UINT, again good for VC6. Using UINT_PTR for 64 bit through this define - TD
#define OXTPARAM UINT_PTR

// For lpdwResult param of SendMessageTimeout
#define OXPDWORD PDWORD_PTR

// GetWindowLong defines - the old ones are #undef'd in WinUser.h for 64 bit,
// so we should be free to reinstate them here - TD
#define GWL_WNDPROC		GWLP_WNDPROC
#define GWL_HINSTANCE	GWLP_HINSTANCE
#define GWL_HWNDPARENT	GWLP_HWNDPARENT
#define GWL_USERDATA	GWLP_USERDATA

// GetClassLongPtr defines - again, the old ones are #undef'd in WinUser.h for 
// 64 bit, so we should be free to reinstate them here - TD
#define GCL_MENUNAME       GCLP_MENUNAME
#define GCL_HBRBACKGROUND  GCLP_HBRBACKGROUND
#define GCL_HCURSOR        GCLP_HCURSOR
#define GCL_HICON          GCLP_HICON
#define GCL_HMODULE        GCLP_HMODULE
#define GCL_WNDPROC        GCLP_WNDPROC
#define GCL_HICONSM        GCLP_HICONSM

// there are many of these calls in the code to which these macros merely add a 
// C style cast - testing may reveal some idiosyncracies - TD
#define PtrToInt(x) PtrToInt((VOID*)(x))	// 64 bit version expects void*, not INT_PTR
#define PtrToLong(x) PtrToLong((VOID*)(x))	// 64 bit version expects void*, not INT_PTR
#define PtrToUint(x) PtrToUint((VOID*)(x))	// 64 bit version expects void*, not INT_PTR
#define PtrToUlong(x) PtrToUlong((VOID*)(x))	// 64 bit version expects void*, not INT_PTR

#else
#define OXINTRET int
#define OXTPARAM UINT
#define OXPDWORD LPDWORD
#endif

#if _MSC_VER < 1400

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GetClassLongPtr
#define GetClassLongPtr GetClassLong
#endif

#ifndef SetClassLongPtr
#define SetClassLongPtr SetClassLong
#endif

#ifndef ULongToPtr
#define ULongToPtr(x) x
#endif

#ifndef LongToPtr
#define LongToPtr(x) x
#endif

#ifndef PtrToUint
#define PtrToUint(x) x
#endif

#ifndef PtrToInt
#define PtrToInt(x) x
#endif

typedef DWORD DWORD_PTR;
typedef LONG LONG_PTR;
typedef ULONG ULONG_PTR;

// Frustratingly, INT_PTR is an int in VC2005, and a long in VC6, so we can't define it here, the code needs to just handle it elsewhere

#endif