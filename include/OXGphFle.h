// ==========================================================================
// 							Class Specification : COXGraphicFile
// ==========================================================================

// Header file : OXGphFle.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	YES	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class encapsulates the base functionality for graphic files.
//	It contains a few virtual functions, the CFile pointer, ...

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __GRAPHICFILE_H__
#define __GRAPHICFILE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


extern "C"
	{
	#define JPEG_CJPEG_DJPEG		/* define proper options in jconfig.h */
	#define JPEG_INTERNAL_OPTIONS	/* cjpeg.c,djpeg.c need to see xxx_SUPPORTED */
	#include "jinclude.h"
	#include "jpeglib.h"
	#include "jerror.h"				/* get library error codes too */
	#include "cderror.h"			/* get application-specific error codes */
	}

#define	ReadOK(buffer, len)	(ReadData(buffer, len) == ((size_t) (len)))

class OX_CLASS_DECL COXGraphicFile : public CObject
{
friend class COXJPEGCompressor;
friend class COXJPEGDecompressor;

// Data Members
public:

protected:
	CFile*		m_pGraphFile;
	CString		m_sFullPath;

	JSAMPARRAY	m_buffer;
	JDIMENSION	m_buffer_height;

	BOOL		m_bTotalReset;
private:

// Member Functions
public:
	COXGraphicFile();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	
	COXGraphicFile(CString sFullPath);  
	// --- In  : sFullPath : the full path subdirs + name + extender of the graphic file
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				File will not be opened yet

	COXGraphicFile(CFile* pGraphicFile);  
	// --- In  : pGraphicFile : a pointer to a CFile.  must already be open
	//							Must have the right access mode
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				Could be used to pass a CMemFile as parameter

	CString GetFullFilePath();
	// --- In  : 
	// --- Out : 
	// --- Returns : Gets the full path. 
	// --- Effect :

	void SetTotalReset(BOOL bTotal);
	// --- In  : bTotal : if TRUE, the graphics file object will reset all its internal members
	//					  like the file pointer.
	// --- Out : 
	// --- Returns :
	// --- Effect : This functionalty is added to prevent or enforce the object to reset itself
	//			    completely.  When the object is used with an opened CFile*, sometimes
	//				it is not desired that the object clears this CFile* to NULL. E.g. This is 
	//				the case when this CFile* is a CMemFile* which contents vary but not its
	//				pointer.  Normally this object was intended to be constructed each time
	//				you need it and then good programming practice dictates that it clears
	//				all internals before destruction.

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXGraphicFile();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
	virtual BOOL StartInput(j_compress_ptr cinfo) = 0;
	virtual BOOL FinishInput(j_compress_ptr cinfo) = 0;
	virtual JDIMENSION GetPixelRows(j_compress_ptr cinfo) = 0;

	virtual BOOL StartOutput(j_decompress_ptr cinfo) = 0;
	virtual BOOL FinishOutput(j_decompress_ptr cinfo) = 0;
	virtual BOOL PutPixelRows(j_decompress_ptr cinfo, JDIMENSION rowsSupplied) = 0;

	virtual BOOL InitRead(j_compress_ptr cinfo) = 0;
	virtual BOOL InitWrite(j_decompress_ptr cinfo, BOOL bExtra = FALSE) = 0;

	size_t ReadData(void* pBuffer, size_t length);
	BOOL WriteData(const void* pBuffer, size_t length);

	// can throw file exceptions
	virtual void OpenFile(UINT nOpenFlags);
	virtual void AbortFile();
	virtual void CloseFile();

private:

};

#endif //__GRAPHICFILE_H__
