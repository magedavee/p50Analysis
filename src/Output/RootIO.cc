#include <sstream>
#include <cassert>

#include <TFile.h>
#include <TTree.h>

#include "RootIO.hh"
#include <G4ios.hh>


RootIO::RootIO(): writecount(0), outfile(NULL), dataTree(NULL) {
    dataTree = new TTree("PG4","PROSPECT Geant4 simulation");
    dataTree->SetDirectory(NULL);
    addPrimBranch();
    addEvtBranch();
    G4cout << "RootIO initialized. Remember to specify output filename." << G4endl;
}

void RootIO::WriteFile() {
    if(!outfile) {
        G4cout << "No ROOT output file opened! Data not saved!" << G4endl;
        return;
    } else outfile->cd();
    
    std::cout << "Writing events to file '" << fname << "' and closing..." << std::endl;
    if(dataTree) dataTree->Write();
    outfile->Write();
    outfile->Close();
    delete outfile;
    outfile = NULL;
}

void RootIO::SaveEvent() {
    if(!dataTree) return;
    dataTree->Fill(); 
    writecount++;
    if(writecount==500) {
        dataTree->AutoSave("SaveSelf");
        writecount=0;
    }
}

void RootIO::addEvtBranch() {
    if(pmcevent) return; // already set up
    G4cout << "RootIO Setting up 'Evt' output branch...\n";
    subObjs.push_back(pmcevent = &mcevent);
    dataTree->Branch("Evt",&pmcevent);
}

void RootIO::addPrimBranch() {
    if(pprimPtcls) return; // already set up
    G4cout << "RootIO Setting up 'Prim' output branch...\n";
    subObjs.push_back(pprimPtcls = &primPtcls);
    dataTree->Branch("Prim",&pprimPtcls);
}

void RootIO::addScIoniBranch() {
    if(pscintIoni) return; // already set up
    G4cout << "RootIO Setting up 'ScIoni' output branch...\n";
    subObjs.push_back(pscintIoni = &scintIoni);
    dataTree->Branch("ScIoni",&pscintIoni);
}

void RootIO::addNCaptBranch() {
    if(pscintNCapt) return; // already set up
    G4cout << "RootIO Setting up 'ScN' output branch...\n";
    subObjs.push_back(pscintNCapt = &scintNCapt);
    dataTree->Branch("ScN",&pscintNCapt);
}

void RootIO::addFluxBranch() {
    if(pfluxCounter) return; // already set up
    G4cout << "RootIO Setting up 'Flux' output branch...\n";
    subObjs.push_back(pfluxCounter = &fluxCounter);
    dataTree->Branch("Flux",&pfluxCounter);
}

void RootIO::addVetoIoniBranch() {
    if(pvetoIoni) return; // already set up
    G4cout << "RootIO Setting up 'VetoIoni' output branch...\n";
    subObjs.push_back(pvetoIoni = &vetoIoni);
    dataTree->Branch("VetoIoni",&pvetoIoni);
}

void RootIO::SetFileName(G4String filename) {
    fname = filename;
    G4cout << "RootIO: Setting output file to '" << fname << "'\n";
    outfile = new TFile(filename,"RECREATE");
    outfile->cd();
    dataTree->SetDirectory(outfile);
}
