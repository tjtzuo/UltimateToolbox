// ==========================================================================
// 					Class Specification : COXGridList
// ==========================================================================

// Header file : gridlist.h

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
//	YES	Derived from CListCtrl

//	YES	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class can be used to simulate a simple grid with a ListView
//	common control (Win95 and NT 3.51).  It lets the user select an entire
//	row in one time and draws vertical and horizontal gridlines, default header
//	sorting, font changing, text color changing, ...

// Remark:
//	The sorting algorithm uses the LPARAM associated with each item.
//	It points to a COXGridListData object which contains the original user data
//	Every item has its own COXGridListData object, so this pointer uniquely
//	 identifies the item
//	The original user data can still be used for other purposes and will still
//   be fully transparantly accessible

//	This file uses resources. (E.g. Bitmap IDB_STATEIMAGELIST)
//	The reserved ID ranges are : 23280 -> 23299 and 53280 -> 53299

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////

#ifndef __GRIDLIST_H__
#define __GRIDLIST_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#define STATEIMAGEMASKTOINDEX(i) (((i) >> 12) & 0xF)
#define ALLSTATEIMAGEMASKS INDEXTOSTATEIMAGEMASK(0xF)

#include "OXGridEdit.h"
#include "OXGridHdr.h"
#include "OXMainRes.h"

#include "UTB64Bit.h"


// Structure that is used with notifications that grid list sends to its parent
//
typedef struct _tagNMGRIDLIST
{
    // standard header
	NMHDR hdr;
	// additional info, depends on notification
	LPARAM lParam;

	_tagNMGRIDLIST()
	{
		ZeroMemory(this,sizeof(_tagNMGRIDLIST));
	}

} NMGRIDLIST, * LPNMGRIDLIST;

// Structure that is used with GLN_CONTEXTMENU notification and sent as lParam element 
// of NMGRIDLIST structure to its parent window before displaying context menu in order 
// to give a chance to parent window to modify this menu.
typedef struct _tagGLCONTEXTMENU
{    
	// pointer to the context menu that is about to be displayed
	CMenu* pMenu;
	// mouse cursor coordinates to display the menu
	CPoint point;

	_tagGLCONTEXTMENU()
	{
		ZeroMemory(this,sizeof(_tagGLCONTEXTMENU));
	}

} GLCONTEXTMENU, * LPGLCONTEXTMENU;



class COXEditList;

class OX_CLASS_DECL COXGridList : public CListCtrl
{
friend class COXEditList;
// Data members -------------------------------------------------------------
public:

	struct COXGridListSortInfo
		{
		public:
			COXGridList*  	m_pThis;
			int   			m_nSubIndex;
			BOOL			m_bSortAscending;

			COXGridListSortInfo(COXGridList* pThis = NULL, int nSubIndex = 0,
								BOOL bSortAscending=TRUE):
				m_pThis(pThis),
				m_nSubIndex(nSubIndex),
				m_bSortAscending(bSortAscending)
			{
			};
		};

protected:
	BOOL 	m_bLastRowWasVisible;
	BOOL 	m_bInitialized;
	BOOL 	m_bSortable;
	BOOL 	m_bCheckable;
	UINT	m_nCheckStyle;
	BOOL	m_bAutoEdit;

	BOOL	m_bShowSel;

	enum EUseExtendedData
		{
		EDNo = 1,
		EDAdding = 2,
		EDYes = 3,
		EDRemoving = 4,
		};
	EUseExtendedData m_eUseExtendedData;
	static EUseExtendedData m_eFirstED;
	static EUseExtendedData m_eLastED;

	int   	m_nNumOfCols;
	int		m_nImageColumn;

	CPen  	m_GridPen;
	BOOL	m_bGridLines;
	BOOL	m_bHorizontalGridLines;
	BOOL	m_bVerticalGridLines;

	CRect	m_SelectedRect;
	CFont   m_TextFont;
	CImageList m_stateImages;

	COXGridEdit m_gridEdit;
	CPoint m_lastClickPos;
	int m_nEditSubItem;

	CWordArray m_rgbEditable;

	static int m_nListEditXOffset;
	static int m_nListEditYOffset;
	static int m_nListEditCYOffset;

	static int m_nListEditCXOffset;

	struct COXGridListData
		{
		public:
			DWORD_PTR m_dwUserData;
			
			COXGridListData(DWORD_PTR dwUserData = 0)
				:
				m_dwUserData(dwUserData)
				{
				};
		};


	COXGridHeader m_gridHeader;

	BOOL m_bSortAscending;

	int m_nSortCol;

	CMenu* m_pContextMenu;

	PFNLVCOMPARE	m_pCompareFunc;
private:
	
// Member functions ---------------------------------------------------------
public:
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
	COXGridList();

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets text color, text background color and the text font
	virtual void InitGrid();

	// --- In  : bSortable : automatic sort or not
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Sets the Grid in an automatic sort mode.  When clicking a header
	//				the strings in that column will be sorted
	BOOL SetSortable(BOOL bSortable = TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns : whether automatic sorting is supported or not
	// --- Effect : 
	BOOL GetSortable() const;

	// --- In  : bResizing : resizable columns or not
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Sets the Gridcolumns to be resizable or not through the header
	BOOL SetResizing(BOOL bResizing = TRUE);
	
	// --- In  :
	// --- Out : 
	// --- Returns : whether the columns can be resized or not
	// --- Effect : 
	BOOL GetResizing();

	void SetGridCompareFunc( PFNLVCOMPARE pCompareFunc );
	// --- In  : nColumn : Zero-based index of the column used to sort upon
	//			 bSortAscending	: TRUE if sort ascending and FALSE if sort descending
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sorts the contents of the control by sorting a specific column
	inline BOOL SortColumn(int nColumn, BOOL bSortAscending) 
	{
		m_bSortAscending=bSortAscending;
		return SortColumn(nColumn);
	}
	virtual BOOL SortColumn(int nColumn = 0);

	// --- In  : 
	// --- Out : 
	// --- Returns: sorted column or -1 if none was sorted
	// --- Effect : 
	int GetSortColumn();

	// --- In  :
	// --- Out : 
	// --- Returns : Number of columns inserted in the listctrl
	// --- Effect : 
	virtual int GetNumCols() const;

	// --- In  :
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Rearranges the inserted columns to have an equal width
	//				that is equidistant divided within the width of the listctrl
	virtual BOOL SetEqualWidth();
		 			
	// --- In  : bGridLines : True if gridlines are wanted
	//			 LineColor : the color you want your gridlines to have
	//			 bUpdate : Redraw Control now?
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : The listctrl can show gridlines with a special color
	virtual BOOL SetGridLines(BOOL bGridLines = TRUE,
		COLORREF LineColor = RGB(0,0,0), BOOL bUpdate = TRUE);

	// --- In  : 
	// --- Out : bGridLines : whether the control shows gridlines
	// 			 LineColor : which color they have 
	// --- Returns : succeeded or not
	// --- Effect : 
	virtual BOOL GetGridLines(BOOL& bGridLines, COLORREF& LineColor) const;

	// --- In  : bHorizontal : Whether horizontal gridlines should be shown
	//			 bVertical : Whether vertical gridlines should be shown
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : By default both horizontal and vertical gridlines will be shown
	//				Make sure the general setting to show gridlines (SetGridLines())
	//				is set to true
	virtual BOOL SetGridLineOrientation(BOOL bHorizontal = TRUE,
		BOOL bVertical = TRUE);

	// --- In  : 
	// --- Out : bHorizontal : Whether horizontal gridlines should be shown
	//			 bVertical : Whether vertical gridlines should be shown
	// --- Returns : succeeded or not
	// --- Effect : 
	BOOL GetGridLineOrientation(BOOL& bHorizontal, BOOL& bVertical) const;

	// --- In  :
	// --- Out : 
	// --- Returns : The current selected row
	//				 When no row is selected -1 is returned
	//				 When more than one row is selected the first is returned
	// --- Effect : 
	int	GetCurSel() const;

	// --- In  : nSelectionItem : The item that should change selection state (-1 = all items)
	//           bSelect : Select (TRUE) or unselect (FALSE)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Selects or unselects an item(s)
	BOOL SetCurSel(int nSelectionItem, BOOL bSelect = TRUE);

	// --- In  : bShow : Whether to show the selection at all
	//				  (even when the control has focus)
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sets selection display mode.  
	void SetShowSel(BOOL bShow = TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns : The number of selected items in the control 
	//					with multiple selection activated
	// --- Effect : 
	int GetSelCount() const;

	// --- In  : The number of the item
	// --- Out : 
	// --- Returns : Whether nItem is selected or not
	// --- Effect : 
	BOOL IsSelected( int nItem ) const;

	// --- In  :
	// --- Out : 
	// --- Returns : The current focussed row 
	//				 When no row has focus -1 is returned
	// --- Effect : 
	int	GetCurFocus() const;

	// --- In  : nFocusItem : The item that should change focused (-1 = all items)
	//           bFocus : Get (TRUE) or lose focuis (FALSE)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets or lose the focus of the specified item
	//				Note that at most one item can have focus
	BOOL SetCurFocus(int nFocusItem, BOOL bFocus = TRUE);

	// --- In  : pFont : new font, if null then the standard list ctrl font	is used
	//			 bUpdate : whether to redraw the ctrl or not
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets a new font to draw the row text with
	virtual BOOL SetTextFont(CFont* pFont = NULL, BOOL bUpdate = TRUE);

	// --- In  : bShowSelAlways : Whether to show the selection always
	//							  (even when the control does not have focus)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets selection display mode.  
	virtual BOOL SetShowSelAlways(BOOL bShowSelAlways = TRUE);

	// --- In  : 
	// --- Out : 
	// --- Returns : The control selection display mode
	//				 (whether a selection should always be shown)
	// --- Effect : 
	BOOL GetShowSelAlways() const;

	// --- In  : bMultiple : Allow multiple selection or not
	// --- Out : 
	// --- Returns : 
	// --- Effect : Whether to allow multiple selection or not 
	//				By default it is not allowed
	virtual void SetMultipleSelection(BOOL bMultiple = TRUE);

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether multiple selection is allowed or not 
	// --- Effect : 
	BOOL GetMultipleSelection() const;

	// --- In  : bEdit : Allow edit or not
	//			 nColumn : The column on which to allow editing (-1 = all columns)
	// --- Out : 
	// --- Returns : 
	// --- Effect : Whether to allow a subitem of a specified column to be edited or not
	//				By default editing is not allowed
	virtual void SetEditable(BOOL bEdit = TRUE, int nColumn = -1);

	// --- In  : nColumn : The column of which the editing state is requested
	// --- Out : 
	// --- Returns : Whether a subitem of a specified column is allowed to be edited or not
	// --- Effect : 
	BOOL GetEditable(int nColumn) const;

	// --- In  : bCheckable : Whether this control uses checkboxes for all items
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets this control in checkable mode.  
	virtual BOOL SetCheckable(BOOL bCheckable = TRUE);

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether this control in in checkable mode.  
	// --- Effect : 
	virtual BOOL GetCheckable() const;

	// --- In  : nStyle : BS_CHECKBOX, BS_3STATE, BS_AUTOCHECKBOX or BS_AUTO3STATE
	// --- Out : 
	// --- Returns : Sets the checkstyle (see the documentation of Win32 for information
	//				 about the different types)
	// --- Effect : 
	void SetCheckStyle(UINT nStyle = BS_AUTOCHECKBOX);

	// --- In  : 
	// --- Out : 
	// --- Returns : Get the present check style
	// --- Effect : 
	UINT GetCheckStyle() const;

	// --- In  : nIndex : The index os the item to change (-1 = all items)
	//			 nCheck : The new check state ( 0 for clear, 1 for checked, and 2 for indeterminate)
	// --- Out : 
	// --- Returns : Whether this control in in checkable mode.  
	// --- Effect : 
	void SetCheck(int nIndex, int nCheck);

	// --- In  : nIndex : The index of the item to investigate
	// --- Out : 
	// --- Returns : The present check state of the specified item.  
	// --- Effect : 
	int GetCheck(int nIndex) const;

	// --- In  : nCheckItem : The index of item which checkbox has been clicked
	// --- Out : 
	// --- Returns : Whether to proceed (TRUE) or abort the check (FALSE);
	// --- Effect : The user clicked the specified item, the check state will change
	virtual BOOL OnCheck(int nCheckItem);

	// --- In  : nCheckItem : The index of item which check state is about to change
	//			 nCheck : The new check state
	// --- Out : 
	// --- Returns : Whether to proceed (TRUE) or abort the check (FALSE);
	// --- Effect : The user clicked an item and because this item was selected
	//				its check state will change as well
	virtual BOOL OnCheckChange(int nCheckItem, int nCheck);

	// --- In  : bAutoEdit : Whether the item will be edited 
	//                       when a valid character is typed
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets this control in auto edit mode.  
	//				Normally valid characters (e.g. A, B, 1, etc) are used to
	//				jump to the item that starts with that character.
	//				This setting can change that behaviouir into editing the item
	virtual BOOL SetAutoEdit(BOOL bAutoEdit = TRUE);

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the control is in auto edit mode
	//               (whether focussed item will be edited when a valid character is typed)
	// --- Effect : 
	BOOL GetAutoEdit() const;

	// --- In  : nItem : The Item to edit
	//			 nSubItem : The subitem to edit
	// --- Out : 
	// --- Returns : If successful, a pointer to the CEdit object that is used to edit the item text; 
	//               otherwise NULL.
	// --- Effect : Begins in-place editing of the specified list view subitem
	//				To edit the first editable subitem of a specified item use
	//				nSubItem = -1
	CEdit* EditLabel(int nItem, int nSubItem);
  
	// --- In  : nColumnIndex : the index of the column where the small icons must appear
	//			 bUpdate : Redraw Control now?
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : assigns the column to show the images attached to an item
	BOOL SetImageColumn(int nColumnIndex, BOOL bUpdate = TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns : the index of the column where the small icons currently appear
	// --- Effect : 
	int GetImageColumn() const;

	// --- In  : pDC : the DC whose DlgBaseUnits will be calculated
	// --- Out : 
	// --- Returns : Dialog base units based on the font selected into the DC
	// --- Effect : If no special font selected, calls ::GetDialogBaseUnits()
	DWORD GetDlgBaseUnits(CDC* pDC);

	// --- In  :
	// --- Out : 
	// --- Returns : Whether the last row is completely visible (TRUE)
	//				 FALSE is returned when the last row is not visible 
	//				 or only partially visible
	// --- Effect : 
	virtual BOOL IsLastRowVisible() const;

	// --- In  :
	// --- Out : 
	// --- Returns : 
	// --- Effect : This overridable function will be called when the last
	//				row of the window was not completely visible before
	//				and becomes completely visible now.
	virtual void OnLastRowAppear();

	// --- In  : lpDIS : A long pointer to a DRAWITEMSTRUCT structure that contains
	// 				information about the type of drawing required.
	// --- Out : 
	// --- Returns :
	// --- Effect : Called by CDialog::OnDrawItem for an ownerdrawn listview, for every
	//				item that needs redrawing.
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

	// --- In  : nItem : Index of the item to make visible
	//			 nSubItem : Index of the subitem to make visible
	//			 bPartialOK : Specifies whether partial visibility is acceptable.
	// --- Out : 
	// --- Returns : 
	// --- Effect : Ensures that the specified subitem is visible
	BOOL EnsureVisible(int nItem, int nSubItem, BOOL bPartialOK);
  
	// --- In  : pos : Position expressed in client coordinates of the list control
	// --- Out : nItem : The index of the item in which the point is located
	//           nSubItem : The sub item in which the point is located (0 = label)
	//			 rect : The rectangle of that subitem
	// --- Returns : Whether a valid subItem was clicked (not outside items)
	// --- Effect : Converts coordinates to index of item and subitem
	BOOL GetSubItemFromPoint(CPoint pos, int& nItem, int& nSubItem, CRect& rect) const;

	// --- In  : nItem : The index of the item in which the point is located
	//           nSubItem : The sub item in which the point is located (0 = label)
	//			 bIncludeImages : Whether to take the size of possible images into account or not
	// --- Out : 
	// --- Returns : The rect of this subitem
	// --- Effect : Converts index of item and subitem to a rect
	CRect GetRectFromSubItem(int nItem, int nSubItem, BOOL bIncludeImages = FALSE) const;

	// --- In  : 
	// --- Out : 
	// --- Returns : The header control of this grid list control
	//				 This pointer may be temporary and should not be stored for later use
	//               NULL is returned when no no header control exists
	// --- Effect : 
	CHeaderCtrl* GetHeaderCtrl();

	// --- In  : 
	// --- Out : 
	// --- Returns : The handle of the header control of this grid list control
	//               or NULL when no header control exists
	// --- Effect : 
	HWND GetHeaderCtrlHandle();

	// --- In  : pMenu : pointer to the popup menu that will be displayed when user right 
	//					 click over the COXGridList control. Set it to NULL if you don't
	//					 want to display any menu.
	// --- Out : 
	// --- Returns : pointer to the previously attached context menu
	// --- Effect : assigns new context menu
	inline CMenu* AttachContextMenu(CMenu* pMenu) 
	{
		CMenu* pOldMenu=m_pContextMenu;
		m_pContextMenu=pMenu;
		return pOldMenu;
	}

	// --- In  : 
	// --- Out : 
	// --- Returns: pointer to the attached context menu, can be NULL if there is no any 
	//				context menu to display when right click over the COXGridList control.
	// --- Effect : 
	inline CMenu* GetContextMenu() { return m_pContextMenu; }

	// --- In  : 
	// --- Out : 
	// --- Returns: pointer to the edit control that will be used internally 
	//				to edit data
	// --- Effect : Retrieves pointer to edit control that will be used to edit 
	//				the grid data
	virtual COXGridEdit* GetGridEditCtrl() { return &m_gridEdit; }
	int FindOriginalItemData(DWORD_PTR dwItemData);

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXGridList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object
													 
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(COXGridList)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	void Empty();

	static int CALLBACK GridCompareProc(LPARAM, LPARAM, LPARAM);

	virtual BOOL SetCheckStateImageList();
	virtual BOOL DrawGridLines(CDC* pDC, LPDRAWITEMSTRUCT lpDIS);
	virtual void SetRowText(LPDRAWITEMSTRUCT lpDIS, BOOL bShowItemSel);
	virtual void SetColItemText(CDC* pDC, CString& sCellText, CRect& rectText, 
		UINT nJustify, UINT nFormat=DT_END_ELLIPSIS|DT_NOPREFIX);

	virtual EUseExtendedData GetExtendedDataState();
	virtual void SetExtendedDataState(EUseExtendedData eUseExtendedData);
	BOOL AddExtendedData();
	BOOL RemoveExtendedData();
	BOOL AdjustNotification(NM_LISTVIEW* pNMListView);

	void RefreshFocusItem();
	void RefreshSelItems();

	BOOL DrawImage(CDC* pDC,  CRect& rectText, int iItemID, BOOL bSelected);
	BOOL ChangeItemText(int nItem, int nSubItem, LPCTSTR pszText);
	DWORD_PTR GetOriginalItemData(int nItem);
	BOOL SetOriginalItemData(int nItem, DWORD_PTR dwData);
	int GetCheckItemFromPoint(const CPoint& point) const;
	BOOL PostEditLabel(int nItem, int nSubItem);
	BOOL SearchNextEditItem(int nItemOffset, int nSubItemOffset, int& nItem, int& nSubItem);

	LRESULT SendGLNotification(LPNMGRIDLIST pNMGL);

// Generated message map functions
   //{{AFX_MSG(COXGridList)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnListCtrlNotify(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLostFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
   
	afx_msg LRESULT OnInsertItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteAllItems(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFindItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditLabel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetColumn(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////
#endif      // __GRIDLIST_H__
