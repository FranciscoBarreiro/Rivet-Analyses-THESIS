// -*- C++ -*-
#include "Rivet/Analyses/MC_JetAnalysis.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "Rivet/Projections/HepMCHeavyIon.hh"
#include "Rivet/Projections/JetShape.hh"

#include <fstream>
#include <map>

namespace Rivet {

/// @brief Analysis to study geometrical bias in heavy ion collisions
class JetProfile : public MC_JetAnalysis {
public:

	JetProfile()
	: MC_JetAnalysis("JetProfile", 4, "Jets")
	{	}


	void init() {
		
		// project all final state particles
		FinalState fs(Cuts::abseta < 2.0);

		FastJets jetpro(fs, FastJets::ANTIKT, 0.3); //radius of jet reconstruction = 0.3
		declare(jetpro, "Jets");

		const JetShape jsp(jetpro,0.0,0.6,12.0,10.0,DBL_MAX,0.0,4.0,PSEUDORAPIDITY); //should it be RAPIDITY or PSEUDORAPIDITY ?? Don't know!!!
		declare(jsp,"JetShapes");

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

		_file<<"r     Jet_Profile     Weight"<<endl; //What are the units of r?

		return;
	}


	void analyze(const Event& event) {		
		// find the two most energetic jets
		const Jets &jetlist = apply<FastJets>(event, "Jets").jetsByPt(Cuts::abseta < 2.0 && Cuts::pT > 100*GeV);

		if(jetlist.size() == 0) vetoEvent;
		const JetShape& jsipt = apply<JetShape>(event,"JetShapes");
		for(size_t ijet=0;ijet < jsipt.numJets();++ijet){
			for(size_t rbin=0; rbin < jsipt.numBins(); ++rbin){
				_file<<jsipt.rBinMid(rbin)<<"     "<<jsipt.diffJetShape(ijet,rbin)<<"     "<< event.weights()[0] <<endl;
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
DECLARE_RIVET_PLUGIN(JetProfile);

}