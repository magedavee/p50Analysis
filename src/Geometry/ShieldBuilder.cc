#include "ShieldBuilder.hh"
#include "MaterialsHelper.hh"
#include "MuVetoSD.hh"
#include "FileIO.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>

ShieldBuilder::ShieldBuilder(): ShellLayerBuilder("Shield"),
shield_dir("/geom/shield/"),
clearCmd("/geom/shield/clear",this),
vetoCmd("/geom/shield/muveto",this),
nshellCmd("/geom/shield/nshell",this) {
    myContents = &myDet;
    place_centered = false;
    
    addLayer(ShellLayerSpec(5*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.)));
    addLayer(ShellLayerSpec(10*cm, MaterialsHelper::M().LiPoly, G4Colour(1.,0.,0.)));
    addLayer(ShellLayerSpec(3*cm, MaterialsHelper::M().nat_Pb, G4Colour(0.,1.,0.)));
    addLayer(ShellLayerSpec(47*cm, MaterialsHelper::M().BPoly5, G4Colour(0.,1.,0.)));
    
    clearCmd.SetGuidance("Remove shield (replace with air)");
    clearCmd.AvailableForStates(G4State_PreInit);
    
    vetoCmd.SetGuidance("Construct muon veto 'shield' layer");
    vetoCmd.AvailableForStates(G4State_PreInit);
    
    nshellCmd.SetGuidance("Construct neutron steel shield layer");
    nshellCmd.AvailableForStates(G4State_PreInit);
}

void ShieldBuilder::_construct() {
    construct_layers();
    
    // muon veto layer
    for(auto it = layers.begin(); it != layers.end(); it++) {
        if(it->mat == MaterialsHelper::M().PVT) {
            FileIO::GetInstance()->addVetoIoniBranch();
            MuVetoSD* V = new MuVetoSD("MuVetoSD",main_log);
            G4SDManager::GetSDMpointer()->AddNewDetector(V);
            main_log->SetSensitiveDetector(V);
        }
    }
}

void ShieldBuilder::SetNewValue(G4UIcommand* command, G4String newvalue) {
    if(command == &clearCmd) {
        ShellLayerSpec S(0*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.));
        for(auto it = layers.begin(); it != layers.end(); it++) {
            S.uthick += it->uthick;
            S.lthick += it->lthick;
        }
        layers.clear();
        addLayer(S);
    } else if(command == &vetoCmd) {
        ShellLayerSpec S(vetoCmd.GetNewDoubleValue(newvalue), MaterialsHelper::M().PVT, G4Colour(1.,0.,0.));
        addLayer(S);
    } else if(command == &nshellCmd) {
        ShellLayerSpec S(vetoCmd.GetNewDoubleValue(newvalue), MaterialsHelper::M().SS444, G4Colour(1.,1.,0.));
        addLayer(S);
    } else G4cout << "Unknown command!" << G4endl;
}
