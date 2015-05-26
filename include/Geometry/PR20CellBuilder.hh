/// \file PR20CellBuilder.hh \brief PROSPECT-20 cell geometry.
#ifndef PR20CELLBUILDER_HH
/// Assure this header is loaded only once
#define PR20CELLBUILDER_HH

#include "ScintSegVol.hh"
#include "SeparatorBuilder.hh"
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>

/// PROSPECT-20 cell
class PR20CellBuilder: public ScintSegVol, public G4UImessenger {
public:
    /// Constructor
    PR20CellBuilder();
    
    /// Construct geometry
    void construct();
    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector& pos) const;
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    double inner_width;         ///< width (and height) of scintillator volume
    double inner_length;        ///< length of scintillator volume
    double wall_thick;          ///< thickness of main cell wall
    double flange_thick;        ///< thickness of flange
    double flange_width;        ///< outer width of flange
    double panel_sep;           ///< center-to-center panel separation
    double scintLiLoading;      ///< scintillator Li loading mass fraction
    
    SeparatorBuilder mySeparator;       ///< separator panels

protected:
    G4UIdirectory P20Dir;               ///< UI directory for P20-related commands
    G4UIcmdWithADouble loadingCmd;      ///< Li loading command
};
    
#endif
