// -*- C++ -*-
#include "Rivet/Analyses/MC_JetAnalysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "Rivet/Projections/HepMCHeavyIon.hh"

#include <fstream>
#include <map>


namespace Rivet {

/// @brief Analysis to study geometrical bias in heavy ion collisions
class JetTest : public MC_JetAnalysis {
public:

	JetTest()
	: MC_JetAnalysis("JetTest", 4, "Jets")
	{	}


	void init() {
		
		// project all final state particles
		FinalState fs(Cuts::abseta < 2.8);

		FastJets jetpro(fs, FastJets::ANTIKT, 0.4);
		declare(jetpro, "Jets");

		// variable initializations

		// init method from inherited class
		MC_JetAnalysis::init();

		// get file name
		string name;
		std::cout << std::endl << std::endl << "Out file name: ";
		std::cin >> name;
		if(name == "e\n") name = "eventdata.dat";
		std::cout << std::endl;

		// open file for writing
		_file.open(name, std::fstream::out);
		if(_file.is_open()) {
			_open = true; 
		} 
		else { 
			_open = false; 
			std::cout << "[WARNING] Could not open file." << std::endl;
		}

		_file<<"pT(GeV)      weights"<<endl;

		return;
	}


	void analyze(const Event& event) {		
		// find the two most energetic jets
		const Jets &jetlist = apply<FastJets>(event, "Jets").jetsByPt(Cuts::abseta < 2.8 && Cuts::pT > 20*GeV);

		if(jetlist.size() == 0) vetoEvent;

		for(const Jet& j : jetlist) {
			_file<<j.momentum().pT()<<"      "<< event.weights()[0] <<endl;
		}

		// analyse method from inherited class
		MC_JetAnalysis::analyze(event);

		return;
	}


	void finalize() {

		// finalize method from inherited class
		MC_JetAnalysis::finalize();

		// close file
		if(_open) {
			_file.close();
			std::cout << "Everything written to file." << std::endl;
		}

		return;
	}




private:

	// class variables
	std::fstream _file;
	bool _open;
	

};

// The hook for the plugin system
DECLARE_RIVET_PLUGIN(JetTest);

}