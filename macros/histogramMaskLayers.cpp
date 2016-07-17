#include "stdafx.h"
#include <iostream>
#include <TH1.h>
#include <TF1.h>
#include <TH2D.h>
#include "calibrateLayersHist.h"
#include "DataSet.h"
#include "Group.h"

// takes the X,Y positions and makes a 2D histogram for each detector
// hist for positrons, ions and electrons separately

void histogramMaskLayers(DataSet *reconData, calibrateLayersHist* UVWMasklayers) {
	//sort for particle type using 
	//Particle is subset of event and contains time and x, y

	for (Group* g : *reconData) {
		if (g->negative.xy_uv == true) {
			if (g->negative.y_uv<60 && g->negative.y_uv>-60) {
				UVWMasklayers->UVMasklayer->Fill(g->negative.x_uv);
			}	
		}
		if (g->negative.xy_uw == true) {
			if (g->negative.y_uw<60 && g->negative.y_uw>-60) {
				UVWMasklayers->UWMasklayer->Fill(g->negative.x_uw);
			}	
		}
		if (g->negative.xy_vw == true) {
			if (g->negative.y_vw<60 && g->negative.y_vw> -60) {
				UVWMasklayers->VWMasklayer->Fill(g->negative.x_vw);
			}	
		}/*
		if (g->positive.xy_uv == true) {
			if (g->positive.y_uv<60 && g->positive.y_uv>-60) {
				UVWMasklayers->UVPosMasklayer->Fill(g->positive.x_uv);
			}
		}
		if (g->positive.xy_uw == true) {
			if (g->positive.y_uw<60 && g->positive.y_uw>-60) {
				UVWMasklayers->UWPosMasklayer->Fill(g->positive.x_uw);
			}
		}
		if (g->positive.xy_vw == true) {
			if (g->positive.y_vw<60 && g->positive.y_vw> -60) {
				UVWMasklayers->VWPosMasklayer->Fill(g->positive.x_vw);
			}
		}*/
		if (g->positive.xy_uv == true) {
			if (g->positive.x_uv<5 && g->positive.x_uv>-5) {
				UVWMasklayers->UVPosMasklayer->Fill(g->positive.y_uv);
			}
		}
		if (g->positive.xy_uw == true) {
			if (g->positive.x_uw<5 && g->positive.x_uw>-5) {
				UVWMasklayers->UWPosMasklayer->Fill(g->positive.y_uw);
			}
		}
		if (g->positive.xy_vw == true) {
			if (g->positive.x_vw<5 && g->positive.x_vw> -5) {
				UVWMasklayers->VWPosMasklayer->Fill(g->positive.y_vw);
			}
		}
	}
}