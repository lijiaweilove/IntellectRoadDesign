#pragma once

class CPoint3D
{
public:
	CPoint3D(void);
	CPoint3D(double x,double y,double z);
	~CPoint3D(void);
	void InitPoint3D();
	void AcGeP3ToCP(AcGePoint3d point);
	void AcGeP2ToCP(AcGePoint2d point);
	AcGePoint3d CPToAcGeP3();
	AcGePoint2d CPToAcGeP2();
public:
	double X;
	double Y;
	double Z;
};
