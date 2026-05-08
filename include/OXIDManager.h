// ==========================================================================
// 					Class Specification : COXFileWatchNotifier
// ==========================================================================

// Header file : OXFileWatchNotifier.h

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
//	NO	Needs a resource

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :
//  This class tread-safe collection assigning an unique ID to each object

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXIDMANAGER_H__
#define __OXIDMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#error Make sure <AfxTempl.h> is added to your StdAfx.h
#endif
#ifndef __AFXMT_H__
#error Make sure <afxmt.h> is added to your StdAfx.h
#endif

#define INITIAL_SIZE 10
#define GROW_BY		  5
                            

template<class ClassType> 
class COXIDManager : public CObject
{      
// Data members -------------------------------------------------------------
public:

protected:
	CArray<ClassType*, ClassType*>	m_arObjs;     			// Array of the elements
	CUIntArray			m_arFreeIDs;			// not assigned ID
	int					m_nLastUsedSlot;		// last used index in m_arObjs
	CCriticalSection	m_crsecGuard;

private:

// Member functions ---------------------------------------------------------
public:     
	COXIDManager();         				// Standard for serialization
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	COXIDManager(int nSize, int nGrowBy);	
	// --- In  : nSize: initial number of expected Elements
	//			 nGrowBy: value for correcting the size of the array whet it grows
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	UINT		AddItem(ClassType* pObj);	
	// --- In  : pObj: the object to be added
	// --- Out : 
	// --- Returns : the ID of the object
	// --- Effect : adds pObj to the ID-Manager and returns a unique ID

	ClassType*	GetItemPtr(UINT nID) ;	
	// --- In  : nID: the ID of the requested object
	// --- Out : 
	// --- Returns : a pointer to the object or NULL if the ID is not valid
	// --- Effect : returns a pointer to the object identified by nID

	ClassType*	RemoveItem(UINT nID);		
	// --- In  : nID: the ID of the object to be removed
	// --- Out : 
	// --- Returns : a pointer to the object or NULL if the ID is not valid
	// --- Effect : returns a pointer to the object identified by nID and
	//				removes the object from the Manager.
	//				Note: Does not delete the Object !

	void		RemoveAll ();           	
	// --- In  :
	// --- Out : 
	// --- Returns : 
	// --- Effect : removes all OBJ from the Manager.
	//				Note: Does not delete the Objects !

	void		DeleteAll ();				
	// --- In  :
	// --- Out : 
	// --- Returns : 
	// --- Effect : Removes all Objects and deletes them

	int			GetHeadPosition()  ;	
	// --- In  :
	// --- Out : 
	// --- Returns : the ID of first slot that is not empty
	// --- Effect : returns first slot (for iteration)

	ClassType*	GetNext(int& nPos) ;	
	// --- In  : nPos: the ID of the current slot
	// --- Out : nPos: the ID of the next non empty slot
	// --- Returns : a pointer to the object at the current slot
	// --- Effect : returns object at slot nPos and calculates next nPos

	UINT		ItemCount();
	// --- In  :
	// --- Out : 
	// --- Returns : the number if items in the IDManager
	// --- Effect : determines the number of items

	virtual		~COXIDManager();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object destructor

#ifdef _DEBUG
	virtual void AssertValid() ;
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.

	virtual void Dump(CDumpContext& dc) ;
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 

#endif

protected:
	BOOL _IsIDFree (UINT nSearchID, int& nFoundSlot) ;
	BOOL _IsIDFree (UINT nSearchID) ;

private:
};	

// Data members -------------------------------------------------------------
// public:             

// protected:

	// CArray<ClassType*, ClassType*>	m_arObjs
	// --- Array of the elements

	// CUIntArray		m_arFreeIDs;			
	// --- not assigned ID

	// int				m_nLastUsedSlot;		
	// --- last used index in m_arObjs
	
	// CCriticalSection	m_crsecGuard;
	// --- synchronization object

// protected:

// Member functions ---------------------------------------------------------
// public:

template<class ClassType> 
COXIDManager<ClassType>::COXIDManager()
{ 
	m_arObjs.SetSize(INITIAL_SIZE, GROW_BY);
	m_nLastUsedSlot = -1;	//no slot in use
}                     

template<class ClassType> 
COXIDManager<ClassType>::COXIDManager(int nSize, int nGrowBy)
{	
	m_arObjs.SetSize(nSize, nGrowBy);
	m_nLastUsedSlot = -1;    //no slot in use
}

template<class ClassType> 
COXIDManager<ClassType>::~COXIDManager()
{   
	// remove all objects from the arrays 
	DeleteAll();
}      
                              
// ---- public services -----------------------------------
template<class ClassType> 
UINT COXIDManager<ClassType>::AddItem (ClassType* pObj)
{       
	UINT nID;

	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	// use ID from the FreeID-Array if possible
	if (m_arFreeIDs.GetUpperBound() >= 0)
		{    
		//get last element and shorten array by one element
		nID = (UINT) m_arFreeIDs[m_arFreeIDs.GetUpperBound()];
		m_arFreeIDs.RemoveAt(m_arFreeIDs.GetUpperBound());
		m_arFreeIDs.FreeExtra();     
		//because IDs in Free-Array are always < m_nLastUsedSlot,
		//wo do not have to correct the m_nLastUsedSlot value
		m_arObjs.SetAt(nID, pObj);
		}
	else
		{ 
		// set object to m_nLastUsedSlot+1		                            
		m_nLastUsedSlot++;
		nID = m_nLastUsedSlot;
		m_arObjs.SetAtGrow(nID, pObj);			
		}
	
	return nID;  // return the used array-slot
}

template<class ClassType> 
ClassType* COXIDManager<ClassType>::GetItemPtr (UINT nID) 
{   
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	//if ID is valid, return Object. Otherwise return NULL                     
	if ( ((int)nID) >= 0 && ((int)nID) <= m_nLastUsedSlot && !_IsIDFree (nID) )
		{	
		return m_arObjs[nID];
		}
	return NULL;
}

template<class ClassType> 
ClassType* COXIDManager<ClassType>::RemoveItem (UINT nID)
{                 
	ASSERT( ((int)nID) >= 0 && ((int)nID) <= (int)m_nLastUsedSlot);
	ASSERT( !_IsIDFree(nID));
	                     
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	ClassType* pObj;                     
	pObj = m_arObjs[nID];
	
	// check if element is the last one, if so shrink array
	if (nID == (UINT)m_nLastUsedSlot)
		{
		m_arObjs.RemoveAt(nID);
		m_nLastUsedSlot--;      
		//find the last used slot using FreeID-Array                  
		int nFreeID;
		while (_IsIDFree(m_nLastUsedSlot, nFreeID))
			{
			m_nLastUsedSlot--;  
			m_arFreeIDs.RemoveAt(nFreeID);
			}		
		}
	else
		{
		// remove and save free ID
		m_arObjs[nID] = NULL;
		m_arFreeIDs.Add(nID);
		}                 
		
	// Free Memory if Size > UsedSlot * 1.2
	if (m_arObjs.GetSize() > ((m_nLastUsedSlot+1) * 1.2))
		{
		m_arObjs.SetSize((int)((m_nLastUsedSlot+1) * 1.2), (int)((m_nLastUsedSlot+1) * 0.2));
		m_arObjs.FreeExtra();                                          
		}
	  
	return pObj;  
}

template<class ClassType> 
void COXIDManager<ClassType>::RemoveAll()
{                           
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	m_arObjs.RemoveAll();
	m_arObjs.SetSize(INITIAL_SIZE, GROW_BY);
	m_arObjs.FreeExtra();    
	
	m_arFreeIDs.RemoveAll();
	m_arFreeIDs.FreeExtra();		
		                                        
	m_nLastUsedSlot = -1;	//no slot in use
} 

template<class ClassType> 
void COXIDManager<ClassType>::DeleteAll()
{
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	for (int i=0; i <= m_nLastUsedSlot; i++)
		{
		if (m_arObjs[i] != NULL)
			{
			ASSERT_VALID(m_arObjs[i]);
			delete m_arObjs[i];
			}
		}
		m_arObjs.RemoveAll();
		m_arObjs.SetSize(INITIAL_SIZE, GROW_BY);
		m_arObjs.FreeExtra();    
	
		m_arFreeIDs.RemoveAll();
		m_arFreeIDs.FreeExtra();		
		                                        
		m_nLastUsedSlot = -1;	//no slot in use
}		
		   
template<class ClassType> 
int COXIDManager<ClassType>::GetHeadPosition() 
{
	// GetHeadPosition should be used similar to the service of
	// MFC-Lists.
	// So NULL is emty!!
	// +++++  Position == Slot+1  +++++
	
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	int nID = 0;
	
	while (_IsIDFree(nID))
		nID++;
	
	nID = (nID > m_nLastUsedSlot) ? NULL : nID+1;
		
	return nID;	
}            

template<class ClassType> 
ClassType* COXIDManager<ClassType>::GetNext(int& nPos)  
{                                  
	nPos--;	//see comment in GetHeadPosition
	  
	ASSERT(nPos >= 0 && nPos <= m_nLastUsedSlot);
	
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	ClassType* pObj = GetItemPtr(nPos);
	          
	//find next used ID	
	nPos++;
	while (_IsIDFree(nPos))
		nPos++;		
	nPos = (nPos > m_nLastUsedSlot) ? NULL : nPos+1;	
	
	return pObj;
}
		   
template<class ClassType> 
UINT	COXIDManager<ClassType>::ItemCount()
	{
	return m_nLastUsedSlot-m_arFreeIDs.GetUpperBound();
	}

// ---- diagnostics ---------------------------------------
#ifdef _DEBUG
template<class ClassType> 
void COXIDManager<ClassType>::AssertValid() 
{
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	CObject::AssertValid();
	                       
	//checks if all Elemets are valid	                       
	for (int i=0; i <= m_arObjs.GetUpperBound(); i++)
		{
		if (m_arObjs[i] != NULL)
			{
			ASSERT_VALID(m_arObjs[i]);
			}
		}
}

template<class ClassType> 
void COXIDManager<ClassType>::Dump(CDumpContext& dc) 
{
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	CObject::Dump(dc);
	dc << _T("\nNumber of elements: ");
	dc << m_nLastUsedSlot-m_arFreeIDs.GetUpperBound() << _T("\n");
	
	if (dc.GetDepth() > 0)
		{
		int nPos;
		nPos = GetHeadPosition();
		while (nPos != 0)
			{
			dc << _T("\tID: ") << nPos;
			dc << _T("\t Element: ");
			GetNext(nPos)->Dump(dc);
			}
		}
		
}
#endif // _DEBUG


// protected:

template<class ClassType> 
BOOL COXIDManager<ClassType>::_IsIDFree (UINT nSearchID, int& nFoundSlot) 
	// --- In  : nSearchID: the ID to be checked
	// --- Out : nFoundSlot: the position of the ID
	// --- Returns : TRUE if the ID is free
	// --- Effect : searchs for nSearchID in the Free-ID Array.
	//				If so, nFoundSlot returns the index from the slot.
{    
	int i;
	
	// Synchronization 
	CSingleLock lockObj( &m_crsecGuard);
	lockObj.Lock();

	for ( i=0; i <= m_arFreeIDs.GetUpperBound(); i++)
		{
		if (m_arFreeIDs[i] == nSearchID)
			{
			nFoundSlot = i;
			return TRUE;
			}
		}
	return FALSE;

}

template<class ClassType> 
BOOL COXIDManager<ClassType>::_IsIDFree (UINT nSearchID)  
	// --- In  : nSearchID: the ID to be checked
	// --- Out : 
	// --- Returns : TRUE if the ID is free
	// --- Effect : searchs for nSearchID in the Free-ID Array.
{   
	// searchs for nSearchID in the Free-ID Array.
	
	int i;
	return _IsIDFree(nSearchID, i);
}

// private:

#endif	// __OXIDMANAGER_H__
