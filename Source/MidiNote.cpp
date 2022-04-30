#include "MidiNote.h"
#include "Pitch.h"

MidiNote::MidiNote(int channel, int midiPitch, int velocity, int pitchWheel) :
	channel(channel),
	midiPitch(midiPitch),
	velocity(velocity),
	pitchWheel(pitchWheel)
{
}

bool MidiNote::operator==(const MidiNote& other)
{
	return channel == other.channel
		&& midiPitch == other.midiPitch
		&& velocity == other.velocity
		&& pitchWheel == other.pitchWheel;
}

bool MidiNote::matches(int channel, int midiPitch)
{
	return this->channel == channel && this->midiPitch == midiPitch;
}

Pitch MidiNote::getPitch()
{
	return midiPitch;
}

void MidiNote::setPitchWheel()
{

}