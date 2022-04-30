#pragma once
#include <JuceHeader.h>
#include "PitchClass.h"
#include "Hash.h"
#include "Pitch.h"

class Pitch;
class PitchClass;

class PitchClassTile : 
	public juce::Component
{
public:
	PitchClassTile(const PitchClass& pitchClass);
	void paint(juce::Graphics& g) override;
	void updatePitchIntensities(const std::map<Pitch, float>&);
	void timerUpdate();
private:
	PitchClass pitchClass;
	float tolerance;
	std::map<Pitch, float> pitchIntensities;
	bool needsRepaint;
	juce::Colour pitchColor(Pitch, float);
};
