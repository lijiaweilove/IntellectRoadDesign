#include "StdAfx.h"
#include "RoadNet.h"
#include<iostream>
#include<algorithm>

int RoadNet::findfather(int a, vector<int> father)
{
	while (a != father[a]) {
		a = father[a];
	}
	return a;
}

inline bool compare(const Edge* a, const Edge* b) { return a->weight < b->weight; };

// 生成最小生成树算法
void RoadNet::kruskal(int n, vector<Edge*>& edge, vector<Edge>& result)
{
	//acutPrintf(_T("\n 长度为：%d"), n);
	vector<int> father;
	father.resize(n);
	std::sort(edge.begin(), edge.end(), compare);
	for (int i = 0; i < n; ++i) {
		father[i] = i;
	}
	for (int i = 0; i < edge.size() && result.size() < n - 1; ++i) {
		int u = edge[i]->u, v = edge[i]->v;
		if (findfather(u, father) != findfather(v, father)) { //判断父节点是否相同
			result.push_back(Edge(*edge[i]));
			//result[result.size() - 1].result = edge[i]->result;
			father[findfather(u, father)] = father[findfather(v, father)]; //将两点并入一个集合中
		}
	}
	//if (result.size() != n - 1) {
	//	acutPrintf(_T("\n 该图不连通"));
	//}
}

// 创建多线段
void RoadNet::createPolyLine(AcDbPolyline* pPolyline) {
	AcDbBlockTable* pBlockTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbBlockTableRecord* pModelSpace = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite);

	// 将多线段添加到模型空间
	if (pModelSpace->appendAcDbEntity(pPolyline) != Acad::eOk) {
		acutPrintf(_T("Failed to append the line to ModelSpace.\n"));
		pModelSpace->close();
		pBlockTable->close();
		return;
	}
	pModelSpace->close();
	pBlockTable->close();
	acDocManager->unlockDocument(acDocManager->curDocument());
}


void RoadNet::createSpline(AcDbSpline* pSpline) {
	AcDbBlockTable* pBlockTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbBlockTableRecord* pModelSpace = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite);

	// 将多线段添加到模型空间
	if (pModelSpace->appendAcDbEntity(pSpline) != Acad::eOk) {
		acutPrintf(_T("Failed to append the line to ModelSpace.\n"));
		pModelSpace->close();
		pBlockTable->close();
		return;
	}
	pModelSpace->close();
	pBlockTable->close();
	acDocManager->unlockDocument(acDocManager->curDocument());
}


AcDbObjectIdArray RoadNet::GetAllEntIds(const ACHAR* layer, AcDbDatabase* pDb)
{
	AcDbObjectIdArray entIds;
	// 图层过滤控制符
	bool bFilterLayer = false;
	AcDbObjectId layerId;
	// 如果传入了图层名
	if (layer != NULL) {	// 获取层表->判断是否有该图层->获得层表id->设置过滤控制符
		AcDbLayerTable* pLayerTbl = NULL;
		pDb->getLayerTable(pLayerTbl, AcDb::kForRead);
		// 容错函数：如果层表不包含该图层，关闭层表，返回空id列表
		if (!pLayerTbl->has(layer)) {
			pLayerTbl->close();
			return entIds;
		}
		pLayerTbl->getAt(layer, layerId);
		pLayerTbl->close();
		bFilterLayer = true;
	}

	// 获取数据库块表->数据库块表记录->关闭块表
	AcDbBlockTable* pBlkTbl = NULL;
	pDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkTblRcd = NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	pBlkTbl->close();

	// 遍历数据库块表记录
	AcDbBlockTableRecordIterator* pItr = NULL;
	pBlkTblRcd->newIterator(pItr);
	for (pItr->start(); !pItr->done(); pItr->step())
	{	// 如果过滤控制符为真，获得该层上所有实体的id
		if (bFilterLayer == true)
		{	// 获得每个实体的指针
			AcDbEntity* pEnt;
			pItr->getEntity(pEnt, AcDb::kForRead);
			// 如果实体的图层为指定过滤的图层
			if (pEnt->layerId() == layerId) {	// 将实体id添加进id列表 entIds 中
				entIds.append(pEnt->objectId());
			}
			pEnt->close();
		}
		else {
			// 将所有实体id添加进id列表 entIds 中
			AcDbObjectId objId;
			pItr->getEntityId(objId);
			entIds.append(objId);
		}
	}

	delete pItr;
	pBlkTblRcd->close();
	return entIds;
}