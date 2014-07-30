#include "DetectorConstruction.hh"
#include "MaterialsHelper.hh"

#include <G4SDManager.hh>
#include <G4PVPlacement.hh>
#include <G4ios.hh>

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    G4cerr << "Starting detector construction..." << G4endl;
    
    MaterialsHelper::init();
    myBuilding.construct();
      
    myScintSD = new ScintSD("ScintSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
    getScintLog()->SetSensitiveDetector(myScintSD);
    
    G4cerr << "Detector construction complete." << G4endl;
    
    // need to return the physical World Volume
    return new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), myBuilding.main_log,
                            "building_phys", NULL, false,  0);
}

/*
    ///////////////////////////
    // PMT dimension parameters
    G4double pmtSEG_i = 0.*PMTscale/(25.4*mm);
    G4double pmtedge_r = 25.4*mm;
    G4double pmtSEG_r = pmtedge_r*PMTscale/(25.4*mm);
    G4double pmtSEG_h = 100.*mm;
    G4double pmtSEGbase_r = 26.4*mm*PMTscale/(25.4*mm);
    G4double pmtSEGbase_h = 44.*mm;
    G4double coverSEG_r = pmtSEG_r + 1.*mm;
    G4double coverSEG_h = pmtSEG_h;
    G4double basepinSEG_r = 17.*mm*PMTscale/(25.4*mm);
    G4double basepinSEG_h = 20.*mm;
    G4double angle_s = 0.*deg;
    G4double angle_f = 360.*deg;
    
    G4double shell_w = (GetSegWidth()+AirGap)*NSegX;
    G4double shell_h = (GetSegHeight()+AirGap)*NSegY;
    G4double shell_l = GetSegLength() + 2.0*(pmtSEG_h + (pmtSEGbase_h + basepinSEG_h));
    
    G4ThreeVector shell_placement(0,0,0);
    
   
    
    G4LogicalVolume* scint_mother;
    
    if(fShieldActivated) {
        
        shell_w += 1.0*mm;
        shell_h += 1.0*mm;
        shell_l += 0.5*mm;
        
        if(fVertical) {  

            G4Box* shieldpolyb_box = new G4Box("ShieldPolyBBox", shell_w/2.+ ShieldPolyB + ShieldLead + ShieldPolyLi, shell_h/2. + ShieldPolyB + ShieldLead + ShieldPolyLi, shell_l/2.+ ShieldPolyB/2.+ ShieldLead/2.+ ShieldPolyLi/2.);    
            shieldpolyb_log = new G4LogicalVolume(shieldpolyb_box, BPoly, "ShieldPolyBLogical", 0,0,0);
            shieldpolyb_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,-0.5),
                                                 shieldpolyb_log, "PolyShieldB", hall_log, false,0,false);
            
            G4Box* shieldlead_box = new G4Box("ShieldLeadBox", shell_w/2.+ ShieldLead + ShieldPolyLi, shell_h/2. + ShieldLead + ShieldPolyLi, shell_l/2.+ ShieldLead/2. + ShieldPolyLi/2.);    
            shieldlead_log = new G4LogicalVolume(shieldlead_box, Pb, "ShieldLeadLogical", 0,0,0);
            shieldlead_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,-ShieldPolyB/2.),
                                                shieldlead_log, "LeadShield", shieldpolyb_log, false,0,false);
            
            G4Box* shieldpolyli_box = new G4Box("ShieldPolyLiBox", shell_w/2.+ ShieldPolyLi, shell_h/2. + ShieldPolyLi, shell_l/2.+ ShieldPolyLi/2.);
            shieldpolyli_log = new G4LogicalVolume(shieldpolyli_box, LiPoly, "ShieldPolyLiLogical", 0,0,0);
            shieldpolyli_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,- ShieldLead/2.),
                                                  shieldpolyli_log, "PolyShieldLi", shieldlead_log, false,0,false);
        } else { // !fVertical:
            
            G4RotationMatrix* rotateAssembly = new G4RotationMatrix(0.,0.,0.);
            rotateAssembly->rotateX(-90.*deg);

            G4Box* shieldpolyb_box = new G4Box("ShieldPolyBBox", shell_w/2.+ ShieldPolyB + ShieldLead + ShieldPolyLi, shell_h/2. + ShieldPolyB/2. + ShieldLead/2. + ShieldPolyLi/2., shell_l/2.+ ShieldPolyB+ ShieldLead+ ShieldPolyLi);    
            shieldpolyb_log = new G4LogicalVolume(shieldpolyb_box, BPoly, "ShieldPolyBLogical", 0,0,0);
            shieldpolyb_phys = new G4PVPlacement(rotateAssembly, G4ThreeVector(0.,-0.5,0.),
                                                 shieldpolyb_log, "PolyShieldB", hall_log, false,0,false);
            
            
            G4Box* shieldlead_box = new G4Box("ShieldLeadBox", shell_w/2.+ ShieldLead + ShieldPolyLi, shell_h/2. + ShieldLead/2. + ShieldPolyLi/2., shell_l/2.+ ShieldLead + ShieldPolyLi);    
            shieldlead_log = new G4LogicalVolume(shieldlead_box, Pb, "ShieldLeadLogical", 0,0,0);
            shieldlead_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,-ShieldPolyB/2.,0.),
                                                shieldlead_log, "LeadShield", shieldpolyb_log, false,0,false);
            
            G4Box* shieldpolyli_box = new G4Box("ShieldPolyLiBox", shell_w/2.+ ShieldPolyLi, shell_h/2. + ShieldPolyLi/2., shell_l/2.+ ShieldPolyLi);
            shieldpolyli_log = new G4LogicalVolume(shieldpolyli_box, LiPoly, "ShieldPolyLiLogical", 0,0,0);
            shieldpolyli_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,- ShieldLead/2.,0.),
                                                  shieldpolyli_log, "PolyShieldLi", shieldlead_log, false,0,false);
            
            shell_placement[1] -= 0.25; 
            
            shell_w -= 1.0*mm;
            shell_l -= 1.0*mm;
            shell_h -= 0.5*mm;
        }
        
    } else { // !fShieldActivated:
        assert(false);
    }
    
    /////////////////
    // Detector shell
    G4Box* shell_box = new G4Box("DetectorShellBox", shell_w/2., shell_h/2., shell_l/2.);
    shell_log = new G4LogicalVolume(shell_box, Air, "ShellLogical", 0,0,0);
    shell_phys = new G4PVPlacement(0, shell_placement, shell_log, "Detector Shell", shieldpolyli_log, false,0,false);
    
    
    ////////////////////////
    // scintillator segments
    G4Tubs* segment_cutout = new G4Tubs("Segment_cutout", pmtSEG_i, pmtSEG_r, WrapThickness/2, angle_s, angle_f); 
    G4SubtractionSolid* segment_box;
    G4VSolid* segment_b1;
    G4VSolid* wrapgap_box;
    G4VSolid* target_cyl;
    G4VSolid* scint_cyl;
    switch(fCylinderActivated) {
        case true: { 
            segment_b1 = new G4Tubs("SegmentMain", 0, GetSegWidth()/2., GetSegLength()/2., angle_s, angle_f); 
            wrapgap_box = new G4Tubs("WrapGapBox",0, GetSegWidth()/2.-WrapThickness, GetSegLength()/2.-WrapThickness, angle_s, angle_f); 
            target_cyl = new G4Tubs("TargetTankCylinder", 0, GetSegWidth()/2.-WrapThickness-WrapGap, GetSegLength()/2.-WrapThickness, angle_s, angle_f); 
            scint_cyl = new G4Tubs("ScintillatorCylinder", 0, ScintWidth/2., ScintLength/2., angle_s, angle_f); 
            break;
        }
        case false: { 
            segment_b1 = new G4Box("SegmentMain", GetSegWidth()/2., GetSegHeight()/2., GetSegLength()/2.); 
            wrapgap_box = new G4Box("WrapGapBox", GetSegWidth()/2.-WrapThickness, GetSegHeight()/2-WrapThickness, GetSegLength()/2.-WrapThickness); 
            target_cyl = new G4Box("TargetTankCylinder", GetSegWidth()/2.-WrapThickness-WrapGap, GetSegHeight()/2-WrapThickness-WrapGap, GetSegLength()/2.-WrapThickness); 
            scint_cyl = new G4Box("ScintillatorCylinder", ScintWidth/2., ScintHeight/2., ScintLength/2.); 
            break;
        }
    }
    segment_box = new G4SubtractionSolid("PMTSEGBaseSolid", segment_b1, segment_cutout, 0, G4ThreeVector(0.,0.,GetSegLength()/2.-WrapThickness/2.));
    segment_box = new G4SubtractionSolid("PMTSEGBaseSolid", segment_box, segment_cutout, 0, G4ThreeVector(0.,0.,-GetSegLength()/2.+WrapThickness/2.));
    
    if(WrapGap>0) wrapgap_log = new G4LogicalVolume(wrapgap_box, Air, "WrapGapLogical", 0,0,0);
    else  wrapgap_log = new G4LogicalVolume(wrapgap_box, PMMA, "WrapGapLogical", 0,0,0);
    
    target_log = new G4LogicalVolume(target_cyl, PMMA, "TargetTankLogical", 0,0,0);
            
    assert(MainScintMat && ScintSegMat);
    scint_log = new G4LogicalVolume(scint_cyl, *MainScintMat, "InnerScintLogical", 0,0,0);
    segment_log = new G4LogicalVolume(segment_box, *ScintSegMat, "SegmentLogical", 0,0,0);
    
    ///////////////
    // PMT geometry
    
    // vacuum-filled quartz cylinder attached to SS base, enclosed in SS cover
    // one on each side of each scintillator segment
    
    G4Tubs* cathSEG_tube = new G4Tubs("CATHSEGTube", pmtSEG_i, pmtSEG_r, pmtSEG_h/2, angle_s, angle_f);
    G4Tubs* pmtSEG_tube = new G4Tubs("PMTSEGTube", pmtSEG_i, pmtSEG_r-10.0*mm, pmtSEG_h/2-10.0*mm, angle_s, angle_f);
    G4Tubs* coverSEG_tube = new G4Tubs("PMTSEGCoverTube", pmtSEG_r, coverSEG_r, coverSEG_h/2, angle_s, angle_f);
    G4Tubs* baseSEG1_tube = new G4Tubs("PMTSEGBaseTube1", pmtSEG_i, pmtSEGbase_r, (pmtSEGbase_h - basepinSEG_h)/2, angle_s, angle_f);
    G4Tubs* baseSEG2_tube = new G4Tubs("PMTSEGBaseTube2", pmtSEG_i, basepinSEG_r, pmtSEGbase_h/2, angle_s, angle_f);
    
    G4RotationMatrix* rotBaseSEG = new G4RotationMatrix(0.,0.,0.);
    
    G4ThreeVector transBaseSEG(0.,0.,basepinSEG_h/2);
    G4UnionSolid* baseSEG_solid = new G4UnionSolid("PMTSEGBaseSolid", baseSEG1_tube, baseSEG2_tube, rotBaseSEG, transBaseSEG);
    
    G4RotationMatrix* pmtFlip = new G4RotationMatrix(0.,0.,0.);
    pmtFlip->rotateY(180.*deg);
    
    cathSEG_log = new G4LogicalVolume(cathSEG_tube, Quartz, "PMTCathodeLogical", 0,0,0);
    pmtSEG_log = new G4LogicalVolume(pmtSEG_tube, Vacuum, "PolyvinylToluenePMTLogical", 0,0,0);
    coverSEG_log = new G4LogicalVolume(coverSEG_tube, StSteel, "PMTSEGCoverLogical", 0,0,0);
    baseSEG_log = new G4LogicalVolume(baseSEG_solid, StSteel, "PMTSEGBaseLogical", 0,0,0);
                
    G4double xpos=0;
    G4double ypos=0;
    
    for(G4int xnum = 0; xnum < NSegX; xnum++){
        xpos = (GetSegWidth()+AirGap)*(xnum-(NSegX-1)/2.);
        for(G4int ynum = 0; ynum<NSegY; ynum++){
            ypos =  (GetSegHeight()+AirGap)*(ynum-(NSegY-1)/2.);
            
            G4int segnum = xnum + ynum*NSegX;
            G4String id1 = to_str(segnum);
            
            segment_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(xpos,ypos,0.), segment_log,
                                                         "Segment "+id1, shell_log, false,segnum,true);
            scint_mother = segment_log;
            
            if(WrapGap>0) {
                wrapgap_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), wrapgap_log,
                                                             "Target Tank "+id1, segment_log, false,segnum,true);
                scint_mother = wrapgap_log;
                if(AcrylThickness>0){
                    target_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), target_log,
                                                                "Target Tank "+id1, wrapgap_log, false,segnum,true);
                    scint_mother=target_log;
                }
            } else if(AcrylThickness>0) {
                target_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), target_log,
                                                            "Target Tank "+id1, segment_log, false,segnum,true);
                scint_mother = target_log;
            }
            
            scint_phys[xnum][ynum] = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), scint_log,
                                                       "Target Scintillator Volume "+id1, scint_mother, false,segnum,true);       
            
            // place PMTs on each side of each segment
            for(uint i=0; i<=1; i++) {
                int copynum = 10000*i + segnum;
                std::string idname = (i? "S" : "N") + id1;
                int smul = i? -1 : 1;
                G4RotationMatrix* doFlip = i? pmtFlip : NULL;
                
                cathSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                G4ThreeVector(xpos, ypos, smul * (GetSegLength()/2.+pmtSEG_h/2.-WrapThickness)),
                                                                cathSEG_log, "PMT Cathode "+idname, shell_log, false, copynum, true);
                pmtSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                               G4ThreeVector(0.,0.,0.),
                                                               pmtSEG_log, "PMT "+idname, cathSEG_log, false, copynum, true);
                coverSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                 G4ThreeVector(xpos, ypos, smul * (GetSegLength()/2.+pmtSEG_h/2.-WrapThickness)),
                                                                 coverSEG_log, "PMT Cover "+idname, shell_log, false, copynum, false);
                baseSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                G4ThreeVector(xpos, ypos, smul * (GetSegLength()/2.+pmtSEG_h/2.+ pmtSEG_h/2. + (pmtSEGbase_h - basepinSEG_h)/2-WrapThickness)),
                                                                baseSEG_log, "PMT Base "+idname, shell_log, false, copynum, false);
            }
        }  
    }
}
*/

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
 * void DetectorConstruction::SetScintillatorBirksConstant(G4double birks) {

    if(birks >= 0.) {
        birksPC = birks;
        G4Material::GetMaterial("Pseudocumene")->GetIonisation()->SetBirksConstant(birksPC);
        G4Material::GetMaterial("PC-0.15wt%Li")->GetIonisation()->SetBirksConstant(birksPC);
        G4Material::GetMaterial("PC-0.30wt%Li")->GetIonisation()->SetBirksConstant(birksPC);
        G4Material::GetMaterial("PC-0.1wt%Gd")->GetIonisation()->SetBirksConstant(birksPC);
        G4Material::GetMaterial("PC-0.5wt%Gd")->GetIonisation()->SetBirksConstant(birksPC);
        G4Material::GetMaterial("PC-100%Gd")->GetIonisation()->SetBirksConstant(birksPC);
        G4cout << "Inner Scintillator Birks Constant set to " << birksPC/(mm/MeV) << " mm/MeV." << G4endl;
    } else {
        G4cout << "*** ATTENTION: Birks Constant must be a positive number. Parameter not registered into the scintillator material. ***" << G4endl;
        return;
    }

    
    if(!fOptical)
        G4cout << "*** CAUTION: Optical processes have not been activated. Birks Constant parameter will not apply to simulation as is. ***" << G4endl;
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
                              