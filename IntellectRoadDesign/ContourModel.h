#pragma once
#include "Point3D.h"

class CContourModel
{
public:
	CContourModel(void);
	CContourModel(AcDbEntity* pEnt);
	~CContourModel(void);
	double GetAltitude();
	double GetLineLength();	
public:
	std::vector<CPoint3D*> linePoint;
public:
	double lineLength;
	double altitude;
};
