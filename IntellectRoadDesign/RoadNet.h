#pragma once

#include<vector>
using std::vector;


struct Edge
{
	int u, v;
	double weight;  // 总权值
	double midWeight;  // 中间权值(不包括起点和终点的路段距离)
	double endDis, startDis;
	int endPointNum, startPointNum;
	double roadDis, mountainDis;
	vector<AcGePoint3d>* result;
	Edge(int u, int v) : u(u), v(v), weight(0.), midWeight(0.), endDis(0.), startDis(0.), endPointNum(0),
		startPointNum(0), roadDis(0.), mountainDis(0.){
		result = new vector<AcGePoint3d>;
	};
	Edge(const Edge& edge) : u(edge.u), v(edge.v), weight(edge.weight), midWeight(edge.midWeight), 
		endDis(edge.endDis), startDis(edge.startDis), endPointNum(edge.endPointNum), 
		startPointNum(edge.startPointNum), roadDis(edge.roadDis), mountainDis(edge.mountainDis){
		result = edge.result;
	};
};

class RoadNet
{
public:
	void kruskal(int n, vector<Edge*>& edge, vector<Edge>& result);
	void createPolyLine(AcDbPolyline* pPolyline);
	void createSpline(AcDbSpline* pSpline);
	static AcDbObjectIdArray GetAllEntIds(const ACHAR* layer = NULL, AcDbDatabase* pDb =
		acdbHostApplicationServices()->workingDatabase());
private:
	int findfather(int a, vector<int> father);
};

