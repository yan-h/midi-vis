#pragma once
#include <JuceHeader.h>
class LogMessage : public juce::Message
{
public:
	LogMessage(const juce::String&);
	juce::String getString() const;
private:
	juce::String string;
};

