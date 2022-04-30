#pragma once

class Pitch;

class MidiNote
{
public:
	MidiNote(int channel, int midiPitch, int velocity, int pitchWheel);
	bool operator==(const MidiNote&);
	Pitch getPitch();
	void setPitchWheel();
	bool matches(int channel, int midiPitch);
private:
	int channel;
	int midiPitch;
	int velocity;
	int pitchWheel;
};

