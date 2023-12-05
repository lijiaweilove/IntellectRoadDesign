#include "stdafx.h"
#include "XDataHelper.h"
#include "EleData.h"
namespace CadCommon {

	bool XDataHelper::GetAppXdata(const AcDbEntity* pEntity, std::wstring appname,std::wstring& wcstrName)
	{
		wcstrName = L"";
		struct resbuf *pRb = pEntity->xData(appname.c_str());
		std::string kks;
		if (pRb != NULL)
		{
		  resbuf* ptemp = pRb->rbnext;
		  CString cstrName = ptemp->resval.rstring;
		  wcstrName = cstrName.GetString();
		}
		acutRelRb(pRb);
		return true;
	}

	bool XDataHelper::GetAppXdata(Adesk::IntDbId  intDbId, std::wstring appname, std::wstring& cstrName)
	{
		AcDbObjectId objectId;
		objectId.setFromOldId(intDbId);
		return GetAppXdata(objectId, appname, cstrName);
	}

	bool XDataHelper::GetAppXdata(AcDbObjectId  objectId, std::wstring appname, std::wstring& cstrName)
	{
		bool bRet = false;
		AcDbEntity* pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, objectId, AcDb::kForRead))
		{//打开选择中选择集中的对象  
			bRet = GetAppXdata(pEnt, appname, cstrName);
			pEnt->close();
		}
		return bRet;
	}

	bool XDataHelper::AddAppXdataByOldId(Adesk::IntDbId intDbId, std::wstring appname,std::wstring const& cstrName, bool removeOldId)
	{
		AcDbObjectId objectId;
		objectId.setFromOldId(intDbId);
		return AddAppXdataByOldId(objectId, appname,cstrName,removeOldId);
	}
	bool XDataHelper::AddAppXdataByOldId(AcDbObjectId objectId, std::wstring appname, std::wstring const& cstrName, bool removeOldId)
	{
		bool bRet = false;
		AcDbEntity* pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, objectId, AcDb::kForWrite))
		{//打开选择中选择集中的对象  
			bRet = AddAppXdataByOldId(pEnt, appname, cstrName,removeOldId);
			pEnt->close();
		}
		return bRet;
	}
	bool XDataHelper::AddAppXdataByOldId(AcDbEntity * pEnt, std::wstring appname, std::wstring const& cstrName, bool removeOldId)
	{
		bool bRet = true;
		CString _cstrName = cstrName.c_str();
		struct  resbuf  *pRb = pEnt->xData(appname.c_str());
		if (pRb != NULL)
		{
			if (removeOldId)
			{
				struct resbuf* rb =
					acutBuildList(
						AcDb::kDxfRegAppName,
						appname.c_str(),
						AcDb::kDxfXdAsciiString, _cstrName.GetString(),
						RTNONE);
				pEnt->setXData(rb);
				acutRelRb(rb);
			}
			else
				bRet = false;
		}
		else
			CreateAppName(pEnt, appname, cstrName);
		return bRet;
	}

	void XDataHelper::RemoveXDataByOldId(const Adesk::IntDbId  intDbId, std::wstring appname)
	{
		AcDbObjectId objectId;
		objectId.setFromOldId(intDbId);
		RemoveXDataByObjectId(objectId, appname);
	}

	void XDataHelper::RemoveXDataByObjectId(const AcDbObjectId  objectId, std::wstring appname)
	{
		AcDbEntity* pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, objectId, AcDb::kForWrite))
		{//打开选择中选择集中的对象  
			RemoveXDataByEntity(pEnt, appname);
			pEnt->close();
		}
	}

	void XDataHelper::RemoveXDataByEntity(AcDbEntity* pEnt, std::wstring appname)
	{
		CString cstrTemp = appname.c_str();
		struct resbuf *rb = acutBuildList(AcDb::kDxfRegAppName, cstrTemp, RTNONE);
		pEnt->setXData(rb);
		acutRelRb(rb);
	}

	void XDataHelper::CreateAppName(AcDbEntity * pEnt, std::wstring appname,std::wstring const& cstrName)
	{
		CString cstrAppName = appname.c_str();
		acdbRegApp(cstrAppName);
		CString cstrTemp = cstrName.c_str();
		//创建结果缓冲区链表
		struct resbuf* rb = acutBuildList(AcDb::kDxfRegAppName, cstrAppName,
			//应用程序名称
			AcDb::kDxfXdAsciiString, cstrTemp,
			RTNONE);
		pEnt->setXData(rb);
		acutRelRb(rb);
	}
}