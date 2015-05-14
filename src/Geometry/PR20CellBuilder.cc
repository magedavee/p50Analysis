#include "PR20CellBuilder.hh"
#include "MaterialsHelper.hh"
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4UnionSolid.hh>

PR20CellBuilder::PR20CellBuilder(): ScintSegVol("PR20_Cell"),
inner_width(6*in), inner_length(1*m), wall_thick(0.5*in),
flange_thick(1*in), flange_width(7.5*in), panel_sep(144*mm),
scintLiLoading(0.001), mySeparator("P20separator"),
P20Dir("/geom/P20/"),
loadingCmd("/geom/P20/scint6Li",this) {
    loadingCmd.SetGuidance("Set P20 cell 6Li loading mass fraction.");
    loadingCmd.AvailableForStates(G4State_PreInit);
}

void PR20CellBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &loadingCmd) scintLiLoading = loadingCmd.GetNewDoubleValue(newValue);
}
    
void PR20CellBuilder::construct() {
    mySeparator.width = panel_sep - mySeparator.totalThick;
    mySeparator.length = inner_length;
    mySeparator.construct();
    addChild(&mySeparator);
    
    dim = G4ThreeVector(flange_width, inner_length + 2*flange_thick, flange_width);
    
    G4Box* pr20_box = new G4Box("pr20_box", dim[0]/2, dim[1]/2, dim[2]/2);
    main_log = new G4LogicalVolume(pr20_box, MaterialsHelper::M().Air, "pr20_main_log");
    main_log->SetVisAttributes(new G4VisAttributes(G4Color(1.,1.,1.,0.1)));
    
    G4Box* flange_box = new G4Box("flange_box", flange_width/2, flange_thick/2, flange_width/2);
    G4LogicalVolume* flange_log = new G4LogicalVolume(flange_box, MaterialsHelper::M().PMMA, "flange_log");
    flange_log->SetVisAttributes(new G4VisAttributes(G4Color(1.,0.5,0.5,0.3)));
    for(int y=-1; y<=1; y+=2)
        new G4PVPlacement(NULL, G4ThreeVector(0,y*(inner_length+flange_thick)/2,0), flange_log, "flange_phys", main_log, true, (y+1)/2, true);
    
    G4Box* cell_core_box = new G4Box("cell_core_box", inner_width/2+wall_thick, inner_length/2, inner_width/2+wall_thick);
    G4UnionSolid* cell_box = new G4UnionSolid("cell_oneflange", cell_core_box, flange_box, NULL, G4ThreeVector(0,(inner_length-flange_thick)/2.,0));
    cell_box = new G4UnionSolid("cell_box", cell_box, flange_box, NULL, G4ThreeVector(0,-(inner_length-flange_thick)/2.,0));
    
    G4LogicalVolume* cell_log = new G4LogicalVolume(cell_box, MaterialsHelper::M().PMMA, "cell_log");
    cell_log->SetVisAttributes(new G4VisAttributes(G4Color(1.,1.,0.5,0.3)));
    new G4PVPlacement(NULL, G4ThreeVector(), cell_log, "cell_phys", main_log, false, 0, true);
    
    G4Box* scint_box = new G4Box("scint_box", inner_width/2, inner_length/2, inner_width/2);
    scint_log =  new G4LogicalVolume(scint_box, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, scintLiLoading), "scint_log");
    scint_phys = new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "scint_phys", cell_log, false, 0, true);
    scint_log->SetVisAttributes(new G4VisAttributes(G4Color(0.5,0.5,1.0,0.7)));
    
    if(mySeparator.main_log) {
        new G4PVPlacement(rot_X_90, G4ThreeVector(-panel_sep/2., 0, 0), mySeparator.main_log, "panel", scint_log, true, 0, true);
        new G4PVPlacement(rot_X_90, G4ThreeVector(panel_sep/2., 0, 0), mySeparator.main_log, "panel", scint_log, true, 1, true);
        new G4PVPlacement(rot_X_90_Z_90, G4ThreeVector(0, 0, -panel_sep/2.), mySeparator.main_log, "panel", scint_log, true, 2, true);
        new G4PVPlacement(rot_X_90_Z_90, G4ThreeVector(0, 0, panel_sep/2.), mySeparator.main_log, "panel", scint_log, true, 3, true);
    }
}

int PR20CellBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    if(!mySeparator.main_log) return 0;
    double lmax = (panel_sep-mySeparator.totalThick)/2.;
    return !(fabs(pos[0]) < lmax && fabs(pos[2]) < lmax && fabs(pos[1]) < inner_length/2);
}
