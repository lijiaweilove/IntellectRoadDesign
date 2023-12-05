#include "StdAfx.h"
#include "Utils.h"

Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}

std::vector<AcDbPolyline*> Utils::getBorders(wchar_t* entityLayerName, AcDbPolyline* range) {
	std::vector<AcDbPolyline*> vecBorder;
	std::vector<AcDbEntity*> vecEntity = getEntitys(entityLayerName, range);
	for(std::vector<AcDbEntity*>::iterator it = vecEntity.begin(); it != vecEntity.end(); ++it) {
		AcDbEntity* pEnt = *it;
		if(pEnt->isA() == AcDbPolyline::desc()) {
			vecBorder.push_back(AcDbPolyline::cast(pEnt));
		}
		pEnt->close();
	}
	
	return vecBorder;
}

std::vector<AcDbEntity*> Utils::getEntitys(wchar_t* entityLayerName, AcDbPolyline* range) {
	std::vector<AcDbEntity*> vecEntity;
	//��õ�ǰͼ�εĲ��
	AcDbLayerTable *pLayerTbl;
	AcDbDatabase* pcCurDB = acdbHostApplicationServices()->workingDatabase();
	Acad::ErrorStatus errstat = pcCurDB->getLayerTable(pLayerTbl, AcDb::kForRead);

	AcDbObjectId entityLayerId;
	entityLayerId.setNull();
		
	//���û��ָ�����з�Χ���ƵĲ�����ؿ��б�
	if (!pLayerTbl->has(entityLayerName)) {
		pLayerTbl->close();
		return vecEntity;
	}

	pLayerTbl->getAt(entityLayerName, entityLayerId);
	pLayerTbl->close();

	ads_name ssname;
	struct resbuf eb1;
	eb1.restype = 8;//��DXF������Ϊ8����Ӧ����Ϊͼ����
	eb1.resval.rstring = entityLayerName;
	eb1.rbnext = NULL;

	if(range) { //�����Χ�ǿ�
		unsigned int ptNum = range->numVerts();
		struct resbuf* pointList = NULL;
		struct resbuf* curPtNode = NULL;
		AcGePoint2d prePt, curPt;
		for(int ptIdx = 0; ptIdx < ptNum; ++ptIdx) {
			if(pointList == NULL) {
				range->getPointAt(ptIdx, curPt);
				ads_point curAdsPt = {curPt.x, curPt.y, 0};
				pointList = acutBuildList(RTPOINT, curAdsPt, RTNONE);
				curPtNode = pointList;
				prePt = curPt;
				continue;
			}
			range->getPointAt(ptIdx, curPt);
			if(curPt == prePt)	continue; // ȥ��
			ads_point curAdsPt = {curPt.x, curPt.y, 0};
			curPtNode->rbnext = acutBuildList(RTPOINT, curAdsPt, RTNONE);
			curPtNode = curPtNode->rbnext;
			prePt = curPt;
		}
		//����Ұָ��
		curPtNode->rbnext = NULL;
		curPtNode = NULL;
		acedSSGet(L"CP", pointList, NULL, &eb1, ssname);
	}
	else	acedSSGet(L"X", NULL, NULL, &eb1, ssname);
	
	//��ȡѡ����ʵ����Ŀ�����浽len
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);

	//����ѡ��
	for (int index = 0; index < len; ++index)
	{
		//��ȡѡ����ʵ�����ƣ�������entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, index, entname))
		{
			//����ʵ�����ƻ�ȡʵ��id
			AcDbObjectId mp_Ref_Id;
			if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entname))
			{
				//��ȡʵ�����
				AcDbEntity* pEnt = NULL;
				if(Acad::eOk == acdbOpenAcDbEntity(pEnt,mp_Ref_Id,AcDb::kForRead))
				{
					vecEntity.push_back(pEnt);
				}
			}
		}
	}
	acedSSFree(ssname);
	return vecEntity;
}

std::vector<AcDbPolyline*>  Utils::getBorders(const ads_name& ssname) {
	std::vector<AcDbPolyline*> vecBorder;
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);
	for(int idx = 0; idx < len; ++idx) {
		//��ȡѡ����ʵ�����ƣ�������entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, idx, entname)) {
			//����ʵ�����ƻ�ȡʵ��id
			AcDbObjectId entId;
			if (Acad::eOk == acdbGetObjectId(entId, entname)) {
				//��ȡʵ�����
				AcDbEntity* pEnt = NULL;
				if(Acad::eOk == acdbOpenAcDbEntity(pEnt, entId, AcDb::kForRead)) {
					if(pEnt->isA() == AcDbPolyline::desc())
						vecBorder.push_back(AcDbPolyline::cast(pEnt));
				}
				pEnt->close();
			}
		}
	}
	return vecBorder;
}

/**
 * ��ȡָ������߷�Χ�ڵ�ָ��ͼ�����ж����
 * @param entityLayerName: ͼ�������������ִ�
 * @param negateLayerName��һ��������ͼ�������ִ�
*/
std::vector<AcDbPolyline*> Utils::getBordersByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName, AcDbPolyline* range) {
	std::vector<AcDbPolyline*> vecBorder;
	std::vector<AcDbEntity*> vecEntity = getEntityByLayer(entityLayerName, negateLayerName, range);
	for(std::vector<AcDbEntity*>::iterator it = vecEntity.begin(); it != vecEntity.end(); ++it) {
		AcDbEntity* pEnt = *it;
		if(pEnt->isA() == AcDbPolyline::desc()) {
			vecBorder.push_back(AcDbPolyline::cast(pEnt));
		}
		else if(pEnt->isA() == AcDbBlockReference::desc()) {
			AcDbBlockReference* pBlockRefrence = AcDbBlockReference::cast(pEnt);
			AcDbVoidPtrArray structSet;
			//�ֽ�飬������ʵ�屣�浽structSet
			pBlockRefrence->explode(structSet);
			for (int i = 0; i < structSet.length(); ++i) {
				AcDbEntity* pExplodeEnt = (AcDbEntity *)structSet.at(i);
				ACHAR* plName = pExplodeEnt->layer();
				std::wstring plNameStr = plName;
				if(pExplodeEnt->isA() == AcDbPolyline::desc() &&  (plNameStr.find(entityLayerName) != std::wstring::npos)) {
					if((negateLayerName && plNameStr.find(negateLayerName) == std::wstring::npos) || !negateLayerName)
						vecBorder.push_back(AcDbPolyline::cast(pExplodeEnt));
				}
				pExplodeEnt->close();
			}
		}
		pEnt->close();
	}
	return vecBorder;
}

/**
 * ��ȡָ������߷�Χ�ڵ�ָ��ͼ������ʵ��
 * @param entityLayerName: ͼ�������������ִ�
 * @param negateLayerName��һ��������ͼ�������ִ�
*/
std::vector<AcDbEntity*> Utils::getEntityByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName, AcDbPolyline* range) {
	std::vector<AcDbEntity*> vecEntity;

	ads_name ssname;

	if(range) { //�����Χ�ǿ�
		unsigned int ptNum = range->numVerts();
		struct resbuf* pointList = NULL;
		struct resbuf* curPtNode = NULL;
		AcGePoint2d prePt, curPt;
		for(int ptIdx = 0; ptIdx < ptNum; ++ptIdx) {
			if(pointList == NULL) {
				range->getPointAt(ptIdx, curPt);
				ads_point curAdsPt = {curPt.x, curPt.y, 0};
				pointList = acutBuildList(RTPOINT, curAdsPt, RTNONE);
				curPtNode = pointList;
				prePt = curPt;
				continue;
			}
			range->getPointAt(ptIdx, curPt);
			if(curPt == prePt)	continue; // ȥ��
			ads_point curAdsPt = {curPt.x, curPt.y, 0};
			curPtNode->rbnext = acutBuildList(RTPOINT, curAdsPt, RTNONE);
			curPtNode = curPtNode->rbnext;
			prePt = curPt;
		}
		//����Ұָ��
		curPtNode->rbnext = NULL;
		curPtNode = NULL;
		acedSSGet(L"CP", pointList, NULL, NULL, ssname);
	}
	else	acedSSGet(L"X", NULL, NULL, NULL, ssname);

	//��ȡѡ����ʵ����Ŀ�����浽len
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);
	// acutPrintf(L"ʵ��������%d\n", len);

	//����ѡ��
	for (int index = 0; index < len; ++index)
	{
		//��ȡѡ����ʵ�����ƣ�������entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, index, entname))
		{
			//����ʵ�����ƻ�ȡʵ��id
			AcDbObjectId mp_Ref_Id;
			if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entname))
			{
				//��ȡʵ�����
				AcDbEntity* pEnt = NULL;
				if(Acad::eOk == acdbOpenAcDbEntity(pEnt,mp_Ref_Id,AcDb::kForRead))
				{
					ACHAR* lName = pEnt->layer();
					std::wstring lNameStr = lName;
					acutDelString(lName); 
					if(negateLayerName) {
						if(lNameStr.find(entityLayerName) != std::wstring::npos && lNameStr.find(negateLayerName) == std::wstring::npos) {
							vecEntity.push_back(pEnt);
						}
					}
					else {
						if(lNameStr.find(entityLayerName) != std::wstring::npos) {
							vecEntity.push_back(pEnt);
						}
					}
				}
			}
		}
	}
	acedSSFree(ssname);
	return vecEntity;
}

