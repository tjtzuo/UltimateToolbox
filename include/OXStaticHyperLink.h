// ==========================================================================
// 						Class Specification : 
//						COXStaticHyperLink
// ==========================================================================

// Header file : OXStaticHyperLink.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
#ifndef _STATICHYPERLINK_H
#define _STATICHYPERLINK_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXMainRes.h"
#include "OXStatic.h"
#include "OXHyperLinkAction.h"



//////////////////////////////////////////////////////////////////////////
// COXStaticHyperLink implements a static control that's a hyperlink
// to any file on your desktop or web. E.g., you can use it in dialog boxes
// to create hyperlinks to web sites. When clicked, opens the file/URL
//
//
// COXStaticHyperLink is based on COXStatic class so you can use all its functions
// to define control's appearance the way you want. Of course there were added 
// some new features to provide functionality that similar to hyper link on web page.
//
// Every COXStaticHyperLink have associated COXHyperLinkAction that will be executed 
// if user clicks mouse left button when it's over control's window. By default 
// COXHyperLinkAction operation is set to "open". You can set any action to 
// COXStaticHyperLink using next functions:
//
//		void SetAction(COXHyperLinkAction& hla);
//		void SetAction(int nActionID = ID_HLACTION_OPEN, LPCTSTR sFile = NULL, 
//			LPCTSTR sParameters = NULL, LPCTSTR sDefaultDirectory = NULL, 
//			int nShowCmd = SW_SHOWNORMAL, UINT nCallbackMsgID=NULL,	
//			HWND hWndRecipient=NULL);
//		// --- Effect : sets the COXHyperLinkAction object that will be executed 
//		//				if user clicks left button on COXStaticHyperLink
//
//
// COXStaticHyperLink could be in two state: unvisited and visited 
// (like common hyperlink). Control's state changes in result of user action 
// (left button click) but you can change it programmatically using next function
//
//		void SetVisited(BOOL bVisited, BOOL bRedraw=TRUE);
//		// --- Effect : sets the state of control as visited or unvisited
//
// To differentiate control in different state we use different colors to draw
// COXStaticHyperLink (by default, blue/purple for unvisited/visited). Next functions
// are used to set color of control in different state:
//
//		void SetUnvisitedColor(COLORREF clr, BOOL bRedraw=TRUE);
//		// --- Effect : sets the color of COXStaticHyperLink object that 
//		//				wasn't visited (like unvisited link in HTML)
//		void SetVisitedColor(COLORREF clr, BOOL bRedraw=TRUE);
//		// --- Effect : sets the color of COXStaticHyperLink object that 
//		//				was visited (like visited link in HTML)
//
// To differentiate COXStaticHyperLink from other CStatic controls we use special 
// cursor. By default we use "hand cursor" that could be found in "OXBitmapButton.rc" 
// file (supplied with the Ultimate Toolbox). So, you have to include reference to 
// "OXBitmapButton.rc" resource in the resource file of your application. Of course 
// you can set whatever cursor you need using next function:
//
//		void SetLinkCursor(HCURSOR hCursor=NULL)
//		// --- Effect : sets the cursor that COXStaticHyperLink uses to visually show 
//		//				that mouse is moving over COXStaticHyperLink control,
//		//				if you don't want to differentiate visually hyperlink object  
//		//				then call this function with hCursor set to NULL.
//
// or you can set default cursor at any time using function:
//
//		virtual void SetDefaultLinkCursor()
//		// --- Effect : sets default cursor that COXStaticHyperLink uses to visually show 
//		//				that mouse is moving over COXStaticHyperLink control.
//		//				COXStaticHyperLink uses cursor wich is defined in "OXBitmapButton.rc" 
//		//				file (supplied with the Ultimate Toolbox) as default one. 
// 
//
// To simplify process of using COXStaticHyperLink object you can use window text as
// COXHyperLinkAction::m_sFile calling next function.
//
//		void SetUseWindowText(BOOL bUseWindowText);
//		// --- Effect :	if bUseWindowText is TRUE then if COXHyperLinkAction::m_sFile is 
//		//				NULL COXStaticHyperLink will use text of control's window as 
//		//				COXHyperLinkAction::m_sFile. By default this feature is set to TRUE.
//
//
// And the last thing. Some error could happen in result of execution of 
// COXHyperLinkAction. By default COXHyperLinkAction will notify you about such error
// displaying MessageBox with error description. To control the process of
// error notification use next function:
//
//		void SetErrorNotify(BOOL bErrorNotify)
//		// --- Effect : if TRUE then COXStaticHyperLink calls 
//						COXHyperLinkAction::ErrorNotify function if an error occurs 
//						during execution of an action
//
//
// Below you will find examples how to use COXStaticHyperLink with different type
// of actions.At the moment 6 type of actions are defined:
//
//		1)	None - use this type if you need functionality
//				this control provide but don't want to invoke any action
//				if user clicks on the control
//
//				Below you will find example of using such action
//
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// do nothing if user clicks
//					m_ctlStaticLink.SetAction(ID_HLACTION_NONE);
//
//
//		2)	Userdefined - use this type if you want to define your own 
//				action that should be taken when a hyperlink is activated.
//				In that case you have to set callback message ID and
//				handle to the recipient	window to the corresponding
//				COXHyperLinkAction object. In your application you 
//				have to define unique message ID (the best way to do 
//				that is to call RegisterWindowMessage function):
//
//					UINT nCallbackMsgID=RegisterWindowMessage(_T("HyperLinkCallbackMessage"));
//
//				and function to react on that message. Such function have 
//				to follow next prototype:
//
//					afx_msg LONG OnHyperLinkActivated(UINT wParam, LONG lParam);
//						wParam - not used
//						lParam - pointer to corresponding COXHyperLinkAction object
//						return value is 1 if your action succeeded or 0 otherwise
//						
//				Below you will find example of using userdefined action
//							
//				in your header file:
//					// let it be COXStaticHyperLink object
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// in MESSAGE_MAP section declare the function:
//					afx_msg LONG OnHyperLinkActivated(UINT wParam, LONG lParam);
//
//				in your *.cpp file:
//					static UINT g_nCallbackMsgID=RegisterWindowMessage(_T("HyperLinkCallbackMessage"));
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					ON_REGISTERED_MESSAGE(g_nCallbackMsgID,OnHyperLinkActivated)
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					m_ctlStaticLink.SetAction(ID_HLACTION_USERDEFINED,NULL,NULL,NULL,NULL,
//						g_nCallbackMsgID,GetSafeHwnd());
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					
//					LONG CYourClass::OnHyperLinkActivated(UINT wParam, LONG lParam)
//					{
//						UNREFERENCED_PARAMETER(wParam);
//
//						COXHyperLinkAction* phla=(COXHyperLinkAction*)lParam;
//						if(phla->GetAction()!=ID_HLACTION_USERDEFINED || 
//							(phla->GetRecipientWnd()!=GetSafeHwnd() && 
//							phla->GetRecipientWnd()!=HWND_BROADCAST))
//						{
//							return (LONG)0;
//						}
//
//						AfxMessageBox(_T("HyperLink had been successfully activated!"));
//
//						return (LONG)1;
//					}
//
//
//		3) Open - use this type if you want to open the file specified by the 
//				COXHyperLinkAction::m_sFile parameter. The file can be 
//				an executable file, shortcut or document file. It can also be a folder.
//				Use this action if you want to open the website specified by m_sFile.
//				If the m_sFile parameter specifies an executable file then 
//				COXHyperLinkActionlpFile::m_sParameters specifies the parameters 
//				to be passed to the application. If m_sFile specifies a document file, 
//				m_sParameters should be NULL. Use this action if you want to send 
//				an e-mail: you can do that setting m_sFile to "mailto:address@you.need"

//
//				Below you will find an example of using COXStaticHyperLink as link to
//				some website:
//
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// open website
//					m_ctlStaticLink.SetAction(ID_HLACTION_OPEN,
//						_T("www.dundas.com//develop//utoolbx//"));
//					
//					// open file
//					m_ctlStaticLink.SetAction(ID_HLACTION_OPEN,
//						_T("sol.exe"));
//
//					// sending e-mail
//					m_ctlStaticLink.SetAction(ID_HLACTION_OPEN,
//						_T("mailto:andreiz@dundas.com"));
//
//		4) Print - use this type if you want to print the file specified by the 
//				COXHyperLinkAction::m_sFile parameter. The file should be 
//				a document file. If the file is an executable file, the action opens 
//				the file, as if "Open" action had been specified. 
//
//				Below you will find an example of using COXStaticHyperLink as link to
//				print some document:
//
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// print the document
//					m_ctlStaticLink.SetAction(ID_HLACTION_PRINT,
//						_T("readme.txt"));
//
//			
//		5) Explore - use this type if you want to explore the folder specified by the 
//				COXHyperLinkAction::m_sFile parameter. 
//
//				Below you will find an example of using COXStaticHyperLink as link to
//				explore some folder:
//
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// explore the folder
//					m_ctlStaticLink.SetAction(ID_HLACTION_EXPLORE,
//						_T("C:\\Windows\\"));
//
//		6) Email - use this type if you want to send a message to a recipient 
//				specified by the COXHyperLinkAction::m_sFile parameter. 
//				Defined only for convinience purposes, eventually calls Open action
//
//				Below you will find an example of using COXStaticHyperLink as link to
//				send an email message:
//
//					COXStaticHyperLink m_StaticLink;
//					. . . . . . . . . . . . 
//					. . . . . . . . . . . . 
//					// explore the folder
//					m_ctlStaticLink.SetAction(ID_HLACTION_EMAIL,
//						_T("andreiz@dundas.com"));
//
////////////////////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL COXStaticHyperLink : public COXStatic 
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXStaticHyperLink();


	// --- In  :	hla	-	Hyperlink action
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the COXHyperLinkAction object that will be executed 
	//				if user clicks on COXStaticHyperLink
	void SetAction(COXHyperLinkAction& hla);

	// --- In  :	refer to description of COXHyperLinkAction class
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the COXHyperLinkAction object that will be executed 
	//				if user clicks on COXStaticHyperLink
	void SetAction(int nActionID = ID_HLACTION_OPEN, LPCTSTR sFile = NULL, 
		LPCTSTR sParameters = NULL, LPCTSTR sDefaultDirectory = NULL, 
		int nShowCmd = SW_SHOWNORMAL, UINT nCallbackMsgID=NULL,	HWND hWndRecipient=NULL);
	

	// --- In  :	
	// --- Out : 
	// --- Returns:	the COXHyperLinkAction object that will be executed 
	//				if user clicks on COXStaticHyperLink
	// --- Effect : 
	inline COXHyperLinkAction GetAction() const { return m_hla; }


	// --- In  :	clr	-	color that COXStaticHyperLink uses to draw text 
	//						in unvisited mode;
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the color of COXStaticHyperLink object that 
	//				wasn't visited (like unvisited link in HTML)
	void SetUnvisitedColor(COLORREF clr, BOOL bRedraw=TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns:	the color of COXStaticHyperLink object that 
	//				wasn't visited (like unvisited link in HTML)
	// --- Effect : 
	inline COLORREF GetUnvisitedColor() const { return m_clrUnvisited; }


	// --- In  :	clr	-	color that COXStaticHyperLink uses to draw text 
	//						in visited mode;
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:
	// --- Effect : sets the color of COXStaticHyperLink object that 
	//				was visited (like visited link in HTML)
	void SetVisitedColor(COLORREF clr, BOOL bRedraw=TRUE);

	// --- In  :
	// --- Out : 
	// --- Returns:	the color of COXStaticHyperLink object that 
	//				was visited (like visited link in HTML)
	// --- Effect : 
	inline COLORREF GetVisitedColor() const { return m_clrVisited; }


	// --- In  :	bVisited -	TRUE - visited, FALSE - unvisited
	//				bRedraw - if TRUE then control will be redrawn 
	// --- Out : 
	// --- Returns:	
	// --- Effect : sets the state of control as visited or unvisited
	void SetVisited(BOOL bVisited, BOOL bRedraw=TRUE);

	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect : TRUE if COXStaticHyperLink object was visited,
	//				or FALSE otherwise
	inline BOOL GetVisited() const { return m_bVisited; }


	// --- In  :	bUseWindowText	-	if TRUE then if COXHyperLinkAction::m_sFile is 
	//									NULL COXStaticHyperLink will use text of 
	//									control's window as COXHyperLinkAction::m_sFile.
	//									By default this feature is set to TRUE.
	// --- Out : 
	// --- Returns:
	// --- Effect : 
	void SetUseWindowText(BOOL bUseWindowText);

	// --- In  :	bUseWindowText	-	if TRUE then if COXHyperLinkAction::m_sFile is 
	//									empty COXStaticHyperLink will use text of 
	//									control's window as COXHyperLinkAction::m_sFile.
	//									By default this feature is set to TRUE.
	// --- Out : 
	// --- Returns:	TRUE if COXStaticHyperLink uses text of control's window as 
	//				COXHyperLinkAction::m_sFile if it's empty, otherwise FALSE
	// --- Effect : 
	inline BOOL GetUseWindowText() const { return m_bUseWindowText; }


	// --- In  :	bErrorNotify	-	TRUE if you want to display a message box
	//									that notifies about errors during execution 
	//									of an action if any happens, or FALSE otherwise
	//									By default this feature is set to TRUE.
	// --- Out : 
	// --- Returns:
	// --- Effect : if TRUE then COXStaticHyperLink calls COXHyperLinkAction::ErrorNotify 
	//				function if an error occurs during execution of an action
	inline void SetErrorNotify(BOOL bErrorNotify) { m_bErrorNotify=bErrorNotify; }

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if COXStaticHyperLink calls COXHyperLinkAction::ErrorNotify 
	//				function if an error occurs during execution of an action, 
	//				or FALSE otherwise
	// --- Effect : 
	inline BOOL GetErrorNotify() const { return m_bErrorNotify; }


	// by default COXStaticHyperLink uses current dialog font with understrike feature
	// you can set whatever font you want to display the object,
	// but remember that you have to adjust the size of window to 
	// make sure text will be drawn correctly (this is not a problem 
	// if you call SetFitToText(TRUE))
	//
	// --- In  :
	// --- Out : 
	// --- Returns:	TRUE if default font was successfully set to the 
	//				COXStaticHyperLink object, or FALSE otherwise
	// --- Effect :	make COXStaticHyperLink object to use default font to draw text.
	//				Note that function declared as virtual, so you can put
	//				in any derivation of COXStaticHyperLink class your own definition
	//				of "default font". COXStaticHyperLink class uses the font which is 
	//				associated with COXStaticHyperLink window with underline 
	//				property set as default.
	virtual BOOL SetDefaultTextFont();


	// --- In  :	hCursor	-	handle of cursor
	// --- Out : 
	// --- Returns:	
	// --- Effect : sets the cursor that COXStaticHyperLink uses to visually show 
	//				that mouse is moving over COXStaticHyperLink control,
	//				if you don't want to differentiate visually hyperlink object  
	//				then call this function with hCursor set to NULL.
	//				COXStaticHyperLink uses cursor wich is defined in "OXBitmapButton.rc" 
	//				file (supplied with the Ultimate Toolbox) as default one. 
	//				So, you have to include reference to "OXBitmapButton.rc" resource 
	//				in the resource file of your application 
	inline void SetLinkCursor(HCURSOR hCursor=NULL) { m_hCursor=hCursor; }
	
	// --- In  :
	// --- Out : 
	// --- Returns:	
	// --- Effect : sets default cursor that COXStaticHyperLink uses to visually show 
	//				that mouse is moving over COXStaticHyperLink control.
	//				COXStaticHyperLink uses cursor wich is defined in "OXBitmapButton.rc" 
	//				file (supplied with the Ultimate Toolbox) as default one. 
	//				So, you have to include reference to "OXBitmapButton.rc" resource 
	//				in the resource file of your application 
	inline virtual void SetDefaultLinkCursor() { 
		m_hCursor=AfxGetApp()->LoadCursor(IDC_OX_HAND_CURSOR); }

	// --- In  :
	// --- Out : 
	// --- Returns:	the cursor that COXStaticHyperLink uses to visually show 
	//				that mouse is moving over COXStaticHyperLink control.
	// --- Effect : gets the cursor that we use to visually show that mouse is 
	//				moving over COXStaticHyperLink control
	inline HCURSOR GetLinkCursor() const { return m_hCursor; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXStaticHyperLink)
	//}}AFX_VIRTUAL

protected:
	DECLARE_DYNAMIC(COXStaticHyperLink)

	// color for unvisited
	COLORREF m_clrUnvisited;	
	// color for visited
	COLORREF m_clrVisited;		
	// whether visited or not
	BOOL m_bVisited;			

	// action to be executed when user click on the object
	COXHyperLinkAction m_hla;	

	// we use special cursor to show that mouse is moving over 
	// COXStaticHyperLink object
	HCURSOR m_hCursor;
	
	// use window text as file name
	BOOL m_bUseWindowText;
	// notify about errors using built-in function
	BOOL m_bErrorNotify;


protected:
	// initialize static control (create tooltip control. etc.)
	virtual BOOL InitStatic();
	// builds tooltip text
	virtual void FormatToolTipText();

	// Generated message map functions
	//{{AFX_MSG(COXStaticHyperLink)
	virtual afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	virtual afx_msg void OnClicked();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif //_STATICHYPERLINK_H
