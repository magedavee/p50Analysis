///////////////////////////////////////////////////////////
// Standalone program for plotting MC primaries information

#include <map>
#include <utility>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <stdlib.h>
#include <cmath>
#include <sstream>

#include "Event.hh"
#include "XMLInfo.hh"
#include "FileKeeper.hh"
#include "strutils.hh"

#include <TCanvas.h>
#include <TSystem.h>
#include <TClonesArray.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

using std::map;

/// Collection of histograms to generate for a primary particle type
class PrimaryHistograms {
public:
    /// Constructor
    PrimaryHistograms(FileKeeper& F, int pid): PID(pid) {
        string pn = to_str(PID);
        
        p_dir = (TH2F*)F.add(new TH2F(("prim_p_"+pn).c_str(), "Momentum direction", 100,-1.2,1.2, 100,-1.2,1.2));
        
        E_hi = (TH1F*)F.add(new TH1F(("E_hi_"+pn).c_str(),"Energy spectrum", 500, 0, 5));
        E_hi->GetXaxis()->SetTitle("kinetic energy [GeV]");
        E_hi->GetYaxis()->SetTitle("rate [Hz/GeV]");
        E_hi->GetYaxis()->SetTitleOffset(1.45);
    }
    
    /// Fill from primaries data
    void Fill(ParticleVertex* pp) {
        p_dir->Fill(pp->p[0], pp->p[1]);
        E_hi->Fill(pp->E/1000.);
    }
    
    /// Process and draw
    void Draw(const string& outpath, double t) {
        
        string pn = to_str(PID);
        
        gStyle->SetOptStat("");
        
        E_hi->Scale(1./E_hi->GetBinWidth(1)/t);
        E_hi->Draw();
        gPad->SetLogy(true);
        gPad->Print((outpath+"/E_hi_"+pn+".pdf").c_str());
        
        gPad->SetLogy(false);
        gPad->SetCanvasSize(700,700);
        
        p_dir->Draw("Col Z");
        gPad->Print((outpath+"/p_Direction_"+pn+".pdf").c_str());

    }
    
    int PID;            ///< PDG particle ID
    TH2F* p_dir;        ///< momentum direction
    TH1F* E_hi;         ///< Energy, high-energy range
};

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"PrimariesOut.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    Event* evt = new Event();
    T->GetBranch("clusts")->SetAutoDelete(kFALSE);
    T->SetBranchAddress("Evt",&evt);
    
    map<Int_t, PrimaryHistograms> primHists;
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T->GetEntry(ev);
        Int_t nPrim = evt->Primaries->GetEntriesFast();
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)evt->Primaries->At(i);
            
            Int_t PID = pp->PID;
            map<Int_t, PrimaryHistograms>::iterator it = primHists.find(PID);
            if(it == primHists.end())
                it = primHists.insert(std::pair<Int_t, PrimaryHistograms>(PID,PrimaryHistograms(f,PID))).first;
            it->second.Fill(pp);
        }
    }
    
    // produce output
    double totalTime = D.getTotalGenTime();
    for(map<Int_t, PrimaryHistograms>::iterator it = primHists.begin(); it !=primHists.end(); it++) {
        it->second.Draw(outpath, totalTime);
    }

    return 0;
}