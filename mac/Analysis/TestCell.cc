#include "Event.hh"
#include "FileKeeper.hh"
#include "XMLInfo.hh"

#include <sys/stat.h>
#include <stdio.h>

#include <TCanvas.h>
#include <TSystem.h>
#include <TClonesArray.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

using std::vector;
using std::map;

int main(int argc, char** argv) {
    
    gSystem->Load("~/Applications/PG4/lib/libEventLib.so");
    gStyle->SetOptStat("");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"Out.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    Event* evt = new Event();
    T->GetBranch("iEvts")->SetAutoDelete(kFALSE);
    T->SetBranchAddress("Evt",&evt);
    
    // set up histograms
    TH1F* hCaptTime = (TH1F*)f.add(new TH1F("hCaptTime","neutron capture time", 120, 0, 120));
    hCaptTime->GetXaxis()->SetTitle("capture time [#mus]");
    hCaptTime->GetYaxis()->SetTitle("Events per #mus");
    //hCaptTime->GetYaxis()->SetTitleOffset(1.45);
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T->GetEntry(ev);
    
        Int_t nNCapts = evt->nCapts->GetEntriesFast();
        Int_t nIoni = evt->iEvts->GetEntriesFast();
        
        if(!(nNCapts && nIoni)) continue;
        
        // find neutron capture time
        double t_ncapt = 0;
        for(Int_t i=0; i<nNCapts; i++) {
            EventNCapt* nc = (EventNCapt*)evt->nCapts->At(i);
            //nCaptZA[10000 * nc->capt_Z + nc->capt_A] += 1;
            t_ncapt = nc->t;
        }
        
        // find first ionization time
        double t_ioni = 0;
        for(Int_t i=0; i<nIoni; i++) {
            EventIoniCluster* ei = (EventIoniCluster*)evt->iEvts->At(i);
            if(!t_ioni && ei->E > 0.2) t_ioni = ei->t;
        }
        if(t_ioni) hCaptTime->Fill((t_ncapt-t_ioni)/1000.);
    }
    
    
    hCaptTime->Sumw2();
    hCaptTime->Scale(1.0/hCaptTime->GetBinWidth(0));
    TF1 expFit("expFit","expo",14.,100.);
    hCaptTime->Fit(&expFit,"R");
    double ctime = expFit.GetParameter(1);
    ctime = -1./ctime;
    double dctime = expFit.GetParError(1);
    dctime *= ctime*ctime;
    std::cout << "Capture time = " << ctime << " +- " << dctime << "\n";
    
    hCaptTime->Draw();
    gPad->SetLogy(true);
    char label[1024];
    sprintf(label,"Capture time: %.1f #pm %.1f #mus",ctime,dctime);
    TLatex L(5, 2, label);
    L.Draw();
    gPad->Print((outpath+"/CaptTime.pdf").c_str());
    
    delete T;
    return 0;
}
