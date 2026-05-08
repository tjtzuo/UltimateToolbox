// ==========================================================================
// 					Class Specification : COXDateTimeCtrl
// ==========================================================================

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
//	YES	Derived from CWnd

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Desciption :         

/*

A COXDateTimeCtrl object encapsulates the functionality of a date and time picker control. 
The date and time picker control (DTP control) provides a simple interface to exchange 
date and time information with a user. This interface contains fields, each of which 
displays a part of the date and time information stored in the control. The user can 
change the information stored in the control by changing the content of the string in a 
given field. The user can move from field to field using the mouse or the keyboard.

You can customize the date and time picker control by applying a variety of styles to the 
object when you create it. The following window styles are specific to date and time 
picker (DTP) controls: 

		DTS_APPCANPARSE			-	Allows the owner to parse user input and take 
									necessary action. It enables users to edit within 
									the client area of the control when they press the 
									F2 key. The control sends DTN_USERSTRING notification 
									messages when users are finished. 
		DTS_LONGDATEFORMAT		-	Displays the date in long format. The default format 
									string for this style is defined by 
									LOCALE_SLONGDATEFORMAT, which produces output like 
									"Friday, April 19, 1996". 
		DTS_RIGHTALIGN			-	The drop-down month calendar will be right-aligned 
									with the control instead of left-aligned, which is 
									the default.  
		DTS_SHOWNONE			-	It is possible to have no date currently selected in 
									the control. With this style, the control displays a 
									check box that users can check once they have entered 
									or selected a date. Until this check box is checked, 
									the application will not be able to retrieve the date 
									from the control because, in essence, the control has 
									no date. This state can be set with the SetTime 
									function or verified with the GetTime function. 
		DTS_SHORTDATEFORMAT		-	Displays the date in short format. The default format 
									string for this style is defined by LOCALE_SSHORTDATE, 
									which produces output like "4/19/96". 
		DTS_TIMEFORMAT			-	Displays the time. The default format string for this 
									style is defined by LOCALE_STIMEFORMAT, which produces 
									output like "5:31:42 PM". 
		DTS_UPDOWN				-	Appears to the right of the DTP control to modify date 
									values. This style can be used instead of the drop-down 
									month calendar, which is the default style. 


You can set the display format of the DTP control using format styles. 
Date and time picker (DTP) controls support the following format characters. 
	"d"			The one- or two-digit day. 
	"dd"		The two-digit day. Single-digit day values are preceded by a zero. 
	"ddd"		The three-character weekday abbreviation. 
	"dddd"		The full weekday name. 
	"h"			The one- or two-digit hour in 12-hour format. 
	"hh"		The two-digit hour in 12-hour format. Single-digit values are preceded 
				by a zero. 
	"H"			The one- or two-digit hour in 24-hour format. 
	"HH"		The two-digit hour in 24-hour format. Single-digit values are preceded 
				by a zero. 
	"m"			The one- or two-digit minute. 
	"mm"		The two-digit minute. Single-digit values are preceded by a zero. 
	"M"			The one- or two-digit month number. 
	"MM"		The two-digit month number. Single-digit values are preceded by a zero. 
	"MMM"		The three-character month abbreviation. 
	"MMMM"		The full month name. 
	"t"			The one-letter AM/PM abbreviation (that is, AM is displayed as "A"). 
	"tt"		The two-letter AM/PM abbreviation (that is, AM is displayed as "AM"). 
	"X"			The callback field. The control still uses the other valid format 
				characters and queries the owner to fill in the "X" portion. So the owner 
				must be prepared to handle the DTN_WMKEYDOWN, DTN_FORMAT, and 
				DTN_FORMATQUERY notification messages. Multiple "X" characters can be used 
				in series to signify unique callback fields. 
	"y"			The one-digit year (that is, 1996 would be displayed as "6"). 
	"yy"		The last two digits of the year (that is, 1996 would be displayed as "96"). 
	"yyy"		The full year (that is, 1996 would be displayed as "1996"). 


	Callback fields
	In addition to the standard format characters that define date and time picker (DTP) 
	fields, you can customize your output by specifying certain parts of a format string 
	as callback fields. To declare a callback field, include one or more "X" characters 
	(ASCII Code 88) anywhere in the body of the format string. Like other DTP control 
	fields, callback fields are displayed in left-to-right order based on their location 
	in the format string. You can create callback fields that are unique to each other 
	by repeating the "X" character. Thus, the format string "XXddddMMMdd', 'yyyXXX" 
	contains two unique callback fields, "XX" and "XXX". Remember, callback fields are 
	treated as valid fields, so your application must be prepared to handle DTN_WMKEYDOWN 
	notification messages. When the DTP control parses the format string and encounters 
	a callback field, it sends DTN_FORMAT and DTN_FORMATQUERY notification messages. 
	The callback field string is included with the notifications so the receiving 
	application can determine which callback field is being queried. The owner of the 
	control must respond to these notifications to ensure that the custom information 
	is properly displayed. 




//////////////////////////
Using COXDateTimeCtrl

The date and time picker control (COXDateTimeCtrl) implements an intuitive and recognizable 
method of entering or selecting a specific date. The main interface of the control is 
similar in functionality to a combo box. However, if the user expands the control, 
a month calendar control appears (by default), allowing the user to specify a particular 
date. When a date is chosen, the month calendar control automatically disappears.

!!!
Note	To use both the CDateTimePicker and COXMonthCalCtrl classes in your project, 
		you must include OXDateTimeCtrl.h
!!!




//////////////////////////
Creating the Date and Time Picker Control

How the date and time picker control is created depends on whether you are using the 
control in a dialog box or creating it in a nondialog window.

To use COXDateTimeCtrl directly in a dialog box 

Visual C++ v6.0:
In the dialog editor, add a Date and Time Picker Control to your dialog template resource. 
Specify its control ID. Specify any styles required, using the Properties dialog box of the 
date and time picker control. Use ClassWizard to map handler functions in the dialog class 
for any date time picker control notification messages you need to handle. In OnInitDialog, 
set any additional styles for the COXDateTimeCtrl object. 

Visual C++ v5.0:
In the dialog editor, add a Static Control to your dialog template resource (it will be
a placeholder for the DTP control). Specify its control ID. In OnInitDialog, you can use 
next code to replace Static Control on our DTP control:
  
	CWnd *pFrame=GetDlgItem(IDC_DATETIMEPICKER);
	CRect rcWnd;
	pFrame->GetWindowRect(rcWnd);
	pFrame->DestroyWindow(); //pFrame was just a placeholder anyway.
	ScreenToClient(rcWnd);
	m_ctlDateTimePicker.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP,rcWnd,this,
		IDC_DATETIMEPICKER);


  
To use COXDateTimeCtrl in a nondialog window 

Declare the control in the view or window class. Call the control's Create member function, 
possibly in OnInitialUpdate, possibly as early as the parent window's OnCreate handler 
function (if you're subclassing the control). Set the styles for the control. 




//////////////////////////
Accessing the Embedded Month Calendar Control

The embedded month calendar control object can be accessed from the COXDateTimeCtrl object 
with a call to the GetMonthCalCtrl member function.

!!!
Note	The embedded month calendar control is used only when the date and time picker 
		control does not have the DTS_UPDOWN style set.
!!!

This is useful if you want to modify certain attributes before the embedded control is 
displayed. To accomplish this, handle the DTN_DROPDOWN notification, retrieve the month 
calendar control (using COXDateTimeCtrl::GetMonthCalCtrl), and make your modifications. 
Unfortunately, the month calendar control is not persistent.

In other words, when the user requests the display of the month calendar control, a new 
month calendar control is created (before the DTN_DROPDOWN notification). The control is 
destroyed (after the DTN_CLOSEUP notification) when dismissed by the user. This means that 
any attributes you modify, before the embedded control is displayed, are lost when the 
embedded control is dismissed.

The following example demonstrates this procedure, using a handler for the DTN_DROPDOWN 
notification. The code changes the background color of the month calendar control, 
with a call to SetMonthCalColor, to gray. The code is as follows:

void CMyDlg::OnDropdown(NMHDR* pNMHDR, LRESULT* pResult)
{

     //set the background color of the month to gray
     COLORREF clr= RGB(100, 100, 100);
     SetMonthCalColor(MCSC_MONTHBK, clr);

     *pResult = 0;
}

As stated previously, all modifications to properties of the month calendar control are 
lost, with two exceptions, when the embedded control is dismissed. The first exception, 
the colors of the month calendar control, has already been discussed. The second exception 
is the font used by the month calendar control. You can modify the default font by making 
a call to COXDateTimeCtrl::SetMonthCalFont, passing the handle of an existing font. 
The following example (where m_dtPicker is the date and time control object) demonstrates 
one possible method:

COXMonthCalCtrl* pMCCtrl= NULL;

pMCCtrl= m_dtPicker.GetMonthCalCtrl();

//create and initialize the font to be used
LOGFONT logFont;

logFont.lfHeight = -12;
logFont.lfWidth = 0;
logFont.lfWeight = FW_NORMAL;
logFont.lfItalic = FALSE;
logFont.lfUnderline = FALSE;
logFont.lfStrikeOut = FALSE;
logFont.lfEscapement = 0;
logFont.lfOrientation = 0;
lstrcpy(logFont.lfFaceName, _T("Verdana"));

m_MCFont.CreateFontIndirect(&logFont);
m_dtPicker.SetMonthCalFont((HFONT)m_MCFont);

Once the font has been changed, with a call to COXDateTimeCtrl::SetMonthCalFont, the new font 
is stored and used the next time a month calendar is to be displayed.




//////////////////////////
Using Callback Fields in a Date and Time Picker Control

In addition to the standard format characters that define date and time picker fields, 
you can customize your output by specifying certain parts of a custom format string as 
callback fields. To declare a callback field, include one or more "X" characters 
(ASCII Code 88) anywhere in the body of the format string. For example, the following 
string "'Today is: 'yy'/'MM'/'dd' (Day 'X')'"causes the date and time picker control 
to display the current value as the year followed by the month, date, and finally the 
day of the year.

!!!
Note	The number of X's in a callback field does not correspond to the number of 
		characters that will be displayed.
!!!

You can distinguish between multiple callback fields in a custom string by repeating 
the "X" character. Thus, the format string "XXddddMMMdd', 'yyyXXX" contains two unique 
callback fields, "XX" and "XXX".

!!!
Note	Callback fields are treated as valid fields, so your application must be 
		prepared to handle DTN_WMKEYDOWN notification messages.
!!!

Implementing callback fields in your date and time picker control consists of three parts: 

	1)	Initializing the custom format string
		Initialize the custom string with a call to COXDateTimeCtrl::SetFormat. A common 
		place to set the custom format string is in the OnInitDialog function of your 
		containing dialog class or OnInitalUpdate function of your containing view class.

	2)	Handling the DTN_FORMATQUERY notification
		When the control parses the format string and encounters a callback field, the 
		application sends DTN_FORMAT and DTN_FORMATQUERY notification messages. The 
		callback field string is included with the notifications so you can determine 
		which callback field is being queried.
		The DTN_FORMATQUERY notification is sent to retrieve the maximum allowable size 
		in pixels of the string that will be displayed in the current callback field.
		To properly calculate this value, you must calculate the height and width of 
		the string, to be substituted for the field, using the control's display font. 
		The actual calculation of the string is easily achieved with a call to the 
		::GetTextExtentPoint32 Win32 function. Once the size is determined, pass the 
		value back to the application and exit the handler function.

		The following example is one method of supplying the size of the callback string:

		NMDATETIMEFORMATQUERY* pDTFormatQuery= (NMDATETIMEFORMATQUERY *)pNMHDR;
		CDC* pDC;
		CFont* pFont;
		CFont* pOrigFont;

		//  Prepare the device context for the GetTextExtentPoint32 call.
		pDC= GetDC();

		pFont= GetFont();
		if(!pFont)
			pFont->CreateStockObject(DEFAULT_GUI_FONT);

		pOrigFont = pDC->SelectObject(pFont);

		// Check to see if this is the callback segment desired. If so,
		// use the longest text segment to determine the maximum 
*		// width of the callback field, and then place the information into 
		// the NMDATETIMEFORMATQUERY structure.
		if(!lstrcmp("X",pDTFormatQuery->pszFormat))
			::GetTextExtentPoint32 (pDC->m_hDC, "366", 3, &pDTFormatQuery->szMax);

		// Reset the font in the device context then release the context.
		pDC->SelectObject(pOrigFont);
		ReleaseDC(pDC);

		*pResult = 0;


		Once the size of the current callback field has been calculated, you must supply 
		a value for the field. This is done in the handler for the DTN_FORMAT notification.

	3)	Handling the DTN_FORMAT notification 
		The DTN_FORMAT notification is used by the application to request the character 
		string that will be substituted. The following example demonstrates one possible 
		method:

		NMDATETIMEFORMAT* pDTFormat= (NMDATETIMEFORMAT *)pNMHDR;

		COleDateTime oCurTime;
		m_dtPicker.GetTime(oCurTime);

		wsprintf(pDTFormat->pszDisplay,"%d",oCurTime.GetDayOfYear());

		*pResult = 0;

		Note	The pointer to the NMDATETIMEFORMAT structure is found by casting the first 
				parameter of the notification handler to the proper type.





//////////////////////////
Using Custom Format Strings in a Date and Time Picker Control

By default, date and time picker controls provide three format types (each format 
corresponding to a unique style) for displaying the current date or time: 

	DTS_LONGDATEFORMAT		-	Displays the date in long format, producing output 
								like "Wednesday, January 3, 1998".
	DTS_SHORTDATEFORMAT		-	Displays the date in short format, producing output 
								like "1/3/98".
	DTS_TIMEFORMAT			-	Displays the time in long format, producing output 
								like "5:31:42 PM". 

However, you can customize the appearance of the date or time by using a custom format 
string. This custom string is comprised of either existing format characters, nonformat 
characters, or a combination of both. Once the custom string is built, make a call to 
COXDateTimeCtrl::SetFormat passing in your custom string. The date and time picker control 
will then display the current value using your custom format string.

The following example code (where m_dtPicker is the COXDateTimeCtrl object) demonstrates 
one possible solution:

	CString formatStr= _T("'Today is: 'yy'/'MM'/'dd");
	m_dtPicker.SetFormat(formatStr);

In addition to custom format strings, date and time picker controls also support callback 
fields.




//////////////////////////
Processing Notification Messages in Date and Time Picker Controls

As users interact with the date and time picker control, the control (COXDateTimeCtrl) 
sends notification messages to its parent window, usually a view or dialog object. 
Handle these messages if you want to do something in response. For example, when the 
user opens the date and time picker to display the embedded month calendar control, 
the DTN_DROPDOWN notification is sent.

The following list describes the various notifications sent by the date and time picker 
control. 

	DTN_DROPDOWN		-	Notifies the parent that the embedded month calendar control 
							is about to be displayed. This notification is only sent 
							when the DTS_UPDOWN style has not been set. For more 
							information on this notification, see Accessing the 
							Embedded Month Calendar Control.

	DTN_CLOSEUP			-	Notifies the parent that the embedded month calendar control 
							is about to be closed. This notification is only sent when 
							the DTS_UPDOWN style has not been set.

	DTN_DATETIMECHANGE	-	Notifies the parent that a change has occurred in the control.

							lpChange = (LPNMDATETIMECHANGE) lParam;
							The owner of the control must return zero. 
							
							lpChange	-	Address of an NMDATETIMECHANGE structure 
											containing information about the change that 
											took place in the control. 

							NMDATETIMECHANGE

							typedef struct tagNMDATETIMECHANGE {
								NMHDR       nmhdr;
								DWORD       dwFlags;
								SYSTEMTIME  st;
							} NMDATETIMECHANGE, FAR * LPNMDATETIMECHANGE;
							Contains information about a change that has taken place 
							in a date and time picker (DTP) control. This structure is 
							used with the DTN_DATETIMECHANGE notification message. 

							nmhdr	-	NMHDR structure that contains information about 
										the notification message. 
							dwFlags	-	Value that indicates if the control was set to 
										"no date" status (for DTS_SHOWNONE only). This 
										flag also specifies whether the contents of the 
										st member are valid and contain current time 
										information. This value can be one of the 
										following: 
										
										GDT_NONE	-	The control is set to "no date" 
														status. The "no date" status 
														applies only to controls that are 
														set to the DTS_SHOWNONE style. 
										GDT_VALID	-	The control is not set to the 
														"no date" status. The st member 
														contains the current date and time. 

							st		-	SYSTEMTIME structure that contains information 
										about the current system date and time.

	DTN_FORMAT			-	Notifies the parent that text is needed to be displayed in a 
							callback field. For more information on this notification and 
							callback fields, see Using Callback Fields in a Date and Time 
							Picker Control.

						    lpNMFormat = (LPNMDATETIMEFORMAT) lParam;
							The owner of the control must return zero. 

							lpNMFormat	-	Address of an NMDATETIMEFORMAT structure 
											containing information regarding this instance 
											of the notification message. The structure 
											contains the substring that defines the 
											callback field and receives the formatted 
											string that the control will display. 

							NMDATETIMEFORMAT

							typedef struct tagNMDATETIMEFORMAT{
								NMHDR       nmhdr;
								LPCTSTR     pszFormat;
								SYSTEMTIME  st;
								LPCTSTR     pszDisplay;
								TCHAR       szDisplay[64];
							} NMDATETIMEFORMAT, FAR * LPNMDATETIMEFORMAT;
							Contains information about a portion of the format string that 
							defines a callback field within a date and time picker (DTP) 
							control. It carries the substring that defines the callback 
							field and contains a buffer to receive the string that will be 
							displayed in the callback field. This structure is used with 
							the DTN_FORMAT notification message. 

							nmhdr		-	NMHDR structure that contains information 
											about the notification message. 
							pszFormat	-	Address of the substring that defines a DTP 
											control callback field The substring comprises 
											one or more "X" characters, followed by a NULL 
											character.
							st			-	SYSTEMTIME structure that contains information 
											about the current system date and time. 
							pszDisplay	-	Address of a null-terminated string that 
											contains the display text of the control. 
											By default, this is the address of the 
											szDisplay member of this structure. It is legal 
											to set this member to point to an existing 
											string. If so, the application is not required 
											to place a string into the szDisplay member. 
							szDisplay	-	64-character buffer that is to receive the 
											zero-terminated string that the DTP control 
											will display. It is not necessary to fill the 
											entire buffer.
											
							Handling this message allows the owner of the control to 
							provide a custom string that the control will display.

	DTN_FORMATQUERY		-	Requests the parent to supply the maximum allowable size of 
							the string that will be displayed in a callback field. 
							Handling this notification allows the control to properly 
							display output at all times, reducing flicker within the 
							control's display. For more information on this notification, 
							see Using Callback Fields in a Date and Time Picker Control.

						    lpDTFormatQuery = (LPNMDATETIMEFORMATQUERY) lParam;
							The owner of the control must calculate the maximum possible 
							width of the text that will be displayed in the callback field, 
							set the szMax member of the NMDATETIMEFORMATQUERY structure, 
							and return zero. 

							lpDTFormatQuery		-	Address of an NMDATETIMEFORMATQUERY 
													structure containing information about 
													the callback field. The structure 
													contains a substring that defines a 
													callback field and receives the maximum 
													allowable size of the string that will 
													be displayed in the callback field. 

							NMDATETIMEFORMATQUERY

							typedef struct tagNMDATETIMEFORMATQUERY{
								NMHDR nmhdr;
								LPCTSTR pszFormat;
								SIZE szMax;
							} NMDATETIMEFORMATQUERY, FAR * LPNMDATETIMEFORMATQUERY;

							Contains information about a date and time picker (DTP) control 
							callback field. It contains a substring (taken from the 
							control's format string) that defines a callback field. 
							The structure receives the maximum allowable size of the text 
							that will be displayed in the callback field. This structure 
							is used with the DTN_FORMATQUERY notification message. 

							nmhdr		-	NMHDR structure that contains information 
											about this notification message. 
							pszFormat	-	Address of a substring that defines a DTP 
											control callback field. The substring is one 
											or more "X" characters followed by a NULL.
							szMax		-	SIZE structure that must be filled with the 
											maximum size of the text that will be displayed 
											in the callback field. 


							Handling this message prepares the control to adjust for the 
							maximum size of the string that will be displayed in a 
							particular callback field. This enables the control to properly 
							display output at all times, reducing flicker within the 
							control's display. 

	DTN_USERSTRING		-	Notifies the parent that the user has finished editing the 
							contents of the date and time picker control. This notification 
							is only sent when the DTS_APPCANPARSE style has been set.

							lpDTstring = (LPNMDATETIMESTRING) lParam;
							The owner of the control must return zero. 

							lpDTstring	-	Address of an NMDATETIMESTRING structure 
											that contains information about the instance 
											of the notification message. 

							NMDATETIMESTRING

							typedef struct tagNMDATETIMESTRING{
								NMHDR      nmhdr;
								LPCTSTR    pszUserString;
								SYSTEMTIME st;
								DWORD      dwFlags;
							} NMDATETIMESTRING, FAR * LPNMDATETIMESTRING;

							Contains information specific to an edit operation that has 
							taken place in a date and time picker (DTP) control. This 
							message is used with the DTN_USERSTRING notification message. 

							nmhdr			-	NMHDR structure that contains information 
												about this notification message. 
							pszUserString	-	Address of the zero-terminated string that 
												the user entered. 
							st				-	SYSTEMTIME structure that must be filled in 
												by the owner when handling the 
												DTN_USERSTRING notification message. 
							dwFlags			-	Return field. Set this member to GDT_VALID 
												to indicate that the st member is valid or 
												to GDT_NONE to set the control to "no date" 
												status (DTS_SHOWNONE style only). 


							Handling this message allows the owner to provide custom 
							responses to strings entered into the control by the user. 
							The owner must be prepared to parse the input string and take 
							action if necessary. 

	DTN_WMKEYDOWN		-	Notifies the parent when the user types in a callback field. 
							Handle this notification to emulate the same keyboard response 
							supported for non-callback fields in a date and time picker 
							control. 

						    lpDTKeystroke = (LPNMDATETIMEWMKEYDOWN)lParam;
							The owner of the control must return zero. 
	
							lpDTKeystroke	-	Address of an NMDATETIMEWMKEYDOWN structure 
												containing information about this instance 
												of the notification. The structure includes 
												information about the key that the user 
												typed, the substring that defines the 
												callback field, and the current system 
												date and time. 

							NMDATETIMEWMKEYDOWN
	
							typedef struct tagNMDATETIMEWMKEYDOWN{
								NMHDR      nmhdr;
								int        nVirtKey;
								LPCTSTR    pszFormat;
								SYSTEMTIME st;
							} NMDATETIMEWMKEYDOWN, FAR * LPNMDATETIMEWMKEYDOWN;
							Carries information used to describe and handle a DTN_WMKEYDOWN 
							notification message. 

							nmhdr		-	NMHDR structure that contains information 
											about the notification message. 
							nVirtKey	-	Virtual key code that represents the key that 
											the user pressed. 
							pszFormat	-	Zero-terminated substring, taken from the 
											format string, that defines the callback field. 
											The substring is one or more "X" characters, 
											followed by a NULL. 
							st			-	SYSTEMTIME structure containing the current 
											date and time from the DTP control. The owner 
											of the control must modify the time information 
											based on the user's keystroke. 


							Handling this message allows the owner of the control to 
							provide specific responses to keystrokes within the callback 
							fields of the control. 


*/

/////////////////////////////////////////////////////////////////////////////

#if !defined(_OXDATETIMECTRL_H_)
#define _OXDATETIMECTRL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


#include "OXMonthCalCtrl.h"

#ifndef MCS_NOTODAYCIRCLE
#undef MCS_NOTODAY
#if (_WIN32_IE >= 0x0400)
#define MCS_NOTODAYCIRCLE   0x0008
#define MCS_NOTODAY         0x0010
#else
#define MCS_NOTODAY         0x0008
#endif	//	(_WIN32_IE >= 0x0400)
#endif	//	MCS_NOTODAYCIRCLE


///////////////////////////////////////////////////////////////////////////////////////
// OXDateTimeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COXDateTimeCtrl window

#if _MFC_VER<=0x0421
class OX_CLASS_DECL COXDateTimeCtrl : public CWnd
#else
class OX_CLASS_DECL COXDateTimeCtrl : public CDateTimeCtrl
#endif	//	_MFC_VER>0x0421
{
//#if _MFC_VER<=0x0421
	DECLARE_DYNCREATE(COXDateTimeCtrl)
//#endif	//	_MFC_VER>0x0421

protected:
	static BOOL bComCtlInitialized;

	COXMonthCalCtrl m_ctlMonthCal;

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Initializes Date Time Picker class in the comctl32.dll
	static void InitializeDTPCtrl();

	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect :	Construct the object
	COXDateTimeCtrl();


	// --- In  :	dwStyle			-	Specifies the combination of date time control 
	//									styles. The following window styles are specific 
	//									to date and time picker (DTP) controls: 
	//
	//					DTS_APPCANPARSE			-	Allows the owner to parse user input 
	//												and take necessary action. It enables 
	//												users to edit within the client area 
	//												of the control when they press the F2 
	//												key. The control sends DTN_USERSTRING 
	//												notification messages when users are 
	//												finished. 
	//					DTS_LONGDATEFORMAT		-	Displays the date in long format. The 
	//												default format string for this style is 
	//												defined by LOCALE_SLONGDATEFORMAT, which 
	//												produces output like 
	//												"Friday, April 19, 1996". 
	//					DTS_RIGHTALIGN			-	The drop-down month calendar will be 
	//												right-aligned with the control instead 
	//												of left-aligned, which is the default.  
	//					DTS_SHOWNONE			-	It is possible to have no date currently 
	//												selected in the control. With this style,
	//												the control displays a check box that 
	//												users can check once they have entered 
	//												or selected a date. Until this check 
	//												box is checked, the application will not 
	//												be able to retrieve the date from the 
	//												control because, in essence, the control 
	//												has no date. This state can be set with 
	//												the SetTime function or verified with 
	//												the GetTime function. 
	//					DTS_SHORTDATEFORMAT		-	Displays the date in short format. The 
	//												default format string for this style is 
	//												defined by LOCALE_SSHORTDATE, which 
	//												produces output like "4/19/96". 
	//					DTS_TIMEFORMAT			-	Displays the time. The default format 
	//												string for this style is defined by 
	//												LOCALE_STIMEFORMAT, which produces 
	//												output like "5:31:42 PM". 
	//					DTS_UPDOWN				-	Appears to the right of the DTP control 
	//												to modify date values. This style can 
	//												be used instead of the drop-down month 
	//												calendar, which is the default style. 
	//
	//				rect			-	A reference to a RECT structure, which is the 
	//									position and size of the date and time picker 
	//									control.
	//				pParentWnd		-	A pointer to a CWnd object that is the parent 
	//									window of the date and time picker control. It must 
	//									not be NULL.
	//				nID				-	Specifies the date and time picker control's 
	//									control ID.
	// --- Out : 
	// --- Returns:	Nonzero if creation was successful; otherwise 0.
	// --- Effect :	Create a date and time picker control in two steps:
	//				1)	Call COXDateTimeCtrl to construct a COXDateTimeCtrl object. 
	//				2)	Call this member function, which creates the Windows date and 
	//					time picker control and attaches it to the COXDateTimeCtrl object. 
	//				When you call Create, the common controls are initialized
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);


#if _MFC_VER<=0x0421
// Attributes
	// --- In  :	iColor			-	An int value specifying which color area of the 
	//									month calendar to retrieve. For a list of values, 
	//									see the iColor parameter for SetMonthCalColor.
	// --- Out : 
	// --- Returns:	A COLORREF value that represents the color setting for the specified 
	//				portion of the month calendar control if successful. The function 
	//				returns -1 if unsuccessful
	// --- Effect :	Retrieves the color for a given portion of the month calendar within 
	//				a date and time picker (DTP) control. 
	inline COLORREF GetMonthCalColor(int iColor) const { 
		ASSERT(::IsWindow(m_hWnd)); 
		return (COLORREF)::SendMessage(m_hWnd,DTM_GETMCCOLOR,(WPARAM)iColor,0); 
	}


	// --- In  :	iColor			-	An int value specifying which color area of the 
	//									month calendar to change. This value can be one 
	//									of the following.
	//
	//					MCSC_BACKGROUND			-	Set the background color displayed 
	//												between months.  
	//					MCSC_MONTHBK			-	Set the background color displayed 
	//												within a month.  
	//					MCSC_TEXT				-	Set the color used to display text 
	//												within a month.  
	//					MCSC_TITLEBK			-	Set the background color displayed 
	//												in the calendar's title.  
	//					MCSC_TITLETEXT			-	Set the color used to display text 
	//												within the calendar's title.  
	//					MCSC_TRAILINGTEXT		-	Set the color used to display header 
	//												and trailing-day text. Header and 
	//												trailing days are the days from the 
	//												previous and following months that 
	//												appear on the current calendar.  
	//
	//				ref				-	A COLORREF value representing the color that will 
	//									be set for the specified area of the month calendar.
	//
	// --- Out : 
	// --- Returns:	A COLORREF value that represents the previous color setting for the 
	//				specified portion of the month calendar control if successful. 
	//				Otherwise, the message returns -1. 
	// --- Effect :	Sets the color for a given portion of the month calendar within a date 
	//				and time picker (DTP) control. 
	inline COLORREF SetMonthCalColor(int iColor, COLORREF ref) { 
		ASSERT(::IsWindow(m_hWnd)); 
		return (COLORREF)::SendMessage(m_hWnd,DTM_SETMCCOLOR,(WPARAM)iColor,(LPARAM)ref); 
	}


	// --- In  :	pstrFormat		-	A pointer to a zero-terminated format string that 
	//									defines the desired display. Setting this parameter 
	//									to NULL will reset the control to the default 
	//									format string for the current style. 
	// --- Out : 
	// --- Returns:	Nonzero if successful; otherwise 0.
	// --- Effect :	Sets the display of a date and time picker (DTP) control based on a 
	//				given format string. Refer to "Custom format strings" section in 
	//				the documentation for description of available format characters.
	inline BOOL SetFormat(LPCTSTR pstrFormat) { 
		ASSERT(::IsWindow(m_hWnd)); 
		return (BOOL)::SendMessage(m_hWnd,DTM_SETFORMAT,0,(LPARAM)pstrFormat); 
	}


	// --- In  :
	// --- Out : 
	// --- Returns:	A pointer to a CFont object, or NULL if unsuccessful.
	// --- Effect :	Call this member function to get the font currently used by the 
	//				date and time picker control's month calendar control. The CFont 
	//				object pointed to by the return value is a temporary object and 
	//				is destroyed during the next idle processing time.
	inline CFont* GetMonthCalFont() const { 
		ASSERT(::IsWindow(m_hWnd)); 
		return CFont::FromHandle((HFONT) ::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0)); 
	}


	// --- In  :	hFont			-	Handle to the font that will be set. 
	//				bRedraw			-	Specifies whether the control should be redrawn 
	//									immediately upon setting the font. Setting this 
	//									parameter to TRUE causes the control to redraw 
	//									itself. 
	// --- Out : 
	// --- Returns:	
	// --- Effect :	Sets the font to be used by the date and time picker (DTP) control's 
	//				child month calendar control. 
	inline void SetMonthCalFont(HFONT hFont, BOOL bRedraw = TRUE) { 
		ASSERT(::IsWindow(m_hWnd)); 
		::SendMessage(m_hWnd, DTM_SETMCFONT, (WPARAM) hFont, MAKELONG(bRedraw, 0)); 
	}

// Operations

	// --- In  :	timeNew			-	A reference to a COleDateTime object containing 
	//									the to which the control will be set.
	//				pTimeNew		-	In the second version above, a pointer to a CTime 
	//									object containing the time to which the control 
	//									will be set. In the third version above, a pointer 
	//									to a SYSTEMTIME structure containing the time to 
	//									which the control will be set.
	// --- Out : 
	// --- Returns:	Nonzero if successful; otherwise 0. 
	// --- Effect :	Sets the time in a date and time picker (DTP) control.
	BOOL SetTime(const COleDateTime& timeNew);
	BOOL SetTime(const CTime* pTimeNew);
	BOOL SetTime(LPSYSTEMTIME pTimeNew = NULL);


	// --- In  :	
	// --- Out :	timeDest		-	In the first version, a reference to a COleDateTime 
	//									object that will receive the system time information.
	//									In the second version, a reference to a CTime object
	//									that will receive the system time information.
	//				pTimeDest		-	A pointer to the SYSTEMTIME structure to receive 
	//									the system time information. Must not be NULL.
	// --- Returns:	In the first version, nonzero if the time is successfully written to 
	//				the COleDateTime object; otherwise 0. In the second and third versions, 
	//				a DWORD value equal to the dwFlag member set in the NMDATETIMECHANGE 
	//				structure.
	// --- Effect :	Retrieves the currently selected time from a date and time picker (DTP) 
	//				control. The return value DWORD in the second and third versions, 
	//				above, indicates whether or not the date and time picker control is set 
	//				to the "no date" status, as indicated in the NMDATETIMECHANGE structure 
	//				member dwFlags. If the value returned equals GDT_NONE, the control is 
	//				set to "no date" status, and uses the DTS_SHOWNONE style. If the value 
	//				returned equals GDT_VALID, the system time is successfully stored in 
	//				the destination location.
	BOOL GetTime(COleDateTime& timeDest) const;
	DWORD GetTime(CTime& timeDest) const;
	inline DWORD GetTime(LPSYSTEMTIME pTimeDest) const 
	{ 
		ASSERT(::IsWindow(m_hWnd)); 
		ASSERT(pTimeDest != NULL); 
		return (DWORD)::SendMessage(m_hWnd,DTM_GETSYSTEMTIME,0,(LPARAM)pTimeDest); 
	}

#endif	//	_MFC_VER>0x0421

	// --- In  :	pMinRange		-	A pointer to a COleDateTime object or a CTime 
	//									object or a pointer to a SYSTEMTIME structure
	//									containing the earliest time allowed in 
	//									the COXDateTimeCtrl object.
	//				pMaxRange		-	A pointer to a COleDateTime object or a CTime 
	//									object or a pointer to a SYSTEMTIME structure
	//									containing the latest time allowed in the 
	//									COXDateTimeCtrl object. 
	// --- Out : 
	// --- Returns:	Nonzero if successful; otherwise 0. 
	// --- Effect :	Sets the minimum and maximum allowable system times for a date and 
	//				time picker (DTP) control.
	BOOL SetRange(const COleDateTime* pMinRange, const COleDateTime* pMaxRange);
	BOOL SetRange(const CTime* pMinRange, const CTime* pMaxRange);
	BOOL SetRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange);


	// --- In  :	
	// --- Out :	pMinRange		-	A pointer to a COleDateTime object or a CTime 
	//									object or a pointer to a SYSTEMTIME structure
	//									containing the earliest time allowed in 
	//									the COXDateTimeCtrl object.
	//				pMaxRange		-	A pointer to a COleDateTime object or a CTime 
	//									object or a pointer to a SYSTEMTIME structure
	//									containing the latest time allowed in the 
	//									COXDateTimeCtrl object.
	// --- Returns:	A DWORD value containing flags that indicate which ranges are set.
	// --- Effect :	Retrieves the current minimum and maximum allowable system times for 
	//				a date and time picker (DTP) control. 
	DWORD GetRange(COleDateTime* pMinRange, COleDateTime* pMaxRange) const;
	DWORD GetRange(CTime* pMinRange, CTime* pMaxRange) const;
	DWORD GetRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const;


	// --- In  :
	// --- Out : 
	// --- Returns:	A pointer to a COXMonthCalCtrl object, or NULL if unsuccessful or 
	//				if the window is not visible
	// --- Effect :	Call this member function to retrieve the COXMonthCalCtrl object 
	//				associated with the date and time picker control. Date and time picker 
	//				controls create a child month calendar control when the user clicks 
	//				the drop-down arrow. When the COXMonthCalCtrl object is no longer needed, 
	//				it is destroyed, so your application must not rely on storing the 
	//				object representing the date time picker control's child month calendar.
	inline COXMonthCalCtrl* GetMonthCalCtrl() const 
	{ 
		ASSERT(::IsWindow(m_hWnd)); 

		HWND hwndMonthCal=(HWND)::SendMessage(m_hWnd,DTM_GETMONTHCAL,0,0);
		if(hwndMonthCal!=NULL)
		{
			ASSERT(::IsWindow(m_ctlMonthCal.GetSafeHwnd()));
			ASSERT(m_ctlMonthCal.GetSafeHwnd()==hwndMonthCal);

			COXMonthCalCtrl* pMonthCal=(COXMonthCalCtrl*)&m_ctlMonthCal;
			return pMonthCal;
		}
		else
			return NULL;
	}


	// --- In  :	hFont			-	Handle to the font that will be set. 
	//				bRedraw			-	Specifies whether the control should be redrawn 
	//									immediately upon setting the font. Setting this 
	//									parameter to TRUE causes the control to redraw 
	//									itself. 
	// --- Out : 
	// --- Returns:	
	// --- Effect :	Sets the font to be used by the date and time picker (DTP) control's 
	//				child month calendar control. 
	inline void SetMonthCalFont(CFont* pFont, BOOL bRedraw = TRUE) 
	{ 
		ASSERT(::IsWindow(m_hWnd)); 
		ASSERT(pFont!=NULL); 
#if _MFC_VER<=0x0421
		SetMonthCalFont((HFONT)*pFont,bRedraw);
#else
		CDateTimeCtrl::SetMonthCalFont((HFONT)*pFont,bRedraw);
#endif	//	_MFC_VER>0x0421
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXSHBListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
// Overridables
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect :	destructor
	virtual ~COXDateTimeCtrl();

protected:

	// Generated message map functions
	//{{AFX_MSG(COXDateTimeCtrl)
	//}}AFX_MSG
	afx_msg BOOL OnDropDown(NMHDR *pHdr, LRESULT *pRes);
	DECLARE_MESSAGE_MAP()

protected:
	// initialize calendar control
	virtual BOOL InitDateTimeCtrl();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXDATETIMECTRL_H_)
