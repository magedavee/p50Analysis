#include "ScintSegVol.hh"
#include <G4UnitsTable.hh>
#include <cassert>

void ScintSegVol::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    if(scint_log) addAttr(E, "scint", scint_log->GetMaterial()->GetName());
}

//////////////////////
//////////////////////
//////////////////////

void ScintChunk::construct() {
    assert(mySolid && myMat);
    if(!mySolid || !myMat) return;
    main_log = scint_log = new G4LogicalVolume(mySolid, myMat, (nodeName+"_log").c_str());
    if(myVis) scint_log->SetVisAttributes(myVis);
}
