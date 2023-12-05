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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nChar == '.')
	{
		CString str;
		// 获取原来编辑框中的字符串
		GetWindowText(str);
		//若原来的字符串中已经有一个小数点,则不将小数点输入,保证了最多只能输入一个小数点
		if(str.Find('.') != -1)
		{
		}
		// 否则就输入这个小数点
		else
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		}
	}
	// 保证负号只能出现一次,并且只能出现在第一个字符
	else if(nChar == '-')
	{
		CString str;
		GetWindowText(str);
		// 还没有输入任何字符串
		if(str.IsEmpty())
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags); 
		}
		else
		{
			CString str;
			// 获取原来编辑框中的字符串
			GetWindowText(str);
			//若原来的字符串中已经有一个负号,则不将负号输入,保证了最多只能输入一个负号
			if(str.Find('-') != -1)
				return;

			int nSource,nDestination;
			GetSel(nSource,nDestination);

			if(nSource == 0 &&
				(nDestination == str.GetLength() // 此时选择了全部的内容
				|| nSource == 0 && nDestination == 0 //光标在第首个字符
				|| nSource == 0 && nDestination == 1))//选择了第一个字符
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags); 
			}

		}  
	}
	// 除了小数点和负号,还允许输入数字,Backspace,Delete
	else if((nChar >= '0' && nChar <= '9') || (nChar == 0x08) || (nChar == 0x10))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags); 
	}
	// 其它的键,都不响应
}
