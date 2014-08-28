#include "ScintTankBuilder.hh"
#include "Utilities.hh"

#include <cassert>
#include <vector>

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>
#include <G4LogicalBorderSurface.hh>

ScintTankBuilder::ScintTankBuilder(): ScintSegVol("ScintTank"),
tank_depth(65*cm), tank_wall_thick(2*cm), ls_buffer_thick(5*cm),
seg_size(25*cm), nSegX(8), nSegY(4), scint6LiLoading(0.007),
tank_ui_dir("/geom/tank/"),
nsegXcmd("/geom/tank/nSegX",this),
nsegYcmd("/geom/tank/nSegY",this),
segWcmd("/geom/tank/segSize",this),
scint6Licmd("/geom/tank/scint6Li",this),
tank_vis(G4Colour(0.6,0.6,0.6)),
scint_vis(G4Colour(0.5,0.5,1.0)) {
    
    tank_ui_dir.SetGuidance("Scintillator tank settings");
    tank_ui_dir.AvailableForStates(G4State_PreInit);
    
    nsegXcmd.SetGuidance("Set the number of detector segments along length");
    nsegXcmd.SetDefaultValue(nSegX);
    nsegXcmd.AvailableForStates(G4State_PreInit);
    
    nsegYcmd.SetGuidance("Set the number of detector segments in height");
    nsegYcmd.SetDefaultValue(nSegY);
    nsegYcmd.AvailableForStates(G4State_PreInit);
    
    segWcmd.SetGuidance("Set the size of each segment");
    segWcmd.SetDefaultValue(seg_size);
    segWcmd.AvailableForStates(G4State_PreInit);
    
    scint6Licmd.SetGuidance("Scintillator 6Li loading fraction by mass");
    scint6Licmd.SetDefaultValue(scint6LiLoading);
    scint6Licmd.AvailableForStates(G4State_PreInit);
    
    addChild(&mySeparator);
    addChild(&mySlottedRod);
}

void ScintTankBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &nsegXcmd)      nSegX = nsegXcmd.GetNewIntValue(newValue);
    else if(command == &nsegYcmd) nSegY = nsegYcmd.GetNewIntValue(newValue);
    else if(command == &segWcmd)  seg_size = segWcmd.GetNewDoubleValue(newValue);
    else if(command == &scint6Licmd)  scint6LiLoading = scint6Licmd.GetNewDoubleValue(newValue);
    else G4cerr << "Unknown command!" << G4endl;
}

void ScintTankBuilder::construct() {
    
    dim = G4ThreeVector(getWidthX(), getWidthY(), tank_depth);
    
    ///////////////
    // acrylic tank
    G4Box* tank_box = new G4Box("tank_box", getWidthX()/2., getWidthY()/2., tank_depth/2.);
    main_log = new G4LogicalVolume(tank_box, MaterialsHelper::M().PMMA_black, "ScintTank_main_log");
    main_log->SetVisAttributes(&tank_vis);
    
    /////////////////////////////
    // liquid scintillator volume
    
    G4Box* scint_box = new G4Box("scint_box", getWidthX()/2.-tank_wall_thick, getWidthY()/2.-tank_wall_thick, tank_depth/2.);
    scint_log = new G4LogicalVolume(scint_box, MaterialsHelper::M().get6LiLS(scint6LiLoading), "ScintTank_scint_log");
    scint_log->SetVisAttributes(&scint_vis);
    G4PVPlacement* scint_phys = new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintTank_scint_phys", main_log, false, 0, false);
    
    ////////////////////////////
    // rods & separators lattice
    
    double sep_gap = mySlottedRod.r_inner + 1.*mm; // gap between edge of separators and center of rods
    mySeparator.width = seg_size - 2*sep_gap;
    mySeparator.length = tank_depth;
    mySeparator.construct();
    mySlottedRod.construct(tank_depth, sep_gap, mySeparator.getThick());
    
    G4ThreeVector r0(-0.5*nSegX*seg_size, -0.5*nSegY*seg_size, 0);      // starting point for rod placement
    G4ThreeVector sx0 = r0 + G4ThreeVector(seg_size/2., 0, 0);          // starting point for x-aligned separators
    G4ThreeVector sy0 = r0 + G4ThreeVector(0, seg_size/2., 0);          // starting point for y-aligned separators
    G4RotationMatrix* rotSepX = new G4RotationMatrix();                 // rotation for x-aligned separators (memory leaked!)
    rotSepX->rotateX(90*deg);
    G4RotationMatrix* rotSepY = new G4RotationMatrix();                 // rotation for y-aligned separators (memory leaked!)
    rotSepY->rotateX(90*deg); rotSepY->rotateY(90*deg);
    std::vector<G4PVPlacement*> seps;
    for(uint nx = 0; nx <= nSegX; nx++) {
        for(uint ny = 0; ny <= nSegY; ny++) {
            
            uint copynum = nx + (nSegX+1)*ny;
            
            G4PVPlacement* rod = new G4PVPlacement (NULL, r0 + G4ThreeVector(nx*seg_size, ny*seg_size, 0),
                                                    mySlottedRod.main_log, "ScintTank_rod_phys_"+to_str(copynum),
                                                    scint_log, true, copynum, true);
            new G4LogicalBorderSurface("RodOpticalBorder_"+to_str(copynum), scint_phys, rod, mySlottedRod.myOptSurf.S);
            
            if(nx < nSegX)
                seps.push_back(new G4PVPlacement (rotSepX, sx0 + G4ThreeVector(nx*seg_size, ny*seg_size, 0),
                                                  mySeparator.main_log, "ScintTank_sepX_phys"+to_str(copynum),
                                                  scint_log, true, copynum, true));
            if(ny < nSegY)
                seps.push_back(new G4PVPlacement (rotSepY, sy0 + G4ThreeVector(nx*seg_size, ny*seg_size, 0),
                                                  mySeparator.main_log, "ScintTank_sepY_phys"+to_str(copynum),
                                                  scint_log, true, copynum, true));
        }
    }
    
    // apply separator boundary reflections
    for(uint i=0; i<seps.size(); i++)
        new G4LogicalBorderSurface("SepOpticalBorder_"+to_str(i), scint_phys, seps[i], mySeparator.myOptSurf.S);
}

G4ThreeVector ScintTankBuilder::getSegmentPosition(uint n) const {
    assert(n<getNSeg());
    return G4ThreeVector( ((n%nSegX) - 0.5*nSegX + 0.5)*seg_size, ((n/nSegX) - 0.5*nSegY + 0.5)*seg_size, 0 );
}

int ScintTankBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    int nx = floor(pos[0]/seg_size + nSegX/2.);
    int ny = floor(pos[1]/seg_size + nSegY/2.);
    if(nx < 0 || nx >= (int)nSegX || ny < 0 || ny >= (int)nSegY) return -1;
    return nx + nSegX*ny;
}

void ScintTankBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    addAttr(E, "seg_size", G4BestUnit(seg_size,"Length"));
    addAttr(E, "wall", G4BestUnit(tank_wall_thick,"Length"));
    addAttr(E, "buffer", G4BestUnit(ls_buffer_thick,"Length"));
    addAttr(E, "scint", scint_log->GetMaterial()->GetName());
    addAttrI(E, "nSegX", nSegX);
    addAttrI(E, "nSegY", nSegY);
}
