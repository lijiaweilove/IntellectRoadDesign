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
	AcDbPolyline* site;//�������
	AcDbCircle* fan;//���
	std::vector<CPoint3D*> linePoint;
public:
	CPoint3D center;//�������
	double radius;//���ռ�ذ뾶
	double altitude;//���غ���
	double area;//�������
};
