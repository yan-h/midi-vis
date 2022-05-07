#define _USE_MATH_DEFINES
#include <math.h>
#include <format>
#include "PitchClassTile.h"
#include "Pitch.h"
#include "PitchClass.h"
#include "Hash.h"

namespace {
	const std::vector<juce::String> letterNames = { "F", "C", "G", "D", "A", "E", "B" };
	const juce::String flatSign = juce::CharPointer_UTF8("\xe2\x99\xad");
	const juce::String sharpSign = juce::CharPointer_UTF8("\xe2\x99\xaf");
	const juce::String commaUpSign = "+";
	const juce::String commaDownSign = "-";
}

PitchClassTile::PitchClassTile(int factor3, int factor5, float semisFactor3, float semisFactor5) :
	pitchClass(PitchClass(Pitch(factor3 * semisFactor3 + factor5 * semisFactor5))),
	tolerance(0.5f),
	needsRepaint(false),
	factor3(factor3),
	factor5(factor5)
{
	// plus one because we start at C, not F
	int numFifths = 1 + factor3 + 4 * factor5;
	int letterNameIndex = numFifths % 7;
	if (letterNameIndex < 0) letterNameIndex += 7;
	int semiOffset = numFifths / 7;
	pitchName = letterNames[letterNameIndex];
	if (semiOffset > 0) {
		for (int i = 0; i < semiOffset; i++)
		{
			pitchName += sharpSign;
		}
	}
	else if (numFifths < 0) {
		for (int i = 0; i < std::abs(semiOffset) + 1; i++)
		{
			pitchName += flatSign;
		}
	}

	pitchClass = PitchClass(Pitch(semisFactor3 * factor3 + semisFactor5 * factor5));
}

void PitchClassTile::setTuning(float semisFactor3, float semisFactor5, float tolerance)
{
	this->tolerance = tolerance;
	DBG(semisFactor3);
	pitchClass = PitchClass(Pitch(semisFactor3 * factor3 + semisFactor5 * factor5));
	needsRepaint = true;
}

juce::Colour PitchClassTile::pitchColor(Pitch pitch, float intensity)
{
	float x = std::powf((pitch.getMidiPitch() - 36.f) / 48.f, 1);
	return juce::Colour(
		0.05f + x * 0.2f, 
		0.9f, 
		0.5f, 
		std::powf(std::fmaxf(0.f, (intensity - 0.95f) * 20.f), 1.5)
		);
}

void PitchClassTile::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bounds = g.getClipBounds();
	float radius = bounds.getWidth() / 2.f; // assume a square component
	float largeRadius = radius * 1.42f; // slightly more than sqrt(2)
	float centerX = radius;
	float centerY = radius;

	// We only want pitches that share the max intensity. All others are ignored
	std::vector<Pitch> maxIntensityPitches;
	float maxIntensity = 0;
	for (const std::pair<Pitch, float> pair : pitchIntensities)
	{
		Pitch pitch = pair.first;
		float intensity = pair.second;
		if (intensity > maxIntensity)
		{
			maxIntensity = intensity;
			maxIntensityPitches = std::vector<Pitch>();
		}
		if (intensity == maxIntensity)
		{
			maxIntensityPitches.push_back(pitch);
		}
	}

	// background color
	g.fillAll(juce::Colour(
		0.f, 
		0.f, 
		std::fmin(0.3f, std::powf(maxIntensity * 6, 1.5f)), 
		1.f));

	size_t numPitches = maxIntensityPitches.size();
	float angle = 0;
	float angleDiff = 2.f * M_PI / (float)numPitches;
	for (Pitch pitch : maxIntensityPitches)
	{
		juce::Path path;
		path.startNewSubPath(centerX, centerY);
		path.addPieSegment(radius - largeRadius, radius - largeRadius, 2 * largeRadius, 2 * largeRadius, angle, angle + angleDiff, 0);
		path.closeSubPath();

		juce::FillType fillType;
		fillType.setColour(pitchColor(pitch, maxIntensity));
		g.setFillType(fillType);
		g.fillPath(path);

		angle += angleDiff;
	}

	int borderSize = 2;

	g.setColour(getLookAndFeel().findColour(juce::TextEditor::textColourId));
	g.fillRect(juce::Rectangle<int>(bounds.getWidth(), borderSize));
	g.fillRect(juce::Rectangle<int>(borderSize, bounds.getHeight()));
	g.fillRect(juce::Rectangle<int>(bounds.getWidth() - borderSize, 0, borderSize, bounds.getHeight()));
	g.fillRect(juce::Rectangle<int>(0, bounds.getHeight() - borderSize, bounds.getWidth(), borderSize));
	
	g.setFont(28);
	g.drawText(pitchName, 
		juce::Rectangle<int>(0, 0, bounds.getWidth(), std::round(bounds.getHeight() * 0.55)), 
		juce::Justification::centredBottom, 
		false);

	g.setFont(20);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << (float)pitchClass.getCents() / 100.f;
	g.drawText(stream.str(), 
		juce::Rectangle<int>(0, std::round(bounds.getHeight() * 0.55), bounds.getWidth(), std::round(bounds.getHeight() * 0.45)),
		juce::Justification::centredTop, 
		false);
}

void PitchClassTile::updatePitchIntensities(const std::map<Pitch, float>& allPitchIntensities)
{
	std::map<Pitch, float> newPitchIntensities = std::map<Pitch, float>();

	for (const std::pair<Pitch, float> pair : allPitchIntensities)
	{
		const Pitch& pitch = pair.first;
		float intensity = pair.second;
		if (pitchClass.matchesPitch(pitch, tolerance))
		{
			if (newPitchIntensities.find(pitch) != newPitchIntensities.end())
			{
				newPitchIntensities[intensity] = std::max(intensity, newPitchIntensities[intensity]);
			} 
			else 
			{
				newPitchIntensities.insert(pair);
			}
		}
	}

	this->pitchIntensities = newPitchIntensities;
	needsRepaint = true;
}

void PitchClassTile::timerUpdate()
{
	if (needsRepaint)
	{
		needsRepaint = false;
		repaint();
	}
}