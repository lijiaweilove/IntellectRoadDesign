#include "StdAfx.h"
#include "CAPoint.h"

CAPoint::CAPoint(void) :m_x(0.), m_y(0.), m_X(0), m_Y(0), m_G(0.), m_H(0.), m_F(0.), m_parent(NULL), m_type(ATYPE_UNKNOWN), isRoad(false), m_slopeLevel(0), m_elevation(0.)
{

}

CAPoint::CAPoint(double x, double y, double X, double Y, double F, double G, double H, AType type, CAPoint* parent, double elevation, int slopeLevel)
{
	m_x = x;
	m_y = y;
	m_X = X;
	m_Y = Y;
	m_F = F;
	m_G = G;
	m_H = H;
	m_type = type;
	isRoad = false;
	m_parent = parent;
	m_slopeLevel = slopeLevel;
	m_elevation = elevation;
}

CAPoint::~CAPoint(void)
{

}
