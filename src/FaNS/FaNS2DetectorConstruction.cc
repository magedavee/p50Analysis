#include "FaNS2DetectorConstruction.hh"

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>
#include <G4Box.hh>
#include <G4SDManager.hh>
#include <G4RotationMatrix.hh>

double FaNS2ScintArrayBuilder::array_spacing = 11*cm;

FaNS2ScintArrayBuilder::FaNS2ScintArrayBuilder(): ScintSegVol("ScintArray"), l_bar(0.56*m), w_bar(9*cm),
theta(25*degree), cos_pw(cos(theta)), sin_pw(sin(theta)) { }

void FaNS2ScintArrayBuilder::construct() {
    dim = G4ThreeVector(w_bar, w_bar, l_bar);
    G4Box* scint_box = new G4Box("scint_box", dim[0]/2, dim[1]/2, dim[2]/2);
    scint_log = new G4LogicalVolume(scint_box, MaterialsHelper::M().PVT, "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.5,0.2,1.0,0.5)));
    
    G4RotationMatrix* scintRot = new G4RotationMatrix();
    scintRot->rotateZ(theta);
    scintRot->rotateX(90*degree);
    
    for(int nx=0; nx<ngrid; nx++) {
        for(int ny=0; ny<ngrid; ny++) {
            G4ThreeVector scintPos((nx+0.5-ngrid/2.)*array_spacing, 0, (ny+0.5-ngrid/2.)*array_spacing);
            myAssembly.AddPlacedVolume(scint_log, scintPos, scintRot);
        }
    }
}

int FaNS2ScintArrayBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    // tube lattice coordinates
    int nx = floor(pos[0]/array_spacing + ngrid/2.);
    int ny = floor(pos[1]/array_spacing + ngrid/2.);
    
    // position relative to tube lattice segment center
    G4TwoVector x(pos[0] - (nx + 0.5 - 0.5*ngrid)*array_spacing, pos[1] - (ny + 0.5 - 0.5*ngrid)*array_spacing);
    // rotated to pinwheeled segment coordinates
    x = G4TwoVector(cos_pw*x[0]-sin_pw*x[1], sin_pw*x[0]+cos_pw*x[1]);
    // adjust coordinates in adjacent squares
    if(x[0] > w_bar/2) nx++;
    else if(x[0] < -w_bar/2) nx--;
    if(x[1] > w_bar/2) ny++;
    else if(x[1] < -w_bar/2) ny--;
    
    return nx + ngrid*ny;
}


/////////////////////////
/////////////////////////
/////////////////////////

FaNS23HeArrayBuilder::FaNS23HeArrayBuilder(): ScintSegVol("3HeArray"), l_tube(0.47*m), r_tube(0.5*in), t_tube(5*mm) { }

void FaNS23HeArrayBuilder::construct() {
    dim = G4ThreeVector(2*r_tube, 2*r_tube, l_tube);
    G4Tubs* al_tube = new G4Tubs("al_tube", 0, r_tube, l_tube/2, 0, 2*M_PI);
    G4Tubs* he_tube = new G4Tubs("he_tube", 0, r_tube-t_tube, l_tube/2-t_tube, 0, 2*M_PI);
    G4LogicalVolume* tube_log = new G4LogicalVolume(al_tube, MaterialsHelper::M().nat_Al, "tube_log");
    tube_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.8,0.8,0.8,0.3)));
    scint_log = new G4LogicalVolume(he_tube, MaterialsHelper::M().get3He(4*68*mg/cm3), "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.8)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), scint_log, "3He_phys", tube_log, false, 0, true);
    
    const double sp = FaNS2ScintArrayBuilder::array_spacing;
    for(int nx=0; nx<ngrid; nx++) {
        for(int ny=0; ny<ngrid; ny++) {
            if((nx == 0 || nx == ngrid-1) && (ny == 0 || ny == ngrid-1)) continue;
            G4ThreeVector tubePos((nx+0.5-ngrid/2.)*sp, 0, (ny+0.5-ngrid/2.)*sp);
            myAssembly.AddPlacedVolume(tube_log, tubePos, rot_X_90);
        }
    }
}

int FaNS23HeArrayBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    const double sp = FaNS2ScintArrayBuilder::array_spacing;
    int nx = floor(pos[0]/sp + ngrid/2.);
    int ny = floor(pos[1]/sp + ngrid/2.);
    return 16 + nx + ngrid*ny;
}

/////////////////////////
/////////////////////////
/////////////////////////


FaNS2MuVetoBuilder::FaNS2MuVetoBuilder(): ScintSegVol("MuVeto"), w_slab(1*m), t_slab(1*in) { }

void FaNS2MuVetoBuilder::construct() {
    dim = G4ThreeVector(w_slab,w_slab,t_slab);
    G4Box* slab_box = new G4Box("slab_box", dim[0]/2, dim[1]/2, dim[2]/2);
    
    scint_log = new G4LogicalVolume(slab_box, MaterialsHelper::M().Polystyrene, "scint_log");
    scint_log->SetVisAttributes(new G4VisAttributes(G4Colour(0.5,1.0,1.0,0.3)));
    
    G4ThreeVector v1pos(0,0,0.4*m+t_slab/2);
    myAssembly.AddPlacedVolume(scint_log, v1pos, NULL);
    G4ThreeVector v2pos(0,0,v1pos[2]+t_slab+2*mm);
    myAssembly.AddPlacedVolume(scint_log, v2pos, NULL);
}

/////////////////////////
/////////////////////////
/////////////////////////


G4VPhysicalVolume* FaNS2DetectorConstruction::Construct() {
        
    myScintArray.construct(); addChild(&myScintArray);
    my3HeArray.construct(); addChild(&my3HeArray);
    myVeto.construct(); addChild(&myVeto);
    
    // world
    dim = G4ThreeVector(1.5*m, 1.5*m, 1.5*m);
    G4Box* worldBox = new G4Box("worldBox", dim[0]/2, dim[1]/2, dim[2]/2);
    main_log = new G4LogicalVolume(worldBox, MaterialsHelper::M().Vacuum, "main_log");
    theWorld = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), main_log, "world_phys", NULL, false,  0);
    
    // components placement
    G4ThreeVector centerpos(0, 0, 0);
    myScintArray.myAssembly.MakeImprint(main_log, centerpos, NULL, 0, true);
    my3HeArray.myAssembly.MakeImprint(main_log, centerpos, NULL, 0, true);
    myVeto.myAssembly.MakeImprint(main_log, centerpos, NULL, 0, true);
    
    myScintSD = new ScintSD("ScintSD", myScintArray, theWorld);
    G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
    myScintArray.setScintSD(myScintSD);
    my3HeArray.setScintSD(myScintSD);
    myVeto.setScintSD(myScintSD);
    
    return theWorld;
}
