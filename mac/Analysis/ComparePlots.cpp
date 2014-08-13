#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <stdio>

#include <TCanvas.h>
#include <TH1.h>

using std::vector;
using std::string;

void ComparePlots() {
    gStyle->SetOptStat("");
    
    vector<string> fnames;
    fnames.push_back("/home/mpmendenhall/tmp/bare/Out.root");
    fnames.push_back("/home/mpmendenhall/tmp/shielded/Out.root");
    fnames.push_back("/home/mpmendenhall/tmp/Pb6cm/Out.root");
        
    string hname = "hTimeCorr";
    
    //vector<TH1*> hs;
    for(int i=0; i<fnames.size(); i++) {
        cout << fnames[i] << "\n";
        TFile* f = new TFile(fnames[i].c_str(),"READ");
        assert(f);
        TH1* h = (TH1*)f->Get(hname.c_str());
        assert(h);
        
        char projname[1024];
        sprintf(projname,"px_%i",i);
        h = ((TH2F*)h)->ProjectionX(projname);
        h->SetMaximum(5000);
        
        h->SetLineColor(4-i);
        h->Draw(i?"Same":"");
    }
    
    gPad->Print((hname+".pdf").c_str());
}
