#ifndef FLUXCOUNTERBUILDER_HH
#define FLUXCOUNTERBUILDER_HH

#include "XMLProvider.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Class for toy models, evacuated flux-counting volume
class FluxCounterBuilder: public XMLProvider {
public:
    /// Constructor
    FluxCounterBuilder();
    
    /// Construct geometry
    void construct();
    
    G4LogicalVolume* main_log;  ///< main mother volume, metal shell enclosure
    G4ThreeVector dim;          ///< outer dimensions
    
private:
    
    G4VisAttributes main_vis;   ///< visualization settings for volume
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
