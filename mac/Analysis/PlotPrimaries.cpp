///////////////////////////////////////////////////////////
// Standalone program for plotting MC primaries information
// export PG4_DIR=../PROSPECT-G4/; export LD_LIBRARY_PATH=./lib/:$LD_LIBRARY_PATH
// g++ `root-config --cflags --libs` -L./lib/ -lEventLib -I${PG4_DIR}/include/Output/ -I${PG4_DIR}/mac/Analysis/ ${PG4_DIR}/mac/Analysis/PlotPrimaries.cpp -o PlotPrimaries

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <sstream>

#include "Event.hh"
#include "FileKeeper.hh"

#include <TCanvas.h>
#include <TSystem.h>
#include <TClonesArray.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TChain.h>

using std::vector;
using std::map;
using std::string;

/// utility function for converting to string
template<typename T>
string to_str(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

/// Collection of histograms to generate for a primary particle type
class PrimaryHistograms {
public:
    /// Constructor
    PrimaryHistograms(FileKeeper& F, int pid): PID(pid) {
        string pn = to_str(PID);
        
        p_dir = (TH2F*)F.add(new TH2F(("prim_p_"+pn).c_str(), "Momentum direction", 100,-1.2,1.2, 100,-1.2,1.2));
        
        E_hi = (TH1F*)F.add(new TH1F(("E_hi_"+pn).c_str(),"Energy spectrum", 500, 0, 5));
        E_hi->GetXaxis()->SetTitle("kinetic energy [GeV]");
        E_hi->GetYaxis()->SetTitle("Events per GeV");
        E_hi->GetYaxis()->SetTitleOffset(1.45);
    }
    
    /// Fill from primaries data
    void Fill(EventPrimaryPtcl* pp) {
        p_dir->Fill(pp->p[0], pp->p[1]);
        E_hi->Fill(pp->E/1000.);
    }
    
    /// Process and draw
    void Draw(const string& outpath) {
        
        string pn = to_str(PID);
        
        gStyle->SetOptStat("");
        
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
    TChain T("sblmc");
    T.SetDirectory(NULL);
    T.Add((inPath+"/*.root").c_str());
    // set readout branches
    Event* evt = new Event();
    T.GetBranch("iEvts")->SetAutoDelete(kFALSE);
    T.SetBranchAddress("MCEvent",&evt);
    
    map<Int_t, PrimaryHistograms> primHists;
    
    // scan events
    Long64_t nentries = T.GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T.GetEntry(ev);
        Int_t nPrim = evt->Primaries->GetEntriesFast();
        for(Int_t i=0; i<nPrim; i++) {
            EventPrimaryPtcl* pp = (EventPrimaryPtcl*)evt->Primaries->At(i);
            
            Int_t PID = pp->PID;
            map<Int_t, PrimaryHistograms>::iterator it = primHists.find(PID);
            if(it == primHists.end())
                it = primHists.insert(std::pair<Int_t, PrimaryHistograms>(PID,PrimaryHistograms(f,PID))).first;
            it->second.Fill(pp);
        }
    }
    
    // produce output
    for(map<Int_t, PrimaryHistograms>::iterator it = primHists.begin(); it !=primHists.end(); it++) {
        it->second.Draw(outpath);
    }
        
    return 0;
}