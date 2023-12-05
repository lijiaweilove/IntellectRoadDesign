#include "StdAfx.h"
#include "CAstar.h"
#include <algorithm>
#include <cmath>

double calculateAngle(const CAPoint& A, const CAPoint& B, const CAPoint& C) {
	// Calculate vectors AB and BC
	double vectorAB_x = B.m_x - A.m_x;
	double vectorAB_y = B.m_y - A.m_y;
	double vectorBC_x = C.m_x - B.m_x;
	double vectorBC_y = C.m_y - B.m_y;

	double dotProduct = vectorAB_x * vectorBC_x + vectorAB_y * vectorBC_y;
	double magnitudeAB = std::sqrt(vectorAB_x * vectorAB_x + vectorAB_y * vectorAB_y);
	double magnitudeBC = std::sqrt(vectorBC_x * vectorBC_x + vectorBC_y * vectorBC_y);
	double angleInRadians = std::atan2(vectorAB_x * vectorBC_y - vectorAB_y * vectorBC_x, dotProduct);
	double angleInDegrees = angleInRadians * (180.0 / M_PI);

	return angleInDegrees;
}


bool mySort(const CAPoint* p1, const CAPoint* p2)
{
	return p1->m_F < p2->m_F;
}


CAstar::CAstar(int rowNum, int colNum, double maxSlope)
{
	_rowNum = rowNum;
	_colNum = colNum;
	_startPoint = NULL;
	_endPoint = NULL;
	_curPoint = NULL;
	_maxSlope = maxSlope;
}


CAstar::~CAstar(void)
{
	_openList.clear();
	_closeList.clear();
	_neighbourList.clear();
	_allPoints.clear();
	delete _startPoint;
	delete _endPoint;
	//delete _curPoint;
	for (size_t i = 0; i < _allPoints.size(); ++i) {
		for (size_t j = 0; j < _allPoints[i].size(); ++j) {
			delete _allPoints[i][j];
		}
	}
	_allPoints.clear();
}


//CAPoint* CAstar::findWay(vector<vector<CAPoint*>>* allPoints)
//{
//	if (*_startPoint == *_endPoint){
//		cout << "��ʼ����ͬ" << endl;
//		return NULL;
//	}
//
//	_allPoints = *allPoints;
//	_openList.push_back(_startPoint);
//	_startPoint->m_type = ATYPE_OPENED;
//	_startPoint->m_F = getF(_startPoint);
//
//	do {
//		//��ȡ��Сֵ�Ľڵ�
//		_curPoint = _openList[0];
//		_openList.erase(_openList.begin());
//		_curPoint->m_type = ATYPE_CLOSED;
//		_closeList.push_back(_curPoint);
//
//		if (*_curPoint == *_endPoint) {
//			acutPrintf(TEXT("have find way"));
//			_curPoint->m_x = _endPoint->m_x;
//			_curPoint->m_y = _endPoint->m_y;
//			return _curPoint;
//		}
//
//		//��ȡ���ڵĽڵ�
//		vector<CAPoint*> neVec = getNeighboringPoint(_curPoint);
//		for (int i = 0; i < neVec.size(); i++) {
//			CAPoint* tmpoint = neVec[i];
//			if (tmpoint->m_type == AType::ATYPE_CLOSED) {
//				continue;
//			}
//			//�ж��Ƿ��ڿ����б���
//			if (tmpoint->m_type != AType::ATYPE_OPENED) {
//				//��ӵ������б�
//				_openList.push_back(tmpoint);
//				tmpoint->m_type = ATYPE_OPENED;
//				//���ø��ڵ�,��ǽڵ��ѷ���
//				tmpoint->m_parent = _curPoint;
//				//����G��Hֵ
//				if (tmpoint->isRoad) {
//					tmpoint->m_F = 0;
//				}
//				else {
//					tmpoint->m_G = getG(_curPoint, tmpoint);
//					tmpoint->m_H = getH(tmpoint);
//					tmpoint->m_F = getF(tmpoint);
//				}
//			}
//			else {
//				//�Ѿ��ڿ����б���
//				if (/*tmpoint->m_G != 10000.0 &&*/ tmpoint->m_G > getG(_curPoint, tmpoint)) {
//					tmpoint->m_parent = _curPoint;
//					tmpoint->m_G = getG(_curPoint, tmpoint);
//					tmpoint->m_F = getF(tmpoint);
//				}
//			}
//		}
//		sort(_openList.begin(), _openList.end(), mySort);
//
//	} while (_openList.size() > 0);
//
//	acutPrintf(TEXT("---can not find way---"));
//	return NULL;
//}


CAPoint* CAstar::findWay(vector<vector<CAPoint*>>* allPoints)
{
	if (*_startPoint == *_endPoint){
		cout << "��ʼ����ͬ" << endl;
		return NULL;
	}

	_allPoints = *allPoints;
	_openList.push_back(_startPoint);
	_startPoint->m_type = ATYPE_OPENED;
	_startPoint->m_F = getF(_startPoint);
	BevelEdge = sqrt(2.0 * departLength * departLength);
	double maxAngle = 91.;
	int count = 0;

	do{
		_curPoint = NULL;

		//  ���ƽǶ�
		/*for (int i = 0; i < _openList.size(); i++) {
			double angle = 0.;
			if (_openList[i]->m_parent != NULL && _openList[i]->m_parent->m_parent != NULL)
				angle = calculateAngle(*_openList[i]->m_parent->m_parent, *_openList[i]->m_parent, *_openList[i]);
			if(angle < maxAngle){
				_curPoint = _openList[i];
				_openList.erase(_openList.begin() + i);
				break;
			}
		}*/

		//  �¶�5���㶼û�䣬ǿ�б�  
		//if(!_openList[0]->isRoad && _openList[0]->m_parent)
		//	if (_openList[0]->m_parent->m_elevation == _openList[0]->m_elevation)
		//		count++;
		//if (count > 5) {
		//	for(int i = 0; i < _openList.size(); i++){
		//		if (abs(_openList[i]->m_elevation - _openList[i]->m_parent->m_elevation) > 3) {
		//		/*if (_openList[i]->m_elevation != _openList[i]->m_parent->m_elevation) {*/   //  ***�޸�
		//			count = 0;
		//			_curPoint = _openList[i];
		//			_openList.erase(_openList.begin() + i);
		//			break;
		//		}
		//	}
		//}


		if(_curPoint == NULL){
			_curPoint = _openList[0];
			_openList.erase(_openList.begin());
		}
		_curPoint->m_type = ATYPE_CLOSED;
		_closeList.push_back(_curPoint);

		if (*_curPoint == *_endPoint){
			acutPrintf(TEXT("have find way"));
			_curPoint->m_x = _endPoint->m_x;
			_curPoint->m_y = _endPoint->m_y;
			return _curPoint;
		}

		//��ȡ���ڵĽڵ�
		vector<CAPoint*> neVec = getNeighboringPoint(_curPoint);
		for (int i = 0; i < neVec.size(); i++){
			double angle = 0.;
			CAPoint* tmpoint = neVec[i];
			if (tmpoint->m_type == AType::ATYPE_CLOSED){
				continue;
			}
			//�ж��Ƿ��ڿ����б���
			if (tmpoint->m_type != AType::ATYPE_OPENED){
				//��ӵ������б�
				tmpoint->m_type = ATYPE_OPENED;
				tmpoint->m_parent = _curPoint;
				//����G��Hֵ
				if (tmpoint->isRoad){
					tmpoint->m_F = 0;
				}
				else {
					tmpoint->m_G = getG(_curPoint, tmpoint);
					if (tmpoint->m_G == 10000.0) {  // �޷�ͨ��,���õ�ǰ�ڵ�
						tmpoint->m_type = ATYPE_UNKNOWN;
						tmpoint->m_parent = NULL;
						tmpoint->m_G = 0.;
						continue;
					}
					tmpoint->m_H = getH(tmpoint);
					tmpoint->m_F = getF(tmpoint);
				}
				_openList.push_back(tmpoint);
			}
			else{
				//�Ѿ��ڿ����б���,�ҵ�ǰ·��Gֵ��С
				double tempG = getG(_curPoint, tmpoint);
				if (tempG != 10000.0 && tmpoint->m_G > tempG){
					tmpoint->m_parent = _curPoint;
					tmpoint->m_G = tempG;
					tmpoint->m_F = getF(tmpoint);
				}
			}
		}

		sort(_openList.begin(), _openList.end(), mySort);

	} while (_openList.size() > 0);

	acutPrintf(TEXT("---can not find way---"));
	return NULL;
}


double CAstar::getF(CAPoint* point)
{
	return point->m_G + getH(point);
}

//double CAstar::getH(CAPoint* point)
//{
//	//�����ٳ��н������㷨
//	return sqrt((double)((_endPoint->m_Y - point->m_Y) * (_endPoint->m_Y - point->m_Y) * departLength * departLength
//		+ (_endPoint->m_X - point->m_X) * (_endPoint->m_X - point->m_X) * departLength * departLength));
//		//+ 0 * (_endPoint->m_elevation - point->m_elevation) * (_endPoint->m_elevation - point->m_elevation)));
//}


double CAstar::getH(CAPoint* point)
{
	return abs(_endPoint->m_Y - point->m_Y) + abs(_endPoint->m_X - point->m_X);
}


double CAstar::getG(CAPoint* p1, CAPoint* p2)
{
	double slope = 0.;
	double xyDis = (p1->m_X != p2->m_X && p1->m_Y != p2->m_Y) ? BevelEdge : departLength;
	slope = atan(abs(p1->m_elevation - p2->m_elevation) / xyDis) * (180.0 / M_PI);
	
	if (slope > _maxSlope)
		return 10000.0;
	return p1->m_G + slope * 10;
}

double CAstar::getElevation(CAPoint* point)
{
	//   ��ǰ����������⣬�����¸߳���ȣ�����Ҳ���ʱ������0
	//    ���޸ģ��ж������£������ĸ����������ĸ���ƽ��ֵ
	int pRow = point->m_X;
	int pCol = point->m_Y;
	double leftValue = 0.;
	double rightValue = 0.;
	double topValue = 0.;
	double downValue = 0.;
	double result = 0.;
	int num1 = 1, num2 = 1, count = 0;
	if (pCol - num1 <= 0 || pCol + num2 >= _colNum - 1 || pCol < num1 || pCol > _colNum - 1 - num2){
		return 0.;
	}

	while (_allPoints[pRow][pCol - num1]->m_elevation == 0){
		if (pCol - num1 <= 0) break;
		num1++;
	}
	leftValue = _allPoints[pRow][pCol - num1]->m_elevation;

	while (_allPoints[pRow][pCol + num2]->m_elevation == 0){
		if (pCol + num2 >= _colNum - 1) break;
		num2++;
	}
	rightValue = _allPoints[pRow][pCol + num2]->m_elevation;

	/*if (leftValue != 0 && rightValue != 0 && leftValue != rightValue)
	{
		return rightValue - ((rightValue - leftValue) * num2 / (num1 + num2));
	}*/

	if (leftValue != 0 && rightValue != 0){
		result = rightValue - ((rightValue - leftValue) * num2 / (num1 + num2));
	}
	count = max(num1, num2);

	num1 = 1;
	num2 = 1;
	if (pRow + num1 >= _rowNum - 1 || pRow - num2 <= 0){
		return 0.;
	}
	while (_allPoints[pRow + num1][pCol]->m_elevation == 0){
		if (pRow + num1 >= _rowNum - 1) break;
		num1++;
	}
	topValue = _allPoints[pRow + num1][pCol]->m_elevation;

	while (_allPoints[pRow - num2][pCol]->m_elevation == 0){
		if (pRow - num2 <= 0) break;
		num2++;
	}
	downValue = _allPoints[pRow - num2][pCol]->m_elevation;

	//if ((topValue != 0) && (downValue != 0) && (topValue != downValue)){
	//	return downValue - ((downValue - topValue) * num2 / (num1 + num2));
	//}

	if (max(num1, num2) > count) {
		if (topValue != 0 && downValue != 0)
			return downValue - ((downValue - topValue) * num2 / (num1 + num2));
		else return 0.;
	}

	return result;

}

std::vector<CAPoint*> CAstar::getNeighboringPoint(CAPoint* point)
{
	_neighbourList.clear();
	int pRow = point->m_X;
	int pCol = point->m_Y;
	if (pCol < _colNum - 1)
	{
		//�����ұߵĵ�
		if (_allPoints[pRow][pCol + 1]->m_type != AType::ATYPE_BARRIER)  // �����ϰ����Ҳ�����
		{
			if (_allPoints[pRow][pCol + 1]->m_slopeLevel == 0)  // û�и߳���Ϣ
			{
				_allPoints[pRow][pCol + 1]->m_elevation = getElevation(_allPoints[pRow][pCol + 1]);
			}
			_neighbourList.push_back(_allPoints[pRow][pCol + 1]);
		}
		//�����ϱߵĵ�����Ͻǵĵ�
		if (pRow < _rowNum - 1)
		{
			if (_allPoints[pRow + 1][pCol]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow + 1][pCol]->m_slopeLevel == 0)
				{
					_allPoints[pRow + 1][pCol]->m_elevation = getElevation(_allPoints[pRow + 1][pCol]);
				}
				_neighbourList.push_back(_allPoints[pRow + 1][pCol]);
			}
			if (_allPoints[pRow + 1][pCol + 1]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow + 1][pCol + 1]->m_slopeLevel == 0)
				{
					_allPoints[pRow + 1][pCol + 1]->m_elevation = getElevation(_allPoints[pRow + 1][pCol + 1]);
				}
				_neighbourList.push_back(_allPoints[pRow + 1][pCol + 1]);
			}
		}
		//�����±ߵĵ�����½ǵĵ�
		if (pRow > 0)
		{
			if (_allPoints[pRow - 1][pCol]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow - 1][pCol]->m_slopeLevel == 0)
				{
					_allPoints[pRow - 1][pCol]->m_elevation = getElevation(_allPoints[pRow - 1][pCol]);
				}
				_neighbourList.push_back(_allPoints[pRow - 1][pCol]);
			}
			if (_allPoints[pRow - 1][pCol + 1]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow - 1][pCol + 1]->m_slopeLevel == 0)
				{
					_allPoints[pRow - 1][pCol + 1]->m_elevation = getElevation(_allPoints[pRow - 1][pCol + 1]);
				}
				_neighbourList.push_back(_allPoints[pRow - 1][pCol + 1]);
			}
		}

	}
	if (pCol > 0)
	{
		//������ߵĵ�
		if (_allPoints[pRow][pCol - 1]->m_type != AType::ATYPE_BARRIER)
		{
			if (_allPoints[pRow][pCol - 1]->m_slopeLevel == 0)
			{
				_allPoints[pRow][pCol - 1]->m_elevation = getElevation(_allPoints[pRow][pCol - 1]);
			}
			_neighbourList.push_back(_allPoints[pRow][pCol - 1]);
		}
		//�������Ͻǵĵ�
		if (pRow < _rowNum - 1)
		{
			if (_allPoints[pRow + 1][pCol - 1]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow + 1][pCol - 1]->m_slopeLevel == 0)
				{
					_allPoints[pRow + 1][pCol - 1]->m_elevation = getElevation(_allPoints[pRow + 1][pCol - 1]);
				}
				_neighbourList.push_back(_allPoints[pRow + 1][pCol - 1]);
			}
		}
		//�������½ǵĵ�
		if (pRow > 0)
		{
			if (_allPoints[pRow - 1][pCol - 1]->m_type != AType::ATYPE_BARRIER)
			{
				if (_allPoints[pRow - 1][pCol - 1]->m_slopeLevel == 0)
				{
					_allPoints[pRow - 1][pCol - 1]->m_elevation = getElevation(_allPoints[pRow - 1][pCol - 1]);
				}
				_neighbourList.push_back(_allPoints[pRow - 1][pCol - 1]);
			}
		}
	}

	return _neighbourList;
}
