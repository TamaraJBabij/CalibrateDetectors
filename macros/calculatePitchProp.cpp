#include <iostream>
#include "DataSet.h"
#include "Group.h"
#include "Hit.h"
#include "Event.h"
#include "stdafx.h"
#include "LayerLengthsConfig.h"
#include <vector>
#include "Constants.h"
#include "MCPTDiff.h"
#include "FitSet.h"
#include "FitData.h"
#include "PitchPropSet.h"
#include "PitchData.h"


//caluculates pitch propogation required for reconstruction of coordinates
PitchPropSet calculatePitchProp() {
	// rough calculation for pitch propogation
	//need to getFits to make info public
	//no longer need FitSet fits to calculate pitch propagation
	double upospitch;
	double vpospitch;
	double wpospitch;
	double unegpitch;
	double vnegpitch;
	double wnegpitch;

	PitchPropSet Pitches;

	upospitch = LENGTH_pU / 140.42;
	vpospitch = LENGTH_pV / 136.27;
	wpospitch = LENGTH_pW / 132.62;
	unegpitch = LENGTH_eU / 122.00; //124.14;
	vnegpitch = LENGTH_eV / 126.00; //124.89;
	wnegpitch = LENGTH_eW / 117.00;//117.39;

	Pitches.setPitchProp(positive, upospitch, vpospitch, wpospitch);
	Pitches.setPitchProp(negative, unegpitch, vnegpitch, wnegpitch);
	return Pitches;

}