// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "Common/ArxHelper.h"
#include "IntellectRoadDesignCmd.h"
#include "RoadUI.h"
#include "ResultUI.h"
//-----------------------------------------------------------------------------
#define szRDS _RXST("NBIM")

bool getApplication(LPDISPATCH * pVal)
{
	LPDISPATCH pDispatch = acedGetAcadWinApp()->GetIDispatch(TRUE);
	if (pDispatch == NULL)
		return false;
	*pVal = pDispatch;
	return true;
}

bool getAcadMenuGroup(IAcadMenuGroup  **pVal)
{

	IAcadApplication *acadApp = NULL;
	LPDISPATCH  pDisp = NULL;

	if (!getApplication(&pDisp))
		return false;

	HRESULT hr = S_OK;
	hr = pDisp->QueryInterface(IID_IAcadApplication, (LPVOID*)&acadApp);
	if (FAILED(hr))
		return false;


	LPDISPATCH  pTempDisp = NULL;
	IAcadMenuGroups *mnuGrps = NULL;
	long cnt = 0;

	//get the menu groups
	hr = acadApp->get_MenuGroups(&mnuGrps);
	if (FAILED(hr))
	{
		acadApp->Release();
		return false;
	}
	mnuGrps->get_Count(&cnt);


	//get AutoCAD menu group. say it is index 0.
	IAcadMenuGroup *mnuGrp = NULL;

	VARIANT  vtName;
	vtName.vt = VT_I4;
	BSTR  grpName;
	bool found = false;
	for (long i = 0; i < cnt; i++)
	{
		vtName.lVal = i;
		hr = mnuGrps->Item(vtName, &mnuGrp);
		if (FAILED(hr))
			return false;


		hr = mnuGrp->get_Name(&grpName);
		CString cgrpName(grpName);
		if (cgrpName.CompareNoCase(_T("Acad")) == 0)
		{
			found = true;
			*pVal = mnuGrp;
			break;
		}
	}

	acadApp->Release();
	return found;
}

void CreateToolbars()
{
	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return;
	//now get all the popup menus 
	IAcadToolbars  *tlbrs = NULL;
	HRESULT hr = S_OK;
	hr = mnuGrp->get_Toolbars(&tlbrs);
	mnuGrp->Release();
	//let us create toolbars for polysamp
	IAcadToolbar  *tlbr = NULL;
	hr = tlbrs->Add(L"道路智能生成工具", &tlbr);
	if FAILED(hr)
		return;
	tlbrs->Release();
	//now add toolbar buttons
	IAcadToolbarItem *button1 = NULL;

	VARIANT index;
	index.vt = VT_I4;
	index.lVal = 100l;

	VARIANT vtFalse;
	vtFalse.vt = VT_BOOL;
	vtFalse.boolVal = VARIANT_FALSE;
	CString csPath(CadCommon::ArxHelper::GetArxPluginPath().c_str());
	hr = tlbr->AddToolbarButton(index, L"道路数据提取", L"提取要绘制的道路周围信息", L"_RoadData ", vtFalse, &button1);
	hr = button1->SetBitmaps(CComBSTR(csPath + _T("\\Icon\\RoadData.ico")), CComBSTR(csPath + _T("\\Icon\\RoadData.ico")));

	button1->Release();

	IAcadToolbarItem* button2 = NULL;
	hr = tlbr->AddToolbarButton(index, L"获取计算结果", L"得到计算的相关结构", L"_ResultData ", vtFalse, &button2);
	hr = button2->SetBitmaps(CComBSTR(csPath + _T("\\Icon\\ResultData.ico")), CComBSTR(csPath + _T("\\Icon\\ResultData.ico")));
	button2->Release();


	//hr = tlbr->AddToolbarButton(index, L"Matlab测试", L"Matlab测试", L"_MATTEST ", vtFalse, &button);
	//hr = button->SetBitmaps(CComBSTR(csPath + _T("\\TerrainCuttingTool\\Icon\\TerrainCuttingTool.ico")), CComBSTR(csPath + _T("\\TerrainCuttingTool\\Icon\\TerrainCuttingTool.ico")));
	//	button->Release();
	// #endif
	tlbr->Dock(acToolbarDockLeft);
	tlbr->Release();
	return;
}

void CleanUpToolbars()
{
	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return;


	IAcadToolbars  *tlbrs = NULL;
	HRESULT hr = S_OK;

	hr = mnuGrp->get_Toolbars(&tlbrs);
	mnuGrp->Release();

	long cnt = 0;
	hr = tlbrs->get_Count(&cnt);

	IAcadToolbar *pPumpingStorageCadToolbr = NULL;
	BSTR  tempName;

	VARIANT vtName;
	for (long i = 0; i < cnt; i++)
	{
		vtName.vt = VT_I4;
		vtName.lVal = i;
		hr = tlbrs->Item(vtName, &pPumpingStorageCadToolbr);
		hr = pPumpingStorageCadToolbr->get_Name(&tempName);
		CString tlbrName(tempName);
		SysFreeString(tempName);
		if (tlbrName.CompareNoCase(_T("道路智能生成工具")) == 0)
		{
			pPumpingStorageCadToolbr->Delete();
			//can not call following because delete has already removed it.
			//    polyTlbr->Release();
			break;
		}
		else
			pPumpingStorageCadToolbr->Release();

	}
	tlbrs->Release();
	return;
}


Result result;
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CIntellectRoadDesignApp : public AcRxArxApp {

public:

	CIntellectRoadDesignApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		CreateToolbars();
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		CleanUpToolbars();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
		//----- Self-register COM server upon loading.
		/*if ( FAILED(::DllRegisterServer ()) )
			acutPrintf (_RXST("Failed to register COM server.\n")) ;*/
	}
	
	static void NBIMRCRoadData() {
		CAcModuleResourceOverride myResources;
		RoadUI dlg;
		dlg.DoModal();
		if(dlg.isOK)
			result = IntellectRoadDesignCmd::getInstace()->GetRoadData(dlg.departLength, dlg.maxSlope);
	}

	static void NBIMRCResultData(){
		CAcModuleResourceOverride myResources;
		ResultUI resultDlg;
		resultDlg.roadDis = result.roadDis;
		resultDlg.mountainDis = result.mountainDis;
		resultDlg.DoModal();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CIntellectRoadDesignApp)
ACED_ARXCOMMAND_ENTRY_AUTO(CIntellectRoadDesignApp, NBIMRC, RoadData, RoadData, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL) // 编号
ACED_ARXCOMMAND_ENTRY_AUTO(CIntellectRoadDesignApp, NBIMRC, ResultData, ResultData, ACRX_CMD_SESSION | ACRX_CMD_MODAL, NULL)
