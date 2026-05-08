// ==========================================================================
//						   Class Specification : 
//				COXBackgroundPainter & COXBackgroundPainterOrganizer
// ==========================================================================

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


COXBackgroundPainter class is designed to simplify the process of window 
background painting. It provides the functionality of painting the 
background of the window with specified image and/or color. 

For some reason background painting never been a trivial task. By default,
painting happens when WM_ERASEBKGND message is sent to window. If you
don't handle this message in any specific way then Windows use background
brush associated with the corresponding window class in order to fill
the background of the window. So in order to paint the window using 
different color you would have to handle WM_ERASEBKGND message or 
alternatively create new window class for your window with the brush of color
you need. 

Painting window's background with different color is neither difficult
nor interesting task. But painting background of window with some image 
(like Windows Desktop) could add some value to your application. 

COXBackgroundPainter class provides that kind of functionality. And it
takes only one function call in order to setup a window to be painted in new 
fancy way.

As a number of other Ultimate Toolbox classes COXBackgroundPainter class
is derived from COXHookWnd class. This class allows to handle any
messages before the hooked window does. Also internally COXBackgroundPainter
uses COXDIB object in order to draw 256> color images.

In order to set any window background to be painted using COXBackgroundPainter
object you have to hook such window using the following function:

	Attach()
	
In the function you specify the pointer to the window to be hooked, info
about the image to be used while painting the background (you can specify
COXDIB object, resource or file name), type of image drawing (Tile, 
Stretch, Center, ...), background color to be used to fill the window
background that is not covered by image (refer to the documentation on
this function for details), and pointer to the window which top/left point
of its paintable rectangle will be used as origin point for painting 
routines (this is very important if you need to align the background
among different child windows of the same parent window). 

It's highly probable that Attach() function will be the only one that you 
will use in your applications but we defined a set of additional functions 
that allows you to control all aspects of the class functionality.

Use Detach() function in order to unhook any attached window.
In order to change window background painting properties at run-time without 
detaching and attaching it use the following set of functions:
	
	SetWallpaper()
	SetPaintType()
	SetBkColor()
	SetOriginWindow()

To retrieve background painting properties call:

	GetWallpaperImage()
	GetPaintType()
	GetBkColor()
	GetOriginWindow()


As you can see it's really easy to use this class, but unfortunatly every
story has two sides. We mentioned above about the standard way Windows
paints the background of any window. It is TRUE for any window unless
specific window handles the process of its drawing and paints its
background in different way (e.g. in WM_PAINT handler). Almost all 
standard controls paint their background and almost all of them do this 
differently. E.g. some of the controls like tree control, list control or 
toolbar support powerful NM_CUSTOMDRAW notification. Others, like static 
or edit controls should handle WM_CTLCOLOR message in order to paint the 
background. And also we shouldn't forget about ownerdraw controls too!

The question is how to use COXBackgroundPainter class in such cases. 
There is no easy answer that will work for any control but generally
you can use the following approach:

	1)	Try to hook the control using Attach() function and test if 
		it paints the background correctly. If everything is alright
		then you are lucky and you are using the control that was designed 
		consistently with the Windows treat background painting.

	2)	Otherwise you have to provide the background painting for such control
		explicitly at some point depending on the functionality of 
		the control (refer to the documentation on the corresponding control 
		to make a decision where to put background painting code). You still 
		can use COXBackgroundPainter class in this case but at the 
		appropriate moment you have to explicitly call the following 
		function in order to paint the background:
	
			DrawWallpaper(CDC* pDC);

		You only have to specify the device context to paint the background in
		and this function will take care of everything else itself.



In most applicable cases (like painting the background of dialog, MDIClient
or View windows) COXBackgroundPainter class will work without any additional 
efforts from your side. But you still have to have one COXBackgroundPainter
object for every window that is going to be hooked. This could be a little
bit inconvinient, especially if you are going to provide background 
painting support for a number of windows within the same framework.


In order to simplify things we designed COXBackgroundPainterOrganizer class.
COXBackgroundPainterOrganizer provides very similar to COXBackgroundPainter
set of functions. E.g. the definition of Attach() function is completely
the same. The only difference is that when you call this function for new
window new COXBackgroundPainter object will be instantiated and its
Attach() function will be called. At any moment you can define whether 
given window is attached to a COXBackgroundPainter object calling function:

	IsAttached(const CWnd* pWnd);

You can retrieve the pointer to the COXBackgroundPainter object that
corresponds to the specified window using function:

	GetPainter(const CWnd* pWnd);


Refere to the COXBackgroundPainterOrganizer class reference documentation
for details on the rest of functions.


As long as COXBackgroundPainterOrganizer class provides almost the same 
functionality as COXBackgroundPainter does and it really simplify things
when you hook multiple windows within the same parent window, we suggest
that COXBackgroundPainterOrganizer should be used preferably. Below you 
will find some scenarios that can be used while implementing background
painting support.


	Dialog window with background:

	1)	Define COXBackgroundPainterOrganizer object in your CDialog 
		derived class

		class CMyDlg : public CDialog
		{
			. . . . . . . . . . . . . . 
		protected:
			COXBackgroundPainterOrganizer m_backPainterOrganizer;
			. . . . . . . . . . . . . . 
		}

	2)	Override OnInitDialog function and attach dialog window
		using COXBackgroundPainterOrganizer object

		BOOL CLogoDlg::OnInitDialog() 
		{
			CDialog::OnInitDialog();
	
			// TODO: Add extra initialization here
	
			. . . . . . . . . . . . . . 

			m_backPainterOrganizer.Attach(this,IDB_WALLPAPER);

			. . . . . . . . . . . . . . 

			return TRUE;  // return TRUE unless you set the focus to a control
					      // EXCEPTION: OCX Property Pages should return FALSE
		}
		
		We assume IDB_WALLPAPER is image ID that you want to use to fill
		the background. Using Attach() function the way it shawn here
		will tile the specified image to cover all background area, 
		default background color will be used and dialog itself will be used 
		as origin window.
		


	
	MDI frame window with toolbar.

	1)	Define COXBackgroundPainterOrganizer object and GetPainter() 
		function in your CMDIFrameWnd derived class

		class CMainFrame : public CMDIFrameWnd
		{
			. . . . . . . . . . . . . . 
		protected:
			COXBackgroundPainterOrganizer m_backPainterOrganizer;
			. . . . . . . . . . . . . . 
		public:
			inline COXBackgroundPainterOrganizer* GetPainter() {
				return &m_backPainterOrganizer; 
			}
			. . . . . . . . . . . . . . 
		}

	2)	Override OnCreate() function and after creating the toolbar use
		the following code:

		int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			. . . . . . . . . . . . . . 

			// hook toolbar after it was created and initialized
			m_backPainterOrganizer.Attach(&m_wndToolBar,IDB_WALLPAPER);

			CWnd* pMDIClient=CWnd::FromHandle(m_hWndMDIClient);
			// hook MDIClient window
			m_backPainterOrganizer.Attach(pMDIClient,IDB_WALLPAPER,Tile,
				CLR_DEFAULT,this);

			. . . . . . . . . . . . . . 

		}

	3)	In order to provide smooth background painting for frame window that 
		supports docking control bars we have to hook undocumented CDockBar
		controls that main frame window uses as placeholder for control bars.
		In order to do that you have to override the EnableDocking function
		in the following way:

		void CMainFrame::EnableDocking(DWORD dwDockStyle)
		{
			CMDIFrameWnd::EnableDocking(dwDockStyle);

			if(GetControlBar(AFX_IDW_DOCKBAR_TOP)!=NULL &&
				!m_backPainterOrganizer.
				IsAttached(GetControlBar(AFX_IDW_DOCKBAR_TOP)))
			{
				m_backPainterOrganizer.
					Attach(GetControlBar(AFX_IDW_DOCKBAR_TOP),IDB_WALLPAPER,
						Tile,CLR_DEFAULT,this);
			}
			if(GetControlBar(AFX_IDW_DOCKBAR_BOTTOM)!=NULL &&
				!m_backPainterOrganizer.
				IsAttached(GetControlBar(AFX_IDW_DOCKBAR_BOTTOM)))
			{
				m_backPainterOrganizer.
					Attach(GetControlBar(AFX_IDW_DOCKBAR_BOTTOM),IDB_WALLPAPER,
						Tile,CLR_DEFAULT,this);
			}
			if(GetControlBar(AFX_IDW_DOCKBAR_LEFT)!=NULL &&
				!m_backPainterOrganizer.
				IsAttached(GetControlBar(AFX_IDW_DOCKBAR_LEFT)))
			{
				m_backPainterOrganizer.
					Attach(GetControlBar(AFX_IDW_DOCKBAR_LEFT),IDB_WALLPAPER,
						Tile,CLR_DEFAULT,this);
			}
			if(GetControlBar(AFX_IDW_DOCKBAR_RIGHT)!=NULL &&
				!m_backPainterOrganizer.
				IsAttached(GetControlBar(AFX_IDW_DOCKBAR_RIGHT)))
			{
				m_backPainterOrganizer.
					Attach(GetControlBar(AFX_IDW_DOCKBAR_RIGHT),IDB_WALLPAPER,
						Tile,CLR_DEFAULT,this);
			}
		}

		
	4)	If you want to paint the background of views too you can override
		OnCreate() function of your view class and use the following code:

		int CMyView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
		{
			if (CView::OnCreate(lpCreateStruct) == -1)
				return -1;
			
			// TODO: Add your specialized creation code here
			
			. . . . . . . . . . . . . . 

			((CMainFrame*)AfxGetMainWnd())->GetPainter()->
				Attach(this,IDB_WALLPAPER);

			. . . . . . . . . . . . . . 

			return 0;
		}





One	of an interesting aplication of COXBackgroundPainterOrganizer class
is the following: you want to display 256> color image in the About Box
of your company logo. If you use Picture (actually, CStatic control)
object to do that then you don't have palette support and the size of 
control will depend on the size of image. Instead you can do the following:

	1)	Create the window that will be used to display picture
	2)	Hook this window using COXBackgroundPainterOrganizer object and 
		specify the image that will be displayed
		
Pretty easy.





Special note:	How to use COXBackgroundPainterOrganizer to display 256> color
				bitmaps when 256 color video mode is set.

In order to do that we have to release the background image palette when
WM_PALETTECHANGED and WM_QUERYNEWPALETTE messages are sent to attached window.
It's implemented automatically for you in COXBackgroundPainter class but there is
small problem. WM_PALETTECHANGED and WM_QUERYNEWPALETTE messages are sent only
to top-level and overlapped windows. So background images will be correctly 
displayed for dialog or mainframe window, but for non-overlapped child windows 
you have to provide some way of notifying them that they have to update system 
palette. We suggest the following solution for your problem. In the class that
is guaranteed to get WM_PALETTECHANGED and WM_QUERYNEWPALETTE messages
(e.g. it could be your main frame window or dialog window) you include handlers
for them. In the handlers you forward these messages to the windows that are
hooked using COXBackgroundPainterOrganizer or alternatively you can send these 
messages to all descendand windows using CWnd::SendMessageToDescendants() function.






All above mentioned techniques are implemented in the BackgroundPainter 
sample that can be found in .\samples\gui\BackgroundPainter subdirectory
of your Ultimate Toolbox directory.




Dependencies:

	#include "OXBackgroundPainter.h"


Source code files:

	"OXBackgroundPainter.cpp"

  The following file implements hooking functionality:

	"OXHookWnd.cpp"	

  The following file implements support for DIB files:

	"OXDib.cpp"
	
		
	

*/


#if !defined(_OXBACKGROUNDPAINTER_H_)
#define _OXBACKGROUNDPAINTER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXHookWnd.h"
#include "OXDib.h"


enum PaintType
{
	Tile,
	Stretch,
	TopLeft,
	TopCenter,
	TopRight,
	LeftCenter,
	Center,
	RightCenter,
	BottomLeft,
	BottomCenter,
	BottomRight
};

/////////////////////////////////////////////////////////////////////////////
// COXBackgroundPainter window

class OX_CLASS_DECL COXBackgroundPainter : public COXHookWnd
{
public:
// Construction

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBackgroundPainter();


	// --- In  :	pWnd				-	pointer to created window
	//				pDIB				-	pointer to COXDIB object that 
	//										will be used as wallpaper or NULL
	//										if you don't want to use any bitmap
	//										as wallpaper
	//				lpszDIBResourceName	-	resource name of the image that 
	//										will be used as wallpaper or NULL 
	//										if you don't want to use any bitmap 
	//										as wallpaper
	//				nDIBResourceID		-	resource ID of the image that will be 
	//										used as wallpaper or NULL if you 
	//										don't want to use any bitmap as 
	//										wallpaper
	//				sDIBFileName		-	full path to the image file that 
	//										will be used as wallpaper or empty 
	//										string if you don't want to use any 
	//										bitmap as wallpaper
	//				paintType			-	painting type. It must take one of 
	//										the following values:
	//
	//							Tile		-	image will be tiled to cover all
	//											paintable area of the specified
	//											window
	//							Stretch		-	image will be stretched to fill 
	//											all paintable area of the 
	//											specified window
	//							TopLeft		-	image will be drawn starting 
	//											from the left/top point of 
	//											the paintable area of the 
	//											specified window
	//							TopRight	-	image will be drawn starting 
	//											from the right/top point of 
	//											the paintable area of the 
	//											specified window
	//							TopCenter	-	image will be drawn starting 
	//											from the top side of the paintable 
	//											area of the specified window and 
	//											will be centered horizontally
	//							LeftCenter	-	image will be drawn starting 
	//											from the left side of the paintable 
	//											area of the specified window and 
	//											will be centered vertically
	//							RightCenter	-	image will be drawn starting 
	//											from the right side of the paintable 
	//											area of the specified window and 
	//											will be centered vertically
	//							Center		-	image will be centered vertically
	//											and horizontally
	//							BottomLeft	-	image will be drawn starting 
	//											from the left/bottom point of 
	//											the paintable area of the 
	//											specified window
	//							BottomRight	-	image will be drawn starting 
	//											from the right/bottom point of 
	//											the paintable area of the 
	//											specified window
	//							BottomCenter-	image will be drawn starting 
	//											from the bottom side of the paintable 
	//											area of the specified window and 
	//											will be centered horizontally	
	//
	//				clrBk				-	background color, if CLR_DEFAULT is
	//										specified then default window 
	//										background color will be used. 
	//										Note that if you use image for
	//										background painting the areas that
	//										is not covered by the image will
	//										be filled with this color.
	//
	//				pOriginWnd			-	pointer to created window which
	//										top/left point of the paintable
	//										rectangle will be used as origin
	//										point while painting the background.
	//										If NULL is specified then the 
	//										window that is being attached will
	//										be used as origin one
	// --- Out : 
	// --- Returns:	TRUE if pWnd was successfully hooked, otherwise FALSE
	// --- Effect : Hooks window. WM_ERASEBKGND message of the hooked window 
	//				will be handled in order to paint the background. Background
	//				can be painted using specified background image or 
	//				background color. If you chose to use an image to paint the
	//				background you can aditionaly specify the type of painting
	//				(refer to the description of paintType argument for details).
	//				Please, note that you can specify the pointer to the window 
	//				which top/left point of its paintable rectangle will be used 
	//				as origin point for painting routines (this is very important 
	//				if you need to align the background among different child windows 
	//				of the same parent window)
	//
	inline BOOL Attach(CWnd* pWnd, LPCTSTR lpszDIBResourceName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		COXDIB dib;
		if(lpszDIBResourceName!=NULL)
		{
			if(!dib.LoadResource(lpszDIBResourceName))
			{
				return FALSE;
			}
		}
		return Attach(pWnd,&dib,paintType,clrBk,pOriginWnd);
	}

	inline BOOL Attach(CWnd* pWnd, UINT nDIBResourceID, PaintType paintType=Tile, 
		COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		return Attach(
			pWnd,MAKEINTRESOURCE(nDIBResourceID),paintType,clrBk,pOriginWnd);
	}

	inline BOOL Attach(CWnd* pWnd, const CString& sDIBFileName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		COXDIB dib;
		if(!sDIBFileName.IsEmpty())
		{
			if(!dib.Read(sDIBFileName))
			{
				return FALSE;
			}
		}
		if(Attach(pWnd,&dib,paintType,clrBk,pOriginWnd))
		{
			m_sImageFileName=sDIBFileName;
			return TRUE;
		}
		return FALSE;
	}

	BOOL Attach(CWnd* pWnd, const COXDIB* pDIB, PaintType paintType=Tile, 
		COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL);
	//
	////////////////////////////////////////////////////


	// --- In  :	bRedraw				-	if TRUE then the attached window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:
	// --- Effect : Unhooks attached window if any was hooked
	void Detach(BOOL bRedraw=TRUE);


	// --- In  :	pDIB				-	pointer to COXDIB object that 
	//										will be used as wallpaper or NULL
	//										if you don't want to use any bitmap
	//										as wallpaper
	//				lpszDIBResourceName	-	resource name of the image that 
	//										will be used as wallpaper or NULL 
	//										if you don't want to use any bitmap 
	//										as wallpaper
	//				nDIBResourceID		-	resource ID of the image that will be 
	//										used as wallpaper or NULL if you 
	//										don't want to use any bitmap as 
	//										wallpaper
	//				sDIBFileName		-	full path to the image file that 
	//										will be used as wallpaper or empty 
	//										string if you don't want to use any 
	//										bitmap as wallpaper
	//				bRedraw				-	if TRUE then the hooked window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if new image was successfully set or FALSE otherwise
	// --- Effect : Resets the image that will be used to paint the background
	//
	inline BOOL SetWallpaper(LPCTSTR lpszDIBResourceName, BOOL bRedraw=TRUE)
	{
		COXDIB dib;
		if(lpszDIBResourceName!=NULL)
		{
			if(!dib.LoadResource(lpszDIBResourceName))
			{
				return FALSE;
			}
		}
		return SetWallpaper(&dib,bRedraw);
	}

	inline BOOL SetWallpaper(UINT nDIBResourceID, BOOL bRedraw=TRUE)
	{
		return SetWallpaper(MAKEINTRESOURCE(nDIBResourceID),bRedraw);
	}

	inline BOOL SetWallpaper(const CString& sDIBFileName, BOOL bRedraw=TRUE)
	{
		COXDIB dib;
		if(!sDIBFileName.IsEmpty())
		{
			if(!dib.Read(sDIBFileName))
			{
				return FALSE;
			}
		}
		if(SetWallpaper(&dib,bRedraw))
		{
			m_sImageFileName=sDIBFileName;
			return TRUE;
		}
		return FALSE;
	}

	inline BOOL SetWallpaper(const COXDIB* pDIB, BOOL bRedraw=TRUE)
	{
		ASSERT(IsHooked());

		m_sImageFileName.Empty();

		if(pDIB!=NULL)
		{
			m_dibWallpaper=*pDIB;	// copy
		}
		else
		{
			m_dibWallpaper.Empty();
		}
		DoRealizePalette(FALSE,TRUE);
		if(bRedraw && ::IsWindow(m_hWndHooked))
		{
			::RedrawWindow(m_hWndHooked,
				NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_UPDATENOW);
		}

		return TRUE;
	}
	//
	////////////////////////////////////////////////////

	// --- In  :	
	// --- Out : 
	// --- Returns:	pointer to the COXDIB object that is used internally
	//				in order to paint the background. It can be empty.
	// --- Effect : Retrieves the pointer to the COXDIB object that
	//				represents the image used to paint the background.
	//
	inline COXDIB* GetWallpaperImage() { return &m_dibWallpaper; }

	// --- In  :	
	// --- Out : 
	// --- Returns:	the name of the image file that is usead as wallpaper image
	// --- Effect : Retrieves the name of the image file used to paint the 
	//				background.
	inline CString GetWallpaperImageFileName() const { return m_sImageFileName; }


	// --- In  :	paintType			-	painting type. Refer to the 
	//										documentation on the Attach()
	//										function for the list of all
	//										possible values it can take.
	//				bRedraw				-	if TRUE then the hooked window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	
	// --- Effect : Resets the type of background image painting.
	//
	inline void SetPaintType(PaintType paintType, BOOL bRedraw=TRUE)
	{
		ASSERT(IsHooked());

		m_paintType=paintType;	// copy
		if(bRedraw && ::IsWindow(m_hWndHooked))
		{
			::RedrawWindow(m_hWndHooked,NULL,NULL,
				RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_UPDATENOW);
		}
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	the type of painting of the background. Refer to the 
	//				documentation on the Attach() function for the list 
	//				of all possible return values.
	// --- Effect : Retrieves the background painting type.
	//
	inline PaintType GetPaintType() const { return m_paintType; }


	// --- In  :	clrBk				-	background color, if CLR_DEFAULT is
	//										specified then default window 
	//										background color will be used. 
	//										Note that if you use image for
	//										background painting the areas that
	//										is not covered by the image will
	//										be filled with this color.
	//				bRedraw				-	if TRUE then the hooked window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the background color.
	inline void SetBkColor(COLORREF clrBk, BOOL bRedraw=TRUE)
	{
		ASSERT(IsHooked());

		m_clrBk=clrBk;	// copy
		if(bRedraw && ::IsWindow(m_hWndHooked))
		{
			::RedrawWindow(m_hWndHooked,NULL,NULL,
				RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_UPDATENOW);
		}
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	The color used to fill the backgroud.
	// --- Effect : Retrieves the background color.
	inline COLORREF GetBkColor() const { return m_clrBk; }


	// --- In  :	pWndOrigin			-	pointer to created window which
	//										top/left point of the paintable
	//										rectangle will be used as origin
	//										point while painting the background.
	//										If NULL is specified then the 
	//										window that is being attached will
	//										be used as origin one
	//				bRedraw				-	if TRUE then the hooked window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the window that defines origin point for background
	//				painting routines.
	inline void SetOriginWnd(CWnd* pOriginWnd, BOOL bRedraw=TRUE)
	{
		ASSERT(IsHooked());

		if(pOriginWnd==NULL)
		{
			m_pOriginWnd=GetHookedWnd();
		}
		else
		{
			m_pOriginWnd=pOriginWnd;
		}

		if(bRedraw && ::IsWindow(m_hWndHooked))
		{
			::RedrawWindow(m_hWndHooked,NULL,NULL,
				RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_UPDATENOW);
		}
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	Pointer to the window that serves as an origin one
	// --- Effect : Retrieves the pointer to the window which top/left
	//				point of its paintable area will serve as an origin point.
	inline CWnd* GetOriginWnd() const 
	{
		ASSERT(IsHooked());
		return m_pOriginWnd;
	}


	// --- In  :	pDC				-	pointer to device context to paint in 
	//									
	// --- Out : 
	// --- Returns:	
	// --- Effect : Paints in the specified device context. This function
	//				is called internally from WM_ERASEBKGND message handler.
	//				As long as most of standard controls don't implement 
	//				extended transparent style and paint their
	//				background in WM_PAINT handler you would probably like 
	//				to call this function explicitly if you draw the control
	//				yourself (e.g. use OWNERDRAW style).
	virtual void DrawWallpaper(CDC* pDC);

	// --- In  :	pDC				-	pointer to device context to paint in 
	//									
	// --- Out : 
	// --- Returns:	
	// --- Effect : Paints in the specified device context. This function
	//				is called internally from WM_ERASEBKGND message handler 
	//				in order to fill all areas that are not covered by
	//				background image or the whole paintable area of the window.
	//				As long as most of standard controls don't implement 
	//				extended transparent style and paint their
	//				background in WM_PAINT handler you would probably like 
	//				to call this function explicitly if you draw the control
	//				yourself (e.g. use OWNERDRAW style).
	virtual void FillBackground(CDC* pDC) const;


	// --- In  :	lpszProfileName	-	name of .ini file or hive in registry 
	//									where all information about 
	//									COXBackgroundPainter will be saved. 
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Saves COXBackgroundPainter state into registry or .ini file
	BOOL SaveState(LPCTSTR lpszProfileName);

	// --- In  :	lpszProfileName	-	name of .ini file or hive in registry 
	//									where all information about 
	//									COXBackgroundPainter was saved. 
	//				bApply			-	if equals TRUE then COXBackgroundPainter 
	//									will redraw the background right away.
	//									
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Loads COXBackgroundPainter state from registry or *.ini file
	BOOL LoadState(LPCTSTR lpszProfileName, BOOL bApply=TRUE);



// Overrides
protected:
	// handles all messages
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	// retrieves the window origin point for background painting
	virtual void GetWallpaperOrigin(CPoint& ptOrigin, CRect& rectContainer) const; 

	// helper function that supports 256> color bitmaps in the 
	// 256 color (8 bits) video mode.
	UINT DoRealizePalette(BOOL bRedraw=TRUE, BOOL bForeground=TRUE);

	// retrieves the rectangle that can be painted (includes client and 
	// non-client areas excluding window borders and window caption).
	CRect GetPaintableRect(CWnd* pWnd) const;

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBackgroundPainter();

protected:
	// image to be used as the background one
	COXDIB m_dibWallpaper;
	// painting type. Refer to Attach() function for details.
	PaintType m_paintType;
	// background color that will be used in order to fill the 
	// areas that are not covered by image
	COLORREF m_clrBk;
	// pointer to the window which top/left point of paintable rectangle
	// will serve as an origin point while painting the background
	CWnd* m_pOriginWnd;
	// filename of the image
	CString m_sImageFileName;

	// internal variable used to save the last known size of the 
	// hooked window
	CRect m_rectWindowSaved;
};

/////////////////////////////////////////////////////////////////////////////


class OX_CLASS_DECL COXBackgroundPainterOrganizer
{
// Construction
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXBackgroundPainterOrganizer()  : 
		m_pFrameWnd(NULL),
		m_dwFrameThreadID(NULL),
		m_pfnOldCBTHookProc(NULL), 
		m_pfnOldGetMessageHookProc(NULL),
		m_threadOrigPaintType(Tile),
		m_threadOrigBkColor(CLR_DEFAULT),
		m_threadOriginWnd(NULL) {};


	// --- In  :	pWnd				-	pointer to created window
	//				pDIB				-	pointer to COXDIB object that 
	//										will be used as wallpaper or NULL
	//										if you don't want to use any bitmap
	//										as wallpaper
	//				lpszDIBResourceName	-	resource name of the image that 
	//										will be used as wallpaper or NULL 
	//										if you don't want to use any bitmap 
	//										as wallpaper
	//				nDIBResourceID		-	resource ID of the image that will be 
	//										used as wallpaper or NULL if you 
	//										don't want to use any bitmap as 
	//										wallpaper
	//				sDIBFileName		-	full path to the image file that 
	//										will be used as wallpaper or empty 
	//										string if you don't want to use any 
	//										bitmap as wallpaper
	//				paintType			-	painting type. It must take one of 
	//										the following values:
	//
	//							Tile		-	image will be tiled to cover all
	//											paintable area of the specified
	//											window
	//							Stretch		-	image will be stretched to fill 
	//											all paintable area of the 
	//											specified window
	//							TopLeft		-	image will be drawn starting 
	//											from the left/top point of 
	//											the paintable area of the 
	//											specified window
	//							TopRight	-	image will be drawn starting 
	//											from the right/top point of 
	//											the paintable area of the 
	//											specified window
	//							TopCenter	-	image will be drawn starting 
	//											from the top side of the paintable 
	//											area of the specified window and 
	//											will be centered horizontally
	//							LeftCenter	-	image will be drawn starting 
	//											from the left side of the paintable 
	//											area of the specified window and 
	//											will be centered vertically
	//							RightCenter	-	image will be drawn starting 
	//											from the right side of the paintable 
	//											area of the specified window and 
	//											will be centered vertically
	//							Center		-	image will be centered vertically
	//											and horizontally
	//							BottomLeft	-	image will be drawn starting 
	//											from the left/bottom point of 
	//											the paintable area of the 
	//											specified window
	//							BottomRight	-	image will be drawn starting 
	//											from the right/bottom point of 
	//											the paintable area of the 
	//											specified window
	//							BottomCenter-	image will be drawn starting 
	//											from the bottom side of the paintable 
	//											area of the specified window and 
	//											will be centered horizontally	
	//
	//				clrBk				-	background color, if CLR_DEFAULT is
	//										specified then default window 
	//										background color will be used. 
	//										Note that if you use image for
	//										background painting the areas that
	//										is not covered by the image will
	//										be filled with this color.
	//
	//				pWndOrigin			-	pointer to created window which
	//										top/left point of the paintable
	//										rectangle will be used as origin
	//										point while painting the background.
	//										If NULL is specified then the 
	//										window that is being attached will
	//										be used as origin one
	// --- Out : 
	// --- Returns:	TRUE if pWnd was successfully hooked, otherwise FALSE
	// --- Effect : Creates COXBackgroundPainter object for specified window 
	//				and attaches the specified window to it. Refer to the 
	//				documentation on COXBackgroundPainter::Attach() for 
	//				additional info.
	//
	inline BOOL Attach(CWnd* pWnd, LPCTSTR lpszDIBResourceName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		COXDIB dib;
		if(lpszDIBResourceName!=NULL)
			if(!dib.LoadResource(lpszDIBResourceName))
				return FALSE;
		return Attach(pWnd,&dib,paintType,clrBk,pOriginWnd);
	}

	inline BOOL Attach(CWnd* pWnd, UINT nDIBResourceID, PaintType paintType=Tile, 
		COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		return Attach(pWnd,MAKEINTRESOURCE(nDIBResourceID),paintType,clrBk,pOriginWnd);
	}

	inline BOOL Attach(CWnd* pWnd, const CString& sDIBFileName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL)
	{
		COXDIB dib;
		if(!sDIBFileName.IsEmpty())
			if(!dib.Read(sDIBFileName))
				return FALSE;
		return Attach(pWnd,&dib,paintType,clrBk,pOriginWnd);
	}

	BOOL Attach(CWnd* pWnd, const COXDIB* pDIB, PaintType paintType=Tile,
		COLORREF clrBk=CLR_DEFAULT, CWnd* pOriginWnd=NULL);
	//
	////////////////////////////////////////////////////


	// --- In  :	pWnd	-	pointer to the attached window or NULL. 
	//							If NULL is specified then all attached
	//							windows will be unhooked.
	//				bRedraw	-	if TRUE then the attached window 
	//							will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if function succeeded, or FALSE otherwise
	// --- Effect : Unhooks specified or all attached window(s).
	BOOL Detach(const CWnd* pWnd=NULL, BOOL bRedraw=TRUE);


	// --- In  :	pFrameWnd			-	pointer to created mainframe window
	//										(main window in SDI or MDI interface)
	//				pDIB				-	pointer to COXDIB object that 
	//										will be used as wallpaper or NULL
	//										if you don't want to use any bitmap
	//										as wallpaper
	//				lpszDIBResourceName	-	resource name of the image that 
	//										will be used as wallpaper or NULL 
	//										if you don't want to use any bitmap 
	//										as wallpaper
	//				nDIBResourceID		-	resource ID of the image that will be 
	//										used as wallpaper or NULL if you 
	//										don't want to use any bitmap as 
	//										wallpaper
	//				sDIBFileName		-	full path to the image file that 
	//										will be used as wallpaper or empty 
	//										string if you don't want to use any 
	//										bitmap as wallpaper
	//				paintType			-	painting type. Refer to Attach() 
	//										function for the list of possible
	//										values
	//				clrBk				-	background color, if CLR_DEFAULT is
	//										specified then default window 
	//										background color will be used. 
	//										Note that if you use image for
	//										background painting the areas that
	//										is not covered by the image will
	//										be filled with this color.
	// --- Out : 
	// --- Returns:	TRUE if the mainframe window was successfully hooked, 
	//				otherwise FALSE
	// --- Effect : Creates COXBackgroundPainter objects for the mainframe
	//				child windows (toolbars, menubar, 
	//				MDIClient (MDI interface), etc.) and attaches the 
	//
	inline BOOL AttachFrame(CFrameWnd* pFrameWnd, LPCTSTR lpszDIBResourceName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT)
	{
		COXDIB dib;
		if(lpszDIBResourceName!=NULL)
			if(!dib.LoadResource(lpszDIBResourceName))
				return FALSE;
		return AttachFrame(pFrameWnd,&dib,paintType,clrBk);
	}

	inline BOOL AttachFrame(CFrameWnd* pFrameWnd, UINT nDIBResourceID, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT)
	{
		return AttachFrame(pFrameWnd,MAKEINTRESOURCE(nDIBResourceID),
			paintType,clrBk);
	}

	inline BOOL AttachFrame(CFrameWnd* pFrameWnd, const CString& sDIBFileName, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT)
	{
		COXDIB dib;
		if(!sDIBFileName.IsEmpty())
			if(!dib.Read(sDIBFileName))
				return FALSE;
		return AttachFrame(pFrameWnd,&dib,paintType,clrBk);
	}

	BOOL AttachFrame(CFrameWnd* pFrameWnd, const COXDIB* pDIB, 
		PaintType paintType=Tile, COLORREF clrBk=CLR_DEFAULT);
	//
	////////////////////////////////////////////////////


	// --- In  :	bRedraw				-	if TRUE then the attached windows 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if all windows were successfully unhooked, 
	//				otherwise FALSE
	// --- Effect : Detaches all previously attached windows that has been 
	//				hooked by the AttachFrame()	function
	BOOL DetachFrame(BOOL bRedraw=TRUE);


	// --- In  :	pWnd				-	pointer to the attached window 
	//										or NULL. If NULL is specified then 
	//										the image will be set to 
	//										all attached windows.
	//				pDIB				-	pointer to COXDIB object that 
	//										will be used as wallpaper or NULL
	//										if you don't want to use any bitmap
	//										as wallpaper
	//				lpszDIBResourceName	-	resource name of the image that 
	//										will be used as wallpaper or NULL 
	//										if you don't want to use any bitmap 
	//										as wallpaper
	//				nDIBResourceID		-	resource ID of the image that will be 
	//										used as wallpaper or NULL if you 
	//										don't want to use any bitmap as 
	//										wallpaper
	//				sDIBFileName		-	full path to the image file that 
	//										will be used as wallpaper or empty 
	//										string if you don't want to use any 
	//										bitmap as wallpaper
	//				bRedraw				-	if TRUE then the attached window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if new image was successfully set or FALSE otherwise
	// --- Effect : Resets the image that will be used to paint the background
	//
	inline BOOL SetWallpaper(const CWnd* pWnd, LPCTSTR lpszDIBResourceName,
		BOOL bRedraw=TRUE)
	{
		COXDIB dib;
		if(lpszDIBResourceName!=NULL)
			if(!dib.LoadResource(lpszDIBResourceName))
				return FALSE;
		return SetWallpaper(pWnd,&dib,bRedraw);
	}

	inline BOOL SetWallpaper(const CWnd* pWnd, UINT nDIBResourceID, BOOL bRedraw=TRUE)
	{
		return SetWallpaper(pWnd,MAKEINTRESOURCE(nDIBResourceID),bRedraw);
	}

	BOOL SetWallpaper(const CWnd* pWnd, const CString& sDIBFileName, BOOL bRedraw=TRUE);

	BOOL SetWallpaper(const CWnd* pWnd, const COXDIB* pDIB, BOOL bRedraw=TRUE);
	//
	////////////////////////////////////////////////////


	// --- In  :	pWnd				-	pointer to the attached window 
	//										or NULL. If NULL is specified then 
	//										the paint type will be applied to 
	//										all attached windows.
	//				paintType			-	painting type. Refer to the 
	//										documentation on the Attach()
	//										function for the list of all
	//										possible values it can take.
	//				bRedraw				-	if TRUE then the attached window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if new paint type  was successfully set 
	//				or FALSE otherwise
	// --- Effect : Resets the type of background image painting.
	//
	BOOL SetPaintType(const CWnd* pWnd,	PaintType paintType, BOOL bRedraw=TRUE);


	// --- In  :	pWnd				-	pointer to the attached window 
	//										or NULL. If NULL is specified then 
	//										the background color will be 
	//										applied to all attached windows.
	//				clrBk				-	background color, if CLR_DEFAULT is
	//										specified then default window 
	//										background color will be used. 
	//										Note that if you use image for
	//										background painting the areas that
	//										is not covered by the image will
	//										be filled with this color.
	//				bRedraw				-	if TRUE then the attached window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	TRUE if new background color was successfully set 
	//				or FALSE otherwise
	// --- Effect : Sets the background color.
	BOOL SetBkColor(const CWnd* pWnd, COLORREF clrBk, BOOL bRedraw=TRUE);


	// --- In  :	pWnd				-	pointer to the attached window 
	//										or NULL. If NULL is specified then 
	//										the background color will be 
	//										applied to all attached windows.
	//				pWndOrigin			-	pointer to created window which
	//										top/left point of the paintable
	//										rectangle will be used as origin
	//										point while painting the background.
	//										If NULL is specified then the 
	//										window that is being attached will
	//										be used as origin one
	//				bRedraw				-	if TRUE then the hooked window 
	//										will be redrawn
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the background color.
	BOOL SetOriginWnd(const CWnd* pWnd, CWnd* pOriginWnd, BOOL bRedraw=TRUE);

		
	// --- In  :	pWnd				-	pointer to the attached window
	// --- Out : 
	// --- Returns:	Pointer to COXBackgroundPainter object that	corresponds 
	//				to the specified attached window if succeed or NULL
	//				otherwise
	// --- Effect : Retrieves the pointer to COXBackgroundPainter object that
	//				corresponds to the specified attached window.
	COXBackgroundPainter* GetPainter(const CWnd* pWnd) const;


	// --- In  :	pWnd				-	pointer to the window
	// --- Out : 
	// --- Returns:	TRUE if specified window was hooked before using 
	//				Attach function or FALSE otherwise
	// --- Effect : Retrieves the flag that specifies whether the given 
	//				window was attached before or not.
	BOOL IsAttached(const CWnd* pWnd) const;


	// --- In  :	pWnd			-	pointer to the attached window 
	//									or NULL. If NULL is specified then 
	//									the saved COXBackgroundPainter settings 
	//									will be applied to all attached windows.
	//				lpszProfileName	-	name of .ini file or hive in registry 
	//									where all information about 
	//									COXBackgroundPainter was saved. 
	//				bApply			-	if equals TRUE then COXBackgroundPainter 
	//									will redraw the background right away.
	//									
	// --- Out : 
	// --- Returns:	TRUE if succeeds, or FALSE otherwise.
	// --- Effect : Loads COXBackgroundPainter state from registry or *.ini file
	BOOL LoadState(const CWnd* pWnd, LPCTSTR lpszProfileName, BOOL bApply=TRUE);

	inline BOOL IsAttachedFrame() const { return (m_pFrameWnd!=NULL); }
	inline CFrameWnd* GetAttachedFrame() const { return m_pFrameWnd; }
	inline DWORD GetAttachedFrameThreadID() const { return m_dwFrameThreadID; }

	inline HHOOK GetSavedGetMessageHookProc() const 
		{ return m_pfnOldGetMessageHookProc; }
	static HHOOK GetOriginalGetMessageHookProc() 
		{ return COXBackgroundPainterOrganizer::m_pfnOriginalGetMessageHookProc; }
	inline HHOOK GetSavedCBTHookProc() const { return m_pfnOldCBTHookProc; }
	static HHOOK GetOriginalCBTHookProc() 
		{ return COXBackgroundPainterOrganizer::m_pfnOriginalCBTHookProc; }

	inline COXBackgroundPainter* GetFirstPainter(POSITION& pos) const 
	{
		pos=m_arrAttachedWnd.GetStartPosition();
		HWND hWnd=NULL;
		COXBackgroundPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hWnd,pPainter);
			if(::IsWindow(hWnd))
			{
				ASSERT(pPainter!=NULL);
				break;
			}
			else
			{
				pPainter=NULL;
			}
		}
		return pPainter;
	}
	
	inline COXBackgroundPainter* GetNextPainter(POSITION& pos) const 
	{
		if(pos==NULL)
			return NULL;
		HWND hWnd=NULL;
		COXBackgroundPainter* pPainter=NULL;
		while(pos!=NULL)
		{
			m_arrAttachedWnd.GetNextAssoc(pos,hWnd,pPainter);
			if(::IsWindow(hWnd))
			{
				ASSERT(pPainter!=NULL);
				break;
			}
			else
			{
				pPainter=NULL;
			}
		}
		return pPainter;
	}

	void AttachAllWindows(HWND hWndStartFrom);

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXBackgroundPainterOrganizer();

protected:
	virtual BOOL IsQualified(CWnd* pWnd);


public:
	// map of all organizers that has been created to handle caption painting
	// for all windows created by particular thread
	static CMap<DWORD,DWORD,COXBackgroundPainterOrganizer*,
		COXBackgroundPainterOrganizer*> m_arrFrameOrganizers;

	// original DIB used in attaching all window in the frame (refer to 
	// AttachFrame() function)
	COXDIB m_threadOrigDIB;
	// original PaintType used in attaching all windows in the frame
	PaintType m_threadOrigPaintType;
	// original background color used in attaching all windows in the frame
	COLORREF m_threadOrigBkColor;
	// original window that defines the origin point used in attaching all 
	// windows in the frame
	CWnd* m_threadOriginWnd;


protected:
	// map of all attached windows and associated COXBackgroundPainter objects
	CMap<HWND,HWND,COXBackgroundPainter*,COXBackgroundPainter*> m_arrAttachedWnd;
	// array of all used COXCaptionPainter objects
	CArray<COXBackgroundPainter*,COXBackgroundPainter*> m_arrUsedPainters;

	// pointer to the attached frame 
	CFrameWnd* m_pFrameWnd;
	// thread ID that created the mainframe window
	DWORD m_dwFrameThreadID;
	

	// handle of the old CBT hook procedure
	HHOOK m_pfnOldCBTHookProc;
	// handle of the original CBT hook procedure
	static HHOOK m_pfnOriginalCBTHookProc;

	// handle of the old GetMessage hook procedure
	HHOOK m_pfnOldGetMessageHookProc;
	// handle of the original GetMessage hook procedure
	static HHOOK m_pfnOriginalGetMessageHookProc;


	// hook procedure for CBT
	static LRESULT CALLBACK BackgroundPainterCBTHookProc(
		int nCode, WPARAM wParam, LPARAM lParam);

	// hook procedure for GetMessage
	static LRESULT CALLBACK BackgroundPainterGetMessageHookProc(
		int nCode, WPARAM wParam, LPARAM lParam);

	// callback for enumerating topmost windows
	static BOOL CALLBACK EnumThreadWindows(HWND hWnd, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXBACKGROUNDPAINTER_H_)
