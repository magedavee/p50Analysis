#include "DetVolBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Box.hh>

/// utility function for converting to string
template<typename T>
std::string to_str(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

DetVolBuilder::DetVolBuilder(): main_log(NULL),
shell_thick(2*mm), air_buffer_thick(10*cm), shell_vis(G4Colour(1.0,0,1.0)) { }

void DetVolBuilder::construct() {
    
    myTank.construct();
    myLid.construct(myTank);
    myPMT.construct();
    
    dim = G4ThreeVector(myTank.getWidthX() + 2*(shell_thick+air_buffer_thick),
                        myTank.getWidthY() + 2*(shell_thick+air_buffer_thick),
                        myTank.tank_depth + 2*(shell_thick+air_buffer_thick+myLid.getThick()+myPMT.getLength()));
    
    ///////////////////////
    // shell and air volume
    
    G4Box* shell_box = new G4Box("shell_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(shell_box, MaterialsHelper::SS444, "DetVol_main_log");
    main_log->SetVisAttributes(&shell_vis);
    
    G4Box* air_box = new G4Box("air_box", dim[0]/2.-shell_thick, dim[1]/2.-shell_thick, dim[2]/2.-shell_thick);
    G4LogicalVolume* air_log = new G4LogicalVolume(air_box, MaterialsHelper::Air, "DetVol_air_log");
    air_log->SetVisAttributes(&shell_vis);
    new G4PVPlacement(NULL, G4ThreeVector(), air_log, "DetVol_air_phys", main_log, false, 0, false);
    
    //////////////////////
    // internal components
    
    new G4PVPlacement(NULL, G4ThreeVector(), myTank.main_log, "DetVol_tank_phys", air_log, false, 0, true);
    double lid_z = (myTank.tank_depth + myLid.getThick())/2.;
    new G4PVPlacement(NULL, G4ThreeVector(0,0,lid_z), myLid.main_log, "DetVol_lid_phys_0", air_log, true, 0, true);
    G4RotationMatrix* lid_flip = new G4RotationMatrix();
    lid_flip->rotateX(180*deg);
    new G4PVPlacement(lid_flip, G4ThreeVector(0,0,-lid_z), myLid.main_log, "DetVol_lid_phys_1", air_log, true, 1, true);
    
    double pmt_z = (myTank.tank_depth + myPMT.getLength())/2. + myLid.getThick();
    for(uint i=0; i<myTank.getNSeg(); i++) {
        G4ThreeVector pos = myTank.getSegmentPosition(i);
        new G4PVPlacement(lid_flip, G4ThreeVector(pos[0],pos[1],pmt_z), myPMT.main_log, "DetVol_PMT_phys_"+to_str(2*i), air_log, true, 2*i, true);
        new G4PVPlacement(NULL, G4ThreeVector(pos[0],pos[1],-pmt_z), myPMT.main_log, "DetVol_PMT_phys_"+to_str(2*i+1), air_log, true, 2*i+1, true);
    }
}
