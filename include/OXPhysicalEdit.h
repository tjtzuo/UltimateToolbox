// ==========================================================================
// 					Class Specification : COXPhysicalEdit
// ==========================================================================

// Header file : OXPhysicalEdit.h

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
//	YES	Derived from CEdit

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	YES	Uses exceptions (CUserException)

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is a base class to encapsulate an edit controls for editing of physical values
//	such as time (COXTimeEdit), angle (COXAngleEdit), length (COXLengthEdit), and
//	temperature (COXTempEdit).
// 
//	Contents of this controls consist of a numerical value and an optional
//	character string describing a physical unit
//	(e.g.: "12 cm", where "12" is the numerical value and "cm" the character
//	string for the unit)
//	member functions allow:
//		setting a value with a specified unit
//		retrieving a value in a specified unit
//		converting values between different units

//	Four classes are derived from this base class.  These implement
//	 length, time, angle and temperature (see OXPhysicalEditEx.h)

//	There are two ways you can use this base class to implement your specific
//  physical edit class
//		1) One way is by defining a static array which contains all the names
//         of the units along with their conversion factors
//		   A pointer to this structor can then be supplied to the constructor of
//		   COXPhysicalEdit.  This is how the 4 supplied classes are implemented
//      2) Another way is to just supply a default unit to the constructor of 
//		   COXPhysicalEdit (nDefaultUnitIndex), in this case you can 
//		   pass NULL for pConversionData
//		   Now you HAVE TO override the functions
//				virtual int GetNumberOfUnits();
//				virtual int GetDefaultUnit();
//				virtual LPCTSTR* GetUnitNames();
//				virtual double Convert(double dValue, int nUnitFrom, int nUnitTo);
//		   This way you can do the conversion in code

// Remark:

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXPHYSICALEDIT_H__
#define __OXPHYSICALEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXEdit.h"
#include "OXDropEdit.h"


#define ID_OXPHYSICALEDIT_INDEXBASE		0x0fff

// The following 3 numbers are used to convert a specic unit (S) to the
// default unit (D) by using the formula : D = (S + m_dPreTerm) * m_dFactor + m_dPostTerm
// E.g. if the default unit is meter and then the conversion paramaters for mm are
//      0 and 0.001 and 0, because 1m = (1mm + 0) * 0.0001 + 0
struct COXConversionParams
{
	// --- Pre multiplication addition
	double m_dPreTerm;
	// --- Multiplcator (must not be 0!)
	double m_dFactor;
	// --- Post multiplication addition
	double m_dPostTerm;
};

struct COXConversionData
{
	// --- Total number of units that can be used
	int						m_nNumberOfUnits;
	// --- The index of the default unit
	int						m_nDefaultUnitIndex;
	// --- Pointer to an array of strings containing the names of the units
	//     (must be at least m_nNumberOfUnits long)
	LPCTSTR*				m_ppszUnitNames;
	// --- Pointer to an array of conversioon paramaters that will be used to convert
	//     (must be at least m_nNumberOfUnits long)
	COXConversionParams*	m_pConversionParams;
	// --- Pointer to a string containing the error message that will be shown
	//     when an unknown unit is encountered (may contain %1 which will 
	//     be substituted by the specific unit name).  May also be a resource ID
	LPCTSTR					m_pszErrorMsg;
};


class OX_CLASS_DECL COXPhysicalEdit: public COXDropEdit<COXNumericEdit>
{
DECLARE_DYNAMIC(COXPhysicalEdit)

// Data members -------------------------------------------------------------
public:

protected:
	int m_nUnitIndex;
	COXConversionData*	m_pConversionData;

	// flag that specifies whether the currently set unit will be displayed
	// right after the value
	BOOL m_bShowUnit;

	// menu with different unit choices
	CMenu m_menu;

private:

// Member functions ---------------------------------------------------------
public:
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Default contructor
	COXPhysicalEdit();

	// --- In  : pConversionData : Pointer to a structure that contains all the 
	//				data to supply a conversion (may be NULL)
	//			 nDefaultUnitIndex : The index of the unit that should be used as default
	// --- Out : 
	// --- Returns :
	// --- Effect : Standard contructor
	COXPhysicalEdit(COXConversionData* pConversionData, int nDefaultUnitIndex = -1,
		BOOL bShowUnit=TRUE);


	// --- In  : dValue : The value to be set
	//			 nUnit : The unit of dValue (one of the predefined units)
	//			 pszFormat : A "printf" like format string (if NULL "%g" will be used)
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets a new unit value of the edit control
	void SetValue(double dValue, int nUnit = -1, LPCTSTR pszFormat = NULL);


	// --- In  : nUnit : The unit in which the value should be retrieved
	//			 bNotify : Whether a messagebox should be shown in case of error
	// --- Out : 
	// --- Returns : The value of the edit control
	// --- Effect : Retrieves the value of the edit control
	//				This function throws a CUserException when the control contains
	//				an unknown unit
	double GetValue(int nUnit = -1, BOOL bNotify = TRUE);

	
	// --- In  : dValue : The value to be converted
	//			 nUnitFrom : The unit of dValue on input (one of the predefined units)
	//			 nUnitTo : The unit of dValue on output (one of the predefined units)
	// --- Out : 
	// --- Returns : The value of the edit control
	// --- Effect : Converts a value bewteen different units
	virtual double Convert(double dValue, int nUnitFrom, int nUnitTo);


	// --- In  : dValue : The value to be converted
	//			 nUnitFrom : The unit of dValue on input (one of the predefined units)
	//			 nUnitTo : The unit of dValue on output (one of the predefined units)
	//			 pConversionParams : The conversion params to use (may not be NULL)
	// --- Out : 
	// --- Returns : The value of the edit control
	// --- Effect : Static function that converts a value bewteen different units
	static double ConversionHelper(double dValue, int nUnitFrom, int nUnitTo, 
		COXConversionParams* pConversionParams);

	// --- In  : 
	// --- Out : 
	// --- Returns : The number of different units supported by this object
	// --- Effect : If you are not using a static table of conversion data
	//				(COXConversionData) this function should be overridden in a derived class
	virtual int	GetNumberOfUnits();


	// --- In  : 
	// --- Out : 
	// --- Returns : The zero-based index of the default unit
	// --- Effect : If you are not using a static table of conversion data
	//				(COXConversionData) this function should be overridden in a derived class
	virtual int	GetDefaultUnit();


	// --- In  : 
	// --- Out : 
	// --- Returns : A pointer to an array of strings containg tne names of the units
	//				 (should be at least GetNumberOfUnits() long)
	// --- Effect : If you are not using a static table of conversion data
	//				(COXConversionData) this function should be overridden in a derived class
	virtual LPCTSTR* GetUnitNames();


	// --- In  : 
	// --- Out : 
	// --- Returns : A pointer to an array of conversion data
	//				 (should be at least GetNumberOfUnits() long)
	// --- Effect : If you are not using a static table of conversion data
	//				(COXConversionData) this function should be overridden in a derived class
	virtual COXConversionParams* GetConversionParams();


	// --- In  : pszUnit : The name of the unit that was not recognized
	// --- Out : 
	// --- Returns : 
	// --- Effect : Shows a message box to the user, warning him that a unknown unit name was encountered
	virtual void ShowErrorMessage(LPCTSTR pszUnit);


	// --- In  :	bShowUnit	-	if TRUE then the name of the unit will be 
	//								shown in edit control
	// --- Out : 
	// --- Returns:	 
	// --- Effect : Shows/hide unit name
	void ShowUnit(BOOL bShowUnit);

	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if the name of the unit will be shown in edit control 
	// --- Effect : retrieves the flag that specifies whether unit name will be
	//				displayed along with value
	inline BOOL GetShowUnit() const { return m_bShowUnit; }


#ifdef _DEBUG
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.
	virtual void AssertValid() const;

	
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 
	virtual void Dump(CDumpContext& dc) const;
#endif

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Standard destructor
	virtual ~COXPhysicalEdit();

// Overrides
	//{{AFX_VIRTUAL(COXPhysicalEdit)
	//}}AFX_VIRTUAL

protected:
	virtual void UpdateMask();

	// Generated message map functions
	//{{AFX_MSG(COXPhysicalEdit)
	//}}AFX_MSG
	afx_msg void OnChangeUnit(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
};


/////////////////////////////////////////////////////////////////////////////
// global public functions
OX_API_DECL void AFXAPI DDX_OXPhysicalEdit(CDataExchange* pDX, int nIDC, 
										   double& dValue, int nUnitIndex);

#endif // __OXPHYSICALEDIT_H__

/////////////////////////////////////////////////////////////////////////////
