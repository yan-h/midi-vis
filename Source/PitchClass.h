#pragma once

class Pitch;

class PitchClass
{
public:
	PitchClass(const Pitch&);
	PitchClass(const PitchClass&);
	bool containsPitch(const Pitch&) const;
	bool operator==(const PitchClass&) const;
	float getCents() const;
private:
	float midiPitchClass;
};

