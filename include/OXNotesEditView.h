/////////////////////////////////////////////////////////////////////////////
// ==========================================================================
// 					Class Specification : COXNotesEditView
// ==========================================================================

// Header file : OXNotesEditView.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

// Desciption :      Class COXNotesEditView is derived from CEditView
//					 and can have on the right or on the left side
//					 of the view an area to set bookmarks like we can set 
//					 breakpoints in Visual Studio. The breakpoints can be 
//					 of different type, so, every type has own image 
//					 representation. The bookmarks is set on a char, not
//					 on the line. There are possibility to have up to 256
//					 types (images) of bookmarks. Bookmarks can be added,
//					 removed, updated. Some virtual functions provide additional
//					 customization. While the cursor is moving from client
//					 area to notes area or vicewersa, OnChangeCursor() virtual
//					 function called, so there is a possibility to get a position
//					 of the cursor and change the cursor to another one.
//					 Also when new bookmark is adding it calls OnSetBookmark()
//					 virtual function that can update the type of the book
//					 mark and must return TRUE, if the bookmark should be added,
//					 or FALSE to prevent adding of the bookmark.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_OXNOTESEDITVIEW_H__)
#define _OXNOTESEDITVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXMainRes.h"
#include "oxDllExt.h"
#include "UTB64Bit.h"

#define OX_NOTES_ALL -1

#ifndef OX_NOTES_BRUSH
#define OX_NOTES_BRUSH IDB_OX_NOTES
#endif

#ifndef OX_NOTES_WIDTH_DEFAULT
#define OX_NOTES_WIDTH_DEFAULT -1
#endif

#ifndef OX_NOTES_BOOKMARKS
#define OX_NOTES_BOOKMARKS IDB_OX_BOOKMARKS
#endif

#ifndef OX_NOTES_BOOKMARKS_SIZE
#define OX_NOTES_BOOKMARKS_SIZE 16
#endif
  
#ifndef OX_CURSOR_NOTES
#define OX_CURSOR_NOTES IDC_OX_CURSOR_NOTES
#endif

class OX_CLASS_DECL COXNotesEditView : public CEditView
{ 
protected: 
	
	// create from serialization only

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object
	COXNotesEditView(UINT nSide=SIDE_LEFT, 
		UINT nWidth=OX_NOTES_WIDTH_DEFAULT, UINT nMode=MODE_BKMARKS);

	DECLARE_DYNCREATE(COXNotesEditView)

public:
	enum {MODE_BKMARKS, MODE_LINES};
	enum {SIDE_NONE, SIDE_LEFT, SIDE_RIGHT};
	enum {NONE, NOTES, EDIT};
	enum {LENGTH_THESAME, LENGTH_MORE, LENGTH_LESS};

//functions

	// --- In  : clrText - new text color to be set 
	// --- Out : 
	// --- Returns :
	// --- Effect : Call this function to set new text color
	inline void SetTextColor(COLORREF clrText)
	{
			m_clrFont=clrText;
	}

	// --- In  :
	// --- Out : 
	// --- Returns : Current text color
	// --- Effect : Call this function to get current text color
	inline COLORREF GetTextColor() const
	{
			return m_clrFont;
	}

	// --- In  : clrBkgnd - new background color of the client area to be set
	// --- Out : 
	// --- Returns : 
	// --- Effect : Call this function to set new background color for the client area
	inline void SetBackground(COLORREF clrBkgnd)
	{
	if (m_bshClient.m_hObject)
		m_bshClient.DeleteObject();
	VERIFY(m_bshClient.CreateSolidBrush(clrBkgnd));

	}

	// --- In  :
	// --- Out : 
	// --- Returns : Current background color of the client area
	// --- Effect : Call this function to get background color of the client area
	inline  COLORREF   GetBackground() 
	{
		LOGBRUSH LogBrush;
		m_bshClient.GetLogBrush(&LogBrush);
		return LogBrush.lbColor;
	}

	// --- In  : nSide - side where notes will be, either SIDE_LEFT or SIDE_RIGHT
	// --- Out : 
	// --- Returns :
	// --- Effect : Call this function to set side for the notes area
	inline void	 SetNotesSide(UINT nSide=SIDE_LEFT)
	{
		m_nSide=nSide;
		RedrawWindow();
	}

	// --- In  : nWidth - width of the notes area in pixels,
	//					  if the value is OX_NOTES_WIDTH_DEFAULT,
	//					  the width will be equal of the width of 
	//					  vertical scrollbar
	// --- Out : 
	// --- Returns :
	// --- Effect : Call this function to set width of the notes area
	inline void	 SetNotesWidth(UINT nWidth=OX_NOTES_WIDTH_DEFAULT)
	{
		m_nNotesWidth=nWidth;
		RedrawWindow();
	}
	
	// --- In  :
	// --- Out : 
	// --- Returns : Side where notes area is located
	// --- Effect : Call this function to get the area where the notes are located
	inline UINT	 GetNotesSide() const
	{
			return m_nSide;
	}
	
	// --- In  :
	// --- Out : 
	// --- Returns : Width of the notes area
	// --- Effect : Gets the notes width
	inline UINT	 GetNotesWidth() const
	{
			return m_nNotesWidth;
	}

	// --- In  : nSide - side where the notes will be inserted
	//			 nWidth - width of the notes area
	// --- Out : 
	// --- Returns :
	// --- Effect : Inserts notes area
	BOOL	 InsertNotes(UINT nSide=SIDE_LEFT, UINT nWidth=OX_NOTES_WIDTH_DEFAULT);

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Deletes notes area and the bookmarks
	void	 DeleteNotes();

	// --- In  :
	// --- Out : 
	// --- Returns : Number of the bookmarks
	// --- Effect : Returns number of the bookmarks
	inline int		 GetBookmarksCount() const
	{
			return PtrToInt(m_arBookmarks.GetSize());
	}

	// --- In  :
	// --- Out : 
	// --- Returns : Bookmark by number
	// --- Effect :  Call this function to get the bookmark by number.
	//				 The bookmark represents 32 bit value where 24 least
	//				 significant bits means number of the character and
	//				 8 most significant bits represents type (number of the 
	//				 image in imagelist) 
	inline int	 GetBookmark(UINT nNumber) const
	{
		if (nNumber<(UINT) m_arBookmarks.GetSize())
			return (int) m_arBookmarks.GetAt(nNumber);
		else
			return -1;
	}

	// --- In  :
	// --- Out : 
	// --- Returns : TRUE if the char is marked, FALSE otherwise
	// --- Effect : Call this function to determine if the char is bookmarked.
	BOOL	 IsMarked(UINT nChar) const;

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Removes bookmarks
	inline void	 ClearBookmarks()
	{
			m_arBookmarks.RemoveAll();
	}

	// --- In  : nChar - bookmarked char to show
	// --- Out : 
	// --- Returns :
	// --- Effect : Makes scrolling to show the bookmark of the char
	BOOL	 ShowBookmark(UINT nChar);

	// --- In  : nChar - number of the char from the beginig of 
	//					 the view to set bookmark
	//			 nType - type of the bookmark
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets bookmark
	void	 SetBookmark(UINT nChar, BYTE nType=NULL);

	// --- In  : clrNotesBkgnd - color to be used for painting
	//			 background area of the notes
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets new background color for the notes
	void	 SetNotesBackground(COLORREF clrNotesBkgnd);

	// --- In  : pBrush - a pointer to a brush object,
	//					  a logical brush will be used to paint the background
	//					  of the notes
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets new background brush for the notes area
	void	 SetNotesBackground(CBrush* pBrush);

	// --- In  :
	// --- Out : 
	// --- Returns : a pointer to the brush that is used
	//				 to paint the background of the notes
	// --- Effect : Retrieves current background brush of the notes
	inline const CBrush* GetNotesBackground() const
	{
		return &m_bshNotes;
	}

	// --- In  :
	// --- Out : 
	// --- Returns : A pointer to the image list that is used for
	//				 drawing bookmarks
	// --- Effect : Gets the bookmarks image list
	inline CImageList* GetNotesImageList() 
	{
		return &m_imgBookmarks;
	}

	// Operations
protected:
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT OnSetMargins(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT OnGetMargins(WPARAM wParam, LPARAM lParam);	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXNotesEditView)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF GetLinesColor() const;
	void SetLinesColor(COLORREF clr, BOOL bRedraw=TRUE);
	const CFont* GetLinesFont() const;
	BOOL SetLinesFont(CFont* pFont=NULL, BOOL bRedraw=TRUE);
	UINT GetMode() const;
	void SetMode(UINT nMode, BOOL bRedraw=TRUE);
		virtual ~COXNotesEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//functions

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Initializes image list. Override this function if
	//				you want to load your own images.
	virtual void LoadNotesImageList()
	{
		if (m_imgBookmarks.GetSafeHandle())
			m_imgBookmarks.DeleteImageList();
		m_imgBookmarks.Create(OX_NOTES_BOOKMARKS,
			OX_NOTES_BOOKMARKS_SIZE,1,CLR_DEFAULT);
	}
	// --- In  : nLine - line image will be searched for 
	// --- Out : 
	// --- Returns : Index of the image for the first bookmarked 
	//				 character in the line, if any.
	// --- Effect : Returns image index for the first bookmarked 
	//				character in the line if any
	virtual int ImageFromLine(UINT nLine) const;

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Draw bookmarks
	virtual void DrawBookmarks(CDC* pDC);

	// --- In  : phCursor - a handle of the cursor will be set on
	//			 nPosition - where the cursor is, either, NOTES or EDIT
	// --- Out : 
	// --- Returns :
	// --- Effect : The class calls this member every time the cursor
	//				has moved from the client area to the notes or vicewersa
	//				It is possible to change cursor handle at this time
	virtual void OnChangeCursor(HCURSOR* phCursor, UINT nPosition);

	// --- In  : nChar - index of the char to set bookmark
	//			 pType - a pointer to the type of bookmark will
	//					 be set on
	// --- Out : 
	// --- Returns :
	// --- Effect : This function is called every time new bookmark will
	//				be set. You can change the type of the bookmark at this
	//				moment. Return TRUE to set bookmark, or FALSE to prevent it.
	virtual BOOL OnSetBookmark(UINT nChar, DWORD* pType);

	CSize GetBookmarkImageSize();
	void GetNotesRect(LPRECT pRect);
	int GetLineHeight();
	int GetLastVisibleLine();
	void RemoveBookmarks( UINT nStartChar, UINT nEndChar);
	void OffsetBookmarks(UINT nStart, int nOffset);

protected:
	//member variables
	CString m_sCopy;
	CBrush m_bshClient;
	CImageList m_imgBookmarks;
	CDWordArray m_arBookmarks;
	CBrush m_bshNotes;

	HCURSOR m_hNotesCursor;
	COLORREF m_clrFont;

	UINT m_nNotesWidth;
	UINT m_nSide;
	UINT m_nPosition;
	UINT m_nMargins;
	BOOL m_bUpdatingMargins;

// Generated message map functions
protected:
	COLORREF m_clrLines;
	CFont* m_pLinesFont;
	UINT m_nMode;
	//{{AFX_MSG(COXNotesEditView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXNOTESEDITVIEW_H__)
