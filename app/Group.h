#ifndef GROUP_H
#define GROUP_H

#include "stdafx.h"
#include <vector>
#include <string>
#include "Hit.h"
#include "Event.h"
#include "TripleCheck.h"
#include "Particle.h"

/*
	Contains information on several hits within 500ns of
	an MCP Hit
*/
struct Set
{
	ParticleID id;// = ParticleID_unknown;
	vector<Hit> v_mcp, v_x1, v_x2, v_y1, v_y2, v_z1, v_z2;
	Hit mcp, x1, x2, y1, y2, z1, z2;
};

//struct TripleCheck {
	//double PosCheck;
	//double ElecCheck;
	//double Ion1Check;
	//double Ion2Check;
	//double UnidentCheck;
	//bool TripleCoinc;
//};

class Group {
private:
    //Numerical id for group generated by TDC
    int id;

	//Final particle information
	//Particle electron, positron, ion;

	//Raw data of Hits from file
	vector<Hit*> rawData;

	
public:
	//Constructor - do everything you need to when you create a Group object
	Group(int id);
	Group();
	
	//Events
 	vector<Event*> events;

	//Destructor - do everything you need to when you destroy a Group object
	~Group();

	void addHit(Hit* h);
	void addEvent(Event* e);

	int getId();

	vector<Hit*>* getHits();

	// iterator is a generic way to access any data structure that c defines, special class in c++ that is like a pointer to some data
	// lets the complier know that the class is a container that holds Hit*s
	// allows for easy looping through Group
	vector<Hit*>::iterator begin();
	vector<Hit*>::iterator end();
	void storeTripleCheck(double PosCheck, double ElecCheck, double Ion1Check, double Ion2Check, double OtherCheck, bool TripleCoinc);

	TripleCheck particles;

	double PosCheck;
	double ElecCheck;
	double Ion1Check;
	double Ion2Check;
	double UnidentCheck;
	bool TripleCoinc;

	Particle electron;
	Particle positron;
	Particle ion;
	//For detector images
};
#endif
