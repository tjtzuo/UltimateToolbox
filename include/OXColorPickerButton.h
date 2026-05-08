// ==========================================================================
// 					Class Specification : COXColorPickerButton
// ==========================================================================

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
//	YES	Derived from COXBitmapButton

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         

/*
COXColorPickerButton is derived from COXBitmapButton (with DROPDOWN style predefined)
and uses COXColorPickerCtrl to display Color Picker popup bar. 

You can set any color to COXColorPickerButton using next function:

	void SetColor(COLORREF clr, BOOL bRedraw=TRUE);
	// --- Effect : associate button with color

and this color can be retrieved using:

	COLORREF GetColor();
	// --- Returns:	color associated with button


To display associated color we draw color band in the bottom of button. By default
the height of that band is 5 pixel (if neither image nor text is associated with the 
button then the color band takes all available space). You can set/get the color band 
height using next functions:

	void SetColorBandHeight(UINT nColorBandHeight); 
	// --- Effect : set the height of color band. By default it is set to 5
	UINT GetColorBandHeight();
	// --- Returns:	the height of color band


COXColorPickerCtrl control associated with COXColorPickerButton can be retrieved using:

	COXColorPickerCtrl* GetColorPickerCtrl();
	// --- Returns:	pointer to COXColorPickerCtrl object associated with button


Also we provide some helper functions to set/get default color of associated 
Color Picker control:

	void SetDefaultColor(COLORREF clrDefault) 
	// --- Effect : set default color of associated COXColorPickerCtrl object
	COLORREF GetDefaultColor();
	// --- Returns:	default color of associated COXColorPickerCtrl object


Use COXBitmapButton and COXColorPickerCtrl functions to customize COXColorPickerButton.

In the ColorPickerButton sample which resides in .\Samples\gui\ColorPickerButton 
subdirectory of your Ultimate Toolbox directory you will find an example of customizing
COXColorPickerButton object to full extent.

*/

// Prerequisites (necessary conditions):
/////////////////////////////////////////////////////////////////////////////

#ifndef __OXCOLORPICKERBUTTON_H__
#define __OXCOLORPICKERBUTTON_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXBitmapButton.h"
#include "OXColorPickerCtrl.h"


// The following function was introduced in order to be specifically used in 
// DoDataExchange function of any Dialog or FormView based application for 
// Color Picker buttons. 
OX_API_DECL void AFXAPI DDX_ColorPicker(CDataExchange *pDX, int nIDC, COLORREF& clr);

class OX_CLASS_DECL COXColorPickerButton : public COXBitmapButton
{
DECLARE_DYNAMIC(COXColorPickerButton);

// Data members -------------------------------------------------------------
public:

protected:
	// offset for color band to be drawn on the button
	static CPoint m_ptColorBandOffset;

	// height of color band to be drawn on the button
	UINT m_nColorBandHeight;

	// color associated with the button
	COLORREF m_clr;

	// color picker control used to pick color
	COXColorPickerCtrl m_colorPicker;


// Member functions ---------------------------------------------------------
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
	COXColorPickerButton(UINT nColors=40, UINT nRows=4, 
		COLORREF m_clrDefault=ID_CLRPICK_COLOR_NONE,
		CSize sizeButton=CSize(18,18),
		int nIDdefault=IDS_OX_BROWSECLRDEFAULT,
		int nIDcustom=IDS_OX_BROWSECLRCUSTOM);


	// --- In  :	
	// --- Out : 
	// --- Returns: size of reserved space (from the bottom and right) that shouldn't be
	//				filled withh button's image or text
	// --- Effect : helper function
	virtual CSize GetReservedSpace();


	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructor of the object
	virtual ~COXColorPickerButton();


	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to COXColorPickerCtrl object associated with button
	// --- Effect : 
	inline COXColorPickerCtrl* GetColorPickerCtrl() { return &m_colorPicker; }


	// --- In  :	nColorBandHeight	-	height of color band in pixels
	// --- Out : 
	// --- Returns:	
	// --- Effect : set the height of color band. By default it is set to 5
	inline void SetColorBandHeight(UINT nColorBandHeight) { 
		m_nColorBandHeight=nColorBandHeight; 
	}
	
	// --- In  :
	// --- Out : 
	// --- Returns:	the height of color band
	// --- Effect : 
	inline UINT GetColorBandHeight() const { return m_nColorBandHeight; }


	// --- In  :	clr			-	color to be associated with the button
	//				bRedraw		-	if TRUE then button will be redrawn
	// --- Out :	
	// --- Returns:	
	// --- Effect : associate button with color
	void SetColor(COLORREF clr, BOOL bRedraw=TRUE);

	// --- In  :	
	// --- Out :	
	// --- Returns:	color associated with button
	// --- Effect : 
	inline COLORREF GetColor() { return m_clr; }

	// --- In  :	clrDefault	-	color that will be used in associated 
	//								COXColorPickerCtrl as default
	// --- Out :	
	// --- Returns:	
	// --- Effect : set default color of associated COXColorPickerCtrl object
	inline void SetDefaultColor(COLORREF clrDefault) { 
		m_colorPicker.SetDefaultData(clrDefault); 
	}

	// --- In  :	
	// --- Out :	
	// --- Returns:	default color of associated COXColorPickerCtrl object
	// --- Effect : 
	inline COLORREF GetDefaultColor() const { return m_colorPicker.GetDefaultData(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXColorPickerButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	// we provide our implementation of this virtual function to display
	// COXColorPickerCtrl
	virtual void OnDropDown();

	// virtual functions that draws color band
	virtual void DrawColorBand(CDC* pDC, UINT nState, CRect colorBandRect);

	//{{AFX_MSG(COXColorPickerButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnChangeColorInTornBar(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

#endif // __OXCOLORPICKERBUTTON_H__
// ==========================================================================
