#pragma once
#include <acuiDialog.h>
#include "acui.h"


class RoadUI : public CAcUiDialog
{
	DECLARE_DYNAMIC(RoadUI)
public:
	RoadUI(CWnd* pParent = NULL, HINSTANCE hInstance = NULL);

	enum { IDD = IDD_DIALOG1 };

	int departLength;
	double maxSlope;
	bool isOK;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg LRESULT OnAcadKeepFocus(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusEdit1();
	afx_msg void OnEnSetfocusEdit2();
	afx_msg void OnBnClickedOk();
};

