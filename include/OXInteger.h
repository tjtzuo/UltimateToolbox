// ==========================================================================
// 					Class Specification : COXInteger
// ==========================================================================

// Header file : OXInteger.h

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
//	NO	Derived from 

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class encapsulates a (64-bit) integer.  It supplies functionality
//	 to convert from one radix to another (e.g. bianry to hex).
//	The internal representation is always decimal.
//	Apart from this is can also insert thousand seperators.

// Remark:
//	This class has a constructor accepting a number (LONGLONG) and a conversion
//   operator producing the number.

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXINTEGER_H__
#define __OXINTEGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


class OX_CLASS_DECL COXInteger
{
// Data members -------------------------------------------------------------
protected:
	// ... Note : This protected data must be the first member
	LONGLONG m_nDecimal;

public:
	static const LONGLONG m_nMinNumber;
	// --- The minimum (negative) value of this number object
	static const LONGLONG m_nMaxNumber;
	// --- The maximum (positive) value of this number object

	static const int m_nMinRadix;
	// --- The minimum valid radix of this number object
	static const int m_nMaxRadix;
	// --- The maximum valid radix of this number object

	static const int m_nMaxStringNumberLength;
	// --- The maximum length of a string representation of this object
	//     (including terminating zero)

	static TCHAR m_cDefaultSeparator;
	// --- The default (thousand) separator

protected:
	static LPCTSTR m_pszDigits;
	static LPCTSTR m_pszMinusDigits;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXInteger(LONGLONG nNumber = 0);
	// --- In  : nNumber : The (decimal) number value
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	COXInteger(LPCTSTR pszNumber, int nRadix = 10);
	// --- In  : pszNumber : The text representation of the number value
	//			 nRadix : the radix of the representation
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	CString GetStringNumber(int nRadix = 10, BOOL bSeparated = FALSE, 
		TCHAR cSeparator = _T('\0'), int nGroupLength = 3) const;
	// --- In  : nRadix : The radix of the requested representation
	//			 bSeparated : Whether a (thousand) separator should be used
	//			 cSeparator : The character used to separate, when _T('\0') is specified
	//						  the default thousand separator will be used
	//			 nGroupLength : The length of a groupthat can be separated.
	// --- Out : 
	// --- Returns : The text representation of this number (or empty otherwise)
	// --- Effect : 

	void SetStringNumber(LPCTSTR pszNumber, int nRadix = 10);
	// --- In  : pszNumber : The text representation of the number value
	//			 nRadix : the radix of the representation
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sets a new number value
	//				Invalid charaters will be skipped

	LONGLONG GetNumber() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The (decimal) number value of this object
	// --- Effect : 

	void SetNumber(LONGLONG nNumber);
	// --- In  : nNumber : The new (decimal) number value for this object
	// --- Out : 
	// --- Returns : 
	// --- Effect : 

	operator LONGLONG() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The (decimal) number value of this object
	// --- Effect : Conversion operator

	void Empty();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Assigns 0 to this object

	BOOL IsEmpty() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the current number value of this object is 0 or not
	// --- Effect : 

	virtual ~COXInteger();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of the object

	static CString ThousandSeparated(LPCTSTR pszOriginalNumberText, 
		TCHAR cSeparator = _T('\0'), int nGroupLength = 3);
	// --- In  : pszOriginalNumberText : The number text into which separator characters should be inserted
	//			 cSeparator : The separator character (or _T('\0') for default thousand separator)
	//			 nGroupLength : The length (in characters) of a group (e.g. 3 for thousand)
	// --- Out : 
	// --- Returns : The separated number representation
	// --- Effect : This function inserts a separator for each group of the specified
	//				length.  Groups are forms from the end to the beginning of the string

protected:

private:
                   
};

// Include inline functions
#include "OXInteger.inl"

#endif // __OXINTEGER_H__
// ==========================================================================
