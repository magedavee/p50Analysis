#include "ScintLayer.hh"
#include "MaterialsHelper.hh"
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <cmath>

ScintLayer::ScintLayer(int vol): ScintSegVol("ScintLayer"), scintLiLoading(.0075),
length(5*2.54*cm), radius(2.5*2.54*cm), wall_thick(0.125*2.54*cm),
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
    volID=vol;
}

void ScintLayer::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &lengthCmd) length = lengthCmd.GetNewDoubleValue(newValue);
    else if(command == &radiusCmd) radius = radiusCmd.GetNewDoubleValue(newValue);
    else if(command == &wallThickCmd) wall_thick = wallThickCmd.GetNewDoubleValue(newValue);
    else if(command == &loadingCmd) scintLiLoading = loadingCmd.GetNewDoubleValue(newValue);
    else G4cout << "Unknown command!" << G4endl;
}
 
void ScintLayer::construct() {
    
    dim = G4ThreeVector(2*radius, 2*radius, length);
    G4Box * cell=new G4Box("cell",2*m,2*m,.01*m);
    G4Tubs* cell_tube = new G4Tubs("cell_tube", 0, radius, length/2., 0, 2*M_PI);
    double scint6LiLoading=0.001;
    main_log = new G4LogicalVolume(cell, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, scint6LiLoading), "scint_Layer_log");
//    main_log = new G4LogicalVolume(cell, MaterialsHelper::M().PMMA, "ScintLayMainLog");
    main_log->SetVisAttributes(G4Colour(0.0,1.,0.0));
//    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, radius-wall_thick, length/2.-wall_thick, 0, 2*M_PI);
    scint_log = new G4LogicalVolume(cell, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().UG_AB, scintLiLoading, false), "ScintCell_scint_log");
    new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintCell_scint_phys", main_log, false, 0, false);
}

void ScintLayer::fillNode(TXMLEngine& E) {
//    addAttr(E, "dim", G4BestUnit(dim,"Length"));
//    addAttr(E, "wall", G4BestUnit(wall_thick,"Length"));
//    addAttr(E, "scint", scint_log->GetMaterial()->GetName());
}
int ScintLayer::getSegmentNum(const G4ThreeVector& pos) const 
{
/*	if(pos[2]>0)
	{
		G4cout<<"pos 420  x"<<pos[0]<<" y "<<pos[1]<<" z "<<pos[2]<<G4endl;
		return 420;
	}
	else
	{
		G4cout<<"pos 520  x"<<pos[0]<<" y "<<pos[1]<<" z "<<pos[2]<<G4endl;
		return 520;
	}*/
//	G4cout<<"layer volID "<<volID<<G4endl;
	return volID;
}
