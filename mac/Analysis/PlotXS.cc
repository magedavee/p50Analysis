#include "AnaUtils.hh"

#include <TLegend.h>

#include <sstream>
#include <fstream>
#include <cmath>
#include <iostream>
#include <cassert>

double m_n = 939.565; // neutron mass, MeV/c^2

void mul_by_v(TGraph* g) {
    assert(g);
    double KE,y;
    for(int i=0; i<g->GetN(); i++) {
        g->GetPoint(i,KE,y);
        double E = KE+m_n;
        double p = sqrt(E*E-m_n*m_n);
        g->SetPoint(i,KE,y*p/E*299792458.);
    }
}

class MCNP_XS {
public:
    MCNP_XS(const string& fname = "/home/mpmendenhall/Documents/PROSPECT/MCNP_nScatter/XS_6Li.txt") {
        
        gs[0] = &gTotal;
        gs[1] = &gCap;
        npts = 0;
        
        std::ifstream infl(fname.c_str());
        while (infl) {
            string s;
            if(!getline(infl, s)) break;
            if(!s.size() || s[0] == 'E') continue;
            vector<double> v = sToDoubles(s," ");
            if(v.size() != 3) {
                std::cerr << "Unreadable line in input file: '" << s << "' (" << v.size() << ")\n";
                continue;
            }
            gTotal.SetPoint(npts, v[0], v[1]);
            gCap.SetPoint(npts, v[0], v[2]);
            npts++;
        }
        
        for(int i=0; i<2; i++) {
            gs[i]->SetName(("MCNP_"+to_str(i)).c_str());
            mul_by_v(gs[i]);
            gs[i]->Draw("AL");
            gs[i]->GetYaxis()->SetTitle("#sigma #times v [bn m/s]");
            gs[i]->GetXaxis()->SetTitle("neutron energy [MeV]");
            gs[i]->SetLineStyle(2);
            gs[i]->SetLineColor(2+2*i);
        }
    }


    TGraph gTotal;
    TGraph gCap;
    TGraph* gs[2];
    
    int npts;
};

int main(int, char**) {
    
    gSystem->Load("libEventLib.so"); // load library describing data classes

    gStyle->SetOptStat("");
    string isotnm = "Z3A6";
    string gnm = "gXS_neutron_"+isotnm;
    
    MCNP_XS MX;
    
    TFile f("NeutronXS.root","READ");
    TGraph* gEl =   (TGraph*)f.Get((gnm+"_Elastic").c_str());
    TGraph* gInEl = (TGraph*)f.Get((gnm+"_Inelastic").c_str());
    TGraph* gCEx =  (TGraph*)f.Get((gnm+"_ChgEx").c_str());
    TGraph* gCap =  (TGraph*)f.Get((gnm+"_Capture").c_str());
    TGraph* gFis =  (TGraph*)f.Get((gnm+"_Fission").c_str());
    TGraph* gTot =  (TGraph*)f.Get((gnm+"_Total").c_str());
    
    mul_by_v(gEl);
    mul_by_v(gInEl);
    mul_by_v(gCEx);
    mul_by_v(gCap);
    mul_by_v(gFis);
    mul_by_v(gTot);
    
    gEl->Draw("AL");
    gEl->SetTitle("n-^{6}Li interaction cross sections");
    gEl->GetYaxis()->SetTitle("#sigma #times v [bn m/s]");
    gEl->GetXaxis()->SetTitle("neutron energy [MeV]");
    gEl->GetXaxis()->SetTitleOffset(1.4);
    
    gEl->SetMinimum(1e2);
    gEl->SetMaximum(1e8);
    
    gEl->SetLineColor(4);
    gInEl->SetLineColor(2);
    
    gPad->SetLogy(true);
    gPad->SetLogx(true);
    
    gEl->Draw("AL");
    gInEl->Draw("L");
    //gCEx->Draw("L");
    //gCap->Draw("L");
    //gFis->Draw("L");
    gTot->Draw("L");
    
    MX.gCap.Draw("L");
    MX.gTotal.Draw("L");
    
    TLegend* leg = new TLegend(0.53, 0.15, 0.88, 0.45);
    leg->SetFillColor(0); // white background
    leg->SetBorderSize(0); // get rid of the box
    leg->SetTextSize(0.045); // set text size
    leg->AddEntry(gTot->GetName(), "G4 Total","l");
    leg->AddEntry(gEl->GetName(), "G4 Elastic","l");
    leg->AddEntry(gInEl->GetName(), "G4 Inelastic","l");
    //leg->AddEntry(gCap->GetName(), "G4 Capture","l");
    leg->AddEntry(MX.gCap.GetName(), "MCNP Capture","l");
    leg->AddEntry(MX.gTotal.GetName(), "MCNP Total","l");
    leg->Draw();
   
    gPad->Print(("XS_"+isotnm+".pdf").c_str());
    
    return 0;
}
