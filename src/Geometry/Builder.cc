#include "Builder.hh"
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>

void ShellLayerSpec::fillNode(TXMLEngine& E) {
    if(mat) addAttr(E, "mat", mat->GetName());
    addAttr(E, "top", G4BestUnit(top_thick,"Length"));
    addAttr(E, "side", G4BestUnit(side_thick,"Length"));
    addAttr(E, "bottom", G4BestUnit(bottom_thick,"Length"));
}

G4VPhysicalVolume* ShellLayerSpec::wrap(G4LogicalVolume*& child, G4ThreeVector& dim, const G4String& nm) const {
    dim += G4ThreeVector(2*side_thick, 2*side_thick, top_thick + bottom_thick);
    G4Box* layer_box = new G4Box(nm+"_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    G4LogicalVolume* s_log = new G4LogicalVolume(layer_box, mat, nm+"_log");
    s_log->SetVisAttributes(vis);

    G4VPhysicalVolume* cplace = new G4PVPlacement(NULL, G4ThreeVector(0,0,(bottom_thick-top_thick)/2.),
                                                  child, nm+"_phys", s_log, false, 0, true);
    child = s_log;
    return cplace;
}
