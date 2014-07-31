#include "MaterialsHelper.hh"
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
    
    G4Element* elLi6  = new G4Element("eleLi6", "Li6", 1);
    G4Isotope* isoLi6 = new G4Isotope("isoLi6", 3, 6, 6.015122*g/mole);
    elLi6->AddIsotope(isoLi6,100.*perCent);
    Li6 = new G4Material("Lithium6", 0.463*g/cm3, 1);
    Li6->AddElement(elLi6,1);
    
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
    
    G4cerr << "Materials initialized." << G4endl;
}

G4Material* MaterialsHelper::get6LiLS(double loading) {
    if(!LiLSs.count(loading)) {
        G4Material* LiPsiCumene = new G4Material("PC-0.70wt%Li", 0.88*g/cm3, 2, kStateLiquid, 293.15*kelvin);
        LiPsiCumene->AddMaterial(RawPsiCumene, 1-loading);
        LiPsiCumene->AddMaterial(Li6, loading);
        LiLSs[loading] = LiPsiCumene;
    }
    return LiLSs[loading];
}

/*
void DetectorConstruction::ConstructMaterials() {
        
    MinOil = new G4Material("Mineral Oil CH1.1", density= 0.877*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    MinOil->AddMaterial(C, massFraction= 91.53*perCent);
    MinOil->AddMaterial(H, massFraction= 8.47*perCent);
    
    MuonScint = new G4Material("Polyvinyl Toluene", density= 1.023*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    MuonScint->AddMaterial(C, massFraction= 91.478*perCent);
    MuonScint->AddMaterial(H, massFraction= 8.522*perCent);
    
    // ************************************************************************************************
    
    Water = new G4Material("Water", density= 1.00*g/cm3, nComponents= 1, kStateLiquid, T=293.15*kelvin);
    Water->AddMaterial(nist->FindOrBuildMaterial("G4_WATER", true, true), massFraction= 100.*perCent);
    
    Polyeth = nist->FindOrBuildMaterial("G4_POLYETHYLENE", true, true);
    
    GdPaintA = new G4Material("GdPaintA", density= 1.21*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintA->AddMaterial(PMMA, massFraction= 99.5*perCent);
    GdPaintA->AddMaterial(Gd, massFraction= 0.5*perCent);
    
    GdPaintB =  new G4Material("GdPaintB", density= 1.24*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintB->AddMaterial(PMMA, massFraction= 99.0*perCent);
    GdPaintB->AddMaterial(Gd, massFraction= 1.0*perCent);
    
    GdPaintC =  new G4Material("GdPaintC", density= 1.30*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    GdPaintC->AddMaterial(PMMA, massFraction= 98.0*perCent);
    GdPaintC->AddMaterial(Gd, massFraction= 2.0*perCent);
    
    
    
    // Example of how to define a vacuum since pure vacuum does not exist in Geant4 library
    Vacuum = new G4Material("Vacuum", z= 2., a= 4.0026*g/mole, density= 1.e-25*g/cm3, kStateGas, T= 2.73*kelvin, P= 3.e-18*pascal);
    
    G4cout << G4endl << *(G4Material::GetMaterialTable()) << G4endl;    // Prints a table of all materials defined above
}

void DetectorConstruction::ConstructMaterialProperties() {
    
    G4cerr << "Setting material optical properties..." << G4endl;
    
    // Photon Energies (520 nm - 360 nm in 2 nm separation)
    const G4int nEntries = 81;
    G4double PhotonEnergy[nEntries]; 
    for(int i=0; i<nEntries; i++) {
        G4double l = (520-2*i)*nm;
        PhotonEnergy[i] = CLHEP::h_Planck * CLHEP::c_light / l;
        }
        
        // Gd-Loaded Liquid Pseudocumene - BC-525
        
        // Refractive Index
        G4double RIndexPC[nEntries];
        for(int i = 0; i < nEntries; i++) RIndexPC[i] = 1.505;
        // Absorption Length
        G4double AbsLengthPC[nEntries] =
        { 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m, 10.*m,
        10.*m, 10.*m, 10.*m, 9.4*m, 8.7*m, 8.0*m, 7.3*m, 6.7*m, 6.2*m,
        5.7*m, 5.3*m, 4.9*m, 4.5*m, 4.2*m, 3.9*m, 3.7*m, 3.5*m, 3.3*m  };
        
        /  G4double AbsLengthPC[nEntries] =
         *      { 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
         *      10.*km, 10.*km, 10.*km, 9.4*km, 8.7*km, 8.0*km, 7.3*km, 6.7*km, 6.2*km,
         *      5.7*km, 5.3*km, 4.9*km, 4.5*km, 4.2*km, 3.9*km, 3.7*km, 3.5*km, 3.3*km  };*
        
        // Scintillation - Fast Component - Double Modified Spectrum
        G4double ScintFastPC[nEntries] =
        { 0.036, 0.042, 0.047, 0.054, 0.062, 0.070, 0.078, 0.087, 0.096,
        0.105, 0.115, 0.124, 0.133, 0.142, 0.152, 0.162, 0.173, 0.183,
        0.194, 0.205, 0.218, 0.231, 0.245, 0.261, 0.281, 0.305, 0.338,
        0.370, 0.402, 0.439, 0.469, 0.493, 0.515, 0.537, 0.559, 0.580,
        0.602, 0.629, 0.677, 0.787, 0.885, 0.939, 0.972, 0.993, 1.004,
        1.000, 0.984, 0.952, 0.909, 0.862, 0.801, 0.744, 0.669, 0.601,
        0.526, 0.455, 0.401, 0.351, 0.299, 0.254, 0.208, 0.173, 0.146,
        0.122, 0.104, 0.090, 0.079, 0.070, 0.062, 0.056, 0.051, 0.046,
        0.041, 0.037, 0.033, 0.029, 0.026, 0.023, 0.021, 0.019, 0.018  };
        // NOTE: The emission spectra had to be modified as it appears that the Scintillation process samples
        //       the photon energy uniformly in photon energy, which translates to 1/x^2 in wavelength. This
        //       was found to distort the emission spectrum when converted back to wavelengths, so a factor
        //       of x^2 was applied to the listed distribution below to counter this effect.
        
        /
         *  // Scintillation - Fast Component - Listed Spectrum
         *  G4double ScintFastPC[nEntries] =
         *  { 0.025, 0.029, 0.033, 0.038, 0.044, 0.050, 0.056, 0.063, 0.070,
         *  0.077, 0.085, 0.093, 0.100, 0.108, 0.116, 0.125, 0.134, 0.143,
         *  0.153, 0.163, 0.175, 0.187, 0.200, 0.215, 0.233, 0.255, 0.285,
         *  0.315, 0.345, 0.380, 0.410, 0.435, 0.458, 0.482, 0.506, 0.530,
         *  0.555, 0.585, 0.635, 0.745, 0.845, 0.905, 0.945, 0.975, 0.995,
         *  1.000, 0.993, 0.970, 0.935, 0.895, 0.840, 0.787, 0.715, 0.648,
         *  0.573, 0.500, 0.445, 0.394, 0.339, 0.291, 0.240, 0.202, 0.172,
         *  0.145, 0.125, 0.110, 0.098, 0.087, 0.078, 0.071, 0.065, 0.059,
         *  0.054, 0.049, 0.044, 0.040, 0.036, 0.032, 0.029, 0.027, 0.025  };
         *
        /
         *  // Scintillation - Fast Component
         *  G4double ScintFastPC[nEntries] =
         *  { 0.025, 0.035, 0.050,
         *  0.065, 0.085, 0.105,
         *  0.125, 0.147, 0.175,
         *  0.210, 0.265, 0.330,
         *  0.410, 0.470, 0.530,
         *  0.600, 0.850, 0.965,
         *  1.000, 0.950, 0.840,
         *  0.670, 0.500, 0.355,
         *  0.240, 0.150, 0.110,
         *  0.085, 0.065, 0.050,
         *  0.040, 0.030, 0.025  };
         *
        /
         *  // Scintillation - Slow Component
         *  G4double ScintSlowPC[nEntries] = 
         *  { 1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000,
         *  1.000, 1.000, 1.000  };
         *
        
        // Generates new material properties table for scintillator
        G4MaterialPropertiesTable* mptCumene = new G4MaterialPropertiesTable();
        
        // Allows specification of properties that are constant with photon energy
        mptCumene->AddConstProperty("FASTTIMECONSTANT", 3.8*ns);            // Fast Component Decay Time
        //  mptCumene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);                // Slow Component Decay Time
        mptCumene->AddConstProperty("YIELDRATIO",1.0);                              // 1 = all fast, 0 = all slow, fractional yield ratio allowed
        mptCumene->AddConstProperty("SCINTILLATIONYIELD",9000./MeV);                // Physical value for scintillation yield for Psuedocumene
        mptCumene->AddConstProperty("RESOLUTIONSCALE",1.0);                 // Standard deviation on scintillation yield number
        
        //  mptCumene->AddConstProperty("SCINTILLATIONYIELD",1./MeV);               // Used for testing phase ONLY, not physically accurate
        
        // Allows specification of properties that vary with photon energy - linearly interpolated
        mptCumene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPC, nEntries);               // Light Yield for Fast Component
        //  mptCumene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPC, nEntries);           // Light Yield for Slow Component
        mptCumene->AddProperty("RINDEX", PhotonEnergy, RIndexPC, nEntries);                 // Refractive Index of Material
        mptCumene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPC, nEntries);           // Abosrption Length of Material
        
        // Applies the optical property table to the material
        RawPsiCumene->SetMaterialPropertiesTable(mptCumene);
        RawPsiCumene->GetIonisation()->SetBirksConstant(birksPC);           // Set Birk's Constant for saturation quenching
        PsiCumeneL->SetMaterialPropertiesTable(mptCumene);
        PsiCumeneL->GetIonisation()->SetBirksConstant(birksPC);
        PsiCumeneT->SetMaterialPropertiesTable(mptCumene);
        PsiCumeneT->GetIonisation()->SetBirksConstant(birksPC);
        PsiCumeneH->SetMaterialPropertiesTable(mptCumene);
        PsiCumeneH->GetIonisation()->SetBirksConstant(birksPC);
        PsiCumeneM->SetMaterialPropertiesTable(mptCumene);
        PsiCumeneM->GetIonisation()->SetBirksConstant(birksPC);
        PsiCumeneG->SetMaterialPropertiesTable(mptCumene);
        PsiCumeneG->GetIonisation()->SetBirksConstant(birksPC);
        
        // Anthracene-doped Polyvinyltoluene Plastic Scintillator - EJ-500
        
        G4double RIndexPVT[nEntries];
        for(int i = 0; i < nEntries; i++) RIndexPVT[i] = 1.58;
        
        G4double AbsLengthPVT[nEntries];
        for(int i = 0; i < nEntries; i++) AbsLengthPVT[i] = 5.*m;
        
        // Scintillation - Fast Component - Listed Spectrum
        G4double ScintFastPVT[nEntries] =
        { 0.015, 0.019, 0.025, 0.031, 0.036, 0.043, 0.050, 0.058, 0.066,
        0.074, 0.083, 0.092, 0.102, 0.112, 0.121, 0.132, 0.147, 0.161,
        0.178, 0.201, 0.222, 0.249, 0.278, 0.312, 0.344, 0.377, 0.410,
        0.437, 0.463, 0.483, 0.504, 0.522, 0.544, 0.564, 0.585, 0.608,
        0.630, 0.658, 0.695, 0.743, 0.789, 0.840, 0.880, 0.924, 0.956,
        0.983, 0.999, 1.000, 0.981, 0.932, 0.860, 0.693, 0.515, 0.406,
        0.337, 0.273, 0.193, 0.118, 0.072, 0.040, 0.018, 0.009, 0.005,
        0.004, 0.003, 0.002, 0.001, 0.000, 0.000, 0.000, 0.000, 0.000,
        0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000  };
        // NOTE: The emission spectra had to be modified as it appears that the Scintillation process samples
        //       the photon energy uniformly in photon energy, which translates to 1/x^2 in wavelength. This
        //       was found to distort the emission spectrum when converted back to wavelengths, so a factor
        //       of x^2 was applied to the listed distribution below to counter this effect.
        
        /
         *  // Scintillation - Fast Component - Listed Spectrum
         *  G4double ScintFastPVT[nEntries] =
         *  { 0.010, 0.013, 0.017, 0.021, 0.025, 0.030, 0.035, 0.041, 0.047,
         *  0.053, 0.060, 0.067, 0.075, 0.083, 0.091, 0.100, 0.112, 0.124,
         *  0.138, 0.157, 0.175, 0.198, 0.223, 0.252, 0.280, 0.310, 0.340,
         *  0.365, 0.390, 0.411, 0.430, 0.452, 0.475, 0.497, 0.520, 0.545,
         *  0.570, 0.600, 0.640, 0.690, 0.740, 0.795, 0.840, 0.890, 0.930,
         *  0.965, 0.990, 1.000, 0.990, 0.950, 0.885, 0.720, 0.540, 0.430,
         *  0.360, 0.295, 0.210, 0.130, 0.080, 0.045, 0.020, 0.010, 0.006,
         *  0.004, 0.003, 0.002, 0.001, 0.000, 0.000, 0.000, 0.000, 0.000,
         *  0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000  };
         *
        /
         *    G4double ScintFastPVT[nEntries] =
         *    { 0.010, 0.020, 0.030,
         *    0.045, 0.060, 0.078,
         *    0.100, 0.130, 0.175,
         *    0.235, 0.310, 0.380,
         *    0.430, 0.485, 0.545,
         *    0.620, 0.745, 0.865,
         *    0.965, 1.000, 0.885,
         *    0.480, 0.295, 0.105,
         *    0.020, 0.005, 0.002,
         *    0.000, 0.000, 0.000,
         *    0.000, 0.000, 0.000  };
         *
        /
         *  // EJ-500
         *  G4double ScintFastPVT[nEntries] =
         *  { 0.010, 0.020, 0.030,
         *  0.045, 0.060, 0.080,
         *  0.100, 0.125, 0.175,
         *  0.235, 0.310, 0.380,
         *  0.430, 0.485, 0.550,
         *  0.610, 0.745, 0.865,
         *  0.950, 1.000, 0.875,
         *  0.635, 0.420, 0.300,
         *  0.210, 0.150, 0.110,
         *  0.085, 0.065, 0.050,
         *  0.035, 0.015, 0.005  };
         *
        /
         *    G4double ScintSlowPVT[nEntries] = 
         *    { 1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000,
         *    1.000, 1.000, 1.000  };
         *
        G4MaterialPropertiesTable* mptToluene = new G4MaterialPropertiesTable();    // Generates new material properties table for scintillator
        mptToluene->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);           // Fast Component Decay Time
        //  mptToluene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);               // Slow Component Decay Time
        mptToluene->AddConstProperty("YIELDRATIO",1.0);                     // 1 = all fast, 0 = all slow, fractional yield ratio allowed
        mptToluene->AddConstProperty("SCINTILLATIONYIELD",10000./MeV);              // Physical value for scintillation yield for Polyvinyltoluene
        mptToluene->AddConstProperty("RESOLUTIONSCALE",1.0);                        // Standard deviation on scintillation yield number
        
        //  mptToluene->AddConstProperty("SCINTILLATIONYIELD",1./MeV);              // Used for testing phase ONLY, not physically accurate
        
        mptToluene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPVT, nEntries);             // Light Yield for Fast Component
        //  mptToluene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPVT, nEntries);         // Light Yield for Slow Component
        mptToluene->AddProperty("RINDEX", PhotonEnergy, RIndexPVT, nEntries);                       // Refractive Index of Material
        mptToluene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPVT, nEntries);         // Abosrption Length of Material
        
        MuonScint->SetMaterialPropertiesTable(mptToluene);
        MuonScint->GetIonisation()->SetBirksConstant(birksPVT);             // Sets the Birk's Constant for saturation quenching
        
        // Plexiglass Windows
        
        G4double RIndex2[nEntries];
        for(int i = 0; i < nEntries; i++) RIndex2[i] = 1.492;
        
        G4double AbsLength2[nEntries] =
        { 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m,
        0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.5*m, 0.4*m,
        0.3*m, 0.2*m, 0.16*m, 0.13*m, 0.10*m, 0.075*m, 0.055*m, 0.04*m, 0.032*m,
        0.024*m, 0.018*m, 0.014*m, 0.01*m, 8.*mm, 6.*mm, 4.*mm, 2.*mm, 0.1*mm    };
        
        G4MaterialPropertiesTable* mptPMMA = new G4MaterialPropertiesTable();
        mptPMMA->AddProperty("RINDEX", PhotonEnergy, RIndex2, nEntries);
        mptPMMA->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength2, nEntries);
        
        PMMA->SetMaterialPropertiesTable(mptPMMA);
        
        // Mineral Oil
        G4double RIndex3[nEntries];
        for(int i = 0; i < nEntries; i++) RIndex3[i] = 1.48;
        G4double AbsLength3[nEntries];
        for(int i = 0; i < nEntries; i++) AbsLength3[i] = 4.*m;
        
        G4MaterialPropertiesTable* mptOil = new G4MaterialPropertiesTable();
        mptOil->AddProperty("RINDEX", PhotonEnergy, RIndex3, nEntries);
        mptOil->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength3, nEntries);
        
        MinOil->SetMaterialPropertiesTable(mptOil);
        
        // Air
        G4double RIndex4[nEntries];
        for(int i = 0; i < nEntries; i++) RIndex4[i] = 1.000293;
        G4double AbsLength4[nEntries];
        for(int i = 0; i < nEntries; i++) AbsLength4[i] = 50.*m;
        
        G4MaterialPropertiesTable* mptAir = new G4MaterialPropertiesTable();
        mptAir->AddProperty("RINDEX", PhotonEnergy, RIndex4, nEntries);
        mptAir->AddProperty("ABSLENGTH", PhotonEnergy, AbsLength4, nEntries);
        
        Air->SetMaterialPropertiesTable(mptAir);
        
        
        // Vacuum - Used in PMTs
        G4double RIndex5[nEntries];
        for(int i = 0; i < nEntries; i++) RIndex5[i] = 1.00;
        G4MaterialPropertiesTable* mptVacuum = new G4MaterialPropertiesTable();
        mptVacuum->AddProperty("RINDEX", PhotonEnergy, RIndex5, nEntries);
        
        Vacuum->SetMaterialPropertiesTable(mptVacuum);
        
        // Quartz - Used in PMTs
        G4double RIndex6[nEntries];
        for(int i = 0; i < nEntries; i++) RIndex6[i] = 1.46;
        G4double AbsLengthQu[nEntries];
        for(int i = 0; i < nEntries; i++) AbsLengthQu[i] = 0.001*mm;
        
        G4MaterialPropertiesTable* mptQuartz = new G4MaterialPropertiesTable();
        mptQuartz->AddProperty("RINDEX", PhotonEnergy, RIndex6, nEntries);
        mptQuartz->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthQu, nEntries);
        
        Quartz->SetMaterialPropertiesTable(mptQuartz);
        }
*/
