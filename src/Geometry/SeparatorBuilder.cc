#include "SeparatorBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <cassert>

SeparatorBuilder::SeparatorBuilder(): Builder("Separator"),
width(0), length(0), totalThick(5*mm), cfThick(3*mm), sep_vis(G4Colour(0.9,0.9,0.9)) {
    addChild(&myOptSurf);
    myOptSurf.refl = 0.96;
}

void SeparatorBuilder::construct() {
    myOptSurf.construct();
    
    dim = G4ThreeVector(width, length, totalThick);
    assert(cfThick < totalThick);
    
    G4Box* main_box = new G4Box("SeparatorMainBox", width/2., length/2., totalThick/2.);    
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().PMMA, "Separator_main_Log");
    main_log->SetVisAttributes(&sep_vis);
}

void SeparatorBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
