#include "StdAfx.h"
#include "BaseData.h"

CBaseData::CBaseData(void)
{
}


CBaseData::~CBaseData(void)
{
	for (size_t i = 0; i < contours.size(); i++) {
		delete contours[i];
		contours[i]=NULL;
	}
	for (size_t i = 0; i < oldRoads.size(); i++) {
		delete oldRoads[i];
		oldRoads[i]=NULL;
	}
	for (size_t i = 0; i < targets.size(); i++) {
		delete targets[i];
		targets[i]=NULL;
	}
	for (size_t i = 0; i < obstacles.size(); i++) {
		delete obstacles[i];
		obstacles[i]=NULL;
	}
}


void CBaseData::InitData(){
	contours.resize(0);
	oldRoads.resize(0);
	targets.resize(0);
	obstacles.resize(0);
}
void CBaseData::DataInfo(){
	acutPrintf(L"\n已有道路数据：%d条",oldRoads.size());
	acutPrintf(L"\n已有等高线数据：%d条",contours.size());
	acutPrintf(L"\n已有场地数据：%d条",targets.size());
	acutPrintf(L"\n已有障碍物数据：%d条",obstacles.size());
}