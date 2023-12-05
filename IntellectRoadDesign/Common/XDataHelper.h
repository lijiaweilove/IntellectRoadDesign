#pragma once
namespace CadCommon {
	class XDataHelper
   	{
	public:
		static bool GetAppXdata(const AcDbEntity* pEntity,std::wstring appname, std::wstring& cstrName);
		static bool GetAppXdata(Adesk::IntDbId  intDbId, std::wstring appname, std::wstring& cstrName);
		static bool GetAppXdata(AcDbObjectId  objectId, std::wstring appname, std::wstring& cstrName);
		static bool AddAppXdataByOldId(Adesk::IntDbId  intDbId, std::wstring appname, std::wstring const& cstrName,  bool removeOldId);
		static bool AddAppXdataByOldId(AcDbObjectId  objectId, std::wstring appname, std::wstring const& cstrName, bool removeOldId);
		static bool AddAppXdataByOldId(AcDbEntity* pEnt, std::wstring appname, std::wstring const& cstrName, bool removeOldId);

		static void RemoveXDataByOldId(const Adesk::IntDbId  intDbId, std::wstring appname);
		static void RemoveXDataByObjectId(const AcDbObjectId  objectId, std::wstring appname);
		static void RemoveXDataByEntity(AcDbEntity* pEnt,std::wstring appname);
	private:
		static void CreateAppName(AcDbEntity* pEnt, std::wstring appname, std::wstring const& cstrName);
	};

}