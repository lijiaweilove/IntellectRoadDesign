#pragma once
#include "RoadNet.h"
#include "BaseData.h"
#include "ContourModel.h"
#include "Obstacle.h"
#include "OldRoad.h"
#include "Target.h"
#include "Common/Tools.h"
#include "Point3D.h"
#include "Struct.h"
#include <windows.h>
#include <vector>
using std::vector;

class RoadCal
{
public:
	RoadCal(void);
	RoadCal(RoadCal& other);
	~RoadCal(void);
	void readDwg();
	void SeparateData();
	void DomainSerach(const AcGePoint3d* point1, const AcGePoint3d* point2);
	void doRoadNetPlan(Result& res);
public:
	const TCHAR* layerName;
	vector<AcGePoint3d*> points;
	static vector<Edge*> edge;

	CBaseData* baseData;
	CPoint3D downLeftPoint;
	size_t targetNum, departLength, row, line;  // 点数, 分割深度, x轴上分的格子数 , y轴上分的格子数
	std::vector<std::vector<std::vector<CPoint3D*>>>* contourPoints;
	std::vector<std::vector<std::vector<CPoint3D*>>>* oldRoadPoints;
	std::vector<std::vector<std::vector<CPoint3D*>>>* obstaclePoints;
	std::vector<std::vector<std::vector<CPoint3D*>>>* targetPoints;
	static CRITICAL_SECTION csLock; // 临界区
};
