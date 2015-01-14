/// \file FluxCounterBuilder.hh \brief Vacuum ``flux counter'' volume for toy models.
#ifndef FLUXCOUNTERBUILDER_HH
#define FLUXCOUNTERBUILDER_HH

#include "Builder.hh"

#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>

/// Class for toy models, evacuated flux-counting volume
class FluxCounterBuilder: public Builder {
public:
    /// Constructor
    FluxCounterBuilder();
    
    /// Construct geometry
    void construct();
    
    /// Set dimensions
    void setDimensions(G4ThreeVector d) { dim = d; }
    
private:
    
    G4VisAttributes main_vis;   ///< visualization settings for volume
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
