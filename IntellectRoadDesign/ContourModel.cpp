#include "StdAfx.h"
#include "ContourModel.h"

CContourModel::CContourModel(void)
{
	linePoint.resize(0);
	lineLength=0;
	altitude=0;
}


CContourModel::CContourModel(AcDbEntity* pEnt)
{
	linePoint.resize(0);
	lineLength=0;
	altitude=0;
	AcDbPolyline* pline=AcDbPolyline::cast(pEnt);  // 将实体转换为多线段类型
	unsigned int ptNum = pline->numVerts();  // 多段线中的顶点数
	altitude = pline->elevation();  // 获取多段线的高程信息
	AcGePoint3d point1;
	AcGePoint3d point2;
	pline->getPointAt(0,point1);  // 获取多段线中第一个顶点的坐标

	linePoint.push_back(new CPoint3D(point1.x,point1.y,altitude));
	for (int i=1;i<ptNum;i++)
	{
		pline->getPointAt(i-1,point1);
		pline->getPointAt(i,point2);
		lineLength+=point1.distanceTo(point2);
		linePoint.push_back(new CPoint3D(point2.x,point2.y,altitude));
	}

}


CContourModel::~CContourModel(void)
{
	//for (int i = 0; i < linePoint.size(); i++) {
	//	delete linePoint[i];
	//	linePoint[i] = NULL;
	//}
}


double CContourModel::GetAltitude(){
	return altitude;
}


double CContourModel::GetLineLength(){
	return lineLength;
}