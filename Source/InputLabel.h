#pragma once

#include <JuceHeader.h>

class InputLabel : public juce::Label
{
public:
	InputLabel();
	InputLabel(const juce::String& name, const juce::String& labelText);
	virtual juce::TextEditor* createEditorComponent() override;
};