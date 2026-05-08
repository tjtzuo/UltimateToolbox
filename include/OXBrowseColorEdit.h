// ==========================================================================
//							Class Specification : 
//				COXBaseBrowseColorEdit & COXBrowseColorEdit
// ==========================================================================

// Header file : OXBrowseColorEdit.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.

// //////////////////////////////////////////////////////////////////////////


/*

DESCRIPTION


COXBaseBrowseColorEdit is the template class derived from COXDropEdit, which in its
turn is the  template class that provides the functionality of adding dropdown button
to any edit control. Dropdown button is displayed on the right or left side of the 
edit box and takes all height of the control window. The width of the dropdown button 
can be set programmatically (by default we use the width of the current width of 
the scroll bar). 

COXBaseBrowseColorEdit class provides the implementation of the dropdown event that 
happens when the dropdown button is clicked. This includes displaying the Color Picker
popup window that implemented using our COXColorPickerCtrl class. The appearance
of the Color Picker can be defined at the construction time (refer to the description
of the COXColorPickerCtrl constructor) or at any time by retrieving the pointewr to
COXColorPickerCtrl object using the following function:

	GetColorPickerCtrl();

The color that is picked in the Color Picker popup window is saved and can be retrieved
or programmtically set using these functions:

	GetColor();
	SetColor();


These two helper functions allow you to set/retrieve the default color for the
Color Picker popup window:

	SetDefaultColor();
	GetDefaultColor();


And, finally, there are two protected virtual functions that can be overridden 
in order to control the process of setting new color to the object and retrieving
the name of the color:

	virtual void OnSetColor(COLORREF clr);
	virtual CString GetColorText(COLORREF clr);


As you can see COXBaseBrowseColorEdit is the template class that allows us to use
any edit control class as the base one. We provide our own COXBaseBrowseColorEdit
derivation that uses our COXEdit class as the base edit control class. The class
is named as COXBrowseColorEdit and declared as following:

	class OX_CLASS_DECL COXBrowseColorEdit : 
		public COXBaseBrowseColorEdit<COXEdit>
	{
	protected:
		// called every time new color is set
		virtual void OnSetColor(COLORREF clr);
	};

We chose COXEdit because it allows us to display the chosen color using it as background.
  


Take a look at the ExtendedEditControls sample that can be found in the 
.\Samples\gui\ExtendedEditControls subdirectory of your Ultimate Toolbox directory.
In the sample check out the 'Drop Edit' page that demonstrate the use of 
COXBrowseColorEdit class.


Dependencies:
	#include "OXBrowseColorEdit.h"

Source:
	"OXBrowseColorEdit.cpp"	-	COXBrowseColorEdit implementation
	"OXEdit.cpp"			-	COXEdit implementation
	"OXColorPickerCtrl.cpp"	-	COXColorPickerCtrl implementation
	"OXPopupBarCtrl.cpp"	-	COXPopupBarCtrl implementation (COXColorPickerCtrl parent)

*/

#if !defined(_OXBROWSECOLOREDIT_H__)
#define _OXBROWSECOLOREDIT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"
#include "OXMainRes.h"

#include "OXEdit.h"
#include "OXDropEdit.h"
#include "OXColorPickerCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// COXBaseBrowseColorEdit window

template<class PARENTEDIT>
class COXBaseBrowseColorEdit : public COXDropEdit<PARENTEDIT>
{
// Construction
public:
	// --- In  :	nButtons			-	number of buttons in the popup bar (doesn't 
	//										include	default and custom buttons)
	//				nRows				-	number of button rows (cannot be more than the 
	//										number of buttons)
	//				dwDefault			-	value to be associated with default button
	//				sizeButton			-	size of buttons (not applicable for default and 
	//										custom buttons their size are calculated 
	//										automatically and depend on the size of 
	//										Popup Bar window)
	//				sDefaultButtonText	-	text to be drawn on default button
	//				sCustomButtonText	-	text to be drawn on custom button
	//				
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object. All the arguments used to initiate 
	//				COXColorPickerCtrl control
	COXBaseBrowseColorEdit(UINT nColors=40, UINT nRows=4, 
		COLORREF m_clrDefault=ID_CLRPICK_COLOR_NONE,
		CSize sizeButton=CSize(18,18),
		int nIDdefault=IDS_OX_BROWSECLRDEFAULT, //"Default"
		int nIDcustom=IDS_OX_BROWSECLRCUSTOM)  //"Custom..."
		: 
	m_colorPicker(nColors, nRows, m_clrDefault, sizeButton, 
		nIDdefault, nIDcustom) { m_clr=m_clrDefault; }

// Attributes
public:
	
protected:
	// color associated with the control
	COLORREF m_clr;
	// color picker control used to pick color
	COXColorPickerCtrl m_colorPicker;

// Operations
public:

	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to COXColorPickerCtrl object associated with the control
	// --- Effect : Retrieves the pointer to internally used Color Picker control
	inline COXColorPickerCtrl* GetColorPickerCtrl() { return &m_colorPicker; }


	// --- In  :	clr			-	color to be associated with the control
	//				bRedraw		-	if TRUE then control will be redrawn
	// --- Out :	
	// --- Returns:	
	// --- Effect : Associates the control with the color
	void SetColor(COLORREF clr);

	// --- In  :	
	// --- Out :	
	// --- Returns:	The color associated with the control
	// --- Effect : Retreieves the color associated with the control
	inline COLORREF GetColor() const { return m_clr; }


	// --- In  :	clrDefault	-	color that will be used in the associated 
	//								COXColorPickerCtrl as default
	// --- Out :	
	// --- Returns:	
	// --- Effect : Sets default color of associated COXColorPickerCtrl object
	inline void SetDefaultColor(COLORREF clrDefault) { 
		m_colorPicker.SetDefaultData(clrDefault); 
	}

	// --- In  :	
	// --- Out :	
	// --- Returns:	Default color of associated COXColorPickerCtrl object
	// --- Effect : Retrieves the default color of associated COXColorPickerCtrl object
	inline COLORREF GetDefaultColor() const { 
		return m_colorPicker.GetDefaultData(); 
	}

// Implementation
public:
	// --- In     :
	// --- Out    :
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBaseBrowseColorEdit() {};

protected:
	// initialize control
	virtual BOOL InitializeDropEdit();

	// called every time dropdown button is pressed
	virtual void OnDropButton();

	// called every time new color is set
	virtual void OnSetColor(COLORREF clr) {
		UNREFERENCED_PARAMETER(clr);
	};

	// function called any time when the text representation of the currently 
	// selected color is requested
	virtual CString GetColorText(COLORREF clr);
};

/////////////////////////////////////////////////////////////////////////////

template<class PARENTEDIT>
void COXBaseBrowseColorEdit<PARENTEDIT>::SetColor(COLORREF clr) 
{ 
	m_clr=clr;
	OnSetColor(clr);
}


template<class PARENTEDIT>
CString COXBaseBrowseColorEdit<PARENTEDIT>::GetColorText(COLORREF clr)
{
	CString sText(_T(""));
	if(clr==ID_CLRPICK_COLOR_NONE)
	{
		sText.LoadString(IDS_OX_BROWSECLRDEFAULT);
	}
	else
	{
		if(::IsWindow(m_colorPicker.GetSafeHwnd()))
		{
			ButtonToolTipMap* pToolTipMap=m_colorPicker.GetToolTipTextTable();
			if(pToolTipMap!=NULL)
			{
				CString sToolTipText;
				if(pToolTipMap->Lookup(clr,sToolTipText))
					sText=sToolTipText;
			}
		}
	}

	CString sColorDescription;
	sColorDescription.Format(/*_T("R:%d G:%d B:%d")*/IDS_OX_BROWSECLRDESCRIPTION,GetRValue(m_clr),
		GetGValue(m_clr),GetBValue(m_clr));

	if(!sText.IsEmpty())
		sText=sText+_T(" ");
	sText=sText+sColorDescription;

	return sText;
}


template<class PARENTEDIT>
BOOL COXBaseBrowseColorEdit<PARENTEDIT>::InitializeDropEdit()
{
	if(!COXDropEdit<PARENTEDIT>::InitializeDropEdit())
		return FALSE;

	ASSERT(!::IsWindow(m_colorPicker.m_hWnd));
	if(!m_colorPicker.Create(this))
	{
		TRACE(_T("COXBaseBrowseColorEdit::Initialize: Creation of COXColorPickerCtrl failed\n"));
		return FALSE;
	}

	SetColor(m_clr);

	return TRUE;
}


template<class PARENTEDIT>
void COXBaseBrowseColorEdit<PARENTEDIT>::OnDropButton()
{
	ASSERT(::IsWindow(m_colorPicker.m_hWnd));

	m_colorPicker.SetSelectedColor(m_clr);
	if(m_colorPicker.Pick((GetButtonAlignment()==OXDROPEDIT_BUTTONLEFT ? 
		ID_POPUPBAR_ALIGNBOTTOM : ID_POPUPBAR_ALIGNBOTTOMRIGHT)))
	{
		SetColor(m_colorPicker.GetSelectedColor());
	}
	SetFocus();
}

/////////////////////////////////////////////////////////////////////////////


class OX_CLASS_DECL COXBrowseColorEdit : public COXBaseBrowseColorEdit<COXEdit>
{
	DECLARE_DYNCREATE(COXBrowseColorEdit)
public:
	COXBrowseColorEdit()
	{
		m_bSetColor = FALSE;
	}
protected:
	// called every time new color is set
	virtual void OnSetColor(COLORREF clr);
public:

	// --- In  :  bShow - Determines if the backgrond of the control should reflect
	//						the selected color.
	// --- Out : 
	// --- Returns:	
	// --- Effect : Call this function and pass TRUE if you want the background
	//				reflecting the selected color.
	void ShowColorInBackground(BOOL bShow = TRUE);

private:
	BOOL m_bSetColor;
};


// The following function was introduced in order to be specifically used in 
// DoDataExchange function of any Dialog or FormView based application for 
// Browse Color Edit controls. 
OX_API_DECL void AFXAPI DDX_EditColorPicker(CDataExchange *pDX, int nIDC, COLORREF& clr);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif / !defined(_OXBROWSECOLOREDIT_H__)
