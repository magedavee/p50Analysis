#ifndef SPHERESHELLSBUILDER_HH
#define SPHERESHELLSBUILDER_HH

#include "ScintSegVol.hh"
#include <vector>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAnInteger.hh>

/// Builder for spherical shells toy geometry
class SphereShellBuilder: public ScintSegVol, public G4UImessenger {
public:
    /// Constructor
    SphereShellBuilder();
    
    /// Construct geometry
    virtual void construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
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
    
    G4UIdirectory sphereDir;                    ///< UI directory for sphere-related commands
    G4UIcmdWithADoubleAndUnit radiusCmd;        ///< radius command
    G4UIcmdWithAnInteger segCmd;                ///< segmentation command
    G4UIcmdWithAString matCmd;                  ///< material command
};
    
#endif
