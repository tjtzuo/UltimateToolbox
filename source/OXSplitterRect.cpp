// ===================================================================================
// 					Class Implementation : COXSplitterRect
// ===================================================================================

// Header file : OXSplitterRect.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
// Some portions Copyright (C)1994-5	Micro Focus Inc, 2465 East Bayshore Rd, Palo Alto, CA 94303.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXSplitterRect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXSplitterRect::COXSplitterRect(int type, const RECT& rect) : 
	m_rect(rect),
	m_type((TCHAR)type)
{
	ASSERT(type == SPLITTER_VERT || type == SPLITTER_HORZ);
} 


COXSplitterRect::~COXSplitterRect()
{
} 


void COXSplitterRect::Draw(CDC* pDC)
{
    CRect rect=m_rect;
    switch(m_type)
	{
    case SPLITTER_VERT:
		{
			rect.left++;
			pDC->FillSolidRect(rect.left, rect.top, 1, rect.Height(), 
				::GetSysColor(COLOR_BTNFACE));
			rect.left++;
			pDC->FillSolidRect(rect.left, rect.top, 1, rect.Height(), 
				::GetSysColor(COLOR_BTNHILIGHT));
			
			rect.right--;
			pDC->FillSolidRect(rect.right, rect.top, 1, rect.Height(), 
				::GetSysColor(COLOR_WINDOWFRAME));
			rect.right--;
			pDC->FillSolidRect(rect.right, rect.top, 1, rect.Height(), 
				::GetSysColor(COLOR_BTNSHADOW));
		}
		break;
	
    case SPLITTER_HORZ:
		{
			rect.top++;
			pDC->FillSolidRect(rect.left, rect.top, rect.Width(), 1, 
				::GetSysColor(COLOR_BTNFACE));
			rect.top++;
			pDC->FillSolidRect(rect.left, rect.top, rect.Width(), 1, 
				::GetSysColor(COLOR_BTNHILIGHT));
			
			rect.bottom--;
			pDC->FillSolidRect(rect.left, rect.bottom, rect.Width(), 1,
				::GetSysColor(COLOR_WINDOWFRAME));
			rect.bottom--;
			pDC->FillSolidRect(rect.left, rect.bottom, rect.Width(), 1, 
				::GetSysColor(COLOR_BTNSHADOW));

		}
        break;
		
	default:
        ASSERT(FALSE);
        break;
	}
}


#ifdef _DEBUG
void COXSplitterRect::Dump( CDumpContext& dc ) const
{
	dc << "pos = " << m_nPos;
	dc << (m_type == SPLITTER_HORZ ? " Horz" : " Vert");
}
#endif


