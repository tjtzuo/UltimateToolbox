// ==========================================================================
//						   Class Specification 
// 							  COXImageListBox
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

COXImageListBox is CListBox derived class designed to display multiple images
of the same size. Images are displayed in multiple columns and if they didn't 
fit the size of the control's window the control will automatically support
scrolling in horizontal direction. COXImageListBox control supports only
single selection and the following styles must be specified: LBS_OWNERDRAWFIXED
and LBS_MULTICOLUMN; and these styles must not be specified: LBS_MULTIPLESEL and
LBS_OWNERDRAWVARIABLE. 

COXImageListBox uses internally an object of CImageList class as a storage for 
displayed images and uses its functionality to draw them. The number of items 
in this image list is always the same as the number of items in the list box. 
And an index of the item in the list box corresponds to the same index of the
displayed image in the internal image list. That means that any displayed image 
can be accessed just by its index.

All CListBox functions can be applied to the COXImageListBox object keeping in mind
the limitations on the used styles (e.g. functions for a list box with multiple 
selection are not applicable).


	Usage

In order to use COXImageListBox object in your application you have to create it
using standard CListBox::Create() function or subclass the existing list box control,
e.g. using DDX/DDV technology for dialog or form views. 

After control was successfully created or subclassed you might want to populate it
with images that will be displayed in it. We provide multiple ways of doing this.

First of all you can specify your own external image list object as source of images.
Call the following function in order to do that:

	int SetImageList(CImageList* pImageList);

You also can add new images to the existing ones through calling the following 
functions:

	int AddImage(CBitmap* pbmImage, CBitmap* pbmMask);
	int AddImage(CBitmap* pbmImage, COLORREF crMask);
	int AddImage(HICON hIcon);

And you can add a set of images by calling:

	int AddImageList(CImageList* pImageList);

function.


We would like to emphisize one specific way of populating the image list box with 
new images. It is known that icons that reside in executable files can retrieved 
using Win32 API functions. We provide an automatic way of retrieving icons from
executables and insert them in the list box. In order to do that you just have to 
call the following function:

	BOOL LoadIconsFromFile(LPCTSTR lpszFileName=NULL, BOOL bSmallIcon=TRUE);


At any time you can replace any displayed image with another one if you call 
the following functions:

	BOOL ReplaceImage(int nImage, CBitmap* pbmImage, CBitmap* pbmMask=NULL);
	BOOL ReplaceImage(int nImage, HICON hIcon);

Note that after you are done with replacing images you have to call RedrawWindow()
function in order to update the displayed images.


And, finally, you can remove any displayed image from the list box using:

	BOOL RemoveImage(int nImage);

All displayed images can be removed at once using:

	void EmptyImageList();

function.


The information about images can be accessed at any time by means of set of the 
following functions:

	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;
	HICON GetIcon(int nImage);
	HICON GetSelectedIcon();


The internal image list object can be accessed directly through calling:

	CImageList* GetImageList();


As an additional functionality we support setting of list box background color. Use
these functions in order to set/retrieve the control's background color:

	virtual COLORREF GetBkColor() const;
	void SetBkColor(COLORREF clrBackground); 

  


The sample that demonstrates COXImageListBox class is called ImageListBox and 
can be found in the .\Samples\gui\ImageListBox subdirectory of your Ultimate Toolbox 
directory. 



Dependency:
	#include "OXImageListBox.h"


Source code files:
	"OXImageListBox.cpp"		
*/

#if !defined(_OXIMAGELISTBOX_H__)
#define _OXIMAGELISTBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

/////////////////////////////////////////////////////////////////////////////
// COXImageListBox window

class OX_CLASS_DECL COXImageListBox : public CListBox
{
// Construction
public:
	// --- In  :	cx		-	width of images displayed in the list box
	//				cy		-	height of images displayed in the list box
	//				nFlags	-	Specifies the type of the internal image list. 
	//							Refer to CImageList::Create() function documentation
	//							for details on the values this argument can take.
	//							The internal image list displayed in the list box 
	//							can be always replaced with external one using
	//							SetImageList() function.
	// --- Out : 
	// --- Returns:
	// --- Effect:	Constructs the object and specifies settings for internal image 
	//				list that will contain images that are to be displayed
	COXImageListBox(int cx=GetSystemMetrics(SM_CXSMICON),
		int cy=GetSystemMetrics(SM_CYSMICON), 
		UINT nFlags=ILC_COLOR24|ILC_MASK);

// Attributes
public:

protected:
	// inernal image list that contains images that is displayed in the list box
	CImageList m_imageList;

	// default width of images displayed in the list box
	int m_nOrigWidth;
	// default height of images displayed in the list box
	int m_nOrigHeight;

	// control's background color 
	COLORREF m_clrBackground;

	// control's Highlight color
	COLORREF m_clrHighlight;

// Operations
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	The background color
	// --- Effect:	Retrieves the background color
	virtual COLORREF GetBkColor() const { return m_clrBackground; }
	
	// --- In  :	clrBackground	-	new background color
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Sets the background color
	inline void SetBkColor(COLORREF clrBackground) 
	{ 
		m_clrBackground=clrBackground; 
		if(::IsWindow(GetSafeHwnd()))
			RedrawWindow();
	}


	// --- In  :	
	// --- Out : 
	// --- Returns:	The Highlight color
	// --- Effect:	Retrieves the Highlight color
	virtual COLORREF GetHighlightColor() const { return m_clrHighlight; }
	
	// --- In  :	clrHighlight	-	new Highlight color
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Sets the Highlight color
	inline void SetHighlightColor(COLORREF clrHighlight) 
	{ 
		m_clrHighlight=clrHighlight; 
		if(::IsWindow(GetSafeHwnd()))
			RedrawWindow();
	}


	// --- In  :	
	// --- Out : 
	// --- Returns:	Pointer to the internal image list that contains images displayed 
	//				in the list box
	// --- Effect:	Retrieves pointer to the internal image list that contains 
	//				images displayed in the list box
	inline CImageList* GetImageList() { return &m_imageList; }


	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Removes all images displayed in the list box. 
	void EmptyImageList();


	// --- In  :	pImageList	-	pointer to an image list object to be used
	//								as a container of images to be displayed in the 
	//								list box
	// --- Out : 
	// --- Returns:	The number of images added to the image list box
	// --- Effect:	Sets new image list as a source of images to be displayed in the 
	//				list box. 
	int SetImageList(CImageList* pImageList);

	// --- In  :	pImageList	-	pointer to an image list object which images
	//								will be added to existing images in the 
	//								internal image list
	// --- Out : 
	// --- Returns:	Zero-based index of the first new image if successful,
	//				otherwise  -1
	// --- Effect:	Adds new images to be displayed in the list box. 
	int AddImageList(CImageList* pImageList);
	

	// --- In  :	pbmImage	-	Pointer to the bitmap containing the image or 
	//								images. The number of images is inferred from 
	//								the width of the bitmap in the internal image list
	//				pbmMask		-	Pointer to the bitmap containing the mask. If no 
	//								mask is used with the image list, this parameter 
	//								is ignored
	//				crMask		-	Color used to generate the mask
	//				hIcon		-	Handle of the icon that contains the bitmap and 
	//								mask for the new image
	// --- Out : 
	// --- Returns:	Zero-based index of the first new image if successful,
	//				otherwise  -1.
	// --- Effect:	Adds one or more images or an icon to the internal image list
	int AddImage(CBitmap* pbmImage, CBitmap* pbmMask);
	int AddImage(CBitmap* pbmImage, COLORREF crMask);
	int AddImage(HICON hIcon);


	// --- In  :	nImage		-	Zero-based index of the image to replace
	//				pbmImage	-	Pointer to the bitmap containing the image
	//				pbmMask		-	Pointer to the bitmap containing the mask. If no 
	//								mask is used with the image list, this parameter 
	//								is ignored
	//				hIcon		-	Handle of the icon that contains the bitmap and 
	//								mask for the new image
	// --- Out : 
	// --- Returns:	TRUE if successful,	otherwise  FALSE.
	// --- Effect:	Replaces an image in the internal image list with a new image
	BOOL ReplaceImage(int nImage, CBitmap* pbmImage, CBitmap* pbmMask=NULL);
	BOOL ReplaceImage(int nImage, HICON hIcon);


	// --- In  :	nImage		-	Zero-based index of the image to remove
	// --- Out : 
	// --- Returns:	TRUE if successful,	otherwise  FALSE.
	// --- Effect:	Removes an image from the list box
	inline BOOL RemoveImage(int nImage) { return DeleteString(nImage); }

	
	// --- In  :	nImage		-	Zero-based index of the image
	//				pImageInfo	-	Pointer to an IMAGEINFO structure that receives 
	//								information about the image. The information in 
	//								this structure can be used to directly manipulate 
	//								the bitmaps for the image
	// --- Out : 
	// --- Returns:	TRUE if successful,	otherwise  FALSE.
	// --- Effect:	Retrieves information about an image
	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;


	// --- In  :	nImage		-	Zero-based index of the image
	// --- Out : 
	// --- Returns:	Handle of the icon for the specified image if successful,
	//				otherwise NULL
	// --- Effect:	Creates an icon based on an image and its related mask in the 
	//				internal image list
	HICON GetIcon(int nImage);

	// --- In  :	
	// --- Out : 
	// --- Returns:	Handle of the icon for the currently selected image in the 
	//				list box if successful,	otherwise NULL
	// --- Effect:	Creates an icon of the selected image in the list box based on 
	//				an image and its related mask in the internal image list
	HICON GetSelectedIcon();


	// --- In  :	lpszFileName	-	name of the executable file that will serve 
	//									as a source of icons. If NULL is specified 
	//									then the executable	of the application that 
	//									called this function is used as source file
	//				bSmallIcon		-	flag that specifies whether small or large
	//									icons should be retrieved from the source 
	//									executable file
	// --- Out : 
	// --- Returns:	TRUE if successful,	otherwise  FALSE.
	// --- Effect:	Clears the current contents of the list box and populate it with 
	//				icons loaded from the specified executable file
	BOOL LoadIconsFromFile(LPCTSTR lpszFileName=NULL, BOOL bSmallIcon=TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXImageListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect:	Destructs the object
	virtual ~COXImageListBox();

protected:
	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if the list box was successfully initialized,
	//				otherwise FALSE;
	// --- Effect:	Initializes the list box. Advanced overridable, can be used
	//				to initialize internal data in the derived classes. This
	//				function is called right after the control has been created 
	//				or subclassed
	virtual BOOL InitializeImageListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COXImageListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnDeleteString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResetContent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXIMAGELISTBOX_H__)
