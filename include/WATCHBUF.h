// ==========================================================================
// 							Class Specification : COXWatchBuffer
// ==========================================================================

// Header file : watchbuf.h

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
//	This class encapsulates an array of BYTE from which 
//	 all modifications are watched and flagged
//  Later on you can request whether a specified byte has changed

// Remark:
//		***

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __WATCHBUF_H__
#define __WATCHBUF_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXWatchBuffer : public CObject
{
DECLARE_DYNAMIC(COXWatchBuffer)

// Data members -------------------------------------------------------------
public:
	
protected:
	LPBYTE	m_pBuffer;
	LPBYTE	m_pFlags;
	int		m_nLength;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXWatchBuffer();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	void Create(int nLength);
	// --- In  : nLength : Length of the buffer
	// --- Out : 
	// --- Returns :
	// --- Effect : Creates a buffer of the specified length

	int GetLength() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The length of the buffer
	// --- Effect : 

	LPBYTE Get(int nIndex = 0) const;
	// --- In  : nIndex : The index of the element to get
	// --- Out : 
	// --- Returns : Pointer to the specified element
	// --- Effect : 

	void Set(int nIndex, LPBYTE pNewElements, int nLength = 1);
	// --- In  : nIndex : The index of the lement to get or set
	//			 pNewElements : Pointer to buffer containing new elements
	//			 nLength : Number of elements
	// --- Out : 
	// --- Returns : 
	// --- Effect : Copies nLength elements form pNewElements to the buffer
	//				starting from nIndex

	BOOL IsModified() const;
	BOOL IsModified(int nIndex) const;
	// --- In  : The index of an element (or none for the entire buffer)
	// --- Out : 
	// --- Returns : Whether the specified element has been changed
	// --- Effect : 

	int GetModified() const;
	int GetUnmodified() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The index of the first (un)modified element or -1 when not found
	// --- Effect : 

	void Empty(int nStartIndex = 0, int nLength = -1);
	// --- In  : nStartIndex : The index of the first element to clear
	//			 nLength : The number of elements to clear
	//					   or -1 to clear to the end
	// --- Out : 
	// --- Returns : 
	// --- Effect : Clears the contents of the specified elements and
	//				resets their modify flag

	void SetModified(BOOL bModified);
	void SetModified(BOOL bModified, int nIndex);
	// --- In  : The new modified state of the element
	//			 The index of an element (or none for the entire buffer)
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sets (or resets) the modified state of an element

	void Destroy();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Destroys the buffer

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXWatchBuffer();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
};

#endif
// ==========================================================================
