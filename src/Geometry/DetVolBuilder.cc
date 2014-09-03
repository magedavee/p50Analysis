#include "DetVolBuilder.hh"
#include "MaterialsHelper.hh"
#include "Utilities.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4Box.hh>

DetVolBuilder::DetVolBuilder(): Builder("DetVol"),
shell_thick(2*mm), buffer_thick(2*cm), shell_vis(G4Colour(1.0,0,1.0)) {
    myRot.rotateX(-90*deg);
    addChild(&myTank);
    addChild(&myLid);
    addChild(&myPMT);
}

void DetVolBuilder::construct() {
    
    myTank.construct();
    myLid.construct(myTank);
    myPMT.construct();
    
    G4ThreeVector airDim(myTank.getWidthX() + 2*buffer_thick,
                         myTank.getWidthY() + 2*buffer_thick,
                         myTank.tank_depth + 2*(buffer_thick+myLid.getThick()+myPMT.getLength()));
        
    dim = myRot * airDim;
    dim = G4ThreeVector(fabs(dim[0])+2*shell_thick, fabs(dim[1])+2*shell_thick, fabs(dim[2])+2*shell_thick);
    
    //////////////
    // outer shell
    
    G4Box* shell_box = new G4Box("shell_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(shell_box, MaterialsHelper::M().SS444, "DetVol_main_log");
    main_log->SetVisAttributes(&shell_vis);
    
    /////////////////////////
    // (rotated) inner volume
    
    G4Box* inner_box = new G4Box("inner_box", airDim[0]/2., airDim[1]/2., airDim[2]/2.);
    G4LogicalVolume* inner_log = new G4LogicalVolume(inner_box, MaterialsHelper::M().MinOil, "DetVol_inner_log");
    inner_log->SetVisAttributes(&shell_vis);
    new G4PVPlacement(&myRot, G4ThreeVector(), inner_log, "DetVol_inner_phys", main_log, false, 0, false);
    
    //////////////////////
    // internal components
    
    new G4PVPlacement(NULL, G4ThreeVector(), myTank.main_log, "DetVol_tank_phys", inner_log, false, 0, true);
    double lid_z = (myTank.tank_depth + myLid.getThick())/2.;
    new G4PVPlacement(NULL, G4ThreeVector(0,0,lid_z), myLid.main_log, "DetVol_lid_phys_0", inner_log, true, 0, true);
    G4RotationMatrix* lid_flip = new G4RotationMatrix();
    lid_flip->rotateX(180*deg);
    new G4PVPlacement(lid_flip, G4ThreeVector(0,0,-lid_z), myLid.main_log, "DetVol_lid_phys_1", inner_log, true, 1, true);
    
    double pmt_z = (myTank.tank_depth + myPMT.getLength())/2. + myLid.getThick();
    for(unsigned int i=0; i<myTank.getNSeg(); i++) {
        G4ThreeVector pos = myTank.getSegmentPosition(i);
        new G4PVPlacement(lid_flip, G4ThreeVector(pos[0],pos[1],pmt_z), myPMT.main_log, "DetVol_PMT_phys_"+to_str(2*i), inner_log, true, 2*i, true);
        new G4PVPlacement(NULL, G4ThreeVector(pos[0],pos[1],-pmt_z), myPMT.main_log, "DetVol_PMT_phys_"+to_str(2*i+1), inner_log, true, 2*i+1, true);
    }
}

void DetVolBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}