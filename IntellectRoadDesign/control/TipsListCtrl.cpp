#include "stdafx.h"
#include "TipsListCtrl.h"

CTipsListCtrl::CTipsListCtrl()
{

}

CTipsListCtrl::~CTipsListCtrl()
{

}

BEGIN_MESSAGE_MAP(CTipsListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CTipListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CTipListCtrl message handlers
void CTipsListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bEnableTips)
	{
		CString str;
		LVHITTESTINFO lvhti;

		// 判断鼠标当前所在的位置(行, 列)
		lvhti.pt = point;
		SubItemHitTest(&lvhti);

		// 如果鼠标移动到另一个单元格内, 则进行处理; 否则, 不做处理
		if ((lvhti.iItem != m_nItem) || (lvhti.iSubItem != m_nSubItem))
		{
			// 保存当前鼠标所在的(行,列)
			m_nItem = lvhti.iItem;
			m_nSubItem = lvhti.iSubItem;

			// 如果鼠标移动到一个合法的单元格内,则显示新的提示信息
			// 否则, 不显示提示

			if ((m_nItem != -1) && (m_nSubItem != -1))
			{
				// @@@@@@@@ 在这里修改要显示的提示信息
				// 这里仅仅是一个例子---获得当前单元格的文字信息, 并设置为新的提示信息
				str = GetItemText(m_nItem, m_nSubItem);
				m_toolTips.AddTool(this, str);
				// 显示提示框
				m_toolTips.Pop();
			}
		}
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

BOOL CTipsListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (::IsWindow(m_toolTips.GetSafeHwnd()))
	{
		m_toolTips.RelayEvent(pMsg);
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CTipsListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
	// TODO: Add your message handler code here
	m_toolTips.DestroyWindow();
	m_toolTips.Detach();
}

BOOL CTipsListCtrl::EnableTips()
{
	EnableToolTips(TRUE);
	// 创建tooltip控件
	m_bEnableTips = m_toolTips.Create(this, TTS_ALWAYSTIP);

	if (m_bEnableTips)
	{
		m_toolTips.Activate(TRUE);
		m_toolTips.SetDelayTime(TTDT_INITIAL,1000);
	}

	return m_bEnableTips;
}

void CTipsListCtrl::OnCustomdraw(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	NMLVCUSTOMDRAW* pNMLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_NOTIFYITEMDRAW;
	if (CDDS_PREPAINT == pNMLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pNMLVCD->nmcd.dwDrawStage)
	{
		COLORREF crBk = RGB(255, 255, 255);
		COLORREF clrNewTextColor = RGB(0, 0, 0);
		int nItem = (int)(pNMLVCD->nmcd.dwItemSpec);
		POSITION pos = GetFirstSelectedItemPosition();
		while (pos != NULL)
		{
			int index = GetNextSelectedItem(pos);
			if (index == nItem)//如果要刷新的项为当前选择的项，则将文字设为白色，背景色设为蓝色
			{
				clrNewTextColor = RGB(255, 255, 255);        //Set the text to white
				crBk = RGB(49, 106, 197);        //Set the background color to blue
				break;
			}
		}
		pNMLVCD->clrText = clrNewTextColor;
		pNMLVCD->clrTextBk = crBk;
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
}