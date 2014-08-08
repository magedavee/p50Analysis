#include "OpticalSurfaceSetup.hh"
#include <G4MaterialPropertiesTable.hh>
#include <G4SystemOfUnits.hh>

OpticalSurfaceSetup::OpticalSurfaceSetup(): XMLProvider("OpticalSurface"),
refl(1.), efficiency(1.), lobe(1.), spike(0.), back(0.) { }

void OpticalSurfaceSetup::construct() {
    const G4int nE = 2;                         // number of photon energies to specify
    
    G4double Ephoton[nE] = { 1.5*eV, 6.0*eV };  // photon Energies
    G4double Reflectivity2[nE]  = { refl, refl };
    G4double Efficiency2[nE]    = { efficiency, efficiency };
    G4double SpecularLobe2[nE]  = { lobe, lobe };
    G4double SpecularSpike2[nE] = { spike, spike };
    G4double Backscatter2[nE]   = { back, back };
    
    G4MaterialPropertiesTable* mst = new G4MaterialPropertiesTable();
    mst->AddProperty("REFLECTIVITY", Ephoton, Reflectivity2, nE);
    mst->AddProperty("EFFICIENCY", Ephoton, Efficiency2, nE);
    mst->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobe2, nE);
    mst->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpike2, nE);
    mst->AddProperty("BACKSCATTERCONSTANT", Ephoton, Backscatter2, nE);
    
    S = new G4OpticalSurface("reflectorOptSurface");
    S->SetMaterialPropertiesTable(mst);
    S->SetType(dielectric_metal);
    S->SetModel(unified);
    S->SetFinish(polished);
}

void OpticalSurfaceSetup::fillNode(TXMLEngine& E) {
    addAttr(E, "refl", refl);
    addAttr(E, "effic", efficiency);
    addAttr(E, "lobe", lobe);
    addAttr(E, "spike", spike);
    addAttr(E, "back", back);
}
