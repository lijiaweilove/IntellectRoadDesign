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

private:
	int departLength;
	double maxSlope;
};

