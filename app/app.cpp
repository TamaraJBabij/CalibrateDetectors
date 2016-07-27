// app.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "macros.h"
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TH1D.h>
#include <TFormula.h>
#include <TH1.h>
#include <TF1.h>
#include <TStyle.h>
#include "DataSet.h"
#include "TFile.h"
#include "HistogramPair.h"
#include "TObject.h"
#include "Event.h"
#include "HistogramTimeSums.h"
#include "HistogramXY.h"
#include "configlayers.h"
#include "Constants.h"
#include "FitSet.h"
#include <TStyle.h>
#include "histogramElecLayers.h"
#include "histogramElecLayers.h"
#include <TGraph.h>
#include <TLegend.h>
#include "dirent.h"
#include <stdio.h>
#include <string.h>
#include <TAxis.h>
#include "HistUVWPositions.h"
#include "histlayerSums.h"

using namespace std;

/*CALIBRATION PROGRAM FOR DETECTORS
* Note: data being parsed into program must be detectors with the calibration mask on
* Two options for loading in data: 1. read, read in the raw data trees, process timesums for the first tree, then obtain X,Y images. 
* After initial processing, this option also loads all layer timing info, u1, u2, v1..... etc into a tree that can be used for the second option.
* 2. load, This option loads the tree produced in option 1, this allows for initial processing and loading to be skipped. Essentially data is loaded in
* at the point where it is ready to be orocessed using the variables in need of calibration. Allows for quicker chanigng of variables and processing. 
* This program does not select out for particles and will use all reconstructable hits to produce an image
* Author: Tamara J Babij 
* version control of software found on: https://github.com/TamaraJBabij/CalibrateDetectors/  */

int main(int argc, char* argv[]) {
	//Initial function to set up the debug environment
	int setUpDebugEnvironment();
	//initialises root app
	TApplication* rootapp = new TApplication("C:/Users/TamaraB/Documents/GitHub/CalibrateDetectors", &argc, argv);

	/*
	This program can be run for lower intensity beams, however maxes out memory above 60 trees
	DataSet* data = scanFiles();
	*/

	/*Implementation of detinput allows for calibration of one detector at a time*/
	string detinput;
	cout << "what detectors do you want to image: neg, pos or both? " << endl;
	cin >> detinput;
	imagingDetectors userDet;
	if (detinput.compare("pos") == 0) {
		userDet = posDet;
		//cout << "pos" << endl;
	}
	else if (detinput.compare("neg") == 0) {
		userDet = negDet;
		//cout << "neg" << endl;
	}
	else if (detinput.compare("both") == 0) {
		userDet = bothDet;
		//cout << "both" << endl;
	}
	else {
		cout << "Input does not match a valid detector" << endl;
		exit(1);
	}

	//	Load from the DAQ tree files, then load from from raw positions tree
	/*loadtype allows for two options of data processing, either loading in the raw DAQ trees, doing initial processing (timesum checks etc) 
	and then saving timing info to a second tree, i.e. u1, u2, v1 etc. This allows for quicker detector calibration without having to reload the 
	raw DAQ trees each time. It also takes up less of the visual studio virtual memory. */
	string loadtype;
	cout << "Would you like to run program to read in DAQ files (type: read), or to load a raw positions tree (type: load), or claibrate by minimising error function (type: error),or create fake data(type:create)" << endl;
	cin >> loadtype;
	CalibrateLoadType sessionOption;
	if (loadtype.compare("read") == 0) {
		sessionOption = PositionTreeWrite;
	}
	else if (loadtype.compare("load") == 0) {
		sessionOption = PositionTreeRead;
	}
	else if (loadtype.compare("create") == 0) {
		sessionOption = PositionTreeCreate;
	}
	else if (loadtype.compare("error") == 0) {
		sessionOption = PositionTreeErrorCalibrate;
	}
	else if (loadtype.compare("slope") == 0) {
		sessionOption = PositionTreeSlope;
	}

	HistogramXY XYpositions;
	/*create fake mask data X,Y tree*/
	TCanvas createdDataCanvas("Data", "X,Y positions");
	XYpositions.modeldataNegDET = new TH2D("electronDET", "Electrons", 400, -60, 60, 400, -60, 60);
	XYpositions.modeldataNegDET->Draw("colz");
	if (sessionOption == PositionTreeCreate) {
		//run data model and then exit
		//run create data program
		createMaskDataTree(userDet, &XYpositions, sessionOption);
		createdDataCanvas.Modified();
		createdDataCanvas.Update();
		rootapp->Draw();
	}


	//scans folder created by the DAQ software and loads all tree ( .root) files into the dataset
	//not currently neccasary for the load options
	string fileLocation;
	cout << "what is the directory? remember to add a backslash at the end" << endl;
	cin >> fileLocation;

	int filenumber = 0;
	DIR* dir;
	dirent* pdir;
	dir = opendir(fileLocation.c_str());
	HistogramTimeSums timesums;
	bool firstFile = true;
	//delcare here as whatever curly brackets the variable is declared in it stays in (called scope)
	FitSet fits;
	gStyle->SetPalette(1);
	double_t canvasWidth = 800;
	double_t h = 800;
	PitchPropSet Pitches;
	//calculate pitch propogation
	Pitches = calculatePitchProp();
	TCanvas timesumsCanvas("Time sums canvas", "Time Sums");

	//Needs to be implemented for pos det
	TCanvas layersCanvas("U V W", "U V W positions");
	UVWpositionsHist UVWPositions;
	if (userDet == negDet) {
		UVWPositions.UNegPositions = new TH1D("Negative Detector", "U layer", 400, -60, 60);
		UVWPositions.VNegPositions = new TH1D("Negative Detector", "V layer", 400, -60, 60);
		UVWPositions.WNegPositions = new TH1D("Negative Detector", "W layer", 400, -60, 60);
		UVWPositions.UNegPositions->SetLineColor(kBlue);
		UVWPositions.VNegPositions->SetLineColor(kRed);
		UVWPositions.WNegPositions->SetLineColor(kBlack);
		UVWPositions.UNegPositions->Draw("hist");
		UVWPositions.VNegPositions->Draw("SameHist");
		UVWPositions.WNegPositions->Draw("SameHist");
		TLegend* negpositionsLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		negpositionsLegend->AddEntry(UVWPositions.UNegPositions, "U layer");
		negpositionsLegend->AddEntry(UVWPositions.VNegPositions, "V layer");
		negpositionsLegend->AddEntry(UVWPositions.WNegPositions, "W layer");
		negpositionsLegend->Draw();
	}
	else if (userDet == posDet) {
		UVWPositions.UPosPositions = new TH1D("Positive Detector", "U layer", 400, -60, 60);
		UVWPositions.VPosPositions = new TH1D("Positive Detector", "V layer", 400, -60, 60);
		UVWPositions.WPosPositions = new TH1D("Positive Detector", "W layer", 400, -60, 60);
		UVWPositions.UPosPositions->SetLineColor(kBlue);
		UVWPositions.VPosPositions->SetLineColor(kRed);
		UVWPositions.WPosPositions->SetLineColor(kBlack);
		UVWPositions.UPosPositions->Draw("hist");
		UVWPositions.VPosPositions->Draw("SameHist");
		UVWPositions.WPosPositions->Draw("SameHist");
		TLegend* pospositionsLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		pospositionsLegend->AddEntry(UVWPositions.UPosPositions, "U layer");
		pospositionsLegend->AddEntry(UVWPositions.VPosPositions, "V layer");
		pospositionsLegend->AddEntry(UVWPositions.WPosPositions, "W layer");
		pospositionsLegend->Draw();
	}
	else if (userDet == bothDet) {
		layersCanvas.Divide(2);
		layersCanvas.cd(1);
		UVWPositions.UNegPositions = new TH1D("Negative Detector", "U layer", 400, -60, 60);
		UVWPositions.VNegPositions = new TH1D("Negative Detector", "V layer", 400, -60, 60);
		UVWPositions.WNegPositions = new TH1D("Negative Detector", "W layer", 400, -60, 60);
		UVWPositions.UNegPositions->SetLineColor(kBlue);
		UVWPositions.VNegPositions->SetLineColor(kRed);
		UVWPositions.WNegPositions->SetLineColor(kBlack);
		UVWPositions.UNegPositions->Draw("hist");
		UVWPositions.VNegPositions->Draw("SameHist");
		UVWPositions.WNegPositions->Draw("SameHist");
		TLegend* negpositionsLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		negpositionsLegend->AddEntry(UVWPositions.UNegPositions, "U layer");
		negpositionsLegend->AddEntry(UVWPositions.VNegPositions, "V layer");
		negpositionsLegend->AddEntry(UVWPositions.WNegPositions, "W layer");
		negpositionsLegend->Draw();
		layersCanvas.cd(2);
		UVWPositions.UPosPositions = new TH1D("Positive Detector", "U layer", 400, -60, 60);
		UVWPositions.VPosPositions = new TH1D("Positive Detector", "V layer", 400, -60, 60);
		UVWPositions.WPosPositions = new TH1D("Positive Detector", "W layer", 400, -60, 60);
		UVWPositions.UPosPositions->SetLineColor(kBlue);
		UVWPositions.VPosPositions->SetLineColor(kRed);
		UVWPositions.WPosPositions->SetLineColor(kBlack);
		UVWPositions.UPosPositions->Draw("hist");
		UVWPositions.VPosPositions->Draw("SameHist");
		UVWPositions.WPosPositions->Draw("SameHist");
		TLegend* pospositionsLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		pospositionsLegend->AddEntry(UVWPositions.UPosPositions, "U layer");
		pospositionsLegend->AddEntry(UVWPositions.VPosPositions, "V layer");
		pospositionsLegend->AddEntry(UVWPositions.WPosPositions, "W layer");
		pospositionsLegend->Draw();
	}
	

	TCanvas XYPosDet("Positive Detector", "XY Positions", canvasWidth, h);
	TCanvas XYNegDet("Negative Detector", "XY Positions", canvasWidth, h);
	
	if (userDet == bothDet) {
		XYpositions.positronDET = new TH2D("positronDET", "Positrons", 400, -60, 60, 400, -60, 60);
		XYpositions.electronDET = new TH2D("electronDET", "Electrons", 400, -60, 60, 400, -60, 60);
		XYPosDet.cd();
		XYpositions.positronDET->Draw("colz");
		XYNegDet.cd();
		XYpositions.electronDET->Draw("colz");
	}
	else if (userDet == posDet) {
		XYpositions.positronDET = new TH2D("positronDET", "Positrons", 400, -60, 60, 400, -60, 60);
		XYPosDet.cd();
		XYpositions.positronDET->Draw("colz");
	}
	else if (userDet == negDet) {
		XYpositions.electronDET = new TH2D("electronDET", "Electrons", 400, -60, 60, 400, -60, 60);
		XYNegDet.cd();
		XYpositions.electronDET->Draw("colz");
	}
	//Histograms the positron and electron layers, need to change name
	HistogramElecLayers UVWlayers;
	calibrateLayersHist UVWMasklayers;
	TCanvas UVWNeglayersCanvas("UVW layers Canvas", "UVW Negative Detector", canvasWidth, h);
	TCanvas UVWPoslayersCanvas("UVW layers Canvas", "UVW Positive Detctor", canvasWidth, h);
	TCanvas UVWNegMaskLayersCanvasX("UVWMaskLayersCanvas", "UVW Negative Detector Layers, X vs Int");
	TCanvas UVWNegMaskLayersCanvasY("UVWMaskLayersCanvas", "UVW Negative Detector Layers, y vs Int");
	TCanvas UVWPosMaskLayersCanvas("UVWMaskLayersCanvas", "UVW Positive Detector Layers");
	if (userDet == negDet) {
		UVWNeglayersCanvas.cd();
		UVWlayers.UVlayers = new TH2D("electronDET", "UV layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.UWlayers = new TH2D("electronDET", "UW layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.VWlayers = new TH2D("electronDET", "VW layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.UVlayers->SetMarkerColor(kBlue);
		UVWlayers.UVlayers->SetLineColor(kBlue);
		UVWlayers.UWlayers->SetMarkerColor(kRed);
		UVWlayers.UWlayers->SetLineColor(kRed);
		UVWlayers.VWlayers->SetLineColor(kBlack);
		UVWlayers.UVlayers->Draw("hist");
		UVWlayers.UWlayers->Draw("SameHist");
		UVWlayers.VWlayers->Draw("SameHist");
		UVWNeglayersCanvas.SetTitle("UVW Layers Combined; x (mm); y (mm)");
		TLegend* elecLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		elecLegend->AddEntry(UVWlayers.UVlayers, "UV layer");
		elecLegend->AddEntry(UVWlayers.UWlayers, "UW layer");
		elecLegend->AddEntry(UVWlayers.VWlayers, "WV layer");
		elecLegend->Draw();
		UVWNegMaskLayersCanvasX.cd();
		UVWMasklayers.UVMasklayer = new TH1D("electronDET", "UV layer", 400, -60, 60);
		UVWMasklayers.UWMasklayer = new TH1D("electronDET", "UW layer", 400, -60, 60);
		UVWMasklayers.VWMasklayer = new TH1D("electronDET", "VW layer", 400, -60, 60);
		UVWMasklayers.UVMasklayer->SetLineColor(kBlue);
		UVWMasklayers.UWMasklayer->SetLineColor(kRed);
		UVWMasklayers.VWMasklayer->SetLineColor(kBlack);
		UVWMasklayers.UVMasklayer->Draw("hist");
		UVWMasklayers.UWMasklayer->Draw("SameHist");
		UVWMasklayers.VWMasklayer->Draw("SameHist");
		TLegend* elecMaskLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		elecMaskLegend->AddEntry(UVWMasklayers.UVMasklayer, "UV layer");
		elecMaskLegend->AddEntry(UVWMasklayers.UWMasklayer, "UW layer");
		elecMaskLegend->AddEntry(UVWMasklayers.VWMasklayer, "WV layer");
		elecMaskLegend->Draw();
		UVWNegMaskLayersCanvasY.cd();
		UVWMasklayers.UVMasklayerY = new TH1D("electronDET", "UV layer", 400, -60, 60);
		UVWMasklayers.UWMasklayerY = new TH1D("electronDET", "UW layer", 400, -60, 60);
		UVWMasklayers.VWMasklayerY = new TH1D("electronDET", "VW layer", 400, -60, 60);
		UVWMasklayers.UVMasklayerY->SetLineColor(kBlue);
		UVWMasklayers.UWMasklayerY->SetLineColor(kRed);
		UVWMasklayers.VWMasklayerY->SetLineColor(kBlack);
		UVWMasklayers.UVMasklayerY->Draw("hist");
		UVWMasklayers.UWMasklayerY->Draw("SameHist");
		UVWMasklayers.VWMasklayerY->Draw("SameHist");
		TLegend* elecMaskLegendY = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		elecMaskLegendY->AddEntry(UVWMasklayers.UVMasklayerY, "UV layer");
		elecMaskLegendY->AddEntry(UVWMasklayers.UWMasklayerY, "UW layer");
		elecMaskLegendY->AddEntry(UVWMasklayers.VWMasklayerY, "WV layer");
		elecMaskLegendY->Draw();
	}
	else if (userDet == posDet) {
		UVWPoslayersCanvas.cd();
		UVWlayers.UVPoslayers = new TH2D("positronDET", "UV layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.UWPoslayers = new TH2D("positronDET", "UW layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.VWPoslayers = new TH2D("positronDET", "VW layer", 400, -60, 60, 400, -60, 60);
		UVWlayers.UVPoslayers->SetMarkerColor(kBlue);
		UVWlayers.UVPoslayers->SetLineColor(kBlue);
		UVWlayers.UWPoslayers->SetMarkerColor(kRed);
		UVWlayers.UWPoslayers->SetLineColor(kRed);
		UVWlayers.VWPoslayers->SetLineColor(kBlack);
		UVWlayers.UVPoslayers->Draw("hist");
		UVWlayers.UWPoslayers->Draw("SameHist");
		UVWlayers.VWPoslayers->Draw("SameHist");
		UVWPoslayersCanvas.SetTitle("UVW Layers Combined; x (mm); y (mm)");
		TLegend* posLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		posLegend->AddEntry(UVWlayers.UVPoslayers, "UV layer");
		posLegend->AddEntry(UVWlayers.UWPoslayers, "UW layer");
		posLegend->AddEntry(UVWlayers.VWPoslayers, "WV layer");
		posLegend->Draw();
		UVWPosMaskLayersCanvas.cd();
		UVWMasklayers.UVPosMasklayer = new TH1D("positronDET", "UV layer", 400, -60, 60);
		UVWMasklayers.UWPosMasklayer = new TH1D("positronDET", "UW layer", 400, -60, 60);
		UVWMasklayers.VWPosMasklayer = new TH1D("positronDET", "VW layer", 400, -60, 60);
		UVWMasklayers.UVPosMasklayer->SetLineColor(kBlue);
		UVWMasklayers.UWPosMasklayer->SetLineColor(kRed);
		UVWMasklayers.VWPosMasklayer->SetLineColor(kBlack);
		UVWMasklayers.UVPosMasklayer->Draw("hist");
		UVWMasklayers.UWPosMasklayer->Draw("SameHist");
		UVWMasklayers.VWPosMasklayer->Draw("SameHist");
		TLegend* posMaskLegend = new TLegend(0.1, 0.7, 0.3, 0.9, "Layers");
		posMaskLegend->AddEntry(UVWMasklayers.UVPosMasklayer, "UV layer");
		posMaskLegend->AddEntry(UVWMasklayers.UWPosMasklayer, "UW layer");
		posMaskLegend->AddEntry(UVWMasklayers.VWPosMasklayer, "WV layer");
		posMaskLegend->Draw();
	}
	
	layerDiffHist diffHist;
	TCanvas layerDiffs("layer diffs", "layer diffs", canvasWidth, h);

	if (sessionOption == PositionTreeSlope) {
		layerDiffs.cd();
		diffHist.uNegLayerDiff = new  TH1D("NegDet", "U layer", 800, -150, 150);
		diffHist.vNegLayerDiff = new  TH1D("NegDet", "V layer", 800, -150, 150);
		diffHist.wNegLayerDiff = new  TH1D("NegDet", "W layer", 800, -150, 150);
		diffHist.vNegLayerDiff->Draw();
	}
	// scans through all files in the folder
	//  parses in all .root files (trees) created by DAQ software
	vector<char*> files;
	if (sessionOption == PositionTreeWrite){

		//Declare trees to write to
		TTree NegDetPositions("Position Info for Detectors", "Tree that stores position info such that calibration can be done without reloading");
		bool ULayer = false;
		bool VLayer = false;
		bool WLayer = false;
		Double_t u1Time, u2Time, v1Time, v2Time, w1Time, w2Time;
		Int_t GroupNumber = 1;

		NegDetPositions.Branch("GroupNumber", &GroupNumber);
		//treePositions.Branch("Detector", &Detector);
		NegDetPositions.Branch("ULayer", &ULayer);
		NegDetPositions.Branch("u1Time", &u1Time);
		NegDetPositions.Branch("u2Time", &u2Time);
		NegDetPositions.Branch("VLayer", &VLayer);
		NegDetPositions.Branch("v1Time", &v1Time);
		NegDetPositions.Branch("v2Time", &v2Time);
		NegDetPositions.Branch("WLayer", &WLayer);
		NegDetPositions.Branch("w1Time", &w1Time);
		NegDetPositions.Branch("w2Time", &w2Time);
		
		TTree PosDetPositions("Position Info for Detectors", "Tree that stores position info such that calibration can be done without reloading");
		PosDetPositions.Branch("GroupNumber", &GroupNumber);
		//treePositions.Branch("Detector", &Detector);
		PosDetPositions.Branch("ULayer", &ULayer);
		PosDetPositions.Branch("u1Time", &u1Time);
		PosDetPositions.Branch("u2Time", &u2Time);
		PosDetPositions.Branch("VLayer", &VLayer);
		PosDetPositions.Branch("v1Time", &v1Time);
		PosDetPositions.Branch("v2Time", &v2Time);
		PosDetPositions.Branch("WLayer", &WLayer);
		PosDetPositions.Branch("w1Time", &w1Time);
		PosDetPositions.Branch("w2Time", &w2Time);
					
		if (dir != NULL) {
			while (pdir = readdir(dir)) {
				char* fileName = pdir->d_name;
				if (strlen(fileName) > 5 && !strcmp(fileName + strlen(fileName) - 5, ".root")) {

					DataSet* data = new DataSet();
					//using string Folder Name acquire intiial tree
					//Initial tree is given by ReMinumber with no underscore number
					//cout << fileName << endl;
					char str[256];
					strcpy(str, fileLocation.c_str());
					strcat(str, fileName);
					//cout << str << endl;
					cout << filenumber << endl;
					filenumber++;

					TFile* rawFile = TFile::Open(str);
					TTree* rawTree = (TTree*)rawFile->Get("T");
					loadFromTreeDataSet(rawTree, data);
					rawFile->Close();
					//Associate hits into events, where event is a single particle/ion hit on the detector. Events are sorted by group
					constructEvents(data);

					// construct timesum histograms on first file
					if (firstFile) {
						timesums = calculateTimeSums(data, userDet);
						firstFile = false;
						//Store fit parameters into a tree for later accessing
						// setting up tree
						TFile file("TimeSumTree.root", "recreate");
						TTree treeTS("TimeSumPeaks", "simple tree that stores timesum peaks and sigma");

						Double_t peak, sigma, error;
						//Will define layer numbers in configuration.h file
						//not sure if text/string can be stored in trees
						Double_t layer;
						treeTS.Branch("Layer", &layer);
						treeTS.Branch("Peak", &peak);
						treeTS.Branch("Sigma", &sigma);
						treeTS.Branch("Error", &error);

						/* CALCULATES AND FITS TIMESUMS BASED ON DETECTORS */
						if (userDet == bothDet) {

							//set up canvas for time sums - 3 for each detector - 6 in total
							//TPad::Divide() specifies number of vertical and horizontal slices of canvas
							timesumsCanvas.Divide(2, 3);

							timesumsCanvas.cd(1);
							timesums.layer_upos->Draw();
							timesums.layer_upos->Fit("gaus");
							gStyle->SetOptFit(0011);
							TF1 *fitu = timesums.layer_upos->GetFunction("gaus");
							layer = CFG_LAYER_UPOS;
							peak = fitu->GetParameter(1);
							sigma = fitu->GetParameter(2);
							error = fitu->GetParError(1);
							treeTS.Fill();
							fits.setFit(u, positive, peak, error, sigma);
							//Want to display fit parameters on timesums plots
							//timesums.layer_upos->SetOptFit();

							timesumsCanvas.cd(2);
							timesums.layer_vpos->Draw();
							timesums.layer_vpos->Fit("gaus");
							TF1 *fitv = timesums.layer_vpos->GetFunction("gaus");
							layer = CFG_LAYER_VPOS;
							peak = fitv->GetParameter(1);
							sigma = fitv->GetParameter(2);
							error = fitv->GetParError(1);
							treeTS.Fill();
							fits.setFit(v, positive, peak, error, sigma);

							timesumsCanvas.cd(3);
							timesums.layer_wpos->Draw();
							timesums.layer_wpos->Fit("gaus");
							TF1 *fitw = timesums.layer_wpos->GetFunction("gaus");
							layer = CFG_LAYER_WPOS;
							peak = fitw->GetParameter(1);
							sigma = fitw->GetParameter(2);
							error = fitw->GetParError(1);
							treeTS.Fill();
							fits.setFit(w, positive, peak, error, sigma);

							timesumsCanvas.cd(4);
							timesums.layer_uneg->Draw();
							timesums.layer_uneg->Fit("gaus");
							TF1 *fitun = timesums.layer_uneg->GetFunction("gaus");
							layer = CFG_LAYER_UNEG;
							peak = fitun->GetParameter(1);
							sigma = fitun->GetParameter(2);
							error = fitun->GetParError(1);
							treeTS.Fill();
							fits.setFit(u, negative, peak, error, sigma);

							timesumsCanvas.cd(5);
							timesums.layer_vneg->Draw();
							timesums.layer_vneg->Fit("gaus");
							TF1 *fitvn = timesums.layer_vneg->GetFunction("gaus");
							layer = CFG_LAYER_VNEG;
							peak = fitvn->GetParameter(1);
							sigma = fitvn->GetParameter(2);
							error = fitvn->GetParError(1);
							treeTS.Fill();
							fits.setFit(v, negative, peak, error, sigma);

							timesumsCanvas.cd(6);
							timesums.layer_wneg->Draw();
							timesums.layer_wneg->Fit("gaus");
							TF1 *fitwn = timesums.layer_wneg->GetFunction("gaus");
							layer = CFG_LAYER_WNEG;
							peak = fitwn->GetParameter(1);
							sigma = fitwn->GetParameter(2);
							error = fitwn->GetParError(1);
							treeTS.Fill();
							fits.setFit(w, negative, peak, error, sigma);
							timesumsCanvas.Modified();
							timesumsCanvas.Update();
						}

						else if (userDet == posDet) {
							//set up canvas for time sums - 3 for each detector - therefore only 3 in one det in use
							//TPad::Divide() specifies number of vertical and horizontal slices of canvas
							timesumsCanvas.Divide(1, 3);

							timesumsCanvas.cd(1);
							timesums.layer_upos->Draw();
							timesums.layer_upos->Fit("gaus");
							gStyle->SetOptFit(0011);
							TF1 *fitu = timesums.layer_upos->GetFunction("gaus");
							layer = CFG_LAYER_UPOS;
							peak = fitu->GetParameter(1);
							sigma = fitu->GetParameter(2);
							error = fitu->GetParError(1);
							treeTS.Fill();
							fits.setFit(u, positive, peak, error, sigma);
							//Want to display fit parameters on timesums plots
							//timesums.layer_upos->SetOptFit();

							timesumsCanvas.cd(2);
							timesums.layer_vpos->Draw();
							timesums.layer_vpos->Fit("gaus");
							TF1 *fitv = timesums.layer_vpos->GetFunction("gaus");
							layer = CFG_LAYER_VPOS;
							peak = fitv->GetParameter(1);
							sigma = fitv->GetParameter(2);
							error = fitv->GetParError(1);
							treeTS.Fill();
							fits.setFit(v, positive, peak, error, sigma);

							timesumsCanvas.cd(3);
							timesums.layer_wpos->Draw();
							timesums.layer_wpos->Fit("gaus");
							TF1 *fitw = timesums.layer_wpos->GetFunction("gaus");
							layer = CFG_LAYER_WPOS;
							peak = fitw->GetParameter(1);
							sigma = fitw->GetParameter(2);
							error = fitw->GetParError(1);
							treeTS.Fill();
							fits.setFit(w, positive, peak, error, sigma);
							timesumsCanvas.Modified();
							timesumsCanvas.Update();
						}

						else if (userDet == negDet) {
							//detmust be negative
							//set up canvas for time sums - 3 for each detector - therefore only 3 in one det in use
							//TPad::Divide() specifies number of vertical and horizontal slices of canvas
							timesumsCanvas.Divide(1, 3);
							timesumsCanvas.cd(1);
							timesums.layer_uneg->Draw();
							timesums.layer_uneg->Fit("gaus");
							TF1 *fitun = timesums.layer_uneg->GetFunction("gaus");
							layer = CFG_LAYER_UNEG;
							peak = fitun->GetParameter(1);
							sigma = fitun->GetParameter(2);
							error = fitun->GetParError(1);
							treeTS.Fill();
							fits.setFit(u, negative, peak, error, sigma);

							timesumsCanvas.cd(2);
							timesums.layer_vneg->Draw();
							timesums.layer_vneg->Fit("gaus");
							TF1 *fitvn = timesums.layer_vneg->GetFunction("gaus");
							layer = CFG_LAYER_VNEG;
							peak = fitvn->GetParameter(1);
							sigma = fitvn->GetParameter(2);
							error = fitvn->GetParError(1);
							treeTS.Fill();
							fits.setFit(v, negative, peak, error, sigma);

							timesumsCanvas.cd(3);
							timesums.layer_wneg->Draw();
							timesums.layer_wneg->Fit("gaus");
							TF1 *fitwn = timesums.layer_wneg->GetFunction("gaus");
							layer = CFG_LAYER_WNEG;
							peak = fitwn->GetParameter(1);
							sigma = fitwn->GetParameter(2);
							error = fitwn->GetParError(1);
							treeTS.Fill();
							fits.setFit(w, negative, peak, error, sigma);
							timesumsCanvas.Modified();
							timesumsCanvas.Update();
						}

						//Want to write timesum information to tree for accessing later in program, also to save to csv such that
						//ts info for all runs can be accessed at later dates without rerunning code
						treeTS.Write();
						
					}

					//Checks timesums are within 2sigma of fitted peak
					checkTimeSums(data, fits, userDet);

					//takes timedifferences of MCP hits for each event within a group
					//identifies particle and stores in MCPTDiff double
					//stored is the particleID and timedifference relative to positron
					//identifyAndTime(data);

					// want to index number of particles and bool triple for each each group
					//selectPosIonElec(data);

					//check each particle hit has enough information to reconstruct X Y position
					//not needed for ion
					//does not need userDet check
					checkReconstructable(data);

					//now need to check for all three particles (ion, pos, elec) and that the pos and elec have a reconBool

					//Copy over triple coincidences with reconstrutable particle hits to new dataset

					//does not need userDet
					DataSet *reconData = sortReconData(data);
					/**
					* Convert time to position info
					*First off need to get U,V,W from u1 u2, v1 v2, w1 w2
					*/

					/*
					*WRITE POSITIONS TO TREE
					*ELIMINATE PROCESSING TIME DURING CALIBRATION
					*/
					if (userDet == posDet) {

					}
					else if (userDet == negDet) {
						//Store fit parameters into a tree for later accessing
						// setting up tree
						//Ideally want to store tree with filename

						for (Group* g : *reconData) {
							for (Event* e : g->events) {
								if (e->mcp->detector == neg) {
									if (e->uPairs.size() == 1) {
										ULayer = true;
										u1Time = e->uPairs.front().line1;
										u2Time = e->uPairs.front().line2;
									}
									else {
										ULayer = false;
										u1Time = 0;
										u2Time = 0;
									}
									if (e->vPairs.size() == 1) {
										VLayer = true;
										v1Time = e->vPairs.front().line1;
										v2Time = e->vPairs.front().line2;
									}
									else {
										VLayer = false;
										v1Time = 0;
										v2Time = 0;
									}
									if (e->wPairs.size() == 1) {
										WLayer = true;
										w1Time = e->wPairs.front().line1;
										w2Time = e->wPairs.front().line2;
									}
									else {
										WLayer = false;
										w1Time = 0;
										w2Time = 0;
									}
									NegDetPositions.Fill();
								}
							}
						}
						GroupNumber++;
					}
					else if (userDet == bothDet) {

					}

					convertLayerPosition(reconData, Pitches, userDet, &UVWPositions);

					convertCartesianPosition(reconData, userDet, &XYpositions, &UVWlayers, &UVWMasklayers);

					//histogram detector images with 2D histogram
					//can have userDet implementation, not currently implemented
					//histogramXYPositions(reconData, XYpositions);

					//draw the detector images
					//histogramElectronLayers(reconData, &UVWlayers, userDet);

					//histogramMaskLayers(reconData, &UVWMasklayers);

					//needed to make root canvases interactive 
					//Lives updates the graphs
					layersCanvas.Modified();
					layersCanvas.Update();
					XYPosDet.Modified();
					XYPosDet.Update();
					XYNegDet.Modified();
					XYNegDet.Update();
					UVWNeglayersCanvas.Modified();
					UVWNeglayersCanvas.Update();
					UVWNegMaskLayersCanvasX.Modified();
					UVWNegMaskLayersCanvasX.Update();
					UVWNegMaskLayersCanvasY.Modified();
					UVWNegMaskLayersCanvasY.Update();
					UVWPoslayersCanvas.Modified();
					UVWPoslayersCanvas.Update();
					UVWPosMaskLayersCanvas.Modified();
					UVWPosMaskLayersCanvas.Update();

					rootapp->Draw();

					delete data;
				}
			}
			TFile rawPositionFile("RawPositionInfoTree.root", "recreate");
			NegDetPositions.Write();
		}
	}
	//else {
		//cout << "Directory does not exist, exiting" << endl;
		//exit(1);
	//}

	closedir(dir);
	/*LOAD FROM TREE OPTION*/
	if (sessionOption == PositionTreeErrorCalibrate) {
		//attempts to calibrate detector by minimising error function
		DataSet* reconData = new DataSet();
		TFile* positionsFile = TFile::Open("C:/Users/Tamara/Documents/GitHub/CalibrateDetectors/app/RawPositionInfoTree.root");
		TTree* positionsTree = (TTree*)positionsFile->Get("Position Info for Detectors");
		if (positionsFile == 0) {
			cout << "File not found" << endl;
		}
		if (positionsTree == 0) {
			cout << "no tree" << endl;
		}

		positionsTreeToDataSet(positionsTree, reconData, userDet);
		positionsFile->Close();

		convertLayerPosition(reconData, Pitches, userDet);

		convertCartesianPosition(reconData, userDet);

		// add a neighbour count to each event in the dataset
		checkDensity(reconData, userDet);

		// 1. make a new th1i called densityHist at the top of the code to put the density in

		// put the density (neighbour count) from each event into the histogram
		//histogramDensity(reconData, userDet, &densityHist);

		/*			//only works for negative detector, as cartesian reconstruction varies with detector
		PitchPropSet calibrated;
		PitchPropData params = getCalibrationParameters(reconData, Pitches, userDet);

		if (userDet == negDet) {
		calibrated.setPitchProp(negative, params);
		}
		if (userDet == posDet) {
		calibrated.setPitchProp(positive, params);
		}

		convertLayerPosition(reconData, calibrated, userDet);

		convertCartesianPosition(reconData, userDet);

		convertLayerPosition(reconData, calibrated, userDet, &UVWPositions);

		convertCartesianPosition(reconData, userDet, &XYpositions, &UVWlayers, &UVWMasklayers); */

		//histogramElectronLayers(reconData, &UVWlayers, userDet);

		//histogramMaskLayers(reconData, &UVWMasklayers);

		double scaleUV = 1.0 / UVWMasklayers.UVMasklayer->GetMaximum();
		//double scaleUV = 1.0 / UVWMasklayers.UVMasklayer->Integral(0,200);
		UVWMasklayers.UVMasklayer->Scale(scaleUV);
		double scaleUW = 1.0 / UVWMasklayers.UWMasklayer->GetMaximum();
		//double scaleUW = 1.0 / UVWMasklayers.UWMasklayer->Integral(0,200);
		UVWMasklayers.UWMasklayer->Scale(scaleUW);
		double scaleVW = 1.0 / UVWMasklayers.VWMasklayer->GetMaximum();
		//double scaleVW = 1.0 / UVWMasklayers.VWMasklayer->Integral(0,200);
		UVWMasklayers.VWMasklayer->Scale(scaleVW);

		//Lives updates the graphs
		layersCanvas.Modified();
		layersCanvas.Update();
		XYPosDet.Modified();
		XYPosDet.Update();
		XYNegDet.Modified();
		XYNegDet.Update();
		UVWNeglayersCanvas.Modified();
			UVWNeglayersCanvas.Update();
			UVWNegMaskLayersCanvasX.Modified();
			UVWNegMaskLayersCanvasX.Update();
			UVWNegMaskLayersCanvasY.Modified();
			UVWNegMaskLayersCanvasY.Update();
			UVWPoslayersCanvas.Modified();
			UVWPoslayersCanvas.Update();
			UVWPosMaskLayersCanvas.Modified();
			UVWPosMaskLayersCanvas.Update();

			rootapp->Draw();
		}
	/*LOAD FROM TREE OPTION*/
	if (sessionOption == PositionTreeRead) {
			//loads from rawpositionstrees
			//so far no calibration implemented
			DataSet* reconData = new DataSet();
			TFile* positionsFile = TFile::Open("C:/Users/Tamara/Documents/GitHub/CalibrateDetectors/app/RawPositionInfoTree.root");
			TTree* positionsTree = (TTree*)positionsFile->Get("Position Info for Detectors");
			if (positionsFile == 0) {
				cout << "File not found" << endl;
			}
			if (positionsTree == 0) {
				cout << "No position tree could be loaded" << endl;
			}

			positionsTreeToDataSet(positionsTree, reconData, userDet);
			positionsFile->Close();

			convertLayerPosition(reconData, Pitches, userDet, &UVWPositions);

			convertCartesianPosition(reconData, userDet, &XYpositions, &UVWlayers, &UVWMasklayers); 

			if (userDet == negDet) {
				double scaleUV = 1.0 / UVWMasklayers.UVMasklayer->GetMaximum();
				UVWMasklayers.UVMasklayer->Scale(scaleUV);
				double scaleUW = 1.0 / UVWMasklayers.UWMasklayer->GetMaximum();
				UVWMasklayers.UWMasklayer->Scale(scaleUW);
				double scaleVW = 1.0 / UVWMasklayers.VWMasklayer->GetMaximum();
				UVWMasklayers.VWMasklayer->Scale(scaleVW);

				double scaleUVY = 1.0 / UVWMasklayers.UVMasklayerY->GetMaximum();
				UVWMasklayers.UVMasklayerY->Scale(scaleUVY);
				double scaleUWY = 1.0 / UVWMasklayers.UWMasklayerY->GetMaximum();
				UVWMasklayers.UWMasklayerY->Scale(scaleUWY);
				double scaleVWY = 1.0 / UVWMasklayers.VWMasklayerY->GetMaximum();
				UVWMasklayers.VWMasklayerY->Scale(scaleVWY);

				double scaleUnegpositions = 1.0 / UVWPositions.UNegPositions->GetMaximum();
				UVWPositions.UNegPositions->Scale(scaleUnegpositions);
				double scaleVnegpositions = 1.0 / UVWPositions.VNegPositions->GetMaximum();
				UVWPositions.VNegPositions->Scale(scaleVnegpositions);
				double scaleWnegpositions = 1.0 / UVWPositions.WNegPositions->GetMaximum();
				UVWPositions.WNegPositions->Scale(scaleWnegpositions);

			}
			
			//Lives updates the graphs
			layersCanvas.Modified();
			layersCanvas.Update();
			XYPosDet.Modified();
			XYPosDet.Update();
			XYNegDet.Modified();
			XYNegDet.Update();
			UVWNeglayersCanvas.Modified();
			UVWNeglayersCanvas.Update();
			UVWNegMaskLayersCanvasX.Modified();
			UVWNegMaskLayersCanvasX.Update();
			UVWNegMaskLayersCanvasY.Modified();
			UVWNegMaskLayersCanvasY.Update();
			UVWPoslayersCanvas.Modified();
			UVWPoslayersCanvas.Update();
			UVWPosMaskLayersCanvas.Modified();
			UVWPosMaskLayersCanvas.Update();

			rootapp->Draw();
		}

	if (sessionOption == PositionTreeSlope) {
		//loads from rawpositionstrees
		//so far no calibration implemented
		DataSet* reconData = new DataSet();
		TFile* positionsFile = TFile::Open("C:/Users/Tamara/Documents/GitHub/CalibrateDetectors/app/RawPositionInfoTree.root");
		TTree* positionsTree = (TTree*)positionsFile->Get("Position Info for Detectors");
		if (positionsFile == 0) {
			cout << "File not found" << endl;
		}
		if (positionsTree == 0) {
			cout << "No position tree could be loaded" << endl;
		}

		positionsTreeToDataSet(positionsTree, reconData, userDet);
		positionsFile->Close();

		convertLayerPosition(reconData, Pitches, userDet);

		convertCartesianPosition(reconData, userDet);

		calibrateFromSlope(reconData, userDet, &diffHist);

		layerDiffs.Modified();
		layerDiffs.Update();

		}

		rootapp->Draw();


			//bin 0 to 200

			//double scaleUV = 1.0 / UVWMasklayers.UVPosMasklayer->GetMaximum();
			//double scaleUV = 1.0 / UVWMasklayers.UVPosMasklayer->Integral(0,200);
			//UVWMasklayers.UVPosMasklayer->Scale(scaleUV);
			//double scaleUW = 1.0 / UVWMasklayers.UWPosMasklayer->GetMaximum();
			//double scaleUW = 1.0 / UVWMasklayers.UWPosMasklayer->Integral(0,200);
			//UVWMasklayers.UWPosMasklayer->Scale(scaleUW);
			//double scaleVW = 1.0 / UVWMasklayers.VWPosMasklayer->GetMaximum();
			//double scaleVW = 1.0 / UVWMasklayers.VWPosMasklayer->Integral(0,200);
			//UVWMasklayers.VWPosMasklayer->Scale(scaleVW);
			//UVWPosMaskLayersCanvas.Modified();
			//UVWPosMaskLayersCanvas.Update();

		/*Fitting Guassians to interesting areas on the 1d Hist (detector slice)
		Dealing with canvas 7
		Finding bins from an axis value
		You can use TAxis::FindBin
		Example in case of a 2-d histogram (same for 1-d and 3-d)
		TAxis *xaxis = h->GetXaxis();
		TAxis *yaxis = h->GetYaxis();
		Int_t binx = xaxis->FindBin(x);
		Int_t biny = yaxis->FindBin(y);*/
		//TAxis *UVx = UVWMasklayers.UVMasklayer->GetXaxis();
		//TAxis *UVy = UVWMasklayers.UVMasklayer->GetYaxis();


	rootapp->Run();

			//should print out metafile of setup, AND csv dump of data. 

	return 0;
		
}

