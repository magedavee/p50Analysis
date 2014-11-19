#include "BuildingBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>

BuildingBuilder::BuildingBuilder(): ShellLayerBuilder("Building"),
wall_thick(0.5*m), wall_clearance(1.*m), ceil_thick(0.5*m), ceil_clearance(0.5*m),
floor_thick(0.1*m), makeVacuum(false),  myDetector(NULL),
building_ui_dir("/geom/building/"),
ceilCmd("/geom/building/ceilthick",this),
vacuumCmd("/geom/building/makeVacuum",this) {
    ceilCmd.SetGuidance("Set thickness of building ceiling");
    ceilCmd.AvailableForStates(G4State_PreInit);
        
    vacuumCmd.SetGuidance("Turn building materials to vacuum");
    vacuumCmd.AvailableForStates(G4State_PreInit);
}

void BuildingBuilder::construct() {
    layers.clear();
    
    ShellLayerSpec Sair(G4ThreeVector(wall_clearance, wall_clearance, ceil_clearance),
                        G4ThreeVector(wall_clearance, wall_clearance, 0),
                        makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Air, G4Colour(0.5, 0.5, 1.0));
    addLayer(Sair);
    
    ShellLayerSpec Swall(G4ThreeVector(wall_thick, wall_thick, ceil_thick),
                         G4ThreeVector(wall_thick, wall_thick, floor_thick),
                         makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Concrete, G4Colour(0.3, 0.4, 0.4));
    addLayer(Swall);
    
    if(myDetector) { addChild(myDetector); constructLayers(*myDetector); }
    else constructLayers(NULL, G4ThreeVector(1*m, 1*m, 1*m));
}

void BuildingBuilder::SetNewValue(G4UIcommand* command, G4String value) {
    if(command == &ceilCmd) ceil_thick = ceilCmd.GetNewDoubleValue(value);
    else if(command == &vacuumCmd) makeVacuum = true;
    else G4cout << "Unknown command!" << G4endl;
}

void BuildingBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "mode", makeVacuum? "vacuum" : "normal");
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
