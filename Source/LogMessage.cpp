#include "LogMessage.h"
#include <JuceHeader.h>

LogMessage::LogMessage(const juce::String& s) : string(s)
{
}

juce::String LogMessage::getString() const
{
	return this->string;
}