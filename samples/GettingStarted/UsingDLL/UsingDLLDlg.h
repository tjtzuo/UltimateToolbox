
// UsingDLLDlg.h : header file
//

#pragma once


// CUsingDLLDlg dialog

#include "OXStaticHyperLink.h"
#include "afxwin.h"

class CUsingDLLDlg : public CDialog
{
// Construction
public:
	CUsingDLLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USINGDLL_DIALOG };
#endif

	COXStaticHyperLink	m_HyperLink;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//CStatic m_HyperLink;
};
