#include "stdafx.h"
#include <iostream>
#include <TH1.h>
#include <TF1.h>
#include <TH2D.h>
#include "histogramElecLayers.h"
#include "DataSet.h"
#include "Group.h"

// takes the X,Y positions and makes a 2D histogram for each detector
// hist for positrons, ions and electrons separately

void histogramElectronLayers(DataSet *reconData, HistogramElecLayers * UVWlayers, imagingDetectors userDet) {
	//sort for particle type using 
	//Particle is subset of event and contains time and x, y

	for (Group* g : *reconData) {
		if (g->negative.xy_uv == true) {
			UVWlayers->UVlayers->Fill(g->negative.x_uv, g->negative.y_uv);
		}
		if (g->negative.xy_uw == true) {
			UVWlayers->UWlayers->Fill(g->negative.x_uw, g->negative.y_uw);
		}
		if (g->negative.xy_vw == true) {
			UVWlayers->VWlayers->Fill(g->negative.x_vw, g->negative.y_vw);
		}
		if (g->positive.xy_uv == true) {
			UVWlayers->UVPoslayers->Fill(g->positive.x_uv, g->positive.y_uv);
		}
		if (g->positive.xy_uw == true) {
			UVWlayers->UWPoslayers->Fill(g->positive.x_uw, g->positive.y_uw);
		}
		if (g->positive.xy_vw == true) {
			UVWlayers->VWPoslayers->Fill(g->positive.x_vw, g->positive.y_vw);
		}
	}
}