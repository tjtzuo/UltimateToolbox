// ==========================================================================
// 					Class Specification : COXClipPocket
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
//	This is a helper class. It works with COXMulticlipboardDlg class
//  implementing storage for the clipboard data and drawing capabilities.
//  The class is derived from CButton and invoke BN_CLICKED message
//  to notify parent dialog on left button click if the pocket has some data
//  
//  The structure sOXData implemets actually storage for the data and provides some
//  simple methods like GetData(), SetData(), Clear()
//	
/////////////////////////////////////////////////////////////////////////////
#if !defined(_OXCLIPPOCKET_H__)
#define _OXCLIPPOCKET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OXClipPocket.h : header file
//


#include "OXdllext.h"
#include "OXDib.h"

class COXMulticlipboardDlg;

#define OX_CLIP_CF_NONE 0
#define OX_CLIP_CUSTOM_FORMAT 14


typedef struct sOXData{

//data members
	UINT nFormat;
	STGMEDIUM Stgmed;

//constructor
	sOXData() : nFormat(OX_CLIP_CF_NONE)
	{
		Stgmed.tymed=TYMED_NULL;
		Stgmed.hGlobal=NULL;
		Stgmed.pUnkForRelease=NULL;
	}

	sOXData(const sOXData& sSource) : nFormat(sSource.nFormat)
	{
		::CopyStgMedium(&sSource.Stgmed, &Stgmed);
	}

	const sOXData& operator=(const sOXData& sSource)
	{
		nFormat=sSource.nFormat;
		::CopyStgMedium(&sSource.Stgmed, &Stgmed);
		return *this;
	}

//destructor
	~sOXData() { Clear();}

//member functions	
	void Clear() 
	{
		::ReleaseStgMedium(&Stgmed);
		Stgmed.tymed=TYMED_NULL;
		Stgmed.hGlobal=NULL;
		nFormat=NULL;
	}

	void SetData(UINT nNewFormat, STGMEDIUM* pData)
	{
		Clear();
		if (!nNewFormat || !pData)
			return;


		Stgmed.tymed=pData->tymed;
		Stgmed.hGlobal=pData->hGlobal;
		Stgmed.pUnkForRelease=pData->pUnkForRelease;
		nFormat=nNewFormat;
	}

}tOXData;

/////////////////////////////////////////////////////////////////////////////
// COXClipPocket window

class OX_CLASS_DECL COXClipPocket : public CButton
{
	DECLARE_DYNAMIC(COXClipPocket)
public:

	// --- In  :	pParent - pointer to a parent window
	// --- Out : 
	// --- Returns:
	// --- Effect: Standard constructor
	COXClipPocket(COXMulticlipboardDlg* pParent);
	// --- In  :
	// --- Out : 
	// --- Returns: pointer to the structure sOXData
	// --- Effect: Call this function to obtain data of the pocket 
	tOXData* GetData();

	// --- In  :	nType - clipboard format of the data
	// --- Out :	pData - pointer to the data
	// --- Returns:
	// --- Effect: Sets data to the pocket. 
	//				This function does not create copy of the data rather
	//				it save data directly
	void SetData(UINT nType, STGMEDIUM* pData);

	// --- In  :
	// --- Out : 
	// --- Returns: Clipboard format of the data
	// --- Effect: Call this function to check clipboard format of the data
	//				in the pocket
	UINT GetDataType() const;
 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXClipPocket)
	public:
	virtual BOOL Create(const POINT& ptTopLeft, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:

	
	virtual ~COXClipPocket();
	// Generated message map functions
protected:
	COXMulticlipboardDlg* m_pParent;
	BOOL m_bLButtonDown;
	BOOL m_bMouseOver;
	tOXData m_data;
	

	//{{AFX_MSG(COXClipPocket)
#if _MSC_VER == 1310	// VS 2003 - this shouldn't be necessary
	afx_msg UINT OnNcHitTest(CPoint point);
#else
	afx_msg LRESULT OnNcHitTest(CPoint point);
#endif
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OXCLIPPOCKET_H__)
