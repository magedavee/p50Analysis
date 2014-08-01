#include "TankLidBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>

TankLidBuilder::TankLidBuilder(): XMLProvider("TankLid"), main_log(NULL),
totalThick(4*cm), lid_vis(G4Colour(0.5,0.5,0.5)) { }

void TankLidBuilder::construct(const ScintTankBuilder& T) {    
    G4Box* main_box = new G4Box("TankLidBox", T.getWidthX()/2., T.getWidthY()/2., totalThick/2.);    
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().PMMA, "TankLid_main_Log");
    main_log->SetVisAttributes(&lid_vis);
}

void TankLidBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "thick", G4BestUnit(totalThick,"Length"));
}
