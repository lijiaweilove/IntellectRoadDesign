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
	//获得当前图形的层表
	AcDbLayerTable *pLayerTbl;
	AcDbDatabase* pcCurDB = acdbHostApplicationServices()->workingDatabase();
	Acad::ErrorStatus errstat = pcCurDB->getLayerTable(pLayerTbl, AcDb::kForRead);

	AcDbObjectId entityLayerId;
	entityLayerId.setNull();
		
	//如果没有指定阵列范围名称的层表，返回空列表
	if (!pLayerTbl->has(entityLayerName)) {
		pLayerTbl->close();
		return vecEntity;
	}

	pLayerTbl->getAt(entityLayerName, entityLayerId);
	pLayerTbl->close();

	ads_name ssname;
	struct resbuf eb1;
	eb1.restype = 8;//将DXF组码设为8，对应类型为图层名
	eb1.resval.rstring = entityLayerName;
	eb1.rbnext = NULL;

	if(range) { //如果范围非空
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
			if(curPt == prePt)	continue; // 去重
			ads_point curAdsPt = {curPt.x, curPt.y, 0};
			curPtNode->rbnext = acutBuildList(RTPOINT, curAdsPt, RTNONE);
			curPtNode = curPtNode->rbnext;
			prePt = curPt;
		}
		//避免野指针
		curPtNode->rbnext = NULL;
		curPtNode = NULL;
		acedSSGet(L"CP", pointList, NULL, &eb1, ssname);
	}
	else	acedSSGet(L"X", NULL, NULL, &eb1, ssname);
	
	//获取选择集中实体数目，保存到len
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);

	//遍历选择集
	for (int index = 0; index < len; ++index)
	{
		//获取选择集中实体名称，保存于entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, index, entname))
		{
			//根据实体名称获取实体id
			AcDbObjectId mp_Ref_Id;
			if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entname))
			{
				//获取实体对象
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
		//获取选择集中实体名称，保存于entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, idx, entname)) {
			//根据实体名称获取实体id
			AcDbObjectId entId;
			if (Acad::eOk == acdbGetObjectId(entId, entname)) {
				//获取实体对象
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
 * 获取指定多段线范围内的指定图层所有多段线
 * @param entityLayerName: 图层名所包含的字串
 * @param negateLayerName：一定不存在图层名的字串
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
			//分解块，产生的实体保存到structSet
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
 * 获取指定多段线范围内的指定图层所有实体
 * @param entityLayerName: 图层名所包含的字串
 * @param negateLayerName：一定不存在图层名的字串
*/
std::vector<AcDbEntity*> Utils::getEntityByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName, AcDbPolyline* range) {
	std::vector<AcDbEntity*> vecEntity;

	ads_name ssname;

	if(range) { //如果范围非空
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
			if(curPt == prePt)	continue; // 去重
			ads_point curAdsPt = {curPt.x, curPt.y, 0};
			curPtNode->rbnext = acutBuildList(RTPOINT, curAdsPt, RTNONE);
			curPtNode = curPtNode->rbnext;
			prePt = curPt;
		}
		//避免野指针
		curPtNode->rbnext = NULL;
		curPtNode = NULL;
		acedSSGet(L"CP", pointList, NULL, NULL, ssname);
	}
	else	acedSSGet(L"X", NULL, NULL, NULL, ssname);

	//获取选择集中实体数目，保存到len
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);
	// acutPrintf(L"实体数量：%d\n", len);

	//遍历选择集
	for (int index = 0; index < len; ++index)
	{
		//获取选择集中实体名称，保存于entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, index, entname))
		{
			//根据实体名称获取实体id
			AcDbObjectId mp_Ref_Id;
			if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entname))
			{
				//获取实体对象
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

