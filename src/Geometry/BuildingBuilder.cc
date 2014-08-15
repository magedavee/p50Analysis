#include "BuildingBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>

BuildingBuilder::BuildingBuilder(): XMLProvider("Building"), main_log(NULL),
wall_thick(0.5*m), wall_clearance(1.*m), ceil_thick(0.5*m), ceil_clearance(0.5*m),
floor_thick(0.1*m), makeVacuum(false), makeBare(false), makeFluxTest(false),
dim(), wall_vis(G4Colour(0.3, 0.4, 0.4)),
building_ui_dir("/geom/building/"),
bareCmd("/geom/building/makeBare",this),
vacuumCmd("/geom/building/makeVacuum",this),
fluxCmd("/geom/building/makeFluxTest",this) {
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
    
    myFluxCounter.dim = G4ThreeVector(6*m,6*m,1*cm);
    myFluxCounter.construct();
    
    G4ThreeVector airDim;
    if(makeFluxTest) {
        ceil_clearance = ceil_thick = 0;
        floor_thick = 5.*m;
        addChild(&myFluxCounter);
        airDim = myFluxCounter.dim;
    } else {
        addChild(&myDetUnit);
        airDim = myDetUnit.getDimensions();
    }

    airDim[0] += 2*wall_clearance;
    airDim[1] += 2*wall_clearance;
    airDim[2] += ceil_clearance;
    
    dim = airDim;
    dim[0] += 2*wall_thick;
    dim[1] += 2*wall_thick;
    dim[2] += ceil_thick + floor_thick;
    
    G4Box* wall_box = new G4Box("wall_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(wall_box, makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Concrete, "Building_main_log");
    main_log->SetVisAttributes(&wall_vis);
    
    G4Box* air_box = new G4Box("air_box", airDim[0]/2., airDim[1]/2., airDim[2]/2.);
    air_log = new G4LogicalVolume(air_box, makeVacuum? MaterialsHelper::M().Vacuum : MaterialsHelper::M().Air, "Building_air_log");
    air_log->SetVisAttributes(&wall_vis);
    new G4PVPlacement(NULL, G4ThreeVector(0, 0, (floor_thick-ceil_thick)/2.), air_log, "Building_air_phys", main_log, false, 0, false);
    
    new G4PVPlacement(NULL, G4ThreeVector(0, 0, -ceil_clearance/2.),
                      makeFluxTest?myFluxCounter.main_log:myDetUnit.main_log,
                      makeFluxTest?"Flux_phys":"Det_phys", air_log, false, 0, true);    
}

void BuildingBuilder::SetNewValue(G4UIcommand* command, G4String) {
    if(command == &bareCmd) makeBare = true;
    else if(command == &vacuumCmd) makeVacuum = true; 
    else if(command == &fluxCmd) makeFluxTest = true; 
    else G4cerr << "Unknown command!" << G4endl;
}

void BuildingBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "mode", makeBare? "bare" : makeVacuum? "vacuum" : "normal");
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
