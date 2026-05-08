// ==========================================================================
// 					Class Specification : COXIteratorUNC
// ==========================================================================

// Header file : OXIteratorUNC.h

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
//	YES	Derived from COXUNC

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class can be used to iterate a files and directories.
//		- You can start by specifying the start directory and file spec
//		   (e.g. C:\Windows\*.ini).  You can assign this to a COXIteratorUNC object.
//		- Now you can iterate by using Start(), Next() several times and End().
//		  Instead of using Next you can also use operator++.
//		  During iteration the COXIteratorUNC will change value until
//		   the end is reached (and the object will be empty)
//		- When starting you can specify whether files and/or directories
//		   should be part of the result.  You can also specify whether to search recursively

// Example :
//		COXIteratorUNC iterUNC("C:\\Windows\\*.ini");
//		iterUNC.Start(TRUE, FALSE, TRUE);
//		while(!iterUNC.IsEmpty())
//			{
//			TRACE1("Ini-file : %s\n", (LPCTSTR)iterUNC);
//			iterUNC++;
//			}
//		iterUNC.End();


// Remark:
//	During iteration you may still call all do-functions of the base class
//	 COXUNC (e.g. Delete() to delete all the iterated files)

//	When using the copy constructor or assignment operator, note that the search 
//	 specific data is NOT copied, only the data from the base class COXUNC.
//	This is because find file handles cannot be duplicated

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXITERATORUNC_H__
#define __OXITERATORUNC_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXUNC.h"

#ifndef __AFXTEMPL_H__
#error Make sure <AfxTempl.h> is added to your StdAfx.h
#endif


class OX_CLASS_DECL COXIteratorUNC : public COXUNC
{
// Data members -------------------------------------------------------------
public:
protected:

	struct COXSearchData
		{
		COXUNC m_searchUNC;
		HANDLE m_hFindFile;
		WIN32_FIND_DATA m_findFileData;

		COXSearchData()
			:
			m_searchUNC(),
			m_hFindFile(NULL)
			{
			::ZeroMemory(&m_findFileData, sizeof(m_findFileData));
			}
		};
	CList<COXSearchData*, COXSearchData*> m_dirSearchStack;
	COXSearchData m_fileSearch;

	BOOL m_bIncludeFiles;
	BOOL m_bIncludeDirs;
	BOOL m_bRecursive;
	BOOL m_bDepthFirst;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXIteratorUNC(LPCTSTR pszUNC = NULL, BOOL bURLPart = FALSE);
	// --- In  : pszURL : Full UNC specification
	//			 bURLPart : Whether this UNC is part of a URL
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructor of the object

	COXIteratorUNC(LPCTSTR pszServer, LPCTSTR pszShare, LPCTSTR pszDirectory, LPCTSTR pszFile, BOOL bURLPart = FALSE);
	// --- In  : pszServer : 
	//			 pszShare : 
	//			 pszDirectory : 
	//			 pszFile : 
	//			 bURLPart : Whether this UNC is part of a URL
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructor of the object


	COXIteratorUNC(const COXUNC& UNC);
	COXIteratorUNC(const COXIteratorUNC& iterUNC);
	// --- In  : UNC / iterUNC : Source UNC
	// --- Out : 
	// --- Returns :
	// --- Effect : Copy constructor
	//				Note that search dependent data is not copied
	//				(Win32 find file hendles cannot be duplicated)

	COXIteratorUNC& operator=(const COXUNC& UNC);
	COXIteratorUNC& operator=(const COXIteratorUNC& iterUNC);
	COXIteratorUNC& operator=(LPCTSTR pszUNC);
	// --- In  : UNC / iterUNC : Source UNC
	// --- Out : 
	// --- Returns : This object
	// --- Effect : Assignment operator
	//				Note that search dependent data is not assigned
	//				(Win32 find file hendles cannot be duplicated)

	BOOL Start(BOOL bIncludeFiles = TRUE, BOOL bIncludeDirs = FALSE, 
		BOOL bRecursive = FALSE, BOOL bDepthFirst = FALSE);
	// --- In  : bIncludeFiles : Whether to include files (non-directories) in the search result
	//			 bIncludeDirs : Whether to include subdirectories in the search result
	//			 bRecursive : Whether subdirectories should be searched recursively
	//			 bDepthFirst : Whether a recursive search should use depth-first (TRUE)
	//						   or breadth-first (FALSE) traversal
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Starts the iteartion.  This object will get the new value
	//				When this object is empty, the end of the iteration has been reached

	BOOL Next();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Iterate to the next UNC.  This object will get the new value
	//				When this object is empty, the end of the iteration has been reached

	BOOL End();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Ends a possible ongoing iteration
	//				This will free used resources

	COXIteratorUNC& operator++();       
	COXIteratorUNC operator++(int);     
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Prefix/Postfix  increment operator (same as Next())
	//				Iterate to the next UNC.  This object will get the new value
	//				When this object is empty, the end of the iteration has been reached


	const WIN32_FIND_DATA* GetCurrentSearchResult() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Pointer to the data associated with the current search result or
	//				 NULL when no search is in progress
	// --- Effect : The extra data can be used to retrive extra information about
	//				the current item (see documentation of WIN32_FIND_DATA)

#ifdef _DEBUG
#endif

	virtual ~COXIteratorUNC();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of the object

protected:
	CString GetNextSearchDirectory();

	BOOL IsAcceptableFile() const;
	BOOL IsAcceptableDirectory() const;

	BOOL EndFileSearch();
	BOOL EndAllDirectorySearches();
	BOOL EndLastDirectorySearch();

private:
};

// Include inline functions
#include "OXIteratorUNC.inl"

#endif // __OXITERATORUNC_H__
// ==========================================================================
