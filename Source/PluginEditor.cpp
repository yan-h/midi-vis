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
    getLookAndFeel().setDefaultSansSerifTypefaceName("Helvetica");

    mpeInstrument.addListener(this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(870, 930);

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

    /*
    addAndMakeVisible(tuningMenu);
    tuningMenu.addItem("12-tet", 1);
    tuningMenu.addItem("31-tet", 2);
    tuningMenu.addItem("53-tet", 3);
    tuningMenu.addItem("5-limit JI", 4);
    tuningMenu.onChange = [this] { tuningChanged(); };
    tuningMenu.setSelectedId(1);
    */

    juce::Font labelFont(16);
    latticeXLabel.setFont(labelFont);
    latticeXLabel.setText("Horizontal (Major third) offset", juce::dontSendNotification);
    latticeXLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(latticeXLabel);

    latticeYLabel.setFont(labelFont);
    latticeYLabel.setText("Vertical (Perfect fifth) offset", juce::dontSendNotification);
    latticeYLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(latticeYLabel);

    latticeZLabel.setFont(labelFont);
    latticeZLabel.setText("Harmonic seventh offset", juce::dontSendNotification);
    latticeZLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(latticeZLabel);

    centsFactor3Label.setFont(labelFont);
    centsFactor3Label.setText("Perfect fifth (cents)", juce::dontSendNotification);
    centsFactor3Label.setJustificationType(juce::Justification::left);
    addAndMakeVisible(centsFactor3Label);

    centsFactor5Label.setFont(labelFont);
    centsFactor5Label.setText("Major third (cents)", juce::dontSendNotification);
    centsFactor5Label.setJustificationType(juce::Justification::left);
    addAndMakeVisible(centsFactor5Label);

    centsFactor7Label.setFont(labelFont);
    centsFactor7Label.setText("Harmonic seventh (cents)", juce::dontSendNotification);
    centsFactor7Label.setJustificationType(juce::Justification::left);
    addAndMakeVisible(centsFactor7Label);

    toleranceLabel.setFont(labelFont);
    toleranceLabel.setText("Tolerance (cents)", juce::dontSendNotification);
    toleranceLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(toleranceLabel);

    factor3ToFactor5Label.setFont(labelFont);
    factor3ToFactor5Label.setText("Set major third in terms of fifths", juce::dontSendNotification);
    factor3ToFactor5Label.setJustificationType(juce::Justification::right);
   // addAndMakeVisible(factor3ToFactor5Label);

    factor3ToFactor5InputLabel.setFont(labelFont);
    factor3ToFactor5InputLabel.setText("4", juce::dontSendNotification);
    initInputLabel(factor3ToFactor5InputLabel);
   // addAndMakeVisible(factor3ToFactor5InputLabel);

    latticeXSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    latticeXSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(latticeXSlider);

    latticeYSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    latticeYSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(latticeYSlider);

    latticeZSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    latticeZSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(latticeZSlider);

    centsFactor3Slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    centsFactor3Slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(centsFactor3Slider);

    centsFactor5Slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    centsFactor5Slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(centsFactor5Slider);

    centsFactor7Slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    centsFactor7Slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(centsFactor7Slider);

    toleranceSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    toleranceSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 50);
    addAndMakeVisible(toleranceSlider);

    latticeXAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LATTICE_X", latticeXSlider);
    latticeYAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LATTICE_Y", latticeYSlider);
    latticeZAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "LATTICE_Z", latticeZSlider);
    centsFactor3Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "CENTS_FACTOR_3", centsFactor3Slider);
    centsFactor5Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "CENTS_FACTOR_5", centsFactor5Slider);
    centsFactor7Attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "CENTS_FACTOR_7", centsFactor7Slider);
    toleranceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "CENTS_TOLERANCE", toleranceSlider);

    float centsFactor3 = audioProcessor.apvts.getRawParameterValue("CENTS_FACTOR_3")->load();
    float centsFactor5 = audioProcessor.apvts.getRawParameterValue("CENTS_FACTOR_5")->load();
    float centsFactor7 = audioProcessor.apvts.getRawParameterValue("CENTS_FACTOR_7")->load();
    int latticeX = std::round(audioProcessor.apvts.getRawParameterValue("LATTICE_X")->load());
    int latticeY = std::round(audioProcessor.apvts.getRawParameterValue("LATTICE_Y")->load());
    int latticeZ = std::round(audioProcessor.apvts.getRawParameterValue("LATTICE_Z")->load());
    float tolerance = audioProcessor.apvts.getRawParameterValue("CENTS_TOLERANCE")->load();

    int size = 70;
    int smallWidth = 24;
    int smallHeight = 24;
    for (int x = 0; x <= 8; x++)
    {
        for (int y = 0; y <= 12; y++)
        {
            int xPos = 10 + x * size;
            int yPos = 10 + y * size;
            int factor3 = -(y - 6);
            int factor5 = x - 4;

            PitchClassTile* newTile = new PitchClassTile(
                factor3 + latticeY, factor5 + latticeX, 0, 
                centsFactor3 * 0.01, centsFactor5 * 0.01, centsFactor7 * 0.01, 
                tolerance * 0.01);
            newTile->setBounds(xPos, yPos, size, size);

            PitchClassTile* upTile = new PitchClassTile(
                factor3 + latticeY, factor5 + latticeX, 1,
                centsFactor3 * 0.01, centsFactor5 * 0.01, centsFactor7 * 0.01,
                tolerance * 0.01);
            upTile->setBounds(xPos + size - smallWidth, yPos, smallWidth, smallHeight);

            PitchClassTile* downTile = new PitchClassTile(
                factor3 + latticeY, factor5 + latticeX, -1,
                centsFactor3 * 0.01, centsFactor5 * 0.01, centsFactor7 * 0.01,
                tolerance * 0.01);
            downTile->setBounds(xPos + size - smallWidth, yPos + size - smallHeight, smallWidth, smallHeight);

            tiles.push_back(std::unique_ptr<PitchClassTile>(newTile));
            tiles.push_back(std::unique_ptr<PitchClassTile>(upTile));
            tiles.push_back(std::unique_ptr<PitchClassTile>(downTile));
            addAndMakeVisible(*newTile);
            addAndMakeVisible(*upTile);
            addAndMakeVisible(*downTile);
        }
    }

    latticeXSlider.addListener(this);
    latticeYSlider.addListener(this);
    latticeZSlider.addListener(this);
    centsFactor3Slider.addListener(this);
    centsFactor5Slider.addListener(this);
    centsFactor7Slider.addListener(this);
    toleranceSlider.addListener(this);

    startTimerHz(60);
}

void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    float centsFactor3 = centsFactor3Slider.getValue();
    float centsFactor5 = centsFactor5Slider.getValue();
    float centsFactor7 = centsFactor7Slider.getValue();
    int latticeX = latticeXSlider.getValue();
    int latticeY = latticeYSlider.getValue();
    int latticeZ = latticeZSlider.getValue();
    float tolerance = toleranceSlider.getValue();
    for (auto& tile : tiles)
    {
        tile->setTuning(
            latticeY, latticeX, latticeZ,
            centsFactor3 * 0.01, centsFactor5 * 0.01, centsFactor7 * 0.01, 
            tolerance * 0.01);
    }
}

void PluginEditor::initInputLabel(juce::Label& label)
{
    label.setEditable(true);
    label.setColour(juce::Label::backgroundColourId, juce::Colours::white);
    label.setColour(juce::Label::textColourId, juce::Colours::black);
    label.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    label.setJustificationType(juce::Justification::left);
}

void PluginEditor::resized()
{
    int xStart = 660;
    //logBox.setBounds(10, 10, getWidth() - 20, 190);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
   // tuningMenu.setBounds(xStart, 10, 200, 30);

    latticeYLabel.setBounds(xStart, 100, 200, 30);
    latticeYSlider.setBounds(xStart, 130, 200, 30);

    latticeXLabel.setBounds(xStart, 180, 200, 30);
    latticeXSlider.setBounds(xStart, 210, 200, 30);

    latticeZLabel.setBounds(xStart, 260, 200, 30);
    latticeZSlider.setBounds(xStart, 290, 200, 30);

    centsFactor3Label.setBounds(xStart, 340, 200, 30);
    centsFactor3Slider.setBounds(xStart, 370, 200, 30);

    centsFactor5Label.setBounds(xStart, 420, 200, 30);
    centsFactor5Slider.setBounds(xStart, 450, 200, 30);

    centsFactor7Label.setBounds(xStart, 500, 200, 30);
    centsFactor7Slider.setBounds(xStart, 530, 200, 30);

    toleranceLabel.setBounds(xStart, 580, 200, 30);
    toleranceSlider.setBounds(xStart, 610, 200, 30);
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

    updateNote(mpeNote);
}

void PluginEditor::notePressureChanged(juce::MPENote mpeNote)
{
    updateNote(mpeNote);
}

void PluginEditor::updateNote(const juce::MPENote& mpeNote)
{
    const juce::ScopedLock lock(mpeNotesLock);
    mpeNotes.erase(mpeNote);
    mpeNotes.insert(mpeNote);
}

void PluginEditor::notePitchbendChanged(juce::MPENote mpeNote)
{
    const juce::ScopedLock lock(mpeNotesLock);

    Pitch pitch = Pitch::fromFreqHz(mpeNote.getFrequencyInHertz());

    if (mpeNotes.find(mpeNote) != mpeNotes.end())
    {
        heldPitches.erase(Pitch::fromFreqHz(mpeNotes.find(mpeNote)->getFrequencyInHertz()));
    }

    heldPitches.insert(pitch);

    double topIntensity = *heldPitches.rbegin() == pitch ? 1.0 : 0.0;
    double bassIntensity = *heldPitches.begin() == pitch ? 1.0 : 0.0;
    pitchInfos[pitch] = PitchInfo(1.0, topIntensity, bassIntensity);
    updateNote(mpeNote);
}
void PluginEditor::noteTimbreChanged(juce::MPENote mpeNote)
{
    updateNote(mpeNote);
}
void PluginEditor::noteKeyStateChanged(juce::MPENote mpeNote)
{
    updateNote(mpeNote);
}
void PluginEditor::noteReleased(juce::MPENote mpeNote)
{
    const juce::ScopedLock lock(mpeNotesLock);

    Pitch pitch = Pitch::fromFreqHz(mpeNote.getFrequencyInHertz());

    mpeNotes.erase(mpeNote);
    for (const auto& mpeNote : mpeNotes) {
        Pitch mpePitch = Pitch::fromFreqHz(mpeNotes.find(mpeNote)->getFrequencyInHertz());
        if (pitch == mpePitch) {
            return;
        }
    }

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

    double markerIntensityChange = 0.15;

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
            pitchInfo.topIntensity = std::max(pitchInfo.topIntensity - markerIntensityChange, 0.0);
        else
            pitchInfo.topIntensity = std::min(pitchInfo.topIntensity + markerIntensityChange, 1.0);

        if (heldPitches.find(pitch) == heldPitches.end() || pitch != minPitch)
            pitchInfo.bassIntensity = std::max(pitchInfo.bassIntensity - markerIntensityChange, 0.0);
        else
            pitchInfo.bassIntensity = std::min(pitchInfo.bassIntensity + markerIntensityChange, 1.0);

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

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}
