#include "HFIRCoreBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>

HFIRCoreBuilder::HFIRCoreBuilder(): Builder("HFIRCore"),
r0(0.075*m), r1(0.2*m), height(0.5*m),
hfir_dir("/geom/HFIR/"),
pos_cmd("/geom/HFIR/corepos",this) {
    hfir_dir.SetGuidance("HFIR geometry settings");
    hfir_dir.AvailableForStates(G4State_PreInit);
    
    pos_cmd.SetGuidance("Placement position for reactor core");
    pos_cmd.AvailableForStates(G4State_PreInit);
}

void HFIRCoreBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &pos_cmd)  pos = pos_cmd.GetNew3VectorValue(newValue);
}

void HFIRCoreBuilder::construct() {
    dim = G4ThreeVector(2*r1, 2*r1, height);
    
    G4Tubs* main_tube = new G4Tubs("HFIRCoreTube", r0, r1, height/2, 0, 2*M_PI);    
    main_log = new G4LogicalVolume(main_tube, MaterialsHelper::M().Vacuum, "HFIRCore_main_Log");
}

void HFIRCoreBuilder::fillNode(TXMLEngine& E) {
    Builder::fillNode(E);
    addAttr(E, "r0", G4BestUnit(r0,"Length"));
    addAttr(E, "r1", G4BestUnit(r1,"Length"));
    addAttr(E, "height", G4BestUnit(height,"Length"));
    addAttr(E, "pos", G4BestUnit(pos,"Length"));
}
