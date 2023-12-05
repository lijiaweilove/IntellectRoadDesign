#include "StdAfx.h"
#include "Point3D.h"

CPoint3D::CPoint3D(void)
{
	X=0;
	Y=0;
	Z=0;
}
CPoint3D::CPoint3D(double x,double y,double z){
	X=x;
	Y=y;
	Z=z;
}

CPoint3D::~CPoint3D(void)
{
}
void CPoint3D::InitPoint3D(){
	X=0;
	Y=0;
	Z=0;
}
void CPoint3D::AcGeP3ToCP(AcGePoint3d point){
	X=point.x;
	Y=point.y;
	Z=point.z;
}
void CPoint3D::AcGeP2ToCP(AcGePoint2d point){
	X=point.x;
	Y=point.y;
	Z=0;
}
AcGePoint3d CPoint3D::CPToAcGeP3(){
	AcGePoint3d last;
	last.x=X;
	last.y=Y;
	last.z=Z;
	return last;
}
AcGePoint2d CPoint3D::CPToAcGeP2(){
	AcGePoint2d last;
	last.x=X;
	last.y=Y;
	return last;
}
