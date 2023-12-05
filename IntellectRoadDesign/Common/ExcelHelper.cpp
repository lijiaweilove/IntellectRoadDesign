#include "stdafx.h"
#include "ExcelHelper.h"

#include "Tools.h"
#include <sstream>

#ifdef USE_LIBXL
#include "libxl.h"
#endif


namespace BIM {
	std::vector<std::wstring> ExcelHelper::GetAllSheetName(std::wstring sExcelFileName)
	{

		std::vector<std::wstring> vec_SheetName;
	#ifdef USE_LIBXL
		std::wstring wstrextention = Tools::getFileExtention(sExcelFileName);
		//¶ÁÈ¡excelÎÄ¼þ
		libxl::Book* book = NULL;
		if (wstrextention == L".xls") {
			book = xlCreateBook();
		}
		else if (wstrextention == L".xlsx") {
			book = xlCreateXMLBook();
		}
		if (book == NULL) return vec_SheetName;
		book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc");
		bool bRet = false;
		if (book->load(sExcelFileName.c_str()))
		{
			int sheetCount = book->sheetCount();
			for (int index = 0; index < sheetCount; ++index)
			{
				std::wstring wstrSheetName = book->getSheetName(index);
				vec_SheetName.push_back(wstrSheetName);
			}
		}
		book->release();
#endif

		return vec_SheetName;
	}
}