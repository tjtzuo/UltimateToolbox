// ==========================================================================
// 					Class Implementation : COXJPEGDecompressor
// ==========================================================================

// Source file : OXJPGDom.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXJPGDom.h"

#include "OXJPGFle.h"
#include "OXGphFle.h"

#include "OXJPGExp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#define SIZE_ERROR_BUF	255

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:


//	CFile*		m_pGraphFile;
//	---			

	// private:

// Member functions ---------------------------------------------------------
// public:

COXJPEGDecompressor::COXJPEGDecompressor()
	:m_nColors(0),
	m_bGrayScale(FALSE),
	m_bFast(FALSE),
	m_eDecompScale(DS_OneOne),
	m_eDisCosTransf(DC_Undefined),
	m_eDitherMethod(DM_Undefined),
	m_bNoSmooth(FALSE),
	m_bOnePass(FALSE),
	m_nMaxMem(0)
	{
	}

#ifdef _DEBUG
void COXJPEGDecompressor::Dump(CDumpContext& dc) const
	{
	COXJPEGCodec::Dump(dc);

	dc << TEXT("\nm_nColors : ") << 		m_nColors;
	dc << TEXT("\nm_bGrayScale : ") << 	(WORD)m_bGrayScale;
	dc << TEXT("\nm_bFast : ") <<	(WORD)m_bFast;
	dc << TEXT("\nm_bNoSmooth ") << 	(WORD)m_bNoSmooth;
	dc << TEXT("\nm_bOnePass ") << 	(WORD)m_bOnePass;
	dc << TEXT("\nm_eDisCosTransf ") << 	(int)m_eDisCosTransf;
	dc << TEXT("\nm_eDecompScale ") << 	(int)m_eDecompScale;
	dc << TEXT("\nm_eDitherMethod ") << 	(int)m_eDitherMethod;
	dc << TEXT("\nm_nMaxMem ") << 	m_nMaxMem;
	}

void COXJPEGDecompressor::AssertValid() const
	{
	COXJPEGCodec::AssertValid();
	}
#endif

COXJPEGDecompressor::~COXJPEGDecompressor()
	{
	}

void COXJPEGDecompressor::SetColors(UINT nColors)
	{
	ASSERT(1 <= nColors && nColors <= 256);

	m_nColors = nColors;
	}

void COXJPEGDecompressor::SetScale(EDecompScale eDecompScale)
	{
	ASSERT(DS_FIRST <= eDecompScale && eDecompScale <= DS_LAST);

	m_eDecompScale = eDecompScale;
	}

void COXJPEGDecompressor::SetDisCosTranf(EDiscreteCosTransf eDisCosTransf)
	{
	ASSERT(DC_FIRST <= eDisCosTransf && eDisCosTransf <= DC_LAST);

	m_eDisCosTransf = eDisCosTransf;
	}

void COXJPEGDecompressor::SetDitherMethod(EDitherMethod eDitherMethod)
	{
	ASSERT(DM_FIRST <= eDitherMethod && eDitherMethod <= DM_LAST);

	m_eDitherMethod = eDitherMethod;
	}


short COXJPEGDecompressor::DoDecompress(COXJPEGFile* pJPEGFile, COXGraphicFile* pGraphicsFile)
	{
	ASSERT(pJPEGFile != NULL && pGraphicsFile != NULL);

	TCHAR ErrorBuffer[SIZE_ERROR_BUF];
	TRY
		{
		pJPEGFile->OpenFile(CFile::modeRead | CFile::shareDenyWrite);

		pGraphicsFile->OpenFile(CFile::modeCreate | CFile::modeWrite);
		}
	CATCH(CFileException, e)
		{
		pGraphicsFile->AbortFile();
		pJPEGFile->AbortFile();
		
		e->GetErrorMessage(ErrorBuffer, SIZE_ERROR_BUF);
		THROW(new COXJPEGException(e->m_cause, (LPCTSTR)ErrorBuffer));
		}
	END_CATCH

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JDIMENSION num_scanlines;

	TRY
		{
		// Mapping needed to call virtual functions of right codec
		m_RunningCodecsMap.SetAt(&cinfo, this);

		/* Initialize the JPEG compression object with default error handling. */
		SetJPEGErrorHandling(&jerr);
		cinfo.err = &jerr;

		jpeg_create_decompress(&cinfo);

		// Configure the JPG Decompression Manager
		ProcessSwitches(&cinfo, FALSE);

		/* Specify data source for decompression */
		pJPEGFile->InitRead(&cinfo);
	
		// Re-Configure the JPG Decompression Manager
		ProcessSwitches(&cinfo, FALSE);

		/* Initialize the output module now to let it override any crucial
		 * option settings (for instance, GIF wants to force color quantization).
		 */
		pGraphicsFile->InitWrite(&cinfo, TRUE);

		/* Start decompressor */
		pJPEGFile->StartInput(&cinfo);

		/* Write output file header */
		pGraphicsFile->StartOutput(&cinfo);

		/* Process data */
		while (cinfo.output_scanline < cinfo.output_height)
			{
			num_scanlines = pJPEGFile->GetPixelRows(&cinfo, pGraphicsFile->m_buffer, pGraphicsFile->m_buffer_height);
			pGraphicsFile->PutPixelRows(&cinfo, num_scanlines);
			}

		/* Finish decompression and release memory.
		 * I must do it in this order because output module has allocated memory
		 * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
		 */
		pGraphicsFile->FinishOutput(&cinfo);
		pJPEGFile->FinishInput(&cinfo);

		jpeg_destroy_decompress(&cinfo);
		}
	CATCH(COXJPEGException, e)
		{
		m_RunningCodecsMap.RemoveKey(&cinfo);
	
		THROW_LAST();
		}
	END_CATCH

	m_RunningCodecsMap.RemoveKey(&cinfo);

	TRY
		{
		/* Close files, if we opened them */
		pGraphicsFile->CloseFile();

		pJPEGFile->CloseFile();
		}
	CATCH(CFileException, e)
		{
		pGraphicsFile->AbortFile();
		pJPEGFile->AbortFile();
		
		e->GetErrorMessage(ErrorBuffer, SIZE_ERROR_BUF);
		THROW(new COXJPEGException(e->m_cause, (LPCTSTR)ErrorBuffer));
		}
	END_CATCH

	CString sWarnings = GetWarningMessages();
	if (!sWarnings.IsEmpty())
		return 2;

	return 0;
	}


// Protected
void COXJPEGDecompressor::ProcessSwitches(j_decompress_ptr cinfo, BOOL bForReal)
	{
	UNREFERENCED_PARAMETER(bForReal);
	
	cinfo->err->trace_level = 0;

	if (m_nColors != 0)
		{
		cinfo->desired_number_of_colors = m_nColors;
		cinfo->quantize_colors = TRUE;
		}

	// Select DCT algoritm
	switch(m_eDisCosTransf)
		{
		case DC_Int:
			cinfo->dct_method = JDCT_ISLOW;
			break;
		case DC_FastInt:	
			cinfo->dct_method = JDCT_IFAST;
			break;
		case DC_Float:
			cinfo->dct_method = JDCT_FLOAT;
			break;
		default:
			// do nothing
			break;
		}

	// Select DCT algoritm
	switch(m_eDitherMethod)
		{
		case DM_FloydStein:
			cinfo->dither_mode = JDITHER_FS;
			break;
		case DM_Ordered:	
			cinfo->dither_mode = JDITHER_ORDERED;
			break;
		case DM_None:
			cinfo->dither_mode = JDITHER_NONE;
			break;
		default:
			// do nothing
			break;
		}

	if (m_bFast)
		{
		/* Select recommended processing options for quick-and-dirty output. */
		cinfo->two_pass_quantize = FALSE;
		cinfo->dither_mode = JDITHER_ORDERED;
		if (!cinfo->quantize_colors) /* don't override an earlier -colors */
			cinfo->desired_number_of_colors = 216;

		cinfo->dct_method = JDCT_FASTEST;
		cinfo->do_fancy_upsampling = FALSE;
		}

	if (m_bGrayScale)
		/* Force monochrome output. */
		cinfo->out_color_space = JCS_GRAYSCALE;

	if (m_nMaxMem != 0)
		// Maximum memory in Kb
		cinfo->mem->max_memory_to_use = m_nMaxMem * 1000L;

	if (m_bNoSmooth)
		/* Suppress fancy upsampling */
		cinfo->do_fancy_upsampling = FALSE;

    if (m_bOnePass)
		cinfo->two_pass_quantize = FALSE;

	switch(m_eDecompScale)
		{
		case DS_OneOne:
			cinfo->scale_num = 1;
			cinfo->scale_denom = 1;
			break;
		case DS_OneHalf:	
			cinfo->scale_num = 1;
			cinfo->scale_denom = 2;
			break;
		case DS_OneFourth:
			cinfo->scale_num = 1;
			cinfo->scale_denom = 4;
			break;
		case DS_OneEight:
			cinfo->scale_num = 1;
			cinfo->scale_denom = 8;
			break;
		default:
			// do nothing
			break;
		}
	}


///////////////////////////////////////////////////////////////////////////
