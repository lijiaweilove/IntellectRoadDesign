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

		// �ж���굱ǰ���ڵ�λ��(��, ��)
		lvhti.pt = point;
		SubItemHitTest(&lvhti);

		// �������ƶ�����һ����Ԫ����, ����д���; ����, ��������
		if ((lvhti.iItem != m_nItem) || (lvhti.iSubItem != m_nSubItem))
		{
			// ���浱ǰ������ڵ�(��,��)
			m_nItem = lvhti.iItem;
			m_nSubItem = lvhti.iSubItem;

			// �������ƶ���һ���Ϸ��ĵ�Ԫ����,����ʾ�µ���ʾ��Ϣ
			// ����, ����ʾ��ʾ

			if ((m_nItem != -1) && (m_nSubItem != -1))
			{
				// @@@@@@@@ �������޸�Ҫ��ʾ����ʾ��Ϣ
				// ���������һ������---��õ�ǰ��Ԫ���������Ϣ, ������Ϊ�µ���ʾ��Ϣ
				str = GetItemText(m_nItem, m_nSubItem);
				m_toolTips.AddTool(this, str);
				// ��ʾ��ʾ��
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
	// ����tooltip�ؼ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			if (index == nItem)//���Ҫˢ�µ���Ϊ��ǰѡ������������Ϊ��ɫ������ɫ��Ϊ��ɫ
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