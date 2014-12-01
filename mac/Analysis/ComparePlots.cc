#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdio>

#include <TCanvas.h>
#include <TH1.h>

using std::vector;
using std::string;

void normalize_to_bin_width(TH1* f) {
    for(int i=1; i<f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
        f->SetBinError(i, f->GetBinError(i)*scale);
    }
}

void CompareMultiFiles() {
    vector<string> fnames;
    //fnames.push_back("${PG4_OUTDIR}/PROSPECT-2_gamma_Jun30/Plots/PROSPECT-2.root");
    //fnames.push_back("${PG4_OUTDIR}/PROSPECT-2_gamma_Aug28P/Plots/PROSPECT-2.root");
    
    fnames.push_back("${PG4_OUTDIR}/PROSPECT-2_cosmic_bg/Plots/PROSPECT-2.root");
    fnames.push_back("${PG4_OUTDIR}/PROSPECT-2_n_bg/Plots/PROSPECT-2.root");
    
    vector<string> hnames;
    hnames.push_back("hSinglesE");
    //hnames.push_back("hNRecoils");
    
    for(auto ithn = hnames.begin(); ithn != hnames.end(); ithn++) {
        
        vector<TH1*> hs;
        double mx =0;
        for(int i=0; i<fnames.size(); i++) {
            cout << fnames[i] << "\n";
            TFile* f = new TFile(fnames[i].c_str(),"READ");
            assert(f);
            TH1* h = (TH1*)f->Get(ithn->c_str());
            assert(h);
            
            if(h->GetMaximum()>mx) mx = h->GetMaximum();
            hs.push_back(h);
            continue;
            
            char projname[1024];
            sprintf(projname,"px_%i",i);
            h = ((TH2F*)h)->ProjectionX(projname);
            string fitname = "fit_";
            fitname += projname;
            TF1* expFit = new TF1(fitname.c_str(),"expo",-20.,-0.25);
            expFit->SetLineColor(4-i);
            h->SetMaximum(4000);
            h->SetLineColor(4-i);
            h->Fit(expFit,"0R"); // "0" = do NOT automatically draw "hist"
            h->GetFunction(fitname.c_str())->ResetBit(TF1::kNotDraw); // make "projname" visible (= 1<<9)
            
        }
        
        for(int i=0; i<hs.size(); i++) {
            hs[i]->SetLineColor(2+2*i);
            hs[i]->GetYaxis()->SetTitleOffset(1.4);
            //hs[i]->Draw(i?"HIST E1 X0 Same":"HIST E1 X0");
            hs[i]->Draw(i?"Same":"");
        }
        
        //gPad->SetLogy(true);
        //gPad->SetLogx(true);
        gPad->Print((*ithn+".pdf").c_str());
    }
}

void CompareMultiHistograms() {
    
    TFile* f = new TFile("$PG4_AUX/BGSpectra/Gammas.root","READ");
    
    vector<string> hnames;
    hnames.push_back("gamma_Jun30_unfold");
    hnames.push_back("gamma_Aug28P_unfold");

    TH1* hAxes = new TH1F("hAxes","ambient gamma spectrum", 10, 0, 8);
    hAxes->GetYaxis()->SetTitle("flux [Hz/MeV/cm^{2}]");
    hAxes->GetYaxis()->SetTitleOffset(1.35);
    hAxes->SetMaximum(1000);
    hAxes->Draw();
    
    int i = 0;
    for(auto ithn = hnames.begin(); ithn != hnames.end(); ithn++) {
        TH1* h = (TH1*)f->Get(ithn->c_str());
        assert(h);
        
        h->SetLineColor(2+2*i);
        normalize_to_bin_width(h);
        hAxes->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
        //hAxes->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
        h->Draw("HIST Same");
        
        i++;
    }
    
    //gPad->SetLogy(true);
    //gPad->SetLogx(true);
    gPad->Print("PlotCompare.pdf");
    
}

void ComparePlots() {
    gStyle->SetOptStat("");
    CompareMultiFiles();
    //CompareMultiHistograms();
}
