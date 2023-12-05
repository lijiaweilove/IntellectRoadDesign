#if !defined(AFX_NUMBEREDIT_H)
#define AFX_NUMBEREDIT_H

#if _MSC_VER > 1000
#pragma once

#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// 
#include "afxcmn.h"

class CNumberEdit : public CEdit
{
	public:
		CNumberEdit();
		~CNumberEdit();
		DECLARE_MESSAGE_MAP()
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
#endif;