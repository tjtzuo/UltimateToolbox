// ==========================================================================
// 							Class Specification : COXJPEGDecompressor
// ==========================================================================

// Header file : OXJPGDom.h

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
//	YES	Derived from COXJPEGCodec

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class encapsulates the parameters of decompression of JPEG images and a decompress
//  method

// Remark:
//	

// Prerequisites (necessary conditions):
//	

/////////////////////////////////////////////////////////////////////////////
#ifndef __JPEGDECOMP_H__
#define __JPEGDECOMP_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "oxjpgcod.h"


	enum EDitherMethod
		{ 
		DM_Undefined 			=   0,
		DM_FloydStein			=   1,
		DM_Ordered				= 	2,
		DM_None					=	3
		};

	const EDitherMethod DM_FIRST = DM_FloydStein;	
	const EDitherMethod DM_LAST =  DM_None;	

	enum EDecompScale
		{ 
		DS_Undefined 			=   0,
		DS_OneOne				=   1,
		DS_OneHalf				= 	2,
		DS_OneFourth			=	3,
		DS_OneEight				=	4
		};

	const EDecompScale DS_FIRST = DS_OneOne;	
	const EDecompScale DS_LAST =  DS_OneEight;	

class COXGraphicFile;
class COXJPEGFile;
	
class OX_CLASS_DECL COXJPEGDecompressor : public COXJPEGCodec
{
// Data Members
public:

protected:
	// Common switches
	UINT				m_nColors;
	BOOL				m_bGrayScale;
	BOOL				m_bFast;
	EDecompScale		m_eDecompScale;

	// Advanced switches
	EDiscreteCosTransf	m_eDisCosTransf;
	EDitherMethod		m_eDitherMethod;
	BOOL				m_bNoSmooth;
	BOOL				m_bOnePass;
	UINT				m_nMaxMem;

private:

// Member Functions
public:
	COXJPEGDecompressor();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object

	// Common switches
	void SetColors(UINT nColors);

	UINT GetColors()
		{ return m_nColors;}

	void SetGrayScale(BOOL bGrayScale)
		{ m_bGrayScale = bGrayScale;}

	BOOL GetGrayScale()
		{ return m_bGrayScale;}

	void SetFast(BOOL bFast)
		{ m_bFast = bFast;}

	BOOL GetFast()
		{ return m_bFast;}

	void SetScale(EDecompScale eDecompScale);
	
	EDecompScale GetScale()
		{ return m_eDecompScale;}

	// Advanced switches
	void SetDisCosTranf(EDiscreteCosTransf eDisCosTransf);

	EDiscreteCosTransf GetDisCosTranf()
		{ return m_eDisCosTransf;}

	void SetDitherMethod(EDitherMethod eDitherMethod);

	EDitherMethod GetDitherMethod()
		{ return m_eDitherMethod;}

	void SetNoSmooth(BOOL bNoSmooth)
		{ m_bNoSmooth = bNoSmooth;}

	UINT GetNoSmooth()
		{ return m_bNoSmooth;}

	void SetOnePass(BOOL bOnePass)
		{ m_bOnePass = bOnePass;}

	BOOL GetOnePass()
		{ return m_bOnePass;}

	void SetMaxMemory(UINT nMaxMem)
		{ m_nMaxMem = nMaxMem;}

	UINT GetMaxMemory()
		{ return m_nMaxMem;}

	virtual short DoDecompress(COXJPEGFile* pJPEGFile, COXGraphicFile* pGraphicsFile);
	// --- In  : pJPEGFile : pointer to a JPEG File
	//			 pGraphicsFile : pointer to a BMP, GIF, TARGA or RLE type of graphics file
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : decompresses the JPEG file to the specified graphics file.

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXJPEGDecompressor();  
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
	virtual void ProcessSwitches(j_decompress_ptr cinfo, BOOL bForReal);

private:

};

#endif //__JPEGDECOMP_H__
