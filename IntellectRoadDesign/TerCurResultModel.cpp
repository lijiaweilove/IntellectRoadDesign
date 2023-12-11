#include "StdAfx.h"
#include "TerCurResultModel.h"
#include "Common/EleData.h"

TerCurResultModel::TerCurResultModel(void)
{
}

TerCurResultModel::~TerCurResultModel(void)
{
	//CadCommon::ClaerVector(vecResultModel);
}

bool CompareBorderXvalue(IntersectResultModel* sb1, IntersectResultModel* sb2)
{
	if (sb1 == NULL || sb2 == NULL)
		return true;
	double distance1 = sb1->m_distance;
	double distance2 = sb2->m_distance;

	if (distance1 - distance2 > Cable_Point_EPSILON)
	{

		return false;
	}
	return true;
}
