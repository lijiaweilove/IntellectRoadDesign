#include "stdafx.h"
#include "ArxGetSelectPoint.h"

namespace CadCommon
{
	AcGePoint3d ArxGetSelectPoint::WcsToUcsPoint(const AcGePoint3d & point)
	{
		// 转换成世界坐标	
		AcGePoint3d pt;
		struct resbuf rbFrom, rbTo;
		rbFrom.restype = RTSHORT;
		rbFrom.resval.rint = 0; // from WCS
		rbTo.restype = RTSHORT;
		rbTo.resval.rint = 1; // to UCS
		acedTrans(asDblArray(point), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));
		return pt;
	}

	AcGePoint3d ArxGetSelectPoint::UcsToWcsPoint(const AcGePoint3d & point)
	{
		// 转换成世界坐标	
		AcGePoint3d pt;
		struct resbuf rbFrom, rbTo;
		rbFrom.restype = RTSHORT;
		rbFrom.resval.rint = 1;
		rbTo.restype = RTSHORT;
		rbTo.resval.rint = 0;

		acedTrans(asDblArray(point), &rbFrom, &rbTo, Adesk::kFalse, asDblArray(pt));

		return pt;
	}

	bool ArxGetSelectPoint::GetPoint(const AcGePoint3d & basePoint, const TCHAR * prompt, AcGePoint3d & point)
	{
		return (GetPointReturnCode(basePoint, prompt, point) == RTNORM);
	}

	int ArxGetSelectPoint::GetPointReturnCode(const AcGePoint3d &basePoint, const TCHAR* prompt, AcGePoint3d &point)
	{
		//将基点转换为UCS坐标
		AcGePoint3d ucsBasePoint = WcsToUcsPoint(basePoint);
		int nReturn = acedGetPoint(asDblArray(ucsBasePoint), prompt, asDblArray(point));
		if (nReturn == RTNORM)
		{
			//acedGetPoint得到UCS坐标，转换为WCS
			point = UcsToWcsPoint(point);
		}
		return nReturn;
	}


	int ArxGetSelectPoint::GetSelectPointReturnCode(const TCHAR* prompt, AcGePoint3d &point,const ads_point pt)
	{

		int nReturn = acedGetPoint(pt, prompt, asDblArray(point));
		if (nReturn == RTNORM)
		{
			point = UcsToWcsPoint(point);
		}
		return nReturn;
	}

}