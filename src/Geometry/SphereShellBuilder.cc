#include "SphereShellBuilder.hh"

#include "ScintCellBuilder.hh"
#include "MaterialsHelper.hh"
#include "Utilities.hh"

#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Orb.hh>
#include <G4Sphere.hh>

SphereShellBuilder::SphereShellBuilder(): ScintSegVol("SphereShells"),
myMat(NULL), radius(100*cm), ndivs(100) { }

void SphereShellBuilder::construct() {
    
    if(!myMat) myMat = MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, 0.001, true);
    
    dim = G4ThreeVector(2*radius, 2*radius, 2*radius);
    
    G4Orb* full_sphere = new G4Orb("full_sphere", radius);
    scint_log = main_log = new G4LogicalVolume(full_sphere, myMat, "SphereShells_main_log");
    
    for(unsigned int i=0; i<ndivs; i++) {
        G4Sphere* shell_sphere = new G4Sphere("shell_sphere_"+to_str(i), i*radius/ndivs, (i+1)*radius/ndivs, 0, 2*M_PI, 0, M_PI);
        shells.push_back(new G4LogicalVolume(shell_sphere, myMat, "SphereShells_shell_log_"+to_str(i)));
        new G4PVPlacement(NULL, G4ThreeVector(), shells.back(), "SphereShells_shell_phys_"+to_str(i), main_log, false, 0, true);
    }
}

void SphereShellBuilder::setScintSD(G4VSensitiveDetector* SD) {
    for(auto it = shells.begin(); it != shells.end(); it++)
        (*it)->SetSensitiveDetector(SD);
}

void SphereShellBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "r", G4BestUnit(radius,"Length"));
    addAttr(E, "n", ndivs);
    addAttr(E, "mat", scint_log->GetMaterial()->GetName());
}

int SphereShellBuilder::getSegmentNum(const G4ThreeVector& x) const {
    double r = x.mag();
    return int(r/(radius/ndivs));
}
