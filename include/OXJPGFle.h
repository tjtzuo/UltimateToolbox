// ==========================================================================
// 							Class Specification : COXJPEGFile
// ==========================================================================

// Header file : OXJPGFle.h

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
//	This class encapsulates the reading and writing of JPG files

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __JPEGFILE_H__
#define __JPEGFILE_H__

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

class OX_CLASS_DECL COXJPEGFile : public CObject
{
friend class COXJPEGCompressor;
friend class COXJPEGDecompressor;

// Data Members
public:

protected:
	CFile*		m_pJPEGFile;
	CString		m_sFullPath;

	BOOL		m_bTotalReset;

private:

// Member Functions
public:
	COXJPEGFile();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object

	COXJPEGFile(CString sFullPath);  
	// --- In  : sFullPath : the full path subdirs + name + extender of the JPEG file
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				File will not be opened yet

	COXJPEGFile(CFile* pJPEGFile);  
	// --- In  : pJPEGFile : a pointer to a CFile(JPEG).  must already be open
	//							Must have the right access mode
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				Could be used to pass a CMemFile as parameter

	void SetTotalReset(BOOL bTotal);
	// --- In  : bTotal : if TRUE, the jpgfile object will reset all its internal members
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

	CString GetFullFilePath();
	// --- In  : 
	// --- Out : 
	// --- Returns : Gets the full path. 
	// --- Effect :

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXJPEGFile();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
	// Compression to JPG 
	static void init_destination (j_compress_ptr cinfo);
	static jpeg_boolean empty_output_buffer (j_compress_ptr cinfo);
	static void term_destination(j_compress_ptr cinfo);
	void InitJPGDestination(j_compress_ptr cinfo, CFile* pOut);

	virtual BOOL InitWrite(j_compress_ptr cinfo, BOOL bExtra = FALSE);
	virtual BOOL StartOutput(j_compress_ptr cinfo);
	virtual BOOL FinishOutput(j_compress_ptr cinfo);
	virtual BOOL PutPixelRows(j_compress_ptr cinfo, JSAMPARRAY Buf, JDIMENSION rowsSupplied);

	// Decompression from JPG 
	static void init_source (j_decompress_ptr cinfo);
	static jpeg_boolean fill_input_buffer (j_decompress_ptr cinfo);
	static void skip_input_data(j_decompress_ptr cinfo, long num_bytes);
	static void term_source(j_decompress_ptr cinfo);
	void InitJPGSource(j_decompress_ptr cinfo, CFile* pInfile);

	virtual BOOL InitRead(j_decompress_ptr cinfo);
	virtual BOOL StartInput(j_decompress_ptr cinfo);
	virtual BOOL FinishInput(j_decompress_ptr cinfo);
	virtual JDIMENSION GetPixelRows(j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);

	// Translation from one JPG flavor to another
	virtual jvirt_barray_ptr* ReadCoefficients(j_decompress_ptr srcinfo);
	virtual void CopyCriticalParams(j_decompress_ptr srcinfo, j_compress_ptr dstinfo);
	virtual void WriteCoefficients(j_compress_ptr dstinfo, jvirt_barray_ptr* coef_arrays);

	// General helper methods
	size_t ReadData(void* pBuffer, size_t length);
	size_t WriteData(const void* pBuffer, size_t length);

	// can throw file exceptions
	virtual void OpenFile(UINT nOpenFlags);
	virtual void AbortFile();
	virtual void CloseFile();

private:

};

#endif //__JPEGFILE_H__
