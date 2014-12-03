#ifndef COSMICNEUTRONMODULE_HH
/// Assure headers are only loaded once
#define COSMICNEUTRONMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SatoNiitaNeutrons.hh"

#include <TH1F.h>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

/// Cosmic neutron event generator module
class CosmicNeutronModule: public PrimaryGeneratorModule, public G4UImessenger, protected SatoNiitaNeutrons {
public:
    /// Constructor
    CosmicNeutronModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    /// get total "real time" for primaries thrown
    virtual G4double GetGeneratorTime() const;

    /// UI response
    void SetNewValue(G4UIcommand*,G4String);
    
protected:
    /// clear previous distribution
    void resetDistribution() { if(myDist) delete myDist; myDist = NULL; }
    /// generate distribution histogram
    void makeDistribution();
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    TH1F* myDist;       ///< underlying energy distribution
    double netFlux;     ///< total flux
    double thermalFlux; ///< thermal peak flux
    
    G4UIdirectory cosn_dir;             ///< UI directory for cosmic neutron generator controls
    G4UIcmdWithADouble tscale_cmd;      ///< UI command for thermal neutron rescaling
    G4UIcmdWithADouble sscale_cmd;      ///< UI command for non-thermal neutron rescaling
    G4UIcmdWithADouble smod_cmd;        ///< UI command for solar modulation setting
    G4UIcmdWithADouble rc_cmd;          ///< UI command for rigidity cutoff setting
    G4UIcmdWithADouble d_cmd;           ///< UI command for atmospheric depth setting
    G4UIcmdWithADouble w_cmd;           ///< UI command for water fraction setting
    G4UIcmdWithADoubleAndUnit t_cmd;    ///< UI command for thermal energy
};

#endif
