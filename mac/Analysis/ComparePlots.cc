#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdio>

#include <TCanvas.h>
#include <TH1.h>

using std::vector;
using std::string;

void ComparePlots() {
    gStyle->SetOptStat("");
    
    vector<string> fnames;
    string matnm = "solidCl_1_cm";
    fnames.push_back("/home/mpmendenhall/Analysis/ShortBaseline/nScatter_"+matnm+"_10_MeV/Plots/nScatterOut.root");
    fnames.push_back("/home/mpmendenhall/Analysis/ShortBaseline/nScatter_"+matnm+"_1_keV/Plots/nScatterOut.root");
    fnames.push_back("/home/mpmendenhall/Analysis/ShortBaseline/nScatter_"+matnm+"_1_eV/Plots/nScatterOut.root");
    fnames.push_back("/home/mpmendenhall/Analysis/ShortBaseline/nScatter_"+matnm+"_0.02_eV/Plots/nScatterOut.root");
    
    vector<string> hnames;
    hnames.push_back("hRefl");
    hnames.push_back("hTrans");
    
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
            //hs[i]->SetMaximum(2*mx);
            hs[i]->SetMaximum(100);
            hs[i]->SetMinimum(1e-6);
            hs[i]->SetLineColor(1+i);
            hs[i]->Draw(i?"Same":"");
        }
        
        gPad->SetLogy(true);
        gPad->SetLogx(true);
        gPad->Print((*ithn+"_"+matnm+".pdf").c_str());
        
    }
}
