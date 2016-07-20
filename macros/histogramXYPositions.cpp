#include "stdafx.h"
#include <iostream>
#include <TH1.h>
#include <TF1.h>
#include <TH2D.h>
#include "HistogramXY.h"
#include "DataSet.h"
#include "Group.h"

// takes the X,Y positions and makes a 2D histogram for each detector
// hist for positrons, ions and electrons separately

void histogramXYPositions(DataSet *reconData, HistogramXY XYpositions) {
	//sort for particle type using 
	//Particle is subset of event and contains time and x, y
	

	for (Group* g : *reconData) {
		//cout << "x " << g->positive.x << endl;
		if (g->positive.x) {
			if (g->positive.y) {
				XYpositions.positronDET->Fill(g->positive.x, g->positive.y);
			}
		}
		if (g->negative.x ) {
			if (g->negative.y) {
				XYpositions.electronDET->Fill(g->negative.x, g->negative.y);
				cout << "hist fill" << endl;
			}
		}
		//XYpositions.positronDET->Fill(g->positive.x, g->positive.y);
		//XYpositions.electronDET->Fill(g->negative.x, g->negative.y);
	}
	
}