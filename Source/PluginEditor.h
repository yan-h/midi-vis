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
#include "InputLabel.h"

class LogMessage;

//==============================================================================
/**
*/
class PluginEditor  :
    public juce::AudioProcessorEditor, 
    public juce::MPEInstrument::Listener,
    public juce::Timer,
    private juce::Slider::Listener
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
    void handleLogMessage(const LogMessage*);
    void updateNote(const juce::MPENote&);
    void initInputLabel(juce::Label&);

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

    juce::Label latticeXLabel;
    juce::Label latticeYLabel;
    juce::Label latticeZLabel;
    juce::Label centsFactor3Label;
    juce::Label centsFactor5Label;
    juce::Label centsFactor7Label;
    juce::Label toleranceLabel;

    juce::Label factor3ToFactor5Label;
    InputLabel factor3ToFactor5InputLabel;

    juce::Slider latticeXSlider;
    juce::Slider latticeYSlider;
    juce::Slider latticeZSlider;
    juce::Slider centsFactor3Slider;
    juce::Slider centsFactor5Slider;
    juce::Slider centsFactor7Slider;
    juce::Slider toleranceSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> latticeXAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> latticeYAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> latticeZAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> centsFactor3Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> centsFactor5Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> centsFactor7Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toleranceAttachment;

    virtual void sliderValueChanged(juce::Slider* slider) override;
};
