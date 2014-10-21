#ifndef SCINTTANKBUILDER_HH
/// Assure this header is loaded only once
#define SCINTTANKBUILDER_HH

#include "ScintSegVol.hh"
#include "SlottedRodBuilder.hh"
#include "SeparatorBuilder.hh"

#include <G4UImessenger.hh>
#include <G4LogicalVolume.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4VisAttributes.hh>

/// Base class for divider-segmented scintillator tank (possibly pinwheeled)
class ScintTankBuilder: public ScintSegVol, public G4UImessenger {
public:
    /// Constructor
    ScintTankBuilder(const std::string& snm = "ScintTank");
    
    /// Construct geometry
    void construct();
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    /// assign SD to scintillator volumes
    virtual void setScintSD(G4VSensitiveDetector* SD);
    
    /// get total number of segments
    unsigned int getNSeg() const { return nSegX*nSegY; }
    /// get position at center of numbered segment
    G4ThreeVector getSegmentPosition(unsigned int n) const;
    /// get segment number at position in scintillator local coordinates
    int getSegmentNum(const G4ThreeVector& pos) const;
    
    double tank_depth;          ///< scintillator depth in long (z) direction
    double gc_thick;            ///< "gamma catcher" scintillator thickness
    double tank_wall_thick;     ///< thickness of tank wall
    double ls_buffer_thick;     ///< thickness of scintillator buffer region around separators
    double seg_size;            ///< size of each segment in x-y plane
    unsigned int nSegX;         ///< number of x segments
    unsigned int nSegY;         ///< number of y segments
    double scint6LiLoading;     ///< loading fraction of 6Li in scintillator
    
    RodBuilder* myRod;                  ///< slotted rod for holding separators
    SeparatorBuilder mySeparator;       ///< separator panels
    G4LogicalVolume* gammacatcher_log;  ///< "gamma catcher" at scintillator ends
    G4RotationMatrix rotRod;            ///< rotation matrix for rods
    
protected:
    /// calculate dimensions and construct rod/dividers
    virtual void setupDividers() = 0;
    /// construct gamma catcher geometry
    virtual void setupGammaCatcher() = 0;
    
    double theta_pw, sin_pw, cos_pw;    ///< pinwheeling rotation angle
    double lat_size;                    ///< rod lattice spacing; possibly different from segment size
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    G4UIdirectory tank_ui_dir;          ///< UI directory for tank-related commands
    G4UIcmdWithAnInteger nsegXcmd;      ///< UI command for number of x segments
    G4UIcmdWithAnInteger nsegYcmd;      ///< UI command for number of y segments
    G4UIcmdWithADoubleAndUnit segWcmd;  ///< UI command for segment width
    G4UIcmdWithADoubleAndUnit gcTcmd;   ///< UI command for gamma catcher width
    G4UIcmdWithADouble scint6Licmd;     ///< UI command for 6Li loading in scintillator
    G4VisAttributes tank_vis;           ///< visualization settings for tank
    G4VisAttributes gc_vis;             ///< visualization settings for gamma catcher
    G4VisAttributes scint_vis;          ///< visualization settings for scintillator volume
};

/// square-lattice tank with slotted rods
class SquareTankBuilder: public ScintTankBuilder {
public:
    /// Constructor
    SquareTankBuilder(): ScintTankBuilder("SquareTank") { addChild(&mySlottedRod); }
    
    SlottedRodBuilder mySlottedRod;     ///< slotted rod separator holder
    
protected:
    /// calculate dimensions and construct rod/dividers
    virtual void setupDividers();
    /// construct gamma catcher geometry
    virtual void setupGammaCatcher();
};
    
#endif
