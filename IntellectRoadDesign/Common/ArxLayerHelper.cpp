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

	//�������δ�����ͼ��
	std::vector<LayerInfo*> ArxLayerHelper::GetAllLayerWithoutFreeze(AcDbDatabase* pdb)
	{
		std::vector<LayerInfo*> vecLayerInfo;
		AcDbLayerTable *pLayerTbl;
		//��ȡ��ǰ������ͼ�����ݿ�
		AcDbDatabase* pcCurDB = pdb == NULL ?  acdbHostApplicationServices()
			->workingDatabase() : pdb;
		//��ֻ����ʽ��ȡͼ���
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForRead);
		//����¼
		AcDbLayerTableRecord *pLayerTableRecord = NULL;
		//ʹ�ò��������������
		AcDbLayerTableIterator* ptbItor = NULL; 
		if(pLayerTbl->newIterator(ptbItor) == Acad::eOk)
		{
			for( ;!ptbItor->done(); ptbItor->step()) 
			{
				//ѭ����ֻ����ʽ��ȡ����¼
				if(Acad::eOk == ptbItor->getRecord(pLayerTableRecord, AcDb::kForRead))
				{
					//��ȡͼ����
					ACHAR* locName;
					pLayerTableRecord->getName(locName);
					if(pLayerTableRecord->isFrozen())
					{
						continue;
					}	
					LayerInfo* playInfo = new LayerInfo();
					//��ȡͼ��id
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
		//��������¼
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTblRcd = new AcDbLayerTableRecord();
		pLayerTblRcd->setName(pLayerName);	
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//���½��Ĳ���¼��ӵ������
		AcDbObjectId layerTblRcdId;
		Acad::ErrorStatus es = pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
		if(es == Acad::eOk)
		{
			//����ͼ�εĵ�ǰͼ��
			acdbHostApplicationServices()
				->workingDatabase()
				->setClayer(layerTblRcdId);
		}
		//�رղ��
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
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);


		//�Ƿ��Ѿ�����ָ���Ĳ���¼
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
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//�Ƿ��Ѿ�����ָ���Ĳ���¼
		if (pLayerTbl->has(pLayerName)) {
			pLayerTbl->getAt(pLayerName, objId);
		}
		else
		{
			//��������¼
			AcDbLayerTableRecord *pLayerTblRcd;
			pLayerTblRcd = new AcDbLayerTableRecord();
			pLayerTblRcd->setName(pLayerName);
			
			// AcCmColor curColor;
			// curColor.setColorIndex(130);
			// pLayerTblRcd->setColor(curColor);
			//���½��Ĳ���¼��ӵ������
			AcDbObjectId layerTblRcdId;
			pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
// 			//����ͼ�εĵ�ǰͼ��
// 			acdbHostApplicationServices()
// 				->workingDatabase()
// 				->setClayer(layerTblRcdId);
			//�رղ��
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

		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);

		//�Ƿ��Ѿ�����ָ���Ĳ���¼
		if (pLayerTbl->has(pLayerName)) {
			pLayerTbl->getAt(pLayerName, objId);
		}
		else
		{
			//��������¼
			AcDbLayerTableRecord *pLayerTblRcd;
			pLayerTblRcd = new AcDbLayerTableRecord();
			pLayerTblRcd->setName(pLayerName);
			
			AcCmColor curColor;
			curColor.setColorIndex(colorIdx);
			pLayerTblRcd->setColor(curColor);
			//���½��Ĳ���¼��ӵ������
			AcDbObjectId layerTblRcdId;
			pLayerTbl->add(layerTblRcdId, pLayerTblRcd);
// 			//����ͼ�εĵ�ǰͼ��
// 			acdbHostApplicationServices()
// 				->workingDatabase()
// 				->setClayer(layerTblRcdId);
			//�رղ��
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
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//�ж��Ƿ����ָ�����ƵĲ���¼
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//���ָ������¼��ָ��
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		AcCmColor color;
		color.setColorIndex(colorIndex);
		pLayerTblRcd->setColor(color);
		//�رղ��
		pLayerTblRcd->close();
		pLayerTbl->close();
	}

	void ArxLayerHelper::SetLayerIsOff(std::wstring layerName,bool bIsOff, AcDbDatabase* pdb )
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;

		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//�ж��Ƿ����ָ�����ƵĲ���¼
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//���ָ������¼��ָ��
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		pLayerTblRcd->setIsOff(bIsOff);
		//�رղ��
		pLayerTblRcd->close();
		pLayerTbl->close();
		RefreshView();
	}

	void ArxLayerHelper::DeleteLayer(std::wstring layerName, AcDbDatabase* pdb)
	{
		CString cstrlayerName = layerName.c_str();
		ACHAR* pLayerName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable *pLayerTbl;
		AcDbDatabase* pcCurDB = pdb == NULL ? 		acdbHostApplicationServices()
			->workingDatabase() : pdb;
		pcCurDB->getLayerTable(pLayerTbl, AcDb::kForWrite);
		//�ж��Ƿ����ָ�����ƵĲ���¼
		if (!pLayerTbl->has(pLayerName)) {
			pLayerTbl->close();
			return;
		}
		//���ָ������¼��ָ��
		AcDbLayerTableRecord *pLayerTblRcd;
		pLayerTbl->getAt(pLayerName, pLayerTblRcd, AcDb::kForWrite);
		//Ϊ�����á�ɾ�������
		pLayerTblRcd->erase();
		pLayerTblRcd->close();		//�رղ��
		pLayerTbl->close();
	}
}