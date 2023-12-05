#pragma once
#include "RoadCal.h"
#include "CAPoint.h"
#include <vector>

struct ThreadParams {
	RoadCal* roadCopy;
	int i;
	int j;
	double maxSlope;
};


// ����ģʽ
class IntellectRoadDesignCmd
{
public:
	static IntellectRoadDesignCmd* getInstace();
	Result GetRoadData(int departLength, double maxSlope);
private:
	IntellectRoadDesignCmd(void);
	~IntellectRoadDesignCmd(void);
public:
	const int numThreads;  // ��������߳���	
};
