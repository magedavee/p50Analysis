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
    // /home/mpmendenhall/Analysis/ShortBaseline/CRY_
    fnames.push_back("/home/mpmendenhall/tmp/bare/Out.root");
    fnames.push_back("/home/mpmendenhall/tmp/shielded/Out.root");
    fnames.push_back("/home/mpmendenhall/tmp/Pb6cm/Out.root");
        
    string hname = "hTimeCorr";
    
    vector<TH1*> hs;
    for(int i=0; i<fnames.size(); i++) {
        cout << fnames[i] << "\n";
        TFile* f = new TFile(fnames[i].c_str(),"READ");
        assert(f);
        TH1* h = (TH1*)f->Get(hname.c_str());
        assert(h);
        
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
        
        hs.push_back(h);
    }
    
    for(int i=0; i<hs.size(); i++)
        hs[i]->Draw(i?"Same":"");
    
    gPad->Print((hname+".pdf").c_str());
}
