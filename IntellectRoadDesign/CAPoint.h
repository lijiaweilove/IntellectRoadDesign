#pragma once
#include <iostream>

enum AType
{
	ATYPE_UNKNOWN,
	ATYPE_CLOSED,
	ATYPE_OPENED,
	ATYPE_BARRIER,
	ATYPE_ROAD
};

class CAPoint
{	
public:
	CAPoint(void);
	CAPoint(double x, double y, double X, double Y, double F = 0., double G = 0., double H = 0., AType type = ATYPE_UNKNOWN
		, CAPoint* parent = NULL, double elevation = 0, int slopeLevel = 0);
	~CAPoint(void);

	double m_x, m_y;	//坐标
	int m_X, m_Y;//节点所处二维数组坐标
	double m_F, m_G, m_H;	//F = G + H
	AType m_type;	////类型:障碍、开放列表、关闭列表、未知、道路
	bool isRoad;
	CAPoint* m_parent;	//父节点
	int m_slopeLevel;	//坡度等级，表示有几条等高线
	double m_elevation;	//海拔高度


	bool operator==(const CAPoint& point)
	{
		if (m_X == point.m_X && m_Y == point.m_Y)
		{
			return true;
		}
		return false;
	}
};
