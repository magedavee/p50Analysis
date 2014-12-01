#include "FNSDetectorConstruction.hh"

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SDManager.hh>

double FNSArrayBuilder::spacing = 6*cm;

FNSArrayBuilder::FNSArrayBuilder(): ScintSegVol("ScintArray"),
l_seg(25*cm), r_seg(2*cm), t_seg(2*mm) { }

void FNSArrayBuilder::construct() {
    dim = G4ThreeVector(2*r_seg, 2*r_seg, l_seg);
    G4Tubs* glass_tube = new G4Tubs("glass_tube", 0, r_seg, l_seg/2, 0, 2*M_PI);
    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, r_seg-t_seg, l_seg/2-t_seg, 0, 2*M_PI);
    G4LogicalVolume* tube_log = new G4LogicalVolume(glass_tube, MaterialsHelper::M().Pyrex, "tube_log");
    tube_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.15)));
    scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, 0.001, true), "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.4,0.2,1.0,0.3)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), scint_log, "scint_phys", tube_log, false, 0, true);
    
    for(int nx=0; nx<ngrid; nx++) {
        for(int ny=0; ny<ngrid; ny++) {
            G4ThreeVector tubePos((nx+0.5-ngrid/2.)*spacing, 0, (ny+0.5-ngrid/2.)*spacing);
            myAssembly.AddPlacedVolume(tube_log, tubePos, rot_X_90);
        }
    }
}

int FNSArrayBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    int nx = floor(pos[0]/spacing + ngrid/2.);
    int ny = floor(pos[1]/spacing + ngrid/2.);
    return 16 + nx + ngrid*ny;
}

/////////////////////////
/////////////////////////
/////////////////////////

G4VPhysicalVolume* FNSDetectorConstruction::Construct() {
        
    myArray.construct(); addChild(&myArray);
    
    // world
    dim = G4ThreeVector(0.5*m, 0.5*m, 0.5*m);
    G4Box* worldBox = new G4Box("worldBox", dim[0]/2, dim[1]/2, dim[2]/2);
    main_log = new G4LogicalVolume(worldBox, MaterialsHelper::M().Vacuum, "main_log");
    theWorld = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), main_log, "world_phys", NULL, false,  0);
    
    // components placement
    G4ThreeVector centerpos(0, 0, 0);
    myArray.myAssembly.MakeImprint(main_log, centerpos, NULL, 0, true);
   
    myScintSD = new ScintSD("ScintSD", myArray, theWorld);
    G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
    myArray.setScintSD(myScintSD);
    
    return theWorld;
}
