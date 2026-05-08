// ==========================================================================
// 					Class Specification : COXBrowseDirEditList
// ==========================================================================

// Header file : OXBrowseDirEditList.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                         
// //////////////////////////////////////////////////////////////////////////

#ifndef __OXBROWSEDIREDITLIST_H__
#define __OXBROWSEDIREDITLIST_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXEditList.h"
#include "OXBrowseDirEdit.h"



class OX_CLASS_DECL COXBrowseDirGridEdit : public COXBaseBrowseDirEdit<COXGridEdit>
{
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


class OX_CLASS_DECL COXBrowseDirGridEdit16 : public COXBaseBrowseDirEdit16<COXGridEdit>
{
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


class OX_CLASS_DECL COXBrowseFileGridEdit : public COXBaseBrowseFileEdit<COXGridEdit>
{
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


////////////////////////////////////////////////////////////////////////////////


class OX_CLASS_DECL COXBrowseDirEditList : public COXEditList
{
public:
	COXBrowseDirEditList(EToolbarPosition eToolbarPosition=TBPHorizontalTopRight,
		BOOL bAllowDuplicates=FALSE, BOOL bOrderedList=TRUE) :
	COXEditList(eToolbarPosition,bAllowDuplicates,bOrderedList) {};

protected:
	COXBrowseDirGridEdit m_browseDirEdit;

public:
	virtual COXGridEdit* GetGridEditCtrl() { return &m_browseDirEdit; }
};


class OX_CLASS_DECL COXBrowseDirEditList16 : public COXEditList
{
public:
	COXBrowseDirEditList16(EToolbarPosition eToolbarPosition=TBPHorizontalTopRight,
		BOOL bAllowDuplicates=FALSE, BOOL bOrderedList=TRUE) :
	COXEditList(eToolbarPosition,bAllowDuplicates,bOrderedList) {};

protected:
	COXBrowseDirGridEdit16 m_browseDirEdit16;

public:
	virtual COXGridEdit* GetGridEditCtrl() { return &m_browseDirEdit16; }
};


class OX_CLASS_DECL COXBrowseFileEditList : public COXEditList
{
public:
	COXBrowseFileEditList(EToolbarPosition eToolbarPosition=TBPHorizontalTopRight,
		BOOL bAllowDuplicates=FALSE, BOOL bOrderedList=TRUE) :
	COXEditList(eToolbarPosition,bAllowDuplicates,bOrderedList) {};

protected:
	COXBrowseFileGridEdit m_browseFileEdit;

public:
	virtual COXGridEdit* GetGridEditCtrl() { return &m_browseFileEdit; }
};


#endif      // __OXBROWSEDIREDITLIST_H__
