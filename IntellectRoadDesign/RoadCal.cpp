#include "StdAfx.h"
#include "RoadCal.h"
#include "Struct.h"
#include <dbmleader.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
CRITICAL_SECTION RoadCal::csLock;
vector<Edge*> RoadCal::edge;



RoadCal::RoadCal(void) : targetNum(-1), row(0), line(0), departLength(-1), layerName(_T("新建路"))
{
	baseData = NULL;
	contourPoints = NULL;
	oldRoadPoints = NULL;
	obstaclePoints = NULL;
	targetPoints = NULL;
}


RoadCal::RoadCal(RoadCal& other)
{
	//InitializeCriticalSection(&csLock);
	row = other.row;
	line = other.line;
	layerName = other.layerName;
	targetNum = other.targetNum;
	departLength = other.departLength;
	downLeftPoint = other.downLeftPoint;

	for (size_t i = 0; i < other.points.size(); i++) {
		AcGePoint3d* newPoint = new AcGePoint3d(*other.points[i]);
		points.push_back(newPoint);
	}
	
	baseData = new CBaseData();
	contourPoints = NULL;
	oldRoadPoints = NULL;
	obstaclePoints = NULL;
	targetPoints = NULL;
}


RoadCal::~RoadCal(void)
{

	for (size_t i = 0; i < points.size(); i++) {
		delete points[i];
		points[i] = NULL;
	}

	if (contourPoints != NULL) {
		for (size_t i = 0; i < contourPoints->size(); i++) {
			for (size_t j = 0; j < (*contourPoints)[i].size(); j++) {
				for (size_t k = 0; k < (*contourPoints)[i][j].size(); k++) {
					if ((*contourPoints)[i][j][k] != NULL) 
						delete (*contourPoints)[i][j][k];
				}
			}
		}
		for (size_t i = 0; i < contourPoints->size(); i++) {
			for (size_t j = 0; j < (*contourPoints)[i].size(); j++) {
				(*contourPoints)[i][j].clear();
			}
		}
		contourPoints->resize(0);
	}
	
	if (oldRoadPoints != NULL) {
		for (size_t i = 0; i < oldRoadPoints->size(); i++) {
			for (size_t j = 0; j < (*oldRoadPoints)[i].size(); j++) {
				for (size_t k = 0; k < (*oldRoadPoints)[i][j].size(); k++) {
					if ((*oldRoadPoints)[i][j][k] != NULL)
						delete (*oldRoadPoints)[i][j][k];
				}
			}
		}
		for (size_t i = 0; i < oldRoadPoints->size(); i++) {
			for (size_t j = 0; j < (*oldRoadPoints)[i].size(); j++) {
				(*oldRoadPoints)[i][j].clear();
			}
		}
		oldRoadPoints->resize(0);
	}
	
	if (obstaclePoints != NULL) {
		for (size_t i = 0; i < obstaclePoints->size(); i++) {
			for (size_t j = 0; j < (*obstaclePoints)[i].size(); j++) {
				for (size_t k = 0; k < (*obstaclePoints)[i][j].size(); k++) {
					if ((*obstaclePoints)[i][j][k] != NULL)
						delete (*obstaclePoints)[i][j][k];
				}
			}
		}
		for (size_t i = 0; i < obstaclePoints->size(); i++) {
			for (size_t j = 0; j < (*obstaclePoints)[i].size(); j++) {
				(*obstaclePoints)[i][j].clear();
			}
		}
		obstaclePoints->resize(0);
	}

	delete baseData;
	delete contourPoints;
	delete oldRoadPoints;
	delete obstaclePoints;
}


double getTargetElevation(AcDbPolyline* pPolyline)
{
	double maxElevation = 0.0;
	AcGePoint3d vertex1, vertex2;
	pPolyline->getPointAt(0, vertex1);
	pPolyline->getPointAt(4, vertex2);
	ads_point pt1, pt2;
	pt1[X] = vertex1.x;
	pt1[Y] = vertex1.y;
	pt1[Z] = vertex1.z;
	pt2[X] = vertex2.x;
	pt2[Y] = vertex2.y;
	pt2[Z] = vertex2.z;
	ads_name ssName;
	if (acedSSGet(_T("C"), pt1, pt2, NULL, ssName) == RTNORM) {
		Adesk::Int32 length;
		acedSSLength(ssName, &length);
		for (int index = 0; index < length; ++index) {
			ads_name entName;
			if (RTNORM == acedSSName(ssName, index, entName)) {  // 从选择集中获取实体名称到entname
				AcDbObjectId mp_Ref_Id;
				if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entName)) {  // 根据实体名称获取实体id
					AcDbEntity* pEnt = NULL;
					if (Acad::eOk == acdbOpenAcDbEntity(pEnt, mp_Ref_Id, AcDb::kForRead)) {  // 获取实体对象
						ACHAR* lName = pEnt->layer();
						std::wstring lNameStr = lName;
						BIM::Tools::lowerWstring(lNameStr);
						if (lNameStr.find(L"首曲线") != std::wstring::npos) {
							if (pEnt->isA() == AcDbPolyline::desc()) {
								AcDbPolyline* pline = AcDbPolyline::cast(pEnt);
								if(pline->elevation() > maxElevation)
									maxElevation = pline->elevation();
							}
						}
						pEnt->close();
					}
				}
			}
		}
	}
	acedSSFree(ssName); // 释放选择集
	return maxElevation;
}

// 获取风机台中多线段的某点坐标
void RoadCal::readDwg()
{
	AcDbObjectIdArray objIds = RoadNet::GetAllEntIds(layerName);
	AcDbObjectId layerId = AcDbObjectId::kNull;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbLayerTable* pLayerTable;
	if (pDb->getLayerTable(pLayerTable, AcDb::kForRead) == Acad::eOk) {
		AcDbObjectId layerTableRecordId;
		if (pLayerTable->getAt(layerName, layerTableRecordId) == Acad::eOk) 
			layerId = layerTableRecordId;
		pLayerTable->close();
	}
	for (int i = 0; i < objIds.length(); i++)
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, objIds.at(i), AcDb::kForRead) == Acad::eOk)
		{
			if (pEnt->isKindOf(AcDbPolyline::desc()))  //判断是否为多段线
			{
				AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
				unsigned int num = pPolyline->numVerts();
				AcGePoint3d tempPoint;
				pPolyline->getPointAt(3, tempPoint);
				AcGePoint3d centerPoint(0.0, 0.0, 0.0);
				for (unsigned int j = 0; j < num; j++){
					AcGePoint3d vertex;
					pPolyline->getPointAt(j, vertex);
					centerPoint += AcGeVector3d(vertex.x, vertex.y, 0.0);
				}
				centerPoint = centerPoint / num;
				centerPoint.z = getTargetElevation(pPolyline);
				pPolyline->close();
				points.push_back(new AcGePoint3d(centerPoint));
				addMLeader(pDb, layerId, tempPoint);
			}
			pEnt->close();
		}
	}
	targetNum = points.size();
}


void RoadCal::DomainSerach(const AcGePoint3d* point1, const AcGePoint3d* point2)
{
	int extand = 160;   //    160

	ads_point pt1, pt2;
	pt1[X] = point1->x;
	pt1[Y] = point1->y;
	pt1[Z] = point1->z;  
	pt2[X] = point2->x;
	pt2[Y] = point2->y;
	pt2[Z] = point2->z;

	//  向外扩展一定的范围
	if (pt1[X] < pt2[X] && pt1[Y] < pt2[Y]) {
		pt1[X] -= extand;
		pt1[Y] -= extand;
		pt2[X] += extand;
		pt2[Y] += extand;
	}			  // 选择集扩展范围
	else if (pt1[X] < pt2[X] && pt1[Y] > pt2[Y]) {
		pt1[X] -= extand;
		pt1[Y] += extand;
		pt2[X] += extand;
		pt2[Y] -= extand;
	}
	else if (pt1[X] > pt2[X] && pt1[Y] > pt2[Y]) {
		pt1[X] += extand;
		pt1[Y] += extand;
		pt2[X] -= extand;
		pt2[Y] -= extand;
	}
	else if(pt1[X] > pt2[X] && pt1[Y] < pt2[Y]){
		pt1[X] += extand;
		pt1[Y] -= extand;
		pt2[X] -= extand;
		pt2[Y] += extand;
	}


	ads_name ssName;
	if (acedSSGet(_T("C"), pt1, pt2, NULL, ssName) == RTNORM) {
		Adesk::Int32 length;
		acedSSLength(ssName, &length);
		for (int index = 0; index < length; ++index) {
			ads_name entName;
			if (RTNORM == acedSSName(ssName, index, entName)) {  // 从选择集中获取实体名称到entname
				AcDbObjectId mp_Ref_Id;
				if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entName)) {  // 根据实体名称获取实体id
					AcDbEntity* pEnt = NULL;
					if (Acad::eOk == acdbOpenAcDbEntity(pEnt, mp_Ref_Id, AcDb::kForRead)) {  // 获取实体对象
						ACHAR* lName = pEnt->layer();
						std::wstring lNameStr = lName;
						BIM::Tools::lowerWstring(lNameStr);
						if (lNameStr.find(L"其他道路") != std::wstring::npos) {
							if (pEnt->isA() == AcDbPolyline::desc()) {
								COldRoad* tempOldRoad = new COldRoad(pEnt);
								baseData->oldRoads.push_back(tempOldRoad);
							}
						}
						else if (lNameStr.find(L"首曲线") != std::wstring::npos) {
							if (pEnt->isA() == AcDbPolyline::desc()) {  // 	CBaseData baseData;
								CContourModel* tempContour = new CContourModel(pEnt);
								baseData->contours.push_back(tempContour);  // 一次只插入一条多线段
							}
						}
						else if ((lNameStr.find(L"宗教文物") != std::wstring::npos) ||
							(lNameStr.find(L"垣栅") != std::wstring::npos) ||
							(lNameStr.find(L"普通房屋") != std::wstring::npos)) {
							CObstacle* tempObstacle = new CObstacle(pEnt);
							baseData->obstacles.push_back(tempObstacle);
						}
						pEnt->close();
					}
				}
			}
		}
	}
	acedSSFree(ssName); // 释放选择集
	//baseData.DataInfo();
}


inline bool compareWeight(Edge e1, Edge e2)
{
	return e1.midWeight < e2.midWeight;        // *********这里调节大小 
}


// 路网规划
void RoadCal::doRoadNetPlan(Result& res)
{
	vector<Edge> result;
	RoadNet roadnet;
	roadnet.kruskal(targetNum, RoadCal::edge, result);
	//std::sort(result.begin(), result.end(), compareWeight);
	double roadDis = 0., mountainDis = 0.;
	size_t i = 0;
	for (i = 0; i < result.size(); i++) {
		roadDis += result[i].roadDis;
		mountainDis += result[i].mountainDis;
	}

	// 删除多余的线
	//for (int i = 0; i < 3; i++)  // 删除midWeight最大的三个
	//	result.pop_back();
	// 查找重复点对应的线，result.result对应的是v(end)->u(start)的点列
	
	for (i = 0; i < points.size(); i++) {
		int count = 0;
		vector<std::pair<int, bool>> temp;
		for (size_t j = 0; j < result.size(); j++) {
			if (result[j].u == i) {
				temp.push_back(std::make_pair(j, true));  // u=true=start
				count++;
			}
			else if (result[j].v == i) {
				temp.push_back(std::make_pair(j, false));  // v=false=end
				count++;
			}
		}
		
		// 存在两条以上的线
		if (count >= 2) {  
			double tempDis = 0., minDis = DBL_MAX /*minDis = 0.*/;
			size_t m = 0;
			int index = -1;
			// 找出最小的dis对应的result
			for (m = 0; m < temp.size(); m++) {
				if (temp[m].second) 
					tempDis = result[temp[m].first].startDis;
				else 
					tempDis = result[temp[m].first].endDis;
				if (tempDis < minDis) {   // *********这里调节大小 
					minDis = tempDis;
					index = m;
				}
			}

			//  删除首尾重复点
			for (m = 0; m < temp.size(); m++) {
				if (m == index)
					continue;
				vector<AcGePoint3d>* prepareDelRes = result[temp[m].first].result;
				if (temp[m].second) {
					prepareDelRes->erase(prepareDelRes->end() - result[temp[m].first].startPointNum,
						prepareDelRes->end());
					mountainDis -= result[temp[m].first].startDis;
				}
				else {
					result[temp[m].first].result->erase(prepareDelRes->begin(),
						prepareDelRes->begin() + result[temp[m].first].endPointNum);
					mountainDis -= result[temp[m].first].endDis;
				}
			}

			//  删除中间重复点
			const double extand = 9.0;  // 查找范围
			for (m = 0; m < temp.size(); m++) {
				if (m == index)
					continue;
				vector<AcGePoint3d>* notDelRes = result[temp[index].first].result;
				vector<AcGePoint3d>* prepareDelRes = result[temp[m].first].result;
				vector<AcGePoint3d> tempDelRes;
				size_t k = 0, l = 0;
				for (k = 0; k < prepareDelRes->size(); ++k) {
					for (l = 0; l < notDelRes->size(); ++l) {
						if ((*prepareDelRes)[k].distanceTo((*notDelRes)[l]) < extand) {
							tempDelRes.push_back((*prepareDelRes)[k]);
							prepareDelRes->erase(prepareDelRes->begin() + k);
							--k;
							break;
						}
					}
				}
				for (k = 0; k + 1 < tempDelRes.size(); k +=2)
					roadDis -= tempDelRes[k].distanceTo(tempDelRes[k + 1]);
				if (temp[m].second) {
					for (k = 0; k < notDelRes->size(); ++k) {
						if ((*notDelRes)[k].distanceTo(prepareDelRes->back()) < extand + 5.0) {
							 prepareDelRes->push_back((*notDelRes)[k]);
							 break;
						}
					}
				}
				else
					for (k = 0; k < notDelRes->size(); ++k) {
						if ((*notDelRes)[k].distanceTo(prepareDelRes->front()) < extand + 5.0){
							prepareDelRes->insert(prepareDelRes->begin(), (*notDelRes)[k]);
							break;
						}
					}
			}
		}
	}

	// 进行多线段绘制
	const int fitInterval = 30; // 间隔点数
	for (size_t i = 0; i < result.size(); i++) {
		AcDbPolyline* pPolyline = new AcDbPolyline();
		std::vector<AcGePoint3d>& polyPoints = *result[i].result;

		AcGePoint3dArray fitPoints;
		for (int j = 0; j < polyPoints.size(); j++) {
			fitPoints.append(AcGePoint3d(polyPoints[j].x, polyPoints[j].y, 0.0));
		}
		AcDbSpline* pSpline = new AcDbSpline(fitPoints);
		pSpline->setLineWeight(AcDb::kLnWt030);
		pSpline->setLayer(layerName);
		roadnet.createSpline(pSpline);
		pSpline->close();

		//for (size_t j = 0; j < polyPoints.size(); j++) {
		//	pPolyline->addVertexAt(j, AcGePoint2d(polyPoints[j].x, polyPoints[j].y));
		//}
		//pPolyline->setLineWeight(AcDb::kLnWt030);
		//pPolyline->setLayer(layerName);
		//roadnet.createPolyLine(pPolyline);
		//pPolyline->close();
	}
	res.mountainDis = mountainDis;
	res.roadDis = roadDis;
}


void RoadCal::SeparateData(){
	double left=DBL_MAX;
	double down=DBL_MAX;
	double right=0;
	double top=0;
	std::vector<CPoint3D*>* contourPs = new std::vector<CPoint3D*>();
	std::vector<CPoint3D*>* oldRoadPs = new std::vector<CPoint3D*>();  // 旧道路
	std::vector<CPoint3D*>* obstaclePs = new std::vector<CPoint3D*>();  // 障碍物
	std::vector<CPoint3D*> targetPs;

	// 计算选取等高线中的左上和右下坐标,并将所有的等高线点放入contourPs中
	for (size_t i=0;i<baseData->contours.size();i++){
		CContourModel* contourModel= baseData->contours[i];
		for (int j=0;j<contourModel->linePoint.size();j++){
			CPoint3D* point=contourModel->linePoint[j];
			contourPs->push_back(point);
			if(point->X<left){
				left=point->X;
			}
			if(point->X>right){
				right=point->X;
			}
			if(point->Y<down){
				down=point->Y;
			}
			if(point->Y>top){
				top=point->Y;
			}
		}
	}
	for (size_t i=0;i<baseData->oldRoads.size();i++){
		COldRoad* oldRoad= baseData->oldRoads[i];
		for (int j=0;j<oldRoad->linePoint.size();j++){
			if (j == 0) {
				CPoint3D* point=oldRoad->linePoint[j];
				oldRoadPs->push_back(point);
				if(point->X<left){
					left=point->X;
				}
				if(point->X>right){
					right=point->X;
				}
				if(point->Y<down){
					down=point->Y;
				}
				if(point->Y>top){
					top=point->Y;
				}
			}
			else {
				CPoint3D* pointPre = oldRoad->linePoint[j - 1];
				CPoint3D* point = oldRoad->linePoint[j];
				double dis = sqrt((double)(pow(pointPre->X - point->X, 2) + pow(pointPre->Y - point->Y, 2)));
				if (dis > departLength){
					int num = dis / departLength;
					for (int m = 0; m < num; m++){
						double posx = pointPre->X - (pointPre->X - point->X) * (m + 1) / (num + 1);
						double posy = pointPre->Y - (pointPre->Y - point->Y) * (m + 1) / (num + 1);
						CPoint3D* newPoint = new CPoint3D(posx, posy, 0);
						oldRoadPs->push_back(newPoint);
					}
				}
				oldRoadPs->push_back(point);
				if (point->X < left) {
					left = point->X;
				}
				if (point->X > right) {
					right = point->X;
				}
				if (point->Y < down) {
					down = point->Y;
				}
				if (point->Y > top) {
					top = point->Y;
				}
			}
		}
	}
	for (size_t i=0;i<baseData->obstacles.size();i++){
		CObstacle* obstacle= baseData->obstacles[i];
		for (int j=0;j<obstacle->pointList.size();j++){
			CPoint3D* point=obstacle->pointList[j];
			obstaclePs->push_back(point);
			if(point->X<left){
				left=point->X;
			}
			if(point->X>right){
				right=point->X;
			}
			if(point->Y<down){
				down=point->Y;
			}
			if(point->Y>top){
				top=point->Y;
			}
		}
	}
	
	AcGePoint2d gePoint(left,down);
	downLeftPoint.AcGeP2ToCP(gePoint);  // 获取左下的点

	row = std::ceil((top - down) / departLength);
	line = std::ceil((right-left) / departLength);
	
	try{
		contourPoints = new std::vector<std::vector<std::vector<CPoint3D*>>>(row, std::vector<std::vector<CPoint3D*>>(line, std::vector<CPoint3D*>(0)));
		oldRoadPoints = new std::vector<std::vector<std::vector<CPoint3D*>>>(row, std::vector<std::vector<CPoint3D*>>(line, std::vector<CPoint3D*>(0)));
		obstaclePoints = new std::vector<std::vector<std::vector<CPoint3D*>>>(row, std::vector<std::vector<CPoint3D*>>(line, std::vector<CPoint3D*>(0)));
	}
	catch (CMemoryException* e){
		return;
	}
	catch (CFileException* e){
		return;
	}
	catch (CException* e){
		resbuf;
	}
	
	size_t rowIndex = -1, lineIndex = -1, m = 0; 
	for (;m< contourPs->size();m++){
		CPoint3D* point = contourPs->at(m);
		rowIndex = std::floor((point->Y - downLeftPoint.Y) / departLength);
		lineIndex = std::floor((point->X - downLeftPoint.X) / departLength);
		if (rowIndex < row && lineIndex < line) {
			(*contourPoints)[rowIndex][lineIndex].push_back(point);
		}
	}
	for (m = 0;m< oldRoadPs->size();m++){
		CPoint3D* point= oldRoadPs->at(m);
		rowIndex = std::floor((point->Y - downLeftPoint.Y) / departLength);
		lineIndex = std::floor((point->X - downLeftPoint.X) / departLength);
		if (rowIndex < row && lineIndex < line) {
			(*oldRoadPoints)[rowIndex][lineIndex].push_back(point);
		}
	}
	for (m = 0;m< obstaclePs->size();m++){
		CPoint3D* point= obstaclePs->at(m);
		rowIndex = std::floor((point->Y - downLeftPoint.Y) / departLength);
		lineIndex = std::floor((point->X - downLeftPoint.X) / departLength);
		if (rowIndex < row && lineIndex < line) {
			(*obstaclePoints)[rowIndex][lineIndex].push_back(point);
		}
	}
}

//  设置风机场地号(设置坐标，字体大小及位置)
void RoadCal::addMLeader(AcDbDatabase* pDb, AcDbObjectId layerId, AcGePoint3d point) {
	int textSize = 8;
	AcDbBlockTable* pBlkTbl = NULL;
	acDocManager->lockDocument(acDocManager->curDocument());
	if (pDb->getBlockTable(pBlkTbl, AcDb::kForRead) == Acad::eOk){
		AcDbBlockTableRecord* pBlkTblRcd = NULL;
		if (pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForWrite) == Acad::eOk){
			AcGePoint3d lastPoint(point.x + 40.0, point.y, 0);
			AcDbLine* pLine = new AcDbLine(point, lastPoint);
			pLine->setLayer(layerId);
			pBlkTblRcd->appendAcDbEntity(pLine);

			AcString result;
			result.format(_T("%d"), points.size() - 1);
			const ACHAR* charValue = result.kACharPtr();
			AcDbMText* pMText = new AcDbMText();
			pMText->setTextHeight(textSize);
			pMText->setContents(charValue);
			pMText->setLayer(layerId);
			AcGePoint3d mTextPoint(lastPoint.x, lastPoint.y + textSize + 1, 0);
			pMText->setLocation(mTextPoint);
			pBlkTblRcd->appendAcDbEntity(pMText);
			pLine->close();
			pMText->close();
			pBlkTblRcd->close();
		}
		pBlkTbl->close();
	}
	acDocManager->unlockDocument(acDocManager->curDocument());
}