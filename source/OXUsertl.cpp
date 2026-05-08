// =============================================================================
// 							Class Implementation : COXUserTool
// =============================================================================
//
// Source file : 		UserTool.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.       
               
// //////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OXUsertl.h"
#include <direct.h>		// For directory functions 

#include "UTBStrOp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const UINT USER_TOOL_SCHEMA = 1;
IMPLEMENT_SERIAL( COXUserTool, CObject, USER_TOOL_SCHEMA );

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

	// CString m_strMenuText;		
	// --- text to display in menu

	// CString m_strCommand;		
	// --- 	command to execute

	// CString m_strArgs;			
	// --- command arguments

	// CString m_strDirectory;	
	// --- initial directory

	// UINT m_uiShowFlag;			
	// --- flags to pass to an WINEXEC command

// private:
	
// Member functions ---------------------------------------------------------
// public:

// This function replaces occurances of the key within the given
// string object with the specified replacement, optionally 
// ignoring case.
//
// returns: the number of substitutions made.

static int Replace( CString& str, LPCTSTR pcszKey, 
				   LPCTSTR pcszReplacement,
				   BOOL bIgnoreCase = FALSE )
	{
	ASSERT( pcszKey != NULL );
	ASSERT( *pcszKey != _T('\0') );
	ASSERT( pcszReplacement != NULL );
	
	if ( pcszKey == NULL || *pcszKey == _T('\0') || pcszReplacement == NULL )
		return -1;
	
	int iStrLength = str.GetLength();
	int iKeyLength = lstrlen( pcszKey );
	
	// A quick optimization; if the key is larger than our current string,
	// it cannot exist within -- get out of here.
	
	if ( iKeyLength > iStrLength )
		return 0;
	
	// First, figure out how large a buffer we need.  If the replacement
	// is smaller or equal to the key, we can simply allocate the same
	// size as the number of characters currently in the string.  If not,
	// we can figure out the maximum number of keys that will be found,
	// multiply by the size differential and allocate that much more.
	// Once we have a number, we can allocate the buffer.
	
	int iBufLength;
	int iRepLength = lstrlen( pcszReplacement );
	
	if ( iRepLength <= iKeyLength )
		{
		iBufLength = iStrLength + 1;
		}
	else
		{
		int iMaxKeys = iStrLength / iKeyLength + 1;
		int iDelta = iRepLength - iKeyLength;
		iBufLength = iStrLength + iMaxKeys * iDelta + 1;
		}
	
	LPTSTR p = new TCHAR[iBufLength];
	memset(p,0,iBufLength*sizeof(TCHAR));
	
	if ( p == NULL )
		return -1;
	
	// declare some strings to handle case insensitivity
	CString strStr( str );
	CString strKey( pcszKey );
	
	if ( bIgnoreCase )
		{
		strStr.MakeUpper();
		strKey.MakeUpper();
		}
	
	// set up some loop controls and get to work!
	LPCTSTR pSource = str;
	LPCTSTR pCurr = strStr;
	LPTSTR pOut = p;
	int iReplacements = 0;
	
	while ( *pCurr != _T('\0') )
		{
		// if we find a match...
		if ( _tcsnccmp( pCurr, strKey, iKeyLength ) == 0 )
			{
			// copy the replacement string into the output buffer
			lstrcpy( pOut, pcszReplacement );
			
			// Increment the output pointer by the string just copied.  
			// Increment the source and current pointers by the key length.
			// Increment the replacement count.
			
			pOut += iRepLength;
			pSource += iKeyLength;
			pCurr += iKeyLength;			
			iReplacements++;
			}
		else
			{
			// otherwise, copy the character across and increment pointers
			*( pOut++ ) = *( pSource++ );
			pCurr++;
			}
		}
	
	str = p;
	delete [] p;
	return iReplacements;
	}

// This function determines if a disk file of the specified name exists,
// optionally returning file status information.
//
// returns: true if the file exists, false otherwise.

static BOOL FileExists( LPCTSTR pcszFilename, CFileStatus* pfs = NULL )
	{
	CFileStatus fs;
	if ( pfs == NULL )
		pfs = &fs;
	return CFile::GetStatus( pcszFilename, *pfs );
	}

// This function locates an item of the given ID with the specified
// menu, optionally returning the item's zero-indexed position.
//
// returns: true if found, false otherwise.

static BOOL FindMenuItem( CMenu* pMenu, UINT uiID, UINT* puiPos )
	{
	if ( pMenu != NULL )
		{
		UINT uiNum = pMenu->GetMenuItemCount();
		for ( UINT ui = 0; ui < uiNum; ui++ )
			{
			if ( pMenu->GetMenuItemID( ui ) == uiID )
				{
				if ( puiPos != NULL )
					*puiPos = ui;
				
				return TRUE;
				}
			}
		}
	
	return FALSE;
	}

// This function launches the specified command from within the given
// directory, showing (or hiding) the window based upon the supplied
// ShowWindow compatible parameter.
//
// returns: true if successful, false otherwise.

static BOOL Run( LPCTSTR pcszDir, LPCTSTR pcszCmd, UINT uiCmdShow )
	{
	BOOL bReturn = FALSE;
	
	// Change directory if needed.
	
	if ( pcszDir != NULL && *pcszDir != _T('\0') )
		{
		_chdrive( _totupper( *pcszDir ) - _T('\0') + 1 );
		_tchdir( pcszDir );
		}               
	
	// Launch the command and set the return variable if successful.
	
	const UINT FIRST_VALID_INSTANCE = 32;

#ifdef _UNICODE
	const size_t len = ::lstrlen(pcszCmd);
	LPSTR pszCommandLine = new char[(char)len];
	size_t t;
	UTBStr::wcstombs(&t, pszCommandLine, len, pcszCmd, lstrlen(pcszCmd));
	if ( WinExec( pszCommandLine, uiCmdShow ) >= FIRST_VALID_INSTANCE )
#else
	if ( WinExec( pcszCmd, uiCmdShow ) >= FIRST_VALID_INSTANCE )
#endif
		bReturn = TRUE;
	
	ASSERT( bReturn );		
	return bReturn;
	}

////////////////////////////////////////////////////////////////////////////////
//
// COXUserTool implementation.
//
////////////////////////////////////////////////////////////////////////////////

COXUserTool::COXUserTool()
	: m_uiShowFlag( SW_SHOWNORMAL )
	{
	}

COXUserTool::COXUserTool( const COXUserTool& rhs )
	{
	*this = rhs;
	}

COXUserTool::~COXUserTool()
	{
	}

void COXUserTool::Serialize( CArchive& archive )
	{
	CObject::Serialize( archive );
	
	if ( archive.IsStoring() )
		{
		archive << m_strMenuText;
		archive << m_strCommand;
		archive << m_strArgs;
		archive << m_strDirectory;
		LONG l = m_uiShowFlag;
		archive << l;
		}	
	else
		{
		archive >> m_strMenuText;
		archive >> m_strCommand;
		archive >> m_strArgs;
		archive >> m_strDirectory;
		LONG l;
		archive >> l;
		m_uiShowFlag = (UINT)l;
		}
	}

BOOL COXUserTool::ParseReplacement( LPCTSTR p, CString& strKey, 
								 CString& strData ) const
	{
	CString strTmp( p );
	int iIndex = strTmp.Find( _T('=') );
	
	if ( iIndex > -1 )
		{
		strKey = strTmp.Left( iIndex );
		strData = strTmp.Right( strTmp.GetLength() - iIndex - 1 );
		return !strKey.IsEmpty() && !strData.IsEmpty();
		}
	
	return FALSE;
	}

BOOL COXUserTool::Execute( CStringList* pReplacements ) const
	{
	CString strCmd( GetCommand() );
	CString strArgs( GetArgs() );
	CString strDir( GetDirectory() );
	
	if ( ( pReplacements != NULL ) && !pReplacements->IsEmpty() )
		{
		CString strKey, strData;
		
		for ( POSITION pos = pReplacements->GetHeadPosition();
		pos != NULL;
		pReplacements->GetNext( pos ) )
			{
			CString& str = pReplacements->GetAt( pos );
			
			if ( ParseReplacement( str, strKey, strData ) )
				{
				Replace( strCmd, strKey, strData );
				Replace( strArgs, strKey, strData );
				Replace( strDir, strKey, strData );
				}
			}
		}
	
	CString strCmdLine( strCmd );
	if ( !strArgs.IsEmpty() )
		{
		strCmdLine += _T(" ");
		strCmdLine += strArgs;
		}
	
	BOOL bReturn = Run( strDir, strCmdLine, GetShowFlag() );
	ASSERT( bReturn );
	return bReturn;
	}

COXUserTool& COXUserTool::operator=( const COXUserTool& rhs )
	{
	if ( &rhs != this )
		{
		m_strMenuText = rhs.m_strMenuText;
		m_strCommand = rhs.m_strCommand;
		m_strArgs = rhs.m_strArgs;
		m_strDirectory = rhs.m_strDirectory;
		m_uiShowFlag = rhs.m_uiShowFlag;
		}
	
	return *this;
	}

COXUserTool* COXUserTool::Clone() const
	{
	return new COXUserTool( *this );
	}

#ifdef _DEBUG
void COXUserTool::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	dc << _T("\nm_strMenuText : ") << m_strMenuText;
	dc << _T("\nm_strCommand : ") << m_strCommand;
	dc << _T("\nm_strArgs : ") << m_strArgs;
	dc << _T("\nm_strDirectory : ") << m_strDirectory;
	}

void COXUserTool::AssertValid() const
	{
	CObject::AssertValid();
	}
#endif


////////////////////////////////////////////////////////////////////////////////
//
// Public utility functions.
//
////////////////////////////////////////////////////////////////////////////////

BOOL ReadUserToolFile( LPCTSTR pcszFilename, CObArray& a )
	{
	ASSERT( pcszFilename != NULL && *pcszFilename != _T('\0') );
	
	if ( FileExists( pcszFilename ) )
		{
		TRY
			{
			CFile i( pcszFilename, CFile::modeRead | CFile::shareDenyNone );
			CArchive archive( &i, CArchive::load );
			INT_PTR iInitialSize = a.GetSize();
			a.Serialize( archive );
			return a.GetSize() > iInitialSize;
			}
		CATCH( CException, e )
			{
			TRACE(_T("ReadUserToolFile exception\n"));
			}
		END_CATCH
		}
	
	return FALSE;
	}

BOOL WriteUserToolFile( LPCTSTR pcszFilename, CObArray& a )
	{
	ASSERT( pcszFilename != NULL && *pcszFilename != _T('\0') );
	ASSERT( a.GetSize() > 0 );
	
	TRY
		{
		CFile o( pcszFilename, CFile::modeWrite | CFile::modeCreate | 
			CFile::shareExclusive );
		CArchive archive( &o, CArchive::store );
		a.Serialize( archive );
		return TRUE;
		}
	CATCH( CException, e )
		{
		TRACE(_T("WriteUserToolFile exception\n"));
		}
	END_CATCH
		
		return FALSE;
	}

BOOL AppendUserTools( CMenu* pMenu, UINT uiFirstID, const CObArray& a )
	{
	ASSERT( pMenu != NULL );
	ASSERT( uiFirstID > 0 );
	
	if ( pMenu != NULL && uiFirstID > 0 )
		{
		int iAdded = 0;
		UINT uiPos;
		UINT uiLastItem = pMenu->GetMenuItemCount() - 1;
		
		// If the menu item already exists, remove all subsequent
		// menu items before continuing.
		
		if ( FindMenuItem( pMenu, uiFirstID, &uiPos ) )
			{
			// Used to be UINT ui, but this caused an infinite loop when
			// uiPos was 0, the UINT ui can never become negative, so the test
			// ui >= uiPos was never FALSE
			for ( int i = uiLastItem; i >= (int)uiPos; i-- )
				pMenu->RemoveMenu( i, MF_BYPOSITION );
			
			uiLastItem = uiPos > 0 ? uiPos - 1 : 0;
			}
		
		// Continue on only if we actually have items to be added
		// to the menu.
		
		if ( a.GetSize() > 0  )
			{
			// Add a separator if the last item is not at the zero
			// position and is not already a separator.
			
			if ( uiLastItem > 0 && 
				!( pMenu->GetMenuState( uiLastItem, MF_BYPOSITION ) & MF_SEPARATOR ) )
				{
				pMenu->AppendMenu( MF_SEPARATOR );
				}
			
			// Append the list of tools to the menu.
			
			INT_PTR iNum = a.GetSize();
			for ( int i = 0; i < iNum; i++ )
				{
				COXUserTool* pTool = (COXUserTool*)a.GetAt( i );
				LPCTSTR pText = pTool->GetMenuText();
				
				if ( pMenu->AppendMenu( MF_STRING, uiFirstID + iAdded, pText ) )
					iAdded++;
				}
			}
		else if ( uiLastItem > 0 )
			{
			UINT uiState = pMenu->GetMenuState( uiLastItem, MF_BYPOSITION );
			if ( uiState & MF_SEPARATOR )
				pMenu->RemoveMenu( uiLastItem, MF_BYPOSITION );
			}
		
		return iAdded == a.GetSize();
		}
	
	return FALSE;	
	}

void EmptyUserToolArray( CObArray& a )
{
	INT_PTR iNum = a.GetSize();
	if ( iNum > 0 )
		{
		for ( int i = 0; i < iNum; i++ )
			delete (COXUserTool*)a.GetAt( i );
		a.RemoveAll();
		}
}

