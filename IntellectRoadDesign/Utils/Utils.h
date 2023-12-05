#pragma once
#include <vector>
#include <dbpl.h>
#include <dbapserv.h>
#include <dbsymtb.h>
#include <adscodes.h>

class Utils
{
public:
	Utils();
	~Utils();
	//获取指定多段线范围内的指定图层所有实体
	static std::vector<AcDbEntity*> getEntitys(wchar_t* entityLayerName, AcDbPolyline* range = NULL);

	//获取指定多段线范围内的指定图层所有多段线
	static std::vector<AcDbPolyline*> getBorders(wchar_t* entityLayerName, AcDbPolyline* range = NULL);

	//根据获取选择集内所有多段线
	static std::vector<AcDbPolyline*> getBorders(const ads_name& ssname);

	/**
	 * 获取指定多段线范围内的指定图层所有多段线
	 * @param entityLayerName: 图层名所包含的字串
	 * @param negateLayerName：一定不存在图层名的字串
	*/
	static std::vector<AcDbPolyline*> getBordersByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName = NULL, AcDbPolyline* range = NULL);

	/**
	 * 获取指定多段线范围内的指定图层所有实体
	 * @param entityLayerName: 图层名所包含的字串
	 * @param negateLayerName：一定不存在图层名的字串
	*/
	static std::vector<AcDbEntity*> getEntityByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName = NULL, AcDbPolyline* range = NULL);
};


