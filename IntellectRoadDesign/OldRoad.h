#pragma once
#include "Point3D.h"
class COldRoad
{
public:
	COldRoad(void);
	COldRoad(AcDbEntity* pEnt);
	~COldRoad(void);
public:
	std::vector<CPoint3D*> linePoint;
public:
	double lineLength;
};
