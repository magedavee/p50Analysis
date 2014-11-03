#include "AnaUtils.hh"

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    gStyle->SetOptStat("");
    
    // set input/output paths
    string inPath = ".";
    if(argc == 2) inPath = argv[1];
    string outpath = inPath + "/Plots/";
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"protonSlab.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    //ParticleEvent* prim = new ParticleEvent();
    //T->SetBranchAddress("Prim",&prim);
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
   
    // set up histograms
    TH2F* hProtons = (TH2F*)f.add(new TH2F("hProtons","proton interactions", 200, 0, 100, 100, 0, 10));
    hProtons->GetXaxis()->SetTitle("Ionizing deposition [MeV]");
    hProtons->GetYaxis()->SetTitle("PSD proxy [mm/MeV]");
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        //prim->Clear();
        sion->Clear();
        T->GetEntry(ev);
        
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); }
        
        /*
        // primaries
        Int_t nPrim = prim->particles->GetEntriesFast();
        double pEnergy = 0;
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)prim->particles->At(i);
            pEnergy = pp->E;
        }
        */
        
        // classify scintillator hits
        vector<IoniCluster> scintHits;
        map<Int_t, double> volIoni = mergeIoniHits(sion->clusts, scintHits, 5.);
        for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
            double psd = its->dxtot()*sqrt(12)/its->E;
            hProtons->Fill(its->E, psd);
        }
    }
    
    hProtons->Draw("Col");
    gPad->SetLogz(true);
    gPad->Print((outpath+"/Protons.pdf").c_str());
    gPad->SetLogz(false);
}