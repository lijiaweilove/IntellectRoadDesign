#include "StdAfx.h"
#include "ArxTextStyleHelper.h"

namespace CadCommon
{

	std::vector<TextStyle*> ArxTextStyleHelper::GetAllTextStyle(AcDbDatabase* pDb)
	{
		std::vector<TextStyle*> vecTextStyle;
		AcDbTextStyleTable* textStyleTable = NULL;
		AcDbDatabase* pCurDb = pDb == NULL ? acdbCurDwg() : pDb;
		if (Acad::eOk == pCurDb->getTextStyleTable(textStyleTable, AcDb::kForRead))
		{
			AcDbTextStyleTableIterator *pIt = NULL;
			textStyleTable->newIterator(pIt);
			for (; !pIt->done(); pIt->step())
			{
				AcDbTextStyleTableRecord *pRcd = NULL;
				if (pIt->getRecord(pRcd, AcDb::kForRead) == Acad::eOk)
				{
					TCHAR *szName = NULL;
					pRcd->getName(szName);
					std::wstring wstrname = szName;
					TextStyle* ts = new TextStyle();
					ts->styleName = wstrname;
					ts->textStyleId = pRcd->id();
					vecTextStyle.push_back(ts);
					pRcd->close();
				}
			}
			delete pIt;
			textStyleTable->close();
		}
		return vecTextStyle;
	}

	std::vector<std::wstring> ArxTextStyleHelper::GetAllTextStyleName(AcDbDatabase* pDb)
	{
		std::vector<std::wstring> vecTextStyleName;
		AcDbTextStyleTable* textStyleTable = NULL;
		AcDbDatabase* pCurDb = pDb == NULL ? acdbCurDwg() : pDb;
		if (Acad::eOk == pCurDb->getTextStyleTable(textStyleTable, AcDb::kForRead))
		{
			AcDbTextStyleTableIterator *pIt = NULL;
			textStyleTable->newIterator(pIt);
			for (; !pIt->done(); pIt->step())
			{
				AcDbTextStyleTableRecord *pRcd = NULL;
				if (pIt->getRecord(pRcd, AcDb::kForRead) == Acad::eOk)
				{
					TCHAR *szName = NULL;
					pRcd->getName(szName);
					std::wstring wstrname = szName;
					vecTextStyleName.push_back(wstrname);
					pRcd->close();
				}
			}
			delete pIt;
			textStyleTable->close();
		}
		return vecTextStyleName;
	}



	AcDbObjectId ArxTextStyleHelper::CreateTextStyle(std::wstring fontName, 
		std::wstring bigFontName, double textSize, double xScale, double obliqueAngle,
		double trPercent, Adesk::Boolean isBackward, Adesk::Boolean isUpsideDown, 
		Adesk::Boolean isVertical, Adesk::Boolean isOverlined, Adesk::Boolean isUnderlined,
		Adesk::Boolean isStrikethrough, std::wstring styleName,AcDbDatabase* pDb)
	{
		AcDbObjectId textStyleId;
		textStyleId.setNull();
		CString cstrlayerName = styleName.c_str();
		ACHAR* pTextStyleName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();

		CString cstrFontName = fontName.c_str();
		ACHAR* pFontName = cstrFontName.GetBuffer();
		cstrFontName.ReleaseBuffer();

		CString cstrbigFontName = bigFontName.c_str();
		ACHAR* pbigFontName = cstrbigFontName.GetBuffer();
		cstrbigFontName.ReleaseBuffer();

		AcDbDatabase* pCurDb = pDb == NULL ? acdbCurDwg() : pDb;

		AcDbTextStyleTable* textStyleTable = NULL;
		if (Acad::eOk == pCurDb->getTextStyleTable(textStyleTable, AcDb::kForWrite))
		{
			if (Acad::eOk != textStyleTable->getAt(L"????", textStyleId, false))
			{
				AcGiTextStyle *TextStyle = new AcGiTextStyle(pFontName, pbigFontName, textSize, xScale, obliqueAngle, trPercent
					, isBackward, isUpsideDown, isVertical, isOverlined, isUnderlined,cstrlayerName); //??????
// 				TextStyle->loadStyleRec();
				Acad::ErrorStatus es = toAcDbTextStyle(*TextStyle, textStyleId);
			}
			textStyleTable->close();
		}
		else {
			AcGiTextStyle *TextStyle = new AcGiTextStyle(pFontName, pbigFontName, textSize, xScale, obliqueAngle, trPercent
				, isBackward, isUpsideDown, isVertical, isOverlined, isUnderlined, cstrlayerName);
// 			TextStyle->setFileName(_T("SIMSUM.TTF"));
// 			TextStyle->loadStyleRec();
			toAcDbTextStyle(*TextStyle, textStyleId);
		}
		return textStyleId;
	}



	bool ArxTextStyleHelper::GetTextStyle(std::wstring styleName, AcDbObjectId& textStyleId,AcDbDatabase* pDb)
	{
		AcDbDatabase* pCurDb = pDb == NULL ? acdbCurDwg() : pDb;
		bool bRet = false;
		textStyleId.setNull();
		CString cstrlayerName = styleName.c_str();
		ACHAR* pTextStyleName = cstrlayerName.GetBuffer();
		cstrlayerName.ReleaseBuffer();
		AcDbTextStyleTable* textStyleTable = NULL;
		if (Acad::eOk == pCurDb->getTextStyleTable(textStyleTable, AcDb::kForWrite))
		{
			if (Acad::eOk == textStyleTable->getAt(pTextStyleName, textStyleId, false))
			{
				bRet = true;
			}
			textStyleTable->close();
		}
		return bRet;
	}
}