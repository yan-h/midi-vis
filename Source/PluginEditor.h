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
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

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
    void handleLogMessage(const LogMessage*);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)

    std::vector<juce::String> logMessages;
    juce::TextEditor logBox;

    std::vector<std::unique_ptr<PitchClassTile>> tiles;

    std::unordered_set<juce::MPENote, MPENoteHash> mpeNotes;
    std::map<Pitch, float> pitchIntensities;

    juce::MPEInstrument& mpeInstrument;
};
