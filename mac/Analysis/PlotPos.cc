////////////////////////////////////////////
// ROOT macro example for MC output analysis

#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <cassert>
#include <sys/stat.h>

#include "AnaUtils.hh"

using std::vector;
using std::map;

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("~/Applications/PG4/lib/libEventLib.so");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";

    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"Out.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
    NCaptEvent* scn = new NCaptEvent();
    T->SetBranchAddress("ScN",&scn);
    
    // set up histograms
    ProfileHistos hPos(400,1.5,"hPos","event positions","[m]");
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); }
        
        sion->Clear();
        T->GetEntry(ev);

        /*
        // scintillator hits
        Int_t nScint = sion->clusts->GetEntriesFast();
        for(Int_t i=0; i<nScint; i++) {
            IoniCluster* ei = (IoniCluster*)sion->clusts->At(i);
            if(ei->vol < 0) continue;
            hPos.Fill(ei->x[0]/1000., ei->x[1]/1000., ei->x[2]/1000.);
        }
        */
        
        // neutron captures
        Int_t nNCapt = scn->nCapts->GetEntriesFast();
        for(Int_t i=0; i<nNCapt; i++) {
            NCapt* nc = (NCapt*)scn->nCapts->At(i);
            if(nc->vol<0 || nc->vol == 4) continue;
            hPos.Fill(nc->x[0]/1000., nc->x[1]/1000., nc->x[2]/1000.);
        }
    }
    
    hPos.h_xy->Draw();
    gPad->SetCanvasSize(700,700);
    
    hPos.ScaleBinsize();
    hPos.Scale(1./D.genTime);
    hPos.Print("Col",outpath+"/IoniPos");
    
    return 0;
}
