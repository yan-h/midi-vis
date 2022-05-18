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
	int borderSize = 3;

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

	// background color
	g.fillAll(juce::Colour(
		0.f,
		0.f,
		std::fmin(0.15f, std::powf(noteIntensity * 6, 1.5f)),
		1.f));

	// held notes are a brighter color
	if (noteIntensity > 0.9) {
		g.setColour(juce::Colour(0.f, 0.f, 0.15f + (noteIntensity - 0.9) * 3.5f, 1.f));
		g.fillRect(juce::Rectangle<int>(bounds.getWidth(), bounds.getHeight()));
	}

	// top note overlay
	g.fillAll(juce::Colour(0.6f, 1.f, 1.f, std::powf((float)topIntensity, 1) * 0.5f));
	// bottom note overlay
	g.fillAll(juce::Colour(0.0f, 1.f, 1.f, std::powf((float)bassIntensity, 1) * 0.5f));

	if (noteIntensity > 0.9) {
		g.setColour(juce::Colour(0.f, 0.f, 1.f, ((float)noteIntensity - 0.9f) * 10.f));
		g.fillRect(juce::Rectangle<int>(bounds.getWidth(), borderSize));
		g.fillRect(juce::Rectangle<int>(0, borderSize, borderSize, bounds.getHeight() - borderSize * 2));
		g.fillRect(juce::Rectangle<int>(bounds.getWidth() - borderSize, borderSize, borderSize, bounds.getHeight() - borderSize * 2));
		g.fillRect(juce::Rectangle<int>(0, bounds.getHeight() - borderSize, bounds.getWidth(), borderSize));
	}

	/*
	bool isMin = false;
	bool isMax = false;
	double yMin = borderSize;
	double yMax = bounds.getHeight() - borderSize;
	double yRange = yMax - yMin;
	for (int i = 0; i < heldPitches.size(); i++)
	{
		if (pitchClass.matchesPitch(Pitch(60)))
		{
			std::stringstream s;
			s << i << " " << heldPitches[i].getMidiPitch();
			DBG(s.str());// i + " " + maxIntensityPitches[i].getMidiPitch());
		}
		Pitch pitch = heldPitches[i];
		if (pitchClass.matchesPitch(pitch))
		{
			DBG(pitch.getMidiPitch());
			double startFrac = 1.0 * i / heldPitches.size();
			double endFrac = (i + 1.0) / heldPitches.size();
			int f = yRange * ((i + 1.0) / heldPitches.size());

			g.setColour(juce::Colour(0.f, 0.f, 0.55f, 1.f));
			//g.setColour(pitchColor(pitch, 1));
			g.fillRect(juce::Rectangle<float>(borderSize, yMax - f, bounds.getWidth(), yRange / heldPitches.size()));
			if (i == 0) isMin = true;
			if (i == heldPitches.size() - 1) isMax = true;
		}
	}*/



	/*
	// Draw pitches
	for (const std::pair<Pitch, double> pair : pitchIntensities)
	{
		Pitch pitch = pair.first;
		double intensity = pair.second;
		if (intensity < 1.0) continue;
		double heightProportion = (pitch.getMidiPitch() - lowerBound.getMidiPitch()) / (upperBound.getMidiPitch() - lowerBound.getMidiPitch());
		double y = (1 - heightProportion) * bounds.getHeight();

		if (localPitches.find(pitch) != localPitches.end()) 
		{
			g.setColour(pitchColor(pitch, intensity));
			g.fillRect(juce::Rectangle<int>(0, y - 5, bounds.getWidth(), 10));
		}
	}
	*/

	g.setColour(getLookAndFeel().findColour(juce::TextEditor::textColourId));
	// Note name text
	g.setFont(28);
	g.drawText(pitchName, 
		juce::Rectangle<int>(0, 0, bounds.getWidth(), std::round(bounds.getHeight() * 0.55)), 
		juce::Justification::centredBottom, 
		false);

	// Semitones text
	g.setFont(20);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << (double)pitchClass.getCents() / 100.f;
	g.drawText(stream.str(), 
		juce::Rectangle<int>(0, std::round(bounds.getHeight() * 0.55), bounds.getWidth(), std::round(bounds.getHeight() * 0.45)),
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