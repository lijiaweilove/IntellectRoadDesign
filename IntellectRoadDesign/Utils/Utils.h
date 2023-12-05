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
	//��ȡָ������߷�Χ�ڵ�ָ��ͼ������ʵ��
	static std::vector<AcDbEntity*> getEntitys(wchar_t* entityLayerName, AcDbPolyline* range = NULL);

	//��ȡָ������߷�Χ�ڵ�ָ��ͼ�����ж����
	static std::vector<AcDbPolyline*> getBorders(wchar_t* entityLayerName, AcDbPolyline* range = NULL);

	//���ݻ�ȡѡ�������ж����
	static std::vector<AcDbPolyline*> getBorders(const ads_name& ssname);

	/**
	 * ��ȡָ������߷�Χ�ڵ�ָ��ͼ�����ж����
	 * @param entityLayerName: ͼ�������������ִ�
	 * @param negateLayerName��һ��������ͼ�������ִ�
	*/
	static std::vector<AcDbPolyline*> getBordersByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName = NULL, AcDbPolyline* range = NULL);

	/**
	 * ��ȡָ������߷�Χ�ڵ�ָ��ͼ������ʵ��
	 * @param entityLayerName: ͼ�������������ִ�
	 * @param negateLayerName��һ��������ͼ�������ִ�
	*/
	static std::vector<AcDbEntity*> getEntityByLayer(wchar_t* entityLayerName, wchar_t* negateLayerName = NULL, AcDbPolyline* range = NULL);
};


