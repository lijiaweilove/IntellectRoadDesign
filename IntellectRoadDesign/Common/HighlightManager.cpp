#include "stdafx.h"
#include "HighlightManager.h"
#include "EleData.h"
namespace CadCommon
{
	static HighlightManager* _instance;

	HighlightManager::HighlightManager()
	{
	}

	HighlightManager::~HighlightManager()
	{
		BeginEdit();
		ClearHighLight();
		EndEdit();
	}

	void HighlightManager::ClearVec() {
		ClaerUnorderMap(mapOriginColor);
	}

	void HighlightManager::SetNewHight(AcDbObjectId objectId) 
	{
		SetHighLight(objectId);
	}

	void HighlightManager::PushHighLight(AcDbObjectId objectId)
	{
		SetHighLight(objectId);
	}

	void HighlightManager::PushHighLight(std::vector<AcDbObjectId> vecobject)
	{
#if _MSC_VER >= 1800
		for (AcDbObjectId objId: vecobject)
		{
#else
		for (int index = 0 ;index < vecobject.size();++index)
		{
			AcDbObjectId objId = vecobject[index];
#endif
			SetHighLight(objId);
		}
	}


	void HighlightManager::ClearHighLight() {
// 		for (int index = 0; index < hightLightVec.size(); ++index)
// 		{
// 			Adesk::IntDbId  objectDbId = hightLightVec[index];
// 			AcDbObjectId objId; objId.setFromOldId(objectDbId);
// 			AcDbEntity *pEnt = NULL;
// 			if (Acad::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
// 			{
// 				pEnt->unhighlight();
// 				pEnt->close();
// 			}
// 		}
// 		ClaerVector(hightLightVec);

		BIMHASH_MAP<Adesk::IntDbId,Adesk::UInt16 >::iterator itor =  mapOriginColor.begin();
		for (;itor != mapOriginColor.end();++itor)
		{
			Adesk::IntDbId  objectDbId = (*itor).first;
			Adesk::UInt16 _colorIndex = (*itor).second;
 			AcDbObjectId objId; objId.setFromOldId(objectDbId);
 			AcDbEntity *pEnt = NULL;
 			if (Acad::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
 			{
 				pEnt->setColorIndex(_colorIndex);
 				pEnt->close();
 			}
		}
	}



	void HighlightManager::SetHighLight(AcDbEntity* pEnt) {
		Adesk::IntDbId  objectDbId = pEnt->objectId().asOldId();
		Adesk::UInt16  _color = pEnt->colorIndex();
		pEnt->setColorIndex(10);
		if(mapOriginColor.find(objectDbId) == mapOriginColor.end())
			mapOriginColor.insert(make_pair(objectDbId,_color));
	}

	void HighlightManager::SetHighLight(AcDbObjectId objectId) {
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, objectId, AcDb::kForWrite))
		{
			Adesk::UInt16  _color = pEnt->colorIndex();
			pEnt->setColorIndex(10);
			pEnt->close();
			if(mapOriginColor.find(objectId.asOldId()) == mapOriginColor.end())
				mapOriginColor.insert(make_pair(objectId.asOldId(),_color));
		}
	}


	void HighlightManager::SetHighLight(Adesk::IntDbId  objectDbId) {
		AcDbObjectId objId; objId.setFromOldId(objectDbId);
		AcDbEntity *pEnt = NULL;
		if (Acad::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
		{
			Adesk::UInt16  _color = pEnt->colorIndex();
			pEnt->setColorIndex(10);
			pEnt->close();
			if(mapOriginColor.find(objectDbId) == mapOriginColor.end())
				mapOriginColor.insert(make_pair(objectDbId,_color));
		}
	}

	void HighlightManager::RefreshHigLight() {
		RefreshView();
	}
}