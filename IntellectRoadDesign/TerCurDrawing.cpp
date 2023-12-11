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

//���ý׶�����
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

//��ȡ������������еȸ���
void TerCurDrawing::readDGX()
{
	int extand = 160;   //    160

	ads_point pt1, pt2;
	pt1[X] = startPoint.x;
	pt1[Y] = startPoint.y;
	pt2[X] = endPoint.x;
	pt2[Y] = endPoint.y;

	//  ������չһ���ķ�Χ
	if (pt1[X] < pt2[X] && pt1[Y] < pt2[Y]) {
		pt1[X] -= extand;
		pt1[Y] -= extand;
		pt2[X] += extand;
		pt2[Y] += extand;
	}			  // ѡ����չ��Χ
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
			if (RTNORM == acedSSName(ssName, index, entName)) {  // ��ѡ���л�ȡʵ�����Ƶ�entname
				AcDbObjectId mp_Ref_Id;
				if (Acad::eOk == acdbGetObjectId(mp_Ref_Id, entName)) {  // ����ʵ�����ƻ�ȡʵ��id
					AcDbEntity* pEnt = NULL;
					if (Acad::eOk == acdbOpenAcDbEntity(pEnt, mp_Ref_Id, AcDb::kForRead)) {  // ��ȡʵ�����
						ACHAR* lName = pEnt->layer();
						std::wstring lNameStr = lName;
						BIM::Tools::lowerWstring(lNameStr);
						
						if (lNameStr.find(L"������") != std::wstring::npos) {
							if (pEnt->isA() == AcDbPolyline::desc()) {  
								CContourModel* tempContour = new CContourModel(pEnt);
								baseData->contours.push_back(tempContour);  // һ��ֻ����һ�����߶�
								vecCurrntLines.push_back(pEnt);
							}
						}
						pEnt->close();
					}
				}
			}
		}
	}
	acedSSFree(ssName); // �ͷ�ѡ��
	//baseData.DataInfo();
}

//��·��ȸ��߽���
void TerCurDrawing::CalcAllIntersectPoint(AcGePoint3dArray points,CString text)
{
	bool bHasAnyJD = false;
	
	//���������
	int intrevalPoint = 20;
	int intervalUnit = std::floor((float)intrevalPoint / DataManager::Instanse()->GetDepartLength());
	
	//ѭ��points,�Լ����Ϊ�е㣬������
	
	AcDbLine* pKDLine;//����
	AcGePoint3dArray intersetPoints;//�ཻ��
	
	std::vector<TerCurResultModel*> vecCutResult;
	for (int pointIndex = 0; pointIndex < points.length(); pointIndex++)
	{
		TerCurResultModel* resultModel;
		AcGeVector3d vectorVectical;//���ߵ�λ����
		AcGePoint3d upPoint;//���ݴ�������ȡ��
		AcGePoint3d downPoint;//���ݴ�������ȡ��
		
		AcGePoint3d point = points.at(pointIndex);
		if (pointIndex == 0)
		{
		     vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex + 1), 1).asVector().normalize();//�߶μ䴹ֱ��λ����
		}
		else if (pointIndex == points.length() - 1)
		{
			vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex - 1), 1).asVector().normalize();//�߶μ䴹ֱ��λ����
		}
		else {
			if (pointIndex % intervalUnit == 0)
			{
				vectorVectical = CadCommon::ArxGeoMathHelper::GetUnitVerticalVector(point, points.at(pointIndex - 1), 1).asVector().normalize();//�߶μ䴹ֱ��λ����
			}
		}
		if (vectorVectical.length() > 1e-6)
		{
			resultModel = new TerCurResultModel();//�������������
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
			    pKDLine = new AcDbLine(upPoint, downPoint);//����ȡֱ��
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

						#pragma region �ж������Ƿ�Ϊ��ֱ���Դ����ж��ཻ�㷽��
						AcGeVector3d vec(1.0, 0.0, 0.0); // ���费ֱ����Ϊ (0, 0, 1)
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
/// ��������ͼ
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

	//����ͼ��
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbObjectId _frameLayerId = CadCommon::ArxLayerHelper::GetLayerId2(L"BIM-�������", pDb);//�������
	AcDbObjectId polyLinelayId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-������", pDb);
	AcDbObjectId roadPolyLinelayId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-�½�·", pDb);
	AcDbObjectId statisLayerId = CadCommon::ArxLayerHelper::GetLayerId2(L"DX-ͳ��", pDb);

	AcDbObjectId _txtStyleId = CadCommon::ArxTextStyleHelper::CreateTextStyle(L"����", L"", 10, 0, 0, 0, Adesk::kFalse, Adesk::kFalse,
		Adesk::kFalse, Adesk::kFalse, Adesk::kFalse, Adesk::kFalse, L"����");

	int rowIndex = 0;
	for (std::map<CString, std::vector<TerCurResultModel*>>::iterator it = mapCutResult.begin(); it != mapCutResult.end(); ++it)
	{
		singleFill = 0;
		singleCut = 0;

		double vStartY = selPoint.y - rowIndex * dVerticalSpacing;
		CString name = it->first;
		std::vector<TerCurResultModel*> loadCutResult = it->second; //һ��·������ͼ
		for (int colIndex = 0; colIndex < loadCutResult.size(); colIndex++)
		{
			TerCurResultModel* terCutResult = loadCutResult.at(colIndex);//һ������
			
			double vStarytX = colIndex * dHorizontalSpacing + selPoint.x;
			double inStartX = vStarytX + offsetX; //�߿�ԭ��X
			double inStartY = vStartY + offsetY;//�߿�ԭ��Y

			double maxElv = terCutResult->maxElv;
			double minElv = terCutResult->minElv;

			double verLength = maxElv - minElv + offsetV1 + offsetV2; //���᳤��
			double horLength = verticalLength * 2;//���᳤��
			double basVerPoint = inStartY + offsetV1;//������

			//�����������Ϊ��ɫ
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

			//��������
			int verSpace = 2;
			int iLength = verLength / verSpace; //��������ʵ������޸�
			double __minElv = minElv - offsetV1;
			int ___minElv = int(__minElv + 0.5);
			double xxxOffset = ___minElv - __minElv;
			AcGePoint3d __kdbasePoint(inStartX, inStartY + xxxOffset, 0.0); //���������

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
			//���ƺ���
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

			//����������
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

			//���Ƶ�·
			AcDbLine* pKDLine;//������
			AcGePoint3dArray ptRoadline;//·
			AcGePoint3d leftRoadPoint;
			AcGePoint3d rightRoadPoint;
			
			/*double roadWidth = DataManager::Instanse()->GetRoadWidth() / 2;
			double roadAngle = DataManager::Instanse()->GetRoadAngle();*/
			double roadWidth = 2.75;
			double roadAngle = 45;

			AcGePoint3d midPoint = CadCommon::ArxGeoMathHelper::GetMultiPolyLineByX(ptArr, inStartX + verticalLength);//·���ĵ�
			AcGeVector3d vec(1.0, 0.0, 0.0);
			AcGePoint3d leftPoint = midPoint - vec * roadWidth;//·���
			AcGePoint3d rightPoint = midPoint + vec * roadWidth;//·�ҵ�

			AcGePoint3d tempPoint = CadCommon::ArxGeoMathHelper::GetMultiPolyLineByX(ptArr, inStartX + verticalLength - roadWidth);
			if (tempPoint.y - leftPoint.y > 0.001)
			{
				leftRoadPoint = GetRoadPoint(180 - roadAngle, leftPoint, pPolyFormLine);
				if (leftRoadPoint.x != 0)
				{
					temp = CalcArea(leftRoadPoint, leftPoint, midPoint, _txtStyleId, statisLayerId, L"�ڷ�", ptArr) * 20;
					cut += temp;
					singleCut += temp;
				}
			}
			else {
				leftRoadPoint = GetRoadPoint(roadAngle - 180, leftPoint, pPolyFormLine);
				if (leftRoadPoint.x != 0)
				{
					temp = CalcArea(leftRoadPoint, leftPoint, midPoint, _txtStyleId, statisLayerId, L"�", ptArr) * 20;
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
					temp = CalcArea(midPoint, rightPoint, rightRoadPoint, _txtStyleId, statisLayerId, L"�ڷ�", ptArr) * 20;
					cut += temp;
					singleCut += temp;
				}
			}
			else {
				rightRoadPoint = GetRoadPoint(0 -  roadAngle, rightPoint, pPolyFormLine);
				if (rightRoadPoint.x != 0)
				{
					temp = CalcArea(midPoint, rightPoint, rightRoadPoint, _txtStyleId, statisLayerId, L"�", ptArr) * 20;
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
			AcDbText* pEndIndexText = new AcDbText(displayPoint, (L"�����λ" + BIM::Tools::NtoWstring(terCutResult->currentPoint.x) + L"," + BIM::Tools::NtoWstring(terCutResult->currentPoint.y)).c_str(), _txtStyleId);
			pEndIndexText->setLayer(statisLayerId);
			pEndIndexText->setHeight(2);
			pEndIndexText->setColorIndex(4);
			CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);
		}

		AcGePoint3d statisPoint(selPoint.x - 50, vStartY + 50, 0);
		AcDbText* pEndIndexText = new AcDbText(statisPoint, (L"�ڷ�" + BIM::Tools::NtoWstring(singleCut)).c_str(), _txtStyleId);
		pEndIndexText->setLayer(statisLayerId);
		pEndIndexText->setHeight(2);
		pEndIndexText->setColorIndex(4);
		CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);


		statisPoint.set(selPoint.x - 50, vStartY + 60, 0);
		pEndIndexText = new AcDbText(statisPoint, (L"�"+ BIM::Tools::NtoWstring(singleFill)).c_str(), _txtStyleId);
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
	AcDbText* pEndIndexText = new AcDbText(statisPoint, (L"���ڷ�" + BIM::Tools::NtoWstring(cut)).c_str(), _txtStyleId);
	pEndIndexText->setLayer(statisLayerId);
	pEndIndexText->setHeight(2);
	pEndIndexText->setColorIndex(4);
	CadCommon::ArxHelper::PostToModelSpace(pEndIndexText);

	statisPoint.set(selPoint.x - 100, selPoint.y + 90, 0);
	pEndIndexText = new AcDbText(statisPoint, (L"���" + BIM::Tools::NtoWstring(fill)).c_str(), _txtStyleId);
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

	pPolyline->addVertexAt(number, point1_2d); // �պ�������ӵ�һ�������Ապ϶����

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
	AcDbLine* pKDLine;//������
	AcGePoint3dArray intersetPoints;//�ཻ��
	AcGePoint3d returnPoint;
	double angleRad = (angle) * M_PI / 180.0;
	//double angleRad = angle;
	// ���� x �� y ����
	double x = cos(angleRad);
	double y = sin(angleRad);
	// ������λ����
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
