#define _USE_MATH_DEFINES
#include <math.h>
#include <format>
#include "PitchClassTile.h"
#include "Pitch.h"
#include "PitchClass.h"
#include "Hash.h"

namespace {
const std::vector<juce::String> letterNames = { "F", "C", "G", "D", "A", "E", "B" };

const juce::String flatSign = "b";
const juce::String sharpSign = "#";

const juce::String syntonicCommaUpSign = "+";
const juce::String syntonicCommaDownSign = "-";

const float spacingOffset = 0.05f;
const int accidentalsFontSize = 20;

#if JUCE_MAC
const int noteNameFontSize = 28;
const int semitonesFontSize = 16;
#else
const int noteNameFontSize = 32;
const int semitonesFontSize = 20;
#endif
}

PitchClassTile::PitchClassTile(int factor3Base, int factor5Base, double semisFactor3, double semisFactor5, double tolerance) :
	pitchClass(0),
	factor3Base(factor3Base),
	factor5Base(factor5Base)
{
	setTuning(0, 0, semisFactor3, semisFactor5, tolerance);
}

void PitchClassTile::setTuning(int factor3Offset, int factor5Offset, double semisFactor3, double semisFactor5, double tolerance)
{
	this->factor3Offset = factor3Offset;
	this->factor5Offset = factor5Offset;

	int factor3 = factor3Base + factor3Offset;
	int factor5 = factor5Base + factor5Offset;

	this->tolerance = tolerance;

	pitchClass = PitchClass(Pitch(semisFactor3 * factor3 + semisFactor5 * factor5));
	needsRepaint = true;

	// plus one because we start at C, not F
	int numFifths = 1 + factor3 + 4 * factor5;
	int letterNameIndex = numFifths % 7;
	if (letterNameIndex < 0) letterNameIndex += 7;
	int semiOffset = numFifths / 7;
	pitchName = letterNames[letterNameIndex];

	accidentals = "";
	if (semiOffset > 0) {
		if (semiOffset >= 1) accidentals += sharpSign;
		if (semiOffset == 2) accidentals += sharpSign;
		else if (semiOffset > 2) accidentals += semiOffset;
	}
	else if (numFifths < 0)
	{
		if (semiOffset <= 0) accidentals += flatSign;
		if (semiOffset == -1) accidentals += flatSign;
		else if (semiOffset < -1) accidentals += std::abs(semiOffset) + 1;
	}

	syntonicCommas = "";
	if (fabs(fmod(semisFactor3 * 4, 12.0) - semisFactor5) > 0.000001)
	{
		int syntonicCommaOffset = -factor5;

		if (syntonicCommaOffset > 0) {
			if (syntonicCommaOffset >= 1) syntonicCommas += syntonicCommaUpSign;
			if (syntonicCommaOffset == 2) syntonicCommas += syntonicCommaUpSign;
			else if (syntonicCommaOffset > 2) syntonicCommas += syntonicCommaOffset;
		}
		else if (syntonicCommaOffset < 0)
		{
			if (syntonicCommaOffset <= -1) syntonicCommas += syntonicCommaDownSign;
			if (syntonicCommaOffset == -2) syntonicCommas += syntonicCommaDownSign;
			else if (syntonicCommaOffset < -2) syntonicCommas += std::abs(syntonicCommaOffset);
		}
	}
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
	double width = bounds.getWidth();
	double height = bounds.getHeight();
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

	int ringOffset1 = borderSize + (width * 0.07);
	juce::Rectangle outerRectangle = juce::Rectangle<int>(ringOffset1, ringOffset1, bounds.getWidth() - ringOffset1 * 2, bounds.getHeight() - ringOffset1 * 2);
	int ringOffset2 = borderSize + (width * 0.14);
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

	const int noteNameWidth = width * 0.5;
	const int noteNameHeight = height * 0.7;


	g.setColour(getLookAndFeel().findColour(juce::TextEditor::textColourId));
	// Note name text
	g.setFont(noteNameHeight * 0.75);
	g.drawText(pitchName,
		juce::Rectangle<int>(0, 0, noteNameWidth, noteNameHeight),
		juce::Justification::centredRight,
		false);

	g.setFont(noteNameHeight * 0.75 * 0.55);
	g.drawText(accidentals,
		juce::Rectangle<int>(noteNameWidth + 2, noteNameHeight * 0.05, noteNameWidth - 2, noteNameHeight * 0.5),
		juce::Justification::bottomLeft,
		false);

	g.drawText(syntonicCommas,
		juce::Rectangle<int>(noteNameWidth + 2, noteNameHeight * 0.45, noteNameWidth - 2, noteNameHeight * 0.5),
		juce::Justification::topLeft,
		false);

	// Semitones text
	g.setFont(noteNameHeight * 0.4);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << (double)pitchClass.getCents() / 100.f;
	g.drawText(stream.str(), 
		juce::Rectangle<int>(0,  noteNameHeight - height * 0.1, width, height - noteNameHeight),
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
