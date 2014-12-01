#include "ScintSegVol.hh"

#include <G4UnitsTable.hh>

void ScintSegVol::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    if(scint_log) addAttr(E, "scint", scint_log->GetMaterial()->GetName());
}
