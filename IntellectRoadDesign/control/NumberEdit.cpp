#include "stdafx.h"
#include "NumberEdit.h"


CNumberEdit::CNumberEdit()
{
}
CNumberEdit::~CNumberEdit()
{
}

BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()


void CNumberEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nChar == '.')
	{
		CString str;
		// ��ȡԭ���༭���е��ַ���
		GetWindowText(str);
		//��ԭ�����ַ������Ѿ���һ��С����,�򲻽�С��������,��֤�����ֻ������һ��С����
		if(str.Find('.') != -1)
		{
		}
		// ������������С����
		else
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		}
	}
	// ��֤����ֻ�ܳ���һ��,����ֻ�ܳ����ڵ�һ���ַ�
	else if(nChar == '-')
	{
		CString str;
		GetWindowText(str);
		// ��û�������κ��ַ���
		if(str.IsEmpty())
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		}
		else
		{
			CString str;
			// ��ȡԭ���༭���е��ַ���
			GetWindowText(str);
			//��ԭ�����ַ������Ѿ���һ������,�򲻽���������,��֤�����ֻ������һ������
			if(str.Find('-') != -1)
				return;

			int nSource,nDestination;
			GetSel(nSource,nDestination);

			if(nSource == 0 &&
				(nDestination == str.GetLength() // ��ʱѡ����ȫ��������
				|| nSource == 0 && nDestination == 0 //����ڵ��׸��ַ�
				|| nSource == 0 && nDestination == 1))//ѡ���˵�һ���ַ�
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags); 
			}

		}  
	}
	// ����С����͸���,��������������,Backspace,Delete
	else if((nChar >= '0' && nChar <= '9') || (nChar == 0x08) || (nChar == 0x10))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags); 
	}
	// �����ļ�,������Ӧ
}
