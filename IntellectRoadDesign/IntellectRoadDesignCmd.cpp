#include "StdAfx.h"
#include "IntellectRoadDesignCmd.h"
#include "CAPoint.h"
#include "CAstar.h"
#include "Struct.h"
#include <algorithm>
#include <map>
#include <windows.h>
#include <set>

// 对所选区域上色
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
                    pPoint->setLayer(_T("新建路"));
                    pModelSpace->appendAcDbEntity(pPoint);
                    pPoint->close();
                }
            }
        }
    }
    pModelSpace->close();
    acDocManager->unlockDocument(acDocManager->curDocument());
}


// 执行A*算法
void getEdgeWeight(RoadCal* road, int start, int end, double maxSlope) {
    std::vector<std::vector<CAPoint*>>* pointMap = new std::vector<std::vector<CAPoint*>>(); //地图，存储每一个节点
    int row_num = road->row;	//行
    int col_num = road->line;	//列
    //地图初始化，对每个节点数据进行初始化
    for (int i = 0; i < row_num; i++) {
        std::vector<CAPoint*> tmp;
        for (int j = 0; j < col_num; j++) {
            CAPoint* point = new CAPoint();
            // 从CAD数据赋值每个节点的中心坐标
            point->m_x = road->downLeftPoint.X + j * road->departLength + road->departLength / 2.;
            point->m_y = road->downLeftPoint.Y + i * road->departLength + road->departLength / 2.;
            point->m_X = i;
            point->m_Y = j;
            //标记障碍物和不可通行区域
            if ((*road->obstaclePoints)[i][j].size() != 0) {
                point->m_type = ATYPE_BARRIER;
            }
            //标记道路
            if ((*road->oldRoadPoints)[i][j].size() != 0) {
                point->isRoad = true;
            }
            //根据等高线疏密程度可以判断缓陡坡，没有等高线的网格没有设置属性
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
                    point->m_type = ATYPE_BARRIER;	//节点中等高线过于密集，难以通行设为障碍
                }
            }           
            tmp.push_back(point);
        }
        pointMap->push_back(tmp);
    }
    
    //开始寻路
    CAstar* star = new CAstar(row_num, col_num, maxSlope);
    star->departLength = road->departLength;
    //获取起点
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
    // 检查起点的数组下标是否越界
    if (startYIndex >= pointMap->size() || startXIndex >= (*pointMap)[startYIndex].size()) {
        delete pointMap;
        return;
    }
    // 检查终点的数组下标是否越界
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

    //打印路线
    CAPoint* nowPoint = point;
    CAPoint* prePoint = point->m_parent;
    double rodDis = 0., tmpDis = 0., mouDis = 0.;
    double tmpmiddleDis = 0., middleDis = 0.;
    double endDis = 0., startDis = 0.;
    int endPointNum = 0;
    int tempEndPointNum = 0, tempStartPointNum = 0;
    bool firstNonRoad = true; // 是否是第一个非道路点
    
    //EnterCriticalSection(&RoadCal::csLock);
    RoadCal::edge.push_back(new Edge(start, end));  // start =u, end =v
    RoadCal::edge[RoadCal::edge.size() - 1]->result->push_back(AcGePoint3d(nowPoint->m_x, nowPoint->m_y, nowPoint->m_elevation));
    while (!(*prePoint == *(star->_startPoint))){  // 从v(end)->u(start)找
        tmpDis = sqrt(pow(nowPoint->m_x - prePoint->m_x, 2.) + pow(nowPoint->m_y - prePoint->m_y, 2.));
        if (!prePoint->isRoad) {
            mouDis += tmpDis;
            tempEndPointNum++;
            if (!firstNonRoad) {
                tempStartPointNum++;  // 先计算非路段,如果遇到路就清零,直到最后一个非路段,就是结果
                startDis += tmpDis;
                tmpmiddleDis += tmpDis;
            }
        }
        else {
            rodDis += tmpDis;
            if (!firstNonRoad) {  // 只会执行第一次,先计算从v(end)开始的尾部的点数和距离
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


// 留下两点之间mountainDis最大的边
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
    getEdgeWeight(roadCopy, i, j, maxSlope);  // 执行A*算法
    
    delete roadCopy;
    delete params;
    return 0;
}


Result IntellectRoadDesignCmd::GetRoadData(int departLength, double maxSlope){
	acutPrintf(_T("\n开始执行"));
    RoadCal* road = new RoadCal();
    road->departLength = departLength;
    // 获取风机平台点坐标
    road->readDwg(); 
    InitializeCriticalSection(&RoadCal::csLock);

    // 多线程循环
    vector<HANDLE> threads;
    // 获取离每个点最近的两个点
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
                ThreadParams* threadParams = new ThreadParams; // 每个线程需要有独立的参数
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

    // 等待所有线程完成并关闭线程句柄
    for (int i = 0; i < threads.size(); i++) {
        WaitForSingleObject(threads[i], INFINITE);
    }
    // 删除所有参数对象并关闭所有线程句柄
    for (int i = 0; i < threads.size(); i++) {
        CloseHandle(threads[i]);
    }
    DeleteCriticalSection(&RoadCal::csLock);
    RemoveDuplicateEdges();
    Result res;
    road->doRoadNetPlan(res);

    // 释放静态资源
    for (int i = 0; i < RoadCal::edge.size(); i++) {
		delete RoadCal::edge[i];
	}
    RoadCal::edge.clear();
    acutPrintf(_T("\n 执行结束"));
    delete road;
    return res;
}


////  两点之间，单线程
//void IntellectRoadDesignCmd::GetRoadData(int departLength) {
//    acutPrintf(_T("\n测试开始"));
//    RoadCal* road = new RoadCal();
//    road->departLength = departLength;
//    // 获取风机平台点坐标
//    //road->readDwg();
//    //  测试两点之间的框选范围,单线程
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
//    acutPrintf(_T("\n 测试完成"));
//    delete start;
//    delete end;
//    delete road;
//}
