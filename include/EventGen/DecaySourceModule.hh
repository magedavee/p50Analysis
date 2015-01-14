/// \file DecaySourceModule.hh \brief Nuclear decay chain event generator.
#ifndef DECAYSOURCEMODULE
#define DECAYSOURCEMODULE

#include "PrimaryGeneratorAction.hh"
#include "NuclEvtGen.hh"

#include <set>
using std::set;

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithABool.hh>

/// Nuclear decay calibration source generator module
class DecaySourceModule: public PrimaryGeneratorModule, public G4UImessenger {
public:
    /// Constructor
    DecaySourceModule(PrimaryGeneratorAction* P);
    
    /// UI response
    void SetNewValue(G4UIcommand*,G4String);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    /// get generator time
    virtual G4double GetGeneratorTime() const;
    
    set<int> ptypes;                    ///< particle types (PDG ID) to throw; {} for all
    bool throwSingle = false;           ///< whether to only throw one particle at a time
    
protected:
    
    NucDecayLibrary NDL;                ///< decay information tables
    string gen_name;                    ///< generator name
    
    G4UIdirectory decaySrcDir;          ///< UI directory for control commands
    G4UIcmdWithAString genNameCmd;      ///< generator selection command
    G4UIcmdWithAnInteger ptclCmd;       ///< particle type selection command
    G4UIcmdWithABool singleCmd;         ///< command for setting throwSingle
    
    vector<NucDecayEvent> evtq;         ///< events waiting to be thrown
    size_t nDecays = 0;                 ///< number of decays
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
