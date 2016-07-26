#pragma once

#include <TH1D.h>

// Container for layer sum histograms

struct layerSumHist {

	//layer Sums, ex: u1+u2, v1+v2 etc
	TH1D* uPosLayerSum;
	TH1D* vPosLayerSum;
	TH1D* wPosLayerSum;
	TH1D* uNegLayerSum;
	TH1D* vNegLayerSum;
	TH1D* wNegLayerSum;
};