#pragma once
#include "CAPoint.h"
#define M_PI 3.14159265358979323846

using namespace std;

class CAstar
{
public:
	CAstar(int rowNum, int colNum, double maxSlope);
	~CAstar(void);

	double departLength, BevelEdge, _maxSlope;
	std::vector<CAPoint*> _openList;      //�����б�
	std::vector<CAPoint*> _closeList;     //�ر��б�
	std::vector<CAPoint*> _neighbourList; //�ܱ߽ڵ�
	CAPoint* _startPoint;
	CAPoint* _endPoint;
	CAPoint* _curPoint;
	std::vector<std::vector<CAPoint*>> _allPoints;
	int _rowNum, _colNum;

	CAPoint* findWay(vector<vector<CAPoint*>>* allPoints);

private:
	double getF(CAPoint* point);
	double getH(CAPoint* point);
	double getG(CAPoint* p1, CAPoint* p2);
	double getElevation(CAPoint* point);
	vector<CAPoint*> getNeighboringPoint(CAPoint* point);
};
