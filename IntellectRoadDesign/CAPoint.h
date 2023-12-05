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

	double m_x, m_y;	//����
	int m_X, m_Y;//�ڵ�������ά��������
	double m_F, m_G, m_H;	//F = G + H
	AType m_type;	////����:�ϰ��������б��ر��б�δ֪����·
	bool isRoad;
	CAPoint* m_parent;	//���ڵ�
	int m_slopeLevel;	//�¶ȵȼ�����ʾ�м����ȸ���
	double m_elevation;	//���θ߶�


	bool operator==(const CAPoint& point)
	{
		if (m_X == point.m_X && m_Y == point.m_Y)
		{
			return true;
		}
		return false;
	}
};
