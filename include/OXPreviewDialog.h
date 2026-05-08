// ==========================================================================
// 					Class Specification : COXPreviewDialog
// ==========================================================================
//
// Header file : OXPreviewDialog.h
//
// Version: 9.3
//
// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
//
// //////////////////////////////////////////////////////////////////////////

/*

  OVERVIEW

COXPreviewDialog is CFileDialog derived class that extends the functionality 
of the standard File Open / Save As... dialog. It is designed to add preview 
functionality when new OFN_EXPLORER style is specified (this style is set by 
default). 

When COXPreviewDialog is used instead of CFileDialog the new "Preview"
check button will appear on the dialog's toolbar. By clicking on this button
a user can set the preview mode on and off. By default the preview mode is on.

When the preview mode is set on the dialog's list control that displays files and 
folders is being shrunken to make a space for preview window. The space between 
the list control and preview window is designated as "splitter" area. If a user 
left clicks over the splitter area and move the mouse cursor while holding the 
left button the list control and preview window will be resized. 

COXPreviewDialog supports to full extent resizing functionality provided by 
Windows 98/2000

COXPreviewDialog "memorizes" its last state (preview mode state and preview window 
position) and restores next time the dialog is instantiated.


Above we provided overview on how "preview area" is being designated in the dialog 
window. Now lets move to the issue of previewing the contents of the selected 
file. Obviously, the problem here is to provide the functionality for previewing as 
many standard file types as possible while giving the flexibility of previewing only 
designated file types (e.g. only image files or only text files) and allowing a 
programmer to easily update the functionality of the class in order to preview 
proprietary file types.

In order to implement that kind of functionality we have introduced "file viewer"
object that is responsible for previewing of selecting files. Programmer can create 
and register with COXPreviewDialog as many file viewer objects as he would like to. 
So using file viewer paradigm we can create a number of file viewer objects that will 
be responsible for handling different kind of file types. Then in any particular case 
we can use any file viewer in any order in order to provide a preview functionality
for particular file types.

File viewers are implemented through COXFileViewer class that has the following set 
of virtual functions:

 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
	virtual void OnChangeSize();
	virtual void OnPreview(BOOL bPreview);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 

Refer to the COXFileViewer class reference for details on this functions. Below 
you will find guidelines on creating your own file viewers.

1)	Override OnNewFileSelect() function. This function will be called by 
	COXPreviewDialog object for every registered file viewer when new file 
	is selected in the list control that displays files and folders. In your
	implementation you have to analyze the selected file type and decide whether
	your object will handle the preview functionality for the selected file or not. 
	Retrurn TRUE if you will preview the selected file or FALSE otherwise. Note
	that the second parameter of this function specifies if file viewer for
	the selected file has already been found. It could happen if you use more than 
	one file viewer object registered with COXPreviewDialog that can preview
	the same file type.

2)	Override OnPaintPreview() function. This function will be called by 
	COXPreviewDialog object for every registered file viewer when selected file 
	hast to be previewed. In your implementation you have to draw the preview
	of currently selected file (of course if this object can preview the selected 
	file) or if you use another control for displaying the preview just return TRUE.
	If file viewer doesn't support the selected file type you have to return FALSE.

3)	Override OnDoRealizePalette() function if you need to implement palette support 
	for your preview drawing routine. This function will be called by 
	COXPreviewDialog object for every registered file viewer before drawing the 
	preview. Return TRUE if this object is going to preview the currently selected 
	file even if you don't need to support palette; otherwise return FALSE.

4)	If file viewer object uses additional control in order to display the preview
	you would probably have to override the following functions

		OnChangeSize()		-	called every time the preview area is resized
		OnPreview()			-	called every time the preview mode changes its value
		OnCreatePreviewWnd()-	called right after the preview window has been created.
								Usually you would use preview window as a parent of
								the additional control(s)
		OnDestroyPreviewWnd()-	called just before destroying the preview window.
								Here you can destroy you additional control(s)


In this overview we mentioned that file viewer objects must be registered with
COXPreviewDialog in order to be used. To do that you can call the following functions

	int AddFileViewer(COXFileViewer* pFileViewer);
	BOOL InsertFileViewer(COXFileViewer* pFileViewer, int nIndex);
	BOOL RemoveFileViewer(int nIndex);

The order in which file viewer objects are registered with COXPreviewDialog is 
important because all COXFileViewer functions will be called for file viewer objects
in the same order they were inserted.


There are a few additional functions implemented in the COXPreviewDialog class 
you might consider to be useful. All of them provide a programmer with 
functionality to programmatically specify the look and feel of the Dialog
that user would normally do instead.

	void SetPreview(BOOL bPreview);
	BOOL IsInPreviewMode() const;
	void SetFlatToolBar(BOOL bFlat);
	BOOL IsFlatToolBar() const;
	int SetPreviewWndOffset(int nPreviewWndOffset);
	int GetPreviewWndOffset() const;



We have implemented for you a number of file viewer objects for previewing most 
of the file types. Below we will give you the details on these objects.

1)	COXGraphicsFileViewer. Designed for previewing image files. Put the following 
	define in your stdafx.h in order to use this file viewer with COXPreviewDialog 

	#define OX_PREVIEW_GRAPHICS

	COXGraphicsFileViewer can optioanlly preview bitmaps (*.bmp,*.dib), 
	jpegs (*.jpeg),	icons (*.ico, *.cur, *.ani, *.icl) and metafiles (*.emf, *.wmf).
	Again you have your choice on what files are going to be previewed through 
	defines:
	
	#define OX_PREVIEW_BMP			-	bitmaps
		#define OXDIB_SUPPORTJPEG	-	jpeg (OX_PREVIEW_BMP is required as well)
	#define OX_PREVIEW_METAFILE		-	metafiles
	#define OX_PREVIEW_ICON			-	icons


2)	COXRTFFileViewer. Previews RTF files only. Put the following 
	define in your stdafx.h in order to use this file viewer with COXPreviewDialog 

	#define OX_PREVIEW_RTF


3)	COXHTMLFileViewer. Previews HTML files and a number of other formats if
	corresponding application is installed (e.g. MS Word documents or MS Excel sheets).
	Internally uses WebControl and IWebBrowser2 interface that is implemented
	by Internet Explorer v4 and higher. So this is a must requirement to have at 
	least IE 4 in order to use this file viewer. Put the following define in 
	your stdafx.h in order to use this file viewer with COXPreviewDialog 

	#define OX_PREVIEW_HTML


4)	COXTextFileViewer. Previews any file, which size lesser than 64 Kb, as a plain 
	text file. Put the following define in your stdafx.h in order to use this file 
	viewer with COXPreviewDialog 

	#define OX_PREVIEW_PLAINTEXT


Using above specified defines you can build your own version of COXPreviewDialog 
object that can preview almost any type of files just by using COXPreviewDialog
instead of CFileDialog!



!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Important

COXPreviewDialog succeeds the class with the same name and COXBmpPrv class that were
introduced long time ago and were designed to handle the same issue though using
a litlle bit different approach. Both of this classes are now considered retired and
moved in to the Archive section of the Ultimate Toolbox. 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



Check out Preview sample that can be found in the ".\Samples\gui\Preview" 
subdirectory of your Ultimate Toolbox directory. It demonstrates how to replace 
standard File Open dialog on COXPreview dialog in MDI application.



Dependency:
	#include "OXPreviewDialog.h"


Source code files:
	"OXPreviewDialog.cpp"		
	#ifdef OX_PREVIEW_BMP		-	"oxdib.cpp"
	#ifdef OX_PREVIEW_METAFILE	-	"OXMetaFile.cpp"
	#ifdef OX_PREVIEW_ICON		-	"OXImageListBox.cpp"
	#ifdef OXDIB_SUPPORTJPEG	-	"OXBMPFle.cpp"
									"OXGphFle.cpp"
									"OXJPGCod.cpp"
									"OXJPGCom.cpp"
									"OXJPGDom.cpp"
									"OXJPGExp.cpp"
									"OXJPGFle.cpp"

Resource files:
	"OXPreviewDialog.rc"

*/


/////////////////////////////////////////////////////////////////////////////
#ifndef __OXPREVIEWDIALOG_H__
#define __OXPREVIEWDIALOG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif

#include "OXDllExt.h"
#include "OXMainRes.h"


#ifdef OX_PREVIEW_BMP
#include "oxdib.h"
#endif

#ifdef OX_PREVIEW_METAFILE
#include "OXMetaFile.h"
#endif

#ifdef OX_PREVIEW_ICON
#include "OXImageListBox.h"
#endif

#ifdef OX_PREVIEW_HTML
#ifndef __AFXDISP_H__
#include <afxdisp.h>
#define __AFXDISP_H__
#endif
#endif

#ifdef OX_PREVIEW_SOUND
#include "OXSound.h"
#endif

#include "UTB64Bit.h"


//////////////////////////////////////////////////////////////////////////////
class OX_CLASS_DECL COXFilePath
{
public:
	COXFilePath() : m_sFilePath(_T("")) {};
	COXFilePath(LPCTSTR pszPath) : m_sFilePath(pszPath) {};

protected:
	CString m_sFilePath;

public:
	inline CString GetPath() const { return m_sFilePath; }
	inline void SetPath(LPCTSTR pszPath) { m_sFilePath=pszPath; }

	inline BOOL Exists()
	{
		CFileStatus fileStatus;
		return CFile::GetStatus(GetPath(), fileStatus);
	}

	inline CString GetExtender()
	{
		int nFullStopPosition=GetPath().ReverseFind(_T('.'));
		if(nFullStopPosition!=-1)
		{
			return GetPath().Mid(nFullStopPosition+1);
		}
		else
		{
			return _T("");
		}
	}
};


//////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXPreviewWnd : public CWnd
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Construct the object
	COXPreviewWnd() : m_pPreviewDlg(NULL) {};


	// --- In  :	pParent	-	parent window
	//				rect	-	window rectangle
	//				nID		-	window ID
	// --- Out : 
	// --- Returns:	TRUE if window was successfully created
	// --- Effect : Creates the window
	virtual BOOL Create(CWnd* pParent, CRect rect, UINT nID);


public:
	// pointer to COXPreviewDialog object that contains this window
	class COXPreviewDialog* m_pPreviewDlg;


protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXFileViewer
{
public:
	// pointer to COXPreviewDialog object that owns this object
	class COXPreviewDialog* m_pPreviewDialog;

protected:
	// flag that specifies if currently selected file can be viewed using 
	// this file viewer object
	BOOL m_bCanPreview;

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXFileViewer() : m_pPreviewDialog(NULL), m_bCanPreview(FALSE) {};


	// --- In  :	sFilename	-	full path to new selected file
	//				bViewerFound-	set to TRUE if viewer for the selected file
	//								has already been found
	// --- Out : 
	// --- Returns: TRUE if the object can preview the selected file
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object when new file is selected. Return TRUE 
	//				if this object can preview the specified file
	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE) 
	{
		UNREFERENCED_PARAMETER(sFilename);
		UNREFERENCED_PARAMETER(bViewerFound);
		return FALSE;
	}


	// --- In  :	pDC			-	device context to draw in
	//				paintRect	-	rectangle to draw in
	// --- Out : 
	// --- Returns: TRUE if the object has painted the image of the selected file
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object to draw the preview of the selected file. 
	//				Return TRUE if this object can draw the preview of the specified 
	//				file
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect)
	{
		UNREFERENCED_PARAMETER(pDC);
		UNREFERENCED_PARAMETER(paintRect);
		return FALSE;
	}


	// --- In  :	pFocusWnd	-	window that have the focus at the moment
	// --- Out : 
	// --- Returns: TRUE if the object has realized its own palette
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object before drawing the preview of the selected 
	//				file. Return TRUE if this object handles the palette
	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd)
	{
		UNREFERENCED_PARAMETER(pFocusWnd);
		return FALSE;
	}

	
	// --- In  :	
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object right after the preview area has 
	//				been resized
	virtual void OnChangeSize() {};


	// --- In  :	bPreview	-	if TRUE then preview mode is set on;
	//								if FALSE then preview mode is set off
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object right after the preview mode has been
	//				changed
	virtual void OnPreview(BOOL bPreview) { UNREFERENCED_PARAMETER(bPreview); }


	// --- In  :	pPreviewWnd	-	pointer to created COXPreviewWnd object that will
	//								host preview area
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object right after the COXPreviewWnd object 
	//				that will host preview area has been created. If you need to 
	//				use your own object for preview purposes you must create
	//				in this function and specify pPreviewWnd as its parent window
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd) 
	{
		UNREFERENCED_PARAMETER(pPreviewWnd);
	}


	// --- In  :	pPreviewWnd	-	pointer to COXPreviewWnd object that hosts 
	//								preview area
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called by COXPreviewDialog object for every registered 
	//				COXFileViewer object right before the COXPreviewWnd object 
	//				that hosts preview area is destroyed. Any object that has been 
	//				created for preview purposes might be destroyed here
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd) 
	{
		UNREFERENCED_PARAMETER(pPreviewWnd);
	}


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if this object can preview the currently selected file;
	//				otherwise FALSE
	// --- Effect : Retrieves a flag that specifies if the viewer can preview 
	//				the currently selected file
	virtual BOOL CanPreview() { return m_bCanPreview; }
};

//////////////////////////////////////////////////////////////////////////////

#ifdef OX_PREVIEW_GRAPHICS
class OX_CLASS_DECL COXGraphicsFileViewer : public COXFileViewer
{
protected:
#ifdef OX_PREVIEW_ICON
	// for displaying icons and cursors
	class COXImageListBox m_imageListBox;
#endif
#ifdef OX_PREVIEW_BMP
	// for displaying bitmaps and jpegs
	COXDIB m_dib;
#endif
#ifdef OX_PREVIEW_METAFILE
	// for displaying metafiles
	COXMetaFile m_MetaFile;
#endif

public:
 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
	virtual void OnChangeSize();
	virtual void OnPreview(BOOL bPreview);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 
};
#endif	//	OX_PREVIEW_GRAPHICS


#ifdef OX_PREVIEW_PLAINTEXT
class OX_CLASS_DECL COXTextFileViewer : public COXFileViewer
{
protected:
	// for displaying text files with size < 64K
	CEdit m_edit;

public:
 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
	virtual void OnChangeSize();
	virtual void OnPreview(BOOL bPreview);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 
};
#endif	//	OX_PREVIEW_PLAINTEXT


#ifdef OX_PREVIEW_RTF
class OX_CLASS_DECL COXRTFFileViewer : public COXFileViewer
{
protected:
	// for displaying RTF files
	CRichEditCtrl m_richEdit;
	// helper object 
	CFile m_file;

public:
 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
	virtual void OnChangeSize();
	virtual void OnPreview(BOOL bPreview);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 

protected:
	// loads the currently selected file as RTF file
	void LoadRTF();
	// check if specified file object represents RTF file
	static BOOL IsRTF(CFile* pFile);
	// callback function for populating rich edit control
	static DWORD CALLBACK RichEditStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, 
		LONG cb, LONG FAR *pcb); 
};
#endif	//	OX_PREVIEW_RTF


#ifdef OX_PREVIEW_HTML
class OX_CLASS_DECL COXHTMLFileViewer : public COXFileViewer
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXHTMLFileViewer() : m_pIWebBrowser(NULL) {};

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	~COXHTMLFileViewer()
	{
		if(m_pIWebBrowser!=NULL)
		{
			m_pIWebBrowser->Release();
			m_pIWebBrowser=NULL;
		}
	}

protected:
	// HTML browser ActiveX control window
	CWnd m_HTMLBrowserWnd;
	// pointer to IWebBrowser2 interface for navigation purposes
	IWebBrowser2* m_pIWebBrowser;

public:
 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
	virtual void OnChangeSize();
	virtual void OnPreview(BOOL bPreview);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 
};
#endif	//	OX_PREVIEW_HTML


#ifdef OX_PREVIEW_SOUND

#ifndef OXSOUNDPREVIEW_BORDER_OFFSET
#define	OXSOUNDPREVIEW_BORDER_OFFSET				5
#endif

#ifndef OXSOUNDPREVIEW_BUTTON_HEIGHT
#define	OXSOUNDPREVIEW_BUTTON_HEIGHT				24
#endif
#ifndef OXSOUNDPREVIEW_BUTTON_WIDTH
#define	OXSOUNDPREVIEW_BUTTON_WIDTH					24
#endif

#ifndef IDC_OXSOUNDPREVIEW_BUTTON_START_PLAYING	
#define	IDC_OXSOUNDPREVIEW_BUTTON_START_PLAYING		1000
#endif
#ifndef IDC_OXSOUNDPREVIEW_BUTTON_STOP_PLAYING	
#define	IDC_OXSOUNDPREVIEW_BUTTON_STOP_PLAYING		1001
#endif


class OX_CLASS_DECL COXSoundFileViewer : public COXFileViewer
{
protected:
	class OX_CLASS_DECL COXSoundFileViewerHelper : public CWnd
	{
	public:
		// sound object
		COXSound m_sound;

		// buttons for playing sound
		CButton m_btnStartPlaying;
		CButton m_btnStopPlaying;

		// sound info 
		CString m_sSoundInfo;
		// rectangle for sound info text
		CRect m_rectSoundInfo;

	protected:
		//{{AFX_MSG(COXSoundFileViewerHelper)
		afx_msg LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
		afx_msg void OnStartPlaying();
		afx_msg void OnStopPlaying();
		afx_msg LRESULT OnPlayComplete(WPARAM wParam, LPARAM lParam);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	} m_wndHelper;

public:
 	virtual BOOL OnNewFileSelect(const CString& sFilename, BOOL bViewerFound=FALSE);
	virtual void OnCreatePreviewWnd(COXPreviewWnd* pPreviewWnd);
	virtual void OnDestroyPreviewWnd(COXPreviewWnd* pPreviewWnd); 
	virtual void OnPreview(BOOL bPreview);
	virtual void OnChangeSize();
	virtual BOOL OnPaintPreview(CDC* pDC, const CRect& paintRect);
 	virtual BOOL OnDoRealizePalette(CWnd* pFocusWnd);
};
#endif	//	OX_PREVIEW_SOUND



/////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXPreviewDialog : public CFileDialog
{
DECLARE_DYNAMIC(COXPreviewDialog)


public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Contructs the object
	COXPreviewDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt=NULL,
		LPCTSTR lpszFileName=NULL,
		DWORD dwFlags=OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter=NULL, CWnd* pParentWnd=NULL);

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXPreviewDialog();

	
	// --- In  :	rect		-	rectangle to adjust
	//				rectBound	-	rectangle to fit into
	// --- Out : 
	// --- Returns: The rectangle centered in the bounding rectangle. If rect 
	//				doesn't fit rectBound then before centering it the function 
	//				scales it
	// --- Effect : Adjusts the coordinates of the specified rectangle to be 
	//				centered in the bounding rectangle. The given rectangle might 
	//				be scaled in order to fit the bounding rectangle
	static CRect AdjustRectToFit(CRect rect, CRect rectBound);


	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if old style of the file open dialog is used; 
	//				FALSE otherwise
	// --- Effect : Retrieves the flag that specifies if old style of the
	//				dialog is being used
	inline BOOL IsOldStyle() const { return ((m_ofn.Flags & OFN_EXPLORER)==0); }


	// --- In  :	pFileViewer	-	pointer to COXFileViewer object
	// --- Out : 
	// --- Returns: The index number of the added file viewer if succeeded;
	//				otherwise -1
	// --- Effect : Adds new COXFileViewer object at the end of the list
	//				of file viewers. When an event important for file viewer 
	//				object is fired the corresponding COXFileViewer function 
	//				will be called for all registered file viewers 
	inline int AddFileViewer(COXFileViewer* pFileViewer)
	{
		return (InsertFileViewer(pFileViewer,GetFileViewerCount()) ? 
			GetFileViewerCount()-1 : -1);
	}

	// --- In  :	pFileViewer	-	pointer to COXFileViewer object
	//				nIndex		-	the index of the position in the list of 
	//								file viewers where the new file viewer
	//								object will be displayed at
	// --- Out : 
	// --- Returns: TRUE if succeeded;	otherwise FALSE
	// --- Effect : Inserts new COXFileViewer object in the list of file viewers
	//				at the specified position. When an event important for file viewer 
	//				object is fired the corresponding COXFileViewer function 
	//				will be called for all registered file viewers 
	BOOL InsertFileViewer(COXFileViewer* pFileViewer, int nIndex);

	// --- In  :	nIndex		-	the index of the position in the list of 
	//								file viewers that identifies the viewer 
	//								to be deleted
	// --- Out : 
	// --- Returns: TRUE if succeeded;	otherwise FALSE
	// --- Effect : Removes existing COXFileViewer object from the list of file 
	//				viewers at the specified position
	BOOL RemoveFileViewer(int nIndex);

	// --- In  :	nIndex		-	the index of the position in the list of 
	//								file viewers where the new file viewer
	//								object will be displayed at
	// --- Out : 
	// --- Returns: A pointer to COXFileViewer object at the specified position 
	//				if succeeded;	otherwise NULL
	// --- Effect : Retrieves a pointer to specified COXFileViewer object in the 
	//				list of file viewers at the specified position 
	COXFileViewer* GetFileViewer(int nIndex) const;

	// --- In  :	
	// --- Out : 
	// --- Returns: The number of registered COXFileViewer objects
	// --- Effect : Retrieves the number of registered COXFileViewer objects
	inline int GetFileViewerCount() const { return PtrToInt(m_arrFileViewers.GetSize()); }


	// --- In  :	
	// --- Out : 
	// --- Returns: A pointer to the window that hosts the preview area
	// --- Effect : Retrieves a pointer to the window that hosts the preview area
	inline COXPreviewWnd* GetPreviewWnd() { return &m_preview; }


protected:
	// flag that specifiew the preview mode state
	BOOL m_bPreview;
	// the last remembered value of the flag that specifiew the preview mode state
	BOOL m_bPrevPreview;
	// the path to the currently selected file
	CString m_sSelectedFile;
	// the last remembered path to the selected file
	CString m_sPrevSelectedFile;
	// frame object for old style dialog
	CStatic m_previewFrame;

	// array of pointers to registered COXFileViewer objects
	CArray<COXFileViewer*,COXFileViewer*> m_arrFileViewers;

	// the window that will host the preview area
	COXPreviewWnd m_preview;
	friend COXPreviewWnd;    

	// internal flag that specifies whether the dialog has been 
	// initialized or not
	BOOL m_bInitialized;

protected:
#ifdef OX_PREVIEW_GRAPHICS
	// file viewer for graphics files
	COXGraphicsFileViewer m_graphicsFileViewer;
#endif
#ifdef OX_PREVIEW_PLAINTEXT
	// file viewer for plain text files
	COXTextFileViewer m_textFileViewer;
#endif
#ifdef OX_PREVIEW_RTF
	// file viewer for RTF files
	COXRTFFileViewer m_rtfFileViewer;
#endif
#ifdef OX_PREVIEW_HTML
	// file viewer for HTML and other viewable by WebControl file types
	COXHTMLFileViewer m_htmlFileViewer;
#endif
#ifdef OX_PREVIEW_SOUND
	// file viewer for WAVE file types
	COXSoundFileViewer m_soundFileViewer;
#endif

protected:
	// if TRUE the dialog's toolbar is set to display flat buttons
	BOOL m_bFlatToolbar;
	// rectangle for the preview area
	CRect m_rectPreviewArea;
	// rectangle for the splitter area
	CRect m_rectSplitterArea;
	// rectangle for the resizable area
	CRect m_rectResizableArea;
	// TRUE if left button was pressed and held in splitter area
	BOOL m_bIsSplitterPressed;
	// the previous position of the mouse cursor when splitter is pressed
	CPoint m_ptOldSplitterCapture;
	// last saved width and height of the list control that displays the 
	// files and folders
	int m_nOldListCtrlWidth;
	int m_nOldListCtrlHeight;

	// offset of the preview window from the list control relatively to  
	// the list control's right side
	int m_nPreviewWndOffset;

	// global array of all preview dialog objects
	static CMap<HWND,HWND,COXPreviewDialog*,COXPreviewDialog*> m_allPreviewDialogs;
	// saved address of the window procedure of subclassed dialog
	WNDPROC m_pfnSuper;
	// global window procedure for subclassed dialog
	static LRESULT CALLBACK GlobalPreviewDialogProc(HWND hWnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);
	// window procedure for subclassed dialog
	virtual LRESULT PreviewDialogProc(HWND hWnd, UINT uMsg, 
		WPARAM wParam, LPARAM lParam);


public:
	// --- In  :	bPreview	-	if TRUE then preview mode will be set on;
	//								if FALSE then preview mode will be set off
	// --- Out : 
	// --- Returns:
	// --- Effect : Sets preview mode on/off
	void SetPreview(BOOL bPreview);

	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if preview mode is on; FALSE otherwise
	// --- Effect : Retrieves the flag that specifies the state of the 
	//				preview mode
	inline BOOL IsInPreviewMode() const { return m_bPreview; }


	// --- In  :	bFlat	-	if TRUE then the dialog's toolbar will be set to
	//							flat mode; otherwise it will be set to normal mode
	// --- Out : 
	// --- Returns: 
	// --- Effect : Sets the dialog's toolbar in the flat/normal mode
	void SetFlatToolBar(BOOL bFlat);

	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if the dialog's toolbar is set to flat mode
	// --- Effect : Retrieves the flag that specifies if the dialog's toolbar 
	//				is in the flat or normal mode
	inline BOOL IsFlatToolBar() const { return m_bFlatToolbar; }


	// --- In  :	nPreviewWndOffset	-	offset in pixels of the preview window
	//										from its normal position relatively
	//										to the list control that displays files 
	//										and folders. Positive number means that 
	//										preview window will be enlarged and list 
	//										control will be shrunken and the 
	//										negative number means that preview 
	//										window will be shrunken and list 
	//										control will be enlarged. Normally,
	//										the width of the preview window equals
	//										to the width of the toolbar
	// --- Out : 
	// --- Returns: The adjusted offset that takes into account the limitation 
	//				for the sizes of the list control and preview window
	// --- Effect : Sets the offset in pixels of the preview window from its normal 
	//				position relatively	to the list control that displays files 
	//				and folders
	int SetPreviewWndOffset(int nPreviewWndOffset);

	// --- In  :	
	// --- Out : 
	// --- Returns: The offset in pixels of the preview window from its normal 
	//				position relatively	to the list control that displays files 
	//				and folders. Positive number means that preview window will be 
	//				enlarged and list control will be shrunken and the negative 
	//				number means that preview window will be shrunken and list 
	//				control will be enlarged. Normally,	the width of the preview 
	//				window equals to the width of the toolbar
	// --- Effect : Retrieves the offset in pixels of the preview window from 
	//				its normal position relatively to the list control that 
	//				displays files and folders
	inline int GetPreviewWndOffset() const { return m_nPreviewWndOffset; }


protected:
	// retrieves a window handle to the dialog's list control that displays 
	// file and folders
	HWND FindListCtrl();
	// retrieves a window handle to the dialog's toolbar
	HWND FindToolBar();

	// resizes dialog controls
	virtual void AdjustControls();
	// updates toolbar position and adds "Preview" button
	virtual void UpdateToolBar();
  
	// standard CFileDialog handlers
	virtual void OnFileNameChange();
	virtual void OnInitDone();
	virtual void OnFolderChange();

	// window messages handlers
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnWndMsg(UINT message,WPARAM wParam,LPARAM lParam,LRESULT* pResult);

	// routines for saving the preview state and preview window offset settings
	void SaveState(LPCTSTR lpszProfileName=_T("COXPreviewDialog Settings"));
	void LoadPreviewState(LPCTSTR lpszProfileName=_T("COXPreviewDialog Settings"));
	void LoadPreviewWndOffsetState(LPCTSTR lpszProfileName=_T("COXPreviewDialog Settings"));

protected:      
	BOOL IsWindows2K();
	// initializes the dialog
	virtual void InitializePreviewDialog();


	// --- In  :	sFilename	-	full path to new selected file
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called every time new file is selected in the list control. 
	//				The default implementation calls 
	//				COXFileViewer::OnNewFileSelected() function for all registered 
	//				file viewer objects
	virtual void OnNewFileSelect(const CString& sFilename);

	// --- In  :	pDC			-	device context to draw in
	//				paintRect	-	rectangle to draw in
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called every time the preview of the selected file must be drawn.
	//				The default implementation calls 
	//				COXFileViewer::OnPaintPreview() function for all registered 
	//				file viewer objects
	virtual void OnPaintPreview(CDC* pDC, const CRect& paintRect);

	// --- In  :	pFocusWnd	-	window that have the focus at the moment
	// --- Out : 
	// --- Returns: 
	// --- Effect : Called every time the preview of the selected file must be drawn.
	//				The default implementation calls 
	//				COXFileViewer::OnDoRealizePalette() function for all registered 
	//				file viewer objects
	virtual void OnDoRealizePalette(CWnd* pFocusWnd);


	// checks if new file was selected
	virtual void CheckFileSelection();

	// draws the preview of the selected file when there is no viewer found
	// that handles the drawing of this file preview
	virtual void DrawPreviewNotAvailable(CDC* pDC, const CRect& paintRect);

	// standard handler
	virtual BOOL OnInitDialog();

	// message handlers
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	DECLARE_MESSAGE_MAP()
};

#endif //__OXPREVIEWDIALOG_H__

