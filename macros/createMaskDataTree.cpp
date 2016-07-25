#include "stdafx.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include "TFunction.h"
#include "TH2D.h"
#include "constants.h"
#include "HistogramXY.h"
#include "TMath.h"
#include "TF2.h"

/*Creates a tree of X,Y positions corresponding to the calibraiton mask on either 
the positive or the negative detector*/

void createMaskDataTree(imagingDetectors userDet, HistogramXY* XYpositions, CalibrateLoadType sessionOption) {
	if (userDet == negDet) {
		/*creates data for the negative detector UV, UW and WV layers, accounting for the gaps in the detector*/
		double y = 0;
		double x = 0;
		TF2 *mygaus = new TF2("mygaus", "TMath::Gaus", -5, 5, -5, 5);
		XYpositions->modeldataNegDET->FillRandom("mygaus", 100);

	}
}