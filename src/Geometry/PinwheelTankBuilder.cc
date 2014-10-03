#include "PinwheelTankBuilder.hh"

#include <cmath>

PinwheelTankBuilder::PinwheelTankBuilder(): ScintTankBuilder("ScintTank") {
    addChild(&myPinwheelRod);
}

void PinwheelTankBuilder::setupDividers() {
    myRod = &myPinwheelRod;
    myPinwheelRod.t_panel = mySeparator.totalThick;
    myPinwheelRod.length = tank_depth - 2*gc_thick;
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
