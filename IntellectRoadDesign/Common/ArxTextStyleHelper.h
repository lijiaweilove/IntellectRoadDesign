#pragma once
#include "ArxHelper.h"
#include <vector>
namespace CadCommon
{

	class TextStyle
	{
	public:
		std::wstring styleName;
		AcDbObjectId textStyleId;
	};


	class ArxTextStyleHelper
	{
	public:
		static std::vector<TextStyle*> GetAllTextStyle(AcDbDatabase* pDb = NULL);
		static std::vector<std::wstring> GetAllTextStyleName(AcDbDatabase* pDb = NULL);
		static AcDbObjectId CreateTextStyle(
			 std::wstring  fontName,
			std::wstring  bigFontName,
			 double textSize,
			 double xScale,
			 double obliqueAngle,
			 double trPercent,
			 Adesk::Boolean isBackward,
			 Adesk::Boolean isUpsideDown,
			 Adesk::Boolean isVertical,
			 Adesk::Boolean isOverlined,
			 Adesk::Boolean isUnderlined,
			 Adesk::Boolean isStrikethrough,
			std::wstring         styleName,AcDbDatabase* pDb = NULL);

		static bool GetTextStyle(std::wstring styleName,AcDbObjectId& textStyleId,AcDbDatabase* pDb = NULL);

	};
}