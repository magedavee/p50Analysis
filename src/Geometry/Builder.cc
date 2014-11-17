#include "Builder.hh"
#include "strutils.hh"
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>

double Builder::in = 25.4*mm;
G4RotationMatrix* Builder::rot_X_90 = NULL;
G4RotationMatrix* Builder::rot_Y_90 = NULL;
G4RotationMatrix* Builder::rot_Z_90 = NULL;

Builder::Builder(const string& n): XMLProvider(n), main_log(NULL), dim() {
    if(!rot_X_90) {
        rot_X_90 = new G4RotationMatrix(); rot_X_90->rotateX(90*deg);
        rot_Y_90 = new G4RotationMatrix(); rot_Y_90->rotateY(90*deg);
        rot_Z_90 = new G4RotationMatrix(); rot_Z_90->rotateZ(90*deg);
    }
}

//////

void ShellLayerSpec::fillNode(TXMLEngine& E) {
    if(mat) addAttr(E, "mat", mat->GetName());
    addAttr(E, "upper", G4BestUnit(uthick,"Length"));
    addAttr(E, "lower", G4BestUnit(lthick,"Length"));
}

G4VPhysicalVolume* ShellLayerSpec::wrap(G4LogicalVolume*& child, G4ThreeVector& dim, const G4String& nn) const {
    dim += uthick + lthick;
    G4Box* layer_box = new G4Box(nn+"_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    G4LogicalVolume* s_log = new G4LogicalVolume(layer_box, mat, nn+"_log");
    s_log->SetVisAttributes(vis);

    G4VPhysicalVolume* cplace = new G4PVPlacement(NULL, offset(), child, nn+"_phys", s_log, false, 0, true);
    child = s_log;
    return cplace;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void ShellLayerBuilder::constructLayers(G4LogicalVolume* core_log, G4ThreeVector ldim) {
    main_log = core_log;
    dim = ldim;
    unsigned int nlayers = 0;
    for(auto it = layers.begin(); it != layers.end(); it++) {
        if(!it->mat) continue;
        nlayers++;
        it->wrap(main_log, dim, nodeName+"_layer_"+to_str(nlayers));
        layer_log.push_back(main_log);
        addChild(&(*it));
    }
}
