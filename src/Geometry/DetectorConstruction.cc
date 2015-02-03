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
ShellLayerBuilder("DetectorConstruction"), mode(PROSPECT), worldShell(0.5*m),
geomDir("/geom/"), modeCmd("/geom/mode",this) {
    modeCmd.SetGuidance("Set geometry mode.");
    modeCmd.AvailableForStates(G4State_PreInit);
    modeCmd.SetCandidates("PROSPECT PROSPECT2 PROSPECT20 P20Inner P20Cell scintCell slab sphere");
    worldShell.mat = MaterialsHelper::M().Vacuum;
}

void DetectorConstruction::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &modeCmd) {
        modeName = newValue;
        if(modeName == "PROSPECT") mode = PROSPECT;
        else if(modeName == "PROSPECT2") mode = PROSPECT2;
        else if(modeName == "PROSPECT20") mode = PROSPECT20;
        else if(modeName == "P20Inner") mode = P20INNER;
        else if(modeName == "P20Cell") mode = P20CELL;
        else if(modeName == "scintCell") mode = TEST_CELL;
        else if(modeName == "slab") mode = SLAB;
        else if(modeName == "sphere") mode = SPHERE;
        else { modeName = "???"; G4cout << "Unknown mode '" << newValue << "'!" << G4endl; assert(false); }
    } else G4cout << "Unknown command!" << G4endl;
}

ScintSegVol* DetectorConstruction::getScint() {
    if(mode == PROSPECT) return &myPRShield.myDet.myTank;
    else if(mode == PROSPECT20 || mode==P20INNER || mode==P20CELL) return &myPR20Cell;
    else if(mode == TEST_CELL || mode == PROSPECT2) return &myTestCell;
    else if(mode == SLAB) return &mySlab;
    else if(mode == SPHERE) return &mySphere;
    return NULL;
}

void DetectorConstruction::_construct() {
    addLayer(worldShell);
    construct_layers();
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    G4cout << "Starting detector construction..." << G4endl;
    
    myContents = (  (mode==PROSPECT || mode==PROSPECT2 || mode==PROSPECT20 || mode==P20INNER) ? &myBuilding
                    : mode==SLAB ? &mySlab
                    : mode==TEST_CELL ? &myTestCell
                    : mode==P20CELL ? &myPR20Cell
                    : (Builder*)&mySphere );

    if(mode==TEST_CELL) {
        worldShell.mat = MaterialsHelper::M().Air;
        //worldShell.setThick(3*m);
    } else if(mode==SLAB) {
        worldShell.lthick[2] =  worldShell.uthick[2];
    } else if(mode==PROSPECT) {
        myBuilding.myContents = &myPRShield;
    } else if(mode==SPHERE) {
        worldShell.setThick(0.5*m);
    } else if(mode==PROSPECT2) {
        myBuilding.myContents = &myPR2Shield;
        myPR2Shield.myContents = &myTestCell;
        myPR2Shield.placementRot = Builder::rot_X_90;
        myBuilding.wall_clearance = myBuilding.ceil_clearance = 0.25*m;
        worldShell.setThick(0.2*m);
    } else if(mode==PROSPECT20) {
        myBuilding.myContents = &myPR20Shield;
        myPR20Shield.myInnerShield.myContents = &myPR20Cell;
    } else if(mode==P20INNER) {
        myBuilding.myContents = &myPR20Shield.myInnerShield;
        myPR20Shield.myInnerShield.myContents = &myPR20Cell;
    }
    
    if(myCore.pos.mag2()) {
        myCore.construct();
        addChild(&myCore);
        worldShell.setThick(myCore.pos.mag()+myCore.height);
    }
        
    construct();
    
    ptclTrg = myContents->main_phys;
    
    if(mode==SPHERE) mySphere.scint_phys = ptclTrg;
    
    if(myCore.pos.mag2()) {
        ptclSrc = new G4PVPlacement(NULL, myCore.pos, myCore.main_log, "core_phys", main_log, false,  0, true);
        if(getScint()) ptclTrg = getScint()->scint_phys;
    }
    
    if(mode == TEST_CELL) {
        //G4Sphere* sun_sphere = new G4Sphere("sun_sphere", 0, 2*cm, 0, 2*M_PI, 0, M_PI);
        //G4LogicalVolume* sun_log = new G4LogicalVolume(sun_sphere, MaterialsHelper::M().Vacuum, "sun_log");
        //ptclSrc = new G4PVPlacement(NULL, G4ThreeVector(200.*cm,200.*cm,200.*cm), sun_log, "sun_phys", main_log, false,  0);
    } else if(mode == PROSPECT2) {
        for(int nx = 0; nx<2; nx++) {
            for(int nz = 0; nz<2; nz++) {
                int nn = nx+2*nz;
                myPR2Veto.push_back(new PR2MuVetoBuilder(nn));
                PR2MuVetoBuilder* V = myPR2Veto.back();
                V->construct();
                addChild(V);
                G4ThreeVector vdim = V->getDimensions();
                double veto_z = myBuilding.floor_pos[2] + myPR2Shield.getDimensions()[2] + 0.5*vdim[2] + 1*cm;
                G4ThreeVector veto_pos((nx-0.5)*(vdim[0]+1*cm), 0, veto_z + 2*nz*vdim[2]);
                V->scint_phys = new G4PVPlacement(NULL, veto_pos, V->main_log, ("veto_phys_"+to_str(nn)).c_str(), myBuilding.inside_log, false, 0, true);
            }
        }
    } else if(mode == PROSPECT20) {
        for(int nn = 0; nn < 4; nn++) {
            myPR2Veto.push_back(new PR2MuVetoBuilder(nn));
            PR2MuVetoBuilder* V = myPR2Veto.back();
            V->construct();
            addChild(V);
            G4ThreeVector vdim = V->getDimensions();
            double veto_z = myBuilding.floor_pos[2] + myPR20Shield.getDimensions()[2] + 0.5*vdim[2] + 1*in;
            G4ThreeVector veto_pos( (nn==0? -15: nn==1? 5: nn==2? -5 : 15)*in, 0, veto_z + (nn/2)*(vdim[2] + 1*mm) );
            V->scint_phys = new G4PVPlacement(NULL, veto_pos, V->main_log, ("veto_phys_"+to_str(nn)).c_str(), myBuilding.inside_log, false, 0, true);
        }
    }
    
    G4cout << *(G4Material::GetMaterialTable()); // print list of all materials
    
    G4cout << "Detector construction complete." << G4endl;
    
    // world physical volume
    theWorld = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), main_log, "world_phys", NULL, false,  0);
    
    // sensitive detectors
    if(getScint()) {
        myScintSD = new ScintSD("ScintSD", *getScint(), theWorld);
        G4SDManager::GetSDMpointer()->AddNewDetector(myScintSD);
        getScint()->setScintSD(myScintSD);
    }
    for(size_t i=0; i<myPR2Veto.size(); i++) {
        ScintSD* vetoSD = new ScintSD("VetoSD_"+to_str(i), *myPR2Veto[i], theWorld);
        G4SDManager::GetSDMpointer()->AddNewDetector(vetoSD);
        myPR2Veto[i]->setScintSD(vetoSD);
    }
    
    return theWorld; // need to return the physical World Volume
}

void DetectorConstruction::fillNode(TXMLEngine& E) {
    addAttr(E, "mode", modeName);
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
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
