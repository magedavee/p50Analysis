#include "DetectorConstruction.hh"
#include "MaterialsHelper.hh"

#include <G4SDManager.hh>
#include <G4PVPlacement.hh>
#include <G4ios.hh>

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    G4cerr << "Starting detector construction..." << G4endl;
    myBuilding.construct();
    
    // assign sensitive detector to scintillator
    myScintSD = new ScintSD("ScintSD", myBuilding.myDetUnit.myDet.myTank);
    G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
    getScintLog()->SetSensitiveDetector(myScintSD);
    
    G4cerr << "Detector construction complete." << G4endl;
    
    // need to return the physical World Volume
    return new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), myBuilding.main_log,
                            "building_phys", NULL, false,  0);
}

/*
void DetectorConstruction::ConstructSDs() {
    
    G4cerr << "Setting up sensitive detectors..." << G4endl;
    
    scintHitInner = new G4MultiFunctionalDetector("scintHitInner");
    SegmentPMT = new G4MultiFunctionalDetector("SegmentPMT");
    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
    
    //////////
    // PMT SDs
    
    // photon energy array, 520nm to 360nm in 5nm steps
    const G4int nSize = (520-360)/5 + 1; // = 33
    std::vector<G4double> photEnergy;
    for(int i=0; i<nSize; i++) {
        G4double l = (520-5*i)*nm;
        photEnergy.push_back(CLHEP::h_Planck * CLHEP::c_light / l);
    }

    // PMT quantum efficiency at each photon energy
    PhotoMultiplierScorer* SEGPMT = new PhotoMultiplierScorer("SegmentHitCollection");
    G4double QEff3[nSize] = {
        0.114, 0.121, 0.128, 0.135, 0.142, 0.149, 0.156, 0.163, 0.170, 0.177, 0.184,
        0.191, 0.198, 0.205, 0.212, 0.218, 0.224, 0.230, 0.236, 0.242, 0.248, 0.254,
        0.260, 0.265, 0.271, 0.276, 0.281, 0.285, 0.288, 0.291, 0.293, 0.294, 0.295
    };
    if(QE >= 0) {
        G4cerr << "CHANGING QE to " << QE << G4endl;
        QE = QE<1. ? QE:1;
        for(int i=0;i<nSize;i++) QEff3[i] = QE;
    }
    std::vector<G4double> QESEG;
    QESEG.assign(QEff3,QEff3+nSize);
    SEGPMT->SetQuantumEfficiency(photEnergy,QESEG);
    
    SegmentPMT->clear();//RemovePrimitive(SEGPMT);
    SegmentPMT->RegisterPrimitive(SEGPMT);
    sd_manager->AddNewDetector((G4VSensitiveDetector*)(SegmentPMT));
    //pmtSEG_log->SetSensitiveDetector(SegmentPMT);
    cathSEG_log->SetSensitiveDetector(SegmentPMT);
}
*/                        


/* void DetectorConstruction::ConstructOpticalSurfaces() {
    
    G4cerr << "Constructing optical surfaces..." << G4endl;
    
    const G4int num = 2;
    G4double Ephoton[num] = { 1.5*eV, 6.0*eV };		// Photon Energies
    
    // GC Inner Tank vs. GC Scintillator Volume
    
    //changes for di-di ground back paint surface only surface only
    //  spike =1; //only specular reflection when TIR occurs
    //  G4double RIndex[num]        = { 1.300, 1.300 };	// Refractive index of reflector, PTFE
    //end changes for di-di ground back paint surface only surface only
    
    G4double Reflectivity2[num]  = { refl, refl };
    
    G4double Efficiency2[num]    = { efficiency, efficiency };
    G4double SpecularLobe2[num]  = { lobe, lobe };
    G4double SpecularSpike2[num] = { spike, spike };
    G4double Backscatter2[num]   = { back, back };
    
    G4MaterialPropertiesTable* mst = new G4MaterialPropertiesTable();
    //  mst->AddProperty("RINDEX", Ephoton, RIndex, num); //d-d groundback paint only
    mst->AddProperty("REFLECTIVITY", Ephoton, Reflectivity2, num);
    mst->AddProperty("EFFICIENCY", Ephoton, Efficiency2, num);
    mst->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobe2, num);
    mst->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpike2, num);
    mst->AddProperty("BACKSCATTERCONSTANT", Ephoton, Backscatter2, num);   
    G4OpticalSurface* InnerTankSurface = new G4OpticalSurface("InnerInSurface");
    
    InnerTankSurface->SetFinish(OptFinish);
    if(OptFinish<6) {
        InnerTankSurface->SetType(dielectric_metal);
        //InnerTankSurface->SetType(dielectric_dielectric);//d-d groundback paint only
        //InnerTankSurface->SetFinish(groundbackpainted);//d-d groundback paint only
        InnerTankSurface->SetModel(unified);
    } else {
        InnerTankSurface->SetType(dielectric_LUT);
        InnerTankSurface->SetModel(LUT);
    }
    
    InnerTankSurface->SetMaterialPropertiesTable(mst);
    
    // scintillator steel cladding optical surface
    // if(WrapGap>0) ScintSteel[i][j] = new G4LogicalSkinSurface("ScintSteelSurface"+id1, wrapgap_log, InnerTankSurface); //NSB 10/14/2013
    //      else 
    new G4LogicalSkinSurface("ScintSteelSurface", segment_log, InnerTankSurface); //NSB 10/14/2013
    
    // G4LogicalSkinSurface* WrapSurf = new G4LogicalSkinSurface("WrapSurface", wrapgap_log, InnerTankSurface);
    // G4MaterialPropertiesTable* wrapfin = new G4MaterialPropertiesTable();
    // if(WrapGap>0){  G4OpticalSurface* WrapGapSurface = new G4OpticalSurface("WrapGapSurface");
    //   WrapGapSurface->SetType(dielectric_dielectric);
    //   WrapGapSurface->SetFinish(polished); //NSB 10/14/2013
    //   //WrapGapSurface->SetFinish(ground);//NSB 10/14/2013
    //   //    WrapGapSurface->SetSigmaAlpha(sigal);//NSB 10/14/2013
    //   WrapGapSurface->SetModel(unified);
    
    //   G4LogicalSkinSurface* AcrylAir = new G4LogicalSkinSurface("AcrylAir", target_log, WrapGapSurface);//NSB 10/14/2013
    // }
    
    G4OpticalSurface* PMTRejectSurface = new G4OpticalSurface("RejectSurface");
    PMTRejectSurface->SetType(dielectric_dielectric);
    PMTRejectSurface->SetFinish(groundfrontpainted);
    PMTRejectSurface->SetModel(unified);
    
    G4double Reflectivity5[num] = { 0.245, 0.305 };
    
    G4MaterialPropertiesTable* mstReject = new G4MaterialPropertiesTable();
    mstReject->AddProperty("REFLECTIVITY", Ephoton, Reflectivity5, num);
    mstReject->AddProperty("EFFICIENCY", Ephoton, Efficiency2, num);
    mstReject->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobe2, num);
    mstReject->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpike2, num);
    mstReject->AddProperty("BACKSCATTERCONSTANT", Ephoton, Backscatter2, num);
    PMTRejectSurface->SetMaterialPropertiesTable(mstReject);
    
    new G4LogicalSkinSurface("CoverAirSurface", coverSEG_log, PMTRejectSurface);
    new G4LogicalSkinSurface("BaseAirSurface", baseSEG_log, PMTRejectSurface);
    
    
    // Muon Veto Panel Foil Wrapping
    
    G4OpticalSurface* VetoFoilSurface = new G4OpticalSurface("VetoFoilSurface");
    VetoFoilSurface->SetType(dielectric_metal);
    VetoFoilSurface->SetFinish(polishedbackpainted);	// Dielectric-dielectric for interface with thin air layer, metallic back surface is implied with polished reflectance
    VetoFoilSurface->SetModel(unified);
    
    G4double RIndexFoil[num]        = { 1.000, 1.000 };	// Refractive index of air layer
    G4double ReflectivityFoil[num]  = { 0.700, 0.700 };	// Reflectivity of foil
    G4double SpecularLobeFoil[num]  = { 0.300, 0.300 };
    G4double SpecularSpikeFoil[num] = { 0.500, 0.500 };
    G4double BackscatterFoil[num]   = { 0.000, 0.000 };
    
    G4MaterialPropertiesTable* mstVetoFoil = new G4MaterialPropertiesTable();
    mstVetoFoil->AddProperty("RINDEX", Ephoton, RIndexFoil, num);
    mstVetoFoil->AddProperty("REFLECTIVITY", Ephoton, ReflectivityFoil, num);
    mstVetoFoil->AddProperty("SPECULARLOBECONSTANT", Ephoton, SpecularLobeFoil, num);
    mstVetoFoil->AddProperty("SPECULARSPIKECONSTANT", Ephoton, SpecularSpikeFoil, num);
    mstVetoFoil->AddProperty("BACKSCATTERCONSTANT", Ephoton, BackscatterFoil, num);
    VetoFoilSurface->SetMaterialPropertiesTable(mstVetoFoil);
    
    // veto air and steel
    for(int i = 0; i < 12; i++) {
        G4String id = to_str(i);
        new G4LogicalBorderSurface("Veto"+id+"AirSurface", door_phys[i], shell_phys, VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface1", door_phys[i], shield_phys[0][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface2", door_phys[i], shield_phys[0][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface3", door_phys[i], shield_phys[1][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface4", door_phys[i], shield_phys[1][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface5", door_phys[i], shield_phys[2][0], VetoFoilSurface);
    }
    for(int i = 12; i < 22; i++) {
        G4String id = to_str(i);
        new G4LogicalBorderSurface("Veto"+id+"AirSurface", side_phys[i-12], shell_phys, VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface1", side_phys[i-12], shield_phys[0][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface2", side_phys[i-12], shield_phys[0][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface3", side_phys[i-12], shield_phys[1][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface4", side_phys[i-12], shield_phys[1][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface5", side_phys[i-12], shield_phys[2][0], VetoFoilSurface);
    }
    for(int i = 22; i < 33; i++) {
        G4String id = to_str(i);
        new G4LogicalBorderSurface("Veto"+id+"AirSurface", top_phys[i-22], shell_phys, VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface1", top_phys[i-22], shield_phys[0][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface2", top_phys[i-22], shield_phys[0][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface3", top_phys[i-22], shield_phys[1][0], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface4", top_phys[i-22], shield_phys[1][1], VetoFoilSurface);
        new G4LogicalBorderSurface("Veto"+id+"SteelSurface5", top_phys[i-22], shield_phys[2][0], VetoFoilSurface);
    }
} */


/*
void DetectorConstruction::SetOpticalFinish(int optfin) {

    switch(optfin){
        case 0: OptFinish=polished; break;                    // smooth perfectly polished surface
        case 1: OptFinish=polishedfrontpainted; break;        // smooth top-layer (front) paint
        case 2: OptFinish=polishedbackpainted; break;         // same is 'polished' but with a back-paint
  
        case 3: OptFinish=ground; break;                      // rough surface
        case 4: OptFinish=groundfrontpainted; break;          // rough top-layer (front) paint
        case 5: OptFinish=groundbackpainted; break;           // same as 'ground' but with a back-paint
 
        case 6: OptFinish=polishedlumirrorair; break;         // mechanically polished surface, with lumirror
        case 7: OptFinish=polishedlumirrorglue; break;        // mechanically polished surface, with lumirror & meltmount
        case 8: OptFinish=polishedair; break;                 // mechanically polished surface
        case 9: OptFinish=polishedteflonair; break;           // mechanically polished surface, with teflon
        case 10: OptFinish=polishedtioair; break;              // mechanically polished surface, with tio paint
        case 11: OptFinish=polishedtyvekair; break;            // mechanically polished surface, with tyvek
        case 12: OptFinish=polishedvm2000air; break;           // mechanically polished surface, with esr film
        case 13: OptFinish=polishedvm2000glue; break;          // mechanically polished surface, with esr film & meltmount
  
        case 14: OptFinish=etchedlumirrorair; break;           // chemically etched surface, with lumirror
        case 15: OptFinish=etchedlumirrorglue; break;          // chemically etched surface, with lumirror & meltmount
        case 16: OptFinish=etchedair; break;                   // chemically etched surface
        case 17: OptFinish=etchedteflonair; break;             // chemically etched surface, with teflon
        case 18: OptFinish=etchedtioair; break;                // chemically etched surface, with tio paint
        case 19: OptFinish=etchedtyvekair; break;              // chemically etched surface, with tyvek
        case 20: OptFinish=etchedvm2000air; break;             // chemically etched surface, with esr film
        case 21: OptFinish=etchedvm2000glue; break;            // chemically etched surface, with esr film & meltmount
  
        case 22: OptFinish=groundlumirrorair; break;           // rough-cut surface, with lumirror
        case 23: OptFinish=groundlumirrorglue; break;          // rough-cut surface, with lumirror & meltmount
        case 24: OptFinish=groundair; break;                   // rough-cut surface
        case 25: OptFinish=groundteflonair; break;             // rough-cut surface, with teflon
        case 26: OptFinish=groundtioair; break;                // rough-cut surface, with tio paint
        case 27: OptFinish=groundtyvekair; break;              // rough-cut surface, with tyvek
        case 28: OptFinish=groundvm2000air; break;             // rough-cut surface, with esr film
        case 29: OptFinish=groundvm2000glue; break;             // rough-cut surface, with esr film & meltmount
    }

    }     */
