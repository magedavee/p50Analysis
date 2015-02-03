#include "VertexPositioner.hh"

#include <Randomize.hh>
#include <G4UnitsTable.hh>

G4ThreeVector VertexPositioner::randomDirection() {
    double phi = 2.*M_PI*G4UniformRand();
    double costheta = 2.*G4UniformRand()-1.;
    double sintheta = sqrt(1.-costheta*costheta);
    return G4ThreeVector(cos(phi)*sintheta, sin(phi)*sintheta, costheta);
}

int VertexPositioner::setVertex(vector<primaryPtcl>& v) {
    int ntries = 0;
    vector<primaryPtcl> vorig = v;
    do {
        v = vorig;
        proposePosition();
        ntries++;
    } while(!tryVertex(v));
    return ntries;
}

//---------------------------

bool IsotPtPositioner::tryVertex(vector<primaryPtcl>& v) {
    for(auto it = v.begin(); it != v.end(); it++) {
        it->pos = pos;
        if(!it->mom.mag2()) it->mom = randomDirection();
    }
    return true;
}

void IsotPtPositioner::fillNode(TXMLEngine& E) {
    addAttr(E, "pos", G4BestUnit(pos,"Length"));
}
