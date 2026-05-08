// ==========================================================================
// 							Class Specification : COXDirSpec
// ==========================================================================

// Header file : dir.h

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
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class encapsulates the directory specification on Windows NT
//		supported file systems (FAT, VFAT, NTFS, HPFS)
//		A directory consists of a drive specification, followed a subdirectory

// Remark:
//		Directory specifications are always represented in upper case characters
//		 An implicit conversion is done when the supplied names are in lower case;
//		All member functions starting with "Do" will physically change something
//		 on disk.  Other functions only change the internal data.
//		E.g. The normal destructor only destroys the internal representation,
//		 to actually remove a directory use DoRemove()

// Prerequisites (necessary conditions):
//		
// Limited edition:
//		In the not registered version (#define NOT_REGISTERED)
// 		a limited implementation of COXDirSpec is provided
// 		The function DoCopyDirectory() is not implemented

/////////////////////////////////////////////////////////////////////////////
#ifndef __DIR_H__
#define __DIR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// #define NOT_REGISTERED 1

#ifndef NOT_REGISTERED
class COXCopyStatusDialog;
#endif // !NOT_REGISTERED


class OX_CLASS_DECL COXDirSpec : public CObject
{
DECLARE_SERIAL(COXDirSpec)

// Data members -------------------------------------------------------------
public:
	
protected:
	CString 	m_sDrive;
	CString		m_sSubdirectory;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXDirSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

    COXDirSpec(LPCTSTR pszDirectory);
	// --- In  : pszDirectory : The new drive and directory of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : 
	// --- Effect : Contruction of an object together with a SetDirectory()
	//				Notice that whether the directory specification is valid or not
	//				cannot be checked in this way

	#ifdef WIN32
	BOOL IsChildDir(LPWIN32_FIND_DATA lpFindFileData) const;
	#endif
	
    COXDirSpec(const COXDirSpec& dirSrc);
	// --- In :	dirSrc : Directory object which will be copied
	// --- Out : 
	// --- Returns :
	// --- Effect : Copy contruction.
	
    BOOL AppendDirectory(const COXDirSpec& dirSecond);
	// --- In :	dirSecond : The directory object which will be appendec to the object
	// --- Out : 
	// --- Returns : Whether drive and directory specification have a correct format
	// --- Effect : The drive specification of the second directory specification
	//				must be empty or the same as that of this object.
	//				The subdirectory of the second dir spec may not start with
	//				back slash.
	//				It is interpreted as a subdirectory beneath the subdirectory
	//				of this object
	
	COXDirSpec& operator=(const COXDirSpec& dirSrc);
	// --- In :	dirSrc : Directory object which will be assign to 'this' directory object
	// --- Out:
	// --- Returns: 
	// --- Effect : Assignment operator
	
	CString GetDrive() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The drive specification of this object
	//			 	It consists of a drive letter and a colon
	//			 	or an empty string
	// --- Effect : 
	BOOL SetDrive(LPCTSTR pszDrive);
	// --- In  : pszDrive : The new drive letter of the object
	// --- Out : 
	// --- Returns : Whether drive specification has a correct format
	//				 Whether it actually exitst is not checked
	// --- Effect : 
	void ForceSetDrive(LPCTSTR pszDrive);
	// --- In  : pszDrive : The new drive letter of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	CString GetSubdirectory() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The subdirectory specification of this object
	//			 	It only ends in a back slash when it is the root directory
	//			 	The result may be an empty string
	// --- Effect : 
	BOOL SetSubdirectory(LPCTSTR pszSubdirectory);
	// --- In  : pszSubdirectory : The new subdirectory of the object
	// --- Out : 
	// --- Returns : Whether subdirectory specification has a correct format
	//				 Wild characters are not allowed
	//				 Whether it actually exitst is not checked
	// --- Effect : 
	void ForceSetSubdirectory(LPCTSTR pszSubdirectory);
	// --- In  : pszSubdirectory : The new subdirectory of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	COXDirSpec GetLastSubdirectory() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The last subdirectory specification of this object
	//				Several subdirectories can be seperated by black slashes,
	//				only the last one is returned
	//				(together with the drive specification)
	//			 	It only ends in a back slash when it is the root directory
	// --- Effect : 
	void RemoveLastSubdirectory();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Removes the last subdirectory specification of this object
	//				Several subdirectories can be seperated by black slashes,
	//				only the last one is removed
	//			 	The root directory is never removed

	CString GetDirectory() const;
	// --- In  :
	// --- Out : 
	// --- Returns :The drive and directory specification of this object
	//			 	It only ends in a back slash when it is the root directory
	//			 	The result may be an empty string
	// --- Effect : 
	BOOL SetDirectory(LPCTSTR pszDirectory);
	// --- In  : pszDirectory : The new drive and directory of the object
	// --- Out : 
	// --- Returns : Whether drive and directory specification have a correct format
	//				 Whether it actually exitst is not checked
	// --- Effect : 
	void ForceSetDirectory(LPCTSTR pszDirectory);
	// --- In  : pszDirectory : The new drive and directory of the object
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function extracts illegal characters and thus
	//				will always succeeds
	
	CString GetFileSystemType();
	// --- In  : 
	// --- Out : 
	// --- Returns : The name identifying the file system of this drive
	//				 When the drive spec is not specified, an empty string
	//				 is returned
	// --- Effect : Determines the file system.  
	//				Possible values are (among others):
	//				FAT :  File Allocation Table
	//				HPFS : High Performance File System
	//				NTFS : New Technology File Sytem
	//				CDFS : Compact Disk File System
	//				Notice that WIN16 can only detect FAT-systems

	BOOL MakeTemp();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the unique directory could be determined
	// --- Effect : Replaces the directory specification by that of the
	//				temporary directory
	
	BOOL MakeUnique();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether a unique name could be made
	// --- Effect : Replaces the directory specification by another 
	//				directory specification	that does not yet exist on disk
	//				So this can be used to make unique directory names
	
	BOOL MakeLargestExisting();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether a unique name could be made
	// --- Effect : Removes subdirectories from the directory specification 
	// 				until a directory specification is found that actually exists on disk
	//				(This will become the current directory)
	//				The drive spec is never changed
	//				The resulting dir spec is an absolute spec

	BOOL Exists() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the directory (not a file!) physically exists on disk
	// --- Effect :
	
	BOOL IsEmpty() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the directory specification is empty
	// --- Effect :

	void Empty();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : CLears the directory specification
	
	BOOL IsEmptyDir() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the directory is empty = contains no files or other subdirs
	// --- Effect : 
 

	UINT DoGetDriveType();
	// --- In  :
	// --- Out : 
	// --- Returns :the type of drive
	// --- Effect : Examines the type of drive associated with this dirspec

	CString DoGetVolumeInformation();
	// --- In  :
	// --- Out : 
	// --- Returns :The volume name of the dirspec. if it fails it returns an empty string
	// --- Effect : 

	DWORD DoGetDiskFreeSpace();
	// --- In  :
	// --- Out : 
	// --- Returns :Free disk space
	// --- Effect : Calculates the free disk space on the drive associated with this dirspec

	BOOL DoGetCurrentDir();
	// --- In  :
	// --- Out : 
	// --- Returns :Whether it succeeded or not
	// --- Effect : Copies the current DOS drive and directory to the object

	BOOL DoSetCurrentDir() const;	
	// --- In  :
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Makes the direcory specified by this object the current
	//				directory
	
	BOOL DoMakeNew() const;
	// --- In  :
	// --- Out : 
	// --- Returns :Whether it succeeded or not
	// --- Effect : Makes a new subdirectory
	//				Several directories can be created in one call

#ifndef NOT_REGISTERED
	BOOL DoCopyDirectory(COXDirSpec DestDirectory, BOOL bOnlyContents = TRUE,
		BOOL bCleanUp  = FALSE, COXCopyStatusDialog* pCpyStatDlg = NULL);
	// --- In  : DestDirectory : the directory to copy to
	//			 bOnlyContents : whether to copy only the contents or also the source dir
	//			 pCpyStatDlg : pointer to Status dlg. MUST ONLY BE ALLOCATED(new) AND
	//						   NOT YET CREATED!!!!!
	// --- Out : 
	// --- Returns :Whether it succeeded or not
	// --- Effect : Copioes the entire contents(files + subdirs) in this dirspec
	//				to another directory
#endif // !NOT_REGISTERED

	BOOL DoRemove(BOOL bRecursively = FALSE, BOOL bAlsoRemoveReadOnly = FALSE) const;	
	// --- In  : bRecursively : Whether all underlying subdirectories
	//		    				sould be deleted first
	//			 bAlsoRemoveReadOnly : Whether to also remove files that are 
	//							marked as read only (TRUE) or not (FALSE)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Removes the directory

	virtual void Serialize( CArchive& archive );
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Read or write to archive

	BOOL operator==(const COXDirSpec& dirSpec) const;
	BOOL operator!=(const COXDirSpec& dirSpec) const;
	BOOL operator<=(const COXDirSpec& dirSpec) const;
	BOOL operator<(const COXDirSpec& dirSpec) const;
	BOOL operator>=(const COXDirSpec& dirSpec) const;
	BOOL operator>(const COXDirSpec& dirSpec) const;
	// --- In  : dirSpec : Second object to which this object will be compared
	// --- Out : 
	// --- Returns : FALSE or TRUE
	// --- Effect : Comparison operators
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXDirSpec();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      
	COXDirSpec FindLowerDirectory() const;
	BOOL RemoveAllFiles(BOOL bAlsoRemoveReadOnly = FALSE) const;

private:

};

#endif
// ==========================================================================
