#pragma once

class Pitch;

class PitchClass
{
public:
	PitchClass(const Pitch&);
	PitchClass(const PitchClass&);
	bool matchesPitch(const Pitch&) const;
	bool matchesPitch (const Pitch&, float) const;
	bool operator==(const PitchClass&) const;
	float getCents() const;
private:
	float midiPitchClass;
};

