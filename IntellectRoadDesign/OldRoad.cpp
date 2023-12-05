#include "StdAfx.h"
#include "OldRoad.h"

COldRoad::COldRoad(void){}


COldRoad::COldRoad(AcDbEntity* pEnt){
	linePoint.resize(0);
	lineLength=0;
	AcDbPolyline* pline=AcDbPolyline::cast(pEnt);
	unsigned int ptNum = pline->numVerts();

	AcGePoint3d point1;
	AcGePoint3d point2;
	pline->getPointAt(0,point1);

	linePoint.push_back(new CPoint3D(point1.x,point1.y,0));
	for (int i=1;i<ptNum;i++)
	{
		pline->getPointAt(i-1,point1);
		pline->getPointAt(i,point2);
		lineLength+=point1.distanceTo(point2);
		linePoint.push_back(new CPoint3D(point2.x,point2.y,0));
	}
}


COldRoad::~COldRoad(void)
{
	/*for (int i = 0; i < linePoint.size(); i++){
		delete linePoint[i];
		linePoint[i]=NULL;
	}*/
}

