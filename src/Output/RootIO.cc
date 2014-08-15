#include <sstream>
#include <cassert>

#include <TFile.h>
#include <TTree.h>

#include "RootIO.hh"
#include "RunAction.hh"

#include <G4RunManager.hh>
#include <G4ios.hh>


static RootIO* instance = NULL;

RootIO::RootIO(): writecount(0), outfile(NULL), dataTree(NULL) {
    pmcevent = &mcevent;
    pfluxCounter = &fluxCounter;
    G4cerr << "RootIO initialized." << G4endl;
}

RootIO* RootIO::GetInstance() {
    if (instance == NULL) {
        G4cerr << "Instantiating ROOT output instance" << G4endl;
        instance = new RootIO();
        GetEvent().Clear();
        GetFlux().Clear();
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
    if(!dataTree) return;
    dataTree->Fill(); 
    writecount++;
    if(writecount==500) {
        dataTree->AutoSave("SaveSelf");
        writecount=0;
    }
}

void RootIO::addFluxBranch() {
    if(!dataTree) {
        std::cout << "*** Please set output file first!\n";
        return;
    }
    std::cout << "RootIO Setting up 'Flux' output branch...\n";
    assert(dataTree);
    dataTree->Branch("Flux",&pfluxCounter);
}

void RootIO::SetFileName(G4String filename) {
    
    fname = filename;
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
   
    mcevent.Clear();
    fluxCounter.Clear();
    
    if(run_action->GetRecordLevel() >= 0){
        G4cerr << "RootIO output file is set to " << filename << G4endl;
        outfile = new TFile(filename,"RECREATE");
        
        // set up output TTree
        dataTree = new TTree("sblmc","Short Baseline MC data");
        dataTree->SetDirectory(outfile);
        dataTree->Branch("MCEvent",&pmcevent);
    }
}


