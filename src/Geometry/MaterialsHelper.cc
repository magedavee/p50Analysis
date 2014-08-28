#include "MaterialsHelper.hh"
#include "Utilities.hh"

#include <G4SystemOfUnits.hh>
#include <G4NistManager.hh>
#include <G4ios.hh>

MaterialsHelper* MaterialsHelper::theMatHelper = NULL;

MaterialsHelper& MaterialsHelper::M() {
    if(!theMatHelper) theMatHelper = new MaterialsHelper;
    return *theMatHelper;
}

MaterialsHelper::MaterialsHelper() {
    
    G4cout << "Initializing materials..." << G4endl;
    
    nist = G4NistManager::Instance();
    
    nat_H  = nist->FindOrBuildMaterial("G4_H",  true);
    nat_Li = nist->FindOrBuildMaterial("G4_Li", true);
    nat_B  = nist->FindOrBuildMaterial("G4_B",  true);
    nat_C  = nist->FindOrBuildMaterial("G4_C",  true);
    nat_N  = nist->FindOrBuildMaterial("G4_N",  true);
    nat_O  = nist->FindOrBuildMaterial("G4_O",  true);
    nat_Al = nist->FindOrBuildMaterial("G4_Al", true);
    nat_Si = nist->FindOrBuildMaterial("G4_Si", true);
    nat_P  = nist->FindOrBuildMaterial("G4_P",  true);
    nat_Cl = nist->FindOrBuildMaterial("G4_Cl", true);
    nat_Ca = nist->FindOrBuildMaterial("G4_Ca", true);
    nat_Fe = nist->FindOrBuildMaterial("G4_Fe", true);
    nat_Cr = nist->FindOrBuildMaterial("G4_Cr", true);
    nat_Mo = nist->FindOrBuildMaterial("G4_Mo", true);
    nat_Pb = nist->FindOrBuildMaterial("G4_Pb", true);
    
    G4Element* elLi6  = new G4Element("eleLi6", "Li6", 1);
    G4Isotope* isoLi6 = new G4Isotope("isoLi6", 3, 6, 6.015122*g/mole);
    elLi6->AddIsotope(isoLi6,100.*perCent);
    Li6 = new G4Material("Lithium6", 0.463*g/cm3, 1);
    Li6->AddElement(elLi6,1);
    
    double room_T = 293.15*kelvin; // materials "room temperature"
    
    Vacuum = new G4Material("Vacuum", 2., 4.0026*g/mole, 1.e-25*g/cm3, kStateGas, 2.73*kelvin, 3.e-18*pascal);
    
    Air = new G4Material("Air", 1.204*kg/m3, 1, kStateGas, room_T);
    Air->AddMaterial(nist->FindOrBuildMaterial("G4_AIR", true, true), 100.*perCent);
    
    MinOil = new G4Material("Mineral Oil CH1.1", 0.877*g/cm3, 2, kStateLiquid, room_T);
    MinOil->AddMaterial(nat_C, 91.53*perCent);
    MinOil->AddMaterial(nat_H, 8.47*perCent);
    
    Water = nist->FindOrBuildMaterial("G4_WATER", true, true);
    
    PVT = new G4Material("Polyvinyl Toluene", 1.023*g/cm3, 2, kStateSolid, room_T);
    PVT->AddMaterial(nat_C, 91.478*perCent);
    PVT->AddMaterial(nat_H, 8.522*perCent);
    
    RawPsiCumene = new G4Material("Pseudocumene", 0.88*g/cm3, 2, kStateLiquid, room_T);
    RawPsiCumene->AddMaterial(nat_C, 89.945*perCent);
    RawPsiCumene->AddMaterial(nat_H, 10.055*perCent);
    
    UG_AB = new G4Material("UG_AB", 0.98*g/cm3, 5, kStateLiquid, room_T);
    UG_AB->AddMaterial(nat_C, 76.3*perCent);
    UG_AB->AddMaterial(nat_H, 9.7*perCent);
    UG_AB->AddMaterial(nat_N, 0.05*perCent);
    UG_AB->AddMaterial(nat_O, 13.8*perCent);
    UG_AB->AddMaterial(nat_P, 0.1*perCent);
    
    PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS", true, true);
    PMMA_black = new G4Material("PMMA_black", PMMA->GetDensity(), PMMA);
    PMMA_white = new G4Material("PMMA_white", PMMA->GetDensity(), PMMA);
    
    PEEK = new G4Material("PEEK", 1.32*g/cm3, 3, kStateSolid, room_T);
    PEEK->AddElement(nist->FindOrBuildElement("H",true), 12);
    PEEK->AddElement(nist->FindOrBuildElement("C",true), 19);
    PEEK->AddElement(nist->FindOrBuildElement("O",true), 3);
    
    Polyeth = nist->FindOrBuildMaterial("G4_POLYETHYLENE", true, true);
    
    BPoly = new G4Material("5wt% Borated Polyethylene", 0.92*g/cm3, 2, kStateSolid, room_T);
    BPoly->AddMaterial(Polyeth, 95.0*perCent);
    BPoly->AddMaterial(nat_B, 5.0*perCent);
    
    LiPoly = new G4Material("5wt% Lithiated Polyethylene", 0.92*g/cm3, 2, kStateSolid, room_T);
    LiPoly->AddMaterial(Polyeth, 95.0*perCent);
    LiPoly->AddMaterial(nat_Li, 5.0*perCent);
    
    SS444 = new G4Material("Stainless Steel Type 444", 8.*g/cm3, 3, kStateSolid, room_T);
    SS444->AddMaterial(nat_Fe, 80.*perCent);
    SS444->AddMaterial(nat_Cr, 18.*perCent);
    SS444->AddMaterial(nat_Mo, 2.*perCent);
    
    Quartz = new G4Material("Quartz", 2.62*g/cm3, 2, kStateSolid, room_T);
    Quartz->AddMaterial(nat_Si, 0.4674);
    Quartz->AddMaterial(nat_O, 0.5326);
    
    Concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
    /*
    Concrete = new G4Material("Concrete", 2.3*g/cm3, 6, kStateSolid, room_T);
    Concrete->AddMaterial(nat_Si, 0.227915);
    Concrete->AddMaterial(nat_O, 0.60541);
    Concrete->AddMaterial(nat_H, 0.09972);
    Concrete->AddMaterial(nat_Ca, 0.04986);
    Concrete->AddMaterial(nat_Al, 0.014245);
    Concrete->AddMaterial(nat_Fe, 0.00285);
    */
        
    Dirt = new G4Material("Dirt", 1.52*g/cm3, 5);
    G4int nAtoms = 0;
    Dirt->AddElement(nist->FindOrBuildElement("C"),  nAtoms=  1);
    Dirt->AddElement(nist->FindOrBuildElement("Si"), nAtoms= 29);
    Dirt->AddElement(nist->FindOrBuildElement("Al"), nAtoms= 15);
    Dirt->AddElement(nist->FindOrBuildElement("Fe"), nAtoms=  5);
    Dirt->AddElement(nist->FindOrBuildElement("O"),  nAtoms= 50);
    
    setupOptical();
}

G4Material* MaterialsHelper::get6LiLS(double loading, bool enriched) {
    std::string mnm = "UG_AB-"+to_str(100*loading)+(enriched?"wt%-6Li":"wt%-Li");
    if(!xmats.count(mnm)) {
        G4cout << "Bulding 6Li-loaded (" << loading << "% by weight) Ultima Gold AB scintillator " << nm << " ...\n";
        G4Material* myLi = enriched? Li6 : nat_Li; 
        double avgLiA = enriched? 6. : 0.0864*6 + .9135*7;      /// Li average mass
        double m_Cl = loading*35.45/avgLiA;                     /// mass fraction Cl, by ratio of masses to Li
        double m_H2O = (1000./8. - avgLiA -35.45)/avgLiA*loading;  /// mass fraction H2O from 8 molar LiCl solution
        G4Material* Li_UG_AB = new G4Material(mnm.c_str(), 0.98*g/cm3, 4, kStateLiquid, 293.15*kelvin);
        Li_UG_AB->AddMaterial(UG_AB, 1.-loading-m_Cl-m_H2O);
        Li_UG_AB->AddMaterial(myLi, loading);
        Li_UG_AB->AddMaterial(nat_Cl, m_Cl);
        Li_UG_AB->AddMaterial(Water, m_H2O);
        if(mptUG_AB) Li_UG_AB->SetMaterialPropertiesTable(mptUG_AB);      // TODO
        Li_UG_AB->GetIonisation()->SetBirksConstant(birksUG_AB);
        xmats[mnm] = Li_UG_AB;
    }
    return xmats[mnm];
}

void MaterialsHelper::setupOptical() {
    
    G4cerr << "Setting material optical properties..." << G4endl;
    
    birksPC = 0.1*mm/MeV;
    birksPVT = 0.2*mm/MeV;
    birksUG_AB = 0.1*mm/MeV; // TODO
    
    // NOTE: The emission spectra had to be modified from manufaturer specifications
    //       as the Scintillation process samples the photon energy uniformly in photon energy
    //       (rather than wavelength), which translates to 1/x^2 in wavelength. This
    //       was found to distort the emission spectrum when converted back to wavelengths, so a factor
    //       of x^2 was applied to the listed distribution to counter this effect.
    
    // table of photon energies, for wavelengths 520 nm to 360 nm in 2 nm steps
    const G4int nEntries = 81;
    G4double PhotonEnergy[nEntries]; 
    for(int i=0; i<nEntries; i++) {
        G4double l = (520-2*i)*nm;
        PhotonEnergy[i] = CLHEP::h_Planck * CLHEP::c_light / l;
    }
    
    /////////////////////////////////////////
    // Gd-Loaded Liquid Pseudocumene - BC-525
    
    // Refractive Index
    G4double RIndexPC[nEntries];
    for(int i = 0; i < nEntries; i++) RIndexPC[i] = 1.505;
    
    // Absorption Length
    G4double AbsLengthPC[nEntries] = {
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 9.4*m, 8.7*m, 8.0*m, 7.3*m, 6.7*m, 6.2*m,
        5.7*m, 5.3*m, 4.9*m, 4.5*m, 4.2*m, 3.9*m, 3.7*m, 3.5*m, 3.3*m
    };
    
    // Scintillation - Fast Component - Double Modified Spectrum
    G4double ScintFastPC[nEntries] = {
        0.036, 0.042, 0.047, 0.054, 0.062, 0.070, 0.078, 0.087, 0.096,
        0.105, 0.115, 0.124, 0.133, 0.142, 0.152, 0.162, 0.173, 0.183,
        0.194, 0.205, 0.218, 0.231, 0.245, 0.261, 0.281, 0.305, 0.338,
        0.370, 0.402, 0.439, 0.469, 0.493, 0.515, 0.537, 0.559, 0.580,
        0.602, 0.629, 0.677, 0.787, 0.885, 0.939, 0.972, 0.993, 1.004,
        1.000, 0.984, 0.952, 0.909, 0.862, 0.801, 0.744, 0.669, 0.601,
        0.526, 0.455, 0.401, 0.351, 0.299, 0.254, 0.208, 0.173, 0.146,
        0.122, 0.104, 0.090, 0.079, 0.070, 0.062, 0.056, 0.051, 0.046,
        0.041, 0.037, 0.033, 0.029, 0.026, 0.023, 0.021, 0.019, 0.018
    };
    
    G4MaterialPropertiesTable* mptCumene = new G4MaterialPropertiesTable();
    
    // Specification of properties that are constant with photon energy
    mptCumene->AddConstProperty("FASTTIMECONSTANT", 3.8*ns);            // Fast Component Decay Time
    //  mptCumene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);        // Slow Component Decay Time
    mptCumene->AddConstProperty("YIELDRATIO",1.0);                      // 1 = all fast, 0 = all slow, fractional yield ratio allowed
    mptCumene->AddConstProperty("SCINTILLATIONYIELD",9000./MeV);        // Physical value for scintillation yield for Psuedocumene
    mptCumene->AddConstProperty("RESOLUTIONSCALE",1.0);                 // Standard deviation on scintillation yield number
    
    // Specification of properties that vary with photon energy - linearly interpolated
    mptCumene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPC, nEntries);       // Light Yield for Fast Component
    //  mptCumene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPC, nEntries);   // Light Yield for Slow Component
    mptCumene->AddProperty("RINDEX", PhotonEnergy, RIndexPC, nEntries);                 // Refractive Index of Material
    mptCumene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPC, nEntries);           // Abosrption Length of Material
    
    RawPsiCumene->SetMaterialPropertiesTable(mptCumene);
    RawPsiCumene->GetIonisation()->SetBirksConstant(birksPC);

    mptUG_AB = NULL; // TODO
    
    //////////////////////////////////////////////////////////////////
    // Anthracene-doped Polyvinyltoluene Plastic Scintillator - EJ-500
    
    G4double RIndexPVT[nEntries];
    for(int i = 0; i < nEntries; i++) RIndexPVT[i] = 1.58;
                                
    G4double AbsLengthPVT[nEntries];
    for(int i = 0; i < nEntries; i++) AbsLengthPVT[i] = 5.*m;
                                
    // Scintillation - Fast Component
    G4double ScintFastPVT[nEntries] = {
        0.015, 0.019, 0.025, 0.031, 0.036, 0.043, 0.050, 0.058, 0.066,
        0.074, 0.083, 0.092, 0.102, 0.112, 0.121, 0.132, 0.147, 0.161,
        0.178, 0.201, 0.222, 0.249, 0.278, 0.312, 0.344, 0.377, 0.410,
        0.437, 0.463, 0.483, 0.504, 0.522, 0.544, 0.564, 0.585, 0.608,
        0.630, 0.658, 0.695, 0.743, 0.789, 0.840, 0.880, 0.924, 0.956,
        0.983, 0.999, 1.000, 0.981, 0.932, 0.860, 0.693, 0.515, 0.406,
        0.337, 0.273, 0.193, 0.118, 0.072, 0.040, 0.018, 0.009, 0.005,
        0.004, 0.003, 0.002, 0.001, 0.000, 0.000, 0.000, 0.000, 0.000,
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000
    };
    
    G4MaterialPropertiesTable* mptToluene = new G4MaterialPropertiesTable();// Generates new material properties table for scintillator
    mptToluene->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);               // Fast Component Decay Time
    //  mptToluene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);           // Slow Component Decay Time
    mptToluene->AddConstProperty("YIELDRATIO",1.0);                         // 1 = all fast, 0 = all slow, fractional yield ratio allowed
    mptToluene->AddConstProperty("SCINTILLATIONYIELD",10000./MeV);          // Physical value for scintillation yield for Polyvinyltoluene
    mptToluene->AddConstProperty("RESOLUTIONSCALE",1.0);                    // Standard deviation on scintillation yield number
    
    mptToluene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPVT, nEntries);         // Light Yield for Fast Component
    //  mptToluene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPVT, nEntries);     // Light Yield for Slow Component
    mptToluene->AddProperty("RINDEX", PhotonEnergy, RIndexPVT, nEntries);                   // Refractive Index of Material
    mptToluene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPVT, nEntries);             // Abosrption Length of Material
    
    PVT->SetMaterialPropertiesTable(mptToluene);
    PVT->GetIonisation()->SetBirksConstant(birksPVT);
    
    /////////////////////
    // Plexiglass Windows
    
    G4double RIndex_PMMA[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex_PMMA[i] = 1.492;
                                 
    G4double AbsLength_PMMA[nEntries] = {
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.4*m,
        0.3*m, 0.2*m, 0.16*m, 0.13*m, 0.10*m, 0.075*m, 0.055*m, 0.04*m, 0.032*m,
        0.024*m, 0.018*m, 0.014*m, 0.01*m, 8.*mm, 6.*mm, 4.*mm, 2.*mm, 0.1*mm
        
    };
    
    G4MaterialPropertiesTable* mptPMMA = new G4MaterialPropertiesTable();
    mptPMMA->AddProperty("RINDEX", PhotonEnergy, RIndex_PMMA, nEntries);
    mptPMMA->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength_PMMA, nEntries);
    
    PMMA->SetMaterialPropertiesTable(mptPMMA);
  
    G4double Ephoton2[2] = { 1.5*eV, 6.0*eV };
    G4double AbsLength_Black[2] = { 0.01*mm, 0.01*mm };
    G4MaterialPropertiesTable* mptPMMA_blk = new G4MaterialPropertiesTable();
    mptPMMA_blk->AddProperty("RINDEX", PhotonEnergy, RIndex_PMMA, nEntries);
    mptPMMA_blk->AddProperty("ABSLENGTH", Ephoton2, AbsLength_Black, 2);
    PMMA_black->SetMaterialPropertiesTable(mptPMMA_blk);
    
    //////////////
    // Mineral Oil
    
    G4double RIndex3[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex3[i] = 1.48;
    G4double AbsLength3[nEntries];
    for(int i = 0; i < nEntries; i++) AbsLength3[i] = 4.*m;
    
    G4MaterialPropertiesTable* mptOil = new G4MaterialPropertiesTable();
    mptOil->AddProperty("RINDEX", PhotonEnergy, RIndex3, nEntries);
    mptOil->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength3, nEntries);
    
    MinOil->SetMaterialPropertiesTable(mptOil);
 
    //////
    // Air
    
    G4double RIndex4[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex4[i] = 1.000293;
    G4double AbsLength4[nEntries];
    for(int i = 0; i < nEntries; i++) AbsLength4[i] = 50.*m;
    
    G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
    mptAir->AddProperty("RINDEX", PhotonEnergy, RIndex4, nEntries);
    mptAir->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength4, nEntries);
    
    Air->SetMaterialPropertiesTable(mptAir);
    
    /////////
    // Vacuum
    
    G4double RIndex5[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex5[i] = 1.00;
    G4MaterialPropertiesTable* mptVacuum = new G4MaterialPropertiesTable();
    mptVacuum->AddProperty("RINDEX", PhotonEnergy, RIndex5, nEntries);
    
    Vacuum->SetMaterialPropertiesTable(mptVacuum);
    
    /////////
    // Quartz
    
    G4double RIndex6[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex6[i] = 1.46;
    G4double AbsLengthQu[nEntries];
    for(int i = 0; i < nEntries; i++) AbsLengthQu[i] = 0.001*mm;
    
    G4MaterialPropertiesTable* mptQuartz = new G4MaterialPropertiesTable();
    mptQuartz->AddProperty("RINDEX", PhotonEnergy, RIndex6, nEntries);
    mptQuartz->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthQu, nEntries);
    
    Quartz->SetMaterialPropertiesTable(mptQuartz);
}
