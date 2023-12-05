#pragma once
#include "Point3D.h"
class CObstacle
{
public:
	CObstacle(void);
	CObstacle(AcDbEntity* pEnt);
	~CObstacle(void);
public:
	AcDbEntity* pEnt;
	CPoint3D midPoint;
	std::vector<CPoint3D*> pointList;
};
