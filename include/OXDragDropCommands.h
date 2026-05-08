// ==========================================================================
// 					Class Specification : COXDragDropCommands
// ==========================================================================

// Header file : OXDragDropCommands.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////


#ifndef _OXDRAGDROPCOMMANDS_H__
#define _OXDRAGDROPCOMMANDS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"
#include "OXBitmapMenuOrganizer.h"

class OX_CLASS_DECL COXDragDropCommands
{
public:
	// custom format for drag'n'drop operations
	static CLIPFORMAT m_cfCommandButton;


public:
	// --- In  :	lpszText	-	item text
	//				nImageIndex	-	item image index
	//				nCommandID	-	item command ID
	//				fsStyle		-	item style (not relevant for menu items,
	//											always set to 0)
	//				hSubmenu	-	item submenu 
	// --- Out : 
	// --- Returns:	A pointer to created COleDataSource object that will contain
	//				item info in specific format
	// --- Effect : Creates COleDataSource for drag'n'drop operation that contains
	//				item info in specific internal format. Caller is responsible
	//				for deleting the returned COleDataSource object
	static COleDataSource* PrepareDragDropData(LPCTSTR lpszText, int nImageIndex, 
		int nCommandID, BYTE fsStyle=0, HMENU hSubmenu=NULL);
	static DROPEFFECT DoDragDrop(COleDataSource* pDataSource,
		COleDropSource* pOleDropSource);
	static int CalcMenuDataSize(HMENU hMenu);
	static void WriteItemDragDropData(BYTE*& lpData, LPCTSTR lpszText, 
		int nImageIndex, int nCommandID, BYTE fsStyle);
	static void WriteMenuDragDropData(BYTE*& lpData, HMENU hMenu);
};

/////////////////////////////////////////////////////////////////////////////

#endif	//	_OXDRAGDROPCOMMANDS_H__