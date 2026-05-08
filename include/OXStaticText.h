// ==========================================================================
// 					Class Specification	: COXStaticText
// ==========================================================================

// Header file :		OXStaticText.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
						  
// //////////////////////////////////////////////////////////////////////////

// Properties :
//	NO	Abstract class (does not have any objects)
//	YES	Derived	from CStatic

//	YES	Is a CWnd.
//	YES	Two	stage creation (constructor	& Create())
//	YES	Has	a message map
//	NO	Needs a	resource (template)

//	NO	Persistent objects (saveable on	disk)
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Description :
//	The	COXStaticText class	is a control where user	can	put	text in. Normal	look of	this control is
//	similar	to a static	control	-- it just shows a text	string.	But	unlike the MFC's CStatic, this
//	class allows the user to give that text	a certain special appearance:
//		* Set/get the text color;
//		* Set/get the background color;
//		* Set/get the text font	and	attributes;
//		* Drawing 3D text =	Embossed text;
//		* Set/get the 3D offset	(pixels);
//		* Text scrolling at	any	direction;
//		* Set/get scrolling	speed (pixels per second);
//		* Drawing text under an	angle;
//		* Drawing ellipses at the begin, end or	middle of a	text string;
//		* Drawing special borders: sunken, raised, flat	line, dotted line.

// Remark :
//	Text scrolling is carried out by the separate worker thread.

// Prerequisites (necessary	conditions):
// Including the <afxmt.h> and <math.h>.

/////////////////////////////////////////////////////////////////////////////

#ifndef	__OXSTATEXT_H__
#define	__OXSTATEXT_H__

#if	_MSC_VER >=	1000
#pragma	once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"


#include <afxmt.h>
#include <math.h>


// //////////////////////////////////////////////////////////////////////////
// Definition of the font attributes constants.
#define	OX_BOLD_FONT		0
#define	OX_ITALIC_FONT		1
#define	OX_UNDERLINED_FONT	2
#define	OX_STRIKED_OUT_FONT	3

// Definition of ellipses replacing	mode constants.
#define	OX_BEGIN_ELLIPSES	4
#define	OX_MIDDLE_ELLIPSES	5
#define	OX_END_ELLIPSES		6
#define	OX_NO_ELLIPSES		7

#define	OX_ALIGNHORZ_LEFT		0
#define	OX_ALIGNHORZ_CENTER		1
#define	OX_ALIGNHORZ_RIGHT		2
#define	OX_ALIGNVERT_TOP		3
#define	OX_ALIGNVERT_CENTER		4
#define	OX_ALIGNVERT_BOTTOM		5

// Definition of PI	constant.
#define	OX_PI				3.1415926535

// Definition of maximum text 3D offset.
#define	OX_MAX_3DOFFSET		100

// //////////////////////////////////////////////////////////////////////////

class OX_CLASS_DECL	COXStaticText :	public CStatic
{
// Data	members	-------------------------------------------------------------
public:
	CBitmap	*m_BMP;

protected:
	int					m_nHorzAlignment;
	int					m_nVertAlignment;

	BOOL				m_bDottedEdge;
	BOOL				m_bAllowRefresh;
	BOOL				m_bEmbossText;
	BOOL				m_bEmbossRaised;
	COLORREF			m_clrEmbossHighLight;
	COLORREF			m_clrEmbossShadow;

	DWORD				m_dwCurrentTickDelta;
	DWORD				m_dwLastTickDelta;
	DWORD				m_dwBeginTickCount;
	DWORD				m_dwEndTickCount;
	DWORD				m_dwBkColor;
	DWORD				m_dwMinTimeOut;
	DWORD				m_dwOffset;
	DWORD				m_dwScrollSpeed;
	DWORD				m_dwScrollTimeOut;
	DWORD				m_dwTextColor;
	LOGFONT				m_LogFont;
	int					m_nEllipseMode;
	int					m_nGraphicsMode;
	int					m_nScrollAmount;
	int					m_nScrollDirection;

	double				m_nXCastDisplacement;
	double				m_nXDisplacement;
	double				m_dXDelta;
	double				m_nYCastDisplacement;
	double				m_nYDisplacement;
	double				m_dYDelta;

	CEvent*				m_pEventLoop;
	CCriticalSection*	m_pCritSecRedrawWait;

	CFont*				m_pObjFont;
	CWinThread*			m_pScrollingThread;
	CString				m_sText;
	CString				m_sTextNarrow;
	
	CSize				m_szTextSize;
	
	CSize				m_szGapSize;

	CRect				m_rectViewMargins;

private:
	// Registered message to to	prepare	bitmap for COXStaticText object. Used in 
	// PrepareBitmap() function
	//
	static UINT	m_nPrepareBitmap;

// Member functions	---------------------------------------------------------
public:
	// --- In :			dwOffset :		Initilal text 3D offset	(0 by default).
	//					nGraphicsMode :	Initilal text graphics mode	(Compatible	by default).
	//					nHorzAlignment	:	Initial	horizontal text	alignment
	//					nVertAlignment	:	Initial	vertical text alignment
	// --- Out :
	// --- Returns :
	// --- Effect :		Constructs the COXStaticText object.
	COXStaticText(DWORD	dwOffset = 0, int nGraphicsMode	= GM_COMPATIBLE, 
		int	nHorzAlignment = OX_ALIGNHORZ_CENTER, 
		int	nVertAlignment = OX_ALIGNVERT_CENTER);

	// --- In :
	// --- Out :
	// --- Returns :
	// --- Effect :		Destroys the COXStaticText object.
	virtual	~COXStaticText();


	// --- In :			lpszText : Specifies the text to place in the control.
	//							   If NULL,	no text	will be	visible
	//					dwStyle	: Specifies	the	static control's window	style.
	//							  Normal Static	control	styles are not allowed 
	//					rect : Specifies the position and size of the static control.
	//						   It can be either	a RECT structure or	a CRect	object.
	//					pParentWnd : Specifies the CStatic parent window, usually a
	//								 CDialog object. It	must not be	NULL.
	//					nID	: Specifies	the	static control's control ID.
	// --- Out :
	// --- Returns : succesful or not.
	// --- Effect :		Creates	the	COXStaticText object.
	BOOL Create(LPCTSTR	lpszText, DWORD	dwStyle, const RECT& rect, CWnd* pParentWnd,
		UINT nID = 0xffff );
	
	// --- In :			nAlignment : Text alignment	(can be	left,right and center).
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text horizontal alignment.
	BOOL SetHorzAlignment(int nAlignment, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text alignment.
	// --- Effect :		Retrieves the text horizontal alignment.
	int	GetHorzAlignment() const;


	// --- In :			nAlignment : Text alignment	(can be	top,bottom and center).
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text vertical alignment.
	BOOL SetVertAlignment(int nAlignment, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text alignment.
	// --- Effect :		Retrieves the text vertical	alignment.
	int	GetVertAlignment() const;


	// --- In :			dwBkColor :	Text background	color.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text background color.
	BOOL SetBkColor(COLORREF dwBkColor,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text background	color.
	// --- Effect :		Retrieves the text background color.
	COLORREF GetBkColor() const;


	// --- In :			dwTextColor	: Text color.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text color.
	BOOL SetTextColor(COLORREF dwTextColor,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text color.
	// --- Effect :		Retrieves the text color.
	COLORREF GetTextColor()	const;


	// --- In :			dwTextColor	: Text color.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text color.
	BOOL SetEmboss(BOOL	bEmboss	= TRUE,	BOOL bRaised = FALSE, BOOL bPrepareNow = FALSE,
				   COLORREF	clrHLight =	::GetSysColor(COLOR_BTNHIGHLIGHT),
				   COLORREF	clrShadow =	::GetSysColor(COLOR_BTNSHADOW));

	// --- In :
	// --- Out :
	// --- Returns :	Is the text	embossed or	not
	// --- Effect :		Retrieves the embossed mode
	BOOL GetEmboss() const;
	

	// --- In :			psText : Text string.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text string.
	BOOL SetWindowText(LPCTSTR psText, BOOL	bPrepareNow	= FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Pointer	to the text	string.
	// --- Effect :		Retrieves the text string.
	LPCTSTR	GetText() const;


	// --- In :			nGraphicsMode :	Specifies the graphics mode.
	//									This parameter can be one of the 
	//									following values:
	//
	//		GM_COMPATIBLE	-	Sets the graphics mode that	is compatible with Windows
	//							version	3.1. This is the default mode. If this value is
	//							specified, the application can only	modify the 
	//							world-to-device	transform by calling functions that	
	//							set	window and viewport	extents	and	origins, but not 
	//							by using SetWorldTransform or ModifyWorldTransform;	
	//							calls to those functions will fail.
	//		GM_ADVANCED		-	Windows	NT:	Sets the advanced graphics mode	that allows
	//							world transformations. This	value must be specified	
	//							if the application will	set	or modify the world	
	//							transformation for the specified device	context. 
	//							In this	mode all graphics, including text output, 
	//							fully comforms to the world-to-device transformation 
	//							specified in the device	context.
	//							Windows	95:	The	GM_ADVANCED	value is not supported.	
	//							When playing enhanced metafiles, Windows 95	does its 
	//							best to	ensure that	enhanced metafiles on Windows 95 
	//							look the same as they do on	Windows	NT.	To accomplish 
	//							this, Windows 95 may simulate GM_ADVANCED mode when	
	//							playing	specific enhanced metafile records.
	//
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the graphics mode.
	BOOL SetGraphicsMode(int nGraphicsMode,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	The	graphics mode (see above).
	// --- Effect :		Retrieves the graphics mode.
	int	GetGraphicsMode() const;


	// --- In :			nEllipseMode :	Ellipses replacing mode. This parameter	can	be one of the following	values:
	//									OX_BEGIN_ELLIPSES	- replace characters at	the	begin of the string,
	//									OX_MIDDLE_ELLIPSES	- replace characters at	the	middle of the string,
	//									OX_END_ELLIPSES		- replace characters at	the	end	of the string,
	//									OX_NO_ELLIPSES		- no characters	replacing.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the mode of symbols/ellipses replacing.
	//					If specified mode is already set, function sets	it to OX_NO_ELLIPSES.
	BOOL SetEllipseMode(int	nEllipseMode, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Mode of	symbols/ellipses replacing (see	above).
	// --- Effect :		Retrieves the ellipses replacing mode.
	int	GetEllipseMode() const;

	
	// --- In :			dwOffset :	Text 3D	offset (pixels).
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the text 3D offset	(MAX = OX_MAX_3DOFFSET).
	//					Function throws	values higher than OX_MAX_3DOFFSET off.
	BOOL Set3Doffset(DWORD dwOffset, BOOL bPrepareNow =	FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text 3D	offset (pixels).
	// --- Effect :		Retrieves the text 3D offset.
	DWORD Get3Doffset()	const;


	// --- In :			nAttr :	Font attribute.	This parameter can be one of the following values:
	//							OX_BOLD_FONT		- Bold font.
	//							OX_ITALIC_FONT		- Italic font.
	//							OX_UNDERLINED_FONT	- Underlined font.
	//							OX_STRIKED_OUT_FONT	- Striked out font.
	//					bSet = TRUE	if the attribute is	to be set, otherwise FALSE
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the font attributes.
	BOOL SetFontAttr(int nAttr,	BOOL bSet =	TRUE, BOOL bPrepareNow = FALSE);
	

	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the font is currently	bold, FALSE	- otherwise.
	// --- Effect :		Returns	whether	the	font is	currenly bold.
	BOOL IsBold() const;

	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the font is currently	italic,	FALSE -	otherwise.
	// --- Effect :		Returns	whether	the	font is	currenly italic.
	BOOL IsItalic()	const;

	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the font is currently	underlined,	FALSE -	otherwise.
	// --- Effect :		Returns	whether	the	font is	currenly underlined.
	BOOL IsUnderlined()	const;

	// --- In :
	// --- Out :
	// --- Returns :	TRUE - if the font is currently	striked	out, FALSE - otherwise.
	// --- Effect :		Returns	whether	the	font is	currenly striked out.
	BOOL IsStrikedOut()	const;


	// --- In:		nAngle :	Specifies the angle, in	tenths of degrees, of each line
	//							of text	written	in the font	(relative to the bottom	of the page).
	//				bPrepareNow:TRUE if	internal image has to be rebuild.  If more than	one
	//							feature	of the COXStaticText control is	set, the last
	//							set	has	to have	this parameter set to TRUE,	to have	the	image build
	// --- Out:
	// --- Returns:	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect:	Sets the text angle.
	BOOL SetStringAngle(int	nAngle,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Text angle.
	// --- Effect :		Retrieves the text angle.
	int	GetStringAngle() const;


	// --- In :			nAngle :	Specifies the angle, in	tenths of degrees, of each
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	//								character's	base line (relative	to the bottom of the page).
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the character's angle (works in Advanced graphics mode	only).
	BOOL SetCharAngle(int nAngle, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Character's	angle.
	// --- Effect :		Retrieves the character's angle.
	int	GetCharAngle() const;


	// --- In :			nCharSet :	Specifies the character	set. The following values are predefined:
	//								ANSI_CHARSET, OEM_CHARSET, SYMBOL_CHARSET, UNICODE_CHARSET.
	//								The	OEM	character set is system	dependent.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the character set.
	BOOL SetCharSet(int	nCharSet, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Character set.
	// --- Effect :		Retrieves the character	set	(see above).
	int	GetCharSet() const;


	// --- In :			nHeight	- Specifies	the	height,	in logical units, of the font. The font	height
	//							can	be specified in	one	of three ways. If nHeight is greater than zero,
	//							it is transformed into device units	and	matched	against	the	cell height
	//							of the available fonts.	If it is zero, a reasonable	default	size is	used.
	//							If it is less than zero, it	is transformed into	device units and the
	//							absolute value is matched against the character	height of the available
	//							fonts. For all height comparisons, the font	mapper looks for the largest
	//							font that does not exceed the requested	size; if there is no such font,
	//							it looks for the smallest font available. This mapping occurs when the
	//							font is	actually used for the first	time.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the font height.
	BOOL SetFontHeight(int nHeight,	BOOL bPrepareNow = FALSE);

	// --- In :			nWidth	- Specifies	the	width,	in logical units, of the font. The font	height
	//							can	be specified in	one	of three ways. If nWidth is greater than zero,
	//							it is transformed into device units	and	matched	against	the	cell width
	//							of the available fonts.	If it is zero, a reasonable	default	size is	used.
	//							If it is less than zero, it	is transformed into	device units and the
	//							absolute value is matched against the character	height of the available
	//							fonts. For all height comparisons, the font	mapper looks for the largest
	//							font that does not exceed the requested	size; if there is no such font,
	//							it looks for the smallest font available. This mapping occurs when the
	//							font is	actually used for the first	time.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the font width.
	BOOL SetFontWidth(int nWidth,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Font height.
	// --- Effect :		Retrieves the font height.
	int	GetFontHeight()	const;

	// --- In :
	// --- Out :
	// --- Returns :	Font width.
	// --- Effect :		Retrieves the font width.
	int	GetFontWidth() const;


	// --- In :			sName	- String that specifies	the	typeface name of the font.
	//							The	length of this string must not exceed LF_FACESIZE characters.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the typeface name of the font.
	BOOL SetFontName(LPCTSTR sName,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	Typeface name of the font.
	// --- Effect :		Retrieves the typeface name	of the font.
	CString	GetFontName() const;


	// --- In :			plf	- A	pointer	to object of LOGFONT structure that	specifies 
	//							font to	be used	to draw	text.
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the font.
	BOOL SetLogFont(LOGFONT* plf, BOOL bPrepareNow = FALSE);

	// --- In :			plf	- A	pointer	to object of LOGFONT structure we use to copy 
	//							information	about the font currently used to draw text.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Retrieves the font info.
	BOOL GetLogFont(LOGFONT* plf) const;


	// --- In :			bSet - if TRUE,	thin-line border is	set, if	FALSE, it cancels.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the window	thin-line border.
	BOOL SetPlainBorder(BOOL bSet =	TRUE);

	// --- In :			bSet - if TRUE,	three-dimensional border is	set, if	FALSE, it cancels.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the window	three-dimensional border.
	BOOL SetStaticEdge(BOOL	bSet = TRUE);

	// --- In :			bSet - if TRUE,	border with	a sunken edge is set, if FALSE,	it cancels.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the border	with a sunken edge.
	BOOL SetClientEdge(BOOL	bSet = TRUE);

	// --- In :			bSet - if TRUE,	border with	a raised edge is set, if FALSE,	it cancels.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the border	with a raised edge.
	BOOL SetRaisedEdge(BOOL	bSet = TRUE);

	// --- In :			bSet - if TRUE,	border with	a dotted edge is set, if FALSE,	it cancels.
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Sets the border	with a dotted edge.	This mode cancels other	borders.
	BOOL SetDottedEdge(BOOL	bSet = TRUE);


	// --- In :			nDirection - text scrolling	directions (in degrees).
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :
	// --- Effect :		Sets the text scrolling	directions (in degrees).
	void SetScrollDirection(int	nDirection,	BOOL bPrepareNow = FALSE);


	// --- In :
	// --- Out :
	// --- Returns :	Text scrolling directions (in degrees).
	// --- Effect :		Retrieves the text scrolling directions	(in	degrees).
	int	GetScrollDirection() const;


	// --- In :			dwScrollSpeed -	text scrolling speed (pixels per second).
	// --- Out :
	// --- Returns :
	// --- Effect :		Sets the text scrolling	speed (pixels per second).
	void SetScrollSpeed(DWORD dwScrollSpeed);

	// --- In :
	// --- Out :
	// --- Returns :	Text scrolling speed (pixels per second).
	// --- Effect :		Retrieves the text scrolling speed (pixels per second).
	DWORD GetScrollSpeed() const;


	// --- In :			bStart - if	TRUE, text scrolling starts, is	FALSE, it stops.
	// --- Out :
	// --- Returns :
	// --- Effect :		Starts/stops the text scrolling.
	//					Initial	speed is 0 pixels/second and initial direction is 0	degrees.
	//					Scroll speed and direction can be changed during text scrolling.
	void StartScrolling(BOOL bStart	= TRUE);

	// --- In :
	// --- Out :
	// --- Returns :	TRUE, if text scrolling	is started,	FALSE otherwise.
	// --- Effect :		Returns	whether	text scrolling is started.
	BOOL IsScrollingStarted() const;


	// --- In :			dwMinTimeOut - minimum time	out	interval for text scrolling.
	// --- Out :
	// --- Returns :
	// --- Effect :		Sets the minimum time out interval for text	scrolling (ms).
	//					Time out interval is calculated	after scroll speed is set.
	//					Its	value can not be lower than	specified minimum (initially 10	ms).
	//					dwMinTimeOut can be	in interval	[1 - 500] ms.
	void SetMinTimeOut(DWORD dwMinTimeOut);

	// --- In :
	// --- Out :
	// --- Returns :	Minimum	time out interval for text scrolling (ms).
	// --- Effect :		Retrieves the minimum time out interval	for	text scrolling.
	DWORD GetMinTimeOut() const;


	// --- In :			szGapSize		-	size of	the	gap	beetwen	the	end	and	
	//										the	begining of	scrolling text.
	//					bPrepareNow		-	if TRUE	then internal image	has	to be rebuild. 
	//										If more	than one feature of	the	COXStaticText 
	//										control	is set,	the	last set has to	have this
	//										parameter set to TRUE, to have the image 
	//										build
	// --- Out :
	// --- Returns :
	// --- Effect :		Sets the size of the gap beetwen the end and 
	//					the	begining of	scrolling text.
	void SetGapSize(CSize& szGapSize, BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	the	size of	the	gap	beetwen	the	end	and	
	//					the	begining of	scrolling text.
	// --- Effect :		Retrieves the size of the gap beetwen the end and 
	//					the	begining of	scrolling text.
	CSize GetGapSize() const;


	// --- In :
	//					bPrepareNow	= TRUE if internal image has to	be rebuild.	 If	more than one
	//								  feature of the COXStaticText control is set, the last
	//								  set has to have this parameter set to	TRUE, to have the image	build
	// --- Out :
	// --- Returns :	TRUE - if the function succeeds; otherwise FALSE.
	// --- Effect :		Restores the text initial position (without	scrolling offset).
	BOOL RestoreTextPos(BOOL bPrepareNow = TRUE);


	// --- In :			rectViewMargins	-	rectangle that contains	left, top, right and 
	//										bottom margins of view rectangle relatively
	//										to control client window rectangle
	//					bPrepareNow		-	if TRUE	then internal image	has	to be rebuild. 
	//										If more	than one feature of	the	COXStaticText 
	//										control	is set,	the	last set has to	have this
	//										parameter set to TRUE, to have the image 
	//										build
	// --- Out :
	// --- Returns :
	// --- Effect :		Sets the size of the gap beetwen the end and 
	//					the	begining of	scrolling text.
	void SetViewMargins(CRect& rectViewMargins,	BOOL bPrepareNow = FALSE);

	// --- In :
	// --- Out :
	// --- Returns :	rectangle that contains	left, top, right and bottom	margins	
	//					of view	rectangle relatively to	control	client window rectangle
	// --- Effect :		Retrieves the sizes	of the margins between view	and	control	
	//					client window rectangles.
	CRect GetViewMargins() const;


	// --- In :
	// --- Out :
	// --- Returns :	the	size of	the	text displayed in the control
	// --- Effect :		Retrieves the size of the text displayed in	the	
	//					control.
	CSize GetTextSize()	const;

protected:
	virtual	BOOL PrepareBitmap(BOOL	bNow);
	static UINT	TextScrollingThreadFunction(LPVOID pParam);
	BOOL RefreshBitmap();
	void EmbossText(CDC* pMemDC, RECT BmpRect);
	void TextOutput(CDC* pMemDC, RECT rect);
	void EllipsesReplace(CDC* pMemDC, LPRECT lpRect);
	void SpeedCalc(DWORD dwScrollSpeed,	DWORD* pdwTimeOut, DWORD* pdwAmount);
	void ScrollAmountRecalc();
	BOOL RebuildFont();
	CSize CalcRectSizes( CDC* pDC);

	void GetInitialDisplacement(CRect& textRect);

	//{{AFX_MSG(COXStaticText)
	afx_msg	BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	void OnPaint();
	afx_msg	void OnNcPaint();
	afx_msg	void OnDestroy();
	afx_msg	void OnSize(UINT nType,	int	cx,	int	cy);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	afx_msg	LRESULT	OnPrepareBitmap(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:

};

// inline functions
#include "OXStaticText.inl"

#endif	// __OXSTATEXT_H__
