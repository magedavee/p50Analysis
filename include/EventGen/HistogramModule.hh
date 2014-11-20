#ifndef HISTOGRAMMODULE_HH
/// Assure headers are only loaded once
#define HISTOGRAMMODULE_HH

#include "PrimaryGeneratorAction.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>

class TH1;

/// Generator module for throwing from user-supplied histogram
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
    double netFlux;                     ///< total flux 1/cm^2/s
    
    int ptcl = 11;                      ///< PDG ID for particle to throw
    string fname;                       ///< file name for histogram
    string hname = "hSpectrum";         ///< name of histogram in file
    
    G4UIdirectory hist_dir;             ///< UI directory for cosmic neutron generator controls
    G4UIcmdWithAString file_cmd;        ///< UI command for setting input file name
    G4UIcmdWithAString hname_cmd;       ///< UI command for setting histogram name
    G4UIcmdWithAnInteger ptcl_cmd;      ///< UI command for setting particle type
};

#endif
