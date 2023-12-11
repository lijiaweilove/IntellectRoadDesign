#pragma once
#include "IntersectResultModel.h"

bool CompareBorderXvalue(IntersectResultModel* sb1, IntersectResultModel* sb2);
//ÆÊÇÐÃæÊý¾Ý
class TerCurResultModel
{
public:
	TerCurResultModel(void);
	~TerCurResultModel(void);
	

public:
	std::vector<IntersectResultModel*> vecResultModel;
public:
	int m_index;
	std::vector<AcGePoint3d> vecPoint;
	AcGePoint3d m_startPoint;
	AcGePoint3d m_endPoint;
	AcGePoint3d m_startExternPoint;
	AcGePoint3d m_endExternPoint;
	AcGePoint3d m_startTextPoint;
	AcGePoint3d m_endTextPoint;
	AcGePoint3d currentPoint;
	double maxDistance;
	double maxElv;
	double minElv;
};




