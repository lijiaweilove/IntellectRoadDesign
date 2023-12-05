#pragma once
#include <afxcmn.h>
class CTipsListCtrl :
	public CListCtrl
{
public:
	CTipsListCtrl();
	~CTipsListCtrl();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCustomdraw(NMHDR * pNMHDR, LRESULT * pResult);
	BOOL PreTranslateMessage(MSG* pMsg);
	void OnDestroy();
	BOOL EnableTips();

	BOOL m_bEnableTips;
	int m_nItem;
	int m_nSubItem;
	int m_nSelectedItemIndex;
	CToolTipCtrl m_toolTips;
};

