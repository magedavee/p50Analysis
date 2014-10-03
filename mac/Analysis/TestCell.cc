#include "Event.hh"
#include "FileKeeper.hh"
#include "XMLInfo.hh"
#include "strutils.hh"

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
    
    gSystem->Load("libEventLib.so");
    gStyle->SetOptStat("");
    
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
    NCaptEvent* snc = new NCaptEvent();
    T->SetBranchAddress("ScN",&snc);
    
    // set up histograms
    TH1F* hCaptTime = (TH1F*)f.add(new TH1F("hCaptTime","neutron capture time", 100, 0, 200));
    hCaptTime->GetXaxis()->SetTitle("capture time [#mus]");
    hCaptTime->GetYaxis()->SetTitle("Events per #mus");
    //hCaptTime->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hRecoils = (TH1F*)f.add(new TH1F("hRecoils","nuclear recoils", 21, -0.5, 20.5));
    hRecoils->GetXaxis()->SetTitle("number of recoils");
    hRecoils->GetYaxis()->SetTitle("Events");
    
    TH2F* hCaptPos = (TH2F*)f.add(new TH2F("hCaptPos","neutron capture positions",100,-3,3,100,-3,3));
    hCaptPos->GetXaxis()->SetTitle("x position [in]");
    hCaptPos->GetYaxis()->SetTitle("y position [in]");
    
    map<Int_t, Int_t> nCaptZA;
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        snc->Clear();
        sion->Clear();
        T->GetEntry(ev);
    
        Int_t nNCapts = snc->nCapts->GetEntriesFast();
        Int_t nIoni = sion->clusts->GetEntriesFast();
        
        if(!(nNCapts && nIoni)) continue;
        
        // find neutron capture time
        double t_ncapt = 0;
        for(Int_t i=0; i<nNCapts; i++) {
            NCapt* nc = (NCapt*)snc->nCapts->At(i);
            nCaptZA[10000 * nc->capt_Z + nc->capt_A] += 1;
            t_ncapt = nc->t;
            hCaptPos->Fill(nc->x[0]/25.4,nc->x[1]/25.4);
        }
        
        // find first ionization time
        double t_ioni = 0;
        int n_recoil = 0;
        for(Int_t i=0; i<nIoni; i++) {
            IoniCluster* ei = (IoniCluster*)sion->clusts->At(i);
            if(!t_ioni && ei->E > 0.1) t_ioni = ei->t;
            n_recoil += (ei->PID >= 2212 && ei->PID != 1000020040  && ei->PID != 1000010030);
        }
        if(t_ioni) {
            hCaptTime->Fill((t_ncapt-t_ioni)/1000.);
            hRecoils->Fill(n_recoil);
        }
    }

    std::cout << "\nNeutron capture nucleus 10000*Z + A:\n";
    display_map<Int_t,Int_t>(nCaptZA);
    
    hCaptTime->Sumw2();
    hCaptTime->Scale(1.0/hCaptTime->GetBinWidth(0));
    //TF1 expFit("expFit","expo",14.,100.);
    TF1 expFit("expFit","[0]*exp(-x*[1])+[2]*exp(-x*[3])", 2., 200.);
    expFit.SetParameter(0,0.9*hCaptTime->GetBinContent(1));
    expFit.SetParameter(2,0.1*hCaptTime->GetBinContent(1));
    expFit.SetParameter(1,1/35.);
    expFit.SetParameter(3,1/7.);
    hCaptTime->Fit(&expFit,"R");
    
    double ctime = expFit.GetParameter(1);
    ctime = 1./ctime;
    double dctime = expFit.GetParError(1);
    dctime *= ctime*ctime;
    
    double ctime2 = expFit.GetParameter(3);
    ctime2 = 1./ctime2;
    double dctime2 = expFit.GetParError(3);
    dctime2 *= ctime2*ctime2;
    
    std::cout << "Capture time = " << ctime << " +- " << dctime << " and "  << ctime2 << " +- " << dctime2 << "\n";
    
    hCaptTime->Draw();
    gPad->SetLogy(true);
    char label[1024];
    sprintf(label,"Capture time: %.1f #pm %.1f #mus",ctime,dctime);
    TLatex L(10, expFit.Eval(150), label);
    L.Draw();
    gPad->Print((outpath+"/CaptTime.pdf").c_str());
    
    hRecoils->Draw();
    gPad->Print((outpath+"/Recoils.pdf").c_str());
    
    gPad->SetLogy(false);
    gPad->SetCanvasSize(500,500);
    hCaptPos->Draw("Col Z");
    gPad->Print((outpath+"/CaptPos.pdf").c_str());
    
    delete T;
    return 0;
}
