#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TLegend.h>

using std::vector;
using std::string;

void PlotXS() {

    gStyle->SetOptStat("");
    string isotnm = "Z3A6"; //"Z1A1";
    string gnm = "gXS_neutron_"+isotnm;
    
    TFile f("NeutronXS.root","READ");
    TGraph* gEl =   (TGraph*)f.Get((gnm+"_Elastic").c_str());
    TGraph* gInEl = (TGraph*)f.Get((gnm+"_Inelastic").c_str());
    TGraph* gCEx =  (TGraph*)f.Get((gnm+"_ChgEx").c_str());
    TGraph* gCap =  (TGraph*)f.Get((gnm+"_Capture").c_str());
    TGraph* gFis =  (TGraph*)f.Get((gnm+"_Fission").c_str());
    
    gEl->Draw("AL");
    gEl->SetTitle("Geant4 neutron interactions");
    gEl->GetYaxis()->SetTitle("cross section [bn]");
    gEl->GetXaxis()->SetTitle("neutron energy [MeV]");
    gEl->GetXaxis()->SetTitleOffset(1.4);
    
    gEl->SetMinimum(1e-6);
    gEl->SetMaximum(1e4);
    
    gEl->SetLineColor(2);
    gCap->SetLineColor(4);
    
    gPad->SetLogy(true);
    gPad->SetLogx(true);
    
    gEl->Draw("AL");
    gInEl->Draw("L");
    //gCEx->Draw("L");
    gCap->Draw("L");
    //gFis->Draw("L");
    
    TLegend* leg = new TLegend(0.13, 0.15, 0.48, 0.30);
    leg->SetFillColor(0); // white background
    leg->SetBorderSize(0); // get rid of the box
    leg->SetTextSize(0.045); // set text size
    leg->AddEntry(gEl->GetName(), "Elastic","l");
    leg->AddEntry(gInEl->GetName(), "Inelastic","l");
    leg->AddEntry(gCap->GetName(), "Capture","l");
    leg->Draw();
   
    gPad->Print(("XS_"+isotnm+".pdf").c_str());
}
