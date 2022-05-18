#pragma once

class Pitch;

class PitchClass
{
public:
	PitchClass(const Pitch&);
	PitchClass(const PitchClass&);
	bool matchesPitch(const Pitch&) const;
	bool matchesPitch (const Pitch&, double) const;
	bool operator==(const PitchClass&) const;
	double getCents() const;
private:
	double midiPitchClass;
};

