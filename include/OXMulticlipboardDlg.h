// ==========================================================================
// 					Class Specification : COXMulticlipboardDlg
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

// Desciption :         
// 
//  This is (surprise!) CDialog derived class that	implements "spy"
//  on a Windows clipboard. Data will be added automatically once you have
//  make copy or paste in any window that appeares in the dialog as new pocket (button).
//  The pocket will have appropriate icon and can provide customizable tips.
//  To copy data from the pocket to the clipboard just press button.
//
//  To use in application you can save last active window and make paste to the window
//  if you override PocketToClip function that is called when a user presses the 
//  pocket. Alternative, you can call this function without pressing the pocket
//  to put data to the clipboard. 
//
//  The 2 function that actually get and set the
//  data are virtual. You can override it if you want to handle the data 
//  in some specific format since clipboard can return the data in different formats usually.
//  The first format is most convinient format. 
//  By default this class does not save the clipboard data in custom formats
//  because of different storage types. If you want to save the data 
//  in registered clipboard format you have to define OX_CLIP_DO_CUSTOM_FORMATS
//  macro. The data in custom registered format may come with different types of the  
//  storage like not only HGLOBAL or HBITMAP that can be copied and stored safely
//  but also like pointer to IStorage object. There is no warranty that if we 
//  save this pointer that next time this pointer actually be valid, but the data 
//  can be changed already. So, it is up to you to decide will you work with this formats
//  or wont.
//
//  Usually we do not have to do anything additional to use this class.
//  Just insert the class in your project and it is ready to use.
//  The class provides many helper functions to customize drawing, like adjusting
//  different offsets, customizables icons and their sizes. 
//
// Dependencies:
//
// "OXMulticlipboardDlg.h"
//
// "OXMulticlipboardDlg.rc"
//   
//  "OXMulticlipboardDlg.cpp"	
//  "OXClipPocket.cpp"
//  "OXToolTipCtrl.cpp"
//
//  Sample: "..\Ultimate Toolbox\Samples\Utility\ClipDock\"
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(_MULTICLIPBOARDDLG_H__)
#define _MULTICLIPBOARDDLG_H__

#include "OXToolTipCtrl.h"	
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXdllext.h"

#include "OXClipPocket.h"
#include "OXToolTipCtrl.h"
#include "OXMainRes.h"
#include "UTB64Bit.h"


////////////////////////////////////////////////////////////////////////////
//definitions

#define OX_CLIP_ICON_WIDTH 32
#define OX_CLIP_ICON_SPACE 4
#define OX_CLIP_EDGE_OFFSET 4
#define OX_CLIP_STATIC_DIMENSION 10

#define OX_CLIP_MAX_POCKETS 8
#define OX_CLIP_POCKETS_IN_ROW 4
#define OX_CLIP_FIRST_ID 0x1000

/////////////////////////////////////////////////////////////////////////////
// CMulticlipboardDlg dialog



class OX_CLASS_DECL COXMulticlipboardDlg : public CDialog
{
	DECLARE_DYNCREATE(COXMulticlipboardDlg)

friend COXClipPocket;

	
public:

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect: Standard constructor
	COXMulticlipboardDlg(CWnd* pParent = NULL);	
	
	// --- In  :	pPocket - pointer to the COXClipPocket object
	// --- Out : 
	// --- Returns:
	// --- Effect: Copies contents of the pocket to the clipboard
	virtual void PocketToClip(COXClipPocket* pPocket);
	
	// --- In  :	pPocket - pointer to the COXClipPocket object
	// --- Out : 
	// --- Returns:
	// --- Effect: Copies contents of the clipboard to the pocket
	virtual void ClipToPocket(COXClipPocket* pPocket);

	
	// --- In  :	nPocket - number of the pocket to remove
	// --- Out : 
	// --- Returns:
	// --- Effect: Removes specified pocket. 
	void RemovePocket(int nPocket);
	
	// --- In  :	nPocket - number of the pocket to empty
	// --- Out : 
	// --- Returns:
	// --- Effect: Empties contents of the pocket. Does not destroy the pocket.
	void EmptyPocket(int nPocket);

	// --- In  :	pPocket - pointer to the COXClipPocket object
	// --- Out : 
	// --- Returns:
	// --- Effect: Empties contents of the pocket. Does not destroy the pocket.
	void EmptyPocket(COXClipPocket* pPocket);

	// --- In  :	nDimension - offset of the static control from the border
	// --- Out : 
	// --- Returns:
	// --- Effect: Affects appearence. Set new offset for static control.
	inline void SetStaticDimension(UINT nDimension)
		{m_nStaticDimension=nDimension;}

	// --- In  :
	// --- Out : 
	// --- Returns: Offset of the static control from the border 
	// --- Effect: Call this function to get static control offset
	inline UINT GetStaticDimension() const
		{return m_nStaticDimension;}

	// --- In  :	nOffset - width of the buttons' borders (COXClipPocket) 
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets new width of the buttons' border. Affects appearence
	inline void SetEdgeOffset(UINT nOffset)
		{m_nEdgeOffset=nOffset;}

	// --- In  :
	// --- Out : 
	// --- Returns: width of the buttons' borders (COXClipPocket) 
	// --- Effect: Gets width of the buttons' border.
	inline UINT GetEdgeOffset() const
		{return m_nEdgeOffset;}

	// --- In  :	nWidth - new width of the icons be calculated 
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets new icon width
	inline void SetIconWidth(UINT nWidth)
		{m_nIconWidth=nWidth;}

	// --- In  :	nHeight - new icon height be calculated
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets new icon height
	inline void SetIconHeight(UINT nHeight)
		{m_nIconHeight=nHeight;}
	
	// --- In  :
	// --- Out : 
	// --- Returns: icon width
	// --- Effect: Gets icon width
	inline UINT GetIconWidth() const
		{return m_nIconWidth;}

	// --- In  :
	// --- Out : 
	// --- Returns: icon height
	// --- Effect: Gets icon height
	inline UINT GetIconHeight() const
		{return m_nIconHeight;}

	// --- In  :
	// --- Out : 
	// --- Returns: space (in pixels) between pockets 
	// --- Effect: Returns space between icons
	inline UINT GetIconSpace() const
		{return m_nIconSpace;}

	// --- In  :	nSpace - new space between icons
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets new space between icons
	inline void SetIconSpace(UINT nSpace)
		{m_nIconSpace=nSpace;}

	// --- In  :
	// --- Out : 
	// --- Returns: number of the pockets per row
	// --- Effect: Returns number of the pockets per row
	inline UINT GetPocketsInRow() const
		{return m_nPocketsInRow;}

	// --- In  :	nPockets - number of the pockets
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets new number of the pockets per row
	inline void SetPocketsInRow(UINT nPockets)
		{m_nPocketsInRow=nPockets;}

	// --- In  :	bReplace - TRUE to replace pockets FALSE otherwise
	// --- Out : 
	// --- Returns:
	// --- Effect: Turns on/off switching from the last pocket 
	//				to the first one if comes data and last pocket 
	//				is not empty
	inline void SetReplacePockets(BOOL bReplace=TRUE)
		{m_bReplacePockets=bReplace;}

	// --- In  :	
	// --- Out : 
	// --- Returns: TRUE if replacing pockets turned on, FALSE if off
	// --- Effect: Gets replace pockets parameter
	inline BOOL GetReplacePockets() const
		{return m_bReplacePockets;}

	// --- In  :	nLast - index of the last pocket 
	//						should be in the dialog
	// --- Out : 
	// --- Returns: number of the pockets actually can be settled on the dialog
	// --- Effect: This function recalculate all geometrical sizes and actually
	//				moves static control and pockets if nessesary. If CanResize()
	//				returns TRUE this function will resize dialog in order to place
	//				all requested buttons.
	int AdjustSize(UINT nLast);

	// --- In  :
	// --- Out : 
	// --- Returns:		index of the last pocket in the collection
	// --- Effect: Gets last pocket index
	inline int GetLastPocketIndex() const
	{return PtrToInt(m_pPockets.GetUpperBound());}

	// --- In  :
	// --- Out : 
	// --- Returns: current pocket 
	// --- Effect:  Returns current pocket. This is a last pocket
	//				that has received data from the clipboard.
	inline int GetCurrentPocket() const
		{return m_nCurrentPocket;}

	// --- In  :	nPocket - number of the pocket in the collection 
	// --- Out : 
	// --- Returns: pointer to the pocket on success, NULL otherwise
	// --- Effect: Retrieves pointer to the COXClipPocketObject
	inline COXClipPocket* GetPocket(int nPocket) const
	{
		return (m_pPockets.GetSize() && (nPocket<m_pPockets.GetSize()))?
			(COXClipPocket*) m_pPockets.GetAt(nPocket):NULL;
	}

	// --- In  :
	// --- Out : 
	// --- Returns: maximum number of pockets 
	// --- Effect: Gets max number of pockets may be created
	inline UINT GetMaxPocketsCount() const
		{return m_nMaxPocketsCount;}

	// --- In  : nCount - max number of pockets
	// --- Out : 
	// --- Returns:
	// --- Effect: Sets max number of the pockets may be created
	inline void SetMaxPocketsCount(UINT nCount)
		{m_nMaxPocketsCount=nCount;}
	
	// --- In  :
	// --- Out : 
	// --- Returns: pointer to the tooltip control
	// --- Effect: Call this function to obtain pointer 
	//				to the tooltip control.
	inline COXToolTipCtrl* GetToolTip() { return &m_tip;}

	// --- In  :
	// --- Out : 
	// --- Returns: pointer to the image list
	// --- Effect: Call this function to obtain 
	//				pointer of the image list if you want provide your own images
	inline CImageList* GetImageList() { return &m_imgList;}

protected:

	// --- In  : pPocket - pocket where the data will be settled
	//			 pObject - COleDataObject representing clipboard
	// --- Out : 
	// --- Returns:
	// --- Effect: Override this function if you want 
	//				to place the data in the pocket in certain format
	virtual BOOL ClipObjectToPocket(COXClipPocket* pPocket, 
		COleDataObject* pObject);

	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE on success, FALSE otherwise
	// --- Effect: Loads imagelist from the resource. To provide 
	//				your own resources override this function
	virtual BOOL LoadImageList();

	// --- In  :	nPocketNumber - number of the pocket 
	//				where the data from the clipboard has been placed
	// --- Out : 
	// --- Returns:
	// --- Effect: Override this member function to handle the changes
	virtual void OnClipInfoChanged(UINT nPocketNumber);

	// --- In  :
	// --- Out : 
	// --- Returns: Height of the buttons placed at the top of the dialog.
	//				Always returns NULL
	// --- Effect: Override this member function if you have placed button 
	//				at the top of the dialog 
	//				to calculate geometric sizes correctly
	virtual UINT GetButtonsHeight();
	
	// --- In  :
	// --- Out : 
	// --- Returns: FALSE if dialog cannot be resized, TRUE otherwise.
	//				Returns TRUE by default
	// --- Effect: Override this member if you do not want to resize the dialog
	virtual BOOL CanResize();

	void MoveStatic();
	void CalculateDesiredRect(int nPockets, CRect& rect);
	void MovePockets(int nLast);
	BOOL GetProposedRect(UINT nNumber, CRect& rect);
	COXClipPocket* GetNextPocket();

//members

protected:

	int m_nCurrentPocket;
	int m_nMaxPocketsCount;
	int m_nPocketsInRow;
	
	UINT m_nIconSpace;
	UINT m_nIconWidth;
	UINT m_nIconHeight;
	UINT m_nStaticDimension;
	UINT m_nEdgeOffset;



	BOOL m_bReplacePockets;
	BOOL m_bPaste;

	HWND m_hWndNext;
	
	COXToolTipCtrl m_tip;
	CPtrArray m_pPockets;
	CImageList m_imgList;
	

//	HICON m_hIcon;
	

// Dialog Data
	//{{AFX_DATA(CMulticlipboardDlg)
	enum { IDD = IDD_OX_MULTICLIPBOARD_DIALOG};
	CString	m_edt;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMulticlipboardDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

	
//	BOOL GetHorizontalPosition(CRect& rect);

	// Generated message map functions
	//{{AFX_MSG(CMulticlipboardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_MULTICLIPBOARDDLG_H__)
