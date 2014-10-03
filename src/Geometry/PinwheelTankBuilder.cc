#include "PinwheelTankBuilder.hh"
#include "Utilities.hh"

#include <cassert>
#include <vector>
#include <cmath>

#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4TwoVector.hh>
#include <G4PVPlacement.hh>
#include <G4LogicalBorderSurface.hh>

PinwheelTankBuilder::PinwheelTankBuilder(): ScintTankBuilder("ScintTank") {
    addChild(&myPinwheelRod);
}

void PinwheelTankBuilder::construct() {
    
    ///////////////////////////////
    // rods & separators dimensions
    
    myPinwheelRod.t_panel = mySeparator.totalThick;
    myPinwheelRod.length = tank_depth;
    myPinwheelRod.construct();
    
    mySeparator.width = seg_size + myPinwheelRod.w_inner - 2*myPinwheelRod.t_end;
    mySeparator.length = tank_depth;
    mySeparator.construct();
    
    // rod lattice spacing
    double w_eff = myPinwheelRod.w_inner + myPinwheelRod.t_panel;
    lat_size = sqrt(w_eff*w_eff + seg_size*seg_size);
    // rotation angle
    theta_pw = atan(w_eff/seg_size);
    cos_pw = cos(theta_pw);
    sin_pw = sin(theta_pw);
    rotRod.rotateZ(theta_pw);
    
    dim = G4ThreeVector(nSegX*lat_size+2*(tank_wall_thick+ls_buffer_thick), 
                        nSegY*lat_size+2*(tank_wall_thick+ls_buffer_thick),
                        tank_depth);
    
    ///////////////
    // acrylic tank
    G4Box* tank_box = new G4Box("tank_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(tank_box, MaterialsHelper::M().PMMA_black, "ScintTank_main_log");
    main_log->SetVisAttributes(&tank_vis);
    
    /////////////////////////////
    // liquid scintillator volume
    
    G4Box* scint_box = new G4Box("scint_box", dim[0]/2.-tank_wall_thick, dim[1]/2.-tank_wall_thick, dim[2]/2.);
    scint_log = new G4LogicalVolume(scint_box, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, scint6LiLoading), "ScintTank_scint_log");
    scint_log->SetVisAttributes(&scint_vis);
    G4PVPlacement* scint_phys = new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintTank_scint_phys", main_log, false, 0, false);
    
    
    ///////////////////
    // pinwheel lattice
    
    G4ThreeVector r0(-0.5*nSegX*lat_size, -0.5*nSegY*lat_size, 0);      // starting point for rod placement
    G4ThreeVector sx0 = r0 + G4ThreeVector(lat_size/2., 0, 0);          // starting point for x-aligned separators
    G4ThreeVector sy0 = r0 + G4ThreeVector(0, lat_size/2., 0);          // starting point for y-aligned separators
    G4RotationMatrix* rotSepX = new G4RotationMatrix();                 // rotation for x-aligned separators (memory leaked!)
    rotSepX->rotateX(90*deg); rotSepX->rotateY(-theta_pw);
    G4RotationMatrix* rotSepY = new G4RotationMatrix();                 // rotation for y-aligned separators (memory leaked!)
    rotSepY->rotateX(90*deg); rotSepY->rotateY(90*deg-theta_pw);
    std::vector<G4PVPlacement*> seps;
    for(unsigned int nx = 0; nx <= nSegX; nx++) {
        for(unsigned int ny = 0; ny <= nSegY; ny++) {
            
            unsigned int copynum = nx + (nSegX+1)*ny;
            
            G4PVPlacement* rod = new G4PVPlacement(&rotRod, r0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                    myPinwheelRod.main_log, "ScintTank_rod_phys_"+to_str(copynum),
                                                    scint_log, true, copynum, true);
            new G4LogicalBorderSurface("RodOpticalBorder_"+to_str(copynum), scint_phys, rod, myPinwheelRod.myOptSurf.S);
            
            if(nx < nSegX)
                seps.push_back(new G4PVPlacement(rotSepX, sx0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                  mySeparator.main_log, "ScintTank_sepX_phys"+to_str(copynum),
                                                  scint_log, true, copynum, true));
            if(ny < nSegY)
                seps.push_back(new G4PVPlacement(rotSepY, sy0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                  mySeparator.main_log, "ScintTank_sepY_phys"+to_str(copynum),
                                                  scint_log, true, copynum, true));
        }
    }
    
    // apply separator boundary reflections
    for(unsigned int i=0; i<seps.size(); i++)
        new G4LogicalBorderSurface("SepOpticalBorder_"+to_str(i), scint_phys, seps[i], mySeparator.myOptSurf.S);
}

G4ThreeVector PinwheelTankBuilder::getSegmentPosition(unsigned int n) const {
    assert(n<getNSeg());
    return G4ThreeVector( ((n%nSegX) - 0.5*nSegX + 0.5)*lat_size, ((n/nSegX) - 0.5*nSegY + 0.5)*lat_size, 0 );
}

int PinwheelTankBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    // rod lattice coordinates
    int nx = floor(pos[0]/lat_size + nSegX/2.);
    int ny = floor(pos[1]/lat_size + nSegY/2.);
    
    // relative to rod lattice segment center
    G4TwoVector x(pos[0] - (nx + 0.5 - 0.5*nSegX)*lat_size, pos[1] - (ny + 0.5 - 0.5*nSegY)*lat_size);
    // rotated to pinwheeled segment coordinates
    x = G4TwoVector(cos_pw*x[0]-sin_pw*x[1], sin_pw*x[0]+cos_pw*x[1]);
    
    // adjust coordinates in adjacent squares
    if(x[0] > seg_size/2) nx++;
    else if(x[0] < -seg_size/2) nx--;
    if(x[1] > seg_size/2) ny++;
    else if(x[1] < -seg_size/2) ny--;
    
    if(nx < 0 || nx >= (int)nSegX || ny < 0 || ny >= (int)nSegY) return -1;
    
    return nx + nSegX*ny;
}

void PinwheelTankBuilder::fillNode(TXMLEngine& E) {
    ScintTankBuilder::fillNode(E);
    addAttr(E, "lat_size", G4BestUnit(lat_size,"Length"));
}
