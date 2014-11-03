#include "ShieldBuilder.hh"
#include "MaterialsHelper.hh"
#include "MuVetoSD.hh"
#include "RootIO.hh"

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
    addLayer(ShellLayerSpec(5*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.)));
    addLayer(ShellLayerSpec(10*cm, MaterialsHelper::M().LiPoly, G4Colour(1.,0.,0.)));
    addLayer(ShellLayerSpec(3*cm, MaterialsHelper::M().nat_Pb, G4Colour(0.,1.,0.)));
    addLayer(ShellLayerSpec(47*cm, MaterialsHelper::M().BPoly, G4Colour(0.,1.,0.)));
    addChild(&myDet);
    
    clearCmd.SetGuidance("Remove shield (replace with air)");
    clearCmd.AvailableForStates(G4State_PreInit);
    
    vetoCmd.SetGuidance("Construct muon veto 'shield' layer");
    vetoCmd.AvailableForStates(G4State_PreInit);
    
    nshellCmd.SetGuidance("Construct neutron steel shield layer");
    nshellCmd.AvailableForStates(G4State_PreInit);
}

void ShieldBuilder::construct() {
    myDet.construct();
    constructLayers(myDet);
    
    // muon veto layer
    for(vector<ShellLayerSpec>::iterator it = layers.begin(); it != layers.end(); it++) {
        if(it->mat == MaterialsHelper::M().PVT) {
            RootIO::GetInstance()->addVetoIoniBranch();
            MuVetoSD* V = new MuVetoSD("MuVetoSD",main_log);
            G4SDManager::GetSDMpointer()->AddNewDetector(V);
            main_log->SetSensitiveDetector(V);
        }
    }
}

void ShieldBuilder::SetNewValue(G4UIcommand* command, G4String newvalue) {
    if(command == &clearCmd) {
        ShellLayerSpec S(0*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.));
        for(vector<ShellLayerSpec>::iterator it = layers.begin(); it != layers.end(); it++) {
            S.side_thick += it->side_thick;
            S.top_thick += it->top_thick;
            S.bottom_thick += it->bottom_thick;            
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

void ShieldBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
