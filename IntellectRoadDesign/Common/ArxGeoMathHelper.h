#pragma once
#include "Cable_Point.h"
#include <cmath>
#include "EleData.h"

namespace CadCommon
{
	enum AngvalueType
	{
		ANGLE,
		RADIAN
	};
	class ArxGeoMathHelper
	{

	public:
	
		static double RadianToAngle(double radian);
		static Cable_Point  CalcVectorBy2Point(Cable_Point const& p1 ,Cable_Point const& p2);
		static Cable_Point CalcVectorBy2Point(AcGePoint3d p1,AcGePoint3d p2);

		static AcGePoint3d CalcPointBySVL(Cable_Point const& p1,Cable_Point const& vec,double length);
		static AcGePoint3d CalcPointBySEL(Cable_Point const& p1,Cable_Point const& p2,double length);
		static AcGePoint3d CalcPointBySEL(AcGePoint3d const& p1,AcGePoint3d const& p2,double length);
		static std::vector<AcGePoint3d> GetPolyLinePointList(AcDbObjectId polyLineId);

		static double CalcPolyLineLength(AcDbObjectId entId);

		static double CalcLengthByTwoPoint(const AcGePoint3d& p1,const AcGePoint3d& p2);

		static bool CalcIntersection(AcDbEntity* pEntity1,AcDbEntity* pEntity2,AcGePoint3d& pointInts);

		static bool PointInLine(AcGePoint3d const& pStart,AcGePoint3d const& pEnd,AcGePoint3d const& pPoint);


		//���ֱ�ߴ�ֱ��λ����
		static 	AcGePoint3d   GetUnitVerticalVector (AcGePoint3d pointO,AcGePoint3d pointA,int type );

		//�����ֱ�߽�ƽ��������
		static AcGePoint3d Angle_bisector(AcGePoint3d left, AcGePoint3d center, AcGePoint3d right );

		//����������н�,����
		static double AngleVecToVec(AcGePoint3d left, AcGePoint3d center, AcGePoint3d right, AngvalueType agt = RADIAN  );

		//���������X��н�
		static double AngleVectorXaxis(AcGePoint3d left, AcGePoint3d center);

		static double AngleVectorXaxis(AcGePoint3d vector);

		static double AngleVectorXaxis(AcGeVector3d vector);

		//���������Y��н�
		static double AngleVectorYaxis(AcGePoint3d left, AcGePoint3d center);

		static double AngleVectorYaxis(AcGePoint3d vector);

	};
}