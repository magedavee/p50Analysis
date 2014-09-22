#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TLegend.h>

using std::vector;
using std::string;

double m_n = 939.565; // neutron mass, MeV/c^2

void mul_by_v(TGraph* g) {
    double KE,y;
    for(int i=0; i<g->GetN(); i++) {
        g->GetPoint(i,KE,y);
        double E = KE+m_n;
        double p = sqrt(E*E-m_n*m_n);
        g->SetPoint(i,KE,y*p/E*299792458.);
    }
}

void PlotXS() {

    gStyle->SetOptStat("");
    string isotnm = "Z17A37";
    string gnm = "gXS_neutron_"+isotnm;
    
    TFile f("NeutronXS.root","READ");
    TGraph* gEl =   (TGraph*)f.Get((gnm+"_Elastic").c_str());
    TGraph* gInEl = (TGraph*)f.Get((gnm+"_Inelastic").c_str());
    TGraph* gCEx =  (TGraph*)f.Get((gnm+"_ChgEx").c_str());
    TGraph* gCap =  (TGraph*)f.Get((gnm+"_Capture").c_str());
    TGraph* gFis =  (TGraph*)f.Get((gnm+"_Fission").c_str());
    
    mul_by_v(gEl);
    mul_by_v(gInEl);
    mul_by_v(gCEx);
    mul_by_v(gCap);
    mul_by_v(gFis);
    
    gEl->Draw("AL");
    gEl->SetTitle("Geant4 neutron interactions");
    gEl->GetYaxis()->SetTitle("#sigma #times v [bn m/s]");
    gEl->GetXaxis()->SetTitle("neutron energy [MeV]");
    gEl->GetXaxis()->SetTitleOffset(1.4);
    
    gEl->SetMinimum(1);
    gEl->SetMaximum(1e8);
    
    gEl->SetLineColor(2);
    gCap->SetLineColor(4);
    
    gPad->SetLogy(true);
    gPad->SetLogx(true);
    
    gEl->Draw("AL");
    gInEl->Draw("L");
    //gCEx->Draw("L");
    gCap->Draw("L");
    //gFis->Draw("L");
    
    TLegend* leg = new TLegend(0.13, 0.15, 0.48, 0.25);
    leg->SetFillColor(0); // white background
    leg->SetBorderSize(0); // get rid of the box
    leg->SetTextSize(0.045); // set text size
    leg->AddEntry(gEl->GetName(), "Elastic","l");
    leg->AddEntry(gInEl->GetName(), "Inelastic","l");
    leg->AddEntry(gCap->GetName(), "Capture","l");
    leg->Draw();
   
    gPad->Print(("XS_"+isotnm+".pdf").c_str());
}
