#include "AnaUtils.hh"

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    gStyle->SetOptStat("");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"nScatterOut.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    ParticleEvent* flux = new ParticleEvent();
    T->SetBranchAddress("Flux",&flux);

    // set up histograms
    TH1* hTrans = (TH1*)f.add(logHist("hTrans","Transmitted neutron spectrum",200,1e-9,10));
    hTrans->GetXaxis()->SetTitle("kinetic energy [MeV]");
    hTrans->GetYaxis()->SetTitle("Lethargy E #times d#Phi/dE");
    hTrans->GetYaxis()->SetTitleOffset(1.45);
    hTrans->GetXaxis()->SetTitleOffset(1.2);
    
    // scan events
    Long64_t nentries = T->GetEntries();
    cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        flux->Clear();
        T->GetEntry(ev);
        
        Int_t nFlux = flux->particles->GetEntriesFast();
        for(Int_t i=0; i<nFlux; i++) {
            ParticleVertex* pp = (ParticleVertex*)flux->particles->At(i);
            if(pp->PID == 2112 && pp->p[2]>0)
                hTrans->Fill(pp->E);
        }
    }
    cout << "Done.\n";
    
    float nTrans = hTrans->Integral();
    cout << "Transmission = " << nTrans/nentries << "\n";
    
    hTrans->Scale(1./nentries);
    normalize_to_bin_width(hTrans);
    scale_times_bin(hTrans);
    
    hTrans->Draw();
    gPad->SetLogy(true);
    gPad->SetLogx(true);
    gPad->Print((outpath+"/Transmitted.pdf").c_str());
}
