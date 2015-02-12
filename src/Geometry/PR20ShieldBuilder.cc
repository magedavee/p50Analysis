#include "PR20ShieldBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4Box.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PVPlacement.hh>
#include "SMExcept.hh"
#include "strutils.hh"

PR20InnerShieldBuilder::PR20InnerShieldBuilder(): ShellLayerBuilder("PROSPECT20_Inner"),
waterBrickCmd("/geom/PR20InnerShield/waterbricks",this) {
    expand_to_contents = false;
    place_centered = false;
    
    waterBrickCmd.SetGuidance("Set whether to build water bricks shield layer.");
    waterBrickCmd.SetDefaultValue(withWaterBricks);
    waterBrickCmd.AvailableForStates(G4State_PreInit);
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
    if(withWaterBricks) addLayer(ShellLayerSpec(G4ThreeVector(9*in,9*in,9*in), G4ThreeVector(9*in,9*in,0), MaterialsHelper::M().getBoratedH2O(0.02), G4Colour(0.5,0.5,1.0,0.5)));
    construct_layers();
}

void PR20InnerShieldBuilder::SetNewValue(G4UIcommand* command, G4String newvalue) {
    if(command == &waterBrickCmd) withWaterBricks = waterBrickCmd.GetNewBoolValue(newvalue);
}

PR20ShieldBuilder::PR20ShieldBuilder(): ShellLayerBuilder("PROSPECT20"),
waterBrickCmd("/geom/PR20Shield/waterbricks",this),
wbLoadingCmd("/geom/PR20Shield/wbloading",this) {
    myContents = &myInnerShield;
    place_centered = false;
    
    waterBrickCmd.SetGuidance("Set number of 9\" waterbrick layers to construct.");
    waterBrickCmd.SetDefaultValue(nWaterBrickLayers);
    waterBrickCmd.AvailableForStates(G4State_PreInit);
    
    wbLoadingCmd.SetGuidance("Set boron mass fraction for waterbrick loading.");
    wbLoadingCmd.SetDefaultValue(wbloading);
    wbLoadingCmd.AvailableForStates(G4State_PreInit);
}

void PR20ShieldBuilder::_construct() {
    addLayer(ShellLayerSpec(G4ThreeVector(4*in, 6*in, 5*in), G4ThreeVector(4*in, 6*in, 0*in), MaterialsHelper::M().BPoly5, G4Colour(1,1,0.8,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(4*in, 4*in, 4*in), G4ThreeVector(4*in, 4*in, 0*in), MaterialsHelper::M().Polyeth, G4Colour(0.7,0.7,1.0,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(in/8,in/8,in/8), G4ThreeVector(in/8,in/8,in/4), MaterialsHelper::M().nat_Al, G4Colour(0.7,0.7,0.7,0.5)));
    addLayer(ShellLayerSpec(G4ThreeVector(0,0,0), G4ThreeVector(0,0,3./8.*in), MaterialsHelper::M().BPoly30, G4Colour(0.5,1.0,0.5,0.5)));
    if(nWaterBrickLayers) {
        double wbt = nWaterBrickLayers*9*in;
        addLayer(ShellLayerSpec(G4ThreeVector(wbt,wbt,wbt), G4ThreeVector(wbt,wbt,0), MaterialsHelper::M().getBoratedH2O(wbloading), G4Colour(0.5,0.5,1.0,0.5)));
    }
    construct_layers();
}

void PR20ShieldBuilder::SetNewValue(G4UIcommand* command, G4String newvalue) {
    if(command == &waterBrickCmd) nWaterBrickLayers = waterBrickCmd.GetNewIntValue(newvalue);
    if(command == &wbLoadingCmd) wbloading = wbLoadingCmd.GetNewDoubleValue(newvalue);
}
