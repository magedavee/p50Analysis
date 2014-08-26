#include "GeomHelper.hh"

#include <G4LogicalVolume.hh>
#include <G4VSolid.hh>

#include <cassert>

void GeomHelper::calcChildOffset() {
    rotCtoP = rotPtoC = G4RotationMatrix();
    transCtoP = G4ThreeVector();
    
    if(!P || !C) return;
    
    while(P != C) {
        G4LogicalVolume* currentLogical = P->GetLogicalVolume();
        G4int nC = currentLogical->GetNoDaughters();
        P = C;
        for(G4int i = 0; i < nC; i++) {
            P = currentLogical->GetDaughter(i);
            if(P->GetLogicalVolume()->IsAncestor(C) || P == C) break;
        }
        transCtoP += P->GetObjectTranslation();
        rotCtoP *= P->GetObjectRotationValue();
    }
    
    rotPtoC = rotCtoP.inverse();
}

G4ThreeVector GeomHelper::coordCtoP(G4ThreeVector x) const {
    x += transCtoP;
    x *= rotCtoP;
    return x;
}

G4ThreeVector GeomHelper::coordPtoC(G4ThreeVector x) const {
    x -= transCtoP;
    x *= rotPtoC;
    return x;
}

bool GeomHelper::intersects(G4ThreeVector x, G4ThreeVector d) {
    if(!C) return false;
    x = coordPtoC(x);
    d *= rotPtoC;
    return C->GetLogicalVolume()->GetSolid()->DistanceToIn(x, d) != kInfinity;
}
