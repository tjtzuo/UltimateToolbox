// ==========================================================================
// 					Class Specification : OXSoundCustomizeDlg
// ==========================================================================
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

// Description :
/*

OXSoundCustomizeDlg class is designed specifically to be used in conjunction with
COXSoundEffectOrganizer class and provides all the functionality to customize
any sound events registered with any window objects using COXSoundEffectOrganizer
object. We assume that you are familiar with the COXSoundEffectOrganizer class, 
if you are not then it's good idea to read the documentation on this class.

Briefly, using COXSoundEffectOrganizer object you can associate any sound event
(by sound event we mean playing sound while specific event has occured, e.g.
playing some sound when user clicks over button) with any window object. 
Different people could prefer to hear different sound or not to hear some of 
them at all. So it's good idea to provide some kind of customization functionality,
especially, taking into account the fact that there are such COXSoundEffectOrganizer
public functions as SaveState and LoadState which are designed specifically to 
save/load information registered about sound effects into/from the registry. 

Although you are free to provide your own solution we decided to come up with the 
standard dialog that can be easily used in any application and in most cases
doesn't require including of any code but the class' constructor. It is defined as
follows:

	COXSoundCustomizeDlg(CWnd* pParent, 
		COXSoundEffectOrganizer* pSoundEffectOrganizer,
		enum LEVELTYPE nStartLevel=LT_EVENT,
		DWORD dwDlgStyle=SCDS_NOTOGGLE);
	// --- In  :	pParent					-	pointer to the sound customize dialog 
	//											parent window. Can be NULL
	//				pSoundEffectOrganizer	-	pointer to the COXSoundEffectOrganizer
	//											object to be used as source of all 
	//											information about sound events to be 
	//											customized
	//				nStartLevel				-	Level to which the tree control used
	//											to display all sound events will be 
	//											expanded. We define three levels:
	//
	//						LT_WINDOW		-	display only titles of all windows 
	//											with which we associated any sounds.
	//						LT_EVENTLIST	-	display titles of windows and titles 
	//											of the lists of start playing and stop 
	//											playing events.
	//						LT_EVENT		-	display titles of windows, titles of
	//											lists of start playing and stop 
	//											playing events and all events.
	//				
	//				dwDlgStyle				-	specific sound customize dialog 
	//											styles. Can be any combination of next:
	//
	//						SCDS_READONLY	-	you can navigate through all stop and 
	//											start playing events but cannot change 
	//											them.
	//						SCDS_NOTOGGLE	-	you cannot expand or collapse any tree
	//											item
	//						SCDS_SAVESTATE	-	save in registry width of tree 
	//											columns, sort column and sort order
	//						SCDS_LOADSTATE	-	set saved in registry width of tree 
	//											columns, sort column and sort order
	//
	// --- Out :			
	// --- Returns:
	// --- Effect : Constructor


As you can see you have to specify only one parameter - pointer to the object of
COXSoundEffectOrganizer class. And optionally you can specify two additional 
parameters that have to do with dialog appearance.

It's worth mentioning that all sound effects are represented as multicolumn tree 
items. The structure of the tree schematically can be represented as follows:

		Type and text of window registered with any sound effect
			List of all playing events
				Event description	Name of sound source	Event ID	Loop
				Event description	Name of sound source	Event ID	Loop
				. . . . . . .

			List of all stop events
				Event description							Event ID	
				. . . . . . .

		Type and text of window registered with any sound effect
			. . . . . . . . .

		. . . . . . . . . . . 

Where	Event description		-	brief description of the event, e.g.: 
									"Left mouse button down"
		Name of sound source	-	sound source file name. Only sound effects
									that use files as sound sources are fully 
									editable. The sound effects that uses other 
									sound sources (e.g. resource ID) still will
									be displayed but you won't be able to change 
									the sound source.
		Event ID				-	sound event ID, e.g. WM_LBUTTONDOWN
		Loop					-	type of looping:
										once;
										indefinitely;
										specified number of times;


By default we populate array of all defined sound events. Next are defined:

	"Mouse enter"					HWM_MOUSEENTER
	"Mouse leave"					HWM_MOUSELEAVE
	"Left button down"				WM_LBUTTONDOWN
	"Left button up"				WM_LBUTTONUP
	"Left button double click"		WM_LBUTTONDBLCLK
	"Right button down"				WM_RBUTTONDOWN
	"Right button up"				WM_RBUTTONUP
	"Right button double click"		WM_RBUTTONDBLCLK
	"Middle button down"			WM_MBUTTONDOWN
	"Middle button up"				WM_MBUTTONUP
	"Middle button double click"	WM_MBUTTONDBLCLK


It's highly probable that you would define your own sound events that are not 
listed above. You can use next functions in order to edit array of defined
sound events:


	void SetEvent(UINT nEventID, LPCTSTR* ppszDescription);
	// --- In  :	nEventID		-	sound event identificator to set to the list
	//									which is displayed in "Event" combo box
	//				ppszDescription	-	pointer to array of two strings
	//									that provide description and additional 
	//									information about the specified event, e.g.
	BOOL RemoveEvent(UINT nEventID);

	void RemoveAllEvents();
	
	UINT GetEventCount();

	POSITION GetFirstEvent();
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the sound events in the list which is displayed in the 
	//				"Event" combo box; or NULL if there is no sound event.

	void GetNextEvent(POSITION& posEvent, UINT& nEventID, 
		LPCTSTR*& ppszDescription)
	// --- In  :	posEvent		-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextEvent or 
	//									GetFirstEvent call.
	// --- Out :	nEventID		-	referenced of the sound event ID found at
	//									posEvent position
	//				ppszDescription	-	reference of the sound event description 
	//									found at posEvent position. Event description
	//									defined as array of two strings	that provide 
	//									description and additional information about 
	//									the sound event


These are all public functions that are available in the class. 



COXSoundCustomizeDlg is an unusual example of the class that is heavily based on 
the existing Ultimate Toolbox classes. Next classes are used:

		COXTreeCtrl
		COXBitmapButton
		COXMultiComboBox
		COXHistoryCombo
		COXSound
		COXSoundEffectOrganizer



Probably the last thing that we should mention here is that any changes that 
could be made to the sound effects registered with the specified object of
COXSoundEffectOrganizer class will be applied immediately but won't be saved
automatically. It's your application's responsibility to provide all the code
to make all sound effects settings persistent. You can easily do that using
COXSoundEffectOrganizer public functions: SaveState(...) and LoadState(...)

*/




#if !defined(_OXSOUNDCUSTOMIZEDLG_H)
#define _OXSOUNDCUSTOMIZEDLG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"
#include "OXMainRes.h"


#include "OXMultiComboBox.h"
#include "OXHistoryCombo.h"
#include "OXBitmapButton.h"
#include "OXTreeCtrl.h"
#include "OXSoundEffectManager.h"

typedef struct sOXSoundTips{
	int nID;
	LPCTSTR lpName;
}tOXSoundTips;


// Sound Customize Dialog specific styles
//
#define SCDS_READONLY		0x00000001
#define SCDS_NOTOGGLE		0x00000002
#define SCDS_SAVESTATE		0x00000004
#define SCDS_LOADSTATE		0x00000008
//
// Next styles can be specified while creating COXSoundCustomizeDlg object:
//
//			SCDS_READONLY	-	you can navigate through all stop and 
//								start playing events but cannot change them.
//			SCDS_NOTOGGLE	-	you cannot expand or collapse any tree item
//			SCDS_SAVESTATE	-	save in registry width of tree 
//								columns, sort column and sort order
//			SCDS_LOADSTATE	-	set saved in registry width of tree 
//								columns, sort column and sort order
///////////////////////////////////////////////////////////////////////////

// number of the columns in the multi combobox used to provide description
// of a sound event (message ID + brief explanation)
#define ID_EVENTINFOCOLUMNS	2


// internal identificators of controls in the dialog used to disable/enable
// them in the most effective way
//
#define CTL_EVENT			0x00000001
#define CTL_FILENAME		0x00000002
#define CTL_PLAY			0x00000004
#define CTL_STOP			0x00000008
#define CTL_LOOPTYPE		0x00000010
#define CTL_FORCETOSTART	0x00000020
#define CTL_ADD				0x00000040
#define CTL_REMOVE			0x00000080
#define CTL_SAVE			0x00000100
#define CTL_RESTORE			0x00000200
#define CTL_OK				0x00000400
#define CTL_CANCEL			0x00000800
#define CTL_TREE			0x00001000
//
// Next table shows the relationships between the above defined flags 
// and controls in the dialog:
//
//		CTL_EVENT			"Event" multi combobox with information about all 
//							defined events
//		CTL_FILENAME		"Name of sound file" history combobox with name of 
//							sound files	used as sound sources
//		CTL_PLAY			"Start" bitmap button; press on it in order 
//							to test	the chosen sound source  
//		CTL_STOP			"Stop" bitmap button; press on it in order 
//							to stop playing the	tested sound 
//		CTL_LOOPTYPE		group of three radio buttons which represents
//							all possible looping options:
//							-	play once
//							-	play infinitely
//							-	play specified number of times
//		CTL_FORCETOSTART	check box; if it is marked then the sound effect 
//							will be played even though at the moment another
//							sound effect could be playing
//		CTL_ADD				"Add" button; press on it in order to add new 
//							start or stop playing event depending on which tree 
//							item was selected when the button was pressed 
//		CTL_REMOVE			remove start or/and stop playing event depending on 
//							which tree item was selected when the button was pressed 
//		CTL_SAVE			"Save" button; press on it in order to save changes made 
//							to currently edited start or stop playing sound event
//		CTL_RESTORE			"Restore" button; press on it in order to skip any 
//							changes made to currently edited start or stop playing 
//							sound event
//		CTL_OK				"OK" button; press on it in order to apply all the 
//							changes made to sound events to the specified at
//							creation time COXSoundEffectOrganizer object and
//							close the dialog
//		CTL_CANCEL			"Cancel" button; press on it in order to close the
//							dialog without applying any changes made to sound 
//							events
//		CTL_TREE			multicolumn tree control; populated at the creation 
//							time used given COXSoundEffectOrganizer object. It is
//							populated with all window objects that are registered 
//							with any sound event and all start and stop playing 
//							events that are placed in separated lists. You can 
//							navigate through the tree and edit any of its items.
//
//////////////////////////////////////////////////////////////////////////////


// Level to which the tree control used	to display all sound events can be 
// expanded. 
//
enum LEVELTYPE
{
	LT_WINDOW=0,
	LT_EVENTLIST=1,
	LT_EVENT=2,
};
//
// We define three levels:
//
//		LT_WINDOW		-	display only titles of all windows 
//							with which we associated any sounds.
//		LT_EVENTLIST	-	display titles of windows and titles 
//							of the lists of start playing and stop 
//							playing events.
//		LT_EVENT		-	display titles of windows, titles of
//							lists of start playing and stop 
//							playing events and all events.


// Type of item in the tree control used to display all sound events
//
enum ITEMTYPE
{
	IT_WINDOW=0,
	IT_PLAYEVENTLIST=1,
	IT_STOPEVENTLIST=2,
	IT_PLAYEVENT=3,
	IT_STOPEVENT=4,
	IT_UNKNOWN=5,
};
//
// Next types are defined:
//
//		IT_WINDOW		-	type and caption of the window which is 
//							registered with any sound event
//		IT_PLAYEVENTLIST-	the parent item of all start playing events for
//							particular window
//		IT_STOPEVENTLIST-	the parent item of all stop playing events for
//							particular window
//		IT_PLAYEVENT	-	description of the sound event: Event, Sound File, 
//							Event Info, Loop
//		IT_STOPEVENT	-	description of the stop playing event: Event,
//							Event Info
//		IT_UNKNOWN		-	type is not specified
//
// Schematically different items on the different levels in the sound events 
// tree can be represented as follows:
//
//		Items							Levels
//
//		IT_WINDOW					-->	LT_WINDOW
//			IT_PLAYEVENTLIST		-->	LT_EVENTLIST
//				IT_PLAYEVENT
//				IT_PLAYEVENT		-->	LT_EVENT
//				. . . . . . .
//
//			IT_STOPEVENTLIST		-->	LT_EVENTLIST
//				IT_STOPEVENT
//				IT_STOPEVENT		-->	LT_EVENT
//				. . . . . . .
//
//		IT_WINDOW					-->	LT_WINDOW
//			. . . . . . . . .
//
//		. . . . . . . . . . . 
//////////////////////////////////////////////////////////////////////////////


// Foolowing structure was designed specifically in order to provide full 
// description of any item in the sound events tree. This structure is used 
// internally.
//
typedef struct _tagITEMINFO
{
	enum ITEMTYPE type;
	CWnd* pWnd;
	UINT nMsgID;
	LPSOUNDSOURCEINFO pSoundSourceInfo;
	LPSOUNDEFFECT pSoundEffect;
	DWORD dwControlState;

	// constructor
	_tagITEMINFO()
	{
		type=IT_UNKNOWN;
		pWnd=NULL;
		nMsgID=SNDEVENT_NONE;
		pSoundSourceInfo=NULL;
		pSoundEffect=NULL;
		dwControlState=0;
	}

	// destructor
	~_tagITEMINFO()
	{
		delete pSoundSourceInfo;
		delete pSoundEffect;
	}

    // assignment operator
    _tagITEMINFO& operator=( const _tagITEMINFO& ii )
	{
		if(this==&ii)
			return *this;

		delete pSoundSourceInfo;
		pSoundSourceInfo=new SOUNDSOURCEINFO;
		delete pSoundEffect;
		pSoundEffect=new SOUNDEFFECT;

		type=ii.type;
		pWnd=ii.pWnd;
		nMsgID=ii.nMsgID;
		if(ii.pSoundSourceInfo!=NULL)
			*pSoundSourceInfo=*ii.pSoundSourceInfo;
		if(ii.pSoundEffect!=NULL)
			*pSoundEffect=*ii.pSoundEffect;
		dwControlState=ii.dwControlState;
		return *this;
	}

} ITEMINFO, * LPITEMINFO;
//
// Below you will find description of all structure's elements:
//
//	type			-	type of item. Refer to description of enum ITEMTYPE
//						for more details
//	pWnd			-	pointer to the window object to which the item is refered
//	nMsgID			-	start/stop playing event identificator
//	pSoundSourceInfo-	pointer to SOUNDSOURCEINFO object. Refer to description 
//						of SOUNDSOURCEINFO structure in the OXSound.h (header
//						file for COXSound class). This structure is used internally
//						by COXSound class and provides information about the
//						nature of sound source, i.e. name of file or resource ID
//	pSoundEffect	-	pointer to SOUNDEFFECT object. Refer to the description of 
//						SOUNDEFFECT structure to the documentation on 
//						COXSoundEffectManager and COXSoundEffectOrganizer classes.
//	dwControlState	-	combination of flags that identifies controls that have 
//						to be enabled if the item is selected. Next flags can be
//						combined:
//
//						CTL_EVENT
//						CTL_FILENAME
//						CTL_PLAY
//						CTL_STOP
//						CTL_LOOPTYPE
//						CTL_FORCETOSTART
//						CTL_ADD
//						CTL_REMOVE
//						CTL_SAVE
//						CTL_RESTORE
//						CTL_OK
//						CTL_CANCEL
//						CTL_TREE
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// COXSoundCustomizeDlg dialog

class OX_CLASS_DECL COXSoundCustomizeDlg : public CDialog
{
// Construction
public:
	COXSoundCustomizeDlg(CWnd* pParent, 
		COXSoundEffectOrganizer* pSoundEffectOrganizer,
		enum LEVELTYPE nStartLevel=LT_EVENT,
		DWORD dwDlgStyle=SCDS_NOTOGGLE);
	// --- In  :	pParent					-	pointer to the sound customize dialog 
	//											parent window. Can be NULL
	//				pSoundEffectOrganizer	-	pointer to the COXSoundEffectOrganizer
	//											object to be used as source of all 
	//											information about sound events to be 
	//											customized
	//				nStartLevel				-	Level to which the tree control used
	//											to display all sound events will be 
	//											expanded. We define three levels:
	//
	//						LT_WINDOW		-	display only titles of all windows 
	//											with which we associated any sounds.
	//						LT_EVENTLIST	-	display titles of windows and titles 
	//											of the lists of start playing and stop 
	//											playing events.
	//						LT_EVENT		-	display titles of windows, titles of
	//											lists of start playing and stop 
	//											playing events and all events.
	//				
	//				dwDlgStyle				-	specific sound customize dialog 
	//											styles. Can be any combination of next:
	//
	//						SCDS_READONLY	-	you can navigate through all stop and 
	//											start playing events but cannot change 
	//											them.
	//						SCDS_NOTOGGLE	-	you cannot expand or collapse any tree
	//											item
	//						SCDS_SAVESTATE	-	save in registry width of tree 
	//											columns, sort column and sort order
	//						SCDS_LOADSTATE	-	set saved in registry width of tree 
	//											columns, sort column and sort order
	//
	// --- Out :			
	// --- Returns:
	// --- Effect : Constructor

protected:
// Dialog Data
	//{{AFX_DATA(COXSoundCustomizeDlg)
	enum { IDD = IDD_OX_DIALOG_CUSTOMIZE_OXSOUND };
	CButton	m_btnRestore;
	CEdit	m_editLoopCount;
	CButton	m_btnForceToStart;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	COXTreeCtrl	m_treeEffect;
	COXHistoryCombo	m_cmbFileName;
	COXMultiComboBox	m_cmbEvent;
	COXBitmapButton	m_btnStop;
	CButton	m_btnSave;
	CButton	m_btnRemove;
	COXBitmapButton	m_btnPlay;
	CButton	m_btnAdd;
	int		m_nEvent;
	CString	m_sFileName;
	BOOL	m_bForceToStart;
	int		m_nLoopCount;
	int		m_nLoopType;
	//}}AFX_DATA

protected:
	// pointer to COXSoundEffectOrganizer object which is specified at the
	// creation time and used to populate the tree control
	COXSoundEffectOrganizer* m_pSoundEffectOrganizer;

	// image list associated with the tree control
	CImageList m_ilTree;

	// map of all defined sound events
	CMap<UINT,UINT,LPCTSTR*,LPCTSTR*> m_allDefinedEvents;
	// map of items and associated ITEMINFO structures
	CMap<HANDLE,HANDLE,LPITEMINFO,LPITEMINFO> m_allTreeItems;

	// flag specifies that one of the event is currently editing
	BOOL m_bEditing;
	// flag specifies that the new event is currently editing
	BOOL m_bAddingNew;

	// the sound object that is used to test chosen sound source
	COXSound m_TestSound;

	// current state (enable/disable) of all dialog's controls
	DWORD m_dwControlState;

	// COXSoundCustomizeDlg specific styles
	DWORD m_dwDlgStyle;

	// the level to which the tree control should be expanded at the beginning
	enum LEVELTYPE m_nStartLevel;

public: 
	inline void SetEvent(UINT nEventID, LPCTSTR* ppszDescription) {
		ASSERT(nEventID!=SNDEVENT_NONE);
		ASSERT(ppszDescription!=NULL);
		m_allDefinedEvents.SetAt(nEventID,ppszDescription); 
	}
	// --- In  :	nEventID		-	sound event identificator to set to the list
	//									which is displayed in "Event" combo box
	//				ppszDescription	-	pointer to array of two strings
	//									that provide description and additional 
	//									information about the specified event, e.g.
	//							
	//		LPTSTR sEvent[]= { _T("Mouse left button down"), _T("WM_LBUTTONDOWN") };
	//		customizeDlg.SetEvent(WM_LBUTTONDOWN,(LPCTSTR*)sEvent);
	//
	// --- Out : 
	// --- Returns: TRUE if sound was successfully set, or FALSE otherwise
	// --- Effect : Set sound event to be displayed in the "Event" combo box. Next 
	//				are defined by default (message IDs):
	//				
	//					HWM_MOUSEENTER
	//					HWM_MOUSELEAVE
	//					WM_LBUTTONDOWN
	//					WM_LBUTTONUP
	//					WM_LBUTTONDBLCLK
	//					WM_RBUTTONDOWN
	//					WM_RBUTTONUP
	//					WM_RBUTTONDBLCLK
	//					WM_MBUTTONDOWN
	//					WM_MBUTTONUP
	//					WM_MBUTTONDBLCLK
	//
	//				Probably you would like to edit additional sound events or change 
	//				the description of already set ones. Then you should call this
	//				function.

	inline BOOL RemoveEvent(UINT nEventID) {
		ASSERT(nEventID!=SNDEVENT_NONE);
		return m_allDefinedEvents.RemoveKey(nEventID); 
	}
	// --- In  :	nEventID		-	sound event identificator to remove from the 
	//									list which is displayed in "Event" combo box
	// --- Out : 
	// --- Returns: TRUE if sound was successfully removed, or FALSE otherwise
	// --- Effect : Remove sound event from the list which is displayed in the 
	//				"Event" combo box. Refer to description of SetEvent function
	//				for list of all predefined events.

	inline void RemoveAllEvents() { 
		m_allDefinedEvents.RemoveAll(); 
	}
	// --- In  :	
	// --- Out : 
	// --- Returns: 
	// --- Effect : Remove all sound events from the list which is displayed in the 
	//				"Event" combo box.

	
	inline UINT GetEventCount() { 
		return (UINT) m_allDefinedEvents.GetCount(); 
	}
	// --- In  :	
	// --- Out : 
	// --- Returns: the number of sound events in the list which is displayed in the 
	//				"Event" combo box.
	// --- Effect : 


	inline POSITION GetFirstEvent() { 
		return m_allDefinedEvents.GetStartPosition(); 
	}
	// --- In  :	
	// --- Out : 
	// --- Returns: A POSITION value that indicates a starting position for iterating 
	//				the sound events in the list which is displayed in the 
	//				"Event" combo box; or NULL if there is no sound event.
	// --- Effect : Starts a sound events iteration by returning a POSITION value that 
	//				can be passed to a GetNextEvent call. The iteration sequence is 
	//				not predictable; therefore, the "first sound event" has no special 
	//				significance.

	inline void GetNextEvent(POSITION& posEvent, UINT& nEventID, 
		LPCTSTR*& ppszDescription) {
		ASSERT(posEvent!=NULL);
		m_allDefinedEvents.GetNextAssoc(posEvent,nEventID,ppszDescription);
	}
	// --- In  :	posEvent		-	Specifies a reference to a POSITION value 
	//									returned by a previous GetNextEvent or 
	//									GetFirstEvent call.
	// --- Out :	nEventID		-	referenced of the sound event ID found at
	//									posEvent position
	//				ppszDescription	-	reference of the sound event description 
	//									found at posEvent position. Event description
	//									defined as array of two strings	that provide 
	//									description and additional information about 
	//									the sound event
	// --- Returns: 
	// --- Effect : Retrieves the message ID and description of a sound event at 
	//				rNextPosition, then updates rNextPosition to refer to the next 
	//				sound event. This function is most useful for iterating through 
	//				all the sound events in the list which is displayed in the 
	//				"Event" combo box. If the retrieved element is the last sound 
	//				event, then the new value of rNextPosition is set to NULL.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXSoundCustomizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COXSoundCustomizeDlg();

protected:

	// populate dialog's control depending on currently selected tree item
	void GetVars();
	// helper function used to initialize COXBitmapButton objects used in the
	// dialog ("Start" and "Stop" buttons)
	void SetupBitmapButton(COXBitmapButton* m_pctl); 
	// helper function to retrieve a substring from the string that is divided
	// using specified character
	CString GetSubString(LPCTSTR pszFullString, int nSubIndex, TCHAR cDelimiter);
	// helper function used to add new start or stop playing event item to the 
	// tree control
	HTREEITEM AddEventItem(CWnd* pWnd, HTREEITEM htiParent, UINT nMsgID, 
		COXSoundEffectManager* pSoundEffectManager, BOOL bPlayEvent);
	// enable/disable controls in the most effective way
	void ShowControls(DWORD dwControlState);
	// helper function to start edit items
	void StartEditItem(BOOL bAddingNew=FALSE);

	// set of verification functions used to check the data that was changed
	//
	BOOL VerifySoundFile(CString sFileName, BOOL bReportError);
	BOOL VerifyEvent(CString sEvent, CWnd* pWnd, BOOL bReportError);
	BOOL VerifyLoopType(int nLoopType, int nLoopCount, BOOL bReportError);
	BOOL VerifyForceToStart(BOOL bForceToStart,	BOOL bReportError);
	////////////////////////////////////////////////////////////////////////////

	// saves state of the tree control (the width of columns, the number of the 
	// sort column and sort order into registry)
	void SaveState();

	// Generated message map functions
	//{{AFX_MSG(COXSoundCustomizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonPlay();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonStop();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnItemchangedExttreeEffectslist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRestore();
	afx_msg void OnCheckForce();
	afx_msg void OnEditchangeComboEvent();
	afx_msg void OnSelchangeComboEvent();
	afx_msg void OnEditchangeComboFilename();
	afx_msg void OnSelchangeComboFilename();
	afx_msg void OnChangeEditLoopcount();
	afx_msg void OnRadioLoopinfinitely();
	afx_msg void OnRadioLooponce();
	afx_msg void OnRadioLooprepeat();
	afx_msg void OnItemExpandingExttreeEffectslist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlaybackComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateVars(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXSOUNDCUSTOMIZEDLG_H)
