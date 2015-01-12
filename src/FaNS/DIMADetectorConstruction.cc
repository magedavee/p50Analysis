#include "DIMADetectorConstruction.hh"

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SDManager.hh>

double DIMAArrayBuilder::spacing = 6*cm;

DIMAArrayBuilder::DIMAArrayBuilder(): ScintSegVol("ScintArray"),
l_seg(15*cm), r_seg(1.5*cm), t_seg(2.5*mm), t_guide(12*mm) { }

void DIMAArrayBuilder::construct() {
    double array_width = (ngrid-1)*spacing+2*r_seg;
    dim = G4ThreeVector(array_width, l_seg+2*t_guide, array_width);
    
    G4Tubs* lg_tube = new G4Tubs("lg_tube", 0, r_seg, l_seg/2+t_guide, 0, 2*M_PI);
    G4Tubs* glass_tube = new G4Tubs("glass_tube", 0, r_seg, l_seg/2, 0, 2*M_PI);
    G4Tubs* scint_tube = new G4Tubs("scint_tube", 0, r_seg-t_seg, l_seg/2, 0, 2*M_PI);
    
    G4LogicalVolume* tube_log = new G4LogicalVolume(lg_tube, MaterialsHelper::M().PMMA, "tube_log");
    tube_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.15)));
    G4LogicalVolume* glass_log = new G4LogicalVolume(glass_tube, MaterialsHelper::M().Quartz, "glass_log");
    glass_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.0,0.15)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), glass_log, "glass_phys", tube_log, false, 0, true);
    //scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, 0.001, true), "scint_log");
    scint_log = new G4LogicalVolume(scint_tube, MaterialsHelper::M().EJ309, "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.4,0.2,1.0,0.3)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), scint_log, "scint_phys", glass_log, false, 0, true);
    
    for(int nx=0; nx<ngrid; nx++) {
        for(int ny=0; ny<ngrid; ny++) {
            G4ThreeVector tubePos((nx+0.5-ngrid/2.)*spacing, 0, (ny+0.5-ngrid/2.)*spacing);
            myAssembly.AddPlacedVolume(tube_log, tubePos, rot_X_90);
        }
    }
}

int DIMAArrayBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    int nx = floor(pos[0]/spacing + ngrid/2.);
    int nz = floor(pos[2]/spacing + ngrid/2.);
    return nx + ngrid*nz;
}

/////////////////////////
/////////////////////////
/////////////////////////

void DIMABoxBuilder::_construct() {
    dim = myArray.getDimensions() + G4ThreeVector(14*cm, 30*cm, 8*cm);
    addLayer(ShellLayerSpec(dim*0.5, dim*0.5, MaterialsHelper::M().Air, G4Colour(0.7,0.0,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*mm, 2*mm, 2*mm), G4ThreeVector(2*mm, 2*mm, 2*mm), MaterialsHelper::M().SS444, G4Colour(0.7,0.0,0.7,0.5)));
    construct_layers();
}

/////////////////////////
/////////////////////////
/////////////////////////

G4VPhysicalVolume* DIMADetectorConstruction::Construct() {
    
    // world
    dim = G4ThreeVector(0.5*m, 0.5*m, 0.5*m);
    G4Box* worldBox = new G4Box("worldBox", dim[0]/2, dim[1]/2, dim[2]/2);
    main_log = new G4LogicalVolume(worldBox, MaterialsHelper::M().Vacuum, "main_log");
    theWorld = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), main_log, "world_phys", NULL, false,  0);
    
    myBox.construct(); addChild(&myBox);
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), myBox.main_log, "box_phys", main_log, false, 0, true);
    
    myScintSD = new ScintSD("ScintSD", myBox.myArray, theWorld);
    G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
    myBox.myArray.setScintSD(myScintSD);
    
    return theWorld;
}
