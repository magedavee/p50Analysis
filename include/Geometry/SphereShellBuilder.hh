#ifndef SPHERESHELLSBUILDER_HH
#define SPHERESHELLSBUILDER_HH

#include "ScintSegVol.hh"
#include <vector>

/// Builder for spherical shells toy geometry
class SphereShellBuilder: public ScintSegVol {
public:
    /// Constructor
    SphereShellBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector& x) const;
    
    /// assign SD to scintillator volumes
    virtual void setScintSD(G4VSensitiveDetector* SD);
    
    G4Material* myMat;          ///< material
    double radius;              ///< outer sphere radius
    unsigned int ndivs;         ///< number of radial divisions
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    std::vector<G4LogicalVolume*> shells;       ///< material shells
};
    
#endif
