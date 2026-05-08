// ==========================================================================
// 							Class Specification : COXImageViewer
// ==========================================================================

// Header file : OXImageViewer.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////
//
/*
	DESCRIPTION

The COXImageViewer class is COXScrollWnd based class which is desined to display 
DIB and, optionally, JPEG images. As long its base class is COXScrollWnd it 
automatically support the following features:

	Scrolling of the image if it is larger then the client area size 
	Zooming of the image: 10% to 100%
	Image can be aligned vertically and horizontally.
	Supports smooth scrolling which means that the process of scrolling of the 
		image from one position to another is divided in a number of smaller 
		scrolling processes 
	Supports 'Always fit to window' option that if set will scale the image
		of the control to its size whenever it is resized
	Context menu can be displayed that allows a user easily set image display
		setting (will be described in details below)
	Track zooming option is supported which means that when a user left clicks and 
		drag the mouse cursor the tracker will be displayed so a user can choose 
		the rectangle to which the image must be zoomed
	Scrolling using mouse wheel is supported
	Optional support for ruler bars (using COXRulerBarOrganizer class)


Refer to COXScrollWnd description and class reference for details on the 
functionality provided by this class. Here we will concentrate on the functionality
that is unique for the COXImageViewer class.


Internally COXImageViewer uses COXDIB object (COXDIB class is part of the Ultimate 
Toolbox library) in order to load and display an image. This internal object can be 
retrieved and all set of the COXDIB class functions can be applied to it.



	Usage

In order to use COXImageViewer object in your application you have to create it
using standard CWnd::Create function. After control was successfully created you have 
to load the image that is to be displayed using:

	BOOL LoadImage(const COXDIB& dib);
	virtual BOOL LoadFile(LPCTSTR lpszPathName);
	virtual BOOL LoadFile(CFile* pFile);
	virtual BOOL LoadResource(LPCTSTR lpszResource);
	BOOL LoadResource(UINT nIDResource);
#ifdef OXDIB_SUPPORTJPEG
	virtual BOOL LoadJPEGFile(LPCTSTR lpszPathName);
#endif

As you can see an image can be loaded from existing COXDIB object, file, 
or resource. Also we provide an optional support for displaying JPEG images
(call LoadJPEGFile() function in order to load JPEG file) but only if you add the 
following define into your "stdafx.h" file or project preprocessor definitions list:

	#define OXDIB_SUPPORTJPEG

After image is loaded it will be displayed at zoom level of 100% in top left of the 
window unless you specify otherwise using COXScrollWnd functions that deal with
contents alignment and zoom level.

Then you can change the loaded image by retrieving a pointer to internal COXDIB 
object and calling COXDIB functions. 

	COXDIB* GetImage();

E.g. you can change the size of the image, rotate and flip it. Then you can save 
any changes made to the object into DIB or JPEG, if supported, file. 
E.g. in order to save the image associated with COXImageViewer object you might call:

	GetImage()->WriteJPEG(_T("MyImage.jpg"));

Refer to COXDIB class reference for details on all functions available. 


The last thing we would like to mention is that in the case when >=256 color image 
must be displayed in 256-color video mode you have provide some additional 
functionality in applications that use the COXImageViewer class. COXDIB provides 
us with information about the palette that should be used in order to get the best 
results while displaying the image. The problem is that only top-level windows will 
get messages that notify a window that it should release its own palette. These 
messages are:

	WM_PALETTECHANGED
	WM_QUERYNEWPALETTE

You as a programmer have to make sure that these messages will be forwarded to
all currently active COXImageViewer objects from main window of your application.
E.g. in the case of MDI or SDI application you can add the following handlers to 
your CMainFrame window:

	void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd) 
	{
		CMDIFrameWnd::OnPaletteChanged(pFocusWnd);
		
		// TODO: Add your message handler code here
		SendMessageToDescendants(WM_PALETTECHANGED,
			(pFocusWnd!=NULL ? (WPARAM)pFocusWnd->GetSafeHwnd() : NULL));
	}

	BOOL CMainFrame::OnQueryNewPalette() 
	{
		// TODO: Add your message handler code here and/or call default
		
		SendMessageToDescendants(WM_QUERYNEWPALETTE);
		return CMDIFrameWnd::OnQueryNewPalette();
	}


This messages will be eventually handled by COXImageViewer objects and images
will be displayed in the most appropriate way. Of course this approach will
work only for COXImageViewer controls that are CMainFrame descendants.

If you need to explicitly release the palette associated with the image you can
call the following function:

	UINT DoRealizePalette();


  
The sample that demonstrates COXImageViewer class is called ImageViewer and can be 
found in the .\Samples\graphics\ImageViewer subdirectory of your Ultimate Toolbox 
directory.



Dependency:
	#include "OXImageViewer.h"


Source code files:
	"OXImageViewer.cpp"		
	"OXScrollWnd.cpp"		-	COXScrollWnd implementation
#ifdef OXSCRLWND_USE_RULER
	"OXRulerOrganizer.cpp"	-	COXRulerOrganizer implementation
	"OXHookWnd.cpp"			-	implementation of the COXHookWnd class 
								from which the COXRulerOrganizer class is derived
#endif
	"OXDIB.cpp"				-	COXDIB implementation
#ifdef OXDIB_SUPPORTJPEG
	"OXBmpFle.cpp"
	"OXGphFle.cpp"
	"OXJPGCod.cpp"
	"OXJPGCom.cpp"			-	JPEG support
	"OXJPGDom.cpp"
	"OXJPGExp.cpp"
	"OXJPGFle.cpp"
#endif	
  


*/


#if !defined(_OXIMAGEVIEWER_H__)
#define _OXIMAGEVIEWER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// OXImageViewer.h : header file
//

#include "OXDIB.h"
#include "OXScrollWnd.h"
#include "OXMainRes.h"

//
//////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// COXImageViewer window

class OX_CLASS_DECL COXImageViewer : public COXScrollWnd
{
	DECLARE_DYNAMIC(COXImageViewer)
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Contructs the object
	COXImageViewer();

// Attributes
public:

protected:
	// number of colors used in current video mode
	static int m_nDisplayColors;

	// DIB image
	COXDIB m_dib;
	
	// special DIB object that we use to render the image on the screen
	// in the case when the current system palette is no more than 8 bits
	// and image consist of no less than 256 colors
	COXDIB m_dibDither;

	// background color
	COLORREF m_clrBackground;

// Operations
public:

	// --- In  :	dib		-	reference to COXDIB object that must be displayed. 
	//							The copy of the object will be made and saved 
	//							internally
	// --- Out : 
	// --- Returns:	TRUE if image was loaded successfully
	// --- Effect:	Copies specified COXDIB object and display it
	inline BOOL LoadImage(const COXDIB& dib) 
	{ 
		m_dib=dib;
		return InitializeImage();
	}


	// --- In  :	lpszPathName	-	filename of a DIB file to be displayed 
	//									in the image viewer
	//				pFile			-	pointer to open CFile object 
	// --- Out : 
	// --- Returns:	TRUE if file was loaded successfully
	// --- Effect:	copies COXDIB object to be displayed in the image viewer from
	//				the specified source (file on disk or open CFile object)
	virtual BOOL LoadFile(LPCTSTR lpszPathName);
	virtual BOOL LoadFile(CFile* pFile);


	// --- In  :	lpszResource	-	resource string that identifies a DIB 
	//									resources to be displayed in the image viewer
	//				nIDResource		-	resource ID that identifies a DIB 
	//									resources to be displayed in the image viewer
	// --- Out : 
	// --- Returns:	TRUE if resource was loaded successfully
	// --- Effect:	copies COXDIB object to be displayed in the image viewer from
	//				the specified resource
	virtual BOOL LoadResource(LPCTSTR lpszResource);
	inline BOOL LoadResource(UINT nIDResource) {
		return LoadResource(MAKEINTRESOURCE(nIDResource));
	}


#ifdef OXDIB_SUPPORTJPEG
	// --- In  :	lpszPathName	-	filename of a JPEG file to be displayed 
	//									in the image viewer
	//				pFile			-	pointer to open CFile object 
	// --- Out : 
	// --- Returns:	TRUE if file was loaded successfully
	// --- Effect:	reads JPEG file and converts it internally into COXDIB object
	//				to be displayed in the image viewer. In order to use this 
	//				function you must add the following line into your "stdafx.h" file
	//				or project settings:
	//
	//				#define OXDIB_SUPPORTJPEG
	//
	//				All convertion logic is implemented in COXDIB class 
	virtual BOOL LoadJPEGFile(LPCTSTR lpszPathName);
	virtual BOOL LoadJPEGFile(CFile* pFile);
#endif

	
	// --- In  :	bRedraw		-	if TRUE then image will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if contents was successfully emptied
	// --- Effect:	Resets the contents of the image viewer
	virtual BOOL Empty(BOOL bRedraw=TRUE);

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if any image is displayed
	// --- Effect:	Retrieves the flag that specifies whether any image is displayed
	virtual BOOL IsEmpty() const { return m_dib.IsEmpty(); } 


	// --- In  :	bRedraw		-	if TRUE then image will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if image was successfully updated
	// --- Effect:	call this function any time the image parameters 
	//				has been changed. E.g. using GetImage() function you can
	//				get a pointer to internal image object and you can call 
	//				COXDIB functions (rotation, resizing, etc.) that might change 
	//				the image charecterstics. After you did it you must call this 
	//				function in order to display the image in the updated state.
	BOOL UpdateImage(BOOL bRedraw=TRUE);


	// --- In  :	
	// --- Out : 
	// --- Returns:	pointer to the COXDIB object used internally to display the image.
	// --- Effect:	retrieves pointer to the COXDIB object used internally to 
	//				display the image. If you call any COXDIB function that might 
	//				change the appearance of the image (rotation, resizing, etc.),
	//				make sure that you call UpdateImage() function in order to
	//				display the image in the updated state. 
	//				If you want to save any changes made to the image in the 
	//				image viewer (e.g. you've rotated the image) you have to call this
	//				function in order to retrieve the pointer to COXDIB object and
	//				then call COXDIB::Write() function. If you want to save the image 
	//				as JPEG file call the COXDIB::WriteJPEG() function.
	inline COXDIB* GetImage() { return &m_dib; }

	// --- In  :	
	// --- Out : 
	// --- Returns:	pointer to the COXDIB object used internally to display 
	//				the dithered version of the original true-color image 
	//				which is used when 256-color video mode is active.
	// --- Effect:	retrieves pointer to the COXDIB object used internally to 
	//				display the dithered version of the original true-color image 
	//				which is used when 256-color video mode is active. 
	//				Any changes you might made to this object will be discarded 
	//				next time when UpdateImage() function is called. It means that
	//				you must treat this object as "read-only"
	inline COXDIB* GetDitheredImage() { return &m_dibDither; }


	// --- In  :	bRedraw		-	if TRUE then image will be redrawn
	//				bForeground	-	if TRUE then palette will be forced to be 
	//								a foreground palette, otherwise, to be 
	//								a background palette
	// --- Out : 
	// --- Returns:	indicates how many entries in the logical palette were mapped 
	//				to different entries in the system palette. This represents 
	//				the number of entries that this function remapped to accommodate 
	//				changes in the system palette since the logical palette was 
	//				last realized.
	// --- Effect:	call this function any time current palette has been changed.
	//				Relevant only when 256-color video mode is set
	UINT DoRealizePalette(BOOL bRedraw=TRUE, BOOL bForeground=TRUE);


	// --- In  :	
	// --- Out : 
	// --- Returns:	Background color
	// --- Effect:	Retrieves background color value
	inline COLORREF GetBackgroundColor() const { return m_clrBackground; }

	// --- In  :	clrBackground	-	background color
	//				bRedraw			-	if TRUE then the contents will be redrawn
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Sets the background color value
	void SetBackgroundColor(COLORREF clrBackground, BOOL bRedraw=TRUE);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXImageViewer)
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Destructs the object
	virtual ~COXImageViewer();


protected:
	// retrieves pointer to the image palette. Automatically handles the case when 
	// ditthered image palette must be retrieved
	CPalette* GetDIBPalette();

	// retrieves size of the image in pixels
	CSize GetDIBSize();

	// initialize scrolling settings for the image. Might be overriden 
	// in order to specify different from default scrolling settings.
	virtual BOOL InitializeImage(const CSize& sizePage=CSize(100,100),
		const CSize& sizeLine=CSize(10,10));

	// returns TRUE if the dithered image must be displayed instead of the 
	// original one
	BOOL CheckUseDithered();
	// returns TRUE if image palette must be selected/realized before 
	// drawing the image
	BOOL CheckUsePalette();

	// helper function that draws the specified part of the image 
	// in the specified rectangle on the window
	virtual BOOL DrawDIB(CDC* pDC, const CRect& rectDest, const CRect& rectSrc);

	// virtual COXScrollWnd function, overriden in order to populate 
	// context menu with items for rotation and flipping operations.
	virtual BOOL OnPopulateContextMenu(CMenu* pMenu, CPoint& point);

	// Generated message map functions
protected:
	//{{AFX_MSG(COXImageViewer)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnRotate(UINT nID);
	afx_msg void OnFlip(UINT nID);
	afx_msg void OnAlign(UINT nID);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXIMAGEVIEWER_H__)
