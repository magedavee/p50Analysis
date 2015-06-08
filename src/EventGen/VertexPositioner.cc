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

IsotPtPositioner::IsotPtPositioner(const G4ThreeVector& x):
VertexPositioner("IsotPtPositioner"), dxn(0,0,1),
isotptDir("/vertex/isotpt/", this),
cthCmd("/vertex/isotpt/cosThetaMin", this) { 
    setPos(x);
    
    isotptDir.SetGuidance("Isotropic point positioner commands");
    isotptDir.AvailableForStates(G4State_Init, G4State_Idle);
    
    cthCmd.SetGuidance("Set minimum cos theta relative to direction vector");
    cthCmd.AvailableForStates(G4State_Init, G4State_Idle);
}

bool IsotPtPositioner::tryVertex(vector<primaryPtcl>& v){
    int nPassCosTheta = 0;
    for(auto it = v.begin(); it != v.end(); it++) {
        it->pos = pos;
        if(!it->mom.mag2()) it->mom = randomDirection();
        double cth = it->mom.dot(dxn);
        nPassCosTheta += (cth >= costheta_min);
    }
    nAttempts++;
    return nPassCosTheta;
}

void IsotPtPositioner::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &cthCmd) costheta_min = cthCmd.GetNewDoubleValue(newValue);
}

void IsotPtPositioner::fillNode(TXMLEngine& E) {
    addAttr(E, "pos", G4BestUnit(pos,"Length"));
    if(costheta_min > -1) {
        addAttr(E, "costheta_min", costheta_min);
    }
}
