////////////////////////////////////////////
// ROOT macro example for MC output analysis
// .x PlotPos.cpp

#include <map>
#include <utility>
#include <iostream>
#include <string>

#include <TCanvas.h>

template<typename T, typename U>
void display_map(const std::map<T,U>& m) {
    U total = 0;
    for(std::map<T,U>::const_iterator it = m.begin(); it != m.end(); it++) {
        std:cout << it->first << ":\t" << it->second << "\n";
        total += it->second;       
    }
    std::cout << "Total:\t" << total << "\n";
}

void PlotPos() {
    // load library describing data classes
    gSystem->Load("~/Applications/PG4/lib/libEventLib.so");
    
    std::string outpath = "/home/mpmendenhall/tmp/Pb6cm/";
    TFile* f = new TFile((outpath+"Out.root").c_str(), "RECREATE");
    
    // load data into TChain
    TChain T("sblmc");
    T.Add("*.root");
    // set readout branches
    Event* evt = new Event();
    T.GetBranch("iEvts")->SetAutoDelete(kFALSE);
    T.SetBranchAddress("MCEvent",&evt);
    
    // set up histograms
    TH2F hit_xy("hit_xy", "Hit positions", 300,-1200,1200, 300,-1200,1200);
    TH2F hit_zy("hit_zy", "Hit positions", 300,-1200,1200, 300,-1200,1200);
    TH2F prim_p("prim_p", "Primary momentum direction", 100,-1.2,1.2, 100,-1.2,1.2);
    
    TH1F hnPrim("hnPrim","Number of primary particles", 15, 0, 15);
    hnPrim.GetXaxis()->SetTitle("Number of primaries");
    hnPrim.GetYaxis()->SetTitle("Number of events");
    
    TH1F hEIoni("hEIoni","Ionizing energy deposition", 200, 0, 400);
    hEIoni.GetXaxis()->SetTitle("energy deposition [MeV]");
    hEIoni.GetYaxis()->SetTitle("Events per MeV");
    hEIoni.GetYaxis()->SetTitleOffset(1.45);
    
    TH1F hEIoniLo("hEIoniLo","Ionizing energy deposition", 200, 0, 20);
    hEIoniLo.GetXaxis()->SetTitle("energy deposition [MeV]");
    hEIoniLo.GetYaxis()->SetTitle("Events per MeV");
    hEIoniLo.GetYaxis()->SetTitleOffset(1.45);
    
    TH1F hPrimN1("hPrimN1","Primary neutron spectrum", 500, 0, 5);
    hPrimN1.GetXaxis()->SetTitle("kinetic energy [GeV]");
    hPrimN1.GetYaxis()->SetTitle("Events per GeV");
    hPrimN1.GetYaxis()->SetTitleOffset(1.45);
    
    TH1F hPrimN2("hPrimN2","Primary neutron spectrum", 200, 0, 100);
    hPrimN2.GetXaxis()->SetTitle("kinetic energy [GeV]");
    hPrimN2.GetYaxis()->SetTitle("Events per GeV");
   
    TH1F hPrimN3("hPrimN3","Primary neutron spectrum", 200, 0, 10);
    hPrimN3.GetXaxis()->SetTitle("kinetic energy [MeV]");
    hPrimN3.GetYaxis()->SetTitle("Events per MeV");
    hPrimN3.GetYaxis()->SetTitleOffset(1.45);
    
    TH1F hPrimN4("hPrimN4","Primary neutron spectrum", 200, 0, 1);
    hPrimN4.GetXaxis()->SetTitle("kinetic energy [keV]");
    hPrimN4.GetYaxis()->SetTitle("Events per keV");
    //hPrimN4.GetYaxis()->SetTitleOffset(1.45);
    
    TH2F hTimeCorr("hTimeCorr","Neutron-capture-correlated energy deposition",
                   250, -20, 5, 200, 0, 25);
    hTimeCorr.GetYaxis()->SetTitle("ionizing deposition [MeV]");
    hTimeCorr.GetXaxis()->SetTitle("time from neutron capture [#mus]");
    
    TH2F hTimeCorr2("hTimeCorr2","Neutron-capture-correlated energy deposition",
                   480, -100, 20, 200, 0, 25);
    hTimeCorr2.GetYaxis()->SetTitle("ionizing deposition [MeV]");
    hTimeCorr2.GetXaxis()->SetTitle("time from neutron capture [ns]");
    
    gStyle->SetOptStat("");
    
    // event counters by PID
    std::map<Int_t, Int_t> primNCapts;
    std::map<Int_t, Int_t> primIoni;
    std::map<Int_t, Int_t> nCaptZA;
    

    // scan events
    Long64_t nentries = T.GetEntries();
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T.GetEntry(ev);
        
        // primaries
        Int_t nPrim = evt->Primaries->GetEntriesFast();
        Int_t primTp = 0;
        hnPrim.Fill(nPrim);
        for(Int_t i=0; i<nPrim; i++) {
            EventPrimaryPtcl* pp = (EventPrimaryPtcl*)evt->Primaries->At(i);
            prim_p.Fill(pp->p[0], pp->p[1]);
            
            primTp = pp->PID;
            if(primTp == 2112) {
                hPrimN1.Fill(pp->E/1000.);
                hPrimN2.Fill(pp->E/1000.);
                hPrimN3.Fill(pp->E);
                hPrimN4.Fill(pp->E*1e3);
            }
        }
        
        // neutron captures
        std::vector<double> capt_times;
        Int_t nNCapts = evt->nCapts->GetEntriesFast();
        for(Int_t i=0; i<nNCapts; i++) {
            EventNCapt* nc = (EventNCapt*)evt->nCapts->At(i);
            nCaptZA[10000 * nc->capt_Z + nc->capt_A] += 1;
            capt_times.push_back(nc->t);
        }
        
        // ionization
        Int_t nIoni = evt->iEvts->GetEntriesFast();
        if(evt->EIoni) {
            hEIoni.Fill(evt->EIoni);
            hEIoniLo.Fill(evt->EIoni);
        }
        for(Int_t i=0; i<nIoni; i++) {
            EventIoniCluster* ei = (EventIoniCluster*)evt->iEvts->At(i);
            if(ei->vol >= 0) {
                hit_xy.Fill(ei->x[0], ei->x[1]);
                hit_zy.Fill(ei->x[2], ei->x[1]);
            }
            
            for(std::vector<double>::const_iterator it = capt_times.begin(); it != capt_times.end(); it++) {
                hTimeCorr.Fill((ei->t - *it)/1000., ei->E);
                hTimeCorr2.Fill((ei->t - *it), ei->E);
            }
        }
        
        if(nPrim == 1) {
            primNCapts[primTp] += (evt->nNCapts > 0);
            primIoni[primTp] += (nIoni > 0);
        }
    }
    
    std::cout << "\nNeutron captures by primary:\n";
    display_map<Int_t,Int_t>(primNCapts);
    std::cout << "\nIonization by primary:\n";
    display_map<Int_t,Int_t>(primIoni);
    std::cout << "\nNeutron capture nucleus 10000*Z + A:\n";
    display_map<Int_t,Int_t>(nCaptZA);
    
    
    hEIoni.Scale(1./hEIoni.GetBinWidth(1));
    hEIoni.SetMaximum(20000);
    hEIoni.Draw();
    hEIoni.Write();
    gPad->Print((outpath+"/EIoni.pdf").c_str());
    
    hEIoniLo.Scale(1./hEIoniLo.GetBinWidth(1));
    //hEIoni.SetMaximum(20000);
    hEIoniLo.Draw();
    hEIoniLo.Write();
    gPad->Print((outpath+"/EIoniLo.pdf").c_str());
    
    hPrimN3.Scale(1./hPrimN3.GetBinWidth(1));
    hPrimN3.Draw();
    hPrimN3.Write();
    gPad->Print((outpath+"/PrimN_lo.pdf").c_str());
    
    hPrimN1.Scale(1./hPrimN1.GetBinWidth(1));
    hPrimN1.Draw();
    hPrimN1.Write();
    gPad->Print((outpath+"/PrimN_mid.pdf").c_str());
    
    gPad->SetLogy(true);
    
    hPrimN4.Scale(1./hPrimN4.GetBinWidth(1));
    hPrimN4.Draw();
    hPrimN4.Write();
    gPad->Print((outpath+"/PrimN_thermal.pdf").c_str());
    
    hnPrim.Draw();
    hnPrim.Write();
    gPad->Print((outpath+"/nPrim.pdf").c_str());
    std::cout << "\nSingle-primary events: " << hnPrim.GetBinContent(2) << " out of " << hnPrim.Integral() << "\n";
    
    hPrimN2.Scale(1./hPrimN2.GetBinWidth(1));
    hPrimN2.Draw();
    hPrimN2.Write();
    gPad->Print((outpath+"/PrimN_hi.pdf").c_str());
    
    gPad->SetLogy(false);
    
    hTimeCorr.SetMaximum(30);
    hTimeCorr.Draw("Col Z");
    hTimeCorr.Write();
    gPad->Print((outpath+"/TimeCorr.pdf").c_str());
    
    hTimeCorr2.SetMaximum(100);
    hTimeCorr2.Draw("Col Z");
    hTimeCorr2.Write();
    gPad->Print((outpath+"/TimeCorr2.pdf").c_str());
    
    gPad->SetCanvasSize(700,700);
    hit_xy.Draw("Col Z");
    gPad->Print((outpath+"/Hit_xy.pdf").c_str());
    hit_zy.Draw("Col Z");
    gPad->Print((outpath+"/Hit_zy.pdf").c_str());
    prim_p.Draw("Col Z");
    gPad->Print((outpath+"/Hit_P0.pdf").c_str());
    
    f->Close();
    delete f;
}
