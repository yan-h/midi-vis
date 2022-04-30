/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LogMessage.h"
#include "PitchClass.h"
#include "Pitch.h"
#include "PitchClassTile.h"
#include "Hash.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p, juce::MPEInstrument& mpeInstrument):
    AudioProcessorEditor (&p), 
    audioProcessor (p), 
    mpeInstrument(mpeInstrument)
{
    mpeInstrument.addListener(this);

    //addAndMakeVisible(logBox);
    logBox.setMultiLine(true);
    logBox.setReturnKeyStartsNewLine(true);
    logBox.setReadOnly(true);
    logBox.setScrollbarsShown(true);
    logBox.setCaretVisible(false);
    logBox.setPopupMenuEnabled(true);
    logBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    logBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    logBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));
    logBox.moveCaretToEnd();
    logBox.insertTextAtCaret("23523w45u324985by73298gb52793y5bvg6392");
    logBox.moveCaretToEnd();
    logBox.insertTextAtCaret("2");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(1600, 900);

    int size = 80;

    for (int x = 0; x <= 10; x++)
    {
        for (int y = 0; y <= 10; y++)
        {
            int xPos = 10 + x * size;
            int yPos = 10 + y * size;
            int factor3 = -(y - 5);
            int factor5 = x - 5;
            PitchClassTile* newTile = new PitchClassTile(PitchClass(Pitch(0 + factor3 * 7 + factor5 * 4)));
            newTile->setBounds(xPos, yPos, size, size);
            tiles.push_back(std::unique_ptr<PitchClassTile>(newTile));
            addAndMakeVisible(*newTile);
        }
    }

    startTimerHz(60);
}

PluginEditor::~PluginEditor()
{
    mpeInstrument.removeListener(this);
}

void PluginEditor::timerCallback()
{
    updateTiles();
    for (const std::unique_ptr<PitchClassTile>& pitchClassTile : tiles)
    {
        pitchClassTile->timerUpdate();
    }
}

void PluginEditor::handleLogMessage(const LogMessage* logMessage)
{
    this->logBox.moveCaretToEnd();
    this->logBox.insertTextAtCaret(logMessage->getString() + juce::newLine);
}

void PluginEditor::noteAdded(juce::MPENote mpeNote)
{
    mpeNotes.insert(mpeNote);
}

void PluginEditor::notePressureChanged(juce::MPENote mpeNote)
{
    mpeNotes.insert(mpeNote);
}

void PluginEditor::notePitchbendChanged(juce::MPENote mpeNote)
{
    mpeNotes.insert(mpeNote);
}
void PluginEditor::noteTimbreChanged(juce::MPENote mpeNote)
{
    mpeNotes.insert(mpeNote);
}
void PluginEditor::noteKeyStateChanged(juce::MPENote mpeNote)
{
    mpeNotes.insert(mpeNote);
}
void PluginEditor::noteReleased(juce::MPENote mpeNote)
{
    mpeNotes.erase(mpeNote);
}
void PluginEditor::zoneLayoutChanged()
{

}

void PluginEditor::updateTiles()
{
    std::unordered_set<Pitch, PitchHash> pitches = std::unordered_set<Pitch, PitchHash>();
    for (const juce::MPENote& mpeNote : mpeNotes)
    {
        Pitch pitch(mpeNote);
        pitches.insert(pitch);
        pitchIntensities[pitch] = 1;
    }
    
    auto it = pitchIntensities.begin();
    while (it != pitchIntensities.end())
    {
        const Pitch& pitch = it->first;
        float& intensity = it->second;
        if (pitches.find(pitch) == pitches.end())
        {
            intensity -= 0.015f;
        }
        if (intensity <= 0)
        {
            it = pitchIntensities.erase(it);
        } 
        else 
        {
            ++it;
        }
    }
    for (const std::unique_ptr<PitchClassTile>& pitchClassTile : tiles)
    {
        pitchClassTile->updatePitchIntensities(pitchIntensities);
    }
}


//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    logBox.setBounds(10, 10, getWidth() - 20, 190);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
