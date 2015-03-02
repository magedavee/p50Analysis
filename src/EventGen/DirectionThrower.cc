#include "DirectionThrower.hh"

void DirectionThrower::fillNode(TXMLEngine& E) {
    SurfaceThrower::fillNode(E);
    addAttr(E, "dx", d[0]);
    addAttr(E, "dy", d[1]);
    addAttr(E, "dz", d[2]);
}
