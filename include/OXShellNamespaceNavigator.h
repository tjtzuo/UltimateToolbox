// ==========================================================================
// 				Class Specification : COXShellNamespaceNavigator
// ==========================================================================

// Header file : OXShellNamespaceNavigator.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

/*

  DESCRIPTION


COXShellNamespaceNavigator class is a helper class designed to simplify the
process of navigating through Shell Namespace. This class represents a set
of functions that can be especially useful while enumerating Shell Namespace
items. Primarily it was designed as a helper class for COXShellFolderTree
class (tree control that is automatically populated with Shell Namespace
folders) but COXShellNamespaceNavigator class methods can be used 
independently in order to surf the Shell.

Although this class simplify a lot of things you still have to have good
understanding of the Shell Namespace objects and interfaces it implements.
Refer to the SDK documentation that can be found on MSDN disc in the
following section: 

	"Platform, SDK, and DDK Documentation"
		"Platform SDK"
			"User Interface Services"
				"Shell and Common Controls"
					"Windows Shell API"
						"Shell Namespace"

Specifically take look at the section that explains Item Identifiers and 
Pointers to Item Identifier Lists (IDLs) and interfaces that 
COXShellNamespaceNavigator class exploits in its methods:

	IShellFolder
	IEnumIDList
	IContextMenu

These are crucial elements of Shell Namespace that you should be familiar 
with in order to fully understand the methods and ideas used in the 
COXShellNamespaceNavigator class. 

Below you will find some brief definition of the objects this class 
operates on.

A namespace is a collection of symbols, such as database keys or file and 
directory names. The shell uses a single hierarchical namespace to organize 
all objects of interest to the user, including files, storage devices, 
printers, network resources, and anything else that can be viewed using 
Microsoft® Windows® Explorer. The root of this unified namespace is the 
desktop. 

In many ways, the shell namespace is analogous to a file system's directory 
structure. However, the namespace contains more types of objects than just 
files and directories.




Folders and File Objects

A folder is a collection of items in the shell namespace. A folder is 
analogous to a file system directory, and many folders are, in fact, 
directories. However, there are also other types of folders, such as remote 
computers, storage devices, the Desktop folder, the Control Panel, 
the Printers folder, and the Fonts folder. A folder can contain other 
folders as well as items called file objects. 

Because there are many kinds of folders and file objects, each folder is an 
OLE component object model (COM) object that can enumerate its contents 
and carry out other actions. More precisely, each folder implements the 
IShellFolder interface. 

Using IShellFolder functions, an application can navigate throughout the 
entire shell namespace. 

The following COXShellNamespaceNavigator function can be used in order to 
retrieve a pointer to the IShellFolder interface of the specified folder:

  	LPSHELLFOLDER GetShellFolder(CString sFolderFullPath) const;




Item Identifiers and Pointers to Item Identifier Lists (IDLs)

Objects in the shell namespace are assigned item identifiers and item 
identifier lists. An item identifier uniquely identifies an item within 
its parent folder. An item identifier list uniquely identifies an item 
within the shell namespace by tracing a path to the item from a known 
point, usually the desktop (so called fully qualified IDL). A pointer to an 
item identifier list (PIDL) is used throughout the shell to identify an item.
If you have the IShellFolder interface of Shell Namespace item's parent 
folder then it can be uniquely identified using IDL relative to partent 
folder (which actually consist only of one Item Identifier. We call such 
IDL as relative IDL).

The following COXShellNamespaceNavigator function can be used in order to 
retrieve IDL for the specified folder:

	BOOL GetShellFolderFullIDL(CString sFolderFullPath, 
		LPITEMIDLIST* ppidlFull) const;
	BOOL GetShellFolderRelativeIDL(const LPSHELLFOLDER lpParentFolder,
		CString sFolderRelativePath, LPITEMIDLIST* ppidlRelative) const;




Item Enumeration

A folder's IShellFolder interface can be used to determine the folder's 
contents by using the IShellFolder::EnumObjects method. We provide the 
following set of functions that you can use in order to enumerate the 
contents of given folder:

Object enumeration is done in three steps:

	1)	BOOL InitObjectsEnumerator(	const LPSHELLFOLDER lpFolder, 
									const LPITEMIDLIST lpParentFullIDL,
									DWORD dwFlags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN); 
		is called that specifies the IShellInterface of the folder which 
		objects will be enumerated, this folder fully qualified IDL and
		attributes of enumerated objects

	2)	If InitObjectsEnumerator() returns success then you can call

		LPNAMESPACEOBJECT EnumerateObjectNext(BOOL& bLastReached);

		function which returns pointer to NAMESPACEOBJECT structure
		that provide extended description of the object. Parameter
		bLastReached will be set to FALSE if the last folder object 
		has been reached

	3)	After all objects has been enumerated you MUST call

		BOOL ReleaseObjectsEnumerator();

		function in order to reset internal COXShellNamespaceNavigator
		variables.





Display Names

Because item identifiers are binary data structures, each item in a shell 
folder also has a display name, which is a string that can be shown to the 
user. You can use the following functions:

	CString GetDisplayName(const LPSHELLFOLDER lpParentFolder, 
		const LPITEMIDLIST lpRelativeIDL) const;
	CString GetFullPath(const LPITEMIDLIST lpFullIDL);




Object Attributes and Interfaces

Every file object and folder has attributes that determine, among other 
things, what actions can be carried out on it. To obtain the attributes of a 
file object or folder, your application can use the 
IShellFolder::GetAttributesOf method. We provide the following functions
that simplify the process of retrieving of Shell Namespace item context
menu and invoking any command from this menu on this item:

	HMENU GetObjectContextMenu(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL, DWORD dwFlags) const;
	BOOL InvokeCommand(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL, UINT nCmdID, 
		DWORD dwFlags) const;




Also we provide set of helper functions that simplifies process of working 
with Shell Namespace elements.

The following functions can be used in order to copy, concatenate and 
enumerate PIDLs:

	LPITEMIDLIST ConcatenatePIDLs(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) const;
	LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidlSource) const;
	UINT GetIDLSize(LPCITEMIDLIST pidl) const;
	LPITEMIDLIST GetNextIDLItem(LPCITEMIDLIST pidl) const;

Every item in shell namespace has image associated with it. Shell keeps
one image list for all items' images. In order to retrieve it use the 
following function:

	HIMAGELIST GetShellImageList(BOOL bSmallIcon=TRUE) const;


Also we provide specific NAMESPACEOBJECT structure that fully desribes 
any Shell Namespace item (including PIDLs, pointer to IShellFolder, 
display name and image index, for details refer to the documentation).
NAMESPACEOBJECT is used as return value of some of the 
COXShellNamespaceNavigator functions but you can retrieve it explicitly
using following function:

	LPNAMESPACEOBJECT GetNameSpaceObject(const LPSHELLFOLDER lpsfParent,
		const LPITEMIDLIST lpRelativeIDL, 
		const LPITEMIDLIST lpFullIDL) const;





The best way to learn how to use COXShellNamespaceNavigator class is to 
take look at the implementation of COXShellFolderTree class which 
represents standard tree control which is automatically populated with 
Shell Namespace folders. 



Dependencies:

	#include "OXShellFolderTree.h"

Source code files:
	
	"OXShellNamespaceNavigator.cpp"


*/


#if !defined(_OXSHELLNAMESPACENAVIGATOR_H_)
#define _OXSHELLNAMESPACENAVIGATOR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

// OXShellNamespaceNavigator.h : header file
//

#ifndef _SHLOBJ_H_
#include <shlobj.h>
#define _SHLOBJ_H_
#endif	//	_SHLOBJ_H_


#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif	//	__AFXTEMPL_H__

#include "UTB64Bit.h"

// shell commands
static const int IDCMD_DELETE=18;
static const int IDCMD_RENAME=19;
static const int IDCMD_PROPERTIES=20;
static const int IDCMD_CUT=25;
static const int IDCMD_COPY=26;
static const int IDCMD_PASTE=27;

/////////////////////////////////////////////////////////////////////////////
// Shell Namespaceobject descriptor
//
//	lpsfParent			-	pointer to item's parent IShellFolder interface 
//	lpRelativeIDL		-	item IDL relative to its parent folder
//	lpFullIDL			-	item IDL relative to Desktop folder
//	dwFlags				-	item attributes. Can be combination of the 
//							following:
//
//						SFGAO_CANDELETE		The specified file objects or 
//											folders can be deleted.  
//						SFGAO_CANLINK		It is possible to create 
//											shortcuts for the specified file 
//											objects or folders (same value 
//											as the DROPEFFECT_LINK flag).  
//						SFGAO_CANMOVE		The specified file objects or 
//											folders can be moved (same value 
//											as the DROPEFFECT_MOVE flag).  
//						SFGAO_CANRENAME		The specified file objects or 
//											folders can be renamed.  
//						SFGAO_CAPABILITYMASK  Mask for the capability flags.  
//						SFGAO_DROPTARGET	The specified file objects or 
//											folders are drop targets.  
//						SFGAO_HASPROPSHEET	The specified file objects or 
//											folders have property sheets.  
//						SFGAO_GHOSTED		The specified file objects or 
//											folders should be displayed 
//											using a ghosted icon.  
//						SFGAO_LINK			The specified file objects are 
//											shortcuts.  
//						SFGAO_READONLY		The specified file objects or 
//											folders are read-only.  
//						SFGAO_SHARE			The specified folders are shared.  
//						SFGAO_HASSUBFOLDER	The specified folders have 
//											subfolders (and are, therefore, 
//											expandable in the left pane of 
//											Windows Explorer).  
//						SFGAO_COMPRESSED	The specified items are 
//											compressed.  
//						SFGAO_FILESYSTEM	The specified folders or file 
//											objects are part of the file 
//											system (that is, they are files, 
//											directories, or root directories).  
//						SFGAO_FILESYSANCESTOR The specified folders contain 
//											one or more file system 
//											folders.  
//						SFGAO_FOLDER		The specified items are folders.  
//						SFGAO_NEWCONTENT	The objects contain new content.  
//						SFGAO_NONENUMERATED	The items are nonenumerated items.  
//						SFGAO_REMOVABLE		The specified file objects or 
//											folders are on removable media.  
//						SFGAO_VALIDATE		Validate cached information. 
//											The shell will validate that the 
//											object still exist and will not 
//											used cached information when 
//											retrieving the attributes. 
//											This is similar to doing a 
//											refresh of the folder.  
//	szDisplayName		-	item's display name
//	nImage				-	item's image (normal)
//	nImageSelected		-	item's image (normal) when it is in 
//							selected state
//	nImageSmall			-	item's image (small)
//	nImageSelectedSmall	-	item's image (small) when it is in 
//							selected state
//
typedef struct tagNAMESPACEOBJECT
{
	LPSHELLFOLDER lpsfParent;
	LPITEMIDLIST lpRelativeIDL;
	LPITEMIDLIST lpFullIDL;
	DWORD dwFlags;
	TCHAR szDisplayName[MAX_PATH];
	int nImage;
	int nImageSelected;
	int nImageSmall;
	int nImageSelectedSmall;

} NAMESPACEOBJECT, *LPNAMESPACEOBJECT;
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// COXShellNamespaceNavigator window

class OX_CLASS_DECL COXShellNamespaceNavigator
{
// Construction
public:
	// --- In:
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Constructs the object
	COXShellNamespaceNavigator();

// Attributes
public:

protected:
	// pointer to the window to which all Shell error notification will 
	// be forwarded (if any Shell error happens message box with 
	// notification will be displayed). If it is NULL then Shell error
	// notification will be surpassed
	CWnd* m_pOwnerWnd;

	////////////////////////////////////////////////////////////////////////
	// set of variables used to enumeratwe parent folder contents 
	// (subfolders and items). Object enumeration is done in three steps:
	//
	//	1)	BOOL InitObjectsEnumerator(	const LPSHELLFOLDER lpFolder, 
	//									const LPITEMIDLIST lpParentFullIDL,
	//									const DWORD dwFlags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN); 
	//		is called that specifies the IShellInterface of the folder which 
	//		objects will be enumerated, this folder fully qualified IDL and
	//		attributes of enumerated objects
	//
	//	2)	If InitObjectsEnumerator() returns success then you can call
	//
	//		LPNAMESPACEOBJECT EnumerateObjectNext(BOOL& bLastReached);
	//
	//		function which returns pointer to NAMESPACEOBJECT structure
	//		that provide extended description of the object. Parameter
	//		bLastReached will be set to FALSE if the last folder object 
	//		has been reached
	//
	//	3)	After all objects has been enumerated you MUST call
	//
	//		BOOL ReleaseObjectsEnumerator();
	//
	//		function in order to reset internal COXShellNamespaceNavigator
	//		variables.
	//
    LPENUMIDLIST m_lpeidl;
	BOOL m_bEnumeratorInitialized;
	LPSHELLFOLDER m_lpEnumerateFolder;
	LPITEMIDLIST m_lpEnumerateParentFullIDL;
	////////////////////////////////////////////////////////////////////////

	mutable CMap<DWORD_PTR,DWORD_PTR,DWORD_PTR,DWORD_PTR> m_mapObjectsToFree;
	mutable CMap<DWORD_PTR,DWORD_PTR,DWORD_PTR,DWORD_PTR> m_mapIShellFolderToRelease;
	BOOL m_bAutoCleanUp;

// Operations
public:

	// --- In:		lParam1		-	pointer to NAMESPACEOBJECT for the
	//								first object to compare
	//				lParam2		-	pointer to NAMESPACEOBJECT for the
	//								second object to compare
	//				lParamSort	-	additional data (unused at the moment)
	// --- Out:		 
	// --- Returns:	Less than zero		-	The first item should precede 
	//										the second 
	//				Greater than zero	-	The first item should follow 
	//										the second 
	//				Zero				-	The two items are the same
	// --- Effect : Static callback function that can be used in fast
	//				sorting routines that operate on Shell Namespace 
	//				objects
	static int CALLBACK CompareObjectsProc(LPARAM lParam1, 
		LPARAM lParam2, LPARAM lParamSort);


	// --- In:		pWnd		-	pointer to CWnd object
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Sets the window to which all Shell error notification 
	//				will be forwarded (if any Shell error happens message 
	//				box with notification will be displayed). If it is NULL 
	//				then Shell error notification will be surpassed.
	inline void SetOwnerWnd(CWnd* pWnd) {
		m_pOwnerWnd=pWnd;
	}


	// --- In:		bSmallIcon	-	flag that specifies whether image with
	//								small or normal images will be retrieved
	// --- Out:		 
	// --- Returns:	Handle of the corresponding image list if succeed 
	//				or NULL otherwise
	// --- Effect : Retrieves Shell image list
	HIMAGELIST GetShellImageList(BOOL bSmallIcon=TRUE) const;
	

	// --- In:		sFolderFullPath	-	full folder path
	// --- Out:		 
	// --- Returns:	pointer to IShellFolder interface of the corresponding 
	//				folder if succeed or NULL otherwise
	// --- Effect : Retrieves IShellFolder interface for specified folder
	LPSHELLFOLDER GetShellFolder(CString sFolderFullPath) const;

	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the parent folder
	//				lpRelativeIDL	-	folder IDL relative to its 
	//									parent folder
	// --- Out:		 
	// --- Returns:	pointer to IShellFolder interface of the corresponding 
	//				folder if succeed or NULL otherwise
	// --- Effect : Retrieves IShellFolder interface for specified folder
	LPSHELLFOLDER GetShellFolder(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL) const;

	// --- In:		pidlFull	-	pointer to fully qualified IDL that uniquely
	//								identifies the folder which IShellFolder 
	//								interface is being requested
	// --- Out:		 
	// --- Returns:	pointer to IShellFolder interface of the corresponding 
	//				folder if succeed or NULL otherwise
	// --- Effect : Retrieves IShellFolder interface for specified folder
	LPSHELLFOLDER GetShellFolder(LPCITEMIDLIST pidlFull) const;


	// --- In:		pidlFull		-	pointer to fully qualified IDL that uniquely
	//									identifies the folder which parent folder 
	//									interface has been requested
	// --- Out:		lppRelativeIDL	-	pointer to a pointer to relative IDL that 
	//									will be filled with relative IDL for the 
	//									folder specified by its full IDL pidlFull.
	// --- Returns:	pointer to IShellFolder interface of the parent 
	//				folder if succeed or NULL otherwise
	// --- Effect : Retrieves IShellFolder interface of the parent folder and 
	//				relative to this parent folder relative IDL
	LPSHELLFOLDER GetParentShellFolder(LPCITEMIDLIST pidlFull, 
		LPITEMIDLIST* lppRelativeIDL) const;

	// --- In:		sFolderFullPath	-	full folder path
	// --- Out:		ppidlFull		-	pointer to pointer to fully 
	//									qualified IDL that will be filled
	//									with specified folder IDL
	// --- Returns:	TRUE if specified folder fully qualified IDL was
	//				successfully retrieved or FALSE otherwise
	// --- Effect : Retrieves fully qualified IDL for specified folder
	BOOL GetShellFolderFullIDL(CString sFolderFullPath, 
		LPITEMIDLIST* ppidlFull) const;

	// --- In:		lpParentFolder		-	pointer to IShellFolder interface 
	//										of the parent folder
	//				sFolderRelativePath	-	folder path relative to its 
	//										parent folder
	// --- Out:		ppidlRelative		-	pointer to pointer to relative 
	//										IDL that will be filled with 
	//										specified folder IDL relative to
	//										its parent folder
	// --- Returns:	TRUE if specified folder relative IDL was
	//				successfully retrieved or FALSE otherwise
	// --- Effect : Retrieves relative IDL for specified folder
	BOOL GetShellFolderRelativeIDL(const LPSHELLFOLDER lpParentFolder,
		CString sFolderRelativePath, LPITEMIDLIST* ppidlRelative) const;


	// --- In:		lpParentFolder		-	pointer to IShellFolder interface 
	//										of the folder which items will be 
	//										enumerated
	//				lpParentFullIDL		-	fully qualified IDL of the folder
	//										wich items will be enumerated 
	//				dwFlags				-	determines the type of items 
	//										included in an enumeration. 
	//										
	//							SHCONTF_FOLDERS			Include items that 
	//													are folders in the 
	//													enumeration. 
	//							SHCONTF_NONFOLDERS		Include items that 
	//													are not folders in 
	//													the enumeration. 
	//							SHCONTF_INCLUDEHIDDEN	Include hidden items 
	//													in the enumeration. 
	// --- Out:		
	// --- Returns:	TRUE if initialization suceeded or FALSE otherwise
	// --- Effect : Initialize the process of enumerating folder items.
	//				In order to enumerate items call EnumerateObjectNext()
	//				function. After all items has been enumerated 
	//				you MUST call ReleaseObjectsEnumerator() function.
	BOOL InitObjectsEnumerator(const LPSHELLFOLDER lpParentFolder, 
		const LPITEMIDLIST lpParentFullIDL,
		DWORD dwFlags=SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN);

	// --- In:		
	// --- Out:		bLastReached	-	flag will be set to TRUE if the 
	//									last item in the enumeration
	//									queue has been reached
	// --- Returns:	pointer to NAMESPACEOBJECT structure if suceeded or
	//				NULL otherwise
	// --- Effect : Enumerates folder items. Before calling this function
	//				the process of enumeration MUST be initialized calling
	//				InitObjectsEnumerator() function. After all items has 
	//				been enumerated you MUST call ReleaseObjectsEnumerator() 
	//				function.			
	LPNAMESPACEOBJECT EnumerateObjectNext(BOOL& bLastReached);

	// --- In:		
	// --- Out:		
	// --- Returns:	TRUE if internal enumeration objects were released
	//				successfully or FALSE otherwise.
	// --- Effect : Releases all internal enumeration objects. After all 
	//				items has been enumerated this function MUST be called.
	BOOL ReleaseObjectsEnumerator();


	// --- In:		lpsfParent		-	pointer to IShellFolder interface 
	//									of parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	//				lpFullIDL		-	fully qualified IDL
	// --- Out:		
	// --- Returns:	pointer to NAMESPACEOBJECT structure if suceeded or
	//				NULL otherwise
	// --- Effect : Creates NAMESPACEOBJECT structure for Shell 
	//				Namespace item.
	LPNAMESPACEOBJECT GetNameSpaceObject(const LPSHELLFOLDER lpsfParent,
		const LPITEMIDLIST lpRelativeIDL, 
		const LPITEMIDLIST lpFullIDL) const;


	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the item's parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	// --- Out:		
	// --- Returns:	Display name of the specified Shell Namespace item
	// --- Effect : Retrieves the display name of the specified Shell 
	//				Namespace item
	CString GetDisplayName(const LPSHELLFOLDER lpParentFolder, 
		const LPITEMIDLIST lpRelativeIDL) const;

	// --- In:		lpFullIDL		-	fully qualified IDL of the 
	//									specified Shell Namespace item
	// --- Out:		
	// --- Returns:	Full path of the specified Shell Namespace item
	// --- Effect : Retrieves the Full path of the specified Shell 
	//				Namespace item
	inline CString GetFullPath(const LPITEMIDLIST lpFullIDL) const {
		TCHAR pchPath[MAX_PATH];
		if(SHGetPathFromIDList(lpFullIDL,pchPath))
			return pchPath;
		else
			return _T("");
	}


	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the item's parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	//				dwFlags			-	optional flags specifying how the 
	//									context menu can be changed. Can be 
	//									any combination of the following 
	//									values: 
	//
	//					CMF_CANRENAME		This flag is set if the calling 
	//										application supports renaming 
	//										of items. A context menu 
	//										extension or drag-and-drop 
	//										handler should ignore this flag. 
	//										A namespace extension should add 
	//										a rename item to the menu if 
	//										applicable.  
	//					CMF_DEFAULTONLY		This flag is set when the user 
	//										is activating the default action,
	//										typically by double-clicking. 
	//										This flag provides a hint for 
	//										the context menu extension to 
	//										add nothing if it does not 
	//										modify the default item in the 
	//										menu. A context menu extension 
	//										or drag-and-drop handler should 
	//										not add any menu items if this 
	//										value is specified. A namespace 
	//										extension should add only the 
	//										default item (if any).  
	//					CMF_EXPLORE			This flag is set when Windows 
	//										Explorer's tree window is 
	//										present. Context menu handlers 
	//										should ignore this value.  
	//					CMF_INCLUDESTATIC	This flag is set when a static 
	//										menu is being constructed. Only 
	//										the browser should use this flag.
	//										All other context menu 
	//										extensions should ignore this 
	//										flag.  
	//					CMF_NODEFAULT		This flag is set if no item in 
	//										the menu should be the default 
	//										item. A context menu extension 
	//										or drag-and-drop handler should 
	//										ignore this flag. A namespace 
	//										extension should not set any of 
	//										the menu items to the default.  
	//					CMF_NORMAL			Indicates normal operation. 
	//										A context menu extension, 
	//										namespace extension, or 
	//										drag-and-drop handler can add 
	//										all menu items.  
	//					CMF_NOVERBS			This flag is set for items 
	//										displayed in the "Send To:" menu.
	//										Context menu handlers should 
	//										ignore this value.  
	//					CMF_VERBSONLY		This flag is set if the context 
	//										menu is for a shortcut object. 
	//										Context menu handlers should 
	//										ignore this value.  
	//
	// --- Out:		
	// --- Returns:	Handle to popup menu for corresponding Shell Namespace 
	//				item if suceeded or FALSE otherwise
	// --- Effect : Retrieves handle to popup menu for corresponding 
	//				Shell Namespace item
	HMENU GetObjectContextMenu(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL, DWORD dwFlags) const;

	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the item's parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	//				nCmdID			-	command ID to be invoked
	//				dwFlags			-	the same as in GetObjectContextMenu()
	//									function
	// --- Out:		
	// --- Returns:	TRUE if command was successfully invoked or FALSE
	//				otherwise
	// --- Effect : Invokes menu command from the context menu that was 
	//				previously retrieved by calling GetObjectContextMenu()
	//				function
	BOOL InvokeCommand(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL, UINT nCmdID, 
		DWORD dwFlags) const;


	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the item's parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	// --- Out:		
	// --- Returns:	TRUE if default command was successfully invoked or FALSE
	//				otherwise
	// --- Effect : Invokes default menu command from the context menu
	BOOL InvokeDefaultCommand(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL) const;


	// --- In:		lpParentFolder	-	pointer to IShellFolder interface 
	//									of the item's parent folder
	//				lpRelativeIDL	-	relative IDL to its parent folder
	//				lppNewRelativeIDL-	pointer to new relative IDL that will be 
	//									allocated in result of changing the name. 
	//									In this case the old relative IDL will be 
	//									automatically freed
	//				lpszNewName		-	new object's name
	// --- Out:		
	// --- Returns:	TRUE if the object was successfully renamed (in this case old 
	//				lpRelativeIDL will be automatically freed and the new one 
	//				allocated in lpNewRelativeIDL) or FALSE otherwise
	// --- Effect : Renames the specified Shell Name space object
	BOOL RenameShellObject(const LPSHELLFOLDER lpParentFolder,
		const LPITEMIDLIST lpRelativeIDL, LPITEMIDLIST* lppNewRelativeIDL, 
		LPCTSTR lpszNewName) const;



	////////////////////////////////////////////////////////////////////////
	// set of helper functions 
	//

	// --- In:		pidl1	-	IDL
	//				pidl2	-	IDL	
	// --- Out:		
	// --- Returns:	pidl1 + pidl2
	// --- Effect : Concatenates two IDLs. Usually you would call this
	//				function in order to concatenate item's parent fully
	//				qualified IDL and item's relative IDL
	LPITEMIDLIST ConcatenatePIDLs(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) const;

	// --- In:		pidlSource	-	IDL
	// --- Out:		
	// --- Returns:	Copy of pidlSource
	// --- Effect : Copies the specified IDL.
	LPITEMIDLIST CopyPIDL(LPCITEMIDLIST pidlSource) const;

	// --- In:		pidl	-	IDL
	// --- Out:		
	// --- Returns:	Size in bytes of the specified IDL
	// --- Effect : Retrieves the size of specified IDL.
	UINT GetIDLSize(LPCITEMIDLIST pidl) const;

	// --- In:		pidl	-	IDL
	// --- Out:		
	// --- Returns:	next IDL item in IDL or NULL if the end has been reached
	// --- Effect : Retrieves next IDL item in IDL.
	LPITEMIDLIST GetNextIDLItem(LPCITEMIDLIST pidl) const;
	
	//
	////////////////////////////////////////////////////////////////////////


	CString GetSpecialFolderPath(int nFolder, CWnd* pWndOwner=NULL) const;


	void FreeShellObject(void* pVoid) const;

// Implementation
public:
	// --- In:
	// --- Out:		 
	// --- Returns:	
	// --- Effect : Object's destructor
	virtual ~COXShellNamespaceNavigator();

protected:


private:
	LPMALLOC m_pMalloc;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(_OXSHELLNAMESPACENAVIGATOR_H_)
