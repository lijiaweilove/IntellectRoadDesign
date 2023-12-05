#pragma once

namespace CadCommon
{
	class ArxGetSelectPoint
	{
	public:
		static AcGePoint3d WcsToUcsPoint(const AcGePoint3d &point);

		static AcGePoint3d UcsToWcsPoint(const AcGePoint3d &point);

		static bool GetPoint(const AcGePoint3d &basePoint,
			const TCHAR* prompt, AcGePoint3d &point);

		static int GetSelectPointReturnCode(const TCHAR* prompt, AcGePoint3d &point ,const ads_point pt = NULL);

		static int GetPointReturnCode(const AcGePoint3d &basePoint, const TCHAR* prompt, AcGePoint3d &point);

	};
}