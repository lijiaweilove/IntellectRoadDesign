#include "StdAfx.h"
#include "ArxGeoMathHelper.h"
#include "ArxHelper.h"
namespace CadCommon
{

	double ArxGeoMathHelper::RadianToAngle(double radian)
	{
		return  180 * radian /  BIM_M_PI  ;                                     
	}

	Cable_Point  ArxGeoMathHelper::CalcVectorBy2Point(Cable_Point const& p1 ,Cable_Point const& p2)
	{
		double firstX = p1.x;
		double firstY = p1.y;
		double firstZ = p1.z;
		double secondX = p2.x;
		double secondY = p2.y;
		double secondZ = p2.z;		
		Cable_Point first_second_vector(firstX-secondX,firstY-secondY,firstZ-secondZ);
		first_second_vector.Normalization();
		return first_second_vector;
	}

	Cable_Point ArxGeoMathHelper::CalcVectorBy2Point(AcGePoint3d p1,AcGePoint3d p2)
	{
		Cable_Point point1(p1);
		Cable_Point point2(p2);
		return CalcVectorBy2Point(point1,point2);
	}


	AcGePoint3d ArxGeoMathHelper::CalcPointBySVL(Cable_Point const& p1,Cable_Point const& vec,double length)
	{
		double point_x = length * vec.x + p1.x;
		double point_y = length * vec.y + p1.y;
		double point_z = length * vec.z + p1.z;
		AcGePoint3d point2(point_x,point_y,point_z);
		return point2;
	}

	AcGePoint3d ArxGeoMathHelper::CalcPointBySEL(Cable_Point const& p1,Cable_Point const& p2,double length)
	{
		Cable_Point vec = CalcVectorBy2Point(p2,p1);
		return CalcPointBySVL(p1,vec,length);
	}

	AcGePoint3d ArxGeoMathHelper::CalcPointBySEL(AcGePoint3d const& p1,AcGePoint3d const& p2,double length)
	{
		Cable_Point point1(p1);
		Cable_Point point2(p2);
		return CalcPointBySEL(point1,point2,length);
	}


	std::vector<AcGePoint3d> ArxGeoMathHelper::GetPolyLinePointList(AcDbObjectId polyLineId)
	{
		std::vector<AcGePoint3d> points;
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, polyLineId, AcDb::kForRead))
		{
			if(pEnt->isA() == AcDbPolyline::desc()){
				 AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
				unsigned int vertsNum = pPolyline->numVerts();
				points.reserve(vertsNum);
				for(unsigned int index = 0;index < vertsNum ;++index)
				{
					AcGePoint2d point2d;
					pPolyline->getPointAt(index,point2d);
					AcGePoint3d point3d(point2d.x,point2d.y,0);
					//判断如果误差在3M内，则为同一个点
					if(points.size() >= 1){
						AcGePoint3d lastPoint = points[points.size() - 1];
						double vvv = lastPoint.distanceTo(point3d);
						if(vvv > 3.0000000000000001){
							points.push_back(point3d);
						}		
					}
					else{
							points.push_back(point3d);
					}
				}	
			}
			else if(pEnt->isA() == AcDbLine::desc()){
				AcDbLine* pPolyline = AcDbLine::cast(pEnt);
				AcGePoint3d startPoint;
				pPolyline->getStartPoint(startPoint);
				AcGePoint3d _point3dStart(startPoint.x,startPoint.y,0);
				points.push_back(_point3dStart);

				AcGePoint3d endPoint;
				pPolyline->getEndPoint(endPoint);
				AcGePoint3d _point3dEnd(endPoint.x,endPoint.y,0);
				points.push_back(_point3dEnd);
			}
			pEnt->close();
		}
		return points;
	}

	double ArxGeoMathHelper::CalcPolyLineLength(AcDbObjectId entId)
	{
		double length = 0.0;
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, entId, AcDb::kForRead))
		{
			ASSERT(pEnt != NULL);
			if(pEnt->isA() == AcDbPolyline::desc())
			{
				AcDbPolyline* pPolyLine = AcDbPolyline::cast(pEnt);
				unsigned int   nPoint = pPolyLine->numVerts();
				AcGePoint3d endPoint;
				pPolyLine->getEndPoint(endPoint);
				pPolyLine->getDistAtPoint(endPoint,length);
			}
			pEnt->close();
		}
		return length;
	}

	double ArxGeoMathHelper::CalcLengthByTwoPoint(const AcGePoint3d& p1,const AcGePoint3d& p2)
	{
		return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
	}

	bool ArxGeoMathHelper::CalcIntersection(AcDbEntity* pEntity1,AcDbEntity* pEntity2,AcGePoint3d& pointInts)
	{
		ASSERT(pEntity1 != NULL);
		ASSERT(pEntity2 != NULL);

		AcDbCurve* pCurve1 = AcDbCurve::cast(pEntity1);
		AcDbCurve* pCurve2 = AcDbCurve::cast(pEntity2);

		if(pCurve2 == NULL || pCurve1 == NULL){
			return false;
		}
		bool bRet = true;
		AcGePoint3d c1StartPoint,c2StartPoint,c1EndPoint,c2EndPoint;
		pCurve1->getStartPoint(c1StartPoint);
		pCurve1->getEndPoint(c1EndPoint);
		pCurve2->getStartPoint(c2StartPoint);
		pCurve2->getEndPoint(c2EndPoint);
		AcDbExtents _l1extents;
		pCurve1->getGeomExtents(_l1extents);
		AcDbExtents _l2extents;
		pCurve1->getGeomExtents(_l2extents);
		if(_l1extents.maxPoint().x < _l2extents.minPoint().x || 
			_l2extents.maxPoint().x < _l1extents.minPoint().x ||
			_l1extents.maxPoint().y < _l2extents.minPoint().y || 
			_l2extents.maxPoint().y < _l1extents.minPoint().y)   return false;

		AcGePoint3d c1NormalVec; 
		c1NormalVec.x = c1EndPoint.y - c1StartPoint.y;
		c1NormalVec.y = c1StartPoint.x - c1EndPoint.x;
		double dist_c1s_c1n = c1StartPoint.x * c1NormalVec.x + c1StartPoint.y * c1NormalVec.y;
		double dist_c2s_c1n = c2StartPoint.x * c1NormalVec.x + c2StartPoint.y * c1NormalVec.y;
		double dist_c2e_c1n = c2EndPoint.x * c1NormalVec.x + c2EndPoint.y * c1NormalVec.y;
		double vaa = (dist_c2s_c1n - dist_c1s_c1n) * (dist_c2e_c1n - dist_c1s_c1n) ;
		if(vaa  > EPSINON) bRet = false;

		AcGePoint3d c2NormalVec; 
		c2NormalVec.x = c2EndPoint.y - c2StartPoint.y;
		c2NormalVec.y = c2StartPoint.x - c2EndPoint.x;
		double dist_c2s_c2n = c2StartPoint.x * c2NormalVec.x + c2StartPoint.y * c2NormalVec.y;
		double dist_c1s_c2n = c1StartPoint.x * c2NormalVec.x + c1StartPoint.y * c2NormalVec.y;
		double dist_c1e_c2n = c1EndPoint.x * c2NormalVec.x + c1EndPoint.y * c2NormalVec.y;
		if(!bRet){
			double vbb = (dist_c1s_c2n - dist_c2s_c2n) * (dist_c1e_c2n - dist_c2s_c2n);
			if( vbb > EPSINON) return false;
		}

		double denominator = c2NormalVec.x * c1NormalVec.y - c2NormalVec.y * c1NormalVec.x;
		double fraction = (dist_c2s_c1n - dist_c1s_c1n) / denominator;
		pointInts.x = c2StartPoint.x + fraction * c2NormalVec.y;
		pointInts.y = c2StartPoint.y - fraction * c2NormalVec.x;
		pointInts.z = 0.0;
		return true;
	}

	bool ArxGeoMathHelper::PointInLine(AcGePoint3d const& pStart,AcGePoint3d const& pEnd,AcGePoint3d const& pPoint)
	{
		if((pPoint.x-pStart.x)*(pEnd.y-pStart.y)==(pEnd.x-pStart.x)*(pPoint.y-pStart.y)
			&&min(pStart.x,pEnd.x)<=pPoint.x&&pPoint.x<=max(pStart.x,pEnd.x)
			&&min(pStart.y,pEnd.y)<=pPoint.y&&pPoint.y<=max(pStart.y,pEnd.y)){
			return true;
		}else{
			return false;
		}
	}

	double ArxGeoMathHelper::AngleVecToVec(AcGePoint3d left, AcGePoint3d center, AcGePoint3d right, AngvalueType agt  )
	{
		AcGePoint3d a;
		a.x = (center.x - left.x);
		a.y = (center.y - left.y);
		a.z = 0;
		AcGeVector3d va = a.asVector();
		AcGePoint3d b;
		b.x = (center.x - right.x);
		b.y = (center.y - right.y);
		b.z = 0;
		AcGeVector3d vb = b.asVector();
		va = va.normalize();
		vb = vb.normalize();
		double value = va.angleTo(vb);
		if(agt == ANGLE)
		{
			value = RadianToAngle(value);
		}
		return value;

	}


	AcGePoint3d ArxGeoMathHelper::Angle_bisector(AcGePoint3d left, AcGePoint3d center, AcGePoint3d right)
	{
		AcGePoint3d a;
		a.x = (center.x - left.x);
		a.y = (center.y - left.y);
		a.z = 0;
		AcGeVector3d va = a.asVector();
		AcGePoint3d b;
		b.x = (center.x - right.x);
		b.y = (center.y - right.y);
		b.z = 0;
		AcGeVector3d vb = b.asVector();
 		va = va.normalize();
		vb = vb.normalize();

// 		bReserve = false;
//  		AcGeVector3d vCross = va.crossProduct(vb);
//  		if(vCross.z < 0 ){
//  			bReserve = true;
//  		}

		AcGePoint3d abs;
		if(va.isParallelTo(vb)){
			AcGeVector3d vp = va.perpVector();
			//bReserve = false;
			abs.set(vp.x,vp.y,0.0);
		}
		else{
			AcGeVector3d vp  = va + vb;
			abs.set(vp.x,vp.y,0.0);
		}
		
 		//如果均为0，则计算垂线
//  		if( (fabs(abs.x - 0) < 0.000000001) && (fabs(abs.y  - 0) < 0.000000001 )){
//  			AcGePoint3d absTemp = GetUnitVerticalVector(left,right,0);
//  			abs.x = absTemp.x;
//  			abs.y = absTemp.y;
//  		}
		return abs;
	}


	double ArxGeoMathHelper::AngleVectorXaxis(AcGePoint3d left, AcGePoint3d center)
	{
		AcGePoint3d a;
		a.x = (left.x - center.x);
		a.y = (left.y - center.y);
		a.z = 0.0;
		return AngleVectorXaxis(a);
	}

	double ArxGeoMathHelper::AngleVectorXaxis(AcGePoint3d vector)
	{
		//double value = atan2(vector.y,vector.x);
		AcGePoint3d a;
		a.x = vector.x;
		a.y = vector.y;
		a.z = vector.z;
		AcGeVector3d va = a.asVector();
		AcGeVector3d vx = AcGeVector3d::kXAxis;
		double value = va.angleTo(vx);
		return value;
	}

	double ArxGeoMathHelper::AngleVectorXaxis(AcGeVector3d vector)
	{
		//double value = atan2(vector.y,vector.x);
		AcGePoint3d a;
		a.x = vector.x;
		a.y = vector.y;
		a.z = vector.z;
		AcGeVector3d va = a.asVector();
		AcGeVector3d vx = AcGeVector3d::kXAxis;
		double value = va.angleTo(vx);
		return value;
	}


	double ArxGeoMathHelper::AngleVectorYaxis(AcGePoint3d left, AcGePoint3d center)
	{
		AcGePoint3d a;
		a.x = (left.x - center.x);
		a.y = (left.y - center.y);
		a.z = 0.0;
		return AngleVectorYaxis(a);
	}

	double ArxGeoMathHelper::AngleVectorYaxis(AcGePoint3d vector)
	{
		//double value = atan2(vector.x,vector.y);
		AcGePoint3d a;
		a.x = vector.x;
		a.y = vector.y;
		a.z = vector.z;
		AcGeVector3d va = a.asVector();
		AcGeVector3d vy = AcGeVector3d::kYAxis;
		double value = va.angleTo(vy);
		return value;
	}

	//根据x求多段线上点
	AcGePoint3d ArxGeoMathHelper::GetMultiPolyLineByX(AcGePoint3dArray ptArr, double pointX)
	{
		AcDbLine* pLine;
		AcGePoint3dArray inSectionPoint;
		AcGePoint3d tmpPoint1, tmpPoint2;

		try
		{
			AcDb2dPolyline* pDistanceLine = new AcDb2dPolyline(AcDb::k2dSimplePoly, ptArr);
			tmpPoint1.set(pointX, 0.0, 0.0);
			tmpPoint2.set(pointX, 9999999.0, 0.0);
			pLine = new AcDbLine(tmpPoint1, tmpPoint2);//垂线
			inSectionPoint.removeAll();
			pLine->intersectWith(pDistanceLine, AcDb::kExtendThis, inSectionPoint);//垂线与距地线求交
			SAFE_DELETE(pLine);
			if (inSectionPoint.length() > 0)
			{
				tmpPoint1 = inSectionPoint.at(0);
			}

			SAFE_DELETE(pDistanceLine);
		}
		catch (CMemoryException* e)
		{

		}
		catch (CFileException* e)
		{
		}
		catch (CException* e)
		{
		}
		return tmpPoint1;
	}

	AcGePoint3d   ArxGeoMathHelper::GetUnitVerticalVector (AcGePoint3d pointO,AcGePoint3d pointA,int type )
	{

		AcGePoint3d a;
		a.x = (pointO.x - pointA.x);
		a.y = (pointO.y - pointA.y);
		a.z = 0;
		AcGeVector3d va = a.asVector();
		AcGeVector3d vd =  va.perpVector();
		AcGePoint3d p(vd.x,vd.y,vd.z);
		return p;

// 		AcGePoint3d point;
// 		AcGePoint3d p;
// 	 
// 		point.x = pointA.x - pointO.x;
// 		point.y = pointA.y - pointO.y;
// 	 
// 		if ((0 == point.x) && ( 0 == point.y))
// 		{
// 			p.x = 0;
// 			p.y = 0;	 
// 			return p;
// 		}
// 	 
// 		if ( 0 == point.x)
// 		{
// 			if (0 == type)//垂直向量的y分量小于于0，向左上
// 			{
// 				p.x = -1;
// 			}
// 			else
// 			{
// 				p.x = 1;
// 			}
// 			p.y = 0;
// 	 
// 			return p;
// 		}
// 	 
// 		if ( 0 == point.y)//垂直向量的y分量小于于0，向左上
// 		{
// 			p.x = 0;
// 			p.y = -1;
// 			return p;
// 		}
// 
// 		p.y = - fabsf((point.x * 1.0f)/sqrtf((powf(point.x, 2)) + powf(point.y, 2)));
// 		p.x = - ((point.y * 1.0f)/point.x)*p.y;
// 	 
// 		if (0 == type)//垂直向量的y分量小于0，向左上
// 		{
// 			if (0 < p.y)
// 			{
// 				p.x = - p.x;
// 				p.y = - p.y;
// 			}
// 		}
// 		else
// 		{
// 			if (0 > p.y)//垂直向量的y分量大于0，向下
// 			{
// 				p.x = - p.x;
// 				p.y = - p.y;
// 			}
// 		}
// 	 
// 		return p;
	}
}