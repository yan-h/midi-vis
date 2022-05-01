
#include <cstdlib>
#include <JuceHeader.h>

#include "Pitch.h"
#include "PitchClass.h"

Pitch::Pitch(float midiPitch)
{
	this->midiPitch = midiPitch;
}

Pitch Pitch::fromFreqHz(float freqHz)
{
	return Pitch(69.f + (log2f(freqHz) - log2f(440)) * 12.f);
}

Pitch::Pitch(const Pitch& pitch)
{
	this->midiPitch = pitch.midiPitch;
}

bool Pitch::operator==(const Pitch& pitch) const 
{
	return std::abs(this->midiPitch - pitch.midiPitch) < Pitch::epsilon;
}


bool Pitch::operator<(const Pitch& pitch) const
{
	return midiPitch < pitch.midiPitch;
}

float Pitch::getMidiPitch() const 
{
	return this->midiPitch;
}