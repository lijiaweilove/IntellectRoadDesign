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
		//��õ�ǰͼֽ������ͼ��
		static std::vector<LayerInfo*> GetAllLayer(AcDbDatabase* pdb = NULL);
		//�������δ�����ͼ��
		static std::vector<LayerInfo*> GetAllLayerWithoutFreeze(AcDbDatabase* pdb = NULL);

	
		//�������ƣ� ����ͼ�㣬����ͼ��ID,ͼ��Ĭ��Ϊ����
		static AcDbObjectId CreateLayer(std::wstring layerName, bool bIsHiden = true,AcDbDatabase* pdb = NULL);
		//�ж�ͼ���Ƿ��Ѿ�����
		static bool IsAlreadyExitLayer(std::wstring layerName, AcDbDatabase* pdb = NULL);
		//
		static AcDbObjectId GetLayerId(std::wstring layerName, AcDbDatabase* pdb = NULL);// Adesk::UInt16 colorIndex,
		
		static AcDbObjectId GetLayerId(std::wstring layerName, int colorIdx, AcDbDatabase* pdb = NULL);
		
		//����ͼ����ɫ
		static void SetLayerColor(std::wstring layerName, uint colorIndex, AcDbDatabase* pdb = NULL);
		//ɾ��ͼ��
		static void DeleteLayer(std::wstring layerName, AcDbDatabase* pdb = NULL);
		//����ͼ���Ƿ�ɼ�
		static void SetLayerIsOff(std::wstring layerName,bool bIsOff, AcDbDatabase* pdb = NULL);
		static AcDbObjectId GetLayerId2(std::wstring name,AcDbDatabase* pdb);
	};
}