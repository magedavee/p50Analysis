#include "DIMABuilder.hh"

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SubtractionSolid.hh>
#include <G4SDManager.hh>

double DIMAArrayBuilder::spacing = 6*cm;

DIMAArrayBuilder::DIMAArrayBuilder(): ScintSegVol("ScintArray"),
l_seg(15*cm), r_seg(1.5*cm), t_seg(2.5*mm), t_guide(12*mm), t_rack(1*cm) {
    place_centered = false;
}

void DIMAArrayBuilder::construct() {
    double rackwidth = ngrid*spacing;
    dim = G4ThreeVector(rackwidth, l_seg+2*t_guide, rackwidth);
    
    G4Box* main_box = new G4Box("main_box", dim[0]/2, dim[1]/2, dim[2]/2);
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().Air, "array_log");
    
    G4Tubs* lg_tube = new G4Tubs("lg_tube", 0, r_seg, l_seg/2+t_guide, 0, 2*M_PI);
    G4Tubs* glass_tube = new G4Tubs("glass_tube", 0, r_seg, l_seg/2, 0, 2*M_PI);
    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, r_seg-t_seg, l_seg/2, 0, 2*M_PI);
    
    G4LogicalVolume* tube_log = new G4LogicalVolume(lg_tube, MaterialsHelper::M().PMMA, "tube_log");
    tube_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.15)));
    G4LogicalVolume* glass_log = new G4LogicalVolume(glass_tube, MaterialsHelper::M().Quartz, "glass_log");
    glass_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.0,0.15)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), glass_log, "glass_phys", tube_log, false, 0, true);
    //scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, 0.001, true), "scint_log");
    scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().EJ309, "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.4,0.2,1.0,0.3)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), scint_log, "scint_phys", glass_log, false, 0, true);
    
    G4Tubs* He3tube = new G4Tubs("3He_tube", 0, 1.2*cm, l_seg/2, 0, 2*M_PI);
    
    G4VSolid* rackSolid = new G4Box("rackSolid", spacing/2, t_rack/2, spacing/2);
    rackSolid = new G4SubtractionSolid("rackSolid", rackSolid, lg_tube, rot_X_90, G4ThreeVector());
    for(int dx = -1; dx <= 1; dx += 2)
        for(int dy = -1; dy <= 1; dy += 2)
            rackSolid  = new G4SubtractionSolid("rackSolid", rackSolid, He3tube, rot_X_90, G4ThreeVector(dx*spacing/2,0,dy*spacing/2));
    G4LogicalVolume* rack_log = new G4LogicalVolume(rackSolid, MaterialsHelper::M().nat_Al, "rack_log");
    rack_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.7,0.3,0.3,0.15)));
    
    for(int n=0; n<ngrid*ngrid; n++) {
        G4ThreeVector tube_pos = getSegCenter(n);
        new G4PVPlacement(rot_X_90, tube_pos, tube_log, "tube_phys", main_log, true, n, true);
        for(int ymul = -1; ymul <= 1; ymul += 2) {
            G4ThreeVector rack_pos = tube_pos + G4ThreeVector(0,ymul*(l_seg-t_rack)/2.,0);
            new G4PVPlacement(NULL, rack_pos, rack_log, "rack_phys", main_log, true, 2*n+(ymul+1)/2, true);
        }
    }
    
}

int DIMAArrayBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    int nx = floor(pos[0]/spacing + ngrid/2.);
    int nz = floor(pos[2]/spacing + ngrid/2.);
    return nx + ngrid*nz;
}

G4ThreeVector DIMAArrayBuilder::getSegCenter(int n) const {
    return G4ThreeVector(((n%ngrid)+0.5-ngrid/2.)*spacing, 0, ((n/ngrid)+0.5-ngrid/2.)*spacing);
}

/////////////////////////
/////////////////////////
/////////////////////////

void DIMABoxBuilder::_construct() {
    dim = myArray.getDimensions() + G4ThreeVector(14*cm, 30*cm, 4*cm);
    G4ThreeVector zOff(0,0,dim[2]/2);
    addLayer(ShellLayerSpec(dim*0.5+zOff, dim*0.5-zOff, MaterialsHelper::M().Air, G4Colour(0.7,0.0,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*mm, 2*mm, 2*mm), G4ThreeVector(2*mm, 2*mm, 10*mm), MaterialsHelper::M().SS444, G4Colour(0.7,0.0,0.7,0.5)));
    construct_layers();
}
