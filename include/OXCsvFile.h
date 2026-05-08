// ==========================================================================
// 				Class Specification : 	  COXCsvFile
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

/////////////////////////////////////////////////////////////////////////////
/*

DESCRIPTION


  Hierarchy:
	CObject
	   CFile
	      CStdioFile
	         COXCsvFile

COXCsvFile extends the MFC class CStdioFile by adding functions for
processing the lines of a CStdioFile as rows in a comma delimited data
file. Each column, or field, of a row can be accessed independently.
When reading the data from the fields, the field name is used to determine
which field on a row to access. This allows a programmer to process
the fields irregardless of their order within the comma delimited file.
One rule for these files is that the fields have to be named. Preferably
the names of the fields will be given by the first row in the file when
it is being read in or alternately a programmer can assign them at run-time.


The reading/writing information from/to CSV file can be described in the 
following steps.


Reading from CSV file.

1)	Instantiate COXCsvFile by specifying path to the CSV file or pointer to 
	already opened FILE object as a parameter in the object's constructor

2)	If the CSV file contains the name of columns as one of its line (usually 
	it is the first line unless you are trying to read CSV file with multiple 
	tables) then initialize internal array of columns name using the following
	function:

	BOOL GetColumns();

	After columns are initialized you might retrieve the number of columns found 
	in the CSV using:

	int GetColumnCount() const;


	If the CSV file doesn't contain the name of columns then you have to 
	specify them explicitely using:

	void SetColumns();

3)	After column info has been gathered/set we can read the contents of the CSV
	file row by row using the following function:

	BOOL ReadLine();

	If this function succeed then we can retrieve data in particular column using
	set of ReadColumn() functions.

	Note, that COXCsvFile class allows you to read CSV files with multiple 
	tables. Refer to the CsvTest sample in the .\samples\database\CsvTest
	subdirectory for an example of reading CSV files with multiple tables.



Writing to CSV file.

1)	Instantiate COXCsvFile by specifying path to the CSV file or pointer to 
	already opened FILE object as a parameter in the object's constructor

2)	If you want the resulting CSV file to contain the name of columns as one of 
	its line (usually it is the first line unless you are trying to write CSV file 
	with multiple tables) then initialize internal array of columns name using the 
	following function:

	void SetColumns();

	Then you must call the following function in order to physically write the 
	headers in to the file:

	void WriteHeaders();

  	In the case you don't want to include headers you have to call the following 
	function that will prepare the CSV file for writing data:

	void Initialize();

3)	After the column info has been specified we can populate the CSV file with data.
	You can set data to particular column in the current row using set of 
	WriteColumn() functions.

	When you set data to every column you can call WriteLine() function in order to 
	save the current row to the CSV file.

	Note, that COXCsvFile class allows you to write CSV files with multiple 
	tables. Refer to the CsvTest sample in the .\samples\database\CsvTest
	subdirectory for an example of writing CSV files with multiple tables.


Refer to the COXCsvFile class reference for full list of public functions.


Dependency:
	#include "OXCsvFile.h"


Source code files:
	"OXCsvFile.cpp"



 */

#if !defined(__CSVFILE_H__)
#define	__CSVFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OXDllExt.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif



class OX_CLASS_DECL COXCsvFile : public CStdioFile
{
	DECLARE_DYNAMIC(COXCsvFile)

public:
	enum Errors {	errNone=0, errBadColumnIndex, errColName, 
					errNumericValue, errNotInSet, errIncompleteLine, 
					errTooManyColumns	};

	enum Types { tDefault=0, tByte, tShort, tLong, tFloat, tDouble, tString };
	
protected:

	// class COXColumnData
	// Define the data that the programmer has defined for each column, as
	// he is looking for them.
	class OX_CLASS_DECL COXColumnData
	{
	public:
		// the name the developer uses to access this column
		CString m_sName;	
		// the text representation of the data for this column
		CString m_strData;	
		// the expected data type for this column
		Types m_nType;	
		// TRUE if this string is to be quoted when written out
		BOOL m_bQuote;	

		 // --- In: 
		 // --- Out: 
		 // --- Returns: 
		 // --- Effect:	Initialize this object for use. Default contructor 
		 //				for the object that will contain the information for each
		 //				column/field of data in a row of the CSV file.
		COXColumnData()
		{
			m_nType=tDefault;
			m_bQuote=FALSE;
		}

		 // --- In:			columnData	-	constant reference to the column data 
		 //									object being copied
		 // --- Out: 
		 // --- Returns: 
		 // --- Effect: Copies the values from columnData to this COXColumnData object
		COXColumnData(const COXColumnData& columnData)
			: m_sName(columnData.m_sName), m_strData(columnData.m_strData),
			 m_nType(columnData.m_nType), m_bQuote(columnData.m_bQuote)
		{
		}
	};

	// class COXDataArray
	// Wrapper for a CArray containing COXColumnData objects.
	class OX_CLASS_DECL COXDataArray : public CArray<COXColumnData, COXColumnData&> 
	{
	};


protected:
	// number of columns in the file
	int m_nColumns;			
	// The data for the columns in this file
	COXDataArray m_arrColumns;		
	// the last error that was encountered
	mutable Errors m_nLastError;		
	// text describing each error
	static LPCTSTR m_pstrErrorMsgs[];	
	// TRUE if the last line read in was empty
	BOOL m_bLineEmpty;		
	// the character that separates fields in the file
	TCHAR m_tcFieldDelim;		
	// the character that deliniates a text string in the file
	TCHAR m_tcStringDelim;	

	// --- In:		lpszName	-	the name of a column that we are looking for.
	// --- Out: 
	// --- Returns:	An integer index of the column within the string arrays, or -1 if
	//				the specified name was not found.
	// --- Effect:	Converts the name of a column into the index of that column 
	//				in the string arrays
	int FindColumn(LPCTSTR lpszName) const;

	// --- In:		nError	-	the error number indicating what has gone wrong.
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Sets the number of the last error encountered in this file.
	inline void SetError(Errors nError) const { m_nLastError=nError; };
	
public:
	//
	// Constructors and Destructors
	//
	
	// --- In: 
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Initializes a COXCsvFile object. Default constructor.
	COXCsvFile();

	// --- In:		pFile	-	pointer to an already opened FILE object
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Setups the COXCsvFile object to use the passed open file handle
	//				for all reading and writing operations. Note, there is no 
	//				checks are done on file mode.
	COXCsvFile(FILE *pFile);

	// --- In:		lpszFileName	-	filename of the CSV file to open
	//				nOpenFlags		-	bit flags used to control how the file is 
	//									opened. These flags are defined in the 
	//									CFile constructor documentation.
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Setups this COXCsvFile object to operate on the specified file,
	//				opening that file for reading/writing, as specified. 
	//				Throws CFileException when an error occurres while trying to open 
	//				the specified file.
	COXCsvFile(LPCTSTR lpszFileName, UINT nOpenFlags);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// --- In: 
	// --- Out: 
	// --- Returns: An integer indicating how many columns are in the table 
	//				being processed.
	// --- Effect:	Retrieves the number of columns in the table currently 
	//				being processed
	inline int GetColumnCount() const {	return m_nColumns; }

	// --- In:		tcDelim	-	the character to be used to delimit fields 
	//							within a row.
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Set what character will be used to indicate when one field 
	//				has ended and another has begun. The default for this character 
	//				is the comma (,)
	inline void SetFieldDelimiter(TCHAR tcDelim) { m_tcFieldDelim=tcDelim; };

	// --- In:		tcDelim	-	the character to be used to delimit strings.
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Sets what character will be used to mark the beginning and 
	//				ending of a text string within a field. To delimit a string 
	//				within a field, this must be the first character in the field. 
	//				In order to include the string delimiter within the body of a 
	//				field, it must be escaped by enclosing the whole field as a 
	//				string, and replacing the occurrance(s) of the delimiter in 
	//				the field with two copies of the delimiter. This is handled 
	//				automatically by the COXCsvFile class when reading and writing 
	//				CSV files. The default for this character is the double quote (").
	inline void SetStringDelimiter(TCHAR tcDelim) { m_tcStringDelim=tcDelim; };

	// --- In: 
	// --- Out: 
	// --- Returns: The ID of the last error encountered on this file
	// --- Effect:	Retrieves the code for the last error that occured when 
	//				working with this file. These error codes are limited to 
	//				errors within the code that processes the comma delimited files, 
	//				and not general I/O errors.
	inline Errors GetLastError() const { return m_nLastError; };

	// --- In: 
	// --- Out: 
	// --- Returns: The description of the last error encountered in this file
	// --- Effect:	Retrieves the string describing the last error that was
	//				encountered while processing this file.
	inline LPCTSTR GetLastErrorMsg() const { return GetErrorMsg(GetLastError()); };

	// --- In:		nError	-	the error code that a textual description is 
	//							desired for.
	// --- Out: 
	// --- Returns: The description of the specified error.
	// --- Effect:	Retrieves the string describing the error code that is passed in.
	inline LPCTSTR GetErrorMsg(Errors nError) const { return m_pstrErrorMsgs[nError]; };
	


	//
	// Column management functions
	//

	// --- In:		nIndex		-	the index of the column to assign this 
	//								information to.
	//				lpszName	-	the name of the column. This will be the name
	//								written out by the WriteHeaders() funtion when
	//								writing a CSV file.
	//				nType		-	enumerated value indicating the type of data that
	//								is expected for this column.
	//				bQuote		-	TRUE if the string value is to be quoted on output.
	//								This value defaults to FALSE
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Sets the description of a column to the file being processed 
	//				by this object. The specified column will have a name associated 
	//				with it, as well as optional aliases to look for when reading 
	//				the file in, and the type of data expected in that column.
	void SetColumnInfo(int nIndex, LPCTSTR lpszName, Types nType, BOOL bQuote=FALSE);

	// --- In:		sName		-	the name associated with the specified aliases.
	//				arrAliases	-	a CStringArray containing the list of aliases
	//								that are possible for this column.
	//				lpstrAliases-	an array of sting pointers containing the list
	//							    of aliases that are possible for this column.
	//								The last pointer in this array must be a NULL
	//								pointer to mark the end of the list. 
	// --- Out: 
	// --- Returns: TRUE if the name was found, or one of the aliases was found
	//				and its name was changed. FALSE if neither the name nor any of
	//				the aliases was found.
	// --- Effect:	Assigns a set of alternative names that may be used to 
	//				define a column in a CSV file. This function should be called 
	//				after GetColumns() when reading in a CSV file. This will scan 
	//				the headers read in, checking first for a column that matches 
	//				sName parameter. If one is found, nothing more is done. 
	//				If not, it than searches for a column that matches one of the
	//				aliases. If one is found, that column's internal name will be 
	//				changed to sName for future reference.
	BOOL SetAliases(const CString& sName, const CStringArray& arrAliases);
	BOOL SetAliases(const CString& sName, LPCTSTR lpstrAliases[]);


	// --- In:		nColumns	-	the number of columns that will be used in 
	//								this file. If -1 specified then the number
	//								of columns will be calculated on the base
	//								of the file contents
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Prepares this CSV file object for reading/writing a new table 
	//				of values from/to the file. This would be used when column 
	//				headers are not used. This function will clear the column 
	//				information and data arrays
	void Initialize(int nColumns=-1);

	
	// --- In:		arrColumns	-	a CStringArray containing the names of the
	//								individual columns, in order.
	//				lpstrColumns-	an array of string pointers containing the
	//								names of the individual columns, in order. The
	//								last element of the array must be a NULL pointer.
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Sets the column names for the current CSV file. The names
	//				will be used when reading/writing records from/to the 
	//				COXCsvFile object.
	void SetColumns(const CStringArray& arrColumns);
	void SetColumns(LPCTSTR lpstrColumns[]);


	// --- In: 
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Writes out a line to the file containing the current set of 
	//				column names, all delimited as strings.
	void WriteHeaders();


	// --- In:		nNumExpected	-	the number of columns expected in the file.
	// --- Out: 
	// --- Returns:	TRUE if no errors occured while reading in the column headers,
	//				FALSE indicates that an error occured, and GetLastError() should
	//				be called to determine what that error was.
	// --- Effect:	Retrieves the column names from the file being read in by
	//				reading the next line from the file, and parse out the names 
	//				of the columns from that line.
	BOOL GetColumns(int nNumExpected);
	


	//
	// output functions
	//

	// --- In:		nColumn		-	the number of the column to assign the
	//								specified data to.
	//				lpszText	-	the text to assign to this column.
	//				bQuote		-	TRUE if the text string is to be quoted on 
	//								output.
	// --- Out: 
	// --- Returns: TRUE if the specified column was found, FALSE if the column
	//				was not found.
	// --- Effect:	Assigns a text string to a column in the current row when 
	//				writing out to a CSV file. The text in this column will be 
	//				quoted if either the quote parameter is TRUE, or if either 
	//				one of the delimiter characters is found in the text of the 
	//				string. The second version of this function will use
	//				column info that previously set using SetColumnInfo() function
	//				in order to retrieve the bQuote flag.
	BOOL WriteColumn(int nColumn, LPCTSTR lpszText, BOOL bQuote);
	BOOL WriteColumn(int nColumn, LPCTSTR lpszText);

	// --- In:		nColumn		-	the number of the column to assign the
	//								specified data to.
	//				ucData		-	the byte sized numeric value to be written out
	//				unData		-	the word sized numeric value to be written out.
	//				ulData		-	the unsigned long sized numeric value to be 
	//								written out.
	//				nData		-	the short integer value to be written out.
	//				lData		-	the long integer value to be written out.
	//				fData		-	the floating point value to be written out.
	//				dData		-	the double floating point value to be written out.
	//				bHex		-	TRUE if this value is to be written out in 
	//								hexidecimal format.
	// --- Out: 
	// --- Returns: TRUE if the specified column was found, FALSE if the column
	//				was not found.
	// --- Effect:	Assigns a numeric value (in different formats) to column. 
	//				The value will be written out as either a decimal or hexidecimal 
	//				number
	BOOL WriteColumn(int nColumn, unsigned char ucData, BOOL bHex=FALSE);
	BOOL WriteColumn(int nColumn, unsigned short unData, BOOL bHex=FALSE);
	BOOL WriteColumn(int nColumn, unsigned long ulData, BOOL bHex=FALSE);
	BOOL WriteColumn(int nColumn, short nData, BOOL bHex=FALSE);
	BOOL WriteColumn(int nColumn, long lData, BOOL bHex=FALSE);
	BOOL WriteColumn(int nColumn, float fData);
	BOOL WriteColumn(int nColumn, double dData);


	// --- In: 
	// --- Out: 
	// --- Returns: 
	// --- Effect:	Writes out the current set of values for the current line to 
	//				the CSV file. After writing out the values, the column values 
	//				stored in the class will be reset to empty strings in preperation 
	//				for the next line of data.
	void WriteLine();



	//
	// input functions
	//

	// --- In: 
	// --- Out: 
	// --- Returns: TRUE if the line was read in, and no errors occurred
	// --- Effect:	Reads in the next line of text from the CSV file, and break 
	//				it into individual columns in the COXCsvFile::m_arrColumns array. 
	//				If more columns of data are encountered in the current line 
	//				than we are looknig for (as specified by the call to 
	//				ReadColumns() function), a errTooManyColumns error will be
	//				generated. The data read will still be available, and 
	//				a programmer is free to ignore this error.
	//				If the line ends with a quoted string, but the closing quote 
	//				is not encountered, more lines will be read from the file and 
	//				appended to this line until the closing quote is found. This 
	//				allows for fields that contain line breaks, which is allowed 
	//				within many spreadsheet and database products that process CSV 
	//				files. After the line is successfully read in breaks it down into 
	//				the individual field values.
	BOOL ReadLine();


	// --- In: 
	// --- Out: 
	// --- Returns: TRUE if the just read line was empty.
	// --- Effect:	Indicates if the line that was just read contained nothing 
	//				but whitespace.
	inline BOOL IsLineEmpty() const { return m_bLineEmpty; };


	// --- In:		lpszName	-	the name of the column to get the data from
	//				nColumn		-	the index of the column to get the data from
	//				lpstrSet	-	the array of string pointers defining the names
	//								of the elements in the set to scan for a match to.
	//								The last element of this array must be a NULL.
	// --- Out:		strText		-	the CString object to return the text string in
	//				ucData		-	the byte variable the result is to be returned in
	//				unData		-	the word variable the result is to be returned in
	//				nDataIndex	-	the integer variable the index of the element 
	//								in lpstrSet array that matches the data in the 
	//								specified column is to be returned in
	//				nData		-	the short integer variable the result is to be 
	//								returned in
	//				lData		-	the long integer variable the result is to be 
	//								returned in
	//				fData		-	the floating point variable the result is to be 
	//								returned in
	//				dData		-	the double floating point variable the result 
	//								is to be returned in
	// --- Returns: TRUE if the specified column was found, FALSE if the column
	//				was not found.
	// --- Effect:	Retrieves the data from the specified column in the current line
	inline BOOL ReadColumn(LPCTSTR lpszName, CString& strText)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,strText) : FALSE);
	}
	BOOL ReadColumn(int nColumn, CString& strText);
	inline BOOL ReadColumn(LPCTSTR lpszName, unsigned char& ucData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,ucData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, unsigned char& ucData);
	inline BOOL ReadColumn(LPCTSTR lpszName, unsigned short& unData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,unData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, unsigned short& unData);
	inline BOOL ReadColumn(LPCTSTR lpszName, int& nDataIndex, LPCTSTR lpstrSet[])
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,nDataIndex,lpstrSet) : FALSE);
	}
	BOOL ReadColumn(int nColumn, int& nDataIndex, LPCTSTR lpstrSet[]);
	inline BOOL ReadColumn(LPCTSTR lpszName, short& nData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,nData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, short& nData);
	inline BOOL ReadColumn(LPCTSTR lpszName, long& lData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,lData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, long& lData);
	inline BOOL ReadColumn(LPCTSTR lpszName, float& fData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,fData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, float& fData);
	inline BOOL ReadColumn(LPCTSTR lpszName, double& dData)
	{
		int nIndex=FindColumn(lpszName);
		return ((nIndex>=0) ? ReadColumn(nIndex,dData) : FALSE);
	}
	BOOL ReadColumn(int nColumn, double& dData);
};

#endif
