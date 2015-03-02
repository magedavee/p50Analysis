/// \file HFIRCoreBuilder.hh \brief HFIR reactor core geometry for antinu vertex positioning.
#ifndef HFIRCOREBUILDER_HH
/// Assure this header is only loaded once
#define HFIRCOREBUILDER_HH

#include "Builder.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

/// Builder for scintillator separator panels
class HFIRCoreBuilder: public Builder, public G4UImessenger {
public:
    /// Constructor
    HFIRCoreBuilder();
    
    /// Construct geometry
    void construct();
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    double r0;                          ///< inner radius
    double r1;                          ///< outer radius
    double height;                      ///< height (z)
    G4ThreeVector pos;                  ///< center placement position
   
protected:
    G4UIdirectory hfir_dir;             ///< UI directory for reactor core geometry commands
    G4UIcmdWith3VectorAndUnit pos_cmd;  ///< UI command for position
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
