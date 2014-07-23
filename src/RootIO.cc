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

RootIO::RootIO(): writecount(0), outfile(NULL), dataTree(NULL), Nevents(0) {
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
    if(run_action->GetRecordLevel()>=0) {
        Nevents++;
        
        std::ostringstream os;
        os << Nevents;
        std::string stevt = "Event_" + os.str(); 
        
        std::cout << "writing " << stevt << std::endl;
        if(dataTree) dataTree->Write();
        std::cout << "closing " <<std::endl;
        outfile->Write();
        outfile->Close();
        std::cout << "closed " <<std::endl;
        delete outfile;
        std::cout << "deleted " <<std::endl;
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

void RootIO::SetFileName(G4String fname) {
    
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());   
   
    mcevent.Clear();
    
    if(run_action->GetRecordLevel()>=0){
        filename = fname;
        G4cerr << "RootIO output file is set to " << filename << G4endl;
        outfile = new TFile(filename,"RECREATE");
        
        // set up output TTree
        dataTree = new TTree("sblmc","Short Baseline MC data");
        dataTree->SetDirectory(outfile);
        pmcevent = &mcevent;
        dataTree->Branch("MCEvent",&pmcevent);
        
        // set up run info output
        mcrun = new Run();
        mcrun->Clear();
        mcrun->NSegX = detector->GetNSegX();
        mcrun->NSegY = detector->GetNSegY();
        mcrun->AirGap = detector->GetAirGap();
        mcrun->WrapGap = detector->GetWrapGap();
        mcrun->WrapThickness = detector->GetWrapThickness();		
        mcrun->AcrylThickness = detector->GetAcrylThickness();       
        mcrun->SegBuffer = detector->GetSegBuffer();	
        mcrun->ScintLength = detector->GetScintLength();
        mcrun->ScintHeight = detector->GetScintHeight();
        mcrun->ScintWidth = detector->GetScintWidth();
        mcrun->SegLength = detector->GetSegLength();	
        mcrun->SegWidth = detector->GetSegWidth();	
        mcrun->SegHeight = detector->GetSegHeight();	
        mcrun->PMTscale = detector->GetPMTscale();                 
        mcrun->ShieldLead = detector->GetShieldLead();
        mcrun->ShieldPolyB = detector->GetShieldPolyB();
        mcrun->ShieldPolyLi = detector->GetShieldPolyLi();
        mcrun->Vertical = detector->GetVertical();
        // mcrun->Scint = detector->GetScint();
        mcrun->Shielded = detector->GetShieldInclusion();
        mcrun->Reflectivity = detector->GetReflectivity();
        mcrun->Write("RunInfo");
    }
}


