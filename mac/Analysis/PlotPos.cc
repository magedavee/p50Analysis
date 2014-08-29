////////////////////////////////////////////
// ROOT macro example for MC output analysis
// export PG4_DIR=../PROSPECT-G4/; export LD_LIBRARY_PATH=./lib/:$LD_LIBRARY_PATH

#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <cassert>
#include <sys/stat.h>

#include "Event.hh"
#include "FileKeeper.hh"
#include "XMLInfo.hh"

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
    Event* evt = new Event();
    T->GetBranch("iEvts")->SetAutoDelete(kFALSE);
    T->SetBranchAddress("Evt",&evt);
    
    // set up histograms
    TH2F* hit_xy = (TH2F*)f.add(new TH2F("hit_xy", "Hit positions", 300,-1200,1200, 300,-1200,1200));
    TH2F* hit_zy = (TH2F*)f.add(new TH2F("hit_zy", "Hit positions", 300,-1200,1200, 300,-1200,1200));
    
    TH2F* ncapt_xy = (TH2F*)f.add(new TH2F("ncapt_xy", "Neutron capture positions", 100,-1.2,1.2, 100,-1.2,1.2));
    ncapt_xy->GetXaxis()->SetTitle("x position [m]");
    ncapt_xy->GetYaxis()->SetTitle("y position [m]");
    TH1F* ncapt_y = (TH1F*)f.add(new TH1F("ncapt_y", "Neutron capture positions", 200,-0.6,0.6));
    ncapt_y->GetXaxis()->SetTitle("y position [m]");
    
    TH2F* prim_p = (TH2F*)f.add(new TH2F("prim_p", "Primary momentum direction", 100,-1.2,1.2, 100,-1.2,1.2));
    
    TH1F* hnPrim = (TH1F*)f.add(new TH1F("hnPrim","Number of primary particles", 15, -0.5, 14.5));
    hnPrim->GetXaxis()->SetTitle("Number of primaries");
    hnPrim->GetYaxis()->SetTitle("Number of events");
    
    TH1F* hEIoni = (TH1F*)f.add(new TH1F("hEIoni","Ionizing energy deposition", 200, 0, 400));
    hEIoni->GetXaxis()->SetTitle("energy deposition [MeV]");
    hEIoni->GetYaxis()->SetTitle("Events per MeV");
    hEIoni->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hEIoniLo = (TH1F*)f.add(new TH1F("hEIoniLo","Ionizing energy deposition", 200, 0, 20));
    hEIoniLo->GetXaxis()->SetTitle("energy deposition [MeV]");
    hEIoniLo->GetYaxis()->SetTitle("Events per MeV");
    hEIoniLo->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hPrimN1 = (TH1F*)f.add(new TH1F("hPrimN1","Primary neutron spectrum", 500, 0, 5));
    hPrimN1->GetXaxis()->SetTitle("kinetic energy [GeV]");
    hPrimN1->GetYaxis()->SetTitle("Events per GeV");
    hPrimN1->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hPrimN2 = (TH1F*)f.add(new TH1F("hPrimN2","Primary neutron spectrum", 200, 0, 100));
    hPrimN2->GetXaxis()->SetTitle("kinetic energy [GeV]");
    hPrimN2->GetYaxis()->SetTitle("Events per GeV");
   
    TH1F* hPrimN3 = (TH1F*)f.add(new TH1F("hPrimN3","Primary neutron spectrum", 200, 0, 10));
    hPrimN3->GetXaxis()->SetTitle("kinetic energy [MeV]");
    hPrimN3->GetYaxis()->SetTitle("Events per MeV");
    hPrimN3->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hPrimN4 = (TH1F*)f.add(new TH1F("hPrimN4","Primary neutron spectrum", 200, 0, 1));
    hPrimN4->GetXaxis()->SetTitle("kinetic energy [keV]");
    hPrimN4->GetYaxis()->SetTitle("Events per keV");
    //hPrimN4.GetYaxis()->SetTitleOffset(1.45);
    
    TH2F* hTimeCorr = (TH2F*)f.add(new TH2F("hTimeCorr","Neutron-capture-correlated energy deposition",
                                     250, -20, 5, 200, 0, 25));
    hTimeCorr->GetYaxis()->SetTitle("ionizing deposition [MeV]");
    hTimeCorr->GetXaxis()->SetTitle("time from neutron capture [#mus]");
    
    TH2F* hTimeCorr2 = (TH2F*)f.add(new TH2F("hTimeCorr2","Neutron-capture-correlated energy deposition",
                                      480, -100, 20, 200, 0, 25));
    hTimeCorr2->GetYaxis()->SetTitle("ionizing deposition [MeV]");
    hTimeCorr2->GetXaxis()->SetTitle("time from neutron capture [ns]");
    
    gStyle->SetOptStat("");
    
    // event counters by PID
    map<Int_t, Int_t> primNCapts;
    map<Int_t, Int_t> primIoni;
    map<Int_t, Int_t> nCaptZA;

    // counter for "IBD-like" neutron captures, as a function of number of primaries
    TH1F* hFakeIBD = (TH1F*)f.add(new TH1F("hFakeIBD","Number of IBD-like events", 15, -0.5, 14.5));
    hFakeIBD->GetXaxis()->SetTitle("Number of primaries");
    hFakeIBD->GetYaxis()->SetTitle("Number of IBD-like events");
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T->GetEntry(ev);
        
        // primaries
        Int_t nPrim = evt->Primaries->GetEntriesFast();
        Int_t primTp = 0;
        hnPrim->Fill(nPrim);
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)evt->Primaries->At(i);
            prim_p->Fill(pp->p[0], pp->p[1]);
            
            primTp = pp->PID;
            if(primTp == 2112) {
                hPrimN1->Fill(pp->E/1000.);
                hPrimN2->Fill(pp->E/1000.);
                hPrimN3->Fill(pp->E);
                hPrimN4->Fill(pp->E*1e3);
            }
        }
        
        // neutron captures
        vector<double> capt_times;
        Int_t nNCapts = evt->nCapts->GetEntriesFast();
        Int_t nNCaptInVol = 0;
        for(Int_t i=0; i<nNCapts; i++) {
            EventNCapt* nc = (EventNCapt*)evt->nCapts->At(i);
            nCaptZA[10000 * nc->capt_Z + nc->capt_A] += 1;
            capt_times.push_back(nc->t);
            nNCaptInVol += nc->vol >= 0;
            //ncapt_xy->Fill(nc->x[0]/1000.,nc->x[1]/1000.);
            //ncapt_y->Fill(nc->x[1]/1000.);
        }
        
        // ionization
        map<Int_t, double> volIoni; // Ionization accumulator by volume
        Int_t nIoni = evt->iEvts->GetEntriesFast();
        if(evt->EIoni) {
            hEIoni->Fill(evt->EIoni);
            hEIoniLo->Fill(evt->EIoni);
        }
        for(Int_t i=0; i<nIoni; i++) {
            EventIoniCluster* ei = (EventIoniCluster*)evt->iEvts->At(i);
            if(ei->vol >= 0) {
                hit_xy->Fill(ei->x[0], ei->x[1]);
                hit_zy->Fill(ei->x[2], ei->x[1]);
                volIoni[ei->vol] += ei->E;
            }
            
            for(vector<double>::const_iterator it = capt_times.begin(); it != capt_times.end(); it++) {
                hTimeCorr->Fill((ei->t - *it)/1000., ei->E);
                hTimeCorr2->Fill((ei->t - *it), ei->E);
            }
        }
        
        if(nPrim == 1) {
            primNCapts[primTp] += (evt->nNCapts > 0);
            primIoni[primTp] += (nIoni > 0);
        }
        
        if(evt->nNCapts > 0 && nNCaptInVol > 0) {
            int nThresh = 0;
            int nHi = 0;
            for(map<Int_t,double>::iterator it = volIoni.begin(); it != volIoni.end(); it++) {
                nThresh += it->second > 0.02;
                nHi += it->second > 20;
            }
            bool isFakeIBD = (nThresh == 1 && nHi == 0);
            if(isFakeIBD) {
                hFakeIBD->Fill(nPrim);
                for(Int_t i=0; i<nNCapts; i++) {
                    EventNCapt* nc = (EventNCapt*)evt->nCapts->At(i);
                    ncapt_xy->Fill(nc->x[0]/1000.,nc->x[1]/1000.);
                    ncapt_y->Fill(nc->x[1]/1000.);
                }
            }
        }
    }
    
    std::cout << "\nNeutron captures by primary:\n";
    display_map<Int_t,Int_t>(primNCapts);
    std::cout << "\nIonization by primary:\n";
    display_map<Int_t,Int_t>(primIoni);
    std::cout << "\nNeutron capture nucleus 10000*Z + A:\n";
    display_map<Int_t,Int_t>(nCaptZA);
    
    std::cout << "IBD-like: \n";
    for(int i=1; i<hFakeIBD->GetNbinsX(); i++) {
        std::cout << "\t" << hFakeIBD->GetXaxis()->GetBinCenter(i) << ":\t";
        float nibd = hFakeIBD->GetBinContent(i);
        float nnp = hnPrim->GetBinContent(i);
        std::cout << nibd << " IBD-like / " << nnp << " = " << nibd/nnp << "\n";
    }
    
    hEIoni->Scale(1./hEIoni->GetBinWidth(1));
    hEIoni->SetMaximum(20000);
    hEIoni->Draw();
    gPad->Print((outpath+"/EIoni.pdf").c_str());
    
    hEIoniLo->Scale(1./hEIoniLo->GetBinWidth(1));
    //hEIoni->SetMaximum(20000);
    hEIoniLo->Draw();
    gPad->Print((outpath+"/EIoniLo.pdf").c_str());
    
    hPrimN3->Scale(1./hPrimN3->GetBinWidth(1));
    hPrimN3->Draw();
    gPad->Print((outpath+"/PrimN_lo.pdf").c_str());
    
    hPrimN1->Scale(1./hPrimN1->GetBinWidth(1));
    hPrimN1->Draw();
    gPad->Print((outpath+"/PrimN_mid.pdf").c_str());
    
    gPad->SetLogy(true);
    
    hPrimN4->Scale(1./hPrimN4->GetBinWidth(1));
    hPrimN4->Draw();
    gPad->Print((outpath+"/PrimN_thermal.pdf").c_str());
    
    hnPrim->Draw();
    gPad->Print((outpath+"/nPrim.pdf").c_str());
    std::cout << "\nSingle-primary events: " << hnPrim->GetBinContent(2) << " out of " << hnPrim->Integral() << "\n";
    
    hPrimN2->Scale(1./hPrimN2->GetBinWidth(1));
    hPrimN2->Draw();
    gPad->Print((outpath+"/PrimN_hi.pdf").c_str());
    
    gPad->SetLogy(false);
    
    hTimeCorr->SetMaximum(30);
    hTimeCorr->Draw("Col Z");
    gPad->Print((outpath+"/TimeCorr.pdf").c_str());
    
    hTimeCorr2->SetMaximum(100);
    hTimeCorr2->Draw("Col Z");
    gPad->Print((outpath+"/TimeCorr2.pdf").c_str());
    
    gPad->SetCanvasSize(700,700);
    hit_xy->Draw("Col Z");
    gPad->Print((outpath+"/Hit_xy.pdf").c_str());
    hit_zy->Draw("Col Z");
    gPad->Print((outpath+"/Hit_zy.pdf").c_str());
    prim_p->Draw("Col Z");
    gPad->Print((outpath+"/Hit_P0.pdf").c_str());
    
    ncapt_xy->Draw("Col Z");
    gPad->Print((outpath+"/nCapt_xy.pdf").c_str());
    ncapt_y->Draw();
    gPad->Print((outpath+"/nCapt_y.pdf").c_str());
    
    delete(T);
    
    return 0;
}
