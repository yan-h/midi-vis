#include <cmath>

#include "PitchClass.h"
#include "Pitch.h"

PitchClass::PitchClass(const Pitch& pitch) 
{
	float x = std::fmod(pitch.getMidiPitch(), 12);
	if (x < 0) x += 12;
	midiPitchClass = std::abs(x);
}

PitchClass::PitchClass(const PitchClass& pitchClass)
{
	midiPitchClass = pitchClass.midiPitchClass;
}

bool PitchClass::operator==(const PitchClass& pitchClass) const
{
	return std::abs(this->midiPitchClass - pitchClass.midiPitchClass) < Pitch::epsilon;
}

bool PitchClass::matchesPitch(const Pitch& pitch) const
{
	return matchesPitch(pitch, 0);
}


bool PitchClass::matchesPitch(const Pitch& pitch, float tolerance) const
{
	return std::abs(PitchClass(pitch).midiPitchClass - midiPitchClass) <= std::fmax(Pitch::epsilon, tolerance);
}

float PitchClass::getCents() const
{
	return midiPitchClass * 100;
}