#pragma once
#include "Point3D.h"
class CTarget
{
public:
	CTarget(void);
	CTarget(AcDbEntity* pEnt1);
	~CTarget(void);
	CPoint3D GetCenter();
	double GetRadius();
	double GetAltitude();
	double GetArea();
public:
	AcDbPolyline* site;//面积对象
	AcDbCircle* fan;//风机
	std::vector<CPoint3D*> linePoint;
public:
	CPoint3D center;//风机中心
	double radius;//风机占地半径
	double altitude;//场地海拔
	double area;//场地面积
};
