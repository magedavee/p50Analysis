#include "CosineThrower.hh"

#include <Randomize.hh>

G4ThreeVector CosineThrower::proposeDirection() {
    if(!x) return randomDirection();
    
    double phi = 2.*M_PI*G4UniformRand();
    double costheta;
    do { costheta = -G4UniformRand(); }
    while(G4UniformRand() > pow(fabs(costheta),x));
    double sintheta = sqrt(1.-costheta*costheta);
    return G4ThreeVector(cos(phi)*sintheta, sin(phi)*sintheta, costheta);
}

void CosineThrower::fillNode(TXMLEngine& E) {
    SurfaceThrower::fillNode(E);
    addAttr(E, "cos_exp", x);
}
