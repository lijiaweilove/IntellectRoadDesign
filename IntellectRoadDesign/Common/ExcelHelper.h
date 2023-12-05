#pragma once
#include <vector>
namespace BIM
{
	class ExcelHelper
	{
	public:
		static std::vector<std::wstring> GetAllSheetName(std::wstring sExcelFileName);
	};
}