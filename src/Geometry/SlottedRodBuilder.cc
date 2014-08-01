#include "SlottedRodBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>

#include <cmath>
#include <cassert>

SlottedRodBuilder::SlottedRodBuilder(): XMLProvider("SlottedRod"), main_log(NULL),
r_outer(9.5*mm), r_inner(4*mm), length(0), r_slot(0), w_slot(0),
rod_vis(G4Colour(1.0,1.0,0.5)) { }

void SlottedRodBuilder::construct(double l, double rslot, double wslot) {
    length = l;
    r_slot = rslot;
    w_slot = wslot;
    assert(r_inner < r_slot && r_slot < r_outer);
    
    G4Tubs* rod_tube = new G4Tubs("rod_tube", r_inner, r_outer, length/2., 0, 2*M_PI);
    G4Box* cutout_box = new G4Box("cutout_box", r_outer, w_slot/2., length);
    G4VSolid* rod_cut = rod_tube;
    double ltrans = r_outer + r_slot;
    int nslots = 4;
    for(int i=0; i<nslots; i++) {
        G4RotationMatrix* R = new G4RotationMatrix();
        double theta = (360./nslots)*i*deg; // Geant4 native units are radians
        R->rotateZ(theta);
        rod_cut = new G4SubtractionSolid("rod_cut", rod_cut, cutout_box, R, G4ThreeVector(cos(theta)*ltrans, sin(theta)*ltrans, 0));
    }
    
    main_log = new G4LogicalVolume(rod_cut, MaterialsHelper::M().PEEK, "SlottedRod_main_Log");
    main_log->SetVisAttributes(&rod_vis);
}

void SlottedRodBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "r_out", G4BestUnit(r_outer,"Length"));
    addAttr(E, "r_in", G4BestUnit(r_inner,"Length"));
    addAttr(E, "r_slot", G4BestUnit(r_slot,"Length"));
    addAttr(E, "w_slot", G4BestUnit(w_slot,"Length"));
    addAttr(E, "len", G4BestUnit(length,"Length"));
}
