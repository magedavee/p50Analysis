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
    
    G4cerr << "Initializing materials..." << G4endl;
    
    nist = G4NistManager::Instance();
    
    nat_H = nist->FindOrBuildMaterial("G4_H", true);
    nat_Li = nist->FindOrBuildMaterial("G4_Li", true);
    nat_C = nist->FindOrBuildMaterial("G4_C", true);
    nat_O = nist->FindOrBuildMaterial("G4_O", true);
    nat_Ca = nist->FindOrBuildMaterial("G4_Ca", true);
    nat_Si = nist->FindOrBuildMaterial("G4_Si", true);
    nat_B = nist->FindOrBuildMaterial("G4_B", true);
    nat_Al = nist->FindOrBuildMaterial("G4_Al", true);
    nat_Fe = nist->FindOrBuildMaterial("G4_Fe", true);
    nat_Cr = nist->FindOrBuildMaterial("G4_Cr", true);
    nat_Mo = nist->FindOrBuildMaterial("G4_Mo", true);
    nat_Pb = nist->FindOrBuildMaterial("G4_Pb", true);
    
    double room_T = 293.15*kelvin; // materials "room temperature"
    
    Vacuum = new G4Material("Vacuum", 2., 4.0026*g/mole, 1.e-25*g/cm3, kStateGas, 2.73*kelvin, 3.e-18*pascal);
    
    Air = new G4Material("Air", 1.204*kg/m3, 1, kStateGas, room_T);
    Air->AddMaterial(nist->FindOrBuildMaterial("G4_AIR", true, true), 100.*perCent);
    
    MinOil = new G4Material("Mineral Oil CH1.1", 0.877*g/cm3, 2, kStateLiquid, room_T);
    MinOil->AddMaterial(nat_C, 91.53*perCent);
    MinOil->AddMaterial(nat_H, 8.47*perCent);
    
    G4Element* elLi6  = new G4Element("eleLi6", "Li6", 1);
    G4Isotope* isoLi6 = new G4Isotope("isoLi6", 3, 6, 6.015122*g/mole);
    elLi6->AddIsotope(isoLi6,100.*perCent);
    Li6 = new G4Material("Lithium6", 0.463*g/cm3, 1);
    Li6->AddElement(elLi6,1);
    
    PVT = new G4Material("Polyvinyl Toluene", 1.023*g/cm3, 2, kStateSolid, room_T);
    PVT->AddMaterial(nat_C, 91.478*perCent);
    PVT->AddMaterial(nat_H, 8.522*perCent);
    
    RawPsiCumene = new G4Material("Pseudocumene", 0.88*g/cm3, 2, kStateLiquid, room_T);
    RawPsiCumene->AddMaterial(nat_C, 89.945*perCent);
    RawPsiCumene->AddMaterial(nat_H, 10.055*perCent);
    
    PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS", true, true);
    
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
    
    Concrete = new G4Material("Concrete", 2.3*g/cm3, 6, kStateSolid, room_T);
    Concrete->AddMaterial(nat_Si, 0.227915);
    Concrete->AddMaterial(nat_O, 0.60541);
    Concrete->AddMaterial(nat_H, 0.09972);
    Concrete->AddMaterial(nat_Ca, 0.04986);
    Concrete->AddMaterial(nat_Al, 0.014245);
    Concrete->AddMaterial(nat_Fe, 0.00285);
    
    setupOptical();
    G4cerr << "Materials initialized." << G4endl;
}

G4Material* MaterialsHelper::get6LiLS(double loading) {
    if(!LiLSs.count(loading)) {
        G4Material* LiPsiCumene = new G4Material(("PC-"+to_str(100*loading)+"wt%Li").c_str(), 0.88*g/cm3, 2, kStateLiquid, 293.15*kelvin);
        LiPsiCumene->AddMaterial(RawPsiCumene, 1-loading);
        LiPsiCumene->AddMaterial(Li6, loading);
        LiPsiCumene->SetMaterialPropertiesTable(mptCumene);
        LiPsiCumene->GetIonisation()->SetBirksConstant(birksPC);
        LiLSs[loading] = LiPsiCumene;
    }
    return LiLSs[loading];
}

/*
void DetectorConstruction::ConstructMaterials() {
    
    
    Water = new G4Material("Water", density= 1.00*g/cm3, nComponents= 1, kStateLiquid, T=293.15*kelvin);
    Water->AddMaterial(nist->FindOrBuildMaterial("G4_WATER", true, true), massFraction= 100.*perCent);
    
    GdPaintA = new G4Material("GdPaintA", density= 1.21*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintA->AddMaterial(PMMA, massFraction= 99.5*perCent);
    GdPaintA->AddMaterial(Gd, massFraction= 0.5*perCent);
    
    GdPaintB =  new G4Material("GdPaintB", density= 1.24*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintB->AddMaterial(PMMA, massFraction= 99.0*perCent);
    GdPaintB->AddMaterial(Gd, massFraction= 1.0*perCent);
    
    GdPaintC =  new G4Material("GdPaintC", density= 1.30*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintC->AddMaterial(PMMA, massFraction= 98.0*perCent);
    GdPaintC->AddMaterial(Gd, massFraction= 2.0*perCent);
}
*/

void MaterialsHelper::setupOptical() {
    
    G4cerr << "Setting material optical properties..." << G4endl;
    
    birksPC = 0.1*mm/MeV;
    birksPVT = 0.2*mm/MeV;
    
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
    
    mptCumene = new G4MaterialPropertiesTable();
    
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
    
    G4double RIndex2[nEntries];
    for(int i = 0; i < nEntries; i++) RIndex2[i] = 1.492;
                                 
    G4double AbsLength2[nEntries] = {
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
    mptPMMA->AddProperty("RINDEX", PhotonEnergy, RIndex2, nEntries);
    mptPMMA->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength2, nEntries);
    
    PMMA->SetMaterialPropertiesTable(mptPMMA);
  
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
