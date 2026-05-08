// ==========================================================================
// 					Class Implementation : COXGraphicFile
// ==========================================================================

// Source file : OXGphFle.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXGphFle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:


//	CFile*		m_pGraphFile;
//	---			

//	CString		m_sFullPath;
//	---			

//	JSAMPARRAY	m_buffer;
//	---			

//	JDIMENSION	m_buffer_height;
//	---			

//	BOOL		m_bTotalReset;
//	---

	// private:

// Member functions ---------------------------------------------------------
// public:

COXGraphicFile::COXGraphicFile()
	: m_pGraphFile(NULL),
	m_bTotalReset(TRUE),
	m_buffer(NULL),
	m_buffer_height(0)
	{
	}

COXGraphicFile::COXGraphicFile(CString sFullPath)
	: m_sFullPath(sFullPath),
	m_pGraphFile(NULL),
	m_bTotalReset(TRUE),
	m_buffer(NULL),
	m_buffer_height(0)
	{
	}

COXGraphicFile::COXGraphicFile(CFile* pGraphicFile)
	: m_pGraphFile(pGraphicFile),
	m_bTotalReset(TRUE),
	m_buffer(NULL),
	m_buffer_height(0)
	{
	ASSERT(m_pGraphFile != NULL);
	}

#ifdef _DEBUG
void COXGraphicFile::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);

	dc << TEXT("\nm_buffer_height : ") << 		m_buffer_height;
	dc << TEXT("\nm_buffer ") << 	(void*)m_buffer;
	dc << TEXT("\nm_pGraphFile ") << 	(void*)m_pGraphFile;
	dc << TEXT("\nm_sFullPath : ") << m_sFullPath;

	}

void COXGraphicFile::AssertValid() const
	{
	CObject::AssertValid();
	}
#endif

COXGraphicFile::~COXGraphicFile()
	{
	CloseFile();
	}

CString COXGraphicFile::GetFullFilePath()
	{
	return m_sFullPath;
	}

void COXGraphicFile::SetTotalReset(BOOL bTotal)
	{
	m_bTotalReset = bTotal;
	}

// Protected
void COXGraphicFile::OpenFile(UINT nOpenFlags)
	{
	if (m_pGraphFile != NULL)
		return;	// Already open
	
	if (m_sFullPath.IsEmpty())
		THROW(new CFileException(CFileException::badPath));	// No path specified
	
	m_pGraphFile = new CStdioFile(m_sFullPath, (nOpenFlags | CFile::typeBinary) & ~CFile::typeText);
	}

void COXGraphicFile::AbortFile()
	{
	if (m_pGraphFile == NULL)
		return;	// there is no file pointer
	ASSERT_VALID(m_pGraphFile);
	
	if (m_sFullPath.IsEmpty())
		{
		TRACE0("In COXGraphicFile::AbortFile() : No path specified. Graphics file pointer no constructed by class");
		if (m_bTotalReset)
			m_pGraphFile = NULL;
		}
	else
		{
		m_pGraphFile->Abort();
		delete m_pGraphFile;
		m_pGraphFile = NULL;
		}

	}

void COXGraphicFile::CloseFile()
	{
	if (m_pGraphFile == NULL)
		return;	// Already closed
	ASSERT_VALID(m_pGraphFile);
	
	if (m_sFullPath.IsEmpty())
		{
		TRACE0("In COXGraphicFile::CloseFile() : No path specified. Graphics file pointer no constructed by class");
		if (m_bTotalReset)
			m_pGraphFile = NULL;
		}
	else
		{
		m_pGraphFile->Close();
		delete m_pGraphFile;
		m_pGraphFile = NULL;
		}

	}

// protected

size_t COXGraphicFile::ReadData(void* pBuffer, size_t length)
	{
	ASSERT(m_pGraphFile != NULL);
	
	size_t nNumRead(0); 

	TRY
		{
		nNumRead =  m_pGraphFile->Read(pBuffer, (UINT)length);
		}
	CATCH(CFileException, e)
		{
		TRACE(_T("COXGraphicFile::ReadData : Catching CFileException\n"));
		return 0;
		}
	END_CATCH
	
	return nNumRead;
	}

BOOL COXGraphicFile::WriteData(const void* pBuffer, size_t length)
	{
	ASSERT(m_pGraphFile != NULL);
	
	TRY
		{
		m_pGraphFile->Write(pBuffer, (UINT) length);
		}
	CATCH(CFileException, e)
		{
		TRACE(_T("COXGraphicFile::WriteData : Catching CFileException\n"));
		return FALSE;
		}
	END_CATCH
	
	return TRUE;
	}


///////////////////////////////////////////////////////////////////////////
