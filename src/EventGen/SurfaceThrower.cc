#include "SurfaceThrower.hh"

#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>
#include <Randomize.hh>

void SurfaceThrower::setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT) {
    S = SS;
    T = TT;
    W2S.setParentChild(W,S);
    W2T.setParentChild(W,T);
}

bool SurfaceThrower::genThrow() {
    assert(S && T);
    if(!S || !T) return false;
    
    // propose throw direction
    mom = proposeDirection();
    
    // propose source surface point.
    // Note: this won't be right if solid doesn't return uniformly distributed points!
    pos = S->GetLogicalVolume()->GetSolid()->GetPointOnSurface();
    G4ThreeVector snorm = S->GetLogicalVolume()->GetSolid()->SurfaceNormal(pos);
    pos = W2S.coordCtoP(pos);
    snorm = W2S.dirCtoP(snorm);
    
    // cos-theta surface normal condition
    double x = snorm.dot(mom);
    if(!(((outer && x>0) || (!outer && x<0)) && G4UniformRand() < fabs(x))) return false;
    
    // whether proposed event points to target surface
    return W2T.intersects(pos,mom);
}

G4ThreeVector SurfaceThrower::proposeDirection() {
    double phi = 2.*M_PI*G4UniformRand();
    double costheta = 2.*G4UniformRand()-1.;
    double sintheta = sqrt(1.-costheta*costheta);
    return G4ThreeVector(cos(phi)*sintheta, sin(phi)*sintheta, costheta);
}

