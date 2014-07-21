// Unrestricted Use - Property of AECLf
//
// DetectorConstruction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Geometry, Material, and Sensitive Volume Specifications
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "DetectorConstruction.hh"              // Specifies the file which contains the class structure

#include "NeutronDetectionScorer.hh"            // Specifies user-defined classes which are called upon in this class
#include "RecoilProtonScorer.hh"
#include "IonisationScorer.hh"
#include "PhotoMultiplierScorer.hh"
#include "DetectorMessenger.hh"

#include "G4Element.hh"                         // These are the GEANT4 classes necessary to construct the desired materials
#include "G4Material.hh"                        //      |
#include "G4Isotope.hh"                         //      |
#include "G4NistManager.hh"                     //      V

#include "G4Box.hh"                             // These are the GEANT4 classes necessary to construct the desired geometries
#include "G4Tubs.hh"                            //      |
#include "G4Para.hh"                            //      |
#include "G4Sphere.hh"                          //      |
#include "G4Trd.hh"                             //      |
#include "G4Cons.hh"                            //      |
#include "G4UnionSolid.hh"                      //      |
#include "G4SubtractionSolid.hh"                //      |
#include "G4IntersectionSolid.hh"               //      |
#include "G4LogicalVolume.hh"                   //      |
#include "G4VPhysicalVolume.hh"                 //      |
#include "G4RotationMatrix.hh"                  //      |
#include "G4Transform3D.hh"                     //      |
#include "G4ThreeVector.hh"                     //      |
#include "G4PVPlacement.hh"                     //      |
#include "G4RunManager.hh"                      //      |
#include "G4GeometryManager.hh"                 //      |
#include "G4PhysicalVolumeStore.hh"             //      |
#include "G4LogicalVolumeStore.hh"              //      |
#include "G4SolidStore.hh"                      //      V

#include "G4LogicalBorderSurface.hh"            // These are the GEANT4 classes necessary to construct the desired surfaces
#include "G4LogicalSkinSurface.hh"              //      |
#include "G4OpBoundaryProcess.hh"               //      V

#include "G4SDManager.hh"                       // These are the GEANT4 classes necessary to create a sensitive volume
#include "G4VSensitiveDetector.hh"              //      |
#include "G4VPrimitiveScorer.hh"                //      |
#include "G4MultiFunctionalDetector.hh"         //      |
#include "G4SDParticleFilter.hh"                //      V

#include "G4VisAttributes.hh"                   // These are the GEANT4 class necessary to define the visual attributes of the geometry for use in OpenGL
#include "G4Colour.hh"                          //      V

#include "G4UnitsTable.hh"
#include "G4ios.hh"                             // Specifies the classes which allow reading/writing into standard input/output

#include "globals.hh"                           // Specifies class defining all global parameters and variables types (double, int, string, etc.)

#include <math.h>

/// utility function for converting to string
template<typename T>
std::string to_str(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

DetectorConstruction::DetectorConstruction() {
    
    det_messenger = new DetectorMessenger(this);
    
    // "world" building size
    modSizeX = 5.*m;    // Half-Length - long edge
    modSizeY = 3.*m;   // Half-Height
    modSizeZ = 5.*m;    // Half-Width - short edge
    
    // scintillator light production and transport
    birksPC = 0.1*mm/MeV;
    birksPVT = 0.2*mm/MeV;
    refl = 1.0;
    lobe = 0.0;
    sigal = 0.0;
    spike = 0.0;
    back = 0.0;
    efficiency=1.0;
    OptFinish = ground;
    
    fOptical = true;
    fInnerActivated = true;
    fOuterActivated = true;
    fBPolyActivated = false;
    fShieldActivated = false;
    fCylinderActivated = false;
    fVetoActivated = false;
    fVertical = true;
    
    QE = -1;
    NSegX = 4;
    NSegY = 12;
    AirGap = 12.7*mm;           
    WrapGap = 0.0*mm;
    WrapThickness = 0.1*mm;     
    AcrylThickness = 6.35*mm;
    SegBuffer = 100.0*mm;
    ScintLength = 1000.0*mm ;
    ScintHeight =  140.0*mm;
    ScintWidth =  140.0*mm;
    
    PMTscale = 2.5*25.4*mm;
    ShieldLead = 30.0*mm;
    ShieldPolyLi = 100.0*mm;
    ShieldPolyB = 470.0*mm;
    
    MainScintMat = &PsiCumeneT;
    ScintSegMat = &Polyeth;
}

DetectorConstruction::~DetectorConstruction() {
    delete cath_vis;
    delete build_vis;
    delete hall_vis;
    delete shieldlead_vis;
    delete shieldpolyb_vis;
    delete shieldpolyli_vis;
    delete shell_vis;
    delete segment_vis;
    delete wrapgap_vis;
    delete target_vis;
    delete scint_vis;
    delete shield_vis;
    delete pmt_vis;
    delete cover_vis;
    delete cath_vis;
    delete base_vis;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    
    G4cerr << "Starting detector construction..." << G4endl;
    
    ConstructMaterials();
    ConstructMaterialProperties();
    ConstructGeometry();
    ConstructOpticalSurfaces();
    ConstructSDs();
    SetupVisualization();
    
    G4cerr << "Detector construction complete." << G4endl;
    
    return build_phys;      // *** IMPORTANT: This function should always return the physical World Volume! ***
}

void DetectorConstruction::ConstructGeometry() {
    
    G4cerr << "Building detector geometry..." << G4endl;
    
    ///////////////////////////////////
    // concrete building "World" volume
    G4Box* build_box = new G4Box("BuildBox", modSizeX+2.0*m, modSizeY+2.0*m, modSizeZ+2.0*m);    
    build_log = new G4LogicalVolume(build_box, concrete, "BuildLogical", 0,0,0);
    build_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), build_log, "Build", 0, false,0,false);
    
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
    
    ///////////////////////////////
    // Experimental hall air volume
    G4Box* hall_box = new G4Box("HallBox", modSizeX, modSizeY, modSizeZ);
    hall_log = new G4LogicalVolume(hall_box, Air, "HallLogical", 0,0,0);
    hall_phys = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), hall_log, "Hall", build_log, false,0,false);
    
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
            
            G4Box* shieldpolyb_box = new G4Box("ShieldPolyBBox", shell_w/2.+ ShieldPolyB + ShieldLead + ShieldPolyLi, shell_h/2. + ShieldPolyB/2. + ShieldLead/2. + ShieldPolyLi/2., shell_l/2.+ ShieldPolyB+ ShieldLead+ ShieldPolyLi);    
            shieldpolyb_log = new G4LogicalVolume(shieldpolyb_box, BPoly, "ShieldPolyBLogical", 0,0,0);
            shieldpolyb_phys = new G4PVPlacement(NULL, G4ThreeVector(0.,-0.5,0.),
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
            
            G4String id1 = to_str(100*xnum+ynum);
            
            PositionX[xnum+ynum*NSegX] = xpos;
            PositionY[xnum+ynum*NSegX] = ypos;
            
            segment_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(xpos,ypos,0.), segment_log,
                                                         "Segment "+id1, shell_log, false,100*xnum+ynum,true);
            scint_mother = segment_log;
            
            if(WrapGap>0) {
                wrapgap_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), wrapgap_log,
                                                             "Target Tank "+id1, segment_log, false,100*xnum+ynum,true);
                scint_mother = wrapgap_log;
                if(AcrylThickness>0){
                    target_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), target_log,
                                                                "Target Tank "+id1, wrapgap_log, false,100*xnum+ynum,true);
                    scint_mother=target_log;
                }
            } else if(AcrylThickness>0) {
                target_phys[xnum][ynum] = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), target_log,
                                                            "Target Tank "+id1, segment_log, false,100*xnum+ynum,true);
                scint_mother = target_log;
            }
            
            scint_phys[xnum][ynum] = new G4PVPlacement(NULL, G4ThreeVector(0.,0.,0.), scint_log,
                                                       "Target Scintillator Volume "+id1,scint_mother, false,xnum+ynum*NSegX,true);       
            
            // place PMTs on each side of each segment
            for(uint i=0; i<=1; i++) {
                int copynum = 10000*i + 100*xnum + ynum;
                std::string idname = (i? "S" : "N") + id1;
                int s = i? -1 : 1;
                G4RotationMatrix* doFlip = i? pmtFlip : NULL;
                
                cathSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                G4ThreeVector(xpos, ypos, s * (GetSegLength()/2.+pmtSEG_h/2.-WrapThickness)),
                                                                cathSEG_log, "PMT Cathode "+idname, shell_log, false, copynum, true);
                pmtSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                               G4ThreeVector(0.,0.,0.),
                                                               pmtSEG_log, "PMT "+idname, cathSEG_log, false, copynum, true);
                coverSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                 G4ThreeVector(xpos, ypos, s * (GetSegLength()/2.+pmtSEG_h/2.-WrapThickness)),
                                                                 coverSEG_log, "PMT Cover "+idname, shell_log, false, copynum, false);
                baseSEG_phys[i][xnum][ynum] = new G4PVPlacement(doFlip,
                                                                G4ThreeVector(xpos, ypos, s * (GetSegLength()/2.+pmtSEG_h/2.+ pmtSEG_h/2. + (pmtSEGbase_h - basepinSEG_h)/2-WrapThickness)),
                                                                baseSEG_log, "PMT Base "+idname, shell_log, false, copynum, false);
            }
        }  
    }
}

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
        photEnergy.push_back(h_Planck * c_light / l);
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
    
    ///////////////////
    // scintillator SDs
    scintHitInner->clear();
    scintHitInner->RegisterPrimitive(new NeutronDetectionScorer("NHitCollection"));
    scintHitInner->RegisterPrimitive(new RecoilProtonScorer("PHitCollection"));
    scintHitInner->RegisterPrimitive(new IonisationScorer("IoniseCollection"));
    sd_manager->AddNewDetector((G4VSensitiveDetector*)(scintHitInner));
    scint_log->SetSensitiveDetector(scintHitInner);
}


void DetectorConstruction::SetupVisualization() {
    
    G4cerr << "Setting visualization attributes..." << G4endl;
    // segment_vis->SetForceSolid(true);
    // segment_vis->SetVisibility(false);
    
    
    // Building volume
    build_vis = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
    build_log->SetVisAttributes(build_vis);
    G4cerr << "Gray:\tbuilding" << G4endl;
    
    // Experiment hall air
    hall_vis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    hall_log->SetVisAttributes(hall_vis);
    G4cerr << "Green:\thall air" << G4endl;
            
    if(fShieldActivated){
        // Borated poly shield
        shieldpolyb_vis = new G4VisAttributes(G4Colour(0.0, 0.5, 0.0));
        shieldpolyb_log->SetVisAttributes(shieldpolyb_vis);
        G4cerr << "Dark green:\tborated poly" << G4endl;
        
        // Lead shield
        shieldlead_vis = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
        shieldlead_log->SetVisAttributes(shieldlead_vis);
        G4cerr << "Cyan:\tlead shield" << G4endl;
        
        // 6Li poly shield
        shieldpolyli_vis = new G4VisAttributes(G4Colour(0.7, 0.5, 0.0));
        shieldpolyli_log->SetVisAttributes(shieldpolyli_vis);
        G4cerr << "Orange: \t6Li poly" << G4endl;
    }
    
    // Detector Shell
    shell_vis = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    shell_log->SetVisAttributes(shell_vis);
    G4cerr << "Blue:\tdetector shell air volume" << G4endl;
    
    // Wrap Gap
    if(WrapGap > 0) {
        wrapgap_vis= new G4VisAttributes(G4Colour(0.0,1.0,0.0));
        wrapgap_vis->SetForceAuxEdgeVisible (true);
        wrapgap_log->SetVisAttributes(wrapgap_vis);
        G4cerr << "Green:\twrap gap" << G4endl;
    }

    // Target Tank
    target_vis = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
    target_vis->SetForceAuxEdgeVisible (true);
    target_log->SetVisAttributes(target_vis);
    G4cerr << "Gray:\ttarget tank" << G4endl;
    
    // Scintillator segment
    segment_vis = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
    segment_log->SetVisAttributes(segment_vis);
    G4cerr << "Red:\tdetector segment" << G4endl;
    
    // Liquid Scintillator Volume
    scint_vis = new G4VisAttributes(G4Colour(1.,1.,0.));
    scint_vis->SetForceAuxEdgeVisible (true);
    scint_log->SetVisAttributes(scint_vis);
    G4cerr << "Yellow:\tscintillator volume" << G4endl;
    
    // Photomultiplier Tubes (PMTs)
    cath_vis = new G4VisAttributes(G4Colour(1.,1.,1.));
    cath_vis->SetForceAuxEdgeVisible(true);
    cathSEG_log->SetVisAttributes(cath_vis);
    G4cerr << "White:\tPMT photocathode" << G4endl;
    
    // Photomultiplier Tubes (PMTs)
    pmt_vis = new G4VisAttributes(G4Colour(0.0,0.7,1.0));
    pmt_vis->SetForceAuxEdgeVisible(true);
    pmtSEG_log->SetVisAttributes(pmt_vis);
    G4cerr << "Cyan:\tPMT segment" << G4endl;
    
    // PMT Covers
    cover_vis = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
    cover_vis->SetForceAuxEdgeVisible(true);
    coverSEG_log->SetVisAttributes(cover_vis);
    G4cerr << "Gray:\tPMT cover" << G4endl;
    
    // PMT Backings
    base_vis = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
    base_vis->SetForceAuxEdgeVisible(true);
    baseSEG_log->SetVisAttributes(base_vis);
    G4cerr << "Dark gray:\tPMT base" << G4endl;
}
                        
void DetectorConstruction::ConstructMaterials() {
    
    G4cerr << "Initializing materials..." << G4endl;
    
    G4double a;         // atomic mass
    G4double z;         // atomic number
    G4double density;   // density
    G4double T;         // temperature
    G4double P;         // pressure
    
    G4NistManager* nist = G4NistManager::Instance();	// NIST Manager contains recipes for pre-defined elements/materials
    
    G4int nComponents = 0;              // number of component elements
    G4double massFraction = 0.0;        // weight percent of component element
    
    // Isotopes
    
    // Elements
    Pb = nist->FindOrBuildMaterial("G4_Pb", true, true);
    G4Material* B = nist->FindOrBuildMaterial("G4_B", true, true);
    G4Material* Li = nist->FindOrBuildMaterial("G4_Li", true, true);
    G4Material* Fe = nist->FindOrBuildMaterial("G4_Fe", true, true);
    G4Material* Cr = nist->FindOrBuildMaterial("G4_Cr", true, true);
    G4Material* Mo = nist->FindOrBuildMaterial("G4_Mo", true, true);
    G4Material* H = nist->FindOrBuildMaterial("G4_H", true, true);
    G4Material* Si = nist->FindOrBuildMaterial("G4_Si", true, true);
    G4Material* O = nist->FindOrBuildMaterial("G4_O", true, true);
    G4Material* Ca = nist->FindOrBuildMaterial("G4_Ca", true, true);
    G4Material* Al = nist->FindOrBuildMaterial("G4_Al", true, true);
    
    // Materials - Elemental materials defined by NIST Manager
    
    // Room air
    Air = new G4Material("Air", density= 1.204*kg/m3, nComponents= 1, kStateGas, T= 293.15*kelvin);
    Air->AddMaterial(nist->FindOrBuildMaterial("G4_AIR", true, true), massFraction= 100.*perCent);

    // Concrete (from geant4 example advanced/underground_physics/src/DMXDetectorMaterial.icc)
    concrete = new G4Material("Concrete", density=2.3*g/cm3, nComponents=6, kStateSolid, T= 293.15*kelvin);
    concrete->AddMaterial(Si, 0.227915);
    concrete->AddMaterial(O, 0.60541);
    concrete->AddMaterial(H, 0.09972);
    concrete->AddMaterial(Ca, 0.04986);
    concrete->AddMaterial(Al, 0.014245);
    concrete->AddMaterial(Fe, 0.00285);
    
    // Quartz, for PMTs
    Quartz = new G4Material
    ("Quartz", density=2.62*g/cm3, nComponents=2, kStateSolid, T= 293.15*kelvin);
    Quartz->AddMaterial(Si, 0.4674);
    Quartz->AddMaterial(O, 0.5326);
    
    // Type 444 Stainless Steel
    StSteel = new G4Material("Stainless Steel Type 444", density= 8.*g/cm3, nComponents= 3, kStateSolid, T= 293.15*kelvin);
    StSteel->AddMaterial(Fe, massFraction= 80.*perCent);
    StSteel->AddMaterial(Cr, massFraction= 18.*perCent);
    StSteel->AddMaterial(Mo, massFraction= 2.*perCent);
    
    G4Material* C = nist->FindOrBuildMaterial("G4_C", true);
    
    MinOil = new G4Material("Mineral Oil CH1.1", density= 0.877*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    MinOil->AddMaterial(C, massFraction= 91.53*perCent);
    MinOil->AddMaterial(H, massFraction= 8.47*perCent);
    
    PMMA = new G4Material("Plexiglass", density= 1.18*g/cm3, nComponents= 1, kStateSolid, T= 293.15*kelvin);
    PMMA->AddMaterial(nist->FindOrBuildMaterial("G4_PLEXIGLASS", true, true), massFraction= 100.*perCent);
    
    G4Material* Gd = nist->FindOrBuildMaterial("G4_Gd", true, true);
    
    static G4Material *Li6 = new G4Material("Lithium6",0.463*g/cm3,1);
    G4Isotope *isoLi6 = new G4Isotope("isoLi6", 3, 6, 6.015122*g/mole);
    G4Element *elLi6  = new G4Element("eleLi6", "Li6", 1);
    elLi6->AddIsotope(isoLi6,100.*perCent);
    Li6->AddElement(elLi6,1);
    
    // Scintillator Materials
    
    RawPsiCumene = new G4Material("Pseudocumene", density= 0.88*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    RawPsiCumene->AddMaterial(C, massFraction= 89.945*perCent);
    RawPsiCumene->AddMaterial(H, massFraction= 10.055*perCent);
    
    PsiCumeneL = new G4Material("PC-0.15wt%Li", density= 0.88*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    PsiCumeneL->AddMaterial(RawPsiCumene, massFraction= 99.85*perCent);
    PsiCumeneL->AddMaterial(Li6, massFraction= 0.15*perCent);
    
    PsiCumeneM = new G4Material("PC-0.30wt%Li", density= 0.88*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    PsiCumeneM->AddMaterial(RawPsiCumene, massFraction= 99.70*perCent);
    PsiCumeneM->AddMaterial(Li6, massFraction= 0.30*perCent);
    
    PsiCumeneT = new G4Material("PC-0.1wt%Gd", density= 0.88*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    PsiCumeneT->AddMaterial(RawPsiCumene, massFraction= 99.90*perCent);
    PsiCumeneT->AddMaterial(Gd, massFraction= 0.10*perCent);
    
    PsiCumeneH = new G4Material("PC-0.5wt%Gd", density= 0.88*g/cm3, nComponents= 2, kStateLiquid, T= 293.15*kelvin);
    PsiCumeneH->AddMaterial(RawPsiCumene, massFraction= 99.50*perCent);
    PsiCumeneH->AddMaterial(Gd, massFraction= 0.50*perCent);
    
    PsiCumeneG = new G4Material("PC-100%Gd", density= 0.88*g/cm3, nComponents= 1, kStateLiquid, T= 293.15*kelvin);
    PsiCumeneG->AddMaterial(Gd, massFraction= 100.0*perCent);
    
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
    
    BPoly = new G4Material("5wt% Borated Polyethylene", density= 0.92*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    BPoly->AddMaterial(Polyeth, massFraction = 95.0*perCent);
    BPoly->AddMaterial(B, massFraction = 5.0*perCent);
    
    LiPoly = new G4Material("5wt% Lithiated Polyethylene", density= 0.92*g/cm3, nComponents= 2, kStateSolid, T= 293.15*kelvin);
    LiPoly->AddMaterial(Polyeth, massFraction = 95.0*perCent);
    LiPoly->AddMaterial(Li, massFraction = 5.0*perCent);
    
    // Example of how to define a vacuum since pure vacuum does not exist in Geant4 library
    Vacuum = new G4Material("Vacuum", z= 2., a= 4.0026*g/mole, density= 1.e-25*g/cm3, kStateGas, T= 2.73*kelvin, P= 3.e-18*pascal);
    
    G4cout << G4endl << *(G4Material::GetMaterialTable()) << G4endl;	// Prints a table of all materials defined above
}

void DetectorConstruction::ConstructMaterialProperties() {
    
    G4cerr << "Setting material optical properties..." << G4endl;
    
    // Photon Energies (520 nm - 360 nm in 2 nm separation)
    const G4int nEntries = 81;
    G4double PhotonEnergy[nEntries]; 
    for(int i=0; i<nEntries; i++) {
        G4double l = (520-2*i)*nm;
        PhotonEnergy[i] = h_Planck * c_light / l;
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
    
    /*  G4double AbsLengthPC[nEntries] =
     *      { 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km, 10.*km,
     *      10.*km, 10.*km, 10.*km, 9.4*km, 8.7*km, 8.0*km, 7.3*km, 6.7*km, 6.2*km,
     *      5.7*km, 5.3*km, 4.9*km, 4.5*km, 4.2*km, 3.9*km, 3.7*km, 3.5*km, 3.3*km  };*/
    
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
    
    /*
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
     */
    /*
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
     */
    /*
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
     */
    
    // Generates new material properties table for scintillator
    G4MaterialPropertiesTable* mptCumene = new G4MaterialPropertiesTable();
    
    // Allows specification of properties that are constant with photon energy
    mptCumene->AddConstProperty("FASTTIMECONSTANT", 3.8*ns);		// Fast Component Decay Time
    //  mptCumene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);		// Slow Component Decay Time
    mptCumene->AddConstProperty("YIELDRATIO",1.0);				// 1 = all fast, 0 = all slow, fractional yield ratio allowed
    mptCumene->AddConstProperty("SCINTILLATIONYIELD",9000./MeV);		// Physical value for scintillation yield for Psuedocumene
    mptCumene->AddConstProperty("RESOLUTIONSCALE",1.0);			// Standard deviation on scintillation yield number
    
    //  mptCumene->AddConstProperty("SCINTILLATIONYIELD",1./MeV);		// Used for testing phase ONLY, not physically accurate
    
    // Allows specification of properties that vary with photon energy - linearly interpolated
    mptCumene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPC, nEntries);		// Light Yield for Fast Component
    //  mptCumene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPC, nEntries);		// Light Yield for Slow Component
    mptCumene->AddProperty("RINDEX", PhotonEnergy, RIndexPC, nEntries);			// Refractive Index of Material
    mptCumene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPC, nEntries);		// Abosrption Length of Material
    
    // Applies the optical property table to the material
    RawPsiCumene->SetMaterialPropertiesTable(mptCumene);
    RawPsiCumene->GetIonisation()->SetBirksConstant(birksPC);		// Set Birk's Constant for saturation quenching
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
    
    /*
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
     */
    /*
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
     */
    /*
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
     */
    /*
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
     */
    G4MaterialPropertiesTable* mptToluene = new G4MaterialPropertiesTable();	// Generates new material properties table for scintillator
    mptToluene->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);		// Fast Component Decay Time
    //  mptToluene->AddConstProperty("SLOWTIMECONSTANT",20.0*ns);		// Slow Component Decay Time
    mptToluene->AddConstProperty("YIELDRATIO",1.0);			// 1 = all fast, 0 = all slow, fractional yield ratio allowed
    mptToluene->AddConstProperty("SCINTILLATIONYIELD",10000./MeV);		// Physical value for scintillation yield for Polyvinyltoluene
    mptToluene->AddConstProperty("RESOLUTIONSCALE",1.0);			// Standard deviation on scintillation yield number
    
    //  mptToluene->AddConstProperty("SCINTILLATIONYIELD",1./MeV);		// Used for testing phase ONLY, not physically accurate
    
    mptToluene->AddProperty("FASTCOMPONENT", PhotonEnergy, ScintFastPVT, nEntries);		// Light Yield for Fast Component
    //  mptToluene->AddProperty("SLOWCOMPONENT", PhotonEnergy, ScintSlowPVT, nEntries);		// Light Yield for Slow Component
    mptToluene->AddProperty("RINDEX", PhotonEnergy, RIndexPVT, nEntries);			// Refractive Index of Material
    mptToluene->AddProperty("ABSLENGTH", PhotonEnergy, AbsLengthPVT, nEntries);		// Abosrption Length of Material
    
    MuonScint->SetMaterialPropertiesTable(mptToluene);
    MuonScint->GetIonisation()->SetBirksConstant(birksPVT);		// Sets the Birk's Constant for saturation quenching
    
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
    
    // Stainless Steel - not necessary

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

void DetectorConstruction::ConstructOpticalSurfaces() {
    
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
}

// ****** Optical Transport Switch ****** //
G4bool DetectorConstruction::SetOpticalProcesses(G4bool SetOptical) {
    if(fOptical == SetOptical) {
        G4cout << "Optical processes are already in the requested state." << SetOptical<< G4endl;
        return false;
    }
    fOptical = SetOptical;
    ConstructOpticalSurfaces();		// Re-establish optical surfaces between new volumes
    return true;
}

void DetectorConstruction::SetXSegments(G4int sp) {
    G4cerr << "Changing number of X segments to " <<sp<< G4endl;
    if(sp <= MaxSegX && sp*NSegY <= 2500) NSegX = sp;
    else G4cerr << "Invalid segment number, segments must be between 0 and " << MaxSegX << " and total segments much be less than 2500" << G4endl; 
}

void DetectorConstruction::SetYSegments(G4int sp) {
    G4cerr << "Changing number of Y segments to " << sp << G4endl;
    if(sp <= MaxSegY && sp*NSegX <= 2500) NSegY = sp;
    else G4cerr << "Invalid segment number, segments must be between 0 and " << MaxSegY << " and total segments much be less than 2500" << G4endl;
}

void DetectorConstruction::SetLeadShieldThickness(G4double sp) {
    if(sp >= 0) ShieldLead = sp;
    else G4cerr << "Invalid lead shield thickness, thickness must be greater than 0" << G4endl;
}

void DetectorConstruction::SetBPolyShieldThickness(G4double sp) {
    if(sp >= 0) ShieldPolyB = sp;
    else G4cerr << "Invalid borated poly shield thickness, thickness must be greater than 0" << G4endl;
}

void DetectorConstruction::SetLiPolyShieldThickness(G4double sp) {
    if(sp >= 0) ShieldPolyLi = sp;
    else G4cerr << "Invalid Lithiated poly shield thickness, thickness must be greater than 0" << G4endl;
}

void DetectorConstruction::SetSegmentLength(G4double sp) {
    if(sp > 0) {
        if(sp<=2.0*(SegBuffer+WrapThickness)){
            ScintLength = 0;
            SegBuffer = (sp-0.1)/2.0;
            WrapThickness = 0.1;
        } else {
            ScintLength = GetSegLength()-2*SegBuffer-2*WrapThickness;
        }
        G4cerr << "Segment Length set to " << G4BestUnit(GetSegLength(),"Length") << G4endl;
    } else G4cerr << "Invalid segment length, segment length must be greater than  0" << G4endl;
}

void DetectorConstruction::SetScintLength(G4double sp) {
    if(sp > 0) {
        ScintLength = sp;
        G4cerr << "Scint Length set to " << G4BestUnit(ScintLength,"Length") << G4endl;
    } else G4cerr << "Invalid scintillator length, scintillator length must be greater than 0" << G4endl;
}

void DetectorConstruction::SetSegmentWidth(G4double sp) {
    if(sp > 0) {
        if(sp <= 2.0*(AcrylThickness + WrapThickness + WrapGap)){
            ScintWidth = 0;
            if(sp <= 2.0*(WrapThickness + WrapGap)) AcrylThickness = 0.;
            else AcrylThickness = (sp-2.0*(WrapThickness+ WrapGap))/2.0;
        } else {
            ScintWidth = GetSegWidth()-2*(AcrylThickness+WrapThickness+WrapGap);
            if(PMTscale*2 > ScintWidth) PMTscale = ScintWidth/2.;
        }
        G4cerr << "Segment Width set to " << GetSegWidth() << G4endl;
    } else G4cerr << "Invalid segment width, segment width must be greater than  0" << G4endl;
}

void DetectorConstruction::SetScintWidth(G4double sp) {
    if(sp > 0) {
        ScintWidth = sp;
        if(PMTscale*2 > ScintWidth) PMTscale = ScintWidth/2.;
        G4cerr << "Scint Width set to " << G4BestUnit(ScintWidth,"Length") << G4endl;
    } else G4cerr << "Invalid scintillator width, scintillator width must be greater than 0" << G4endl;
}

void DetectorConstruction::SetSegmentHeight(G4double sp) {
    if(sp>0) {
        if(sp <= 2.0*(AcrylThickness + WrapThickness + WrapGap)){
            ScintHeight = 0;
            if(sp<=2.0*(WrapThickness + WrapGap)) AcrylThickness=0.;
            else AcrylThickness=(sp-2.0*(WrapThickness + WrapGap))/2.0;
        } else {
            ScintHeight = GetSegHeight()-2*(AcrylThickness + WrapThickness + WrapGap);
            if(PMTscale*2>ScintHeight) PMTscale = ScintHeight/2.;
        }
        G4cerr << "Segment Height set to " << G4BestUnit(GetSegHeight(),"Length") << G4endl;
    } else G4cerr << "Invalid segment height, segment height must be greater than  0" << G4endl;
}

void DetectorConstruction::SetScintHeight(G4double sp) {
    if(sp > 0) {
        ScintHeight = sp;
        if(PMTscale*2>ScintHeight) PMTscale = ScintHeight/2.;
        G4cerr << "Scint Height set to " << G4BestUnit(ScintHeight,"Length") << G4endl;
    } else G4cerr << "Invalid scintillator height, scintillator height must be greater than 0" << G4endl;
}

void DetectorConstruction::SetSegmentBuffer(G4double sp) {
    if(sp >= 0) {
        SegBuffer = sp;
        G4cerr << "Segment Buffer set to " << G4BestUnit(SegBuffer,"Length") << G4endl;
    } else G4cerr << "Invalid buffer length, buffer length must be greater than 0" << G4endl;
}

void DetectorConstruction::SetAirGap(G4double sp) {
    if(sp >= 0) {
        AirGap = sp;
        G4cerr << "Air Gap set to " << G4BestUnit(AirGap,"Length") << G4endl;
    } else G4cerr << "Invalid Air Gap, air gap must be greater than or equal to 0" << G4endl;
}

void DetectorConstruction::SetWrapGap(G4double sp) {
    if(sp >= 0) {
        WrapGap = sp;
        G4cerr << "Wrap Gap set to " << G4BestUnit(WrapGap,"Length") << G4endl;
    } else G4cerr << "Invalid Wrap Gap, wrap gap must be greater than or equal to 0" << G4endl;
}

void DetectorConstruction::SetWrapThickness(G4double sp) {
    if(sp >= 0) {
        WrapThickness = sp;
        G4cerr << "Wrap Thickness set to " << G4BestUnit(WrapThickness,"Length") << G4endl;
    } else G4cerr << "Invalid outer wrap thickness, outer wrap thickness must be greater than 0" << G4endl;
}

void DetectorConstruction::SetAcrylThickness(G4double sp) {
    if(sp >= 0) {
        AcrylThickness = sp;
        G4cerr << "Acrylic Thickness set to " << G4BestUnit(AcrylThickness,"Length") << G4endl;
    } else G4cerr << "Invalid Acrylic thickness, acrylic thickness must be greater than 0" << G4endl;
}

void DetectorConstruction::SetPMTDiameter(G4double sp) {
    if(sp > 0 && sp < ScintHeight && sp < ScintWidth) {
        PMTscale = sp/2.;
        G4cerr << "PMT Diameter set to " << G4BestUnit(PMTscale*2,"Length") << G4endl;
    } else G4cerr << "Invalid PMT diameter, PMT Diameter must be greater than 0 and les than the scintillator height and width" << G4endl;
}

// ****** Change Liquid Scintillator Birks Quenching Constant ****** //
void DetectorConstruction::SetScintillatorBirksConstant(G4double birks) {
    
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
}

void DetectorConstruction::SetScintillatorComposition(G4String identifier) {
    switch(identifier(0)) {
        case 'S': { G4cout << "Scintillator composition set to Raw Pseudocumene." << G4endl;    MainScintMat = &RawPsiCumene;   break; }
        case 'L': { G4cout << "Scintillator composition set to PC-0.15wt%Li." << G4endl;        MainScintMat = &PsiCumeneL;     break; }
        case 'T': { G4cout << "Scintillator composition set to PC-0.1wt%Gd." << G4endl;         MainScintMat = &PsiCumeneT;     break; }
        case 'H': { G4cout << "Scintillator composition set to PC-0.5wt%Gd." << G4endl;         MainScintMat = &PsiCumeneH;     break; }
        case 'M': { G4cout << "Scintillator composition set to PC-0.30wt%Li." << G4endl;        MainScintMat = &PsiCumeneM;     break; }
        case 'G': { G4cout << "Scintillator composition set to PC-100%Gd." << G4endl;           MainScintMat = &PsiCumeneG;     break; }
        case 'A': { G4cout << "Scintillator composition set to Raw Pseudocumene with 0.5wt% Gd in the reflector." << G4endl; MainScintMat = &RawPsiCumene; ScintSegMat = &GdPaintA; break; }
        case 'B': { G4cout << "Scintillator composition set to Raw Pseudocumene with 1.0wt% Gd in the reflector." << G4endl; MainScintMat = &RawPsiCumene; ScintSegMat = &GdPaintB; break; }
        case 'C': { G4cout << "Scintillator composition set to Raw Pseudocumene with 2.0wt% Gd in the reflector." << G4endl; MainScintMat = &RawPsiCumene; ScintSegMat = &GdPaintC; break; }
        default:  G4cout << "*** ATTENTION: Specified material identifier has not been programmed in DetectorConstruction.cc. Material selection not applied. ***" << G4endl;
    }
}

void DetectorConstruction::PrintPhysicalVolumes() const {
    G4cout << "Physical Volume Names:" << G4endl;
    std::vector<G4VPhysicalVolume*>* volumes = G4PhysicalVolumeStore::GetInstance();
    for(std::vector<G4VPhysicalVolume*>::iterator itr = volumes->begin(); itr != volumes->end(); itr++)
        G4cout << (*itr)->GetName() << G4endl;
}

G4double DetectorConstruction::GetMaxHalfDimension() {
    
    G4double shell_w = (GetSegWidth()+AirGap)*NSegX;
    G4double shell_h = (GetSegHeight()+AirGap)*NSegY;
    G4double pmtSEG_h = 100.*mm;
    G4double pmtSEGbase_h = 44.*mm;
    G4double basepinSEG_h = 20.*mm;
    G4double shell_l = GetSegLength()+ 2.0*(pmtSEG_h + (pmtSEGbase_h + basepinSEG_h));
    G4double dx,dy,dz;
    
    if(fShieldActivated){
        if(fVertical){
            dx = shell_w/2.+ ShieldLead + ShieldPolyB + ShieldPolyLi;
            dy = shell_h/2. + ShieldLead + ShieldPolyB + ShieldPolyLi;
            dz = shell_l/2.+ ShieldLead/2. + ShieldPolyB/2. + ShieldPolyLi/2.;
        } else {
            dx = shell_w/2.+ ShieldLead + ShieldPolyB + ShieldPolyLi;
            dy = shell_h/2. + ShieldLead/2. + ShieldPolyB/2. + ShieldPolyLi/2.;
            dz = shell_l/2.+ ShieldLead + ShieldPolyB  + ShieldPolyLi;
        }
    } else {
        dx = shell_w/2;
        dy = shell_h/2.;
        dz =  shell_l/2.;
    }
    
    G4double length = dx;
    if (dy>length) length = dy;
    if (dz>length) length = dz;
    return length;
}

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
}
                              