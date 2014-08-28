#include "TankLidBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>

TankLidBuilder::TankLidBuilder(): Builder("TankLid"),
totalThick(4*cm), lid_vis(G4Colour(0.5,0.5,0.5)) { }

void TankLidBuilder::construct(const ScintTankBuilder& T) {   
    dim = G4ThreeVector(T.getWidthX(),T.getWidthY(),totalThick);
    G4Box* main_box = new G4Box("TankLidBox", dim[0]/2., dim[1]/2., dim[2]/2.);    
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().PMMA_black, "TankLid_main_Log");
    main_log->SetVisAttributes(&lid_vis);
}

void TankLidBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
