#pragma once

#include "InputLabel.h"
#include <JuceHeader.h>

InputLabel::InputLabel()
{}


InputLabel::InputLabel(const juce::String& name, const juce::String& labelText) 
    : Label(name, labelText)
{}

juce::TextEditor* InputLabel::createEditorComponent()
{
    juce::TextEditor* const ed = juce::Label::createEditorComponent();
    ed->setInputRestrictions(9, "0123456789.");
    return ed;
}