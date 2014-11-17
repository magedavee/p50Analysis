#include "PR2ShieldBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4Box.hh>
#include <G4UnitsTable.hh>

void PR2ShieldBuilder::construct() {
    
    dim = G4ThreeVector(6*in, 24*in, 6*in);
    G4Box* cave_box = new G4Box("cave_box", dim[0]/2, dim[1]/2, dim[2]/2);
    cave_log = new G4LogicalVolume(cave_box, MaterialsHelper::M().Air, "cave_log");
    cave_log->SetVisAttributes(new G4VisAttributes(G4Colour(1.0,0,0,0.5)));
    
    
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 2*in, 2*in), G4ThreeVector(2*in, 2*in, 2*in), MaterialsHelper::M().LiPoly, G4Colour(0.7,0.0,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 4*in, 2*in), G4ThreeVector(4*in, 4*in, 2*in), MaterialsHelper::M().nat_Pb, G4Colour(0.5,0.5,0.5,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(6*in, 6*in, 10*in), G4ThreeVector(6*in, 6*in, 0*in), MaterialsHelper::M().BPoly, G4Colour(0.5,0.5,0.0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(0,0,0), G4ThreeVector(0,0,1*in), MaterialsHelper::M().BPoly, G4Colour(0.0,0.7,0.0,0.5)));
    
    constructLayers(cave_log, dim);
    
    // lead layer gap
    G4Box* leadgap_box = new G4Box("leadgap_box", 1*in, 16*in, 1*in);
    G4LogicalVolume* leadgap_log = new G4LogicalVolume(leadgap_box, MaterialsHelper::M().BPoly, "leadgap_log");
}

void PR2ShieldBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
