#include "stdafx.h"
#include "PitchPropSet.h"
#include <vector>
#include "Constants.h"

void PitchPropSet::setPitchProp(Charge charge, double uOffset, double uPitchProp, double vOffset, double vPitchProp, double wOffset, double wPitchProp) {
	PitchPropData data;
	data.uPitchProp = uPitchProp;
	data.vPitchProp = vPitchProp;
	data.wPitchProp = wPitchProp;
	data.uOffset = uOffset;
	data.vOffset = vOffset;
	data.wOffset = wOffset;
	setPitchProp(charge, data);
}

void PitchPropSet::setPitchProp(Charge charge, PitchPropData data) {
	if (charge == positive) {
		posPitch = data;
	}
	else {
		negPitch = data;
	}
}

PitchPropData PitchPropSet::getPitchProp(Charge charge) {
	if (charge == positive) {
		return posPitch;
	}
	else {
		return negPitch;
	}
}