#include "StdAfx.h"
#include "Obstacle.h"

CObstacle::CObstacle(void){}


CObstacle::CObstacle(AcDbEntity* pEnt1)
{
	pEnt=pEnt1;
	if(pEnt->isA() == AcDbPolyline::desc()) {
		AcDbPolyline* pLine=AcDbPolyline::cast(pEnt1);
		unsigned int ptNum = pLine->numVerts();
		AcGePoint3d point1;
		double x=0;
		double y=0;
		for (int i=0;i<ptNum;i++)
		{
			pLine->getPointAt(i,point1);
			x+=point1.x;
			y+=point1.y;
			pointList.push_back(new CPoint3D(point1.x,point1.y,0));
		}
		x/=ptNum;
		y/=ptNum;
		point1.x=x;
		point1.y=y;
		point1.z=0;
		midPoint.AcGeP3ToCP(point1);
	}
	else if(pEnt->isA() == AcDbPoint::desc()) {
		AcDbPoint* pPoint=AcDbPoint::cast(pEnt1);
		AcGePoint3d position= pPoint->position();
		midPoint.AcGeP3ToCP(position);
		pointList.push_back(new CPoint3D(position.x,position.y,0));
	}
	else if(pEnt->isA() == AcDbBlockReference::desc()) {
		AcDbBlockReference* pBlock=AcDbBlockReference::cast(pEnt1);
		AcGePoint3d position= pBlock->position();
		midPoint.AcGeP3ToCP(position);
		pointList.push_back(new CPoint3D(position.x,position.y,0));
	}
}


CObstacle::~CObstacle(void)
{
	if (pEnt){
		pEnt->close();
	}
	//for (int i = 0; i < pointList.size(); i++) {
		//delete pointList[i];
	//	pointList[i] = NULL;
	//}
	
}
