#pragma once
#include <vector>
#include <iostream>
#include "EleData.h"
using namespace std;
namespace CadCommon
{
	class HighlightManager
	{
	public:
		HighlightManager();
		~HighlightManager();


		void SetNewHight(AcDbObjectId objectId);
		void PushHighLight(AcDbObjectId objectId);
		void PushHighLight(std::vector<AcDbObjectId> vecobject);

		void InsertVec(Adesk::IntDbId objectDbId);
		inline void ClearVec();
		void ClearHighLight();
		void SetHighLight(AcDbEntity* pEnt);
		void SetHighLight(AcDbObjectId objectId);
		void SetHighLight(Adesk::IntDbId  objectOldId);

		void RefreshHigLight();
	private:
		BIMHASH_MAP<Adesk::IntDbId,Adesk::UInt16 > mapOriginColor;		
	};

}