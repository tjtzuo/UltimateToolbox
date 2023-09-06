// ==========================================================================
// 					Class Specification : COXHistoryCombo
// ==========================================================================

// Header file : OXHistoryCombo.h

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
//	YES	Derived from CComboBox

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         

// Remark:

// Prerequisites (necessary conditions):
//	This control needs idle processing to update its GUI (enable/disabme buttons etc)
//	It uses the WM_IDLEUPDATECMDUI message for this.
//	This message is send during on idle to all the children of the main frame

//	This kind of idle processing is not available from a message box
//	Here you can use the not documented WM_KICKIDLE message of the dialog
//  to explicly call the OnIdle function of this control
//		#include <afxpriv.h>	// for WM_KICKIDLE
//		ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
//		LRESULT CMyDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
//			{
//			ASSERT(wParam == MSGF_DIALOGBOX);
//			LONG lIdleCount = lParam;
//			BOOL bContinueIdle = TRUE;
//		
//			// Update the edit list
//			m_wndEditList.OnIdle();
//		
//			return bContinueIdle;
//			}

// This control needs the toolbar resource with the ID IDR_OX_HISTORY_COMBO_TOOLS
//	It contains the buttons ID_OX_HISTORY_COMBO_NEW, ID_OX_HISTORY_COMBO_DELETE,
//	ID_OX_HISTORY_COMBO_BROWSE, ID_OX_HISTORY_COMBO_RESERVED_1 and ID_OX_HISTORY_COMBO_RESERVED_2
//	The name of these last two ID may change in the future
//	So the first 5 buttons are fixed, but you can add your own buttons behind them

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXHISTORYCOMBO_H__
#define __OXHISTORYCOMBO_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef __AFXPRIV_H__
#include <afxpriv.h>
#endif	// __AFXPRIV_H__

// Maximum number of buttons in the toolbar of a history combo
#ifndef OX_HISTORY_COMBO_MAX_TOOLBUTTONS
#define OX_HISTORY_COMBO_MAX_TOOLBUTTONS	10
#endif

#ifndef ITEM_SEPERATOR
#define ITEM_SEPERATOR	_T("|")
#endif

#include "OXMainRes.h"
#include "MFCToolBar.h"

// v9.3 - update 05 - CMFCToolBar is now COXMFCToolBar to avoid conflict (VS2010)
//#define CToolBar CMFCToolBar
#define CToolBar COXMFCToolBar

#ifdef _UTDEMOBROWSER // DO NOT DEFINE THIS IN YOUR PROJECT
#include "OXSkinnedComboBox.h"
#define CComboBox COXSkinnedComboBox
#endif // _UTDEMOBROWSER

class OX_CLASS_DECL COXHistoryCombo : public CComboBox
{
DECLARE_DYNAMIC(COXHistoryCombo);
// Data members -------------------------------------------------------------
	enum EToolbarPosition
		{
		TBPNone = 1,
		TBPHorizontalTopLeft,
		TBPHorizontalTopCenter,
		TBPHorizontalTopRight,
		TBPHorizontalBottomLeft,
		TBPHorizontalBottomCenter,
		TBPHorizontalBottomRight,
		TBPHorizontalLeftTop,
		TBPHorizontalLeftCenter,
		TBPHorizontalLeftBottom,
		TBPHorizontalRightTop,
		TBPHorizontalRightCenter,
		TBPHorizontalRightBottom
		};
	static EToolbarPosition TBP_FIRST;
	static EToolbarPosition TBP_LAST;

protected:
	EToolbarPosition m_eToolbarPosition;
	BOOL		m_bUseGap;
	int			m_nMaxHistoryCount;
	CToolBar*	m_pToolbar;
	BOOL		m_rgbShowToolButton[OX_HISTORY_COMBO_MAX_TOOLBUTTONS];
	LPCTSTR		m_lpszToolbarResource;
	
	BOOL	m_bAutoPersistent;
	CString	m_sAutoPersistentCompany;
	CString	m_sAutoPersistentApplication;
	CString	m_sAutoPersistentValueName;

	struct CFileOpenParams
		{
		BOOL	m_bOpenFileDialog;
		CString m_sDefExt;
		CString	m_sFileName;
		DWORD	m_dwFlags;
		CString	m_sFilter;
		CWnd*	m_pParentWnd;
		CString m_sStartDir;//added 11/12/1999

		CFileOpenParams()
			:
			m_bOpenFileDialog(TRUE),
			m_sDefExt(_T("")),
			m_sFileName(_T("*.*")),
			m_dwFlags(OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT),
			m_sFilter(_T("All Files (*.*)| *.* ||")),
			m_pParentWnd(NULL)
			{
				//changed 11/12/1999
				TCHAR szCurrentDir[_MAX_PATH+1];
				::GetCurrentDirectory(_MAX_PATH, szCurrentDir);
				m_sStartDir=szCurrentDir;
			}

		} m_fileOpenParams;


private:
	
// Member functions ---------------------------------------------------------
public:
	// --- In  : eToolbarPosition : The initial position of the toolbar
	//			 bUseGap : Whether a small gap should be used between
	//                     the toolbar and the combo control
	//           lpszToolbarResource : The resource that will be used for the toolbar
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object
	COXHistoryCombo(EToolbarPosition eToolbarPosition = TBPHorizontalRightCenter,
		BOOL bUseGap = FALSE, LPCTSTR lpszToolbarResource = MAKEINTRESOURCE(IDR_OX_HISTORY_COMBO_TOOLS));

	// --- In  : nIDToolbarResource or lpszToolbarResource : 
	//				The resource that will be used for the toolbar
	// --- Out : 
	// --- Returns : 
	// --- Effect : This functions sets the resource for the toolbar
	//				This must be set BEFORE the toolbar is created
	void SetToolbarResource(UINT nIDToolbarResource = IDR_OX_HISTORY_COMBO_TOOLS);
	void SetToolbarResource(LPCTSTR lpszToolbarResource = MAKEINTRESOURCE(IDR_OX_HISTORY_COMBO_TOOLS));

	// --- In  : pParentWnd : The window that we be the parent window
	//						  If NULL GetParent() is used
	// --- Out : 
	// --- Returns :
	// --- Effect : Initializes this control
	virtual void InitCombo(CWnd* pParentWnd = NULL);

	// --- In  : eToolbarPosition : The new toolbar position
	//			 bUseGap : Whether a small gap should be used between
	//                     the toolbar and the combo control
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets the toolbar to a new position or 
	//              hides it (when eToolbarPosition == TBPNone)
	void PositionToolbar(EToolbarPosition eToolbarPosition = TBPHorizontalRightCenter,
		BOOL bUseGap = FALSE);

	// --- In  :
	// --- Out : 
	// --- Returns : The current toolbar position
	// --- Effect : 
	EToolbarPosition GetToolbarPosition() const;

	// --- In  : nButtonIndex : The zero-based index of the button
	//			 bShow : Whether the button should be visible or not
	// --- Out : 
	// --- Returns : 
	// --- Effect : Chenges the visibility of individual toolbar buttons
	//				By default are all buttons (except the reserved buttons) visible
	//				Call RefreshToolbar() to make these changes visible
	void ShowButton(int nButtonIndex, BOOL bShow = FALSE);

	// --- In  : nButtonIndex : The zero-based index of the button
	// --- Out : 
	// --- Returns : Whether the specified button is MARKED as visible
	//				 If you have not called ShowButton() after the last RefreshToolbar()
	//				 this reflect the actual current state
	// --- Effect : 
	BOOL IsButtonShown(int nButtonIndex) const;

	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Refreshes the toolbar so that chenges in the 
	//				visibility of individual buttons are shown
	void RefreshToolbar();

	// --- In  : nMaxHistoryCount : The maximum number of items that will be kept as a history
	//								(default is 6, -1 == no maximum)
	// --- Out : 
	// --- Returns : 
	// --- Effect : Set a new meximum
	void SetMaxHistoryCount(int nMaxHistoryCount);

	// --- In  : 
	// --- Out : 
	// --- Returns : The maximum number of items that will be kept as a history 
	//				 (-1 == no maximum)
	// --- Effect : 
	int GetMaxHistoryCount() const;

	// --- In  : pszItemText : Text of the item to add
	//				(if NULL : current contents of the edit control)
	// --- Out : 
	// --- Returns : Whether a NEW item was added
	// --- Effect : This function adds a new item
	//				When the item alredy existed, it is moved to the first position
	//				No empty items are added
	virtual BOOL AddNewItem(LPCTSTR pszItemText = NULL);

	// --- In  : nItemIndex : Zero-based index of the item to delete
	//				(if -1 : Current selection in the list)
	// --- Out : 
	// --- Returns : Whether an item was deleted
	// --- Effect : This function deletes an item
	virtual BOOL DeleteItem(int nItemIndex = -1);

	// --- In  : 
	// --- Out : 
	// --- Returns : TRUE if OK button was selected,
	//				 FALSE otherwise
	// --- Effect : This function shows file dialog to select a file
	virtual BOOL BrowseItem();

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether a new item is allowed to be added
	// --- Effect : 
	virtual BOOL CanAddNewItem() const;

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the currently selected item is allowed to be deleted
	// --- Effect : 
	virtual BOOL CanDeleteItem() const;

	// --- In  : 
	// --- Out : 
	// --- Returns : Whether a item is allowed to be browsed
	// --- Effect : 
	virtual BOOL CanBrowseItem() const;

	// --- In  : bOpenFileDialog :  (See CFileDialog::CFileDialog())
	//			 pszDefExt : 
	//			 pszFileName : 
	//			 dwFlags : 
	//			 pszFilter : 
	//			 pParentWnd :
	//			 lpszInitDir: initial directory	//changed 11/12/1999
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function sets the new params for the file open dialog
	void SetFileDialogParams(BOOL bOpenFileDialog = TRUE, LPCTSTR pszDefExt = NULL,
		LPCTSTR pszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		LPCTSTR pszFilter = NULL, CWnd* pParentWnd = NULL,
		/*added 11/12/1999*/LPCTSTR lpszInitDir =NULL);

	// --- In  :
	// --- Out : 
	// --- Returns : Whether furter idle processing is requested
	// --- Effect : This function should be called so that this control
	//              can update its GUI (enable/disable buttons etc.)
	BOOL OnIdle(LONG lCount = 0);

	// --- In  : pszValueName   The name of the value to set. 
	//			 pszCompany : The name of the subkey with which a value is associated. 
	//				If the parameter is NULL, AfxGetApp()->m_pszRegistryKey is used 
	//				(use AfxGetApp()->SetRegistryKey() to set this to your company name)
	//			 pszApplication : Name of this application
	//				If the paramater is NULL, AfxGetApp()->m_pszProfileName is used
	// --- Out : 
	// --- Returns : Whether save succeeded
	// --- Effect : This function restores the persistent values upon creation and 
	//				saves them when the control is destroyed
	BOOL SetAutoPersistent(LPCTSTR pszValueName, LPCTSTR pszCompany = NULL, LPCTSTR pszApplication = NULL);

	// --- In  :
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function cancels Auto Persistent mode
	void RemoveAutoPersistent();

	// --- In  : pszValueName   The name of the value to set. 
	//			 pszCompany : The name of the subkey with which a value is associated. 
	//				If the parameter is NULL, AfxGetApp()->m_pszRegistryKey is used 
	//				(use AfxGetApp()->SetRegistryKey() to set this to your company name)
	//			 pszApplication : Name of this application
	//				If the paramater is NULL, AfxGetApp()->m_pszProfileName is used
	//			 hKeyRoot : An open key in the registry or any of the following predefined handle values: 
	//				HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS
	//			 bAddNewItem : Whether the current string in the edit control should
	//				be added to the list before saving the contents
	// --- Out : 
	// --- Returns : Whether save succeeded
	// --- Effect : This function saves the current contents in the specified value of the registry key
	//				The contents includes all entries in the list 
	//				All items in registry are seperated by a vertical bar (|)
	//				The key used is hKeyRoot\"Software"\<pszCompany>\<pszApplication>\"HistoryCombo"
	BOOL SaveContents(LPCTSTR pszValueName, LPCTSTR pszCompany = NULL, 
		LPCTSTR pszApplication = NULL, HKEY hKeyRoot = HKEY_CURRENT_USER, 
		BOOL bAddNewItem = TRUE);

	// --- In  : pszValueName   The name of the value to set. 
	//			 pszCompany : The name of the subkey with which a value is associated. 
	//				If the parameter is NULL, AfxGetApp()->m_pszRegistryKey is used 
	//				(use AfxGetApp()->SetRegistryKey() to set this to your company name)
	//			 pszApplication : Name of this application
	//				If the paramater is NULL, AfxGetApp()->m_pszProfileName is used
	//			 hKeyRoot : An open key in the registry or any of the following predefined handle values: 
	//				HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS
	//			 bRestoreFirstItem : Whether the first item of the list should be put
	//				in the edit control
	// --- Out : 
	// --- Returns : Whether restore succeeded
	// --- Effect : This function restores the current contents from the specified value of the registry key
	//				All items in registry should be seperated by a vertical bar (|)
	//				The key used is hKeyRoot\"Software"\<pszCompany>\<pszApplication>\"HistoryCombo"
	BOOL RestoreContents(LPCTSTR pszValueName, LPCTSTR pszCompany = NULL, 
		LPCTSTR pszApplication = NULL, HKEY hKeyRoot = HKEY_CURRENT_USER, 
		BOOL bRestoreFirstItem = TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns: Number of toolbar buttons that set as visible 
	// --- Effect : calculate number of visible toolbar buttons
	int GetVisibleButtonCount();

	// --- In  :
	// --- Out : 
	// --- Returns: Pointer to internal toolbar control
	// --- Effect : Retrieves pointer to internal toolbar control
	inline CToolBar* GetToolBar() const { return m_pToolbar; }

	// --- In		:	bFlatToolBar	-	flag that specifies whether the internal 
	//										toolbar will be displayed flat or not
	// --- Out		: 
	// --- Returns	:	TRUE if function succeeds, or FALSE otherwise
	// --- Effect	:	Sets/Removes internal toolbar flat style
	inline BOOL SetFlatToolbar(BOOL bFlatToolBar) 
	{
		BOOL bResult=FALSE;
		if(::IsWindow(m_pToolbar->GetSafeHwnd()))
		{
			if(bFlatToolBar)
				bResult=m_pToolbar->ModifyStyle(NULL,TBSTYLE_FLAT);
			else
				bResult=m_pToolbar->ModifyStyle(TBSTYLE_FLAT,NULL);
			if(bResult)
				m_pToolbar->RedrawWindow();
		}
		return bResult;
	}

#ifdef _DEBUG
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.
	virtual void AssertValid() const;
	
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 
	virtual void Dump(CDumpContext& dc) const;
#endif

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of the object
	virtual ~COXHistoryCombo();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXHistoryCombo)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


protected:
	virtual BOOL IsFrameWnd() const;
	void RemoveButtons();
	CString GetContents() const;
	void SetContents(LPCTSTR pszContents);
	static BOOL SaveContentsToRegistry(HKEY hKeyRoot, LPCTSTR pszCompany, LPCTSTR pszApplication,
		LPCTSTR pszValueName, LPCTSTR pszValue);
	static BOOL LoadContentsFromRegistry(HKEY hKeyRoot, LPCTSTR pszCompany, LPCTSTR pszApplication,
		LPCTSTR pszValueName, CString& sValue);

	//{{AFX_MSG(COXHistoryCombo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnNew();
	afx_msg void OnUpdateNew(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnBrowse();
	afx_msg void OnUpdateBrowse(CCmdUI* pCmdUI);
	afx_msg void OnReserved1();
	afx_msg void OnUpdateReserved1(CCmdUI* pCmdUI);
	afx_msg void OnReserved2();
	afx_msg void OnUpdateReserved2(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nControlID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolbarCommand(UINT nCommand);
	afx_msg void OnToolbarUpdateUI(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
private:
                   
};

#endif // __OXHISTORYCOMBO_H__
// ==========================================================================
