#include "ScintSegVol.hh"
#include "strutils.hh"
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <cassert>

void ScintSegVol::fillNode(TXMLEngine& E) {
    Builder::fillNode(E);
    if(scint_log) {
        addAttr(E, "scint", scint_log->GetMaterial()->GetName());
        addAttr(E,"e_density", to_str(scint_log->GetMaterial()->GetElectronDensity()/(6.022e23/cm3))+" mol/cm^3");
    }
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
