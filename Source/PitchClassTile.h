#pragma once
#include <JuceHeader.h>
#include "PitchClass.h"
#include "Hash.h"
#include "Pitch.h"
#include "PitchInfo.h"

class Pitch;
class PitchClass;

class PitchClassTile : 
	public juce::Component
{
public:
	PitchClassTile(int, int, double, double);
	void setTuning(double, double, double);
	void paint(juce::Graphics& g) override;
	void updatePitchIntensities(const std::map<Pitch, PitchInfo>&);
	void timerUpdate();
private:
	PitchClass pitchClass;
	double tolerance;
	std::map<Pitch, PitchInfo> pitchInfos;
	bool needsRepaint;
	juce::Colour pitchColor(Pitch, double);
	juce::String pitchName;
	int factor3;
	int factor5;
};

