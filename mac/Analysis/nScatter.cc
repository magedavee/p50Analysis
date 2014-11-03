#include "AnaUtils.hh"

// read Pieter's MCNP results
void readMCNP(string fname, TH1*& hTrans, TH1*& hRefl) {
    
    std::cout << "Reading MCNP results '" << fname << "'...\n";
    std::ifstream inf;
    inf.open(fname.c_str());

    double E, enterSlab, denterSlab, back, dBack, enterOut, dEnterOut, trans, dTrans;
    vector<double> binedges, vtrans, vdtrans, vback, vdback;
    while(inf.good()) {
        inf >> std::skipws >> E >> enterSlab >> denterSlab >> back >> dBack >> enterOut >> dEnterOut >> trans >> dTrans;
        if(!inf.good()) break;
        binedges.push_back(E);
        //std::cout << "\t" << E << "\n";
        if(E >= 100) break;
        vtrans.push_back(trans);
        vdtrans.push_back(dTrans);
        vback.push_back(back);
        vdback.push_back(dBack);
    }
    std::cout << "Found " << vtrans.size() << " histogram bins.\n";
    
    hTrans = new TH1F("hTransM","Transmitted neutron spectrum", binedges.size()-1, &binedges[0]);
    hRefl = new TH1F("hReflM","Reflected neutron spectrum", binedges.size()-1, &binedges[0]);
    for(size_t i=0; i < vtrans.size(); i++) {
        hTrans->SetBinContent(i+1, vtrans[i]);
        hTrans->SetBinError(i+1, vdtrans[i]);
        hRefl->SetBinContent(i+1, vback[i]);
        hRefl->SetBinError(i+1, vdback[i]);
    }
    
    float nTrans = hTrans->Integral();
    float nRefl = hRefl->Integral();
    cout << "MCNP Transmission = " << 100*nTrans << " %\n";
    cout << "MCNP Reflection =  " << 100*nRefl << " %\n";
    cout << "MCNP Absorption =  " << 100*(1-nRefl-nTrans) << " %\n";
    
    hTrans->Rebin(2);
    hRefl->Rebin(2);
    
    normalize_to_bin_width(hTrans);
    scale_times_bin(hTrans);
    normalize_to_bin_width(hRefl);
    scale_times_bin(hRefl);
    
    hTrans->GetXaxis()->SetTitle("kinetic energy [MeV]");
    hRefl->GetXaxis()->SetTitle("kinetic energy [MeV]");
    hTrans->GetYaxis()->SetTitle("E #times d#Phi/dE per incident");
    hRefl->GetYaxis()->SetTitle("E #times d#Phi/dE per incident");
    
    hTrans->GetYaxis()->SetTitleOffset(1.45);
    hTrans->GetXaxis()->SetTitleOffset(1.2);
    hRefl->GetYaxis()->SetTitleOffset(1.45);
    hRefl->GetXaxis()->SetTitleOffset(1.2);
    
    hRefl->SetMaximum(100);
    hRefl->SetMinimum(1e-6);
    hRefl->SetLineWidth(0.5);
    //hRefl->SetLineStyle(2);
    
    hTrans->SetMaximum(100);
    hTrans->SetMinimum(1e-6);
    
    std::cout << "Set up histograms.\n";
}

int main(int argc, char** argv) {
    if(argc != 3) return -1;
    
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    gStyle->SetOptStat("");
    
    string inPath = argv[1];
    string mcnppath = argv[2];
    string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"nScatterOut.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    ParticleEvent* flux = new ParticleEvent();
    T->SetBranchAddress("Flux",&flux);

    // set up histograms
    TH1* hTransM = NULL;
    TH1* hReflM = NULL;
    readMCNP(mcnppath, hTransM, hReflM);
    f.add(hTransM);
    f.add(hReflM);
    
    TH1* hTrans = (TH1*)f.add(hTransM->Clone("hTrans"));
    hTrans->Reset();
    TH1* hRefl = (TH1*)f.add(hReflM->Clone("hRefl"));
    hRefl->Reset();
    
    // scan events
    Long64_t nentries = T->GetEntries();
    cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        flux->Clear();
        T->GetEntry(ev);
        
        Int_t nFlux = flux->particles->GetEntriesFast();
        for(Int_t i=0; i<nFlux; i++) {
            ParticleVertex* pp = (ParticleVertex*)flux->particles->At(i);
            if(pp->PID == 2112) {
                if (pp->p[2]>0) hTrans->Fill(pp->E);
                else hRefl->Fill(pp->E);
            }
        }
    }
    cout << "Done.\n";
    
    float nTrans = hTrans->Integral();
    float nRefl = hRefl->Integral();
    cout << "Transmission = " << 100*nTrans/nentries << " %\n";
    cout << "Reflection =  " << 100*nRefl/nentries << " %\n";
    cout << "Absorption =  " << 100*(nentries-nRefl-nTrans)/nentries << " %\n";
    
    hTrans->Scale(1./nentries);
    normalize_to_bin_width(hTrans);
    scale_times_bin(hTrans);
    
    hRefl->Scale(1./nentries);
    normalize_to_bin_width(hRefl);
    scale_times_bin(hRefl);
    
    hTrans->SetLineColor(2);
    hTransM->SetLineColor(4);
    hRefl->SetLineColor(1);
    hReflM->SetLineColor(3);
    
    hTransM->SetTitle("neutron transmission and reflection");
    hTransM->Draw("HIST");
    gPad->SetLogy(true);
    gPad->SetLogx(true);
    hTrans->Draw("HIST Same");
    hReflM->Draw("HIST Same");
    hRefl->Draw("HIST Same");
    
    TLegend* leg = new TLegend(0.13, 0.7, 0.48, 0.88);
    leg->SetFillColor(0); // white background
    leg->SetBorderSize(0); // get rid of the box
    leg->SetTextSize(0.045); // set text size
    leg->AddEntry("hTrans","Geant4 transmitted","l");
    leg->AddEntry("hTransM","MCNP transmitted","l");
    leg->AddEntry("hRefl","Geant4 reflected","l");
    leg->AddEntry("hReflM","MCNP reflected","l");
    leg->Draw();
    
    gPad->Print((outpath+"/nScatter.pdf").c_str());
}
