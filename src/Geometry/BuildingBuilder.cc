#include "BuildingBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>

BuildingBuilder::BuildingBuilder(): ShellLayerBuilder("Building"),
wall_thick(0.5*m), wall_clearance(1.*m), ceil_thick(0.5*m), ceil_clearance(0.5*m),
floor_thick(0.1*m), makeVacuum(false), makeBare(false), makeFluxTest(false),
building_ui_dir("/geom/building/"),
ceilCmd("/geom/building/ceilthick",this),
bareCmd("/geom/building/makeBare",this),
vacuumCmd("/geom/building/makeVacuum",this),
fluxCmd("/geom/building/makeFluxTest",this) {
    ceilCmd.SetGuidance("Set thickness of building ceiling");
    ceilCmd.AvailableForStates(G4State_PreInit);
    
    bareCmd.SetGuidance("No walls, ceiling, or floor around detector");
    bareCmd.AvailableForStates(G4State_PreInit);
    
    vacuumCmd.SetGuidance("Turn building materials to vacuum");
    bareCmd.AvailableForStates(G4State_PreInit);
}

void BuildingBuilder::construct() {
    
    if(makeBare) {
        wall_thick = ceil_thick = floor_thick = ceil_clearance = 0;
        wall_clearance = 5.0;
        makeVacuum = true;
    }
    
    myDetUnit.construct();
    
    if(makeFluxTest) {
        myFluxCounter.setDimensions(G4ThreeVector(10*m,10*m,1*cm));
        myFluxCounter.construct();
        ceil_clearance = ceil_thick = 0;
        floor_thick = 2.*m;
        addChild(&myFluxCounter);
    } else {
        addChild(&myDetUnit);
    }

    layers.clear();
    
    ShellLayerSpec Sair(wall_clearance, makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Air, G4Colour(0.5, 0.5, 1.0));
    Sair.uthick[2] = ceil_clearance;
    Sair.lthick[2] = 0;
    addLayer(Sair);
    
    ShellLayerSpec Swall(wall_thick, makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Concrete, G4Colour(0.3, 0.4, 0.4));
    Swall.uthick[2] = ceil_thick;
    Swall.lthick[2] = floor_thick;
    addLayer(Swall);
    
    constructLayers(makeFluxTest? (Builder&)myFluxCounter : (Builder&)myDetUnit);
}

void BuildingBuilder::SetNewValue(G4UIcommand* command, G4String value) {
    if(command == &ceilCmd) ceil_thick = ceilCmd.GetNewDoubleValue(value);
    else if(command == &bareCmd) makeBare = true;
    else if(command == &vacuumCmd) makeVacuum = true; 
    else if(command == &fluxCmd) makeFluxTest = true; 
    else G4cout << "Unknown command!" << G4endl;
}

void BuildingBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "mode", makeBare? "bare" : makeVacuum? "vacuum" : "normal");
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}


