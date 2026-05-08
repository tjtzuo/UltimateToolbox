// ==========================================================================
// 							Class Specification : 
//							      COXTrace
// ==========================================================================

// Header file : OXTrace.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
/*
  COXTrace
 
  Hierarchy:
 	COXTrace
 
 	COXTrace, and its supporting macros, provide an extension to the TRACE() macros that
  are supplied with MFC. These extensions include:
 	1. Built-in word wrapping
 	2. Copying of the trace output to a file
 	3. Automatic marking of the start and end of a block of code
 	4. Automatic indenting of the messages to provide a way to track nesting of COXTrace
 	   blocks.
  
  The macros provide a means for quickly removing all of the tracing code from your
  project, without editing your files.
 
*/

//
// Undefine the macros, just in case
//
#undef	OXTRACE
#undef	OXTRACE_MSG
#undef	OXTRACE_MSGVAL
#undef	OXTRACE_WATCH
#undef	OXTRACE_FMSG
#undef	OXTRACE_SETWRAPWIDTH
#undef	OXTRACE_SETDUMPFILE
#undef	OXTRACE_SETFLUSH
#undef	OXTRACE_EMPTY

#if	defined(OXTRACEON)
	#define	OXTRACE(lpstrMsg)					COXTrace __oxTrace(lpstrMsg)
	#define	OXTRACE_WRITE(lpstrMsg)				__oxTrace.Write(lpstrMsg)
	#define	OXTRACE_WRITEVAL(lpstrMsg, pVal)	__oxTrace.Write((lpstrMsg), (pVal))
	#define	OXTRACE_WATCH(pVar)					__oxTrace.Write( #pVar _T(" == "), (pVar))
	#define	OXTRACE_WRITEFMT					__oxTrace.WriteFormatted
	#define	OXTRACE_SETINDENTSTEP(nValue)		(COXTrace::m_nIndentStep = (nValue))
	#define	OXTRACE_SETWRAPWIDTH(nValue)		(COXTrace::m_nWrapWidth = (nValue))
	#define	OXTRACE_SETPREFIX(bOn)				(COXTrace::m_bPrefixOn = (bOn))
	#define	OXTRACE_SETDUMPFILE(lpstrFile)		COXTrace::SetDumpFile(lpstrFile)
	#define	OXTRACE_SETFLUSH(nValue)			(COXTrace::m_bFlushOnWrite = (nValue))
#else
	#define	OXTRACE(lpstrMsg)
	#define	OXTRACE_WRITE(lpstrMsg)
	#define	OXTRACE_WRITEVAL(lpstrMsg, pVal)
	#define	OXTRACE_WATCH(pVar)
	#define	OXTRACE_SETINDENTSTEP(nValue)
	#define	OXTRACE_SETWRAPWIDTH(nValue)
	#define	OXTRACE_SETPREFIX(bOn)
	#define	OXTRACE_SETDUMPFILE(lpstrFile)
	#define	OXTRACE_SETFLUSH(nValue)
#endif

#if !defined(__OXTRACE_H__)
#define __OXTRACE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef OXTRACEON
/*
	OXTRACE_WRITEFMT
	Special replacement for the OXTRACE_WRITEFMT macro. When OXTRACEON is not defined,
	this will remove the parameters from OXTRACE_WRITEFMT calls from the code.
 
  --- In      : 
  --- Out     : 
  --- Returns : 
  --- Effect  : The parameters for this method will be removed from the compiled code.
*/
inline void OXTRACE_WRITEFMT(LPCTSTR , ...)
{
	;
}
#endif

class OX_CLASS_DECL COXTrace
{
private:
	// The header describing this block of code
	CString m_strHeader;	
	// The current indentation level
	static int m_nIndent;		
	// Object defining the file that the trace output will be copied to
	static CFile m_File;			

protected:
	/*
	  WriteMsg
	 	This function performs the actual work of outputing the message to the debugging
	  console, as well as to the dump file, if one has been specified.
	 
	  --- In      : lpstrMessage : Pointer to the text to be written out.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	 */
	void WriteMsg(LPCTSTR lpstrMessage);

public:
	// The number of symbols the indention will be increased on the next trace block
	static int m_nIndentStep;	
	// The number of symbols to use when determining if text output should be 
	// wrapped to a new line
	static int m_nWrapWidth;		
	// TRUE if tracing is currently enabled
	static BOOL m_bTracing;			
	// TRUE if each message should be written with the block's prefix
	static BOOL m_bPrefixOn;		
	// TRUE if te file should be flushed after each message is written out
	static BOOL m_bFlushOnWrite;	

	/**
	  COXTrace
	 	Constructor for this object, that will define the beginning of a new trace block,
	  outputing a message indicating the start of the block.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	 				 The indentation level (m_nIndent) is incremented.
	*/
	COXTrace(LPCTSTR lpstrHeader);

	/*
	  ~COXTrace
	 	Destructor for this object. This will be called when a COXTrace falls out of scope.
	  A message will be output to mark the end of the block.
	 
	  --- In      : 
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	 				 The indentation level (m_nIndent) is decremented.
	*/
	virtual ~COXTrace();

	/*
	  SetDumpFile
	 	Open the specified file for output. Any tracing output generated by 
		COXTrace objects will now be copied to this file.
	 
	  --- In      : lpstrFilename	-	The name of the file to write the tracing 
										output to.
					nOpenFlags		-	Sharing and access mode. Specifies the action 
										to take when opening the file. Refer to the
										CFile::Open() documentation on details.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The file will be opened (by default its previous contents 
					will be overwritten).
	*/
	static void SetDumpFile(LPCTSTR lpstrFilename, 
		UINT nOpenFlags=CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate);

	/*
	  SetIndentStep
	 	Set the number of symbols the indention will be increased when next block of 
	  TRACE statements is initialized
	 
	  --- In      : nIndentStep : number of symbols to increase the indent.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : Indention step will be changed. By default we use 4. It's good 
	 				 idea to call this function before 
	*/

	/*
	  WriteFormatted
	 	Write out a trace message using printf() style formatting for the message.
	 
	  --- In      : lpstrFormat : Pointer to a printf() style formatting string
	 				 ... : additional arguments, as needed by the formatting string
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void WriteFormatted(LPCTSTR lpstrFormat, ...);

	/*
	  Write
	 	Write out a text message
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 nValue : Integer value to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, int nValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 unValue : Unsigned integer value to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, unsigned int unValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 lValue : Long integer value to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, long lValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 ulValue : Unsigned long integer value to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, unsigned long ulValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 fValue : Floating point value to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, double fValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				 lpstrValue : Text string to append to the end of lpstrMsg
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, will have this
	 				 message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, LPCTSTR lpstrValue);

	/*
	  Write
	 	Write out a text message, followed by the passed in value. 
	  In this case, the object's Dump() method will be called, 
	  if _DEBUG is defined. The data will be dumped only to the console.
	 
	  --- In      : lpstrMsg : Pointer to the text to be written out.
	 				rObject : Reference to a CObject derived object.
	  --- Out     : 
	  --- Returns : 
	  --- Effect  : The debugging console, and possiblely the dump file, 
					will have this message sent to them for viewing.
	*/
	void Write(LPCTSTR lpstrMsg, const CObject& rObject);
};

#endif	// !defined(__OXTRACE_H__)
