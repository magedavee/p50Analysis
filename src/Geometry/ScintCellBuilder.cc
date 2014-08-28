#include "ScintCellBuilder.hh"
#include "MaterialsHelper.hh"
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <cmath>

ScintCellBuilder::ScintCellBuilder(): ScintSegVol("ScintCell"), scintLiLoading(.0075),
length(5*2.54*cm), radius(2.5*2.54*cm), wall_thick(0.25*2.54*cm) { }
    
void ScintCellBuilder::construct() {
    
    dim = G4ThreeVector(2*radius, 2*radius, length);
    
    G4Tubs* cell_tube = new G4Tubs("cell_tube", 0, radius, length/2., 0, 2*M_PI);
    main_log = new G4LogicalVolume(cell_tube, MaterialsHelper::M().PMMA, "ScintCell_main_log");

    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, radius-wall_thick, length/2.-wall_thick, 0, 2*M_PI);
    scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().get6LiLS(scintLiLoading,false), "ScintCell_scint_log");
    new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintCell_scint_phys", main_log, false, 0, false);
}
    
void ScintCellBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    addAttr(E, "wall", G4BestUnit(wall_thick,"Length"));
    addAttr(E, "scint", scint_log->GetMaterial()->GetName());
}
