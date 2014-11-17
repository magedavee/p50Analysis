#include "DetectorConstruction.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4SDManager.hh>
#include <G4PVPlacement.hh>
#include <G4ios.hh>
#include <G4Sphere.hh>

#include <cassert>

DetectorConstruction::DetectorConstruction():
Builder("DetectorConstruction"), mode(PROSPECT), worldShell(0.5*m),
geomDir("/geom/"), modeCmd("/geom/mode",this) {
    modeCmd.SetGuidance("Set geometry mode.");
    modeCmd.AvailableForStates(G4State_PreInit);
    modeCmd.SetCandidates("PROSPECT PROSPECT2 scintCell slab sphere");
    worldShell.mat = MaterialsHelper::M().Vacuum;
}

void DetectorConstruction::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &modeCmd) {
        if(newValue == "PROSPECT") mode = PROSPECT;
        else if(newValue == "PROSPECT2") mode = PROSPECT2;
        else if(newValue == "scintCell") mode = TEST_CELL;
        else if(newValue == "slab") mode = SLAB;
        else if(newValue == "sphere") mode = SPHERE;
        else G4cout << "Unknown mode!" << G4endl;
    } else G4cout << "Unknown command!" << G4endl;
}

ScintSegVol* DetectorConstruction::getScint() {
    if(mode == PROSPECT) return &myBuilding.myDetUnit.myDet.myTank;
    else if(mode == TEST_CELL || mode == PROSPECT2) return &myTestCell;
    else if(mode == SLAB) return &mySlab;
    else if(mode == SPHERE) return &mySphere;
    return NULL;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    G4cout << "Starting detector construction..." << G4endl;
   
    addChild(&worldShell);
    
    Builder& myContents = (     mode==PROSPECT ? (Builder&)myBuilding
                                : mode==PROSPECT2? (Builder&)myPR2Shield
                                : mode==SLAB ? (Builder&)mySlab
                                : mode==TEST_CELL ? (Builder&)myTestCell
                                : (Builder&)mySphere );
    myContents.construct();
    addChild(&myContents);
    
    if(mode==TEST_CELL) {
        worldShell.mat = MaterialsHelper::M().Air;
    } else if(mode==SLAB) {
        worldShell.lthick[2] =  worldShell.uthick[2];
    } else if(mode==PROSPECT) {
        worldShell.setSideThick(25*m);
    } else if(mode==SPHERE) {
        worldShell.setThick(0.5*m);
    } else if(mode==PROSPECT2) {
        myTestCell.construct();
        addChild(&myTestCell);
        new G4PVPlacement(Builder::rot_X_90, G4ThreeVector(), myTestCell.main_log, "cell_phys", myPR2Shield.cave_log, false, 0, true);
    }
    
    dim = myContents.getDimensions();
    main_log = myContents.main_log;
    ptclTrg = worldShell.wrap(main_log, dim, "world");
    
    if(mode==SPHERE) mySphere.scint_phys = ptclTrg;
    
    if(mode == TEST_CELL) {
        G4Sphere* sun_sphere = new G4Sphere("sun_sphere", 0, 1.*mm, 0, 2*M_PI, 0, M_PI);
        G4LogicalVolume* sun_log = new G4LogicalVolume(sun_sphere, MaterialsHelper::M().Vacuum, "sun_log");
        ptclSrc = new G4PVPlacement(NULL, G4ThreeVector(30.*cm,0.,0.), sun_log, "sun_phys", main_log, false,  0);
    }
    
    G4cout << *(G4Material::GetMaterialTable()); // print list of all materials
    
    G4cout << "Detector construction complete." << G4endl;
    
    // need to return the physical World Volume
    theWorld = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), main_log, "world_phys", NULL, false,  0);
    
    if(getScint()) {
        myScintSD = new ScintSD("ScintSD", *getScint(), theWorld);
        G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
        getScint()->setScintSD(myScintSD);
    }
    
    return theWorld;
}

void DetectorConstruction::fillNode(TXMLEngine& E) {
    addAttr(E, "mode", (mode==TEST_CELL)?"TestCell":"PROSPECT");
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    if(myScintSD) addAttr(E,"scint_e_density",myScintSD->mat_n);
}

/*
void DetectorConstruction::ConstructSDs() {
    
    G4cout << "Setting up sensitive detectors..." << G4endl;
    
    scintHitInner = new G4MultiFunctionalDetector("scintHitInner");
    SegmentPMT = new G4MultiFunctionalDetector("SegmentPMT");
    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();
    
    //////////
    // PMT SDs
    
    // photon energy array, 520nm to 360nm in 5nm steps
    const G4int nSize = (520-360)/5 + 1; // = 33
    vector<G4double> photEnergy;
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
        G4cout << "CHANGING QE to " << QE << G4endl;
        QE = QE<1. ? QE:1;
        for(int i=0;i<nSize;i++) QEff3[i] = QE;
    }
    vector<G4double> QESEG;
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
