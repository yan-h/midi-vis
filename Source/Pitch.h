#pragma once

#include <cstdlib>
#include <JuceHeader.h>

class PitchClass;

class Pitch
{
public:
	Pitch(double);
	Pitch(const Pitch&);
	static Pitch fromFreqHz(double);
	bool operator==(const Pitch&) const;
	bool operator<(const Pitch&) const;
	double getMidiPitch() const;

	// Two Pitches are equal if their difference is below this value
	// 1/1000 cent
	static constexpr double epsilon = 0.00001; 
private: 
	double midiPitch; 
};

struct PitchHash {
public:
	size_t operator()(const Pitch& x) const
	{
		return std::hash<double>()((int)x.getMidiPitch());
	}
};