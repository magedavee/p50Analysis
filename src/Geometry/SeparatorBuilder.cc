#include "SeparatorBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <cassert>

SeparatorBuilder::SeparatorBuilder(): XMLProvider("Separator"), main_log(NULL),
totalThick(5*mm), cfThick(3*mm), width(0), length(0), sep_vis(G4Colour(0.9,0.9,0.9)) {
    addChild(&myOptSurf);
    myOptSurf.refl = 0.96;
}

void SeparatorBuilder::construct(double w, double l) {
    myOptSurf.construct();
    
    width = w;
    length = l;
    assert(cfThick < totalThick);
    
    G4Box* main_box = new G4Box("SeparatorMainBox", width/2., length/2., totalThick/2.);    
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().PMMA, "Separator_main_Log");
    main_log->SetVisAttributes(&sep_vis);
}

void SeparatorBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "thick", G4BestUnit(totalThick,"Length"));
    addAttr(E, "len", G4BestUnit(length,"Length"));
    addAttr(E, "wid", G4BestUnit(width,"Length"));
}
