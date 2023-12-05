#include "StdAfx.h"
#include "resource.h"
#include "ResultUI.h"

//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(ResultUI, CAcUiDialog)

BEGIN_MESSAGE_MAP(ResultUI, CAcUiDialog)
	ON_MESSAGE(WM_ACAD_KEEPFOCUS, OnAcadKeepFocus)
    ON_BN_CLICKED(IDOK, &ResultUI::OnBnClickedOk)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
ResultUI::ResultUI(CWnd* pParent /*=NULL*/, HINSTANCE hInstance /*=NULL*/) : CAcUiDialog(ResultUI::IDD, pParent, hInstance) {
}

//-----------------------------------------------------------------------------
void ResultUI::DoDataExchange(CDataExchange* pDX) {
	CAcUiDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------
//----- Needed for modeless dialogs to keep focus.
//----- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT ResultUI::OnAcadKeepFocus(WPARAM, LPARAM) {
	return (TRUE);
}


BOOL ResultUI::OnInitDialog()
{
    CAcUiDialog::OnInitDialog();
    CString str1, str2;
    str1.Format(_T("%f"), roadDis);
    str2.Format(_T("%f"), mountainDis);
    SetDlgItemText(IDC_EDIT1, str2);
    SetDlgItemText(IDC_EDIT2, str1);
    return TRUE;
}

void ResultUI::OnBnClickedOk()
{
    CString strValue;
    GetDlgItemText(IDC_EDIT3, strValue);
    double nValue = _wtof(strValue.GetBuffer());
    nValue = nValue * mountainDis;
    strValue.Format(_T("%f"), nValue);
    SetDlgItemText(IDC_EDIT4, strValue);
}
