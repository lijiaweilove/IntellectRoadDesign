#include "StdAfx.h"
#include "DataManager.h"
#include "Common/EleData.h"
static DataManager* pInstanse = NULL;

DataManager::DataManager(void)
{

}

DataManager::~DataManager(void)
{

}

DataManager* DataManager::Instanse()
{
	if (pInstanse == NULL)
	{
		pInstanse = new DataManager();
	}
	return pInstanse;
}

void DataManager::Terminal()
{
	SAFE_DELETE(pInstanse);
}

void DataManager::SetDepartLength(int value)
{
	departLength = value;
}
int DataManager::GetDepartLength() 
{
	return departLength;
}

void DataManager::SetMaxSlope(double value)
{
	maxSlope = value;
}
double DataManager::GetMaxSlope() 
{
	return maxSlope;
}

void DataManager::SetSelectPoint(AcGePoint3d value)
{
	ptSelect = value;
}

AcGePoint3d DataManager::GetSelectPoint()
{
	return ptSelect;
}
