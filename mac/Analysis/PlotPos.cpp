////////////////////////////////////////////
// ROOT macro example for MC output analysis
// .x PlotPos.cpp

void PlotPos() {
    // load library describing data classes
    gSystem->Load("~/Applications/SB_G4_MC/lib/libSB_G4.so");
    
    // load data into TChain
    TChain T("sblmc");
    T.Add("Run_*");
    // set readout branches
    Event* evt = new Event();
    T.GetBranch("iEvts")->SetAutoDelete(kFALSE);
    T.SetBranchAddress("MCEvent",&evt);
    
    // set up histograms
    TH2F* hit_xy = new TH2F("hit_xy", "Hit positions", 200,-1200,1200, 200,-1200,1200);
    TH2F* hit_yz = new TH2F("hit_yz", "Hit positions", 200,-1200,1200, 200,-1200,1200);
    TH2F* prim_p = new TH2F("prim_p", "Primary momentum direction", 100,-1.2,1.2, 100,-1.2,1.2);
    
    // scan events
    Long64_t nentries = T.GetEntries();
    for (Long64_t ev=0; ev<nentries; ev++) {
        evt->Clear();
        T.GetEntry(ev);
        Int_t nPrim = evt->Primaries->GetEntriesFast();
        for(Int_t i=0; i<nPrim; i++) {
            EventPrimaryPtcl* pp = (EventPrimaryPtcl*)evt->Primaries->At(i);
            prim_p->Fill(pp->p[0], pp->p[1]);        
        }
        Int_t nIoni = evt->iEvts->GetEntriesFast();
        for(Int_t i=0; i<nIoni; i++) {
            EventIoniCluster* ei = (EventIoniCluster*)evt->iEvts->At(i);
            hit_xy->Fill(ei->x[0], ei->x[1]);
            hit_yz->Fill(ei->x[1], ei->x[2]);
        }
    }
    
    // display histograms
    hit_xy->Draw("Col Z");
    new TCanvas();
    hit_yz->Draw("Col Z");
    new TCanvas();
    prim_p->Draw("Col Z");
}
