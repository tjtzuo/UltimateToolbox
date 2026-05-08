// ==========================================================================
// 					Class Specification : COXColorPickerCtrl
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
//	YES	Derived from COXPopupBarCtrl

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         

/*
COXColorPickerCtrl is derived from COXPopupBarCtrl class so below I provide some 
basic information on this class. Refer to the documentation to get full description
of COXPopupBarCtrl class. We'll assume that you've already read it before moving 
to that topic.

As a reminder, we define Popup Bar control as any control that can be represented as 
top most popup window with items on it that can be chosen using mouse or keyboard. 
For example standard Popup Menu Bar or Color Picker from Word 97 are Popup Bar 
controls. Our Popup Bar can be graphically represented as:

  --------------------------------
  |  --------------------------  |
  |  |        Default         |  | <--		"Default" button. Always is located on the top. 
  |  --------------------------  |			Size of this button calculated automatically.
  | ---  ---  ---  ---  ---  --- |
  | | |  | |  | |  | |  | |  | | | <--  | 
  | ---  ---  ---  ---  ---  --- |		|
  | ---  ---  ---  ---  ---  --- |      |
  | | |  | |  | |  | |  | |  | | | <--  |
  | ---  ---  ---  ---  ---  --- |      |
  | ---  ---  ---  ---  ---  --- |      |
  | | |  | |  | |  | |  | |  | | | <--  |  <--	"Common" buttons. You can set the number
  | ---  ---  ---  ---  ---  --- |      |		of common buttons and its size. You can set 
  | ---  ---  ---  ---  ---  --- |      |		the number of rows used to display them. 
  | | |  | |  | |  | |  | |  | | | <--  |
  | ---  ---  ---  ---  ---  --- |      |
  | ---  ---  ---  ---  ---  --- |      |
  | | |  | |  | |  | |  | |  | | | <--  |
  | ---  ---  ---  ---  ---  --- |
  |  --------------------------  |
  |  |         Custom         |  | <--		"Custom" button. Always is located in the bottom
  |  --------------------------  |			Size of this button calculated automatically.
  --------------------------------

Default and Custom buttons are optional and at least one common button has to be assigned
to Popup Bar control.

After displaying, Popup Bar control capture all mouse messages and hide itself if any mouse 
button was clicked or user press ESC, ENTER or SPACE button. If at the moment of clicking 
mouse wasn't over any button within Popup Bar or user pressed ESC then nothing was chosen.

COXPopupBarCtrl is a generic class designed specifically as a base class to provide 
basic functionality of any Popup Bar control. 

Our COXColorPickerCtrl actually add color picking related logic and drawing routines.

As value associated with common buttons we use colors. Also we owerwrite next virtual 
functions in order to draw buttons:

	virtual void DrawButton(CDC* pDC, UINT nIndex);
	virtual void DrawDefaultButton(CDC* pDC);


Wnen Custom button is pressed we display standard color dialog. In order to implement
it we overwrite next virtual function:

	virtual BOOL OnCustomButton();


Next are the main points about this class:

1)	Creation and displaying of COXColorPickerCtrl are the same as COXPopupBarCtrl:
	Color Picker Control is created in two steps: constructor and Create function. 

	After creation you can change whatever control's attributes you want (including those
	specified in constructor and Create function) using corresponding COXPopupBarCtrl
	functions. Plus we redefined some of them in order to provide more friendly
	naming convention. Next functions are redefined:

		void SetSelectedColor(COLORREF clrSelected);
		// --- Effect : set selected button using color associated with it
		COLORREF GetSelectedColor();
		// --- Returns : color of selected button
		void SetDefaultColor(COLORREF clrDefault);
		// --- Effect : set color to be associated with default button
		COLORREF GetDefaultColor();
		// --- Returns:	color of default button

2)	We include with this class predefined array of 40 colors plus array of
	tooltips for them. If those predefined colors satisfy requirements of your 
	application then you shouldn't worry to call SetButtonDataTable and
	SetToolTipTextTable functions. Everything will be done automatically. But
	you still can provide your own colors and tooltips. 


3)	To simplify task of populating color table with colors from current or system palette
	we introduce next function:

	int PopulateColorTable(UINT nStartFrom=0, BOOL bUseSystemPalette=FALSE);
	// --- In  :	nStartFrom			-	index of color in current or system palette to 
	//										start from filling array of colors
	//				bUseSystemPalette	-	if TRUE then color entries will be taken from
	//										system palette (available only in 256 - color 
	//										video regime)
	// --- Returns:	real number of colors included in array of colors
	// --- Effect : fill array of colors with values from current or system palette 
	//				starting from m_nStartFrom color index in the palette
	int PopulateColorTable(UINT* arrIndexes, UINT nElements, BOOL bUseSystemPalette=FALSE);
	// --- In  :	arrIndexes			-	array of indexes of colors in current or system 
	//										palette
	//				nElements			-	the number of elements in arrIndexes array
	//				bUseSystemPalette	-	if TRUE then color entries will be taken from
	//										system palette (available only in 256 - color 
	//										video regime)
	// --- Returns:	real number of colors included in array of colors
	// --- Effect : fill array of colors with values from current or system palette 
	//				starting from m_nStartFrom color index in the palette

	
	Still you have to provide tooltips for colors that was taken from palette in order to
	display them

4)	COXColorPickerCtrl provide palette handling so it will display all colors in 
	256 - colors video regime too.


5)	And here is an example of displaying of any Color Picker control:

		COXColorPickerCtrl Picker;
		if(Picker.Create(this))
		{
			COLORREF clr=GetColor();
			Picker.SetSelectedColor(clr);
			Picker.SetDefaultColor(RGB(0,0,128));
			if(Picker.Pick(ID_POPUPBAR_ALIGNBOTTOM))
			{
				clr=Picker.GetSelectedColor();
			}
		}


6)	It's good idea to take look at the PopupBar sample that resides in 
	.\Samples\gui\PopupBar subdirectory of your Ultimate Toolbox directory that provides 
	an example of COXColorPickerCtrl used in Toolbar. And the ColorPickerButton sample
	that resides in .\Samples\gui\ColorPickerButton subdirectory demonstrate 
	COXColorPickerButton class that uses COXColorPickerCtrl to pick colors.

*/

#if !defined(_OXCOLORPICKERCTRL_H_)
#define _OXCOLORPICKERCTRL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"
#include "OXMainRes.h"

#include "OXPopupBarCtrl.h"

// OXColorPickerCtrl.h : header file
//

// special color specifies that none was chosen
// equivalent to ID_POPUPBAR_DATA_NONE
#define ID_CLRPICK_COLOR_NONE		0xffffffff

// the number of predefined tooltiptext - color pairs in the default table
#define CLRPICK_DEFINEDCOLORS		40


/////////////////////////////////////////////////////////////////////////////
// COXColorPickerCtrl window

class OX_CLASS_DECL COXColorPickerCtrl : public COXPopupBarCtrl
{
// Construction
public:
	// --- In  :	nColors				-	number of color buttons in the Color Picker 
	//										(doesn't include	default and custom buttons)
	//				nRows				-	number of button rows (cannot be more than the 
	//										number of buttons)
	//				clrDefault			-	color to be associated with default button
	//				sizeButton			-	size of buttons (not applicable for default and 
	//										custom buttons their size are calculated 
	//										automatically and depend on the size of 
	//										Color Picker window)
	//				sDefaultButtonText	-	text to be drawn on default button
	//				sCustomButtonText	-	text to be drawn on custom button
	//				
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXColorPickerCtrl(UINT nColors=20, UINT nRows=4, 
		COLORREF clrDefault=ID_CLRPICK_COLOR_NONE,
		CSize sizeButton=CSize(18,18),
		int nIDdefault=IDS_OX_BROWSECLRDEFAULT,
		int nIDcustom=IDS_OX_BROWSECLRCUSTOM);

// Data
protected:
	// array of predefined colors used in the control by default
	static COLORREF m_defaultArrColor[];

	// array of predefined tooltip - color pairs used in the control by default
	static ButtonToolTipIDTable m_defaultArrToolTipText[];

// data for inner use

// Attributes
public:

// Operations
public:
	// --- In  :	nAlignment	-	color picker control window alignment relatively to 
	//								pParentRect	rectangle if it's not NULL or to parent 
	//								window rectangle otherwise. Can be one of:
	//
	//							ID_POPUPBAR_ALIGNLEFT	-	control's window RightTop will 
	//														be displayed at the LeftBottom 
	//														point of relative rectangle
	//							ID_POPUPBAR_ALIGNRIGHT	-	control's window LeftTop will 
	//														be displayed at the RightTop 
	//														point of relative rectangle
	//							ID_POPUPBAR_ALIGNTOP	-	control's window LeftBottom will 
	//														be displayed at the LeftTop 
	//														point of relative rectangle
	//							ID_POPUPBAR_ALIGNBOTTOM	-	control's window LeftTop will be 
	//														displayed at the LeftBottom 
	//														point of relative rectangle
	//
	//				pParentRect	-	if it's not NULL then it will be used as relative 
	//								rectangle to position Color Picker
	//				sizeOffset	-	size of Color Picker window position offset
	// --- Out : 
	// --- Returns:	TRUE if user selects any button, or FALSE otherwise
	// --- Effect : Displays Color Picker control. To get color of selected button call 
	//				GetSelectedColor function
	virtual BOOL Pick(UINT nAlignment=ID_POPUPBAR_ALIGNBOTTOM, 
		CRect* pParentRect=NULL, CSize sizeOffset=CSize(0,0));


	// --- In  :	nStartFrom			-	index of color in current or system palette to 
	//										start from filling array of colors
	//				bUseSystemPalette	-	if TRUE then color entries will be taken from
	//										system palette (available only in 256 - color 
	//										video regime)
	// --- Out : 
	// --- Returns:	real number of colors included in array of colors
	// --- Effect : fill array of colors with values from current or system palette 
	//				starting from m_nStartFrom color index in the palette
	int PopulateColorTable(UINT nStartFrom=0, BOOL bUseSystemPalette=FALSE);

	// --- In  :	arrIndexes			-	array of indexes of colors in current or system 
	//										palette
	//				nElements			-	the number of elements in arrIndexes array
	//				bUseSystemPalette	-	if TRUE then color entries will be taken from
	//										system palette (available only in 256 - color 
	//										video regime)
	// --- Out : 
	// --- Returns:	real number of colors included in array of colors
	// --- Effect : fill array of colors with values from current or system palette 
	//				starting from m_nStartFrom color index in the palette
	int PopulateColorTable(UINT* arrIndexes, UINT nElements, BOOL bUseSystemPalette=FALSE);


	
	int PopulateColorTable(COLORREF* pColorArray, UINT nElements); 

	// --- In  :	clrSelected	-	color associated with button
	// --- Out : 
	// --- Returns:	
	// --- Effect : set selected button using color associated with it
	inline void SetSelectedColor(COLORREF clrSelected) 
	{ 
		if(clrSelected==CLR_DEFAULT)
		{
			SetSelectedData(GetDefaultColor());
		}
		else
		{
			SetSelectedData(clrSelected); 
		}
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	color of selected button
	// --- Effect : 
	inline COLORREF GetSelectedColor() const { return GetSelectedData(); }

	// --- In  :	clrDefault	-	color associated with button
	// --- Out : 
	// --- Returns:	
	// --- Effect : set color to be associated with default button
	inline void SetDefaultColor(COLORREF clrDefault) { 
		SetDefaultData(clrDefault); 
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	color of default button
	// --- Effect : 
	inline COLORREF GetDefaultColor() const { return GetDefaultData(); }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXColorPickerCtrl)
	//}}AFX_VIRTUAL
public:
	virtual ~COXColorPickerCtrl() {} ;

// implementation
protected:
	// Well, the class is for professionals, so we provide as
	// much functionality as we can. You can even populate 
	// array of displayed colors with entries from current palette. 
	// Following function retrieves current system palette
	BOOL GetSystemPalette(CPalette* pPalette);
	// Sometimes current system palette doesn't match all colors we want
	// to show on the control that is a problem if current video mode is 256 colors. 
	// If this is a case then we build our own custom palette that will be saved 
	// in m_customPalette variable and will be realized as appropriate
	BOOL BuildCustomPalette(COLORREF* arrColor, UINT nEntries, CPalette* pPalette);
	BOOL BuildCustomPalette(ButtonDataTable* arrColor, CPalette* pPalette);

	// find index of button in the control
	// 0 - default button
	// 1 ... m_nColors - color button(s)
	// m_nColors+1 - custom button
	virtual void DrawButton(CDC* pDC, UINT nIndex);
	virtual void DrawDefaultButton(CDC* pDC);

	virtual BOOL OnCustomButton();

// Generated message map functions
protected:
	//{{AFX_MSG(COXColorPickerCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCOLORPICKERCTRL_H_)
