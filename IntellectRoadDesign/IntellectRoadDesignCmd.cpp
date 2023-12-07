#include "StdAfx.h"
#include "IntellectRoadDesignCmd.h"
#include "CAPoint.h"
#include "CAstar.h"
#include "Struct.h"
#include <algorithm>
#include <map>
#include <windows.h>
#include <set>

// ����ѡ������ɫ
void ColorContourPoints(std::vector<std::vector<std::vector<CPoint3D*>>>& contourPoints) {
    AcDbDatabase* pDatabase = acdbHostApplicationServices()->workingDatabase();
    AcDbBlockTable* pBlockTable = NULL;
    pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);

    AcDbBlockTableRecord* pModelSpace = NULL;
    acDocManager->lockDocument(acDocManager->curDocument());
    pBlockTable->getAt(ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite);
    pBlockTable->close();
    for (std::vector<std::vector<std::vector<CPoint3D*>>>::iterator outer = contourPoints.begin(); outer != contourPoints.end(); ++outer) {
        for (std::vector<std::vector<CPoint3D*>>::iterator inner = outer->begin(); inner != outer->end(); ++inner) {
            for (std::vector<CPoint3D*>::iterator point = inner->begin(); point != inner->end(); ++point) {
                if (*point) {
                    AcDbPoint* pPoint = new AcDbPoint(AcGePoint3d((*point)->X, (*point)->Y, (*point)->Z));
                    pPoint->setLayer(_T("�½�·"));
                    pModelSpace->appendAcDbEntity(pPoint);
                    pPoint->close();
                }
            }
        }
    }
    pModelSpace->close();
    acDocManager->unlockDocument(acDocManager->curDocument());
}


// ִ��A*�㷨
void getEdgeWeight(RoadCal* road, int start, int end, double maxSlope) {
    std::vector<std::vector<CAPoint*>>* pointMap = new std::vector<std::vector<CAPoint*>>(); //��ͼ���洢ÿһ���ڵ�
    int row_num = road->row;	//��
    int col_num = road->line;	//��
    //��ͼ��ʼ������ÿ���ڵ����ݽ��г�ʼ��
    for (int i = 0; i < row_num; i++) {
        std::vector<CAPoint*> tmp;
        for (int j = 0; j < col_num; j++) {
            CAPoint* point = new CAPoint();
            // ��CAD���ݸ�ֵÿ���ڵ����������
            point->m_x = road->downLeftPoint.X + j * road->departLength + road->departLength / 2.;
            point->m_y = road->downLeftPoint.Y + i * road->departLength + road->departLength / 2.;
            point->m_X = i;
            point->m_Y = j;
            //����ϰ���Ͳ���ͨ������
            if ((*road->obstaclePoints)[i][j].size() != 0) {
                point->m_type = ATYPE_BARRIER;
            }
            //��ǵ�·
            if ((*road->oldRoadPoints)[i][j].size() != 0) {
                point->isRoad = true;
            }
            //���ݵȸ������̶ܳȿ����жϻ����£�û�еȸ��ߵ�����û����������
            if ((*road->contourPoints)[i][j].size() != 0) {
                std::set<double> tmp;
                for (int m = 0; m < (*road->contourPoints)[i][j].size(); m++) {
                    double z = (*road->contourPoints)[i][j][m]->Z;
                    tmp.insert(z);
                }
                point->m_slopeLevel = tmp.size();
                if (tmp.size() == 1) {
                    point->m_elevation = *tmp.begin();
                }
                else if (tmp.size() == 2) {
                    double total_num = 0.;
                    for (std::set<double>::iterator it = tmp.begin(); it != tmp.end(); it++) {
                        total_num += *it;
                    }
                    point->m_elevation = total_num / 2;
                }
                else if (tmp.size() > 2) {
                    point->m_type = ATYPE_BARRIER;	//�ڵ��еȸ��߹����ܼ�������ͨ����Ϊ�ϰ�
                }
            }           
            tmp.push_back(point);
        }
        pointMap->push_back(tmp);
    }
    
    //��ʼѰ·
    CAstar* star = new CAstar(row_num, col_num, maxSlope);
    star->departLength = road->departLength;
    //��ȡ���
    double startPointX = road->points[start]->x;
    double startPointY = road->points[start]->y;
    double endPointX = road->points[end]->x;
    double endPointY = road->points[end]->y;
    double startElevation = road->points[start]->z;
    double endElevation = road->points[end]->z;
    
  //  star->_startPoint = (*pointMap)[abs(road->downLeftPoint.Y - startPointY) / road->departLength]
		//[abs(road->downLeftPoint.X - startPointX) / road->departLength];
  //  star->_endPoint = (*pointMap)[abs(road->downLeftPoint.Y - endPointY) / road->departLength]
  //      [abs(road->downLeftPoint.X - endPointX) / road->departLength];

    int startYIndex = abs(road->downLeftPoint.Y - startPointY) / road->departLength;
    int startXIndex = abs(road->downLeftPoint.X - startPointX) / road->departLength;
    int endYIndex = abs(road->downLeftPoint.Y - endPointY) / road->departLength;
    int endXIndex = abs(road->downLeftPoint.X - endPointX) / road->departLength;
    // ������������±��Ƿ�Խ��
    if (startYIndex >= pointMap->size() || startXIndex >= (*pointMap)[startYIndex].size()) {
        delete pointMap;
        return;
    }
    // ����յ�������±��Ƿ�Խ��
    if (endYIndex >= pointMap->size() || endXIndex >= (*pointMap)[endYIndex].size()) {
        delete pointMap;
        return;
    }
    star->_startPoint = (*pointMap)[startYIndex][startXIndex];
    star->_endPoint = (*pointMap)[endYIndex][endXIndex];
    star->_startPoint->m_elevation = startElevation;
    star->_endPoint->m_elevation = endElevation;
    CAPoint* point = star->findWay(pointMap);
    if (point == NULL) {
        delete star;
        delete pointMap;
        return ;
    }

    //��ӡ·��
    CAPoint* nowPoint = point;
    CAPoint* prePoint = point->m_parent;
    double rodDis = 0., tmpDis = 0., mouDis = 0.;
    double tmpmiddleDis = 0., middleDis = 0.;
    double endDis = 0., startDis = 0.;
    int endPointNum = 0;
    int tempEndPointNum = 0, tempStartPointNum = 0;
    bool firstNonRoad = true; // �Ƿ��ǵ�һ���ǵ�·��
    
    //EnterCriticalSection(&RoadCal::csLock);
    RoadCal::edge.push_back(new Edge(start, end));  // start =u, end =v
    RoadCal::edge[RoadCal::edge.size() - 1]->result->push_back(AcGePoint3d(nowPoint->m_x, nowPoint->m_y, nowPoint->m_elevation));
    while (!(*prePoint == *(star->_startPoint))){  // ��v(end)->u(start)��
        tmpDis = sqrt(pow(nowPoint->m_x - prePoint->m_x, 2.) + pow(nowPoint->m_y - prePoint->m_y, 2.));
        if (!prePoint->isRoad) {
            mouDis += tmpDis;
            tempEndPointNum++;
            if (!firstNonRoad) {
                tempStartPointNum++;  // �ȼ����·��,�������·������,ֱ�����һ����·��,���ǽ��
                startDis += tmpDis;
                tmpmiddleDis += tmpDis;
            }
        }
        else {
            rodDis += tmpDis;
            if (!firstNonRoad) {  // ֻ��ִ�е�һ��,�ȼ����v(end)��ʼ��β���ĵ����;���
                endDis = mouDis;
                endPointNum = tempEndPointNum;
            }
            firstNonRoad = false; 
            if (tmpmiddleDis > 0.) {
			    middleDis += tmpmiddleDis;
			    tmpmiddleDis = 0.;
                startDis = 0.;
                tempStartPointNum = 0;
            }
        }
        AcGePoint3d startPoint(nowPoint->m_x, nowPoint->m_y, nowPoint->m_elevation);
        AcGePoint3d endPoint(prePoint->m_x, prePoint->m_y, prePoint->m_elevation);
        RoadCal::edge[RoadCal::edge.size() - 1]->result->push_back(endPoint);
        nowPoint = prePoint;
        prePoint = prePoint->m_parent;
    }
    RoadCal::edge[RoadCal::edge.size() - 1]->roadDis += rodDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->mountainDis += mouDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->result->push_back(AcGePoint3d(prePoint->m_x, prePoint->m_y, prePoint->m_elevation));
    RoadCal::edge[RoadCal::edge.size() - 1]->weight = mouDis + rodDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->midWeight = middleDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->endDis = endDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->startDis = startDis;
    RoadCal::edge[RoadCal::edge.size() - 1]->endPointNum = endPointNum;
    RoadCal::edge[RoadCal::edge.size() - 1]->startPointNum = tempStartPointNum;
    //LeaveCriticalSection(&RoadCal::csLock);
    delete star;
    delete pointMap;
    return ;
}


// ��������֮��mountainDis���ı�
void RemoveDuplicateEdges() {
    int n = RoadCal::edge.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (RoadCal::edge[i]->u == RoadCal::edge[j]->v && RoadCal::edge[i]->v == RoadCal::edge[j]->u) {
                if (RoadCal::edge[i]->mountainDis < RoadCal::edge[j]->mountainDis) {
                    swap(RoadCal::edge[i], RoadCal::edge[j]);
                }
                delete RoadCal::edge[j];
                RoadCal::edge.erase(RoadCal::edge.begin() + j);
                n--;
                j--;
            }
        }
    }
}


IntellectRoadDesignCmd::IntellectRoadDesignCmd(void) : numThreads(10)
{
}


IntellectRoadDesignCmd::~IntellectRoadDesignCmd(void)
{

}


IntellectRoadDesignCmd* IntellectRoadDesignCmd::getInstace() {	
	static IntellectRoadDesignCmd instance;
	return &instance;
}


DWORD WINAPI ThreadFunction(LPVOID param)
{
    ThreadParams* params = (ThreadParams*)param;
    RoadCal* roadCopy = params->roadCopy;
    int i = params->i;
    int j = params->j;
    double maxSlope = params->maxSlope;
    roadCopy->SeparateData();
    getEdgeWeight(roadCopy, i, j, maxSlope);  // ִ��A*�㷨
    
    delete roadCopy;
    delete params;
    return 0;
}


Result IntellectRoadDesignCmd::GetRoadData(int departLength, double maxSlope){
	acutPrintf(_T("\n��ʼִ��"));
    RoadCal* road = new RoadCal();
    road->departLength = departLength;
    // ��ȡ���ƽ̨������
    road->readDwg(); 
    InitializeCriticalSection(&RoadCal::csLock);

    // ���߳�ѭ��
    vector<HANDLE> threads;
    // ��ȡ��ÿ���������������
    for (int i = 0; i < road->targetNum; i++) {
        std::map<double, int> temp;
        for (int j = 0; j < road->targetNum; j++) {
            if (i != j) {
                temp[road->points[i]->distanceTo(*road->points[j])] = j;
            }
        }
        std::map<double, int>::iterator it = temp.begin();
        int threadCount = 0;
        for (int j = 0; j < 2 && it != temp.end() ; j++) {
            if (threadCount < numThreads) {
                ThreadParams* threadParams = new ThreadParams; // ÿ���߳���Ҫ�ж����Ĳ���
                threadParams->roadCopy = new RoadCal(*road);
                threadParams->roadCopy->DomainSerach(road->points[i], road->points[it->second]);
                threadParams->i = i;
                threadParams->j = it->second;
                threadParams->maxSlope = maxSlope;
                HANDLE hThread = CreateThread(NULL, 0, ThreadFunction, threadParams, 0, NULL);
                if (hThread) {
                    threads.push_back(hThread);
                    threadCount++;
                }
            }
            else {
                WaitForSingleObject(threads[0], INFINITE);
                CloseHandle(threads[0]);
                threads.erase(threads.begin());
                threadCount--;
            }
            it++;
        }
    }

    // �ȴ������߳���ɲ��ر��߳̾��
    for (int i = 0; i < threads.size(); i++) {
        WaitForSingleObject(threads[i], INFINITE);
    }
    // ɾ�����в������󲢹ر������߳̾��
    for (int i = 0; i < threads.size(); i++) {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&RoadCal::csLock);
    RemoveDuplicateEdges();
    Result res;
    road->doRoadNetPlan(res);

    // �ͷž�̬��Դ
    for (int i = 0; i < RoadCal::edge.size(); i++) {
		delete RoadCal::edge[i];
	}
    RoadCal::edge.clear();
    acutPrintf(_T("\n ִ�н���"));
    delete road;
    return res;
}


////  ����֮�䣬���߳�
//void IntellectRoadDesignCmd::GetRoadData(int departLength) {
//    acutPrintf(_T("\n���Կ�ʼ"));
//    RoadCal* road = new RoadCal();
//    road->departLength = departLength;
//    // ��ȡ���ƽ̨������
//    //road->readDwg();
//    //  ��������֮��Ŀ�ѡ��Χ,���߳�
//    AcGePoint3d* start = new AcGePoint3d(622026.116,4129788.1501, 1498.0000);
//    AcGePoint3d* end = new AcGePoint3d(622323.2018, 4129546.3174, 1446.0000);
//    //AcGePoint3d* start = new AcGePoint3d(621383.6821, 4130125.2931, 1476.0000);
//    //AcGePoint3d* end = new AcGePoint3d(622026.116, 4129788.1501, 1498.0000);
//    road->points.push_back(new AcGePoint3d(*start));
//    road->points.push_back(new AcGePoint3d(*end));
//    road->baseData = new CBaseData();
//    road->DomainSerach(start, end);
//    road->SeparateData();
//    getEdgeWeight(road, 0, 1);
//    
//    RoadNet roadnet;
//    AcDbPolyline* pPolyline = new AcDbPolyline();
//    std::vector<AcGePoint2d>& polyPoints = *(RoadCal::edge[RoadCal::edge.size() - 1]->result);
//    for (size_t j = 0; j < polyPoints.size(); j++) {
//        pPolyline->addVertexAt(j, AcGePoint2d(polyPoints[j].x, polyPoints[j].y));
//    }
//    pPolyline->setLineWeight(AcDb::kLnWt030);
//    pPolyline->setLayer(road->layerName);
//    roadnet.createPolyLine(pPolyline);
//    pPolyline->close();
//    acutPrintf(_T("\n �������"));
//    delete start;
//    delete end;
//    delete road;
//}
