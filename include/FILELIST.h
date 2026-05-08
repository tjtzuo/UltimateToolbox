// ==========================================================================
// 							Class Specification : COXFileList
// ==========================================================================

// Header file : filelist.h

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
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class is used to get a list of all the files in a
//		certain path.
//		You first set the path, by specifying it	(SetPath())
//		Then you start the search					(Search())
//		And then you examine the result				(GetList())

// Remark:
//		***

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "path.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXFileList : public CObject
{
DECLARE_DYNAMIC(COXFileList)

// Data members -------------------------------------------------------------
public:
	
protected:
	COXPathSpec	m_path;
	CObArray	m_fileArray;	

private:
	
// Member functions ---------------------------------------------------------
public:
	COXFileList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXPathSpec GetPath() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The path of filelist
	// --- Effect : 
	BOOL SetPath(COXPathSpec path);
	// --- In  : path : The new path specification of the filelist
	// --- Out : 
	// --- Returns : Whether the setting succeeded or not
	// --- Effect : The specified path is converted to an absolute path
	//				if it is relative
	//				When no file specification is supplied *.* is assumed
                    
	BOOL Search();                    
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the search succeeded or not
	// --- Effect : This function will search the directory specified
	//				by SetPath and build a list of file specifications
	//				All the found files are appended to the list
	//				These can be accessed by GetList

	const CObArray* GetList() const;                    
	// --- In  : 
	// --- Out : 
	// --- Returns : A const pointer to the list of const file specifications
	// --- Effect : The list may be empty, when search did not find any files
	
	const COXFileSpec* GetAt(int nIndex) const;
	// --- In  :
	// --- Out :
	// --- Returns :
	// --- Effect :
	
	void Sort();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sorts the file list
	
	void ClearList();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Clears the file list
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXFileList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
// Message handlers ---------------------------------------------------------

};

#endif              
// ==========================================================================
