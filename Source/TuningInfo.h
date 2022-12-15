#pragma once
class TuningInfo
{
private:
	double semisFactor3;
	double semisFactor5;
	double semisFactor7;
	double semisFactor11;

	int factor3To5;
	int factor3To7;
	int factor3To11;

	bool factor3Generates5;
	bool factor3Generates7;
	bool factor3Generates11;

public:
	TuningInfo(double semisFactor3,
		bool factor3Generates5, int factor3To5, double semisFactor5,
		bool factor3Generates7, int factor3To7, double semisFactor7,
		bool factor3Generates11, int factor3To11, double semisFactor11);

	double getSemisFactor3();
	double getSemisFactor5();
	double getSemisFactor7();
	double getSemisFactor11();

	int getFactor3To5();
	int getFactor3To7();
	int getFactor3To11();

	bool getFactor3Generates5();
	bool getFactor3Generates7();
	bool getFactor3Generates11();
};
