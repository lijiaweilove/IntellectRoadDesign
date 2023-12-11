#pragma once
class IntersectResultModel
{
public:
	IntersectResultModel(void);
	~IntersectResultModel(void);

public:
	void SetValue(double _distance, AcGePoint3d _point);

public:
	double m_distance;
	double m_height;
	AcGePoint3d m_point;
	int m_pointType; //0 计算点 1 起点 2 终点
};

