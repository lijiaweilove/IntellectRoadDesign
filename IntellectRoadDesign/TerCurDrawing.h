#pragma once
#include "RoadNet.h"
#include "BaseData.h"
#include "TerCurResultModel.h"
class TerCurDrawing
{
	
public:
	TerCurDrawing();
	static TerCurDrawing* Instanse();
	~TerCurDrawing(void);

public:
	void SetStageValue(Edge edge);
	void ClearDGX();
	void readDGX();//��ȡ�ȸ���
	void CalcAllIntersectPoint(AcGePoint3dArray points,CString text);//������ȸ��ߵ��ཻ��
	void DrawingSectionalView();//��������ͼ
	double CalcArea(AcGePoint3d point1, AcGePoint3d point2, AcGePoint3d point3, AcDbObjectId textLayerId, AcDbObjectId _frameLayerId, std::wstring text, AcGePoint3dArray array);

private:
	AcGePoint3d startPoint;
	AcGePoint3d endPoint;
	CString startName;
	CString endName;
	CBaseData* baseData;
	double fill;
	double cut;

	//���߳�������
	int verticalLength;
	std::vector<AcDbEntity*> vecCurrntLines;
	AcGePoint3d GetRoadPoint(double angle,AcGePoint3d point, AcDb2dPolyline* pPolyFormLine);

public:
	//std::vector<TerCurResultModel*> vecCutResult;
	std::map<CString, std::vector<TerCurResultModel*>> mapCutResult;//�洢������Ҫ���Ƶ�����ͼ����


};

