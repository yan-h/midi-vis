#include "TuningInfo.h"
#include <cmath>
#pragma once

TuningInfo::TuningInfo(
	double semisFactor3,
	bool factor3Generates5, int factor3To5, double semisFactor5,
	bool factor3Generates7, int factor3To7, double semisFactor7,
	bool factor3Generates11, int factor3To11, double semisFactor11) :
	semisFactor3(semisFactor3), 
	factor3Generates5(factor3Generates5), factor3To5(factor3To5), semisFactor5(semisFactor5),
	factor3Generates7(factor3Generates7), factor3To7(factor3To7), semisFactor7(semisFactor7),
	factor3Generates11(factor3Generates11), factor3To11(factor3To11), semisFactor11(semisFactor11)
{}

double TuningInfo::getSemisFactor3()
{
	return semisFactor3;
}

double TuningInfo::getSemisFactor5()
{
	if (factor3Generates5)
	{
		return std::fmod((double)factor3To5 * semisFactor3, 12.0);
	}
	return semisFactor5;
}

double TuningInfo::getSemisFactor7()
{
	if (factor3Generates5)
	{
		return std::fmod((double)factor3To7 * semisFactor3, 12.0);
	}
	return semisFactor5;
}

double TuningInfo::getSemisFactor11()
{
	if (factor3Generates5)
	{
		return std::fmod((double)factor3To11 * semisFactor3, 12.0);
	}
	return semisFactor5;
}

int TuningInfo::getFactor3To5()
{
	return factor3To5;
}

int TuningInfo::getFactor3To7()
{
	return factor3To7;
}

int TuningInfo::getFactor3To11()
{
	return factor3To11;
}

bool TuningInfo::getFactor3Generates5()
{
	return factor3Generates5;
}

bool TuningInfo::getFactor3Generates7()
{
	return factor3Generates7;
}

bool TuningInfo::getFactor3Generates11()
{
	return factor3Generates11;
}