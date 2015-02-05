/// \file LLNLFissModule.hh \brief Interface to LLNL fission event generator
#ifndef LLNLFISSMODULE_HH
/// Assure header is only loaded once
#define LLNLFISSMODULE_HH

#include "PrimaryGeneratorAction.hh"

#include <G4UImessenger.hh>
#include <G4UIcmdWithAnInteger.hh>

/// Event generator module with interface to LLNL fission event generator.
/// see "Fission" at http://nuclear.llnl.gov/simulation/ ,
/// though I don't think the current Geant4 actually includes
/// the more advanced "FREYA" correlated decay model.
class LLNLFissModule: public PrimaryGeneratorModule, public G4UImessenger {
public:
    /// Constructor
    LLNLFissModule(PrimaryGeneratorAction* P);
  
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

    /// UI response
    void SetNewValue(G4UIcommand*,G4String);
    
protected:
    
    G4int isotope = 0;                  ///< isotope for fission
    G4UIdirectory fissDir;              ///< UI directory for fission generator commands
    G4UIcmdWithAnInteger isotCmd;       ///< UI command for setting isotope
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
