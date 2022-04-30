#define _USE_MATH_DEFINES
#include <math.h>
#include "PitchClassTile.h"
#include "Pitch.h"
#include "PitchClass.h"
#include "Hash.h"

PitchClassTile::PitchClassTile(const PitchClass& pitchClass) : 
	pitchClass(pitchClass),
	tolerance(0.5),
	needsRepaint(false)
{
}

juce::Colour PitchClassTile::pitchColor(Pitch pitch, float intensity)
{
	float x = (pitch.getMidiPitch() - 60.f)/36.f;
	float sign = x >= 0 ? 1 : -1;
	x = 1 - std::powf((1.f - std::abs(x)), 1);
	x *= sign;
	return juce::Colour(0.15 + x * 0.15, 0.8f, 0.4 + x * 0.2, std::powf(intensity, 2));
}

void PitchClassTile::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bounds = g.getClipBounds();
	float radius = bounds.getWidth() / 2.f; // assume a square component
	float largeRadius = radius * 1.42; // slightly more than sqrt(2)
	float centerX = radius;
	float centerY = radius;

	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

	// We only want pitches that share the max intensity. All others are ignored
	std::vector<Pitch> maxIntensityPitches;
	float maxIntensity = 0;
	for (const std::pair<Pitch, float> pair : pitchIntensities)
	{
		Pitch pitch = pair.first;
		float intensity = pair.second;
		DBG(intensity);
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

	int numPitches = maxIntensityPitches.size();
	float angle = 0;
	float angleDiff = 2.f * M_PI / numPitches;
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
	
	g.setFont(20);
	g.drawText(juce::String(pitchClass.getCents()) + juce::String("c"), bounds, juce::Justification::centred, false);
}

void PitchClassTile::updatePitchIntensities(const std::map<Pitch, float>& pitchIntensities)
{
	std::map<Pitch, float> newPitchIntensities = std::map<Pitch, float>();

	for (const std::pair<Pitch, float> pair : pitchIntensities)
	{
		const Pitch& pitch = pair.first;
		float intensity = pair.second;
		if (pitchClass.containsPitch(pitch))
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

	if (newPitchIntensities.size() > 0 || pitchIntensities.size() > 0)
	{
		this->pitchIntensities = newPitchIntensities;
		needsRepaint = true;
	}
}

void PitchClassTile::timerUpdate()
{
	if (needsRepaint)
	{
		needsRepaint = false;
		repaint();
	}
}