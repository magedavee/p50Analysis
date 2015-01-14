/// \file OpticalSurfaceSetup.hh \brief Optical reflective surface definition class.
#ifndef OPTICALSURFACESETUP_HH
/// Assure this header is loaded only once
#define OPTICALSURFACESETUP_HH

#include "XMLProvider.hh"

#include <G4OpticalSurface.hh>

/// Definition of optical surface properties.
class OpticalSurfaceSetup: public XMLProvider {
public:
    /// Constructor
    OpticalSurfaceSetup();
    
    /// Construct optical properties
    void construct();
    
    G4double refl;              ///< optical reflectivity
    G4double efficiency;        ///< optical "efficiency"
    G4double lobe;              ///< diffuse reflection component
    G4double spike;             ///< specular reflection component
    G4double back;              ///< backscatter reflection component
    
    G4OpticalSurface* S;        ///< optical surface properties
    
protected:
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};


#endif
