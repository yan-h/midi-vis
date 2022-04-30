#pragma once

#include <JuceHeader.h>

// Custom hash for MPENote
struct MPENoteHash {
public:
    size_t operator()(const juce::MPENote& mpeNote) const
    {
        return std::hash<int>()(mpeNote.noteID);
    }
};
