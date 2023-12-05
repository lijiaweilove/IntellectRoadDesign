#pragma once


#define nweps 1e-6
#define BIM_M_PI_2     1.57079632679489661923
#define BIM_M_PI  3.14159265358979323846

namespace CadCommon
{
	class ArxMathHelper
	{
	public:
		//弧度转角度
		static double RadianToAngle(double dRadian);
		//角度转弧度
		static double AngleToRadian(double dAngle);

	};
}
