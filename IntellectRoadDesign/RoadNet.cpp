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

// ������С�������㷨
void RoadNet::kruskal(int n, vector<Edge*>& edge, vector<Edge>& result)
{
	//acutPrintf(_T("\n ����Ϊ��%d"), n);
	vector<int> father;
	father.resize(n);
	std::sort(edge.begin(), edge.end(), compare);
	for (int i = 0; i < n; ++i) {
		father[i] = i;
	}
	for (int i = 0; i < edge.size() && result.size() < n - 1; ++i) {
		int u = edge[i]->u, v = edge[i]->v;
		if (findfather(u, father) != findfather(v, father)) { //�жϸ��ڵ��Ƿ���ͬ
			result.push_back(Edge(*edge[i]));
			//result[result.size() - 1].result = edge[i]->result;
			father[findfather(u, father)] = father[findfather(v, father)]; //�����㲢��һ��������
		}
	}
	//if (result.size() != n - 1) {
	//	acutPrintf(_T("\n ��ͼ����ͨ"));
	//}
}

// �������߶�
void RoadNet::createPolyLine(AcDbPolyline* pPolyline) {
	AcDbBlockTable* pBlockTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);

	acDocManager->lockDocument(acDocManager->curDocument());
	AcDbBlockTableRecord* pModelSpace = NULL;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite);

	// �����߶���ӵ�ģ�Ϳռ�
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

	// �����߶���ӵ�ģ�Ϳռ�
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
	// ͼ����˿��Ʒ�
	bool bFilterLayer = false;
	AcDbObjectId layerId;
	// ���������ͼ����
	if (layer != NULL) {	// ��ȡ���->�ж��Ƿ��и�ͼ��->��ò��id->���ù��˿��Ʒ�
		AcDbLayerTable* pLayerTbl = NULL;
		pDb->getLayerTable(pLayerTbl, AcDb::kForRead);
		// �ݴ�����������������ͼ�㣬�رղ�����ؿ�id�б�
		if (!pLayerTbl->has(layer)) {
			pLayerTbl->close();
			return entIds;
		}
		pLayerTbl->getAt(layer, layerId);
		pLayerTbl->close();
		bFilterLayer = true;
	}

	// ��ȡ���ݿ���->���ݿ����¼->�رտ��
	AcDbBlockTable* pBlkTbl = NULL;
	pDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkTblRcd = NULL;
	pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	pBlkTbl->close();

	// �������ݿ����¼
	AcDbBlockTableRecordIterator* pItr = NULL;
	pBlkTblRcd->newIterator(pItr);
	for (pItr->start(); !pItr->done(); pItr->step())
	{	// ������˿��Ʒ�Ϊ�棬��øò�������ʵ���id
		if (bFilterLayer == true)
		{	// ���ÿ��ʵ���ָ��
			AcDbEntity* pEnt;
			pItr->getEntity(pEnt, AcDb::kForRead);
			// ���ʵ���ͼ��Ϊָ�����˵�ͼ��
			if (pEnt->layerId() == layerId) {	// ��ʵ��id��ӽ�id�б� entIds ��
				entIds.append(pEnt->objectId());
			}
			pEnt->close();
		}
		else {
			// ������ʵ��id��ӽ�id�б� entIds ��
			AcDbObjectId objId;
			pItr->getEntityId(objId);
			entIds.append(objId);
		}
	}

	delete pItr;
	pBlkTblRcd->close();
	return entIds;
}