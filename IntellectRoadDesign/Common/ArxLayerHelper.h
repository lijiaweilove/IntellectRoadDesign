#pragma once
#include "ArxHelper.h"
#include <vector>
namespace CadCommon
{

	class  LayerInfo
	{
	public:
		LayerInfo(){
			_name = L"";
			_entId.setNull();
		}
		~LayerInfo(){}
	public:
		std::wstring _name;
		AcDbObjectId _entId;
	};



	class ArxLayerHelper
	{
	public:
		//获得当前图纸中所有图层
		static std::vector<LayerInfo*> GetAllLayer(AcDbDatabase* pdb = NULL);
		//获得所有未冻结的图层
		static std::vector<LayerInfo*> GetAllLayerWithoutFreeze(AcDbDatabase* pdb = NULL);

	
		//输入名称， 创建图层，返回图层ID,图层默认为隐藏
		static AcDbObjectId CreateLayer(std::wstring layerName, bool bIsHiden = true,AcDbDatabase* pdb = NULL);
		//判断图层是否已经存在
		static bool IsAlreadyExitLayer(std::wstring layerName, AcDbDatabase* pdb = NULL);
		//
		static AcDbObjectId GetLayerId(std::wstring layerName, AcDbDatabase* pdb = NULL);// Adesk::UInt16 colorIndex,
		
		static AcDbObjectId GetLayerId(std::wstring layerName, int colorIdx, AcDbDatabase* pdb = NULL);
		
		//设置图层颜色
		static void SetLayerColor(std::wstring layerName, uint colorIndex, AcDbDatabase* pdb = NULL);
		//删除图层
		static void DeleteLayer(std::wstring layerName, AcDbDatabase* pdb = NULL);
		//设置图层是否可见
		static void SetLayerIsOff(std::wstring layerName,bool bIsOff, AcDbDatabase* pdb = NULL);
		static AcDbObjectId GetLayerId2(std::wstring name,AcDbDatabase* pdb);
	};
}