// ==========================================================================
// 							Class Specification : 
//							      COXStatic
// ==========================================================================

// Header file : OXStatic.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
#ifndef _OXSTATIC_H
#define _OXSTATIC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


// enumerator of different options for filling the background
enum BackgroundFillType { NOGRADIENT, GRADIENT_LEFT, GRADIENT_CENTER, GRADIENT_RIGHT };

// identificator for tool tip control
#define ID_HLTOOLTIP	0x00000001

////////////////
// COXStatic implements a static control which could be set to use
// user-defined color and font to draw text. Also you can show tooltip
// and you can make COXStatic automatically adjust its size to fit drawn text
//
// You can use standard DDX/DDV technique or subclass common static control. 
// Then you can use next function to give your static control special appearence
//
//		void SetTextColor(COLORREF clr, BOOL bRedraw=TRUE);
//		// --- Effect : sets the color that COXStatic object uses to draw text
//
//		BOOL SetTextLogFont(LOGFONT* plf);
//		BOOL SetTextFont(CFont* pFont);
//		// --- Effect : sets the font that COXStatic object uses to draw text
//
//		virtual BOOL SetDefaultTextFont();
//		// --- Effect :	make COXStatic object to use default font to draw text.
//		//				Note that function declared as virtual, so you can put
//		//				in any derivation of COXStatic class your own definition
//		//				of "default font". COXStatic class uses the font which is 
//		//				associated with CStatic window as default.
//
//		void SetFitToText(BOOL bFitText);
//		// --- Effect :	if bFitText is TRUE COXStatic object updates its size 
//		//				everytime the size of drawn text changes
//
//		void SetShowToolTip(BOOL bShowToolTip);
//		// --- Effect : use this function to show/hide tooltip control 
//
//		void SetToolTipText(LPCTSTR sText);
//		// --- Effect :	Sets the tooltip text. If you haven't set the tooltip text and
//		//				haven't called SetShowToolTip(FALSE) then COXStatic would use
//		//				window's text as the tooltip text
//
//
//
// For full description of all public functions refer to documentation
//
//

class OX_CLASS_DECL COXStatic : public CStatic 
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXStatic();


	// --- In  :	lpszText	-	Specifies the text to place in the control. If NULL, 
	//								no text will be visible
	//				dwStyle		-	Specifies the static control's style. Apply any 
	//								combination of static control styles to the control. 
	//								See the CStatic:Create function for details.
	//				rect		-	Specifies the static control's size and position. It can 
	//								be either a CRect object or a RECT structure.
	//				pParentWnd	-	Specifies the static control's parent window, usually a 
	//								CDialog. It must not be NULL.
	//				nID			-	Specifies the static control's ID.
	// --- Out : 
	// --- Returns: TRUE if successful or FALSE otherwise
	// --- Effect : Create the control
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, 
		CWnd* pParentWnd, UINT nID=0xffff);


	// --- In  :	clr	-	color of text
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the color that COXStatic object uses to draw text
	void SetTextColor(COLORREF clr, BOOL bRedraw=TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns: text color
	// --- Effect :	gets the color that COXStatic object uses to draw text
	inline COLORREF GetTextColor() const { return m_clr; }


	// --- In  :	clr	-	color of the background
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the color that COXStatic object uses to draw its background
	void SetBkColor(COLORREF clr, BOOL bRedraw=TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns: background color
	// --- Effect :	gets the color that COXStatic object uses to draw its background
	inline COLORREF GetBkColor() const { return m_clrBk; }


	// --- In  :	typeFillBackground	-	type of the background filling. Could be
	//										on of the following:
	//						NOGRADIENT		-	solid filling
	//						GRADIENT_LEFT	-	gradient filling,
	//											the darkest color is on the left side
	//											and the lightest - on the right side
	//						GRADIENT_CENTER	-	gradient filling,
	//											the darkest color is in the center
	//											and the lightest - on the right and
	//											left sides
	//						GRADIENT_RIGHT	-	gradient filling,
	//											the darkest color is on the right side
	//											and the lightest - on the left side
	//				bRedraw		-	if TRUE then control will be redrawn right away
	// --- Out : 
	// --- Returns:
	// --- Effect : Sets the type of background filling.
	void SetBackgroundFillType(BackgroundFillType typeFillBackground, BOOL bRedraw=TRUE); 

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if caption is filled gradiently, or FALSE otherwise.
	// --- Effect :	Retrieves the type of background filling.
	inline BackgroundFillType GetBackgroundFillType() const { 
		return m_typeFillBackground; 
	}

	// by default we use current dialog font, but
	// you can set whatever font you want to display the object,
	// but remember that you have to adjust the size of window to 
	// make sure text will be drawn correctly (this is not a problem 
	// if you call SetFitToText(TRUE))
	//
	// --- In  :	plf	-	pointer to LOGFONT structure
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:	TRUE if font was successfully set
	// --- Effect : sets the font that COXStatic object uses to draw text;
	//				if you call this function as SetTextLogFont(NULL) then
	//				text will be drawn using the font which is associated 
	//				with CStatic window by default 
	BOOL SetTextLogFont(LOGFONT* plf, BOOL bRedraw=TRUE);

	// --- In  :	pFont	-	pointer to CFont
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:	TRUE if font was successfully set
	// --- Effect : sets the font that COXStatic object uses to draw text
	BOOL SetTextFont(CFont* pFont, BOOL bRedraw=TRUE);

	// --- In  :	plf	-	pointer to LOGFONT structure
	// --- Out :	plf -	filled the structure with the LOGFONT
	//						of the font that COXStatic object uses to draw text
	// --- Returns:	TRUE if plf was successfully populated with 
	//				LOGINFO of the font used by COXStatic object, 
	//				otherwise returns FALSE and plf is undefined
	// --- Effect : 
	BOOL GetTextLogFont(LOGFONT* plf) const;

	// --- In  :	
	// --- Out :	
	//				
	// --- Returns:	pointer to the font used by COXStatic object if succeed, 
	//				otherwise returns NULL
	// --- Effect : 
	CFont* GetTextFont();


	// if you want COXStatic to use default font to draw text then call this function,
	// returns TRUE if operation was successful, otherwise FALSE
	//
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if default font was successfully set to the 
	//				COXStatic object, otherwise returns FALSE
	// --- Effect :	make COXStatic object to use default font to draw text.
	//				Note that function declared as virtual, so you can put
	//				in any derivation of COXStatic class your own definition
	//				of "default font". COXStatic class uses the font which is 
	//				associated with CStatic window as default.
	virtual BOOL SetDefaultTextFont();


	// --- In  :	bFitText	-	if TRUE that anytime the size of drawn text
	//								changes the size of COXStatic window will be 
	//								updated correspondingly. 
	// --- Out : 
	// --- Returns :
	// --- Effect :
	void SetFitToText(BOOL bFitText);
	
	// --- In  :	bFitText	-	if TRUE that anytime the size of drawn text
	//								changes the size of COXStatic window will be 
	//								updated correspondingly. 
	// --- Out : 
	// --- Returns :	TRUE if COXStatic object updates its size everytime the size 
	//					of drawn text changes
	// --- Effect :
	inline BOOL GetFitToText() const { return m_bFitToText; }

	// Tooltips
	//
	// By default the tooltip control is not shown. You have to call 
	// SetShowToolTip(TRUE) to initialize it. By default we use window text
	// as tooltip text, if you want to display something else (I bet you will :-))
	// then you have to call SetToolTipText(LPCTSTR sText) function
	//
	// 
	// --- In  :	sText	-	text to be shown in the tooltip window
	// --- Out : 
	// --- Returns :
	// --- Effect :	Sets the tooltip text. If you haven't set the tooltip text and
	//				haven't called SetShowToolTip(FALSE) then COXStatic would use
	//				window's text as the tooltip text
	void SetToolTipText(LPCTSTR sText);

	// --- In  :	bForce	-	if TRUE then retrieve the tooltip text even if 
	//							it wasn't set by SetToolTipText function (tooltip 
	//							text could be set automatically to display the 
	//							COXStatic window's text), otherwise returns empty string
	// --- Out : 
	// --- Returns :	tooltip text
	// --- Effect :		returns tooltip text if it was set previously by 
	//					SetToolTipText function. If bForse is TRUE and 
	//					SetToolTipText wasn't called then returns the tooltip 
	//					text that was set automatically (COXStatic window's text)
	CString GetToolTipText(BOOL bForce=TRUE);


	// --- In  :	bShowToolTip	-	if TRUE then tooltip will be shown,
	//									otherwise not
	// --- Out : 
	// --- Returns :
	// --- Effect : use this function to show/hide tooltip control 
	void SetShowToolTip(BOOL bShowToolTip);

	// --- In  :
	// --- Out : 
	// --- Returns :	TRUE if tooltip control is to be shown, otherwise FALSE
	// --- Effect :
	inline BOOL GetShowToolTip() const { return m_bShowToolTip; }

	// helper function.
	//
	// --- In  :	pStatic	-	pointer to any CStatic object
	// --- Out : 
	// --- Returns:	TRUE if static control displays text, otherwise FALSE
	// --- Effect :
	BOOL IsStaticText(CStatic* pStatic) const;

	// helper function for gradient filling of the background
	static void FillGradient(CDC* pDC, BackgroundFillType typeFillBackground,
		CRect rect, COLORREF clr);
	static void PaintRect(CDC* pDC, int x, int y, int w, int h, COLORREF color);
	////////////////////////////////////

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXStatic)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	DECLARE_DYNAMIC(COXStatic)

	// color of background
	COLORREF m_clrBk;	
	// type of filling
	BackgroundFillType m_typeFillBackground;
	// color of text
	COLORREF m_clr;	
	// font for drawing text
	CFont m_font;				 

	// if TRUE, show a tooltip
	BOOL m_bShowToolTip;
	// text of the tooltip
	CString m_sToolTipText;
	// tooltip control
	CToolTipCtrl m_ttc;

	// if TRUE, size of window of static control will be changed to fit size of text
	// every time length or attributes of text change
	BOOL m_bFitToText;
	// original coordinates of the control 
	CRect m_rectOriginal;

	// inner var to define if SetToolTipText function was called
	BOOL m_bToolTipUserDefined;

private:
	// flag that notifies that static control was created using Create function 
	// (not subclassed)
	BOOL m_bCreated;

	// flag that notifies that static control has called SetDefaultTextFont
	// (should be called only once)
	BOOL m_bIsDefaultFontSet;


protected:
	// initialize static control (create tooltip control. etc.)
	virtual BOOL InitStatic();

	// If m_bFitToText is TRUE then adjusts the size of control's window 
	// to the size of the text, otherwise sets size of COXStatic window 
	// to its original value.
	void AdjustToFitText();
	
	// Builds tooltip text. This function is called if tooltip text is not set by 
	// SetToolTipText function. Note that it is defined as virtual so any derived
	// COXStatic class can define its own way to implicitly set tooltip text 
	// if it wasn't set explicitly.
	virtual void FormatToolTipText();

	// fills in the background of the control
	void DrawBackground(CDC* pDC);


	// Generated message map functions
	//{{AFX_MSG(COXStatic)
	virtual afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif	// _OXSTATIC_H
