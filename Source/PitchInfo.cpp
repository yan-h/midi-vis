#include "PitchInfo.h"
#pragma once

PitchInfo::PitchInfo() : noteIntensity(0.0), topIntensity(0.0), bassIntensity(0.0) {}

PitchInfo::PitchInfo(double noteIntensity, double topIntensity, double bassIntensity) :
	topIntensity(topIntensity), bassIntensity(bassIntensity), noteIntensity(noteIntensity) {}