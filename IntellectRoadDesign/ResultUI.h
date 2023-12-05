#pragma once
#include "acui.h"
#include <acuiDialog.h>


class ResultUI : public CAcUiDialog {
	DECLARE_DYNAMIC(ResultUI)

public:
	ResultUI(CWnd* pParent = NULL, HINSTANCE hInstance = NULL);

	enum { IDD = IDD_DIALOG2 };
	double roadDis, mountainDis;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg LRESULT OnAcadKeepFocus(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

