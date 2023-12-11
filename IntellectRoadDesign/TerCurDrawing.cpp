#include "StdAfx.h"
#include "TerCurDrawing.h"
#include "Common/Tools.h"
#include "ContourModel.h"
#include "Common/ArxGeoMathHelper.h"
#include "TerCurResultModel.h"
#include "DataManager.h"
#include "common/EleData.h"
#include "Common/ArxLayerHelper.h"
#include "Common/ArxTextStyleHelper.h"
#include "CAstar.h"
#include <algorithm>
static TerCurDrawing* pInstanse = NULL;

TerCurDrawing::TerCurDrawing()
{
	verticalLength = 50;
	fill = cut = 0;
	baseData = new CBaseData();
}





TerCurDrawing* TerCurDrawing::Instanse()
{
	if (pInstanse == NULL)
	{
		pInstanse = new TerCurDrawing();
	}
	return pInstanse;
}

TerCurDrawing::~TerCurDrawing(void)
{
	if (!mapCutResult.empty())
	{
		for (std::map<CString, std::vector<TerCurResultModel*>>::iterator it = mapCutResult.begin(); it != mapCutResult.end(); ++it)
		{
			std::vector<TerCurResultModel*>  vec_pt = (*it).second;
			CadCommon::SafeClaerVector(vec_pt);
		}
		mapCutResult.clear();
		mapCutResult.swap(std::map<CString, std::vector<TerCurResultModel*>>());
	}
	delete baseData;
}

//设置阶段数据
void TerCurDrawing::SetStageValue(Edge edge)
{
	std::vector<AcGePoint3d>* polyPoints = edge.result;
	startPoint = (*polyPoints)[edge.result->size() - 1];
	endPoint = (*polyPoints)[0];
	startName = BIM::Tools::NtoWstring(edge.u).c_str();
	endName = BIM::Tools::NtoWstring(edge.v).c_str();

}

void TerCurDrawing::ClearDGX()
{
	if (vecCurrntLines.size() > 0 )
	{
		for (int i = 0; i < vecCurrntLines.size(); i++) {
			vecCurrntLines[i]->close();
		}
		vecCurrntLines.clear();
	}

}

//获取风机矩形内所有等高线
void TerCurDrawing::readDGX()
{
	int extand = 160;   //    160

	ads_point pt1, pt2;
	pt1[X] = startPoint.x;
	pt1[Y] = startPoint.y;
	pt2[X] = endPoint.x;
	pt2[Y] = endPoint.y;

	//  向外扩展一定的范围
	if (pt1[X] < pt2[X] && pt1[Y] < pt2[Y]) {
		pt1[X] -= extand;
		pt1[Y] -= extand;
		pt2[X] += extand;
		pt2[Y] += extand;
	}			  // 选择集扩展范围
	else if (pt1[X] < pt2[X] && pt1[Y] > pt2[Y]) {
		pt1[X] -= extand;
		pt1[Y] += extand;
		pt2[X] += extand;
		pt2[Y] -= extand;
	}
	else if (pt1[X] > pt2[X] && pt1[Y] > pt2[Y]) {
		pt1[X] += extand;
		pt1[Y] += extand;
		pt2[X] -= extand;
		pt2[Y] -= extand;
	}
	else if (pt1[X] > pt2[X] && pt1[Y] < pt2[Y]) {
		pt1[X] += extand;
		pt1[Y] -= extand;
		pt2[X] -= extand;
		pt2[Y] += extand;
	}


	ads_name ssName;
	if (acedSSGet(_T("C"), pt1, pt2, NULL, ssName) == RTNORM) {
		Adesk::Int32 length;
		acedSSLength(ssName, &length);
		for (int index = 0; index < length; ++index) {
			ads_name entName;
			if (RTNORM == acedSSName(ssName, index, entName)) {  // 从选择集中获取实体名称到entname
				AcDbObjectId mp_Ref_Id;
				if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entName)) {  // 根据实体名称获取实体id
					AcDbEntity* pEnt = NULL;
					if (Acad::eOk == acdbOpenAcDbEntity(pEnt, mp_Ref_Id, AcDb::kForRead)) {  // 获取实体对象
						ACHAR* lName = pEnt->layer();
						std::wstring lNameStr = lName;
						BIM::Tools::lowerWstring(lNameStr);
						
						if (lNameStr.find(L"首曲线") != std::wstring::npos) {
							if (pEnt->isA() == AcDbPolyline::desc()) {  
								CContourModel* tempContour = new CContourModel(pEnt);
								baseData->contours.push_back(tempContour);  // 一次只插入一条多线段
								vecCurrntLines.push_back(pEnt);
							}
						}
						pEnt->close();
					}
				}
			}
		}
	}
	acedSSFree(ssName); // 释放选择集
	//baseData.DataInfo();
}

//道路与等高线交点
void TerCurDrawing::CalcAllIntersectPoint(AcGePoint3dArray points,CString text)
{
	bool bHasAnyJD = false;
	
	//间隔点设置
	int intrevalPoint = 20;
	int intervalUnit = std::floor((float)intrevalPoint / DataManager::Instanse()->GetDepartLength());
	
	//循环points,以间隔点为中点，画垂线
	
	AcDbLine* pKDLine;//垂线
	AcGePoint3dArray intersetPoints;//相交点
	
	std::vector<TerCurResultModel*> vecCutResult;
	for (int pointIndex = 0; pointIndex < points.length(); pointIndex++)
	{
		TerCurResultModel* resultModel;
		AcGeVector3d vectorVectical;//垂线单位向量
		AcGePoint3d upPoint;//根据垂线向上取点
		AcGePoint3d downPoint;//根据垂线向下取点
		
		AcGePoint3d point = points.at(pointIndex);
		if (pointIndex == 0)
		{
		     vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex + 1), 1).asVector().normalize();//线段间垂直单位向量
		}
		else if (pointIndex == points.length() - 1)
		{
			vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex - 1), 1).asVector().normalize();//线段间垂直单位向量
		}
		else {
			if (pointIndex % intervalUnit == 0)
			{
				vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex - 1), 1).asVector().normalize();//线段间垂直单位向量
			}
		}
		if (vectorVectical.length() > 1e-6)
		{
			resultModel = new TerCurResultModel();//剖切面相关数据
			resultModel->currentPoint = point;
			upPoint = point + verticalLength * vectorVectical;
			downPoint = point - verticalLength * vectorVectical;
			bool bFirst = true;
			
			for (int dgxIndex = 0; dgxIndex < baseData->contours.size(); dgxIndex++)
			{
				//AcDbEntity* pLine = vecCurrntLines.at(dgxIndex);
				AcDbPolyline* pLine = baseData->contours.at(dgxIndex)->pline;
				upPoint.z = baseData->contours.at(dgxIndex)->altitude;
				downPoint.z = baseData->contours.at(dgxIndex)->altitude;
				point.z = baseData->contours.at(dgxIndex)->altitude;
			    pKDLine = new AcDbLine(upPoint, downPoint);//两点取直线
				intersetPoints.removeAll();
				if (Acad::eOk == pKDLine->intersectWith(pLine, AcDb::kOnBothOperands, intersetPoints))
				{
					int len = intersetPoints.length();
					for (int pointIndex = 0;pointIndex < len;pointIndex++)
					{
						double elev = baseData->contours.at(dgxIndex)->altitude;
						bHasAnyJD = true;
						if (bFirst) {
							resultModel->maxElv = elev;
							resultModel->minElv = elev;
							bFirst = false;
						}
						else 
						{
							if (elev - resultModel->maxElv > 1e-6)
							{
								resultModel->maxElv = elev;
							}
							else if(elev - resultModel->minElv < 1e-6)
							{
								resultModel->minElv = elev;
							}
						}
						AcGePoint3d _insPoint = intersetPoints.at(pointIndex);
						double _distance = _insPoint.distanceTo(point);

						#pragma region 判断向量是否为垂直，以此来判断相交点方向
						AcGeVector3d vec(1.0, 0.0, 0.0); // 假设垂直向量为 (0, 0, 1)
						double dotProduct = vec.dotProduct(vectorVectical);
						double angle = acos(dotProduct) * 180.0 / M_PI;

						if (fabs(angle - 90.0) <= 1.0) {
							if (_insPoint.y > point.y)
							{
								_distance = 0 - _distance;
							}
						}
						else {
							if (_insPoint.x < point.x)
							{
								_distance = 0 - _distance;
							}
						}
						#pragma endregion
						
						IntersectResultModel* intersectRsult = new IntersectResultModel();
						intersectRsult->SetValue(_distance, _insPoint);
						resultModel->vecResultModel.push_back(intersectRsult);
					}
				}
			}
			if (resultModel!= NULL)
			{
				std::sort(resultModel->vecResultModel.begin(), resultModel->vecResultModel.end(), CompareBorderXvalue);
				vecCutResult.push_back(resultModel);
			}
		} 
	}
	if (vecCutResult.size() > 0)
	{
		CString name = startName + L"-" + endName + text;
		mapCutResult.insert(std::make_pair(name, vecCutResult));
	}
	pKDLine->close();
	/*if (pKDLine->startPoint().x != 0)
	{
		delete ;
	}*/
	
	
}

/// <summary>
/// 绘制剖切图
/// </summary>
void TerCurDrawing::DrawingSectionalView()
{
	double singleFill = 0;
	double singleCut = 0;
	double temp = 0;
	BeginEdit();
	AcGePoint3d selPoint = DataManager::Instanse()->GetSelectPoint();

	double  dHorizontalSpacing = 400;
	double  dVerticalSpacing = 200;
	double offsetX = 23;
	double offsetY = 30;
	double offsetV1 = 3.5;
	double offsetV2 = 7;

	//创建图层
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId _frameLayerId = CadCommon::ArxLayerHelper::GetLayerId2(L"BIM-剖面外框", pDb);//剖面外框
	AcDbObjectId polyLinelayId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-剖切线", pDb);
	AcDbObjectId roadPolyLinelayId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-新建路", pDb);
	AcDbObjectId statisLayerId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-统计", pDb);

	AcDbObjectId _txtStyleId = CadCommon::ArxTextStyleHelper::CreateTextStyle(L"宋体", L"", 10, 0, 0, 0, Adesk::kFalse, Adesk::kFalse,
		Adesk::kFalse, Adesk::kFalse, Adesk::kFalse, Adesk::kFalse, L"宋体");

	int rowIndex = 0;
	for (std::map<CString, std::vector<TerCurResultModel*>>::iterator it = mapCutResult.begin(); it != mapCutResult.end(); ++it)
	{
		singleFill = 0;
		singleCut = 0;

		double vStartY = selPoint.y - rowIndex * dVerticalSpacing;
		CString name = it->first;
		std::vector<TerCurResultModel*> loadCutResult = it->second; //一条路的剖切图
		for (int colIndex = 0; colIndex < loadCutResult.size(); colIndex++)
		{
			TerCurResultModel* terCutResult = loadCutResult.at(colIndex);//一条垂线
			
			double vStarytX = colIndex * dHorizontalSpacing + selPoint.x;
			double inStartX = vStarytX + offsetX; //边框原点X
			double inStartY = vStartY + offsetY;//边框原点Y

			double maxElv = terCutResult->maxElv;
			double minElv = terCutResult->minElv;

			double verLength = maxElv - minElv + offsetV1 + offsetV2; //竖轴长度
			double horLength = verticalLength * 2;//横轴长度
			double basVerPoint = inStartY + offsetV1;//外框零点

			//绘制外框，设置为黄色
			AcGePoint2d point1, point2, point3, point4;
			point1.set(inStartX, inStartY);
			point2.set(inStartX, inStartY + verLength);
			point3.set(inStartX + horLength, inStartY + verLength);
			point4.set(inStartX + horLength, inStartY);

			AcDbPolyline* pPolyLine = new AcDbPolyline(4);
			pPolyLine->addVertexAt(0, point1, 0, 0, 0);
			pPolyLine->addVertexAt(1, point2, 0, 0, 0);
			pPolyLine->addVertexAt(2, point3, 0, 0, 0);
			pPolyLine->addVertexAt(3, point4, 0, 0, 0);
			pPolyLine->setClosed(Adesk::kTrue);
			pPolyLine->setColorIndex(2);
			pPolyLine->setLayer(_frameLayerId);
			AcDbObjectId _entityFrameId = CadCommon::ArxHelper::PostToModelSpace(pPolyLine);

			//绘制竖轴
			int verSpace = 2;
			int iLength = verLength / verSpace; //除数根据实际情况修改
			double __minElv = minElv - offsetV1;
			int ___minElv = int(__minElv + 0.5);
			double xxxOffset = ___minElv - __minElv;
			AcGePoint3d __kdbasePoint(inStartX, inStartY + xxxOffset, 0.0); //坐标轴零点

			for (int index = 0;index < iLength; ++index)
			{
				AcGePoint3d pointStart(__kdbasePoint.x, __kdbasePoint.y + verSpace * index, 0.0);
				AcGePoint3d pointEnd(__kdbasePoint.x - 1, __kdbasePoint.y + verSpace * index, 0.0);
				int vvv = ___minElv + index * verSpace;
				std::wstring _vvvstr = BIM::Tools::IntToWstring(vvv);
				AcGePoint3d txtPoint(__kdbasePoint.x - 4, __kdbasePoint.y + verSpace * index - 0.3, 0.0);
				AcDbText* pXKDText = new AcDbText(txtPoint, _vvvstr.c_str());
				pXKDText->setTextStyle(_txtStyleId);
				pXKDText->setLayer(_frameLayerId);
				pXKDText->setHeight(0.5);
				CadCommon::ArxHelper::PostToModelSpace(pXKDText);
				AcDbLine* pKDLine = new AcDbLine(pointStart, pointEnd);
				pKDLine->setLayer(_frameLayerId);
				pKDLine->setColorIndex(2);
				CadCommon::ArxHelper::PostToModelSpace(pKDLine);
			}
			//绘制横轴
			int horSpace = 5;
			int iXLength = horLength / horSpace;
			for (int index = 0;index < iXLength; ++index)
			{
				AcGePoint3d pointStart(inStartX + index * horSpace, inStartY, 0.0);
				AcGePoint3d pointEnd(inStartX + index * horSpace, inStartY - 0.75, 0.0);
				
				pointEnd.y -= 0.25;
				int vvv = index * horSpace;
				std::wstring _vvvstr = BIM::Tools::IntToWstring(vvv);
				AcGePoint3d txtPoint(inStartX + index * horSpace, inStartY - 3, 0.0);
				AcDbText* pXKDText = new AcDbText(txtPoint, _vvvstr.c_str());
				pXKDText->setTextStyle(_txtStyleId);
				pXKDText->setLayer(_frameLayerId);
				pXKDText->setHeight(0.5);
				CadCommon::Cable_Box textNameBox;
				CadCommon::ArxHelper::GetEntityBox(pXKDText, textNameBox);
				double fontWidth = textNameBox.max.x - textNameBox.min.x;
				txtPoint.x = txtPoint.x - (fontWidth / 2);
				pXKDText->setPosition(txtPoint);
				CadCommon::ArxHelper::PostToModelSpace(pXKDText);
				
				AcDbLine* pKDLine = new AcDbLine(pointStart, pointEnd);
				pKDLine->setLayer(_frameLayerId);
				pKDLine->setColorIndex(2);
				CadCommon::ArxHelper::PostToModelSpace(pKDLine);
			}

			//绘制剖切线
			AcGePoint3dArray ptArr;
			ptArr.setLogicalLength(terCutResult->vecResultModel.size());
			for (int intersectIndex = 0; intersectIndex < terCutResult->vecResultModel.size(); intersectIndex++)
			{
				IntersectResultModel* model = terCutResult->vecResultModel.at(intersectIndex);

				double _y = model->m_height - minElv + basVerPoint;
				double _x = verticalLength + model->m_distance + inStartX;
				ptArr[intersectIndex].set(_x, _y, 0.0);

			}
			AcDb2dPolyline* pPolyFormLine = new AcDb2dPolyline(AcDb::k2dSimplePoly, ptArr);
			pPolyFormLine->setLayer(polyLinelayId);
			pPolyFormLine->setColorIndex(6);
			CadCommon::ArxHelper::PostToModelSpace(pPolyFormLine);

			//绘制道路
			AcDbLine* pKDLine;//辅助线
			AcGePoint3dArray ptRoadline;//路
			AcGePoint3d leftRoadPoint;
			AcGePoint3d rightRoadPoint;
			
			/*double roadWidth = DataManager::Instanse()->GetRoadWidth() / 2;
			double roadAngle = DataManager::Instanse()->GetRoadAngle();*/
			double roadWidth = 2.75;
			double roadAngle = 45;

			AcGePoint3d midPoint = CadCommon::ArxGeoMathHelper::GetMultiPolyLineByX(ptArr, inStartX + verticalLength);//路中心点
			AcGeVector3d vec(1.0, 0.0, 0.0);
			AcGePoint3d leftPoint = midPoint - vec * roadWidth;//路左点
			AcGePoint3d rightPoint = midPoint + vec * roadWidth;//路右点

			AcGePoint3d tempPoint = CadCommon::ArxGeoMathHelper::GetMultiPolyLineByX(ptArr, inStartX + verticalLength - roadWidth);
			if (tempPoint.y - leftPoint.y > 0.001)
			{
				leftRoadPoint = GetRoadPoint(180 - roadAngle, leftPoint, pPolyFormLine);
				if (leftRoadPoint.x != 0)
				{
					temp = CalcArea(leftRoadPoint, leftPoint, midPoint, _txtStyleId, statisLayerId, L"挖方", ptArr) * 20;
					cut += temp;
					singleCut += temp;
				}
			}
			else {
				leftRoadPoint = GetRoadPoint(roadAngle - 180, leftPoint, pPolyFormLine);
				if (leftRoadPoint.x != 0)
				{
					temp = CalcArea(leftRoadPoint, leftPoint, midPoint, _txtStyleId, statisLayerId, L"填方", ptArr) * 20;
					fill += temp;
					singleFill += temp;
				}
			}

		    tempPoint = CadCommon::ArxGeoMathHelper::GetMultiPolyLineByX(ptArr, inStartX + verticalLength + roadWidth);
			if (tempPoint.y - rightPoint.y > 0.001)
			{
				rightRoadPoint = GetRoadPoint(roadAngle, rightPoint, pPolyFormLine);
				if (rightRoadPoint.x != 0)
				{
					temp = CalcArea(midPoint, rightPoint, rightRoadPoint, _txtStyleId, statisLayerId, L"挖方", ptArr) * 20;
					cut += temp;
					singleCut += temp;
				}
			}
			else {
				rightRoadPoint = GetRoadPoint(0 -  roadAngle, rightPoint, pPolyFormLine);
				if (rightRoadPoint.x != 0)
				{
					temp = CalcArea(midPoint, rightPoint, rightRoadPoint, _txtStyleId, statisLayerId, L"填方", ptArr) * 20;
					fill += temp;
					singleFill += temp;
				}
			}
			
			if (leftRoadPoint.x != 0) ptRoadline.append(leftRoadPoint);
			ptRoadline.append(leftPoint);
			ptRoadline.append(midPoint);
			ptRoadline.append(rightPoint);
			if (rightRoadPoint.x != 0) ptRoadline.append(rightRoadPoint);
			AcDb2dPolyline* pRoadPolyFormLine = new AcDb2dPolyline(AcDb::k2dSimplePoly, ptRoadline);
			pRoadPolyFormLine->setLayer(roadPolyLinelayId);
			pRoadPolyFormLine->setColorIndex(12);
			CadCommon::ArxHelper::PostToModelSpace(pRoadPolyFormLine);


			AcGePoint3d displayPoint(inStartX + horLength / 2, inStartY - 20, 0);
			AcDbText* pEndIndexText = new AcDbText(displayPoint, (L"代表点位" + BIM::Tools::NtoWstring(terCutResult->currentPoint.x) + L"," + BIM::Tools::NtoWstring(terCutResult->currentPoint.y)).c_str(), _txtStyleId);
			pEndIndexText->setLayer(statisLayerId);
			pEndIndexText->setHeight(2);
			pEndIndexText->setColorIndex(4);
			CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);
		}

		AcGePoint3d statisPoint(selPoint.x - 50, vStartY + 50, 0);
		AcDbText* pEndIndexText = new AcDbText(statisPoint, (L"挖方" + BIM::Tools::NtoWstring(singleCut)).c_str(), _txtStyleId);
		pEndIndexText->setLayer(statisLayerId);
		pEndIndexText->setHeight(2);
		pEndIndexText->setColorIndex(4);
		CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);


		statisPoint.set(selPoint.x - 50, vStartY + 60, 0);
		pEndIndexText = new AcDbText(statisPoint, (L"填方"+ BIM::Tools::NtoWstring(singleFill)).c_str(), _txtStyleId);
		pEndIndexText->setLayer(statisLayerId);
		pEndIndexText->setHeight(2);
		pEndIndexText->setColorIndex(4);
		CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);

		statisPoint.set(selPoint.x - 50, vStartY + 70, 0);
		pEndIndexText = new AcDbText(statisPoint, name, _txtStyleId);
		pEndIndexText->setLayer(statisLayerId);
		pEndIndexText->setHeight(2);
		pEndIndexText->setColorIndex(4);
		CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);

		rowIndex++;
	}
	AcGePoint3d statisPoint(selPoint.x - 100, selPoint.y + 100, 0);
	AcDbText* pEndIndexText = new AcDbText(statisPoint, (L"总挖方" + BIM::Tools::NtoWstring(cut)).c_str(), _txtStyleId);
	pEndIndexText->setLayer(statisLayerId);
	pEndIndexText->setHeight(2);
	pEndIndexText->setColorIndex(4);
	CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);

	statisPoint.set(selPoint.x - 100, selPoint.y + 90, 0);
	pEndIndexText = new AcDbText(statisPoint, (L"总填方" + BIM::Tools::NtoWstring(fill)).c_str(), _txtStyleId);
	pEndIndexText->setLayer(statisLayerId);
	pEndIndexText->setHeight(2);
	pEndIndexText->setColorIndex(4);
	CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);
	
	EndEdit();
}

double TerCurDrawing::CalcArea(AcGePoint3d point1, AcGePoint3d point2, AcGePoint3d point3, AcDbObjectId textLayerId, AcDbObjectId _frameLayerId, std::wstring text, AcGePoint3dArray array)
{
	AcDbPolyline* pPolyline = new AcDbPolyline();
	AcGePoint2d point1_2d(point1.x, point1.y);
	AcGePoint2d point2_2d(point2.x, point2.y);
	AcGePoint2d point3_2d(point3.x, point3.y);
	AcGePoint2d temp2d;

	pPolyline->addVertexAt(0, point1_2d);
	pPolyline->addVertexAt(1, point2_2d);
	pPolyline->addVertexAt(2, point3_2d);

	int number = 3;
	for (int index = 0; index < array.length(); index++)
	{
		AcGePoint3d temp = array.at(index);
		if (temp.x > point1.x && temp.x < point3.x)
		{
			temp2d.set(temp.x, temp.y);
			pPolyline->addVertexAt(number, temp2d);
			number++;
		}
	}

	pPolyline->addVertexAt(number, point1_2d); // 闭合区域，添加第一个顶点以闭合多段线

	double area = 0.0;
	pPolyline->getArea(area);

	delete pPolyline;


	std::wstring areaStr = BIM::Tools::NtoWstring(area);
	AcGePoint3d txtPoint(point2.x, point2.y - 3, 0.0);
	AcDbText* pXKDText = new AcDbText(txtPoint, areaStr.c_str());
	pXKDText->setTextStyle(textLayerId);
	pXKDText->setLayer(_frameLayerId);
	pXKDText->setHeight(0.5);
	CadCommon::Cable_Box textNameBox;
	CadCommon::ArxHelper::GetEntityBox(pXKDText, textNameBox);
	double fontWidth = textNameBox.max.x - textNameBox.min.x;
	txtPoint.x = txtPoint.x - (fontWidth / 2);
	pXKDText->setPosition(txtPoint);
	CadCommon::ArxHelper::PostToModelSpace(pXKDText);

	return area;
}

AcGePoint3d TerCurDrawing::GetRoadPoint(double angle,AcGePoint3d point, AcDb2dPolyline* pPolyFormLine)
{
	AcDbLine* pKDLine;//辅助线
	AcGePoint3dArray intersetPoints;//相交点
	AcGePoint3d returnPoint;
	double angleRad = (angle) * M_PI / 180.0;
	//double angleRad = angle;
	// 计算 x 和 y 分量
	double x = cos(angleRad);
	double y = sin(angleRad);
	// 构建单位向量
	AcGeVector3d vec(x, y, 0);
	AcGePoint3d tempPoint = point + vec * 10;

	pKDLine = new AcDbLine(point, tempPoint);
	if (Acad::eOk == pKDLine->intersectWith(pPolyFormLine, AcDb::kOnBothOperands, intersetPoints))
	{
		if (intersetPoints.length() > 0)
		{
			returnPoint = intersetPoints.at(0);
		}
	}
	return returnPoint;
}
