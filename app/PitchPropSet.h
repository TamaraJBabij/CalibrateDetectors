#pragma once
#include "PitchData.h"
#include <vector>
#include "Constants.h"

//struct to store the pitch propogation values to be used
//to get X Y position info

using namespace std;

class PitchPropSet {
private:
	PitchPropData posPitch;
	PitchPropData negPitch;
public:
	void setPitchProp(Charge charge, double uOffset, double uPitchProp, double vOffset, double vPitchProp, double wOffset, double wPitchProp);
	PitchPropData getPitchProp(Charge charge);
};
