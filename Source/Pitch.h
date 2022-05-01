#pragma once

#include <cstdlib>
#include <JuceHeader.h>

class PitchClass;

class Pitch
{
public:
	Pitch(float);
	Pitch(const Pitch&);
	static Pitch fromFreqHz(float);
	bool operator==(const Pitch&) const;
	bool operator<(const Pitch&) const;
	float getMidiPitch() const;

	// Two Pitches are equal if their difference is below this value
	// 1/1000 cent
	static constexpr float epsilon = 0.00001; 
private: 
	float midiPitch; 
};

struct PitchHash {
public:
	size_t operator()(const Pitch& x) const
	{
		return std::hash<float>()((int)x.getMidiPitch());
	}
};