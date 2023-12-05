#include "StdAfx.h"
#include "Target.h"
#include "Common/Tools.h"

CTarget::CTarget(void)
{
	center.InitPoint3D();
	site=NULL;
	fan=NULL;
	radius=0;
	altitude=0;
	area=0;
}


CTarget::CTarget(AcDbEntity* pEnt)
{
	center.InitPoint3D();
	site=NULL;
	fan=NULL;
	radius=0;
	altitude=0;
	area=0;
	AcDbPolyline* pline=AcDbPolyline::cast(pEnt);
	site=AcDbPolyline::cast(pEnt);
	unsigned int ptNum = pline->numVerts();
	struct resbuf* pointList = NULL;
	struct resbuf* curPtNode = NULL;
	AcGePoint2d prePt, curPt;
	std::vector<AcGePoint2d> prePtList;
	for(int ptIdx = 0; ptIdx < ptNum; ++ptIdx) {
		if(pointList == NULL) {
			pline->getPointAt(ptIdx, curPt);
			ads_point curAdsPt = {curPt.x, curPt.y, 0};
			pointList = acutBuildList(RTPOINT, curAdsPt, RTNONE);
			curPtNode = pointList;	
			prePt = curPt;
			prePtList.push_back(curPt);
			linePoint.push_back(new CPoint3D(curPt.x,curPt.y,0));
			continue;
		}
		pline->getPointAt(ptIdx, curPt);
		bool isbreak=false;
		for (int i=0;i<prePtList.size();i++)
		{
			if(curPt.x == prePtList[i].x&&curPt.y == prePtList[i].y)	
				isbreak=true;
		}
		if(isbreak||prePtList.size()==4){
			continue;
		}
		prePtList.push_back(curPt);
	
		
		linePoint.push_back(new CPoint3D(curPt.x,curPt.y,0));
		ads_point curAdsPt = {curPt.x, curPt.y, 0};
		curPtNode->rbnext = acutBuildList(RTPOINT, curAdsPt, RTNONE);
		curPtNode = curPtNode->rbnext;
		prePt = curPt;
	}
	//����Ұָ��
	curPtNode->rbnext = NULL;
	curPtNode = NULL;

	ads_name ssname;
	acedSSGet(L"CP", pointList, NULL, NULL, ssname);

	//��ȡѡ����ʵ����Ŀ�����浽len
	Adesk::Int32  len = 0;
	acedSSLength(ssname, &len);

	for (int index = 0; index < len; ++index)
	{
		//��ȡѡ����ʵ�����ƣ�������entname
		ads_name entname;
		if (RTNORM == acedSSName(ssname, index, entname))
		{
			//����ʵ�����ƻ�ȡʵ��id
			AcDbObjectId mp_Ref_Id;
			if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entname))
			{
				//��ȡʵ�����
				AcDbEntity* pEnt1 = NULL;
				if(Acad::eOk == acdbOpenAcDbEntity(pEnt1,mp_Ref_Id,AcDb::kForRead)){
					ACHAR* lName = pEnt1->layer();
					std::wstring lNameStr = lName;
					acutDelString(lName); 
					//��ͼ����ת��ΪСд
					BIM::Tools::lowerWstring(lNameStr);
					// ƽ����
					if(lNameStr.find(L"�½�·") != std::wstring::npos) {
						if(pEnt1->isA() == AcDbCircle::desc()) {
							fan=AcDbCircle::cast(pEnt1);
							radius=fan->radius();
							AcGePoint3d midPoint=fan->center();
							center.AcGeP3ToCP(midPoint);
						}
					}
					else{
						if(pEnt)pEnt->close();
					}

				}
			}
		}
	}

	altitude = site->elevation();
	site->getArea(area);
}
CTarget::~CTarget(void)
{
	if (site)
	{
		site->close();
	}

	if (fan)
	{
		fan->close();
	}
	
}
CPoint3D CTarget::GetCenter(){
	return center;
}
double CTarget::GetRadius(){
	return radius;
}
double CTarget::GetAltitude(){
	return altitude;
}
double CTarget::GetArea(){
	return area;
}
