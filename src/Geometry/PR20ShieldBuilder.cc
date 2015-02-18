#include "PR20ShieldBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4Box.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include "SMExcept.hh"
#include "strutils.hh"

PR20InnerShieldBuilder::PR20InnerShieldBuilder(): WaterbrickerBuilder("PROSPECT20_Inner") {
    expand_to_contents = false;
    place_centered = false;
}

void PR20InnerShieldBuilder::_construct() {    
    dim = G4ThreeVector(8*in, 72*in, 0);
    addLayer(ShellLayerSpec(dim/2+G4ThreeVector(0,0,8*in), dim/2, MaterialsHelper::M().Air, G4Colour(1.0,0,0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(4*in, 4*in, 4*in), G4ThreeVector(4*in, 4*in, 4*in), MaterialsHelper::M().BPoly5, G4Colour(0.7,0.0,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 4*in, 2*in), G4ThreeVector(4*in, 4*in, 2*in), MaterialsHelper::M().nat_Pb, G4Colour(0.5,0.5,0.5,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(2*in, 0*in, 5*in), G4ThreeVector(2*in, 0*in, 1*in), MaterialsHelper::M().BPoly5, G4Colour(0.5,0.5,0.0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(in/8, in/4, in/8), G4ThreeVector(in/8, in/4, in/8), MaterialsHelper::M().Air, G4Colour(1,1,1,0.2)));
    const double st = 3./8.*in; // aluminum shell thickness
    addLayer(ShellLayerSpec(G4ThreeVector(st,st,st), G4ThreeVector(st,st,st), MaterialsHelper::M().nat_Al, G4Colour(0.7,0.7,0.7,0.5)));
    construct_layers();
}

////////////////////////////////
////////////////////////////////

PR20ShieldBuilder::PR20ShieldBuilder(): WaterbrickerBuilder("PROSPECT20") {
    myContents = &myInnerShield;
    place_centered = false;
}

void PR20ShieldBuilder::_construct() {
    addLayer(ShellLayerSpec(G4ThreeVector(4*in, 6*in, 5*in), G4ThreeVector(4*in, 6*in, 0*in), MaterialsHelper::M().BPoly5, G4Colour(1,1,0.8,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(4*in, 4*in, 4*in), G4ThreeVector(4*in, 4*in, 0*in), MaterialsHelper::M().Polyeth, G4Colour(0.7,0.7,1.0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(in/8,in/8,in/8), G4ThreeVector(in/8,in/8,in/4), MaterialsHelper::M().nat_Al, G4Colour(0.7,0.7,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(0,0,0), G4ThreeVector(0,0,3./8.*in), MaterialsHelper::M().BPoly30, G4Colour(0.5,1.0,0.5,0.5)));
    addWaterbricks();
    construct_layers();
}
