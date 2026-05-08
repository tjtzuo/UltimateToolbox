// ==========================================================================
// 							Class Specification : COXPathSpec
// ==========================================================================

// Header file : path.h

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
//	YES	Derived from COXDirSpec and COXFileSpec

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class encapsulates a path specification
//		A file specification consists of a directory (drive and subdirectory)
//		 and a file name (base name and extender) and file attributes (length, ...)
//		All member functions starting with "Do" will physically change something
//		 on disk.  Other functions only change the internal data.
//		E.g. The normal destructor only destroys the internal representation,
//		 to actually remove a directory use DoRemove()

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __PATH_H__
#define __PATH_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "dir.h"
#include "file.h"
#include "pathiter.h"


class OX_CLASS_DECL COXPathSpec : public COXDirSpec, public COXFileSpec
{
DECLARE_DYNAMIC(COXPathSpec)

// Data members -------------------------------------------------------------
public:
	
protected:

private:
	
// Member functions ---------------------------------------------------------
public:
	COXPathSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXPathSpec(LPCTSTR pszPath);
	// --- In  : pszPath : The new path name of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : 
	// --- Effect : Contruction of an object together with a SetPath()
	//				Notice that whether the path specification is valid or not
	//				cannot be checked in this way
	
    COXPathSpec(const COXPathSpec& pathSrc);
	// --- In :	pathSrc : Path object which will be copied
	// --- Out : 
	// --- Returns :
	// --- Effect : Copy contruction.
	
	COXPathSpec& operator=(const COXPathSpec& pathSrc);
	// --- In :	pathSrc : Path object which will be assign to 'this' file object
	// --- Out:
	// --- Returns: 
	// --- Effect : Assignment operator
	
	CString GetPath() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The path specification  of this object
	// --- Effect : 
	BOOL SetPath(LPCTSTR pszPath);
	// --- In  : pszPath : The new path specification of the object
	// --- Out : 
	// --- Returns : Whether path has a correct format
	//				 Wild characters only allowed in the file specification
	// --- Effect : 
	void ForceSetPath(LPCTSTR pszPath);
	// --- In  : pszPath : The new path specification of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	BOOL SetPath(const COXDirSpec& dirSpec, const COXFileSpec& fileSpec);
	// --- In  : dirSpec : The directory part of the new path
	//           fileSpec : The file part of the new path
	// --- Out : 
	// --- Returns : Whether path has a correct format
	// --- Effect : 
	
	CString GetShortDescription();
	// --- In  :
	// --- Out : 
	// --- Returns :The path specification with all but the last subdirectory 
	//              replaced by a double full stop
	//				(E.g. C:\ONE\TWO\THREE\test.txt becomes C:\..\THREE\test.txt)
	// --- Effect : 
		
	BOOL GetShortPathName(CString& sShortPath);
	// --- In  :
	// --- Out :	sShortPath	-	short form of the path 8.3
	// --- Returns:	TRUE if succeed or FALSE otherwise
	// --- Effect : 
		
	BOOL MakeTemp(BOOL bCreateEmpty = TRUE, LPCTSTR pszPrefix = _T("TMP"), LPCTSTR pszTempDir = _T(""));
	// --- In  : bCreateEmpty : Whether to create an empty file with the resulting name
	//			 pszPrefix : Prefix that will be used in the temporary file name
	//						 Only the first three characters will be used
	//			 pszTempDir : address of directory name for temporary file. if empty, windows temp dir
	//						  will be used.
	// --- Out : 
	// --- Returns : Whether a unique path spec could be determined
	// --- Effect : Replaces the path specification by that of the unique
	//				temporary file
	//				By default an empty temporary file with that name will be created on disk
	
	BOOL MakeAbsolute();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether path has a correct format
	// --- Effect : Replaces the path specification by an absolute path specification
	
	BOOL MakeUnique();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether a unique file name could be made
	// --- Effect : Replaces the file specification by another file specification
	//				that does not yet exist on disk
	//				So this can be used to make unique file names
	
	BOOL Exists() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the path physically exists on disk
	// --- Effect : 
	
	BOOL IsEmpty() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the path specification is empty
	// --- Effect : 
	
	void Empty();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : CLears the path specification
	
	BOOL GetFirstFile(COXPathIterator& Iterator) const;
	// --- In  :
	// --- Out : A COXPathIterator value that can be used for iteration or object pointer
	//				 retrieval; Invalid iterator if the the dir is empty.
	// --- Returns : Whether the iterator is valid or not
	// --- Effect : Gets the COXPathIterator pointer to the first file
	//				of this dir.
	
    COXFileSpec GetNextFile(COXPathIterator& Iterator) const;
	// --- In  : 
	// --- Out : FIterator : a reference to a COXPathIterator returned by a previuos call
	//					  to GetFirstFile or GetNextFile
	// --- Returns : the file specification that represents the COXPathIterator
	// --- Effect : Gets the list element identified by COXPathIterator, then sets
	//				COXPathIterator to the COXPathIterator value of the next file in the dir
	//				You can use GetNextFile in a forward iteration loop if
	//				you establish the initial COXPathIterator with a call to
	//				GetFirstFile.
	//				If the retrieved COXFileSpec is the last in the dir, then
	//				COXPathIterator is in his INVALID state (check the IsItValid function) .
	
	BOOL GetFirstDir(COXPathIterator& Iterator) const;
	// --- In  :
	// --- Out :
	// --- Returns : A void pointer value that can be used for iteration or object pointer
	//				 retrieval; NULL if the the dir is empty.
	// --- Effect : Gets the void pointer to the fileinfo of the first subdir
	//				of this dir.
	
    COXDirSpec GetNextDir(COXPathIterator& Iterator) const;
	// --- In  : 
	// --- Out : FIterator : a reference to a fileinfo returned by a previuos call
	//					  to GetFirstDir or GetNextDir
	// --- Returns : the dir specification that represents the pFInfo
	// --- Effect : Gets the dir element identified by pFInfo, then sets
	//				pFInfo to the fileinfo value of the next subdir in the dir
	//				You can use GetNextDir in a forward iteration loop if
	//				you establish the initial fileinfo with a call to
	//				GetFirstDir.
	//				If the retrieved COXDirSpec is the last in the dir, then
	//				COXPathIterator is in his INVALID state (check the IsItValid function) .
	
	BOOL DoSearch(COXFileSpec fileName, COXDirSpec startingDir = COXDirSpec(), BOOL bRecursively = FALSE);
	// --- In  : fileName : The name of the file to search for (must not include wild chars)
	//			 startingDir : The directory in which to search
	//			 bRecursively : Whether subdirectories of the specified directory 
	//   						should be searched as well
	// --- Out : 
	// --- Returns : Whether specified file was found
	// --- Effect : Searches for the specified file and assigns the result to this object
	//				The search order is :
	//					1 - The specified directory if the directory specification is not empty
	//					2 - Subdirectories of the specified directory if
	//						the directory specification is not empty and bRecursively is TRUE
	//						(This is not yet implemented)
	//					3 - The directory that contains the executable file of this running program
	//					4 - The current directory
	//					5 - The Windows directory
	//					6 - The System directory
	//					7 - The directories specified in the PATH-environment variable
	
	BOOL DoCopy(COXPathSpec destinationPath);	
	// --- In  : destinationPath : To destination to where the file must be
	//			 				   copied.  The filename may be empty.
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Physically copies the contents of one file into another
	//				When the destination file already exists, it will be truncated
	
	BOOL DoMove(COXPathSpec destinationPath) const;	
	// --- In  : destinationPath : To destination to where the file must be
	//			 				   renamed or moved.  The filename must not be empty.
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Physically renames or moves the file to another name or location
	//				When the destination file already exists, it will be truncated
	//				Directories cannot be renamed on moving across disks is not supported
	
	BOOL DoRemove(BOOL bIgnoreReadOnly = FALSE) const;	
	// --- In  : bIgnoreReadOnly : Whether the file should be removed,
	//		   			even when it is read-only
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Removes the directory

	BOOL DoGetInfo();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Get the real time, file length and attributes from disk
	//				and puts them in this path specification

	BOOL DoSetTime();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Makes the real  time of the file on disk equal to the
	//				time specified in the path specification
		
	BOOL DoSetLength();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Makes the real length of the file on disk equal to the
	//				length specified in the path specification
		
	BOOL DoSetAttributes();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Makes the real attributes of the file on disk equal to the
	//				attributes specified in the path specification
	//				Notice that other function may change the attribute as a
	//				side effect (archive attribute)

	virtual void Serialize( CArchive& archive );
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Read or write to archive
	
	BOOL operator==(const COXPathSpec& pathSpec) const;
	BOOL operator!=(const COXPathSpec& pathSpec) const;
	BOOL operator<=(const COXPathSpec& pathSpec) const;
	BOOL operator<(const COXPathSpec& pathSpec) const;
	BOOL operator>=(const COXPathSpec& pathSpec) const;
	BOOL operator>(const COXPathSpec& pathSpec) const;
	// --- In  : pathSpec : Second object to which this object will be compared
	// --- Out : 
	// --- Returns : FALSE or TRUE
	// --- Effect : Comparison operators

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXPathSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

#ifndef _DEBUG
	void* operator new(size_t nSize);
#else
	void* operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#endif
	void operator delete(void* p);
	// This functions must be overloaded, because both direct parents of COXPathSpec,
	// namely COXDirSpec and COXFileSpec are derived from CObject
	// So this is done to unambiguize
protected:      
	static HMODULE GetThisModule();
	static BOOL SearchEnvironment(LPCTSTR pszFileName, LPCTSTR pszVarName, LPTSTR pszPathName);
	static DWORD GetEnvironmentVar(LPCTSTR pszVarName, LPTSTR pszValue, DWORD nLength);

private:
                   
};

#endif
// ==========================================================================
