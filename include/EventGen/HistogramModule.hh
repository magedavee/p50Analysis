/// \file HistogramModule.hh \brief User-defined histogram event generator.
#ifndef HISTOGRAMMODULE_HH
/// Assure headers are only loaded once
#define HISTOGRAMMODULE_HH

#include "PrimaryGeneratorAction.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

class TH1;

/// Generator module for throwing from user-supplied histogram
/// either as rate [Hz/bin] (for volume sources) or flux [Hz/cm^2/bin] (for surface source) vs kinetic energy [MeV]
/// particle type thrown is set by /gun/ commands
class HistogramModule: public PrimaryGeneratorModule, public G4UImessenger {
public:
    /// Constructor
    HistogramModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    /// get total "real time" for primaries thrown
    virtual G4double GetGeneratorTime() const;

    /// UI response
    void SetNewValue(G4UIcommand*,G4String);
    
protected:

    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    /// clear previous distribution
    void resetDistribution();
    /// generate distribution histogram
    void makeDistribution();
    
    TH1* myDist;                        ///< underlying energy distribution
    double monoE = -1;                  ///< monoenergetic generator energy (set < 0 for histogram use)
    double netFlux;                     ///< total rate or flux
    
    string fname;                       ///< file name for histogram
    string hname = "hSpectrum";         ///< name of histogram in file
    
    G4UIdirectory hist_dir;             ///< UI directory for cosmic neutron generator controls
    G4UIcmdWithADoubleAndUnit monoE_cmd;///< UI command for monoenergetic production
    G4UIcmdWithAString file_cmd;        ///< UI command for setting input file name
    G4UIcmdWithAString hname_cmd;       ///< UI command for setting histogram name
};

#endif
