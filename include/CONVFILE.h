// ==========================================================================
// 							Class Specification : COXConvertedFile
// ==========================================================================

// Header file : convfile.h

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
//		Pure functions : ConvertRead 
//						 ConvertWrite
//	YES	Derived from CFile

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO 	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	Yes	Uses exceptions (same as CFile)

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class enhances the CFile bass class by enabling conversions
//	Every block that is read from file or written to file is first
//   converted by calling the pure virtual functions ConvertRead and ConvertWrite
//  These conversion functions work with buffers of fixed size that must specified
//   when a COXConvertedFile is constructed

// Remark:
//	To enable the conversion blockwise are file functions are buffered until
//   an entire block can be processed
//  The actual file positioning occurs always with that specified size
//  Blocks are padded with 0-characters

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __CONVFILE_H__
#define __CONVFILE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "watchbuf.h"

#ifndef LPCBYTE
#define LPCBYTE const unsigned char*
#endif


class OX_CLASS_DECL COXConvertedFile : public CFile
{
DECLARE_DYNAMIC(COXConvertedFile)

// Data members -------------------------------------------------------------
public:
	
protected:
	BOOL			m_bEnabled;

	const UINT 		m_nBufferLength;
	LPBYTE			m_pOriginalBuffer;
	COXWatchBuffer	m_convertedBuffer;
	
	int				m_nBufferPos;
	DWORD			m_nFilePos;
	BOOL			m_bRead;

	BOOL			m_bOpenedForRead;
	BOOL			m_bOpenedForWrite;

	CFile*			m_pDelegateToFile;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXConvertedFile(UINT wBufferLength, BOOL bConvertEnabled = TRUE);
	// --- In  :wBufferLength : The length used for conversions
	//			bConvertEnabled : Whether conversion should be actually done
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
	
	UINT GetBufferLength() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The length of the internal buffer
	// --- Effect : 

	BOOL IsConvertEnabled() const;
	// --- In  :
	// --- Out : 
	// --- Returns : Whether conversions are enabled or not
	// --- Effect : 

	BOOL EnableConvert(BOOL bEnable = TRUE);
	// --- In  : bEnable : The new enable state
	// --- Out : 
	// --- Returns : Whether it succeeds or not 
	// --- Effect : Enables or disables the file conversion
	//				This can only be done on buffer boundaries
	//				(So must be : GetPosition() % GetBufferLength() == 0)

	WORD ForceEnableConvert(BOOL bEnable = TRUE);
	// --- In  : bEnable : The new enable state
	// --- Out : 
	// --- Returns : The number of bytes skipped to reach th next buffer boundary
	// --- Effect : Enables or disables the file conversion
	// 				When this function is invoked and the buffer is not
	//				 on a boundary (GetPosition() % GetBufferLength() != 0)
	//				 bytes will be skipped until a boundary is reached
	//				When the file has been opened for writing, zero-characters
	//				 will be written

	virtual BOOL IsOpen() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the file has been opened and not yet closed
	// --- Effect : 
	
	virtual BOOL ConvertRead(LPCBYTE pOriginal, LPBYTE pConverted) = 0;
	// --- In  : pOriginal : Buffer with original contents
	//			 pConverted : Empty (but allocated) buffer
	// --- Out : pConverted : Converted buffer
	// --- Returns : Whether it succeeded or not
	//				 Should always be TRUE for now
	// --- Effect : Converts a block when it is read from file
	//				pOriginal contains the original file contents
	// 				Both blocks have the length that was specified at construction time

	virtual BOOL ConvertWrite(LPCBYTE pOriginal, LPBYTE pConverted) = 0;
	// --- In  : pOriginal : Buffer with original contents
	//			 pConverted : Empty (but allocated) buffer
	// --- Out : pConverted : Converted buffer
	// --- Returns : Whether it succeeded or not
	//				 Should always be TRUE for now
	// --- Effect : Converts a block when it is written from file
	//				pConverted should contain the new file contents
	// 				Both blocks have the length that was specified at construction time

// ANSI functions
	virtual LPSTR ReadString(LPSTR psz, UINT nMax);
	// --- In  : psz : Specifies a pointer to a user-supplied buffer that will receive 
	// 				   a null-terminated text string.
	//			 nMax : Specifies the maximum number of characters to read. 
	//					Should be one less than the size of the lpsz buffer.
	// --- Out : 
	// --- Returns : A pointer to the buffer containing the text data; 
	//				 NULL if end-of-file was reached.
	// --- Effect : This function completely mimics the CStdioFile::ReadString behaviour
	//				including the CR/LF - LF conversion and EOF-character detection

	virtual void WriteString(LPCSTR psz);
	// --- In  : psz : Specifies a pointer to a buffer containing a null-terminated 
	// 				   text string.
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function completely mimics the CStdioFile::WriteString behaviour
	//				including the CR/LF - LF conversion and EOF-character detection

#ifdef WIN32
// UNICODE functions
	virtual LPSTR ReadString(LPWSTR psz, UINT nMax);
	// --- In  : psz : Specifies a pointer to a user-supplied buffer that will receive 
	// 				   a null-terminated text string.
	//			 nMax : Specifies the maximum number of characters to read. 
	//					Should be one less than the size of the lpsz buffer.
	// --- Out : 
	// --- Returns : A pointer to the buffer containing the text data; 
	//				 NULL if end-of-file was reached.
	// --- Effect : This function completely mimics the CStdioFile::ReadString behaviour
	//				including the CR/LF - LF conversion and EOF-character detection

	virtual void WriteString(LPCWSTR psz);
	// --- In  : psz : Specifies a pointer to a buffer containing a null-terminated 
	// 				   text string.
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function completely mimics the CStdioFile::WriteString behaviour
	//				including the CR/LF - LF conversion and EOF-character detection
#endif

	BOOL DelegateOpen(CFile* pDelegateToFile, UINT nOpenFlags);
	// --- In  : pDelegateToFile : The new file pointer to delegated to.
	//			 nOpenFlags : The flags that will determine how this encrypted file should
	//						  be treated.
	// --- Out : 
	// --- Returns : succesfull or not
	// --- Effect : This function enables this conversion to be inserted into a chain of transformations.
	//				This chaining requires a setup sequence of transformations.  Via this function 
	//				you tell this conversion on what file pointer he has to do his conversion.
	//				This file pointer can allready be converted himself, so that's the way to 
	//				form a sequence of transformations.  More specifically this file pointer replaces
	//				the CFile baseclass of this encrypted file.  All calls to the baseclass will now
	//				be routed(delegated) to this file pointer, which is the next in the transformation chain.

	BOOL UnDelegateOpen();
	// --- In  : 
	// --- Out : 
	// --- Returns : succesfull or not
	// --- Effect : Calling this function ensures that this transformation is not anymore
	//				chained to other transfromations.  It's again a stand-alone transformation.
	//				The state where this object will be in after calling this function, is the
	//				same when the object was just constructed.

	// The following functions are the overloaded versions of the original CFile functions
	// They take care of the buffering and conversions
	virtual BOOL Open(LPCTSTR pszFileName, UINT nOpenFlags,	CFileException* pError = NULL);

	virtual CFile* Duplicate() const;
#if _MFC_VER >= 0x0700
	virtual ULONGLONG GetPosition() const;
	virtual ULONGLONG GetLength() const;
#else
	virtual DWORD GetPosition() const;
	virtual DWORD GetLength() const;
#endif
	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);

	virtual UINT Read(void FAR* lpBuf, UINT nCount);
	virtual void Write(const void FAR* lpBuf, UINT nCount);

	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXConvertedFile();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      
	DWORD GetLengthPrivate() const;
	BOOL ReadBuffer();
	void FlushBuffer();
	void CheckBufferPosition();

private:
                   
};

#endif
// ==========================================================================
