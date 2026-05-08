// ==========================================================================
// 							Class Specification : COXDirList
// ==========================================================================

// Header file : dirlist.h

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
//		This class is used to get a list of all the directories in a
//		certain path.
//		You first set the path, by specifying it	(SetPath())
//		Then you start the search					(Search())
//		And then you examine the result				(GetList())

// Remark:
//		***

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __DIRLIST_H__
#define __DIRLIST_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "path.h"


class OX_CLASS_DECL COXDirList : public CObject
{
DECLARE_DYNAMIC(COXDirList)

// Data members -------------------------------------------------------------
public:
	
protected:
	COXPathSpec	m_path;
	CObArray	m_dirArray;	

private:
	
// Member functions ---------------------------------------------------------
public:
	COXDirList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXPathSpec GetPath() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The path of dirlist
	// --- Effect : 
	BOOL SetPath(COXPathSpec path);
	// --- In  : path : The new path specification of the dirlist
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
	//				by SetPath and build a list of dir specifications
	//				All the found dirs are appended to the list
	//				These can be accessed by GetList

	const CObArray* GetList() const;                    
	// --- In  : 
	// --- Out : 
	// --- Returns : A const pointer to the list of const dir specifications
	// --- Effect : The list may be empty, when search did not find any dirs
	
	const COXDirSpec* GetAt(int nIndex) const;
	// --- In  :
	// --- Out :
	// --- Returns :
	// --- Effect :	

	void Sort();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sorts the Dir list
	
	void ClearList();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Clears the dir list
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXDirList();
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
