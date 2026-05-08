// ==========================================================================
// 							Class Specification : COXJPEGCodec
// ==========================================================================

// Header file : OXJPGCod.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is the base class for compressionn decompression and translation of
//  JPEG images.  It some global error handling and progression callbacks.

//  method

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __JPEGCODEC_H__
#define __JPEGCODEC_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


extern "C"
	{
	#define JPEG_CJPEG_DJPEG		/* define proper options in jconfig.h */
	#define JPEG_INTERNAL_OPTIONS	/* cjpeg.c,djpeg.c need to see xxx_SUPPORTED */
	#include "jversion.h"
	#include "jinclude.h"
	#include "jpeglib.h"
	#include "jerror.h"				/* get library error codes too */
	#include "cderror.h"			/* get application-specific error codes */
	}

	enum EDiscreteCosTransf
		{ 
		DC_Undefined 			=   0,
		DC_Int					=   1,
		DC_FastInt				= 	2,
		DC_Float				=	3
		};

	const EDiscreteCosTransf DC_FIRST = DC_Int;	
	const EDiscreteCosTransf DC_LAST =  DC_Float;	


class OX_CLASS_DECL COXJPEGCodec : public CObject
{
// Data Members
public:

protected:
	static CMapPtrToPtr m_RunningCodecsMap;

	CString				m_sWarnings;
private:

// Member Functions
public:
	COXJPEGCodec();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object

	virtual ~COXJPEGCodec();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	CString GetWarningMessages()
		{ return m_sWarnings; }
	// --- In  : 
	// --- Out : 
	// --- Returns : a concatenated string (separator = \n) of warnings
	// --- Effect : 

protected:
	static void error_exit(j_common_ptr cinfo);
	static void output_message(j_common_ptr cinfo);
	static void emit_message(j_common_ptr cinfo, int msg_level);
	static void format_message(j_common_ptr cinfo, TCHAR* buffer);
	static void reset_error_mgr(j_common_ptr cinfo);

	BOOL SetJPEGErrorHandling(struct jpeg_error_mgr* err);

	virtual void ProcessError(j_common_ptr cinfo);
	virtual void OutputMessage(j_common_ptr cinfo);
	virtual void EmitMessage(j_common_ptr cinfo, int msg_level);
	virtual void FormatMessage(j_common_ptr cinfo, LPTSTR buffer);
	virtual void ResetError(j_common_ptr cinfo);

private:

};

#endif //__JPEGCODEC_H__
