// ==========================================================================
// 							Class Specification : COXFileSpec
// ==========================================================================

// Header file : file.h

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
//	NO Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class encapsulates a file specification.
//		A file specification consists of a fule base name, a file extender
//		and file attributes (length, ...)

// Remark:
//		File specifications are always represented in lower case characters
//		 An implicite conversion is done when the supplied names are in upper case;
//		All member functions starting with "Do" will physically change something
//		 on disk.  Other functions only change the internal data.

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __FILE_H__
#define __FILE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXFileSpec : public CObject
{
DECLARE_SERIAL(COXFileSpec)

// Data members -------------------------------------------------------------
public:
	
protected:
	CString 			m_sFileName;
	CTime				m_time;
	LONG				m_lnLength;
	BYTE 				m_eAttributes;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXFileSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXFileSpec(LPCTSTR pszFileName);
	// --- In  : pszFileName : The new file name of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : 
	// --- Effect : Contruction of an object together with a SetFileName()
	//				Notice that whether the file specification is valid or not
	//				cannot be checked in this way
	
    COXFileSpec(const COXFileSpec& fileSrc);
	// --- In :	fileSrc : File object which will be copied
	// --- Out : 
	// --- Returns :
	// --- Effect : Copy contruction.
	
	COXFileSpec& operator=(const COXFileSpec& fileSrc);
	// --- In :	fileSrc : File object which will be assign to 'this' file object
	// --- Out:
	// --- Returns: 
	// --- Effect : Assignment operator
	
	CString GetBaseName() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The base name of this object
	//			 	It does not end in a full stop
	// --- Effect : 
	BOOL SetBaseName(LPCTSTR pszBaseName);
	// --- In  : pszBaseName : The new base name of the object
	// --- Out : 
	// --- Returns : Whether base name has a correct format
	//				 Wild characters are allowed
	// --- Effect : 
	void ForceSetBaseName(LPCTSTR pszBaseName);
	// --- In  : pszBaseName : The new base name of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	CString GetExtender() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The extender of this object
	//			 	It never begins with a full stop
	//			 	The result may be an empty string
	// --- Effect : 
	BOOL SetExtender(LPCTSTR sExtender);
	// --- In  : pszExtender : The new extender of the object
	// --- Out : 
	// --- Returns : Whether extender has a correct format
	//				 Wild characters are allowed
	// --- Effect : 
	void ForceSetExtender(LPCTSTR sExtender);
	// --- In  : pszExtender : The new extender of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	CString GetFileName() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The base name and the extender of this object
	//			 	The result may be an empty string
	// --- Effect : 
	BOOL SetFileName(LPCTSTR sFileName);
	// --- In  : pszFileName : The new base name and extender of the object
	// --- Out : 
	// --- Returns : Whether file name has a correct format
	//				 Wild characters are allowed
	// --- Effect :                           
	void ForceSetFileName(LPCTSTR sFileName);
	// --- In  : pszFileName : The new base name and extender of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	CTime GetTime() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The creation date and time of this object
	// --- Effect : 

	BOOL SetTime(CTime time);
	// --- In  : time : The new date and time of the object
	// --- Out : 
	// --- Returns : Whether date and time specification is valid
	// --- Effect : Notice that the DOS file system can only work with
	//				even numbered seconds (12 seconds exist, 13 not)
	//				DOS will automatically convert this
	
	LONG GetLength() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The file length in bytes of this object
	// --- Effect : 

	BOOL SetLength(LONG lnLength);
	// --- In  : lnLength : The length of the object
	// --- Out : 
	// --- Returns : Whether length specification is valid
	// --- Effect :                           
	
	BYTE GetAttributes() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The file attributes of this object
	// --- Effect : 

	BOOL SetAttributes(CFile::Attribute attributes);
	// --- In  : atrributes : The attributes of the object
	// --- Out : 
	// --- Returns : Whether attributes specification is valid
	// --- Effect :                           

	BOOL IsEmpty() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the file specification is empty
	// --- Effect : 

	void Empty();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Clears the file specification

	virtual void Serialize( CArchive& archive );
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Read or write to archive
	
	BOOL operator==(const COXFileSpec& fileSpec) const;
	BOOL operator!=(const COXFileSpec& fileSpec) const;
	BOOL operator<=(const COXFileSpec& fileSpec) const;
	BOOL operator<(const COXFileSpec& fileSpec) const;
	BOOL operator>=(const COXFileSpec& fileSpec) const;
	BOOL operator>(const COXFileSpec& fileSpec) const;
	// --- In  : fileSpec : Second object to which this object will be compared
	// --- Out : 
	// --- Returns : FALSE or TRUE
	// --- Effect : Comparison operators
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXFileSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
};

#endif
// ==========================================================================
