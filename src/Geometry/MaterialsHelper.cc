#include "MaterialsHelper.hh"
#include "strutils.hh"

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
    room_T = 293.15*kelvin;
    G4int nAtoms = 0; // placeholder variable for assuring G4int type
    
    nist = G4NistManager::Instance();
    
    nat_H  = nist->FindOrBuildMaterial("G4_H",  true);
    //nat_Li = nist->FindOrBuildMaterial("G4_Li", true);
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
    nat_Cu = nist->FindOrBuildMaterial("G4_Cu", true);
    nat_Zn = nist->FindOrBuildMaterial("G4_Zn", true);
    nat_Cr = nist->FindOrBuildMaterial("G4_Cr", true);
    nat_Mo = nist->FindOrBuildMaterial("G4_Mo", true);
    nat_Pb = nist->FindOrBuildMaterial("G4_Pb", true);
    nat_W = nist->FindOrBuildMaterial("G4_W", true);
    
    el6Li  = new G4Element("eleLi6", "Li6", 1);
    G4Isotope* isoLi6 = new G4Isotope("isoLi6", 3, 6, 6.015122*g/mole);
    el6Li->AddIsotope(isoLi6,100.*perCent);
    
    G4Element* elLi7  = new G4Element("eleLi7", "Li7", 1);
    G4Isotope* isoLi7 = new G4Isotope("isoLi7", 3, 7, 7.01600455*g/mole);
    elLi7->AddIsotope(isoLi7,100.*perCent);
    
    Li6 = new G4Material("Lithium6", 1.0*g/cm3, 1, kStateSolid, room_T);
    Li6->AddElement(el6Li,100.*perCent);
    
    nat_Li = new G4Material("nat_Li", 0.463*g/cm3, 2);
    nat_Li->AddElement(el6Li,0.0811);
    nat_Li->AddElement(elLi7,1-0.0811);
    
    el1H  = new G4Element("eleH1", "H1", 1);
    G4Isotope* isoH1 = new G4Isotope("isoH1", 1, 1, 1.00782504*g/mole);
    el1H->AddIsotope(isoH1,100.*perCent);
    
    el2H  = new G4Element("eleH2", "H2", 1);
    G4Isotope* isoH2 = new G4Isotope("isoH2", 1, 2, 2.01410178*g/mole);
    el2H->AddIsotope(isoH2,100.*perCent);
    
    H1 = new G4Material("Protonium", 1.0*g/cm3, 1, kStateSolid, room_T);
    H1->AddElement(el1H,100.*perCent);
    
    H2 = new G4Material("Deuterium", 1.0*g/cm3, 1, kStateSolid, room_T);
    H2->AddElement(el2H,100.*perCent);
    
    el3He  = new G4Element("eleHe3", "He3", 1);
    G4Isotope* iso3He = new G4Isotope("isoHe3", 2, 3, 3.0160293*g/mole);
    el3He->AddIsotope(iso3He,100.*perCent);
    
    Vacuum = new G4Material("Vacuum", 2., 4.0026*g/mole, 1.e-25*g/cm3, kStateGas, 2.73*kelvin, 3.e-18*pascal);
    
    Air = nist->FindOrBuildMaterial("G4_AIR", true, true);
    
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
    
    EJ309 = new G4Material("EJ309", 0.959*g/cm3, 2, kStateLiquid, room_T);
    EJ309->AddMaterial(nat_C, 90.578*perCent);
    EJ309->AddMaterial(nat_H, 9.422*perCent);
    
    Mylar = nist->FindOrBuildMaterial("G4_MYLAR", true, true);
    
    PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS", true, true);
    PMMA_black = new G4Material("PMMA_black", PMMA->GetDensity(), PMMA);
    PMMA_white = new G4Material("PMMA_white", PMMA->GetDensity(), PMMA);
    
    PEEK = new G4Material("PEEK", 1.32*g/cm3, 3, kStateSolid, room_T);
    PEEK->AddElement(getEl("H"), nAtoms = 12);
    PEEK->AddElement(getEl("C"), nAtoms = 19);
    PEEK->AddElement(getEl("O"), nAtoms = 3);
    
    Polyeth = nist->FindOrBuildMaterial("G4_POLYETHYLENE", true, true);
    Polystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE", true, true);
    PTFE = nist->FindOrBuildMaterial("G4_TEFLON", true, true);
    
    BPoly5 = new G4Material("5wt% Borated Polyethylene", 0.94*g/cm3, 2, kStateSolid, room_T);
    BPoly5->AddMaterial(Polyeth, 95.0*perCent);
    BPoly5->AddMaterial(nat_B, 5.0*perCent);
    
    BPoly30 = new G4Material("30wt% Borated Polyethylene", 0.94*g/cm3, 2, kStateSolid, room_T);
    BPoly30->AddMaterial(Polyeth, 70.0*perCent);
    BPoly30->AddMaterial(nat_B, 30.0*perCent);
    
    LiPoly = new G4Material("5wt% Lithiated Polyethylene", 0.94*g/cm3, 2, kStateSolid, room_T);
    LiPoly->AddMaterial(Polyeth, 95.0*perCent);
    LiPoly->AddMaterial(nat_Li, 5.0*perCent);
    
    SS444 = new G4Material("SS444", 8.*g/cm3, 3, kStateSolid, room_T);
    SS444->AddMaterial(nat_Fe, 80.*perCent);
    SS444->AddMaterial(nat_Cr, 18.*perCent);
    SS444->AddMaterial(nat_Mo, 2.*perCent);
    
    Quartz = new G4Material("Quartz", 2.62*g/cm3, 2, kStateSolid, room_T);
    Quartz->AddMaterial(nat_Si, 0.4674);
    Quartz->AddMaterial(nat_O, 0.5326);
    
    Pyrex = nist->FindOrBuildMaterial("G4_Pyrex_Glass", true, true);
    
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
    Dirt->AddElement(getEl("C"),  nAtoms=  1);
    Dirt->AddElement(getEl("Si"), nAtoms= 29);
    Dirt->AddElement(getEl("Al"), nAtoms= 15);
    Dirt->AddElement(getEl("Fe"), nAtoms=  5);
    Dirt->AddElement(getEl("O"),  nAtoms= 50);
        
    BoricAcid = new G4Material("BoricAcid", 1.435*g/cm3, 3);
    BoricAcid->AddElement(getEl("B"), nAtoms=  1);
    BoricAcid->AddElement(getEl("O"), nAtoms= 3);
    BoricAcid->AddElement(getEl("H"), nAtoms= 3);
    
    Lead_II_Acetate = new G4Material("Lead_II_Acetate", 3.25*g/cm3, 4);
    Lead_II_Acetate->AddElement(getEl("Pb"), nAtoms=  1);
    Lead_II_Acetate->AddElement(getEl("C"), nAtoms= 4);
    Lead_II_Acetate->AddElement(getEl("H"), nAtoms= 6);
    Lead_II_Acetate->AddElement(getEl("O"), nAtoms= 4);
    
    LeadWater = new G4Material("LeadWater", Water->GetDensity()+0.4*g/cm3, 2, Water->GetState(), Water->GetTemperature());
    const double PbFrac = 0.4/1.4;
    LeadWater->AddMaterial(Water, 1.-PbFrac);
    LeadWater->AddMaterial(Lead_II_Acetate, PbFrac);
    
    setupOptical();
    
    // pre-build some materials for availability in named materials catalog
    get6LiLS(EJ309, 0.001);
    getBoratedH2O(0.02);
    getBoratedH2O(0.05);
}

G4Element* MaterialsHelper::getEl(const string& name) const {
    return G4NistManager::Instance()->FindOrBuildElement(name);
}

G4Material* MaterialsHelper::getBoratedH2O(double loading) {
    if(!loading) return Water;
    string mnm = "H2O_"+to_str(100*loading)+"wt%_Boron";
    if(!xmats.count(mnm)) {
        G4Material* BW = new G4Material(mnm.c_str(), Water->GetDensity(), 2, Water->GetState(), Water->GetTemperature());
        const double massratio = (3*(15.999+1.008)+10.81)/10.81; // ratio of boric acid mass to boron mass
        BW->AddMaterial(Water, 1.-loading*massratio);
        BW->AddMaterial(BoricAcid, loading*massratio);
        xmats[mnm] = BW;
    }
    return xmats[mnm];
}

G4Material* MaterialsHelper::get6LiLS(G4Material* base, double loading, bool enriched) {
    if(!loading || !base) return base;
    
    string mnm = base->GetName()+string("-")+to_str(100*loading)+(enriched?"wt%-6Li":"wt%-Li");
    if(!xmats.count(mnm)) {
        G4cout << "Bulding 6Li-loaded (" << loading*100 << "% by weight) scintillator " << mnm << " ...\n";
        G4Material* myLi = enriched? Li6 : nat_Li;
        
        G4Material* Li_mat = NULL;
        double avgLiA = enriched? 6.02 : 0.075*6.02 + .925*7.02;        // Li average mass
        double m_Cl = loading*35.45/avgLiA;                             // mass fraction Cl (for equal LiCl mixture), by ratio of masses to Li
        if(base == UG_AB) {
            double m_H2O = (1000./8. - avgLiA -35.45)/avgLiA*loading;   // mass fraction H2O from 8 molar LiCl solution
            Li_mat = new G4Material(mnm.c_str(), base->GetDensity(), 4, base->GetState(), base->GetTemperature());
            Li_mat->AddMaterial(UG_AB, 1.-loading-m_Cl-m_H2O);
            Li_mat->AddMaterial(myLi, loading);
            Li_mat->AddMaterial(nat_Cl, m_Cl);
            Li_mat->AddMaterial(Water, m_H2O);
        } else if(base == EJ309) {
            double frac_H2O = 0.07*loading/0.001;
            Li_mat = new G4Material(mnm.c_str(), base->GetDensity(), 4, base->GetState(), base->GetTemperature());
            Li_mat->AddMaterial(base, 1.-loading-m_Cl-frac_H2O);
            Li_mat->AddMaterial(Water, frac_H2O);
            Li_mat->AddMaterial(myLi, loading);
            Li_mat->AddMaterial(nat_Cl, m_Cl);
        } else {
            Li_mat = new G4Material(mnm.c_str(), base->GetDensity(), 2, base->GetState(), base->GetTemperature());
            Li_mat->AddMaterial(base, 1.-loading);
            Li_mat->AddMaterial(myLi, loading);
        }
        
        Li_mat->SetMaterialPropertiesTable(base->GetMaterialPropertiesTable());
        Li_mat->GetIonisation()->SetBirksConstant(base->GetIonisation()->GetBirksConstant());
        
        xmats[mnm] = Li_mat;
        
    }
    return xmats[mnm];
}

G4Material* MaterialsHelper::get3He(double dens) {
    const string mnm = "3He_" + to_str(dens/(mg/cm3)) + "_mg/cm3";
    if(!xmats.count(mnm)) {
        G4Material* He3gas = new G4Material(mnm.c_str(), dens, 1, kStateGas, room_T);
        He3gas->AddElement(el3He,2);
        xmats[mnm] = He3gas;
    }
    return xmats[mnm];
}

void MaterialsHelper::setupOptical() {
    
    G4cout << "Setting material optical properties..." << G4endl;
    
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
    RawPsiCumene->GetIonisation()->SetBirksConstant(0.1*mm/MeV);

    ////////////////////////
    UG_AB->GetIonisation()->SetBirksConstant(0.1*mm/MeV); // TODO
    
    ////////////////////////
    EJ309->GetIonisation()->SetBirksConstant(0.1*mm/MeV); // TODO
    
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
    PVT->GetIonisation()->SetBirksConstant(0.2*mm/MeV);
    
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
