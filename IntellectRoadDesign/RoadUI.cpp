#include "StdAfx.h"
#include "resource.h"
#include "RoadUI.h"
#include "DataManager.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(RoadUI, CAcUiDialog)

BEGIN_MESSAGE_MAP(RoadUI, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
    ON_EN_SETFOCUS(IDC_EDIT1, &RoadUI::OnEnSetfocusEdit1)
    ON_EN_SETFOCUS(IDC_EDIT2, &RoadUI::OnEnSetfocusEdit2)
    ON_BN_CLICKED(IDOK, &RoadUI::OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
RoadUI::RoadUI(CWnd* pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog(RoadUI::IDD, pParent, hInstance){
}

//-----------------------------------------------------------------------------
void RoadUI::DoDataExchange(CDataExchange* pDX) {
	CAcUiDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT RoadUI::OnAcadKeepFocus(WPARAM, LPARAM) {
	return (TRUE);
}


BOOL RoadUI::OnInitDialog()
{
    departLength = -1;
    maxSlope = 0.;
    isOK = false;
	CAcUiDialog::OnInitDialog();
    SetDlgItemText(IDC_EDIT1, _T("默认为6"));
    SetDlgItemText(IDC_EDIT2, _T("默认为15"));
    return TRUE;
}


void RoadUI::OnEnSetfocusEdit1()
{
    CString strText;
    GetDlgItemText(IDC_EDIT1, strText);
    CString strDefaultText = _T("默认为6");

    if (strText == strDefaultText){
        SetDlgItemText(IDC_EDIT1, _T(""));
    }
}


void RoadUI::OnEnSetfocusEdit2()
{
    CString strText;
    GetDlgItemText(IDC_EDIT2, strText);
    CString strDefaultText = _T("默认为15");

    if (strText == strDefaultText) {
        SetDlgItemText(IDC_EDIT2, _T(""));
    }
}


void RoadUI::OnBnClickedOk()
{
    isOK = true;
    CString strValue1, strValue2;
    GetDlgItemText(IDC_EDIT1, strValue1);
    GetDlgItemText(IDC_EDIT2, strValue2);
    int nValue1 = _ttoi(strValue1); // 转换为整数
    double nValue2 = _wtof(strValue2.GetBuffer()); // 转换为浮点数

        
    if (nValue1 != 0) {
        departLength = nValue1;
        maxSlope = nValue2;
        DataManager::Instanse()->SetDepartLength(departLength);
        DataManager::Instanse()->SetMaxSlope(maxSlope);
        CAcUiDialog::OnOK();
    }
    else {
        // 内容不是有效的整数
        AfxMessageBox(_T("请输入有效的整数。"));
    }
}
