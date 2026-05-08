// ==========================================================================
// 					Class Specification : COXFolderPickerCombo
// ==========================================================================

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
                          
// //////////////////////////////////////////////////////////////////////////

#ifndef _OX_FOLDERPICKERCOMBO__
#define _OX_FOLDERPICKERCOMBO__


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include "OXFolderPickerDlg.h"


class OX_CLASS_DECL COXFolderPickerCombo : public COXHistoryCombo
{
public:
	virtual BOOL BrowseItem()
	{
		ASSERT_VALID(this);

		BOOL bBrowseOK = FALSE;
		CString sCurrentDirectory;
		GetWindowText(sCurrentDirectory);
		COXFolderPickerDlg folderPicker(sCurrentDirectory);
		if(folderPicker.DoModal()==IDOK)
		{
			sCurrentDirectory = folderPicker.GetFolderPath();
			SetWindowText(sCurrentDirectory);

			CWnd* pParentWnd=GetParent();
			if(pParentWnd!=NULL)
			{
				pParentWnd->SendMessage(WM_COMMAND,
					MAKEWPARAM(GetDlgCtrlID(),CBN_EDITCHANGE),
					(LPARAM)GetSafeHwnd());
			}
		
			bBrowseOK = TRUE;
		}

		ASSERT_VALID(this);

		return bBrowseOK;
	}
};

#endif	//	_OX_FOLDERPICKERCOMBO__