// ==========================================================================
// 							Class Specification : COXPathIterator
// ==========================================================================

// Header file : pathiter.h

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
//		This class encapsulates the a path iterator

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __PATHITER_H__
#define __PATHITER_H__         

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef WIN32
	#include <dos.h>
#endif


class COXPathSpec;
class OX_CLASS_DECL COXPathIterator : public CObject
{
friend class COXPathSpec;

DECLARE_DYNAMIC(COXPathIterator)

// Data members -------------------------------------------------------------
public:
	
protected:                   

#ifdef WIN32
	HANDLE 			m_hFindFile;
	WIN32_FIND_DATA m_FindFileData;	
#else
    _find_t			m_FileInfo;
#endif

	BOOL 			m_bValid;
private:
	
// Member functions ---------------------------------------------------------
public:
	COXPathIterator();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
	
	BOOL IsItValid();
	// --- In  :
	// --- Out : 
	// --- Returns : whether the iterator can be used in a iteration proces
	//				 or not.
	// --- Effect : 
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXPathIterator();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

};

#endif
// ==========================================================================
