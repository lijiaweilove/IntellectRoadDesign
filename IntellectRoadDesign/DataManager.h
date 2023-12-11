#pragma once

class DataManager
{
public:
	DataManager(void);
	~DataManager(void);
public:
	static DataManager* Instanse();
	static void Terminal();

public:
	void SetDepartLength(int value);
	int GetDepartLength();

	void SetMaxSlope(double value);
	double GetMaxSlope();

	void SetRoadWidth(double value);
	double GetRoadWidth();

	void SetRoadAngle(double value);
	double GetRoadAngle();

	void SetSelectPoint(AcGePoint3d value);
	AcGePoint3d GetSelectPoint();

private:
	int departLength;
	double maxSlope;

	double roadWidth;
	double roadAngle;
	AcGePoint3d ptSelect;
};

