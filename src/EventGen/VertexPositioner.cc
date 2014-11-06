#include "VertexPositioner.hh"

#include <Randomize.hh>

G4ThreeVector VertexPositioner::randomDirection() {
    double phi = 2.*M_PI*G4UniformRand();
    double costheta = 2.*G4UniformRand()-1.;
    double sintheta = sqrt(1.-costheta*costheta);
    return G4ThreeVector(cos(phi)*sintheta, sin(phi)*sintheta, costheta);
}

void IsotPtPositioner::setVertex(vector<primaryPtcl>& v) {
    for(auto it = v.begin(); it != v.end(); it++) {
        it->pos = x0;
        it->mom = randomDirection();
    }
}
