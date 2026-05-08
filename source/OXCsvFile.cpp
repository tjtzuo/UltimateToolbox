// ==========================================================================
// 				Class Implementation : 	  COXCsvFile
// ==========================================================================

// Header file : OXCsvFile.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <limits.h>
#include <float.h>
#include "OXCsvFile.h"

#include "UTBStrOp.h"
#include "UTB64Bit.h"

// _ttoi64 is incorrectly defined in TCHAR.H, so you need to undefine
// it and redefine it correctly to avoid a compile error when
// building a non-Unicode version of your application.   
#ifdef _UNICODE
#ifdef _ttoi64
#undef _ttoi64
#define _ttoi64		_wtoi64
#endif
#else
#ifdef _ttoi64
#undef _ttoi64
#define _ttoi64		_atoi64
#endif
#endif

//
// Internal constants
//
const TCHAR tcZero=_T('0');
const TCHAR tcNulc=_T('\0');

/**
static strip(CString& text)

	This function takes the passed in CString object, and removes
any leading or trailing whitespace characters. The CString is
modified directly, returning a string that either starts and 
ends with non-whitepace characters, or an empty string.

--- In      : strText : Reference to the CString that is to be stripped.
--- Out     : strText : The string, with all leading and trailing whitespace
						   removed.
--- Returns : 
--- Effect  : 
**/
static void inline strip(CString& strText)
{
	strText.TrimRight();
	strText.TrimLeft();
}

IMPLEMENT_DYNAMIC(COXCsvFile, CStdioFile)

/**
Textual error messages for reporting the error to a user.
**/
LPCTSTR	COXCsvFile::m_pstrErrorMsgs[]={
	_TEXT("No Error"),
	_TEXT("Invalid column index was specified"),
	_TEXT("Invalid column name was specified"),
	_TEXT("Column contained an invalid numeric value"),
	_TEXT("Column data did not match data in set"),
	_TEXT("Line was too long"),
	_TEXT("Line contained too many records"),
};

COXCsvFile::COXCsvFile()
	: CStdioFile(), m_nColumns(0), m_nLastError(errNone), m_bLineEmpty(FALSE),
	m_tcFieldDelim(_T(',')), m_tcStringDelim(_T('\"'))
{
}

COXCsvFile::COXCsvFile(FILE *pFile)
	: CStdioFile(pFile), m_nColumns(0), m_nLastError(errNone), m_bLineEmpty(FALSE),
	m_tcFieldDelim(_T(',')), m_tcStringDelim(_T('\"'))
{
}

COXCsvFile::COXCsvFile(LPCTSTR lpszFileName, UINT nOpenFlags)
	: CStdioFile(lpszFileName,nOpenFlags), m_nColumns(0), m_nLastError(errNone),
	m_bLineEmpty(FALSE), m_tcFieldDelim(_T(',')), m_tcStringDelim(_T('\"'))
{
}

#ifdef _DEBUG
void COXCsvFile::AssertValid() const
{
	CStdioFile::AssertValid();
	ASSERT(m_nColumns==m_arrColumns.GetSize());
	ASSERT(m_tcFieldDelim!=tcNulc);
	ASSERT(m_tcStringDelim!=tcNulc);
}

void COXCsvFile::Dump(CDumpContext& dc) const
{
	CStdioFile::Dump(dc);
	dc << _T("Number of Columns: ") << m_nColumns << _T("\n");
	dc << _T("Last Error: (") << GetLastError() << _T(") ") << GetLastErrorMsg() << _T("\n");
	if(IsLineEmpty())
	{
		dc << _T("The current line is empty\n");
	}
	dc << _T("Field Delimiter: '") << m_tcFieldDelim << _T("'\n");
	dc << _T("String Delimiter: '") << m_tcStringDelim << _T("'\n");
}
#endif

/////////////////////////////////
// Column management functions //
/////////////////////////////////
void COXCsvFile::SetColumnInfo(int nIndex, LPCTSTR lpszName, Types nType, BOOL bQuote)
{
	ASSERT(nIndex>=0 && nIndex<m_nColumns);

	if(nIndex>=0 && nIndex<m_nColumns)
	{
		COXColumnData& data=m_arrColumns[nIndex];
		data.m_sName=lpszName;
		data.m_nType=nType;
		data.m_bQuote=bQuote;
	}
}

BOOL COXCsvFile::SetAliases(const CString& sName, const CStringArray& arrAliases)
{
	if(FindColumn(sName)!=1)
	{
		//
		// The original name was found, no need to look any further
		//
		return TRUE;
	}

	//
	// The original name was not found, look for the aliases
	//
	int nIndex;
	int nAlias;
	int nCount=PtrToInt(arrAliases.GetSize());

	for(nAlias=0; nAlias<nCount; ++nAlias)
	{
		nIndex=FindColumn(arrAliases[nAlias]);
		if(nIndex!=-1)
		{
			//
			// We have found an alias, rename this column and return
			//
			m_arrColumns[nIndex].m_sName=sName;
			return TRUE;
		}
	}

	//
	// No luck finding the original name, or any of its aliases
	//
	return FALSE;
}	

BOOL COXCsvFile::SetAliases(const CString& sName, LPCTSTR lpstrAliases[])
{
	if(FindColumn(sName)!=-1)
	{
		//
		// The original name was found, no need to look any further
		//
		return TRUE;
	}

	//
	// The original name was not found, look for the aliases
	//
	int nIndex;
	int nAlias;

	for(nAlias=0; lpstrAliases[nAlias]!=NULL; ++nAlias)
	{
		nIndex=FindColumn(lpstrAliases[nAlias]);
		if(nIndex!=-1)
		{
			//
			// We have found an alias, rename this column and return
			//
			m_arrColumns[nIndex].m_sName=sName;
			return TRUE;
		}
	}

	//
	// No luck finding the original name, or any of its aliases
	//
	return FALSE;
}	

void COXCsvFile::SetColumns(LPCTSTR lpstrColumns[])
{
	int	nIndex, nCount;

	SetError(errNone);
	
	if(lpstrColumns==NULL)
	{
		//
		// Base the number of columns off of the first line of the table
		// read in. Do not use headers.
		//
		nCount=-1;
	}
	else
	{
		//
		// determine how many headers there are
		//
		for(nCount=0; lpstrColumns[nCount]!=NULL; ++nCount)
			;
	}
	
	//
	// store the header information
	//
	m_arrColumns.RemoveAll();
	if(nCount>0)
		m_arrColumns.SetSize(nCount);
	m_nColumns=nCount;
	
	for(nIndex=0; nIndex<nCount; ++nIndex)
	{
		m_arrColumns[nIndex].m_sName=lpstrColumns[nIndex];
	}
}

void COXCsvFile::Initialize(int nColumns)
{
	SetError(errNone);
	
	m_arrColumns.RemoveAll();
	if(nColumns>0)
		m_arrColumns.SetSize(nColumns);
	m_nColumns=nColumns;
}

void COXCsvFile::SetColumns(const CStringArray& arrColumns)
{
	int	nIndex;
	
	SetError(errNone);
	
	//
	// store the header information
	//
	m_nColumns=PtrToInt(arrColumns.GetSize());
	m_arrColumns.RemoveAll();
	m_arrColumns.SetSize(m_nColumns);
	
	for(nIndex=0; nIndex<m_nColumns; ++nIndex)
	{
		m_arrColumns[nIndex].m_strData=arrColumns[nIndex];
	}
}

void COXCsvFile::WriteHeaders()
{
	int	nColumn;
	
	SetError(errNone);
	
	for(nColumn=0; nColumn<m_nColumns; ++nColumn)
	{
		WriteColumn(nColumn,m_arrColumns[nColumn].m_sName,TRUE);
	}
	WriteLine();
}

BOOL COXCsvFile::GetColumns(int nNumExpected)
{
	m_arrColumns.RemoveAll();
	m_arrColumns.SetSize(nNumExpected);
	m_nColumns=nNumExpected;
	
	if(ReadLine() || GetLastError()==errTooManyColumns)
	{
		if(m_nColumns<m_arrColumns.GetSize())
			m_nColumns= PtrToInt(m_arrColumns.GetSize());

		for(int nColumn=0; nColumn<m_nColumns; ++nColumn)
		{
			m_arrColumns[nColumn].m_sName=m_arrColumns[nColumn].m_strData;
			m_arrColumns[nColumn].m_strData.Empty();
		}
		return m_nColumns==nNumExpected;
	}

	return FALSE;
}
	
//
// output functions
//
BOOL COXCsvFile::WriteColumn(int nColumn, LPCTSTR lpszText)
{
	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}
	return WriteColumn(nColumn, lpszText, m_arrColumns[nColumn].m_bQuote);
}	

BOOL COXCsvFile::WriteColumn(int nColumn, LPCTSTR lpszText, BOOL bQuote)
{
	CString	strSpecialChars;	// the field and string delimiters, and line break characters

	strSpecialChars=m_tcFieldDelim;
	strSpecialChars+=m_tcStringDelim;
	strSpecialChars+=_T("\r\n");

	SetError(errNone);
	
	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	//
	// Important: The string needs to be quoted if one of the following conditions 
	// is met:
	//	1. The string contains one of the following characters:
	//		a. the field delimiter character
	//		b. the string delimiter character
	//		c. carraige return
	//		d. line feed
	//	2. The string begins with a whitespace character
	//	3. The string ends with a whitespace character
	//
	if(!bQuote
		&& _tcscspn(lpszText,strSpecialChars)>=_tcslen(lpszText)
		&& !_istspace(lpszText[0])
		&& !_istspace(lpszText[_tcslen(lpszText)-1]))
	{
		m_arrColumns[nColumn].m_strData=lpszText;
	}
	else
	{
		CString	strItem(m_tcStringDelim);
		for(int nIndex=0; lpszText[nIndex]!=tcNulc; ++nIndex)
		{
			if(lpszText[nIndex]==m_tcStringDelim)
			{
				strItem+=m_tcStringDelim;
				strItem+=m_tcStringDelim;
			}
			else
			{
				strItem+=lpszText[nIndex];
			}
		}
		strItem+=m_tcStringDelim;
		
		m_arrColumns[nColumn].m_strData=strItem;
		m_arrColumns[nColumn].m_nType=tString;
	}

	return TRUE;
}

inline TCHAR toHex(int nDigit)
{
	ASSERT(nDigit>=0 && nDigit<16);

	return (TCHAR)((nDigit>=10) ? _T('a')+nDigit-10 : tcZero+nDigit);
}

BOOL COXCsvFile::WriteColumn(int nColumn, unsigned char ucData, BOOL bHex)
{
	TCHAR text[5];
	
	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	if(bHex)
	{
		//
		// generate a hexidecimal string
		//
		text[0]=tcZero;
		text[1]=_T('x');
		text[2]=toHex((ucData >> 4) & 0x0f);
		text[3]=toHex(ucData & 0x0f);
		text[4]=tcNulc;
	}
	else
	{
		if(ucData>=100)
		{
			//
			// generate a 3 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(ucData/100));
			text[1]=(TCHAR)(tcZero+((ucData%100)/10));
			text[2]=(TCHAR)(tcZero+(ucData%10));
			text[3]=tcNulc;
		}
		else if(ucData>=10)
		{
			//
			// generate a 2 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(ucData/10));
			text[1]=(TCHAR)(tcZero+(ucData%10));
			text[2]=tcNulc;
		}
		else
		{
			//
			// generate a 1 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+ucData);
			text[1]=tcNulc;
		}
	}

	m_arrColumns[nColumn].m_strData=text;
	m_arrColumns[nColumn].m_nType=tByte;

	return TRUE;
}

BOOL COXCsvFile::WriteColumn(int nColumn, unsigned short unData, BOOL bHex)
{
	TCHAR text[7];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	if(bHex)
	{
		//
		// generate a hexidecimal value
		//
		text[0]=tcZero;
		text[1]=_T('x');
		text[2]=toHex((unData >> 12) & 0x0f);
		text[3]=toHex((unData >> 8) & 0x0f);
		text[4]=toHex((unData >> 4) & 0x0f);
		text[5]=toHex(unData & 0x0f);
		text[6]=tcNulc;
	}
	else
	{
		if(unData>=10000)
		{
			//
			// generate a 5 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(unData/10000));
			text[1]=(TCHAR)(tcZero+((unData%10000)/1000));
			text[2]=(TCHAR)(tcZero+((unData%1000)/100));
			text[3]=(TCHAR)(tcZero+((unData%100)/10));
			text[4]=(TCHAR)(tcZero+(unData%10));
			text[5]=tcNulc;
		}
		else if(unData>=1000)
		{
			//
			// generate a 4 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(unData/1000));
			text[1]=(TCHAR)(tcZero+((unData%1000)/100));
			text[2]=(TCHAR)(tcZero+((unData%100)/10));
			text[3]=(TCHAR)(tcZero+(unData%10));
			text[4]=tcNulc;
		}
		else if(unData>=100)
		{
			//
			// generate a 3 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(unData/100));
			text[1]=(TCHAR)(tcZero+((unData%100)/10));
			text[2]=(TCHAR)(tcZero+(unData%10));
			text[3]=tcNulc;
		}
		else if(unData>=10)
		{
			//
			// generate a 2 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+(unData/10));
			text[1]=(TCHAR)(tcZero+(unData%10));
			text[2]=tcNulc;
		}
		else
		{
			//
			// generate a 1 digit decimal string
			//
			text[0]=(TCHAR)(tcZero+unData);
			text[1]=tcNulc;
		}
	}
	
	m_arrColumns[nColumn].m_strData=text;
	m_arrColumns[nColumn].m_nType=tShort;

	return TRUE;
}

BOOL COXCsvFile::WriteColumn(int nColumn, unsigned long ulData, BOOL bHex)
{
	TCHAR text[20];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	if(bHex)
	{
		UTBStr::stprintf(text, 20, _T("0x%08x"), ulData);
	}
	else
	{
		UTBStr::stprintf(text, 20, _T("%d"), ulData);
	}
	
	m_arrColumns[nColumn].m_strData=text;
	m_arrColumns[nColumn].m_nType=tLong;

	return TRUE;
}

BOOL COXCsvFile::WriteColumn(int nColumn, short nData, BOOL bHex)
{
	TCHAR text[20];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	if(bHex)
	{
		UTBStr::stprintf(text, 20, _T("0x%04x"), nData);
	}
	else
	{
		UTBStr::stprintf(text, 20, _T("%d"), nData);
	}
	
	m_arrColumns[nColumn].m_strData=text;
	m_arrColumns[nColumn].m_nType=tShort;

	return TRUE;
}	

BOOL COXCsvFile::WriteColumn(int nColumn, long lData, BOOL bHex)
{
	TCHAR text[20];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	if(bHex)
	{
		UTBStr::stprintf(text, 20, _T("0x%08x"), lData);
	}
	else
	{
		UTBStr::stprintf(text, 20, _T("%d"), lData);
	}
	
	m_arrColumns[nColumn].m_strData=text;
	m_arrColumns[nColumn].m_nType=tLong;

	return TRUE;
}	

BOOL COXCsvFile::WriteColumn(int nColumn, float fData)
{
	TCHAR text[20];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	UTBStr::stprintf(text, 20, _T("%f"), fData);

	//
	// Use the string write column in case quoting is needed. Some nations use comma
	// as the decimal character, and also to account for the variable delimiter characters
	// that might be used.
	//
	WriteColumn(nColumn, text);
	m_arrColumns[nColumn].m_nType=tFloat;

	return TRUE;
}	

BOOL COXCsvFile::WriteColumn(int nColumn, double dData)
{
	TCHAR text[20];

	if(nColumn<0 || nColumn>=m_nColumns)
	{
		SetError(errBadColumnIndex);
		return FALSE;
	}

	UTBStr::stprintf(text, 20, _T("%f"), dData);
	
	//
	// Use the string write column in case quoting is needed. Some nations use comma
	// as the decimal character, and also to account for the variable delimiter characters
	// that might be used.
	//
	WriteColumn(nColumn, text);
	m_arrColumns[nColumn].m_nType=tDouble;

	return TRUE;
}	

void COXCsvFile::WriteLine()
{
	CString	strLine;
	//
	// Run through the columns, adding each of them to the line for output to the file.
	// Clear the data value for each column after it has been added.
	//
	for(int nColumn=0; nColumn<m_nColumns; ++nColumn)
	{
		COXColumnData &data=m_arrColumns[nColumn];
		if(!data.m_strData.IsEmpty())
		{
			if(data.m_nType==tFloat || data.m_nType==tDouble)
			{
				//
				// Special check-look for field delimiter. This is because some 
				// nations use commas where the US uses decimal points.
				//
				if(data.m_strData.Find(m_tcFieldDelim)!=-1
					|| data.m_strData.Find(m_tcStringDelim)!=-1)
				{
					//
					// Need to quote this value, write it out as if it were a string
					//
					CString tmp=data.m_strData;
					Types nType=data.m_nType;

					WriteColumn(nColumn, tmp, TRUE);
					data.m_nType=nType;
					data.m_strData=tmp;
				}
			}
			strLine+=data.m_strData;
		}
		if(nColumn!=m_nColumns-1)
			strLine+=m_tcFieldDelim;
		
		//
		// clear out the data for the next line
		//
		m_arrColumns[nColumn].m_strData.Empty();
	}
	strLine+="\n";
	WriteString(strLine);
}
	
/////////////////////
// input functions //
/////////////////////
BOOL COXCsvFile::ReadLine()
{
	CString	strLine;
	BOOL bRetval=FALSE;
	
	SetError(errNone);
	m_bLineEmpty=TRUE;
	
	//
	// clear out previous line's data
	//
	int nIndex=0;
	for(nIndex=0; nIndex<m_arrColumns.GetSize(); ++nIndex)
		m_arrColumns[nIndex].m_strData.Empty();
	
	if(ReadString(strLine))
	{
		//
		// Append a newline character to mark the end of the line, as this is removed
		// by ReadString.
		//
		strLine+=TEXT("\n");

		//
		// get the length of the line that has been read in
		//
		int	nLength=strLine.GetLength();
		
		//
		// begin processing the line
		//
		int nColumn=0;
		BOOL bInQuote=FALSE;
		CString	strText;
		CString	strBlanks;
		TCHAR tcNext;
		
		//
		// Loop through the characters in the line, parsing out the individual fields
		//
		for(nIndex=0; nIndex<nLength; ++nIndex)
		{
			tcNext=strLine[nIndex];
			if(tcNext==m_tcStringDelim)
			{
				//
				// A quoted string
				//
				if(!strBlanks.IsEmpty())
				{
					strText+=strBlanks;
					strBlanks.Empty();
				}
				bInQuote=TRUE;

				//
				// loop through looking for the end of the quoted string
				//
				for(++nIndex; bInQuote && nIndex<nLength; ++nIndex)
				{
					tcNext=strLine[nIndex];
					if(tcNext==m_tcStringDelim)
					{
						//
						// End of string or inset the delimiter into the field?
						//
						if(strLine[nIndex+1]==m_tcStringDelim)
						{
							//
							// place a quote into the string
							//
							strText+=tcNext;
							++nIndex;
						}
						else
						{
							//
							// reached the end of the string
							//
							--nIndex;
							bInQuote=FALSE;
						}
					}
					else if(tcNext==_T('\n'))
					{
						strText+=tcNext;

						//
						// The end of the line has been reached, while within
						// a quote, need to get the next line
						//
						if(ReadString(strLine))
						{
							nIndex=-1;
							strLine+=_TEXT("\n");
							nLength=strLine.GetLength();
						}
						else
						{
							//
							// This line was incomplete
							//
							SetError(errIncompleteLine);
							nIndex=strLine.GetLength();
						}
					}
					else
					{
						strText+=tcNext;
					}
				}
				//inQuote=FALSE;
			}
			else if(tcNext==_T('\n'))
			{
				//
				// we have reached the end of the line, and therefore an item
				//
				nIndex=strLine.GetLength();

				if(nColumn==m_nColumns)
					SetError(errTooManyColumns);
				if(nColumn>=m_arrColumns.GetSize())
				{
					m_arrColumns.SetSize(nColumn+1);
					m_nColumns=nColumn+1;
				}
				if (nColumn)
					m_bLineEmpty=FALSE;
				else
					m_bLineEmpty=TRUE; 
				m_arrColumns[nColumn].m_strData=strText;
				++nColumn;
				strBlanks.Empty();
				strText.Empty();
			}
			else if(tcNext==m_tcFieldDelim)
			{
				//
				// We have reached the end of an item
				//
				if(nColumn==m_nColumns)
					SetError(errTooManyColumns);
				if(nColumn>=m_arrColumns.GetSize())
				{
					m_arrColumns.SetSize(nColumn+1);
					m_nColumns=nColumn+1;
				}

				m_bLineEmpty=FALSE;
				m_arrColumns[nColumn].m_strData=strText;
				++nColumn;
				
				strBlanks.Empty();
				strText.Empty();
			}
			else if(_istspace(tcNext))
			{
				if(!strText.IsEmpty())
				{
					strBlanks+=tcNext;
				}
			}
			else
			{
				if(!strBlanks.IsEmpty())
				{
					strText+=strBlanks;
					strBlanks.Empty();
				}
				strText+=tcNext;
			}
		}
		if(!strText.IsEmpty())
		{
			if(nColumn==m_nColumns)
				SetError(errTooManyColumns);
			if(nColumn>=m_arrColumns.GetSize())
			{
				m_arrColumns.SetSize(nColumn+1);
				m_nColumns=nColumn+1;
			}
			m_arrColumns[nColumn].m_strData=strText;
			m_bLineEmpty=FALSE;
		}

		if(m_nColumns==-1)
		{
			m_nColumns=nColumn;
			if(GetLastError()==errTooManyColumns)
				SetError(errNone);
		}

		bRetval=(GetLastError()==errNone);
	}
	
	return bRetval;
}

int COXCsvFile::FindColumn(LPCTSTR lpszName) const
{
	int	nIndex;
	
	for(nIndex=0; nIndex<m_nColumns; ++nIndex)
	{
		if(m_arrColumns[nIndex].m_sName.CompareNoCase(lpszName)==0)
			return nIndex;
	}
	
	SetError(errColName);
	return -1;
}

BOOL COXCsvFile::ReadColumn(int nColumn, CString& strText)
{
	strText.Empty();
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		strText=m_arrColumns[nColumn].m_strData;
		return TRUE;
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}

BOOL COXCsvFile::ReadColumn(int nColumn, unsigned char& ucData)
{
	ucData=0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		int		nValue=0;
		CString	text=m_arrColumns[nColumn].m_strData;

		nValue=_ttoi(text);
		
		if(nValue>=0 && nValue<=UCHAR_MAX)
		{
			ucData=(unsigned char)nValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}

BOOL COXCsvFile::ReadColumn(int nColumn, unsigned short& unData)
{
	unData=0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		long	lValue=0;
		CString	text=m_arrColumns[nColumn].m_strData;

		lValue=_ttol(text);

		if(lValue>=0 && lValue<=USHRT_MAX)
		{
			unData=(unsigned short)lValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}

BOOL COXCsvFile::ReadColumn(int nColumn, int& nDataIndex, LPCTSTR lpstrSet[])
{
	CString	strText, strItem;
	int nIndex;

	nDataIndex=0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		strText=m_arrColumns[nColumn].m_strData;
		strText.MakeLower();
		
		for(nIndex=0; lpstrSet[nIndex]!=NULL; ++nIndex)
		{
			strItem=lpstrSet[nIndex];
			strItem.MakeLower();
			if(strText==strItem)
			{
				nDataIndex=nIndex;
				return TRUE;
			}
		}
		SetError(errNotInSet);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}

BOOL COXCsvFile::ReadColumn(int nColumn, short& nData)
{
	nData=0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		long	nValue=0;
		CString	text=m_arrColumns[nColumn].m_strData;

		nValue=_ttol(text);

		if(nValue>=SHRT_MIN && nValue<=SHRT_MAX)
		{
			nData=(short)nValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}	

BOOL COXCsvFile::ReadColumn(int nColumn, long& lData)
{
	lData=0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		__int64	lValue=0;
		CString	text=m_arrColumns[nColumn].m_strData;

		lValue=_ttoi64(text);

		if(lValue>=LONG_MIN && lValue<=LONG_MAX)
		{
			lData=(long)lValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}	

BOOL COXCsvFile::ReadColumn(int nColumn, float& fData)
{
	fData=0.0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		double	fValue=0;
		LPTSTR	ptr;

		CString	text=m_arrColumns[nColumn].m_strData;

		fValue=_tcstod(text, &ptr);

		if(fValue>=(1-FLT_MAX) && fValue<=FLT_MAX)
		{
			fData=(float)fValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}

	return FALSE;
}	

BOOL COXCsvFile::ReadColumn(int nColumn, double& dData)
{
	dData=0.0;
	if(nColumn>=0 && nColumn<m_arrColumns.GetSize())
	{
		double	fValue=0;
		LPTSTR	ptr;
		CString	text=m_arrColumns[nColumn].m_strData;

		fValue=_tcstod(text, &ptr);

		if(fValue>=(1-DBL_MAX) && fValue<=DBL_MAX)
		{
			dData=(double)fValue;
			return TRUE;
		}
		SetError(errNumericValue);
	}
	else
	{
		SetError(errBadColumnIndex);
	}
	
	return FALSE;
}	
