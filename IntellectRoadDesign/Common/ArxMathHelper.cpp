#include "stdafx.h"
#include "ArxMathHelper.h"

double CadCommon::ArxMathHelper::RadianToAngle(double dRadian)
{
	return dRadian / BIM_M_PI * 180.;
}

double CadCommon::ArxMathHelper::AngleToRadian(double dAngle)
{
	return dAngle / 180. * BIM_M_PI;
}
