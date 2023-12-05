#pragma once


#define nweps 1e-6
#define BIM_M_PI_2     1.57079632679489661923
#define BIM_M_PI  3.14159265358979323846

namespace CadCommon
{
	class ArxMathHelper
	{
	public:
		//����ת�Ƕ�
		static double RadianToAngle(double dRadian);
		//�Ƕ�ת����
		static double AngleToRadian(double dAngle);

	};
}
