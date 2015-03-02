#include "ScintCellBuilder.hh"
#include "MaterialsHelper.hh"
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <cmath>

ScintCellBuilder::ScintCellBuilder(): ScintSegVol("ScintCell"), scintLiLoading(.0010),
length(5*in), radius(2.5*in), wall_thick(0.125*in),
cellDir("/geom/testcell/"),
lengthCmd("/geom/testcell/length",this),
radiusCmd("/geom/testcell/radius",this),
wallThickCmd("/geom/testcell/wall",this),
loadingCmd("/geom/testcell/loading",this) {
    
    lengthCmd.SetGuidance("Set test cell length.");
    lengthCmd.AvailableForStates(G4State_PreInit);

    radiusCmd.SetGuidance("Set test cell radius.");
    radiusCmd.AvailableForStates(G4State_PreInit);
    
    wallThickCmd.SetGuidance("Set test cell wall thickness.");
    wallThickCmd.AvailableForStates(G4State_PreInit);
    
    loadingCmd.SetGuidance("Set test cell Li loading.");
    loadingCmd.AvailableForStates(G4State_PreInit);
}

void ScintCellBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &lengthCmd) length = lengthCmd.GetNewDoubleValue(newValue);
    else if(command == &radiusCmd) radius = radiusCmd.GetNewDoubleValue(newValue);
    else if(command == &wallThickCmd) wall_thick = wallThickCmd.GetNewDoubleValue(newValue);
    else if(command == &loadingCmd) scintLiLoading = loadingCmd.GetNewDoubleValue(newValue);
    else G4cout << "Unknown command!" << G4endl;
}
 
void ScintCellBuilder::construct() {
    
    dim = G4ThreeVector(2*radius, 2*radius, length);
    
    G4Tubs* cell_tube = new G4Tubs("cell_tube", 0, radius, length/2., 0, 2*M_PI);
    main_log = new G4LogicalVolume(cell_tube, MaterialsHelper::M().PMMA, "ScintCell_main_log");

    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, radius-wall_thick, length/2.-wall_thick, 0, 2*M_PI);
    scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, scintLiLoading, true), "ScintCell_scint_log");
    scint_phys = new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintCell_scint_phys", main_log, false, 0, false);
}

void ScintCellBuilder::fillNode(TXMLEngine& E) {
    ScintSegVol::fillNode(E);
    addAttr(E, "wall", G4BestUnit(wall_thick,"Length"));
}
