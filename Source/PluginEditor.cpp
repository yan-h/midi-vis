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
    getLookAndFeel().setDefaultSansSerifTypefaceName("Lucida Sans Unicode");

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
    setSize(1100, 900);

    int size = 80;
    for (int x = 0; x <= 10; x++)
    {
        for (int y = 0; y <= 10; y++)
        {
            int xPos = 10 + x * size;
            int yPos = 10 + y * size;
            int factor3 = -(y - 5);
            int factor5 = x - 5;
            PitchClassTile* newTile = new PitchClassTile(factor3, factor5, 7, 4);
            newTile->setBounds(xPos, yPos, size, size);
            tiles.push_back(std::unique_ptr<PitchClassTile>(newTile));
            addAndMakeVisible(*newTile);
        }
    }

    addAndMakeVisible(tuningMenu);
    tuningMenu.addItem("12-tet", 1);
    tuningMenu.addItem("31-tet", 2);
    tuningMenu.addItem("53-tet", 3);
    tuningMenu.addItem("5-limit JI", 4);
    tuningMenu.onChange = [this] { tuningChanged(); };
    tuningMenu.setSelectedId(1);

    startTimerHz(60);
}

void PluginEditor::remakeTiles(double semisFactor3, double semisFactor5, double tolerance)
{
    for (auto& tile : tiles)
    {
        tile->setTuning(semisFactor3, semisFactor5, tolerance);
    }
}

PluginEditor::~PluginEditor()
{
    mpeInstrument.removeListener(this);
}

void PluginEditor::timerCallback()
{
    const juce::ScopedLock lock(mpeNotesLock);
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
    const juce::ScopedLock lock(mpeNotesLock);

    Pitch pitch = Pitch::fromFreqHz(mpeNote.getFrequencyInHertz());

    heldPitches.insert(pitch);

    double topIntensity = *heldPitches.rbegin() == pitch ? 1.0 : 0.0;
    double bassIntensity = *heldPitches.begin() == pitch ? 1.0 : 0.0;
    pitchInfos[pitch] = PitchInfo(1.0, topIntensity, bassIntensity);
}

void PluginEditor::notePressureChanged(juce::MPENote mpeNote)
{
   // updateNote(mpeNote);
}

void PluginEditor::notePitchbendChanged(juce::MPENote mpeNote)
{
   // updateNote(mpeNote);
}
void PluginEditor::noteTimbreChanged(juce::MPENote mpeNote)
{
   // updateNote(mpeNote);
}
void PluginEditor::noteKeyStateChanged(juce::MPENote mpeNote)
{
  //  updateNote(mpeNote);
}
void PluginEditor::noteReleased(juce::MPENote mpeNote)
{
    const juce::ScopedLock lock(mpeNotesLock);

    Pitch pitch = Pitch::fromFreqHz(mpeNote.getFrequencyInHertz());

    heldPitches.erase(pitch);
}
void PluginEditor::zoneLayoutChanged()
{

}

void PluginEditor::updateTiles()
{
    Pitch maxPitch = Pitch(-9999.0);
    Pitch minPitch = Pitch(-9999.0);
    if (heldPitches.size() > 0)
    {
        maxPitch = *heldPitches.rbegin();
        minPitch = *heldPitches.begin();
    }

    for (auto& it : heldPitches) {
     //   DBG(it.getMidiPitch());
    }

    auto it = pitchInfos.begin();
    while (it != pitchInfos.end())
    {
        Pitch pitch = it->first;
        PitchInfo& pitchInfo = it->second;

        if (heldPitches.find(pitch) == heldPitches.end())
            pitchInfo.noteIntensity = std::max(pitchInfo.noteIntensity - 0.01, 0.0);
        else 
            pitchInfo.noteIntensity = 1.0;

        if (heldPitches.find(pitch) == heldPitches.end() || pitch != maxPitch)
            pitchInfo.topIntensity = std::max(pitchInfo.topIntensity - 0.15, 0.0);
        else
            pitchInfo.topIntensity = std::min(pitchInfo.topIntensity + 0.15, 1.0);

        if (heldPitches.find(pitch) == heldPitches.end() || pitch != minPitch)
            pitchInfo.bassIntensity = std::max(pitchInfo.bassIntensity - 0.15, 0.0);
        else
            pitchInfo.bassIntensity = std::min(pitchInfo.bassIntensity + 0.15, 1.0);

        if (pitchInfo.noteIntensity <= 0)
            it = pitchInfos.erase(it);
        else 
            ++it;
    }

    // Update PitchClassTiles
    for (const std::unique_ptr<PitchClassTile>& pitchClassTile : tiles)
    {
        pitchClassTile->updatePitchIntensities(pitchInfos);
    }
}

void PluginEditor::tuningChanged()
{
    switch (tuningMenu.getSelectedId())
    {
    case 1:
        remakeTiles(7, 4, 0.5);
        break;
    case 2:
        remakeTiles(6.967742, 3.870968, 0.01);
        break;
    case 3:
        remakeTiles(7.018868, 3.849057, 0.01);
        break;
    case 4:
        remakeTiles(7.01955, 3.86314, 0.01);
        break;
    default:
        remakeTiles(7, 4, 0.5);
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
    //logBox.setBounds(10, 10, getWidth() - 20, 190);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    tuningMenu.setBounds(900, 10, 170, 30);
}
