#ifndef DECAYSOURCEMODULE
#define DECAYSOURCEMODULE

#include "PrimaryGeneratorAction.hh"
#include "NuclEvtGen.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>

/// Nuclear decay calibration source generator module
class DecaySourceModule: public PrimaryGeneratorModule, public G4UImessenger {
public:
    /// Constructor
    DecaySourceModule(PrimaryGeneratorAction* P);
    
    /// UI response
    void SetNewValue(G4UIcommand*,G4String);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
        
protected:
    
    NucDecayLibrary NDL;                ///< decay information tables
    string gen_name;                    ///< generator name
    
    G4UIdirectory decaySrcDir;          ///< UI directory for control commands
    G4UIcmdWithAString genNameCmd;      ///< generator selection command
    vector<NucDecayEvent> evtq;         ///< events waiting to be thrown
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
