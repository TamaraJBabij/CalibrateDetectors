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
	double uposoffset;
	double vpospitch;
	double vposoffset;
	double wpospitch;
	double wposoffset;
	double unegpitch;
	double unegoffset;
	double vnegpitch;
	double vnegoffset;
	double wnegpitch;
	double wnegoffset;

	PitchPropSet Pitches;

	upospitch = LENGTH_pU / 140.42;
	vpospitch = LENGTH_pV / 136.27;
	wpospitch = LENGTH_pW / 132.62;
	uposoffset = 0.00;
	vposoffset = 0.00; 
	wposoffset = 0.00; 
	unegpitch = LENGTH_eU / 122.00; //124.14;
	cout << "unegpitch" << unegpitch << endl;
	vnegpitch = LENGTH_eV / 126.00; //124.89;
	cout << "vnegpitch" << vnegpitch << endl;
	wnegpitch = LENGTH_eW / 117.00;//117.39;
	cout << "wnegpitch" << wnegpitch << endl;
	unegoffset = 2.65;
	cout << "unegoffset" << unegoffset << endl;
	vnegoffset = 1.40; //0.75;
	cout << "vnegoffset" << vnegoffset << endl;
	wnegoffset = 0.25; //1.05;
	cout << "wnegoffset" << wnegoffset << endl;
	Pitches.setPitchProp(positive, upospitch, uposoffset, vpospitch, vposoffset, wpospitch, wposoffset);
	Pitches.setPitchProp(negative, unegpitch, unegoffset, vnegpitch, vnegoffset, wnegpitch, wnegoffset);
	return Pitches;

}