#include "SurfaceThrower.hh"
#include "SMExcept.hh"

#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>
#include <Randomize.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>

SurfaceThrower::SurfaceThrower(G4VPhysicalVolume* w, G4VPhysicalVolume* SS, G4VPhysicalVolume* TT, const string& n):
VertexPositioner(n), W(w) {
    if(!W) throw SMExcept("undefinedWorldVolume");
    setSourceTarget(SS,TT);
}

void SurfaceThrower::setSourceTarget(G4VPhysicalVolume* SS, G4VPhysicalVolume* TT) {
    S = SS;
    if(!S) { S = W; outer = fromVolume = false; }
    Ssolid = S->GetLogicalVolume()->GetSolid();
    W2S.setParentChild(W,S);
    sourceExtent = Ssolid->GetExtent();
    sourceRadius = sourceExtent.GetExtentRadius();
    sourceCenter = W2S.coordCtoP(sourceExtent.GetExtentCenter());
    
    T = TT;
    if(T) {
        W2T.setParentChild(W,T);
        Tsolid = T->GetLogicalVolume()->GetSolid();
    } else {
        W2T.setParentChild(W,W);
        Tsolid = W->GetLogicalVolume()->GetSolid();
    }
    targetExtent = Tsolid->GetExtent();
    targetRadius = targetExtent.GetExtentRadius();
    targetCenter = W2T.coordCtoP(targetExtent.GetExtentCenter());
    
    rMin = (sourceCenter-targetCenter).mag();
    rMax = rMin + sourceRadius + targetRadius;
    rMin -= sourceRadius + targetRadius;
    rMin = (rMin>0)? rMin : 0;
}

G4ThreeVector pointInSolid(const G4VisExtent& E, const G4VSolid* S) {
    assert(S);
    G4ThreeVector v;
    do {
        double x = G4UniformRand(); x = E.GetXmin()*(1-x) + E.GetXmax()*x;
        double y = G4UniformRand(); y = E.GetYmin()*(1-y) + E.GetYmax()*y;
        double z = G4UniformRand(); z = E.GetZmin()*(1-z) + E.GetZmax()*z;
        v = G4ThreeVector(x,y,z);
    } while (S->Inside(v) != kInside);
    return v;
}

void SurfaceThrower::proposePosition() {
    while(true) {
        if(fromVolume) pos = pointInSolid(sourceExtent, Ssolid);
        else  {
            // Note: this won't be right if solid doesn't return uniformly distributed points!
            pos = Ssolid->GetPointOnSurface();
            snorm = Ssolid->SurfaceNormal(pos);
        }
        pos = W2S.coordCtoP(pos);
        
        if(reScatter) {
            originPoint = pos;
            G4ThreeVector d = proposeDirection();
            bool passNormal = true;
            if(!fromVolume) {
                double x = snorm.dot(d);
                passNormal = ((outer && x>0) || (!outer && x<0)) && G4UniformRand() < fabs(x);
            }
            nSurfaceThrows += passNormal;
            if(!passNormal) continue;
            pos += d * (rMin + G4UniformRand()*(rMax-rMin));
            if(!T || (T && Tsolid->Inside(W2T.coordPtoC(pos)) != kInside)) continue;
        } else {
            originPoint = pos + randomDirection()*um;
        }
        break;
    }
}

bool SurfaceThrower::tryMomentum(G4ThreeVector& mom) {
    if(reScatter) {
        if(!mom.mag2()) mom = proposeRescatter();
        return true;
    }
    
    if(!mom.mag2()) mom = proposeDirection();
    bool passNormal = true;
    if(!fromVolume) {
        double x = snorm.dot(mom);
        passNormal = ((outer && x>0) || (!outer && x<0)) && G4UniformRand() < fabs(x);
    }
    nSurfaceThrows += passNormal;
    return passNormal && (!T || W2T.intersects(pos,mom));
}

bool SurfaceThrower::tryVertex(vector<primaryPtcl>& v) {
    if(!v.size()) return false;
    
    vector<primaryPtcl> thrown;
    nAttempts++;
    for(auto it = v.begin(); it != v.end(); it++) {
        primaryPtcl p = *it;
        p.pos = pos;
        if(tryMomentum(p.mom)) thrown.push_back(p);
    }
    
    nHits += thrown.size();
    if(thrown.size()) v = thrown;
    else return false;
    
    return true;
}

void SurfaceThrower::fillNode(TXMLEngine& E) {
    VertexPositioner::fillNode(E);
    if(fromVolume) addAttr(E, "s_volume", G4BestUnit(getOriginVolume(),"Volume"));
    else addAttr(E, "s_area", G4BestUnit(getOriginArea(),"Surface"));
    if(T) addAttr(E, "t_volume", G4BestUnit(getTargetVolume(),"Volume"));
    if(reScatter) {
        addAttr(E, "reScatter","true");
        addAttr(E, "ixn_length", G4BestUnit(rMax-rMin,"Length"));
    }
    addAttr(E, "nAttempts", nAttempts);
    addAttr(E, "nSurfaceThrows", nSurfaceThrows);
    addAttr(E, "nHits", nHits);    
}
