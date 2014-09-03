#include "ShieldBuilder.hh"
#include "MaterialsHelper.hh"
#include "MuVetoSD.hh"
#include "RootIO.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4SDManager.hh>
        
void ShieldLayerSpec::fillNode(TXMLEngine& E) {
    if(mat) addAttr(E, "mat", mat->GetName());
    addAttr(E, "top", G4BestUnit(top_thick,"Length"));
    addAttr(E, "side", G4BestUnit(side_thick,"Length"));
    addAttr(E, "bottom", G4BestUnit(bottom_thick,"Length"));
}

ShieldBuilder::ShieldBuilder(): Builder("Shield"),
shield_dir("/geom/shield/"),
clearCmd("/geom/shield/clear",this),
vetoCmd("/geom/shield/muveto",this) {
    addLayer(ShieldLayerSpec(5*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.)));
    addLayer(ShieldLayerSpec(10*cm, MaterialsHelper::M().LiPoly, G4Colour(1.,0.,0.)));
    addLayer(ShieldLayerSpec(3*cm, MaterialsHelper::M().nat_Pb, G4Colour(0.,1.,0.)));
    addLayer(ShieldLayerSpec(47*cm, MaterialsHelper::M().BPoly, G4Colour(0.,1.,0.)));
    addChild(&myDet);
    
    clearCmd.SetGuidance("Remove shield (replace with air)");
    clearCmd.AvailableForStates(G4State_PreInit);
    
    vetoCmd.SetGuidance("Construct muon veto 'shield' layer");
    vetoCmd.AvailableForStates(G4State_PreInit);
}

void ShieldBuilder::construct() {
    
    myDet.construct();
    main_log = myDet.main_log;
    dim = myDet.getDimensions();
    
    // add each layer
    unsigned int nlayers = 0;
    for(std::vector<ShieldLayerSpec>::iterator it = layers.begin(); it != layers.end(); it++) {
        if(!it->mat) continue;
        nlayers++;
        
        dim += G4ThreeVector(2*it->side_thick, 2*it->side_thick, it->top_thick + it->bottom_thick);
        
        G4Box* shield_box = new G4Box("shield_box", dim[0]/2., dim[1]/2., dim[2]/2.);
        G4LogicalVolume* s_log = new G4LogicalVolume(shield_box, it->mat, "Shield_layer_log_"+to_str(nlayers));
        s_log->SetVisAttributes(&it->vis);
        
        new G4PVPlacement(NULL, G4ThreeVector(0,0,(it->bottom_thick-it->top_thick)/2.),
                          main_log, "Shield_layer_phys_"+to_str(nlayers), s_log, false, 0, true);
        
        main_log = s_log;
        
        addChild(&(*it));
        
        // muon veto layer
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
        ShieldLayerSpec S(0*cm, MaterialsHelper::M().Air, G4Colour(0.,0.,1.));
        for(std::vector<ShieldLayerSpec>::iterator it = layers.begin(); it != layers.end(); it++) {
            S.side_thick += it->side_thick;
            S.top_thick += it->top_thick;
            S.bottom_thick += it->bottom_thick;            
        }
        layers.clear();
        addLayer(S);
    } else if(command == &vetoCmd) {
        ShieldLayerSpec S(vetoCmd.GetNewDoubleValue(newvalue), MaterialsHelper::M().PVT, G4Colour(1.,0.,0.));
        addLayer(S);
    } else G4cout << "Unknown command!" << G4endl;
}

void ShieldBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
}
