#include "StdAfx.h"
#include "ArxLayerHelper.h"

namespace CadCommon
{

	std::vector<LayerInfo*> ArxLayerHelper::GetAllLayer(AcDbDatabase* pdb)
	{
		std::vector<LayerInfo*> vecLayerInfo;
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ?  acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForRead);
		AcDbLayerTableRecord *pLayerTableRecord = NULL;
	    AcDbLayerTableIterator* ptbItor = NULL; 
		if(pLayerTbl->newIterator(ptbItor) == Acad::eOk)
		{
			for( ;!ptbItor->done(); ptbItor->step()) 
			{
				if(Acad::eOk == ptbItor->getRecord(pLayerTableRecord, AcDb::kForRead))
				{
				    ACHAR* locName;
					pLayerTableRecord->getName(locName);

					LayerInfo* playInfo = new LayerInfo();
					playInfo->_entId = pLayerTableRecord->id();
					playInfo->_name = locName;
					pLayerTableRecord->close();
					vecLayerInfo.push_back(playInfo);
				}
			}
			delete ptbItor;
		}
		pLayerTbl->close();
		return vecLayerInfo;
	}

	//获得所有未冻结的图层
	std::vector<LayerInfo*> ArxLayerHelper::GetAllLayerWithoutFreeze(AcDbDatabase* pdb)
	{
		std::vector<LayerInfo*> vecLayerInfo;
		AcDbLayerTable *pLayerTbl;
		//获取当前工作的图形数据库
		AcDbDatabase* pcCurDB = pdb == NULL ?  acdbHostApplicationServices()
			->workingDatabase() : pdb;
		//以只读方式获取图层表
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForRead);
		//层表记录
		AcDbLayerTableRecord *pLayerTableRecord = NULL;
		//使用层表迭代器遍历层表
		AcDbLayerTableIterator* ptbItor = NULL; 
		if(pLayerTbl->newIterator(ptbItor) == Acad::eOk)
		{
			for( ;!ptbItor->done(); ptbItor->step()) 
			{
				//循环以只读方式获取层表记录
				if(Acad::eOk == ptbItor->getRecord(pLayerTableRecord, AcDb::kForRead))
				{
					//获取图层名
					ACHAR* locName;
					pLayerTableRecord->getName(locName);
					if(pLayerTableRecord->isFrozen())
					{
						continue;
					}	
					LayerInfo* playInfo = new LayerInfo();
					//获取图层id
					playInfo->_entId = pLayerTableRecord->id();
					playInfo->_name = locName;
					pLayerTableRecord->close();
					vecLayerInfo.push_back(playInfo);
				}
			}
			delete ptbItor;
		}
		pLayerTbl->close();
		return vecLayerInfo;
	}



	AcDbObjectId ArxLayerHelper::CreateLayer(std::wstring layerName, bool bIsHiden ,AcDbDatabase* pdb )
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//创建层表记录
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTblRcd = new AcDbLayerTableRecord();
		pLayerTblRcd->setName(pLayerName);	
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//将新建的层表记录添加到层表中
		AcDbObjectId layerTblRcdId;
		Acad::ErrorStatus es = pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
		if(es == Acad::eOk)
		{
			//设置图形的当前图层
			acdbHostApplicationServices()
				->workingDatabase()
				->setClayer(layerTblRcdId);
		}
		//关闭层表
		pLayerTblRcd->setIsOff(bIsHiden);
		pLayerTblRcd->close();
		pLayerTbl->close();
		return layerTblRcdId;
	}
	bool ArxLayerHelper::IsAlreadyExitLayer(std::wstring layerName, AcDbDatabase* pdb )
	{
		bool bRet = false;
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);


		//是否已经包含指定的层表记录
		if (pLayerTbl->has(pLayerName)) {
			bRet = true;
		}
		pLayerTbl->close();
		return bRet;
	}
	AcDbObjectId ArxLayerHelper::GetLayerId(std::wstring layerName, AcDbDatabase* pdb)// Adesk::UInt16 colorIndex,
	{
		AcDbObjectId objId;
		objId.setNull();
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//是否已经包含指定的层表记录
		if (pLayerTbl->has(pLayerName)) {
			pLayerTbl->getAt(pLayerName, objId);
		}
		else
		{
			//创建层表记录
			AcDbLayerTableRecord *pLayerTblRcd;
			pLayerTblRcd = new AcDbLayerTableRecord();
			pLayerTblRcd->setName(pLayerName);
			
			// AcCmColor curColor;
			// curColor.setColorIndex(130);
			// pLayerTblRcd->setColor(curColor);
			//将新建的层表记录添加到层表中
			AcDbObjectId layerTblRcdId;
			pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
// 			//设置图形的当前图层
// 			acdbHostApplicationServices()
// 				->workingDatabase()
// 				->setClayer(layerTblRcdId);
			//关闭层表
			pLayerTblRcd->close();
			//pLayerTbl->close();
			objId = layerTblRcdId;

		}
		pLayerTbl->close();
		return objId;
	}

	AcDbObjectId ArxLayerHelper::GetLayerId(std::wstring layerName, int colorIdx, AcDbDatabase* pdb)// Adesk::UInt16 colorIndex,
	{
		AcDbObjectId objId;
		objId.setNull();
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();

		acDocManager->lockDocument(acDocManager->curDocument());

		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//是否已经包含指定的层表记录
		if (pLayerTbl->has(pLayerName)) {
			pLayerTbl->getAt(pLayerName, objId);
		}
		else
		{
			//创建层表记录
			AcDbLayerTableRecord *pLayerTblRcd;
			pLayerTblRcd = new AcDbLayerTableRecord();
			pLayerTblRcd->setName(pLayerName);
			
			AcCmColor curColor;
			curColor.setColorIndex(colorIdx);
			pLayerTblRcd->setColor(curColor);
			//将新建的层表记录添加到层表中
			AcDbObjectId layerTblRcdId;
			pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
// 			//设置图形的当前图层
// 			acdbHostApplicationServices()
// 				->workingDatabase()
// 				->setClayer(layerTblRcdId);
			//关闭层表
			pLayerTblRcd->close();
			//pLayerTbl->close();
			objId = layerTblRcdId;

		}
		pLayerTbl->close();
		acDocManager->unlockDocument(acDocManager->curDocument());
		return objId;
	}

	AcDbObjectId ArxLayerHelper::GetLayerId2(std::wstring name,AcDbDatabase* pdb)
 {
  if(IsAlreadyExitLayer(name,pdb))
  {
   return GetLayerId(name,pdb);
  }
  else
  {
   return CreateLayer(name,false,pdb);  
  }
 }

	void ArxLayerHelper::SetLayerColor(std::wstring layerName, uint colorIndex, AcDbDatabase* pdb )
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//判断是否包含指定名称的层表记录
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//获得指定层表记录的指针
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		AcCmColor color;
		color.setColorIndex(colorIndex);
		pLayerTblRcd->setColor(color);
		//关闭层表
		pLayerTblRcd->close();
		pLayerTbl->close();
	}

	void ArxLayerHelper::SetLayerIsOff(std::wstring layerName,bool bIsOff, AcDbDatabase* pdb )
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//判断是否包含指定名称的层表记录
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//获得指定层表记录的指针
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		pLayerTblRcd->setIsOff(bIsOff);
		//关闭层表
		pLayerTblRcd->close();
		pLayerTbl->close();
		RefreshView();
	}

	void ArxLayerHelper::DeleteLayer(std::wstring layerName, AcDbDatabase* pdb)
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//获得当前图形的层表
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//判断是否包含指定名称的层表记录
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//获得指定层表记录的指针
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		//为其设置“删除”标记
		pLayerTblRcd->erase();
		pLayerTblRcd->close();		//关闭层表
		pLayerTbl->close();
	}
}