#include "SeparatorBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4Box.hh>
#include <cassert>

SeparatorBuilder::SeparatorBuilder(): main_log(NULL),
totalThick(5*mm), cfThick(3*mm), width(0), length(0), sep_vis(G4Colour(0.9,0.9,0.9)) { }

void SeparatorBuilder::construct(double w, double l) {
    width = w;
    length = l;
    assert(cfThick < totalThick);
    
    G4Box* main_box = new G4Box("SeparatorMainBox", width/2., length/2., totalThick/2.);    
    main_log = new G4LogicalVolume(main_box, MaterialsHelper::M().PMMA, "Separator_main_Log");
    main_log->SetVisAttributes(&sep_vis);
}
