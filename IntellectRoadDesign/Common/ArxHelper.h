#pragma once
#include "EleData.h"
#include <vector>
#include <set>
namespace CadCommon
{
#define EPSINON 1e-15
#define PI 3.1415926
	class ArxHelper
	{
	public:
		//��õ�ǰDWGͼֽĿ¼
		static std::wstring GetDWGFilePath();
		//��õ�ǰarx���Ŀ¼
		static std::wstring GetArxPluginPath();
		static std::wstring GetCADPath();

		//RGBת	CAD��ɫ����
		static uint GetColorIndexByColor(CadCommon::ARXColor _color);
		//CAD��ɫ����תRGB
		static bool GetRGBColorByIndex(uint _colorIndex, CadCommon::ARXColor& color);
		static uint GetColorIndexByColor(long red , long green, long blue);
		//������ͼ
		static int CenterObject(AcDbObjectId _objID);
		static int CenterObject(std::vector<AcDbObjectId> _objIDs);
		static int CenterObject(AcDbEntity * _obj);
		static int CenterObject(std::set<AcDbObjectId> objectList);
		static AcGePoint2d GetMidPoint(CadCommon::Cable_Box & pBox);
		static AcGePoint2d GetMidPoint(AcGePoint2d & p1, AcGePoint2d & p2);
		static bool Zoom(AcGePoint2d & p1, AcGePoint2d & p2);
		static bool Zoom(CadCommon::Cable_Box & pBox);
		//���Ԫ�ذ�Χ��
		static bool GetEntityBox(AcDbEntity* entity, CadCommon::Cable_Box& box);
		//���Ԫ�����ĵ�
		static bool GetMidPoint(AcDbEntity* entity, CadCommon::Cable_Point& point);

		static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt);
		static AcDbObjectId PostToModelSpace(AcDbDatabase* pDb,AcDbEntity* pEnt);

		static void SetEntityColor(AcDbObjectId objId,uint colorIndex);
		static void AcgePoint3dToads_point(const AcGePoint3d& pt, ads_point& adsPoint);

		static double Max(double a, double b);
		static double Min(double a, double b);
		//�ƶ�
		static Acad::ErrorStatus Move(AcDbObjectId entId, AcGePoint3d ptBase,
			AcGePoint3d ptDest);
		//����
		static Acad::ErrorStatus Scale(AcDbObjectId entId,
			AcGePoint3d ptBase, double scaleFactor);
		static AcDbObjectId CreateRectangle(AcGePoint2d pt1, AcGePoint2d pt2,
			double width);
		static AcDbObjectId WhCHAP2CreateLine(AcGePoint3d pt1, AcGePoint3d pt2);

		static bool CopyDBEntity(std::wstring filePath,AcDbDatabase*& pDB,AcGePoint3d mPoint);

		static AcDbObjectId CreateLine(AcGePoint3d point1,AcGePoint3d point2,AcDbObjectId _layerId,int colorIndex);
		static AcDbObjectId CreateLine(AcGePoint3d point1,AcGePoint3d point2,AcDbObjectId _layerId,int colorIndex,AcDbObjectId _lineTypeId,double _linetypeScale);

		static AcDbObjectId CreateText(AcGePoint3d point,std::wstring txtStr,int colorIndex,double _height, double widFactor,
			AcDbObjectId _layId = AcDbObjectId::kNull,AcDbObjectId txtStyleId = AcDbObjectId::kNull);

		static  AcDbObjectId CreateCircle(double radius,const AcGePoint3d& _centerPoint,AcDbObjectId _layId,int colorIndex);
		static  AcDbObjectId CreateCircle(double radius,const AcGePoint3d& _centerPoint,AcDbObjectId _layId,int colorIndex,AcDbObjectId _lineTypeId,double _linetypeScale);

		static AcDbObjectId CreateDiametricDimension(double txtradius,double radius,const AcGePoint3d& _centerPoint,AcGeVector3d vDir,AcDbObjectId _dimStyleId);

	};
}

