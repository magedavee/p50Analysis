#include "PR2ShieldBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4Box.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include "SMExcept.hh"
#include "strutils.hh"

void PR2ShieldBuilder::_construct() {
    
    dim = G4ThreeVector(6*in, 24*in, 6*in);
    addLayer(ShellLayerSpec(dim/2, dim/2, MaterialsHelper::M().Air, G4Colour(1.0,0,0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 2*in, 2*in), G4ThreeVector(2*in, 2*in, 2*in), MaterialsHelper::M().LiPoly, G4Colour(0.7,0.0,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 4*in, 2*in), G4ThreeVector(4*in, 4*in, 2*in), MaterialsHelper::M().nat_Pb, G4Colour(0.5,0.5,0.5,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(6*in, 6*in, 10*in), G4ThreeVector(6*in, 6*in, 0*in), MaterialsHelper::M().BPoly5, G4Colour(0.5,0.5,0.0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(0,0,0), G4ThreeVector(0,0,1*in), MaterialsHelper::M().BPoly30, G4Colour(0.0,0.7,0.0,0.5)));
    const double st = in/16.; // steel shell thickness
    addLayer(ShellLayerSpec(G4ThreeVector(st,st,st), G4ThreeVector(st,st,0), MaterialsHelper::M().SS444, G4Colour(0.7,0.7,0.7,0.5)));
    
    construct_layers();
    
    // lead layer gap
    //G4Box* leadgap_box = new G4Box("leadgap_box", 1*in, 16*in, 1*in);
    //G4LogicalVolume* leadgap_log = new G4LogicalVolume(leadgap_box, MaterialsHelper::M().BPoly5, "leadgap_log");
    //new G4PVPlacement(NULL, G4ThreeVector(-7*in, 2*in, 6*in), leadgap_log, "leadgap_phys", layer_log[1], false, 0, true);
}

////////////////////////////
////////////////////////////
////////////////////////////

PR2MuVetoBuilder::PR2MuVetoBuilder(int n): ScintChunk("PR2_Veto_"+to_str(n), 1000+n) {
    dim = G4ThreeVector(12.5*in, 60*in, 1*in);
    mySolid = new G4Box("veto_box", dim[0]/2, dim[1]/2, dim[2]/2);
    myMat = MaterialsHelper::M().PVT;
    myVis = new G4VisAttributes(G4Colour(0.2,0.2,1,0.5));
}
