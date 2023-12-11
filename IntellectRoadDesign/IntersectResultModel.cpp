#include "StdAfx.h"
#include "IntersectResultModel.h"

IntersectResultModel::IntersectResultModel(void)
{
}

IntersectResultModel::~IntersectResultModel(void)
{
}

void IntersectResultModel::SetValue(double _distance, AcGePoint3d _point)
{
	m_distance = _distance;
	m_height = _point.z;
	m_point = _point;
}