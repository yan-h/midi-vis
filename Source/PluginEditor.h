/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PitchClassTile.h"
#include "Hash.h"
#include "PitchInfo.h"

class LogMessage;

//==============================================================================
/**
*/
class PluginEditor  :
    public juce::AudioProcessorEditor, 
    public juce::MPEInstrument::Listener,
    public juce::Timer
{
public:
    PluginEditor (PluginProcessor&, juce::MPEInstrument&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&);

   // void handleMessage(const juce::Message&) override;

    void noteAdded(juce::MPENote) override;
    void notePressureChanged(juce::MPENote) override;
    void notePitchbendChanged(juce::MPENote) override;
    void noteTimbreChanged(juce::MPENote) override;
    void noteKeyStateChanged(juce::MPENote) override;
    void noteReleased(juce::MPENote) override;
    void zoneLayoutChanged() override;

    void updateTiles();
    void timerCallback();
private:
    void remakeTiles(double, double, double);
    void handleLogMessage(const LogMessage*);
    void updateNote(const juce::MPENote&);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)

    std::vector<juce::String> logMessages;
    juce::TextEditor logBox;

    std::vector<std::unique_ptr<PitchClassTile>> tiles;

    juce::CriticalSection mpeNotesLock;
    std::unordered_set<juce::MPENote, MPENoteHash> mpeNotes;
    std::map<Pitch, PitchInfo> pitchInfos;
    std::set<Pitch> heldPitches;

    juce::MPEInstrument& mpeInstrument;

    juce::ComboBox tuningMenu;
    void tuningChanged();
};
