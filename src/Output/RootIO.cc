#include <sstream>
#include <cassert>

#include "RootIO.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"


static RootIO* instance = NULL;

RootIO::RootIO(): writecount(0), outfile(NULL), dataTree(NULL) {
    G4cerr << "RootIO initialized." << G4endl;
}

RootIO* RootIO::GetInstance() {
    if (instance == NULL) {
        G4cerr << "Instantiating ROOT output instance" << G4endl;
        instance = new RootIO();
        instance->GetEvent().Clear();
    }
    return instance;
}

void RootIO::WriteFile() {
    if(!outfile) {
        G4cerr << "No ROOT output file opened! Data not saved!" << G4endl;
        return;
    }
    
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    assert(run_action);
    if(run_action->GetRecordLevel() >= 0) {
        std::cout << "Writing events to file and closing..." << std::endl;
        if(dataTree) dataTree->Write();
        outfile->Write();
        outfile->Close();
        delete outfile;
        outfile = NULL;
    }
}

void RootIO::FillTree() {
    dataTree->Fill(); 
    writecount++;
    if(writecount==500) {
        dataTree->AutoSave("SaveSelf");
        writecount=0;
    }
}

void RootIO::SetFileName(G4String filename) {
    
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());   
   
    mcevent.Clear();
    
    if(run_action->GetRecordLevel()>=0){
        G4cerr << "RootIO output file is set to " << filename << G4endl;
        outfile = new TFile(filename,"RECREATE");
        
        // set up output TTree
        dataTree = new TTree("sblmc","Short Baseline MC data");
        dataTree->SetDirectory(outfile);
        pmcevent = &mcevent;
        dataTree->Branch("MCEvent",&pmcevent);
        
        // set up run info output
        mcrun.Clear();
        
        mcrun.geom.NSegX = detector->GetNSegX();
        mcrun.geom.NSegY = detector->GetNSegY();
        mcrun.geom.AirGap = detector->GetAirGap();
        mcrun.geom.WrapGap = detector->GetWrapGap();
        mcrun.geom.WrapThickness = detector->GetWrapThickness();		
        mcrun.geom.AcrylThickness = detector->GetAcrylThickness();       
        mcrun.geom.SegBuffer = detector->GetSegBuffer();	
        mcrun.geom.ScintLength = detector->GetScintLength();
        mcrun.geom.ScintHeight = detector->GetScintHeight();
        mcrun.geom.ScintWidth = detector->GetScintWidth();
        mcrun.geom.SegLength = detector->GetSegLength();	
        mcrun.geom.SegWidth = detector->GetSegWidth();	
        mcrun.geom.SegHeight = detector->GetSegHeight();	
        mcrun.geom.PMTscale = detector->GetPMTscale();                 
        mcrun.geom.ShieldLead = detector->GetShieldLead();
        mcrun.geom.ShieldPolyB = detector->GetShieldPolyB();
        mcrun.geom.ShieldPolyLi = detector->GetShieldPolyLi();
        mcrun.geom.Vertical = detector->GetVertical();
        mcrun.geom.Reflectivity = detector->GetReflectivity();
        mcrun.Write("RunInfo");
    }
}


