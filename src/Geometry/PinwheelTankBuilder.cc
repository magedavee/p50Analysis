#include "PinwheelTankBuilder.hh"

#include <cmath>

#include <G4ExtrudedSolid.hh>

PinwheelTankBuilder::PinwheelTankBuilder(): ScintTankBuilder("ScintTank") {
    addChild(&myPinwheelRod);
}

void PinwheelTankBuilder::setupDividers() {
    myRod = &myPinwheelRod;
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
}

void PinwheelTankBuilder::setupGammaCatcher() {
    // catcher cross-section
    double gc_hw = 0.5*(seg_size-mySeparator.totalThick);
    double slotw = myPinwheelRod.l_hook + myPinwheelRod.t_end;
    std::vector<G4TwoVector> vertices;
    vertices.push_back(G4TwoVector(-gc_hw, gc_hw-slotw));
    vertices.push_back(G4TwoVector(-gc_hw+myPinwheelRod.t_hook, gc_hw-slotw));
    vertices.push_back(G4TwoVector(-gc_hw+myPinwheelRod.t_hook, gc_hw));
    rotvert(vertices,4);
    G4ExtrudedSolid* extruded = new G4ExtrudedSolid("gc_extrusion", vertices, gc_thick/2, G4TwoVector(), 1, G4TwoVector(), 1);
    gammacatcher_log = new G4LogicalVolume(extruded, MaterialsHelper::M().PMMA, "gammacatcher_log");
}
