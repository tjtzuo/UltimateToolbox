// ==========================================================================
//			Class Implementation: COXCustomizeTabbedMDIPage
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "OXCustomizeTabbedMDIPage.h"
#include "OXCustomizeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const UINT IDC_RADIO_ORIENT_LEFT=2000;
const UINT IDC_RADIO_ORIENT_TOP=2001;
const UINT IDC_RADIO_ORIENT_RIGHT=2002;
const UINT IDC_RADIO_ORIENT_BOTTOM=2003;

const UINT IDC_RADIO_APPEARANCE_TAB_BUTTONS=2010;
const UINT IDC_RADIO_APPEARANCE_PUSH_BUTTONS=2011;
const UINT IDC_RADIO_APPEARANCE_FLAT_BUTTONS=2012;

const UINT IDC_RADIO_POSITIONING_MULTILINE=2020;
const UINT IDC_RADIO_POSITIONING_SINGLELINE=2021;

const UINT IDC_CHECK_DISPLAY_HOTTRACK=2040;
const UINT IDC_CHECK_DISPLAY_FIXEDWIDTH=2041;
const UINT IDC_CHECK_DISPLAY_NORAGGEDRIGHT=2042;
const UINT IDC_CHECK_DISPLAY_SCROLLOPPOSITE=2043;
const UINT IDC_CHECK_DISPLAY_FORCEICONLEFT=2044;
const UINT IDC_CHECK_DISPLAY_FORCELABELLEFT=2045;

#ifndef TCS_FLATBUTTONS 
#define TCS_FLATBUTTONS         0x0008
#endif




/////////////////////////////////////////////////////////////////////////////
// COXCustomizeTabbedMDIPage dialog


IMPLEMENT_DYNCREATE(COXCustomizeTabbedMDIPage, COXCustomizePage)


COXCustomizeTabbedMDIPage::COXCustomizeTabbedMDIPage()
{
	//{{AFX_DATA_INIT(COXCustomizeTabbedMDIPage)
	m_bSupportTabbedMDI = FALSE;
	m_dwOffset = 0;
	//}}AFX_DATA_INIT
	m_pHelpWnd=NULL;
	m_dwStyle=0;
	m_nDialogID=IDD;
	m_sProfileName.Empty();
}


COXCustomizeTabbedMDIPage::~COXCustomizeTabbedMDIPage()
{
}


BOOL COXCustomizeTabbedMDIPage::
InitializeTabbedMDI(DWORD dwTabCtrlStyle/*=DEFAULT_TABCTRLSTYLE*/,
					DWORD dwOffset/*=ID_TABOFFSET*/, 
					LPCTSTR lpszProfileName/*=_T("CustomizeTabbedMDI")*/,
					BOOL bSupportTabbedMDI/*=TRUE*/)
{

	CWnd* pWnd=AfxGetMainWnd();
	ASSERT(pWnd!=NULL && ::IsWindow(pWnd->GetSafeHwnd()));
	ASSERT_KINDOF(CMDIFrameWnd,pWnd);
	if(!pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
		TRACE(_T("COXCustomizeTabbedMDIPage::InitializeTabbedMDI: tabbed MDI is supported only for MDI applications\n"));
		return FALSE;
	}

	if(bSupportTabbedMDI)
	{
		if(!m_MTIClientWnd.Attach((CMDIFrameWnd*)pWnd,dwTabCtrlStyle))
		{
			TRACE(_T("COXCustomizeTabbedMDIPage::InitializeTabbedMDI: failed to initialize tabbed MDI support\n"));
			return FALSE;
		}
		m_MTIClientWnd.GetTabCtrl()->SetOffset(dwOffset);
	}

	if(lpszProfileName!=NULL)
	{
		m_sProfileName=lpszProfileName;
		m_MTIClientWnd.LoadState(lpszProfileName);
	}

	m_bSupportTabbedMDI=m_MTIClientWnd.IsAttached();

	return TRUE;
}


BOOL COXCustomizeTabbedMDIPage::OnCloseManager(BOOL bIsOk) 
{	
	if(bIsOk)
	{
		if(!ApplyChanges())
		{
			return FALSE;
		}
		if(!m_sProfileName.IsEmpty())
		{
			m_MTIClientWnd.SaveState(m_sProfileName); 
		}
	}

	return TRUE;
}


void COXCustomizeTabbedMDIPage::DoDataExchange(CDataExchange* pDX)
{
	COXCustomizePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COXCustomizeTabbedMDIPage)
	DDX_Control(pDX, IDC_OX_TREE_TABCTRL_SETTINGS, m_treeTabCtrlSettings);
	DDX_Control(pDX, IDC_OX_SPIN_OFFSET, m_spinOffset);
	DDX_Check(pDX, IDC_OX_CHECK_SUPPORT_TABBEDMDI, m_bSupportTabbedMDI);
	DDX_Text(pDX, IDC_OX_EDIT_OFFSET, m_dwOffset);
	DDV_MinMaxDWord(pDX, m_dwOffset, 0, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COXCustomizeTabbedMDIPage, COXCustomizePage)
	//{{AFX_MSG_MAP(COXCustomizeTabbedMDIPage)
	ON_BN_CLICKED(IDC_OX_CHECK_SUPPORT_TABBEDMDI, OnCheckSupportTabbedmdi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXCustomizeTabbedMDIPage message handlers

BOOL COXCustomizeTabbedMDIPage::Load(const COXCustomizeManager* pCustomizeManager)
{
	// Call default implementation. It will load this demo dialog as the first 
	// page and will create About and CodeSample pages if specified.
	if(!COXCustomizePage::Load(pCustomizeManager))
		return FALSE;

	return TRUE;
}


void COXCustomizeTabbedMDIPage::Unload()
{
	// add here code for cleaning up all objects created by demo
	//
	//
	//////////////////////////////////////////////////////////////////////////

	COXCustomizePage::Unload();
}

void COXCustomizeTabbedMDIPage::OnInitDialog()
{
	// must call default implementation
	COXCustomizePage::OnInitDialog();

	// add here initialization code for your demo dialog. Treat it as a
	// normal CDialog::OnInitDialog function

	// spin control
	m_spinOffset.SetRange(0,1000);

	GetVars();

	// layout
	//
	m_LayoutManager.TieChild(&m_treeTabCtrlSettings,OX_LMS_ANY,OX_LMT_SAME);

	m_LayoutManager.TieChild(IDC_OX_STATIC_OFFSET,OX_LMS_BOTTOM,OX_LMT_SAME);
	m_LayoutManager.TieChild(IDC_OX_EDIT_OFFSET,OX_LMS_BOTTOM,OX_LMT_SAME);
	m_LayoutManager.TieChild(IDC_OX_SPIN_OFFSET,OX_LMS_BOTTOM,OX_LMT_SAME);
	//
	///////////////////////////////////////

	m_bInitialized=TRUE;

	UpdateData(FALSE);

	OnCheckSupportTabbedmdi();
}


BOOL COXCustomizeTabbedMDIPage::ApplyChanges()
{
	if(!UpdateData(TRUE))
		return FALSE;

	CWnd* pWnd=AfxGetMainWnd();
	ASSERT(pWnd!=NULL && ::IsWindow(pWnd->GetSafeHwnd()));
	ASSERT_KINDOF(CMDIFrameWnd,pWnd);
	if(!pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
		TRACE(_T("COXCustomizeTabbedMDIPage::InitializeTabbedMDI: tabbed MDI is supported only for MDI applications\n"));
		return FALSE;
	}

	m_dwStyle=0;
	// orientation
	switch(m_treeTabCtrlSettings.GetCheckedRadioButton(IDC_RADIO_ORIENT_LEFT,
		IDC_RADIO_ORIENT_BOTTOM))
	{
	case IDC_RADIO_ORIENT_LEFT:
		m_dwStyle|=TCS_VERTICAL;
		break;
	case IDC_RADIO_ORIENT_TOP:
		break;
	case IDC_RADIO_ORIENT_RIGHT:
		m_dwStyle|=TCS_VERTICAL|TCS_BOTTOM;
		break;
	case IDC_RADIO_ORIENT_BOTTOM:
		m_dwStyle|=TCS_BOTTOM;
		break;
	default:
		ASSERT(FALSE);
	}

	// appearance
	switch(m_treeTabCtrlSettings.GetCheckedRadioButton(IDC_RADIO_APPEARANCE_TAB_BUTTONS,
		IDC_RADIO_APPEARANCE_FLAT_BUTTONS))
	{
	case IDC_RADIO_APPEARANCE_TAB_BUTTONS:
		break;
	case IDC_RADIO_APPEARANCE_PUSH_BUTTONS:
		m_dwStyle|=TCS_BUTTONS;
		break;
	case IDC_RADIO_APPEARANCE_FLAT_BUTTONS:
		m_dwStyle|=TCS_BUTTONS|TCS_FLATBUTTONS;
		break;
	default:
		ASSERT(FALSE);
	}

	// positioning
	switch(m_treeTabCtrlSettings.GetCheckedRadioButton(IDC_RADIO_POSITIONING_SINGLELINE,
		IDC_RADIO_POSITIONING_MULTILINE))
	{
	case IDC_RADIO_POSITIONING_SINGLELINE:
		break;
	case IDC_RADIO_POSITIONING_MULTILINE:
		m_dwStyle|=TCS_MULTILINE;
		break;
	default:
		ASSERT(FALSE);
	}

	// display
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_HOTTRACK)==OTITEM_CHECKED)
		m_dwStyle|=TCS_HOTTRACK;
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_FIXEDWIDTH)==OTITEM_CHECKED)
		m_dwStyle|=TCS_FIXEDWIDTH;
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_NORAGGEDRIGHT)==OTITEM_UNCHECKED)
		m_dwStyle|=TCS_RAGGEDRIGHT;
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_SCROLLOPPOSITE)==OTITEM_CHECKED)
		m_dwStyle|=TCS_SCROLLOPPOSITE;
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_FORCEICONLEFT)==OTITEM_CHECKED)
		m_dwStyle|=TCS_FORCEICONLEFT;
	if(m_treeTabCtrlSettings.GetCheck(IDC_CHECK_DISPLAY_FORCELABELLEFT)==OTITEM_CHECKED)
		m_dwStyle|=TCS_FORCELABELLEFT;

	//
	/////////////////////////////////

	if ( m_MTIClientWnd.m_hWnd != NULL )
	{
		if(m_bSupportTabbedMDI==m_MTIClientWnd.IsAttached() && 
			m_dwOffset==m_MTIClientWnd.GetTabCtrl()->GetOffset() &&
			m_dwStyle==(m_MTIClientWnd.GetTabCtrl()->GetStyle()&0x0fff))
		{
			return TRUE;
		}

		
		if(m_MTIClientWnd.IsAttached())
		{
			// check if it has been attached to background painter before
	#ifdef OX_CUSTOMIZE_BACKGROUND
			BOOL bPaintBackground=FALSE;
			COXBackgroundPainterOrganizer* pBackgroundOrganizer=
				m_pCustomizeManager->GetBackgroundPainterOrganizer();
			ASSERT(pBackgroundOrganizer!=NULL);
			COXDIB dib;
			PaintType paintType=Tile;
			COLORREF clrBk=RGB(0,0,0);
			CWnd* pOriginWnd=NULL;
			if(pBackgroundOrganizer->IsAttached(&m_MTIClientWnd))
			{
				COXBackgroundPainter* pBackgroundPainter=
					pBackgroundOrganizer->GetPainter(&m_MTIClientWnd);
				ASSERT(pBackgroundPainter!=NULL);
				COXDIB* pDIB=pBackgroundPainter->GetWallpaperImage();
				if(pDIB!=NULL)
					dib=*pDIB;
				paintType=pBackgroundPainter->GetPaintType();
				clrBk=pBackgroundPainter->GetBkColor();
				pOriginWnd=pBackgroundPainter->GetOriginWnd();

				pBackgroundOrganizer->Detach(&m_MTIClientWnd);
				bPaintBackground=TRUE;
			}
	#endif	//	OX_CUSTOMIZE_BACKGROUND

			VERIFY(m_MTIClientWnd.Detach());

	#ifdef OX_CUSTOMIZE_BACKGROUND
			if(bPaintBackground)
			{
				//static CWnd g_MTIClientWnd;
				if (m_pHelpWnd)
				{
					m_pHelpWnd->DestroyWindow();
					delete m_pHelpWnd;
				}
				m_pHelpWnd=new COXHelperWnd;
				m_pHelpWnd->Attach(((CMDIFrameWnd*)pWnd)->m_hWndMDIClient);
				ASSERT(pBackgroundOrganizer!=NULL);
				VERIFY(pBackgroundOrganizer->Attach(m_pHelpWnd,&dib,
					paintType,clrBk,pOriginWnd));
			}
	#endif	//	OX_CUSTOMIZE_BACKGROUND
		}
	}
	else if(m_bSupportTabbedMDI)
	{
		// check if there is CWnd object attached to MDIClient
		CWnd* pMDIClientWnd=CWnd::FromHandlePermanent(((CMDIFrameWnd*)pWnd)->
			m_hWndMDIClient);
		if(pMDIClientWnd!=NULL)
		{
			// check if it has been attached to background painter before
	#ifdef OX_CUSTOMIZE_BACKGROUND
			BOOL bPaintBackground=FALSE;
			COXBackgroundPainterOrganizer* pBackgroundOrganizer=
				m_pCustomizeManager->GetBackgroundPainterOrganizer();
			ASSERT(pBackgroundOrganizer!=NULL);
			COXDIB dib;
			PaintType paintType=Tile;
			COLORREF clrBk=RGB(0,0,0);
			CWnd* pOriginWnd=NULL;
			if(pBackgroundOrganizer->IsAttached(pMDIClientWnd))
			{
				COXBackgroundPainter* pBackgroundPainter=
					pBackgroundOrganizer->GetPainter(pMDIClientWnd);
				ASSERT(pBackgroundPainter!=NULL);
				COXDIB* pDIB=pBackgroundPainter->GetWallpaperImage();
				if(pDIB!=NULL)
					dib=*pDIB;
				paintType=pBackgroundPainter->GetPaintType();
				clrBk=pBackgroundPainter->GetBkColor();
				pOriginWnd=pBackgroundPainter->GetOriginWnd();

				pBackgroundOrganizer->Detach(pMDIClientWnd);
				bPaintBackground=TRUE;
			}
	#endif	//	OX_CUSTOMIZE_BACKGROUND

			pMDIClientWnd->Detach();

			VERIFY(m_MTIClientWnd.Attach((CMDIFrameWnd*)pWnd,m_dwStyle));

	#ifdef OX_CUSTOMIZE_BACKGROUND
			if(bPaintBackground)
			{
				ASSERT(pBackgroundOrganizer!=NULL);
				VERIFY(pBackgroundOrganizer->Attach(&m_MTIClientWnd,&dib,
					paintType,clrBk,pOriginWnd));
			}
	#endif	//	OX_CUSTOMIZE_BACKGROUND
		}
		else
		{
			VERIFY(m_MTIClientWnd.Attach((CMDIFrameWnd*)pWnd,m_dwStyle));
		}

		m_MTIClientWnd.GetTabCtrl()->SetOffset(m_dwOffset);
	}

	return TRUE;
}


void COXCustomizeTabbedMDIPage::GetVars()
{
	m_bSupportTabbedMDI=m_MTIClientWnd.IsAttached();
	m_dwOffset=(m_bSupportTabbedMDI ? 
		m_MTIClientWnd.GetTabCtrl()->GetOffset() : ID_TABOFFSET);
	m_dwStyle=(m_bSupportTabbedMDI ? 
		m_MTIClientWnd.GetTabCtrl()->GetStyle() : DEFAULT_TABCTRLSTYLE);

	// build tree control
	//
	m_treeTabCtrlSettings.DeleteAllItems();

	// root item
	int nImageIndex=m_treeTabCtrlSettings.
		AddImage(IDB_OX_CUSTOMIZETABBEDMDI_SETTINGS,RGB(192,192,192));
	ASSERT(nImageIndex!=-1);
	CString sItem;
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDSETTINGS));//"Tab control settings"
	HTREEITEM hRootItem=m_treeTabCtrlSettings.
		AddControlGroup(sItem,NULL,TRUE,nImageIndex,nImageIndex);
	ASSERT(hRootItem!=NULL);
	m_treeTabCtrlSettings.SetItemState(hRootItem,TVIS_BOLD,TVIS_BOLD);

	// orientation
	nImageIndex=m_treeTabCtrlSettings.
		AddImage(IDB_OX_CUSTOMIZETABBEDMDI_ORIENTATION,RGB(192,192,192));
	ASSERT(nImageIndex!=-1);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDORIENTATION));//"Orientation"
	HTREEITEM hGroupItem=m_treeTabCtrlSettings.
		AddControlGroup(sItem,hRootItem,TRUE,nImageIndex,nImageIndex);
	ASSERT(hGroupItem!=NULL);
	m_treeTabCtrlSettings.SetItemState(hGroupItem,TVIS_BOLD,TVIS_BOLD);

	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDLEFT));//"Left"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_ORIENT_LEFT,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDTOP));//"Top"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_ORIENT_TOP,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDRIGHT));//"Right"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_ORIENT_RIGHT,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDBOTTOM));//"Bottom"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_ORIENT_BOTTOM,
		sItem,hGroupItem)!=NULL);

	if((m_dwStyle&(TCS_VERTICAL|TCS_BOTTOM))==(TCS_VERTICAL|TCS_BOTTOM))
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_ORIENT_RIGHT,OTITEM_CHECKED);
	else if((m_dwStyle&TCS_VERTICAL)==TCS_VERTICAL)
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_ORIENT_LEFT,OTITEM_CHECKED);
	else if((m_dwStyle&TCS_BOTTOM)==TCS_BOTTOM)
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_ORIENT_BOTTOM,OTITEM_CHECKED);
	else 
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_ORIENT_TOP,OTITEM_CHECKED);


	// appearance
	nImageIndex=m_treeTabCtrlSettings.
		AddImage(IDB_OX_CUSTOMIZETABBEDMDI_APPEARANCE,RGB(192,192,192));
	ASSERT(nImageIndex!=-1);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDAPPEARENCE));//"Appearance"
	hGroupItem=m_treeTabCtrlSettings.
		AddControlGroup(sItem,hRootItem,TRUE,nImageIndex,nImageIndex);
	ASSERT(hGroupItem!=NULL);
	m_treeTabCtrlSettings.SetItemState(hGroupItem,TVIS_BOLD,TVIS_BOLD);

	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDTABBUTTONS));//"Tab buttons"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_APPEARANCE_TAB_BUTTONS,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDPUSHBUTTONS));//"Push buttons"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_APPEARANCE_PUSH_BUTTONS,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDFLATBUTTONS));//"Flat buttons"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_APPEARANCE_FLAT_BUTTONS,
		sItem,hGroupItem)!=NULL);

	if((m_dwStyle&(TCS_BUTTONS|TCS_FLATBUTTONS))==(TCS_BUTTONS|TCS_FLATBUTTONS))
	{
		m_treeTabCtrlSettings.
			SetCheck(IDC_RADIO_APPEARANCE_FLAT_BUTTONS,OTITEM_CHECKED);
	}
	else if((m_dwStyle&TCS_BUTTONS)==TCS_BUTTONS)
	{
		m_treeTabCtrlSettings.
			SetCheck(IDC_RADIO_APPEARANCE_PUSH_BUTTONS,OTITEM_CHECKED);
	}
	else 
	{
		m_treeTabCtrlSettings.
			SetCheck(IDC_RADIO_APPEARANCE_TAB_BUTTONS,OTITEM_CHECKED);
	}
	

	// display
	nImageIndex=m_treeTabCtrlSettings.
		AddImage(IDB_OX_CUSTOMIZETABBEDMDI_DISPLAY,RGB(192,192,192));
	ASSERT(nImageIndex!=-1);

	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDDISPLAY));//"Display"
	hGroupItem=m_treeTabCtrlSettings.
		AddControlGroup(sItem,hRootItem,TRUE,nImageIndex,nImageIndex);
	ASSERT(hGroupItem!=NULL);
	m_treeTabCtrlSettings.SetItemState(hGroupItem,TVIS_BOLD,TVIS_BOLD);

	// positioning
	nImageIndex=m_treeTabCtrlSettings.
		AddImage(IDB_OX_CUSTOMIZETABBEDMDI_POSITIONING,RGB(192,192,192));
	ASSERT(nImageIndex!=-1);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDPOSITIONING));//"Positioning"
	HTREEITEM hSubGroupItem=m_treeTabCtrlSettings.
		AddControlGroup(sItem,hGroupItem,TRUE,nImageIndex,nImageIndex);
	ASSERT(hSubGroupItem!=NULL);
	m_treeTabCtrlSettings.SetItemState(hSubGroupItem,TVIS_BOLD,TVIS_BOLD);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDDISPLAY1ROW));//"Display only one row of tabs"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_POSITIONING_SINGLELINE,
		sItem,hSubGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDDISPLAYMROWS));//"Displays multiple rows of tabs, if necessary"
	VERIFY(m_treeTabCtrlSettings.AddRadioButton(IDC_RADIO_POSITIONING_MULTILINE,
		sItem,hSubGroupItem)!=NULL);

	if((m_dwStyle&TCS_MULTILINE)==TCS_MULTILINE)
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_POSITIONING_MULTILINE,OTITEM_CHECKED);
	else 
		m_treeTabCtrlSettings.SetCheck(IDC_RADIO_POSITIONING_SINGLELINE,OTITEM_CHECKED);

	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDHIGHLIGHT));//"Items under the pointer are automatically highlighted"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_HOTTRACK,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDWIDTH));//"Same width for all buttons"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_FIXEDWIDTH,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDSTRETCH));//"Stretch tabs to fill the entire width of the control"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_NORAGGEDRIGHT,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDOPPOSITE));//"Unused tabs move to the opposite side of the control"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_SCROLLOPPOSITE,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDALIGNICON));//"Aligns an icon with the left edge of a fixed-width tab"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_FORCEICONLEFT,
		sItem,hGroupItem)!=NULL);
	VERIFY(sItem.LoadString(IDS_OX_CSTMZETABBEDALIGNLABEL));//"Aligns a label with the left edge of a fixed-width tab"
	VERIFY(m_treeTabCtrlSettings.AddCheckBox(IDC_CHECK_DISPLAY_FORCELABELLEFT,
		sItem,hGroupItem)!=NULL);

	if((m_dwStyle&TCS_HOTTRACK)==TCS_HOTTRACK)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_HOTTRACK,OTITEM_CHECKED);
	if((m_dwStyle&TCS_FIXEDWIDTH)==TCS_FIXEDWIDTH)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_FIXEDWIDTH,OTITEM_CHECKED);
	if((m_dwStyle&TCS_RAGGEDRIGHT)!=TCS_RAGGEDRIGHT)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_NORAGGEDRIGHT,OTITEM_CHECKED);
	if((m_dwStyle&TCS_SCROLLOPPOSITE)==TCS_SCROLLOPPOSITE)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_SCROLLOPPOSITE,OTITEM_CHECKED);
	if((m_dwStyle&TCS_FORCEICONLEFT)==TCS_FORCEICONLEFT)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_FORCEICONLEFT,OTITEM_CHECKED);
	if((m_dwStyle&TCS_FORCELABELLEFT)==TCS_FORCELABELLEFT)
		m_treeTabCtrlSettings.SetCheck(IDC_CHECK_DISPLAY_FORCELABELLEFT,OTITEM_CHECKED);

	//
	/////////////////////////////////
}

void COXCustomizeTabbedMDIPage::OnCheckSupportTabbedmdi() 
{
	// TODO: Add your control notification handler code here

	if(!UpdateData(TRUE))
		return;

	m_treeTabCtrlSettings.EnableWindow(m_bSupportTabbedMDI);
	GetDlgItem(IDC_OX_EDIT_OFFSET)->EnableWindow(m_bSupportTabbedMDI);
	m_spinOffset.EnableWindow(m_bSupportTabbedMDI);
}
