#define _USE_MATH_DEFINES
#include <math.h>
#include <format>
#include "PitchClassTile.h"
#include "Pitch.h"
#include "PitchClass.h"
#include "Hash.h"

namespace {
const std::vector<juce::String> letterNames = { "F", "C", "G", "D", "A", "E", "B" };

#if JUCE_MAC
const juce::String flatSign = "b";
const juce::String sharpSign = "#";
const int noteNameFontSize = 28;
const int semitonesFontSize = 16;
const float spacingOffset = 0.05f;
#else
const juce::String flatSign = juce::CharPointer_UTF8("\xe2\x99\xad");
const juce::String sharpSign = juce::CharPointer_UTF8("\xe2\x99\xaf");
const int noteNameFontSize = 40;
const int semitonesFontSize = 20;
const float spacingOffset = 0.f;
#endif

const juce::String commaUpSign = "+";
const juce::String commaDownSign = "-";

}

PitchClassTile::PitchClassTile(int factor3, int factor5, double semisFactor3, double semisFactor5) :
	pitchClass(PitchClass(Pitch(factor3 * semisFactor3 + factor5 * semisFactor5))),
	tolerance(0.5),
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
		if (semiOffset >= 1) pitchName += sharpSign;
		if (semiOffset == 2) pitchName += sharpSign;
		else if (semiOffset > 2) pitchName += semiOffset;
	}
	else if (numFifths < 0) 
	{
		if (semiOffset <= 0) pitchName += flatSign;
		if (semiOffset == -1) pitchName += flatSign;
		else if (semiOffset < -1) pitchName += std::abs(semiOffset) + 1;
	}

	pitchClass = PitchClass(Pitch(semisFactor3 * factor3 + semisFactor5 * factor5));
}

void PitchClassTile::setTuning(double semisFactor3, double semisFactor5, double tolerance)
{
	this->tolerance = tolerance;
	pitchClass = PitchClass(Pitch(semisFactor3 * factor3 + semisFactor5 * factor5));
	needsRepaint = true;
}

juce::Colour PitchClassTile::pitchColor(Pitch pitch, double intensity)
{
	double x = std::powf((pitch.getMidiPitch() - 30.0) / 60.0, 1);
	return juce::Colour(
		0.05f + x * 0.2f, 
		0.9f, 
		0.8f,
		(float)intensity//std::powf(std::fmaxf(0.f, (intensity - 0.95f) * 20.f), 1.5f)
		);
}

void PitchClassTile::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bounds = g.getClipBounds();
	double radius = bounds.getWidth() / 2.f; // assume a square component
	double largeRadius = radius * 1.42f; // slightly more than sqrt(2)
	double centerX = radius;
	double centerY = radius;
	int borderSize = 1;

	std::unordered_set<Pitch, PitchHash> localPitches;
	std::vector<Pitch> heldPitches;
	double noteIntensity = 0.0; // max of all notes with this pitch class
	double topIntensity = 0.0; // max of all notes with this pitch class
	double bassIntensity = 0.0; // max of all notes with this pitch class

	for (const std::pair<Pitch, PitchInfo>& pair : pitchInfos)
	{
		Pitch pitch = pair.first;
		PitchInfo pitchInfo = pair.second;
		if (pitchClass.matchesPitch(pitch, tolerance))
		{
			localPitches.insert(pitch);
			noteIntensity = std::max(noteIntensity, pitchInfo.noteIntensity);
			topIntensity = std::max(topIntensity, pitchInfo.topIntensity);
			bassIntensity = std::max(bassIntensity, pitchInfo.bassIntensity);
		}
		if (pitchInfo.noteIntensity == 1.0)
		{
			heldPitches.push_back(pitch);
		}
	}
	std::sort(heldPitches.begin(), heldPitches.end());

	int numMaxIntensityPitches = heldPitches.size();

	float ghostBrightness = 0.22f;
	float borderBrightness = 0.7f;

	// background color
	g.fillAll(juce::Colour(
		0.6f,
		0.4f,
		std::fmin(ghostBrightness, std::powf(noteIntensity * 6, 1.5f)),
		1.f));

	// held notes are a brighter color
	if (noteIntensity > 0.9) {
		g.setColour(juce::Colour(0.6f, 0.5f, 0.5f, 10.f * ((float)noteIntensity - 0.9f)));
		g.fillRect(juce::Rectangle<int>(bounds.getWidth(), bounds.getHeight()));
	}

	int ringOffset1 = borderSize + 6;
	juce::Rectangle outerRectangle = juce::Rectangle<int>(ringOffset1, ringOffset1, bounds.getWidth() - ringOffset1 * 2, bounds.getHeight() - ringOffset1 * 2);
	int ringOffset2 = borderSize + 12;
	juce::Rectangle innerRectangle = juce::Rectangle<int>(ringOffset2, ringOffset2, bounds.getWidth() - ringOffset2 * 2, bounds.getHeight() - ringOffset2 * 2);

	// bottom note overlay
	juce::Path path;
	path.addRectangle(bounds);
	path.setUsingNonZeroWinding(false);
	path.addRectangle(outerRectangle);
	g.setColour(juce::Colour(0.6f, 0.5f, 0.9f, (float)bassIntensity));
	g.fillPath(path);

	// top note overlay
	juce::Path path2;
	path2.addRectangle(outerRectangle);
	path2.setUsingNonZeroWinding(false);
	path2.addRectangle(innerRectangle);
	g.setColour(juce::Colour(0.6f, 0.2f, 1.f, (float)topIntensity));
	g.fillPath(path2);

	// outline
	g.setColour(juce::Colour(0.6f, 0.5f, 0.9f, 1.f));
	g.fillRect(juce::Rectangle<int>(bounds.getWidth(), borderSize));
	g.fillRect(juce::Rectangle<int>(0, borderSize, borderSize, bounds.getHeight() - borderSize * 2));
	g.fillRect(juce::Rectangle<int>(bounds.getWidth() - borderSize, borderSize, borderSize, bounds.getHeight() - borderSize * 2));
	g.fillRect(juce::Rectangle<int>(0, bounds.getHeight() - borderSize, bounds.getWidth(), borderSize));

	g.setColour(getLookAndFeel().findColour(juce::TextEditor::textColourId));
	// Note name text
	g.setFont(noteNameFontSize);
	g.drawText(pitchName, 
		juce::Rectangle<int>(0, bounds.getHeight() * 0.1, bounds.getWidth(), std::round(bounds.getHeight() * (0.55 - spacingOffset))),
		juce::Justification::centredBottom, 
		false);

	// Semitones text
	g.setFont(semitonesFontSize);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << (double)pitchClass.getCents() / 100.f;
	g.drawText(stream.str(), 
		juce::Rectangle<int>(0, std::round(bounds.getHeight() * (0.55 + spacingOffset)), bounds.getWidth(), std::round(bounds.getHeight() * 0.45)),
		juce::Justification::centredTop, 
		false);
}

void PitchClassTile::updatePitchIntensities(const std::map<Pitch, PitchInfo>& allPitchInfos)
{
	pitchInfos = allPitchInfos;
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
