#pragma once
#include "ContourModel.h"
#include "Obstacle.h"
#include "OldRoad.h"
#include "Target.h"

class CBaseData
{
public:
	CBaseData(void);
	~CBaseData(void);
	void InitData();
	void DataInfo();
public:
	std::vector<CContourModel*> contours;
	std::vector<COldRoad *> oldRoads;
	std::vector<CTarget *> targets;
	std::vector<CObstacle*> obstacles;
};
