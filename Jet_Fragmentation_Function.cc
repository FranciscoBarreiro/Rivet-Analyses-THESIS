// -*- C++ -*-
#include "Rivet/Analyses/MC_JetAnalysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "Rivet/Projections/HepMCHeavyIon.hh"

#include <fstream>

namespace Rivet {

/// @brief Analysis to study geometrical bias in heavy ion collisions
class Jet_Fragmentation_Function : public MC_JetAnalysis {
public:

	Jet_Fragmentation_Function()
	: MC_JetAnalysis("Jet_Fragmentation_Function", 4, "Jets")
	{	}


	void init() {
		
		// project all final state particles
		FinalState fs(Cuts::abseta < 2.0 && Cuts::abseta > 0.3);

		FastJets jetpro(fs, FastJets::ANTIKT, 0.3);
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

		_file<<"zeta      Weight"<<endl;

		return;
	}


	void analyze(const Event& event) {		
		// find the two most energetic jets
		const Jets &jetlist = apply<FastJets>(event, "Jets").jetsByPt(Cuts::abseta < 2.0 && Cuts::abseta > 0.3 && Cuts::pT > 100*GeV && Cuts::pT < 300*GeV);
		double delta_R = 0.;
		if(jetlist.size() == 0) vetoEvent;
		for(const Jet& j : jetlist) {
            for(const Particle& k : j.particles()){
				delta_R = sqrt((k.momentum().phi()-j.momentum().phi())*(k.momentum().phi()-j.momentum().phi())+(k.momentum().eta()-j.momentum().eta())*(k.momentum().eta()-j.momentum().eta()));
                _file<< log((j.momentum().pT())/(k.momentum().pT()*cos(delta_R))) <<"      "<< event.weights()[0] <<endl;   
            }
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
DECLARE_RIVET_PLUGIN(Jet_Fragmentation_Function);

}