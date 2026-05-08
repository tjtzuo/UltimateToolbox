// ==========================================================================
// 						Class Specification : COXString
// ==========================================================================

// Header file : xstring.h

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
//	YES	Derived from CString

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	YES	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		Here some usefulle extensions to the CString class are made available                                              

// Remark:
//		A "white space" character is a character in the range 0x09 - 0x0D or 0x20.
//			0x09 : HT : Horizontal Tab	(\t)
//			0x0A : LF : Linefeed        (part of \n)
//			0x0B : VT : Vertical Tab    (\v)
//			0x0C : FF : Formfeed		(\f)
//			0x0D : CR : Carriage return (part of \n)
//			0x20 : space
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////

#ifndef __XSTRING_H__
#define __XSTRING_H__ 

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef WIN32
//	#include "oxtchar.h"
#endif


class OX_CLASS_DECL COXString : public CString
{
// Data members -------------------------------------------------------------
public:
protected:
	static TCHAR cDecimalCharacter;
private:
	
// Member functions ---------------------------------------------------------
public:
	COXString();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	COXString::COXString(LPCTSTR psz);
	// --- In  : psz
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object out of a pointer to LPCSTR
	//				(conversion operator)
	
	COXString(const CString& sSource);
	// --- In  : sSource
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object out of a CString object
	//				(conversion operator)

	COXString(LONG nSource);
	// --- In  : nSource
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object out of an int
	//				(conversion operator)

	COXString(const COXString& sString);
	// --- In  : xString : the COXString object to be copied
	// --- Out : 
	// --- Returns : 
	// --- Effect : this is the copy constructor

	COXString& operator=(const COXString& sString);
    // --- In  : xString : the COXString object to be copied
	// --- Out : 
	// --- Returns : 
	// --- Effect : this is the assignment operator

	int GetInt() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The numeric value represented in the string.
	// --- Effect :  The conversion stops at the first character it does not 
	//				 recognize as part of the number

	long GetLongInt() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The numeric value represented in the string.
	// --- Effect :  The conversion stops at the first character it does not 
	//				 recognize as part of the number

	void LTrim();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Removes leading white spaces

	void RTrim();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Removes trailing white spaces

	void XTrim();
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Replaces multiple white space characters by one space character

	BOOL IsInt();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the string contains only digits (TRUE) or not (FALSE)
	//				 Digits are the characters 0 ... 9
	// --- Effect :  
	
	BOOL IsNumber();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the string represents a valid number
	// --- Effect :  A valid number has the syntax :
	// 		[whitespace] [sign] [digits] [.digits] [ {d | D | e | E}[sign]digits] [whitespace]
	// 		A whitespace consists of space and tab characters, which are ignored; 
	// 		sign is either plus (+) or minus (-); and digits are one or more 
	// 		decimal digits. If no digits appear before the decimal point, 
	// 		at least one must appear after the decimal point. 
	// 		The decimal digits can be followed by an exponent, which consists of 
	// 		an introductory letter (b, D, e, or E) and an optionally signed decimal integer.
	
	void Format(LPCTSTR pszFormat, LPCTSTR* rglpsz, int nString);
	// --- In  :pszFormat : The format specifier containing %1, %2 etc
	//			rglpsz : Pointer to array of strings that must replace %1 etc
	//			nString : The number of strings in the array
	// --- Out : 
	// --- Returns :
	// --- Effect : Substitues all occurences of %1, %2 etc by the corresponding string
	//				in the array
	
	void BarToNull();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Replaces all the vertical bars ('|') in this string
	//              by NULL-characters
	//				The length of the string stay the same
	//				Because there are NULL-characters embedded within the string
	//              now, be careful when using certain functions

	virtual ~COXString();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:

private:

};

#endif
// ==========================================================================
