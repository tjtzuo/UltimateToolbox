// ==========================================================================
//			Class Specification: COXShortkeysOrganizer
// ==========================================================================

// Header file : OXShortkeysOrganizer.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

/*
	Overview

COXShortkeysOrganizer is designed to simplify the process of updating at the run-time 
the contents of accelerator tables assigned to an application. Usually accelerator 
tables are static and defined in the resource file. In document/view architecture 
compatible application there could be multiple accelerator tables defined (as many as 
one table for every document template plus default accelerator table). In MFC 
accelerator tables are managed internally. That adds new level of complexity to the 
task of updating accelerator tables at run-time.

COXShortkeysOrganizer hides all these details and gives you an easy interface for 
working with accelerator tables. Follow these steps in order to update accelerators
for any SDI/MDI application:

1)	Attach main frame window of your application to COXShortkeysOrganizer object
by calling:

	BOOL Attach(CFrameWnd* pFrameWnd, BOOL bRetrieveAccelerators=FALSE);

If bRetrieveAccelerators was set to TRUE then go to step 3

2)	If Attach() function was called with second argument set to FALSE then before 
modifying any accelerator you have to retrieve information about the current 
accelerators associated with attached application commands by calling:

	BOOL RetrieveAccelerators();

3)	At this moment the current accelerators associated with attached application 
commands has been saved to internal member variables of this class. Now you can do 
the following operations on them:

	1.	Add new accelerator:

		BOOL AddAccelerator(BYTE fVirt, WORD key, WORD cmd, 
							CMultiDocTemplate* pDocTemplate, 
							BOOL bIgnoreConflict=FALSE);
		BOOL AddAccelerator(ACCEL* pAccel, CMultiDocTemplate* pDocTemplate, 
							BOOL bIgnoreConflict=FALSE);
		
	2.	Delete existing accelerator:

		BOOL RemoveAccelerator(BYTE fVirt, WORD key, WORD cmd, 
							   CMultiDocTemplate* pDocTemplate);
		BOOL RemoveAccelerator(ACCEL* pAccel, CMultiDocTemplate* pDocTemplate);
		BOOL RemoveAllAccelerators(WORD cmd, CMultiDocTemplate* pDocTemplate);

	3.	Search for an accelerator or acelerators:

		COXArrAccelerators* FindAcceleratorTable(CMultiDocTemplate* pDocTemplate);
		BOOL FindAllAccelerators(BYTE fVirt, WORD key, COXArrAccelerators& arrAccels);
		BOOL FindAllAccelerators(WORD cmd, CMultiDocTemplate* pDocTemplate, 
								 COXArrAccelerators& arrAccels);
		WORD FindAccelerator(BYTE fVirt, WORD key, CMultiDocTemplate* pDocTemplate);

	4.	Serialize the contents of accelerator to or from registry or any 
		CArchive-derived object:

	    virtual void Serialize(CArchive& ar);
		BOOL SaveState(LPCTSTR lpszProfileName=_T("ShortkeysOrganizerState"));
		BOOL LoadState(LPCTSTR lpszProfileName=_T("ShortkeysOrganizerState"),
					   BOOL bApply=TRUE);


4)	In order to apply any changes made to accelerator tables the following function
must be called:

	BOOL SetAccelerators();


These four steps defines the general routine for updating accelerator tables. We use
this object in our Customize Manager Framework for managing an application's 
accelerator tables. The page that provides this functionality is implemented through
COXCustomizeShortkeysPage class. Take look at this class source code - it is a good 
example on how you can use the COXShortkeysOrganizer class functionality.


We don't have a specific sample that demonstrate COXShortkeysOrganizer class 
functionality. But, as we mentioned above, COXCustomizeShortkeysPage is a good 
example itself. Take look at the VisualStudioLikeDemo sample that can be found in the 
.\Samples\Advanced\VisualStudioLikeDemo subdirectory of your Ultimate Toolbox 
directory. This sample demonstrates our Customize manager that include Customize
Workspace State Page. In the sample menu choose "View"-"Customize" in order to 
display the Customize manager window. 



Dependency:
	#include "OXShortkeysOrganizer.h"


Source code files:
	"OXShortkeysOrganizer.cpp"		

*/


#if !defined(_OXSHORTKEYSORGANIZER_H__)
#define _OXSHORTKEYSORGANIZER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif //__AFXTEMPL_H__


// name og the default accelerator table
#ifndef OXDEFAULTACCEL
#define OXDEFAULTACCEL _T("Main")
#endif	//	OXDEFAULTACCEL

// array of accelerators (shortkeys)
typedef CArray<ACCEL,ACCEL&> COXArrAccelerators;


// OXACCELTABLE defines accelerator table associated with a document template
// (if document template is not defined then this structure defines default 
// accelerator table)
//
typedef struct _tagOXACCELTABLE
{
	// array of accelerators
	COXArrAccelerators m_accelerators;
	// pointer to a document template associated with these accelerators
	CMultiDocTemplate* m_docTemplate;

	// default constructor
	_tagOXACCELTABLE() : m_docTemplate(NULL) {};

	// copy constructor
	_tagOXACCELTABLE(const _tagOXACCELTABLE& accelTable)
	{
		VERIFY(m_accelerators.Append(accelTable.m_accelerators)==0);
		m_docTemplate=accelTable.m_docTemplate;
	}

	// assignment operator
	_tagOXACCELTABLE& operator=(const _tagOXACCELTABLE& accelTable)
	{
		if(this==&accelTable)
			return *this;

		m_accelerators.RemoveAll();
		VERIFY(m_accelerators.Append(accelTable.m_accelerators)==0);
		m_docTemplate=accelTable.m_docTemplate;

		return *this;
	}

} OXACCELTABLE;


// array of accelerator tables
typedef CArray<OXACCELTABLE,OXACCELTABLE&> COXArrAccelTables;


class OX_CLASS_DECL COXShortkeysOrganizer : public CObject  
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXShortkeysOrganizer();

// Attributes
protected:
	// pointer to main frame window of the application attached to the organizer
	CFrameWnd* m_pFrameWnd;
	// array of all accelerator tables 
	COXArrAccelTables m_arrAccelTables;
	// array of all internally created accelerators
	CArray<HACCEL,HACCEL> m_arrCreatedAccel;
	// array of all removed accelerators. Use RemoveAccelerator() or 
	// RemoveAllAccelerators() functions to remove accelerators. Apply any 
	// chnages made to accelerator tables by calling SetAccelerators()
	// function. After this function is called this array will be emptied 
	COXArrAccelTables m_arrRemovedAccelTables;

// Operations
public:
	// --- In  :	pFrameWnd				-	a pointer to the main frame window 
	//											of the application, which accelerators 
	//											will be managed by this object
	//				bRetrieveAccelerators	-	if TRUE then current accelerators 
	//											will be read and saved in internal 
	//											members. Before adding or removing any 
	//											accelerator the existing accelerators 
	//											must be retrieved. You can do it in 
	//											this function or later by calling 
	//											RetrieveAccelerators() function
	// --- Out : 
	// --- Returns: TRUE if main frame window was successfully attached and, 
	//				if specified, the existing accelerators is retrieved; 
	//				otherwise FALSE 
	// --- Effect : Attaches the main frame window of the application, which 
	//				accelerators will be managed by this object
	virtual BOOL Attach(CFrameWnd* pFrameWnd, BOOL bRetrieveAccelerators=FALSE);

	// --- In  :
	// --- Out : 
	// --- Returns:	A pointer to attached main frame window
	// --- Effect : Retrieves a pointer to attached main frame window
	inline CFrameWnd* GetAttached() const { return m_pFrameWnd; }

	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if main frame window is attached to the object;
	//				FALSE otherwise
	// --- Effect : Retrieves a flag that specifies if main frame window is 
	//				attached to the object
	inline BOOL IsAttached() const { return (m_pFrameWnd!=NULL); }
	

	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if succeed; FALSE otherwise
	// --- Effect : Detaches any attached main frame window.
	virtual BOOL Detach();


	// --- In  :
	// --- Out : 
	// --- Returns:	A pointer to internal array that represents all 
	//				application's accelerators
	// --- Effect : Retrieves a pointer to internal array that represents all 
	//				application's accelerators. Any changes made to this array
	//				will be applied only after SetAccelerator() function is called
	inline COXArrAccelTables* GetAccelTables() { return &m_arrAccelTables; }


	// --- In  :
	// --- Out : 
	// --- Returns:	A pointer to internal array that represents all 
	//				application's accelerators that has been removed using
	//				RemoveAccelerator() or RemoveAllAccelerators() functions.
	// --- Effect : Retrieves a pointer to internal array that represents all 
	//				application's accelerators that has been removed using
	//				RemoveAccelerator() or RemoveAllAccelerators() functions. 
	//				This array will be emptied after SetAccelerator() function 
	//				is called
	inline COXArrAccelTables* GetRemovedAccelTables() 
	{ 
		return &m_arrRemovedAccelTables; 
	}


	// --- In  :	fVirt			-	Specifies the accelerator flags. This member 
	//									can be a combination of the following values: 
	//							FALT		-	The alt key must be held down when the 
	//											accelerator key is pressed. 
	//							FCONTROL	-	The ctrl key must be held down when 
	//											the accelerator key is pressed. 
	//							FNOINVERT	-	Specifies that no top-level menu item 
	//											is highlighted when the accelerator 
	//											is used. If this flag is not 
	//											specified, a top-level menu item will 
	//											be highlighted, if possible, when the 
	//											accelerator is used. 
	//							FSHIFT		-	The shift key must be held down when 
	//											the accelerator key is pressed. 
	//							FVIRTKEY	-	The key member specifies a virtual-key 
	//											code. If this flag is not specified, 
	//											key is assumed to specify an ASCII 
	//											character code. 
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				cmd				-	Specifies the accelerator identifier. This 
	//									value is placed in the low-order word of the 
	//									wParam parameter of the WM_COMMAND or 
	//									WM_SYSCOMMAND message when the accelerator is 
	//									pressed
	//				pAccel			-	a pointer to ACCEL structure that represents 
	//									the new accelerator
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator will be associated.
	//									If NULL is specified the accelerator will be
	//									to default accelerator table
	//				bIgnoreConflict	-	if TRUE then in the case specified key 
	//									combination has already been assigned to a 
	//									different command then conflicting accelerator
	//									will be removed and new one will be added;
	//									otherwise in the situation like this the 
	//									function will fail and return FALSE
	// --- Out : 
	// --- Returns: TRUE if new accelerator was successfully added;
	//				FALSE otherwise
	// --- Effect : Adds new accelerator. New accelerator will be activated only
	//				after SetAccelerators() function is called
	BOOL AddAccelerator(BYTE fVirt, WORD key, WORD cmd, 
		CMultiDocTemplate* pDocTemplate, BOOL bIgnoreConflict=FALSE);
	BOOL AddAccelerator(ACCEL* pAccel, CMultiDocTemplate* pDocTemplate, 
		BOOL bIgnoreConflict=FALSE);


	// --- In  :	fVirt			-	Specifies the accelerator flags. Refer to
	//									documentation on AddAccelerator() function 
	//									for details on possible values this argument
	//									can take
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				cmd				-	Specifies the accelerator identifier. This 
	//									value is placed in the low-order word of the 
	//									wParam parameter of the WM_COMMAND or 
	//									WM_SYSCOMMAND message when the accelerator is 
	//									pressed
	//				pAccel			-	a pointer to ACCEL structure that represents 
	//									the accelerator to be deleted
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator is associated.
	//									if NULL is specified the default accelerator 
	//									table will be used
	// --- Out : 
	// --- Returns: TRUE if specified accelerator was successfully deleted;
	//				FALSE otherwise
	// --- Effect : Deletes existing accelerator. Note that the changes in application
	//				accelerator tables will be applied only after SetAccelerators() 
	//				function is called
	BOOL RemoveAccelerator(BYTE fVirt, WORD key, WORD cmd, 
		CMultiDocTemplate* pDocTemplate);
	BOOL RemoveAccelerator(ACCEL* pAccel, CMultiDocTemplate* pDocTemplate);


	// --- In  :	cmd				-	Specifies the accelerator identifier. This 
	//									value is placed in the low-order word of the 
	//									wParam parameter of the WM_COMMAND or 
	//									WM_SYSCOMMAND message when the accelerator is 
	//									pressed. Any accelerator assigned to this ID
	//									will be removed
	//				pDocTemplate	-	pointer to application's document template
	//									for which the accelerator for specified 
	//									command is associated. If NULL is specified 
	//									the default accelerator table is assumed
	// --- Out : 
	// --- Returns: TRUE if accelerators for specified command were successfully 
	//				deleted; FALSE otherwise
	// --- Effect : Deletes existing accelerators for specified command. Note that 
	//				the changes in application accelerator tables will be applied 
	//				only after SetAccelerators() function is called
	BOOL RemoveAllAccelerators(WORD cmd, CMultiDocTemplate* pDocTemplate);


	// --- In  :	pDocTemplate	-	pointer to application's document template
	//									for which array of accelerator will be 
	//									retrieved
	// --- Out : 
	// --- Returns: A pointer to an array of accelerators defined for the specified 
	//				document template if succeed; otherwise NULL
	// --- Effect : Finds accelerators for specified document template.
	COXArrAccelerators* FindAcceleratorTable(CMultiDocTemplate* pDocTemplate);


	// --- In  :	fVirt			-	Specifies the accelerator flags. Refer to
	//									documentation on AddAccelerator() function 
	//									for details on possible values this argument
	//									can take
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				cmd				-	Specifies the accelerator identifier. This 
	//									value is placed in the low-order word of the 
	//									wParam parameter of the WM_COMMAND or 
	//									WM_SYSCOMMAND message when the accelerator is 
	//									pressed
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator is associated.
	//									If NULL is specified the default accelerator 
	//									table is assumed
	// --- Out :	arrAccels		-	array for found accelerators
	// --- Returns: TRUE if accelerators that match specified arguments are found and
	//				copied into arrAccels argument; FALSE otherwise
	// --- Effect : Finds accelerators for specified document template that match 
	//				specified arguments. First version of this function searches
	//				all accelerator tables associated with different document 
	//				templates for accelerators that have the same key combination.
	//				The second version retrieves all accelerators found in the
	//				accelerator table associated with specified document template 
	//				that have the same command ID
	BOOL FindAllAccelerators(BYTE fVirt, WORD key, COXArrAccelerators& arrAccels);
	BOOL FindAllAccelerators(WORD cmd, CMultiDocTemplate* pDocTemplate, 
		COXArrAccelerators& arrAccels);
	

	// --- In  :	fVirt			-	Specifies the accelerator flags. Refer to
	//									documentation on AddAccelerator() function 
	//									for details on possible values this argument
	//									can take
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator is associated.
	//									If NULL is specified the default accelerator 
	//									table is assumed
	// --- Out :	
	// --- Returns: Command ID associated with the found accelerator if succeed;
	//				otherwise 0
	// --- Effect : Finds accelerator for specified document template that match 
	//				specified arguments.
	WORD FindAccelerator(BYTE fVirt, WORD key, CMultiDocTemplate* pDocTemplate);

	
	// --- In  :	pDocTemplate	-	pointer to application's document template
	//									for which array of removed accelerators 
	//									will be retrieved
	// --- Out : 
	// --- Returns: A pointer to an array of deleted accelerators defined for the 
	//				specified document template if succeed; otherwise NULL
	// --- Effect : Finds deleted accelerators for specified document template.
	//				Accelerators can be deleted using RemoveAccelerator() and
	//				RemoveAllAccelerators() functions but we keep track of such 
	//				items and save them into internal member variables. Information
	//				about deleted accelerators is valid only until SetAccelerators()
	//				function is called. After this function is called these internal
	//				member variables will be emptied
	COXArrAccelerators* FindRemovedAcceleratorTable(CMultiDocTemplate* pDocTemplate);
	
	// --- In  :	fVirt			-	Specifies the accelerator flags. Refer to
	//									documentation on AddAccelerator() function 
	//									for details on possible values this argument
	//									can take
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				cmd				-	Specifies the accelerator identifier. This 
	//									value is placed in the low-order word of the 
	//									wParam parameter of the WM_COMMAND or 
	//									WM_SYSCOMMAND message when the accelerator is 
	//									pressed
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator is associated.
	//									If NULL is specified the default accelerator 
	//									table is assumed
	// --- Out :	arrAccels		-	array for found deleted accelerators
	// --- Returns: TRUE if deleted accelerators that match specified arguments are 
	//				found and copied into arrAccels argument; FALSE otherwise
	// --- Effect : Finds deleted accelerators for specified document template that
	//				match specified arguments. First version of this function searches
	//				all accelerator tables associated with different document 
	//				templates for deleted accelerators that have the same key 
	//				combination. The second version retrieves all accelerators found 
	//				in the accelerator table associated with specified document 
	//				template that have the same command ID
	BOOL FindAllRemovedAccelerators(BYTE fVirt, WORD key, 
		COXArrAccelerators& arrAccels);
	BOOL FindAllRemovedAccelerators(WORD cmd, CMultiDocTemplate* pDocTemplate, 
		COXArrAccelerators& arrAccels);
	
	// --- In  :	fVirt			-	Specifies the accelerator flags. Refer to
	//									documentation on AddAccelerator() function 
	//									for details on possible values this argument
	//									can take
	//				key				-	Specifies the accelerator key. This member 
	//									can be either a virtual-key code or an ASCII 
	//									character code
	//				pDocTemplate	-	pointer to application's document template
	//									with which this accelerator is associated.
	//									If NULL is specified the default accelerator 
	//									table is assumed
	// --- Out :	
	// --- Returns: Command ID associated with the found deleted accelerator 
	//				if succeed;	otherwise 0
	// --- Effect : Finds deleted accelerator for specified document template that 
	//				match specified arguments.
	WORD FindRemovedAccelerator(BYTE fVirt, WORD key, CMultiDocTemplate* pDocTemplate);

	
	// --- In  :
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Retrieves current accelerators in the attached application and
	//				save them into internal member variables
	BOOL RetrieveAccelerators();
	

	// --- In  :
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Builds accelerator tables on the base of the internal member 
	//				variables and applies them to the attached application
	BOOL SetAccelerators();

	
	// --- In  :	lpszProfileName	-	the name of the hive in the registry where 
	//									current information about accelerators will
	//									be saved
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Saves in the registry current information about accelerators
	BOOL SaveState(LPCTSTR lpszProfileName=_T("ShortkeysOrganizerState"));

	// --- In  :	lpszProfileName	-	the name of the hive in the registry where 
	//									information about accelerators was saved
	//				bApply			-	if TRUE then retrieved accelerators will
	//									be applied to the attached application 
	//									right away by calling SetAccelerators()
	//									function
	// --- Out : 
	// --- Returns: TRUE if succeed; otherwise FALSE
	// --- Effect : Retrieves from the registry saved information about accelerators
	//				and, if specified, applies it by calling SetAccelerators() function
	BOOL LoadState(LPCTSTR lpszProfileName=_T("ShortkeysOrganizerState"),
		BOOL bApply=TRUE);


public:  
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXShortkeysOrganizer();


	// --- In  :	ar	-	A CArchive object to serialize to or from
	// --- Out : 
	// --- Returns:
	// --- Effect : Serializes the contents of the accelerators to or 
	//				from a CArchive object
    virtual void Serialize(CArchive& ar);


public:
	// --- In  :	pAccel	-	pointer to ACCEL structure that defines an accelerator
	// --- Out : 
	// --- Returns: A string that represents accelerator's key combination
	// --- Effect : Retrieves a string that represents accelerator's key combination
	static CString GetAccelKeyString(ACCEL* pAccel);

	// --- In  :	lpszString	-	a string that represents accelerator's key 
	//								combination
	// --- Out :	pAccel		-	pointer to ACCEL structure which fVirt and key
	//								element will be populated with data retrieved 
	//								from lpszString argument
	// --- Returns: TRUE if succeed; FALSE otherwise
	// --- Effect : Converts a string that represents accelerator's key combination
	//				into accelerator. Note that cmd element of ACCEL structure 
	//				will not be populated
	static BOOL GetAccelKeyFromString(LPCTSTR lpszString, ACCEL* pAccel);


protected:
	// reads accelerator table associated with the specified document template
	BOOL AddAccelTable(HACCEL hAccelTable, CMultiDocTemplate* pDocTemplate);
	// build accelerator table for specified document template on the base 
	// of internal member variables
	HACCEL BuildAccelTable(CMultiDocTemplate* pDocTemplate);
	// serializes the contents of the specifyed array of accelerators to or from 
	// a CArchive object
	virtual void SerializeAccelTable(CArchive& ar, OXACCELTABLE* pAccelTable);
};

#endif // !defined(_OXSHORTKEYSORGANIZER_H__)
