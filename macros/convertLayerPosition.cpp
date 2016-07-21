#include <iostream>
#include "DataSet.h"
#include "stdafx.h"
#include "Group.h"
#include "PitchPropSet.h"
#include "PitchData.h"
#include "math.h"
#include "Constants.h"
#include "HistUVWPositions.h"

//converts from timing u1 u2 to U
//uses pitch propogation factor to convert timing info to position info
//stills need to be converted to cartesian after this
void convertLayerPosition(DataSet* reconData, PitchPropSet Pitches, imagingDetectors userDet, UVWpositionsHist *UVWPositions) {
	//retrieve pitch propagation calculations
	//these are same for all events/groups
	PitchPropData posPitches = Pitches.getPitchProp(positive);
	PitchPropData negPitches = Pitches.getPitchProp(negative);

	if (userDet == bothDet) {
		for (Group* g : *reconData) {
			for (Event* e : g->events) {
				if (e->mcp->detector == pos) {
					if (e->uPairs.size() == 1) {
						e->U = (posPitches.ulayer / 2)*(e->uPairs.front().line1 - e->uPairs.front().line2);
						//cout << e->uPairs.front().line1 << endl;
						//cout << "U layer: " << e->U << endl;
						UVWPositions->UPosPositions->Fill(e->U);
					}
					if (e->vPairs.size() == 1) {
						e->V = (posPitches.vlayer / 2)*(e->vPairs.front().line1 - e->vPairs.front().line2);
						//cout << "V layer: " << e->V << endl;
						UVWPositions->VPosPositions->Fill(e->V);
					}
					if (e->wPairs.size() == 1) {
						e->W = (posPitches.wlayer / 2)*(e->wPairs.front().line1 - e->wPairs.front().line2);
						//cout << "W layer: " << e->W << endl;
						UVWPositions->WPosPositions->Fill(e->W);
					}
				}
				if (e->mcp->detector == neg) {
					//see logbook 9 page 114 for diagram of electron detector
					//including gap sizes
					if (e->uPairs.size() == 1) {
						//line2 - line 1 as u1 and u2 reversed (u2 on right hand side) x = righthandside-lefthandside
						double Unogap = (negPitches.ulayer / 2)*(e->uPairs.front().line1 - e->uPairs.front().line2);
						//cout << "U layer: " << Unogap << endl;
						if (Unogap < -1) {
							e->U = Unogap;// -(9 / 2);
						}
						else if (Unogap > -1) {
							e->U = Unogap;// +(9 / 2);
						}
						UVWPositions->UNegPositions->Fill(e->U);
					}
					if (e->vPairs.size() == 1) {
						double Vnogap = (negPitches.vlayer / 2)*(e->vPairs.front().line1 - e->vPairs.front().line2);
						//cout << "V layer: " << Vnogap << endl;
						if (Vnogap < 0) {
							e->V = Vnogap;// -(8 / 2);
						}
						else {
							e->V = Vnogap;// +(8 / 2);
						}
						UVWPositions->VNegPositions->Fill(e->V);
					}
					if (e->wPairs.size() == 1) {
						//line2 0 lin1 due to electronics configuration
						double Wnogap = (negPitches.wlayer / 2)*(e->wPairs.front().line1 - e->wPairs.front().line2);
						//cout << "W layer: " << Wnogap << endl;
						if (Wnogap < 0.5) {
							e->W = Wnogap;// -(8 / 2);
						}
						else {
							e->W = Wnogap;// +(8 / 2);
						}
						UVWPositions->WNegPositions->Fill(e->W);
					}
				}
			}
		}
	}
	else if (userDet == posDet) {
		for (Group* g : *reconData) {
			for (Event* e : g->events) {
				if (e->mcp->detector == pos) {
					if (e->uPairs.size() == 1) {
						e->U = (posPitches.ulayer / 2)*(e->uPairs.front().line1 - e->uPairs.front().line2);
						//cout << e->uPairs.front().line1 << endl;
						//cout << "U layer: " << e->U << endl;
						UVWPositions->UPosPositions->Fill(e->U);
					}
					if (e->vPairs.size() == 1) {
						e->V = (posPitches.vlayer / 2)*(e->vPairs.front().line1 - e->vPairs.front().line2);
						//cout << "V layer: " << e->V << endl;
						UVWPositions->VPosPositions->Fill(e->V);
					}
					if (e->wPairs.size() == 1) {
						e->W = (posPitches.wlayer / 2)*(e->wPairs.front().line1 - e->wPairs.front().line2);
						//cout << "W layer: " << e->W << endl;
						UVWPositions->WPosPositions->Fill(e->W);
					}
				}
			}
		}
	}
	else if (userDet == negDet) {
		//det must be negative
		//cout << "neg detector detected in convertlayerpostion" << endl;
		for (Group* g : *reconData) {
			for (Event* e : g->events) {
				if (e->mcp->detector == neg) {
					//see logbook 9 page 114 for diagram of electron detector
					//including gap sizes
					if (e->uPairs.size() == 1) {
						//line2 - line 1 as u1 and u2 reversed (u2 on right hand side) x = righthandside-lefthandside
						//double Unogap = (negPitches.ulayer / 2)*(e->uPairs.front().line1 - e->uPairs.front().line2);
						//cout << "U layer: " << Unogap << endl;
						double Unogap = (negPitches.ulayer / 2)*(e->uPairs.front().line1 - e->uPairs.front().line2) + 2.65;// +2.55 + 0.2;
						//if (Unogap < 0 && Unogap>-3) {
							//cout << Unogap << endl;
						//}
						if (Unogap < -1) {
							e->U = Unogap-(7/2);// -(9 / 2);
						}
						else {
							e->U = Unogap+(7/2);// +(9 / 2);
						}
						UVWPositions->UNegPositions->Fill(e->U);
						//cout << "U layer: " << e->U << endl;
					}
					if (e->vPairs.size() == 1) {
						double Vnogap = (negPitches.vlayer / 2)*(e->vPairs.front().line1 - e->vPairs.front().line2)+0.75;
						//cout << "V layer: " << Vnogap << endl;
						if (Vnogap < -0.5) {
							e->V = Vnogap - (4);// -(8 / 2);
						}
						else {
							e->V = Vnogap + (4);// +(8 / 2);
						}
						UVWPositions->VNegPositions->Fill(e->V);
						//cout << "V layer: " << e->V << endl;
					}
					if (e->wPairs.size() == 1) {
						//line2 0 lin1 due to electronics configuration
						double Wnogap = (negPitches.wlayer / 2)*(e->wPairs.front().line1 - e->wPairs.front().line2)+1.05;
						//cout << "W layer: " << Wnogap << endl;
						if (Wnogap < -0.5) {
							e->W = Wnogap -(7 / 2);// -(8 / 2);
						}
						else {
							e->W = Wnogap +(7 / 2);// +(8 / 2);
						}
						UVWPositions->WNegPositions->Fill(e->W);
						//cout << "W layer: " << e->W << endl;
					}
				}
			}
		}
	}
	else {
		cout << "no detector detected in convert layer position" << endl;
	}
}