// export LD_LIBRARY_PATH=../../../PG4/lib/:$LD_LIBRARY_PATH

#include "AnaUtils.hh"
#include "strutils.hh"

bool checkVeto(double t, const std::vector<double>& vt, double dt0, double dt1) {
    for(auto itv = vt.begin(); itv != vt.end(); itv++)
        if(dt0 < t-*itv && t-*itv < dt1) return true;
    return false;
}

void dispVFrac(double* vcounts, const std::string& suffix) {
    double nVetoed = vcounts[false]-vcounts[true];
    cout << "Vetoed " << nVetoed << "/" << vcounts[false] << " (" << nVetoed/vcounts[false] << ") " << suffix << "\n";
}

class ProfileHistos {
public:
    ProfileHistos(int nbins, double r, const string& nm, const string& ttl, const std::string& u) {
        h_xy = new TH2F((nm+"_xy").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
        h_xy->GetXaxis()->SetTitle(("x position "+u).c_str());
        h_xy->GetYaxis()->SetTitle(("y position "+u).c_str());
        h[0] = h_xy;

        h_xz = new TH2F((nm+"_xz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
        h_xz->GetXaxis()->SetTitle(("x position "+u).c_str());
        h_xz->GetYaxis()->SetTitle(("z position "+u).c_str());
        h[1] = h_xz;

        h_yz = new TH2F((nm+"_yz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
        h_yz->GetXaxis()->SetTitle(("y position "+u).c_str());
        h_yz->GetYaxis()->SetTitle(("z position "+u).c_str());
        h[2] = h_yz;
        
        for(int i=0; i<3; i++) {
            h[i]->GetYaxis()->SetTitleOffset(1.4);
            hProf[i] = NULL;
        }
    }
    
    void ScaleBinsize() { for(int i=0; i<3; i++) h[i]->Scale(1./h[i]->GetXaxis()->GetBinWidth(1)/h[i]->GetYaxis()->GetBinWidth(1)); }
    void Scale(double s) { for(int i=0; i<3; i++) h[i]->Scale(s); }
    void SetMaximum(double m) { for(int i=0; i<3; i++) h[i]->SetMaximum(m); }
    
    void Fill(double x, double y, double z, double w=1.) {
        h_xy->Fill(x,y,w);
        h_xz->Fill(x,z,w);
        h_yz->Fill(y,z,w);
    }
    
    void Print(const char* opt, const string& bpath) {
        h_xy->Draw(opt);
        gPad->Print((bpath+"_xy.pdf").c_str());
        h_xz->Draw(opt);
        gPad->Print((bpath+"_xz.pdf").c_str());
        h_yz->Draw(opt);
        gPad->Print((bpath+"_yz.pdf").c_str());
    }
    
    void makeProf() {
        hProf[0] = h_xy->ProjectionX();
        hProf[1] = h_yz->ProjectionX();
        hProf[2] = h_xz->ProjectionY();
        for(int i=0; i<3; i++) {
            hProf[i]->Scale(1./hProf[i]->GetXaxis()->GetBinWidth(1));
            hProf[i]->SetLineColor(2+i);
        }
    }
    
    TH2F* h_xy;
    TH2F* h_xz;
    TH2F* h_yz;
    TH2F* h[3];
    
    TH1* hProf[3];
};

bool isAdjacentPair(const map<Int_t, Int_t>& volHits, const OutDirLoader& D) {
    if(volHits.size() != 2) return false;
    auto it = volHits.begin();
    int v1 = it->first;
    int v2 = (++it)->first;
    return D.isAdjacent(v1,v2);
}

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    gStyle->SetOptStat("");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"MuVetoOut.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    ParticleEvent* prim = new ParticleEvent();
    T->SetBranchAddress("Prim",&prim);
    IoniClusterEvent* vion = new IoniClusterEvent();
    T->SetBranchAddress("VetoIoni",&vion);
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
    NCaptEvent* scn = new NCaptEvent();
    T->SetBranchAddress("ScN",&scn);
    
    // set up histograms
    TH2F* hSingles = (TH2F*)f.add(new TH2F("hSingles","Scintillator events", 200, 0, 20, 100, 0, 10));
    hSingles->GetXaxis()->SetTitle("Ionizing deposition [MeV]");
    hSingles->GetYaxis()->SetTitle("PSD proxy [mm/MeV]");
    //hSingles->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hCoinc[2];
    TH1F* hIBDSpec[2];
    for(unsigned int i=0; i<2; i++) {
        hCoinc[i] = (TH1F*)f.add(new TH1F(("hCoinc_"+to_str(i)).c_str(),"IBD-like coincidences", 100, -100, 300));
        hCoinc[i]->GetXaxis()->SetTitle("t_{n} - t_{e} [#mus]");
        hCoinc[i]->GetYaxis()->SetTitle("Event rate [mHz/#mus]");
        hCoinc[i]->GetYaxis()->SetTitleOffset(1.45);
        hCoinc[i]->SetLineColor(4-2*i);
        
        hIBDSpec[i] = (TH1F*)f.add(new TH1F(("hIBDSpec"+to_str(i)).c_str(),"IBD-like spectrum", 100, 0, 20));
        hIBDSpec[i]->GetXaxis()->SetTitle("Prompt ionization [MeV]");
        hIBDSpec[i]->GetYaxis()->SetTitle("Event rate [mHz/MeV]");
        hIBDSpec[i]->GetYaxis()->SetTitleOffset(1.45);
        hIBDSpec[i]->SetLineColor(4-2*i);
    }

    TH1F* hOuterSpec = (TH1F*)f.add(new TH1F("hOuterSpec","Outer scintillator ionization", 100, 0, 40));
    hOuterSpec->GetXaxis()->SetTitle("energy deposition [MeV]");
    hOuterSpec->GetYaxis()->SetTitle("Event rate [mHz/MeV]");
    hOuterSpec->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hVetoSpec = (TH1F*)f.add(new TH1F("hVetoSpec","Muon Veto ionization", 400, 0, 50));
    hVetoSpec->GetXaxis()->SetTitle("Ionizing deposition [MeV]");
    hVetoSpec->GetYaxis()->SetTitle("Event rate [Hz/MeV]");
    hVetoSpec->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* htMuToScint = (TH1F*)f.add(new TH1F("htMuToScint","Timing between veto and LS segment ionization", 400, -10, 20));
    htMuToScint->GetXaxis()->SetTitle("t_{scint}-t_{veto} [ns]");
    htMuToScint->GetYaxis()->SetTitle("Event rate [Hz/ns]");
    
    TH1F* htMuToIBD = (TH1F*)f.add(new TH1F("htMuToIBD","Timing between veto ionization and `IBD-like' event", 100, -100, 100));
    htMuToIBD->GetXaxis()->SetTitle("t_{IBD}-t_{veto} [ns]");
    htMuToIBD->GetYaxis()->SetTitle("Event rate [mHz/ns]");
    htMuToIBD->SetLineColor(2);
    
    TH1F* hPrimE = (TH1F*)f.add(logHist("hPrimE","Cosmic neutrons inducing `IBD-like' events", 100, 1e-3, 1e4));
    hPrimE->GetXaxis()->SetTitle("primary neutron energy [MeV]");
    hPrimE->GetXaxis()->SetTitleOffset(1.3);
    
    ProfileHistos hIBDpos(200,1.5,"hIBDpos","IBD-like event positions","[m]");
    
    double veto_thresh = 5;     //< muon veto trigger threshold energy, MeV
    
    // counters for events by particle
    map<int,int> trigVeto;
    map<int,int> trigScint[2];
    map<int,int> trigIBD[2];
    map<int,int> nCapts;
    map<int,int> nNCapts;
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        prim->Clear();
        vion->Clear();
        sion->Clear();
        scn->Clear();
        T->GetEntry(ev);
        
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); }
        
        // primaries
        map<int,int> primCounter;
        Int_t nPrim = prim->particles->GetEntriesFast();
        double nEnergy = 0;
        int PID = 0;
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)prim->particles->At(i);
            PID = pp->PID;
            primCounter[PID]++;
            if(PID == 2112) nEnergy = pp->E;
        }
        if(nPrim > 1) PID = 0;

        // neutron captures
        Int_t nNCapt = scn->nCapts->GetEntriesFast();
        int nCaptInVol = 0;
        for(Int_t i=0; i<nNCapt; i++) {
            NCapt* nc = (NCapt*)scn->nCapts->At(i);
            nCaptInVol += (nc->vol >= 0);
        }
        nNCapts[nCaptInVol]++;
        if(nCaptInVol) nCapts[PID]++;
        
        if(vion->EIoni) hVetoSpec->Fill(vion->EIoni);
        
        vector<IoniCluster> scintHits;
        map<Int_t, double> volIoni = mergeIoniHits(sion->clusts, scintHits, 5.);
        
        vector<IoniCluster> vetoHits;
        map<Int_t, double> vetoIoni = mergeIoniHits(vion->clusts, vetoHits, 5.);
        int nVetoTrigs = 0;
        for(auto itv = vetoHits.begin(); itv != vetoHits.end(); itv++) {
            hVetoSpec->Fill(itv->E);
            if(itv->E > veto_thresh) {
                nVetoTrigs++;
                trigVeto[PID]++;
            }
        }
        
        // classify scintillator hits
        vector<IoniCluster> nCaptHits;
        vector<IoniCluster> recoilHits;
        vector<IoniCluster> eLikeHits;
        map<Int_t, Int_t> volHits;
        for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
            if(its->vol < 0) continue;
            if(its->E < 0.1) continue;
            volHits[its->vol]++;
            
            //if(fabs(its->x[0]) > 144*6 || fabs(its->x[1]) > 144*4 || fabs(its->x[2]) > 500-144) continue;
            
            double psd = its->dxtot()*sqrt(12)/its->E;
            hSingles->Fill(its->E, psd);
            
            if(4.5 < its->E && its->E < 5.0 && psd < 1) nCaptHits.push_back(*its);
            else if(0.5 < its->E && psd < 1) recoilHits.push_back(*its);
            if(0.1 < its->E && its->E < 20. && 1 < psd) eLikeHits.push_back(*its);
        }
        
        if(volHits.size()) {
            trigScint[false][PID]++;
            if(!nVetoTrigs) trigScint[true][PID]++;
        }
        
        // single- or adjacent-segments-only events
        if(false && isAdjacentPair(volHits,D)) {
            eLikeHits = mergeIoniHits(eLikeHits, 5.);
        } else if(volHits.size() != 1) continue;
        
        // coincidences
        int nIBD = 0;
        for(auto ite = eLikeHits.begin(); ite != eLikeHits.end(); ite++) {
            //if(!(2. < ite->E && ite->E < 5)) continue;
            for(auto itn = nCaptHits.begin(); itn != nCaptHits.end(); itn++) {
                if(ite->vol != itn->vol) continue; // require n/e coincidences in same volume
                nIBD++;
                trigIBD[false][PID]++;
                hIBDpos.Fill(ite->x[0]/1000., ite->x[1]/1000., ite->x[2]/1000.);
                bool isOuterLayer = (fabs(ite->x[0]) > 144*6 || fabs(ite->x[1]) > 144*4);
                hIBDSpec[false]->Fill(ite->E);
                hCoinc[false]->Fill( (itn->t - ite->t)/1000. );
                if(isOuterLayer) hOuterSpec->Fill(volIoni[-1]);
                
                bool isVetoed = false;
                for(auto itv = vetoHits.begin(); itv != vetoHits.end(); itv++) {
                    if(itv->E < veto_thresh) continue;
                    double dt = ite->t - itv->t;
                    htMuToIBD->Fill(dt);
                    if(0 < dt && dt < 50) isVetoed = true;
                }
                if(!isVetoed) {
                    trigIBD[true][PID]++;
                    hIBDSpec[true]->Fill(ite->E);
                    hCoinc[true]->Fill( (itn->t - ite->t)/1000. );
                }
            }
        }
        
        if(nIBD) hPrimE->Fill(nEnergy);
    }
    
    cout << "\n\n";
    
    // vetoed events in energy range of interest
    int b1 = hIBDSpec[false]->FindBin(2.0);
    int b2 = hIBDSpec[false]->FindBin(5.0);
    double nIBDV[2] = {0,0};
    for(int i=0; i<2; i++) nIBDV[i] = hIBDSpec[i]->Integral(b1,b2);
    dispVFrac(nIBDV,"IBD-like");
    
    cout << "\n\nVeto triggers:\n";
    display_map(trigVeto,D.genTime);
    cout << "\n\nScintillator triggers:\n";
    for(int i=0; i<2; i++) display_map(trigScint[i],D.genTime);
    cout << "\n\nIBD triggers:\n";
    for(int i=0; i<2; i++) display_map(trigIBD[i],D.genTime);
    cout << "\n\nNeutron captures:\n";
    display_map(nCapts,D.genTime);
    //cout << "\n\nNumber of neutron captures in event:\n";
    //display_map(nNCapts,D.genTime);
    
    ///////////////
    // draw results
    
    hSingles->Scale(1./hSingles->GetYaxis()->GetBinWidth(1)/hSingles->GetXaxis()->GetBinWidth(1)/D.genTime);
    hSingles->SetMinimum(0.1);
    hSingles->SetMaximum(100);
    hSingles->Draw("Col Z");
    gPad->SetLogz(true);
    gPad->Print((outpath+"/Singles.pdf").c_str());
    gPad->SetLogz(false);
    
    
    hVetoSpec->Scale(1./hVetoSpec->GetBinWidth(1)/D.genTime);
    hVetoSpec->SetMaximum(500.);
    hVetoSpec->Draw();
    cout << "Total veto rate " << hVetoSpec->Integral("width") << " Hz\n";
    gPad->Print((outpath+"/VetoSpec.pdf").c_str());
    
    double nIBDcounts = hCoinc[false]->Integral();
    for(int i=0; i<2; i++) hCoinc[i]->Scale(1000./hCoinc[i]->GetXaxis()->GetBinWidth(1)/D.genTime);
    hCoinc[false]->Draw();
    hCoinc[true]->Draw("Same");
    gPad->Print((outpath+"/Coincidences.pdf").c_str());
    double IBDrate = hCoinc[0]->Integral("width");
    cout << "IBD-like rate: " << IBDrate << " (" << int(IBDrate/sqrt(nIBDcounts)) << ") mHz\n";
    cout << "Passed veto: " << hCoinc[1]->Integral("width") << " mHz\n";
    
    htMuToIBD->Scale(1000./htMuToIBD->GetBinWidth(1)/D.genTime);
    htMuToIBD->Draw();
    gPad->Print((outpath+"/IBDVetoTiming.pdf").c_str());
    
    for(int i=0; i<2; i++) hIBDSpec[i]->Scale(1000./hIBDSpec[i]->GetBinWidth(1)/D.genTime);
    hIBDSpec[false]->Draw();
    hIBDSpec[true]->Draw("Same");
    gPad->Print((outpath+"/IBDSpectrum.pdf").c_str());
    
    hOuterSpec->Scale(1000./hOuterSpec->GetBinWidth(1)/D.genTime);
    hOuterSpec->SetMaximum(100);
    hOuterSpec->Draw();
    gPad->Print((outpath+"/IBDOuterSpectrum.pdf").c_str());
    
    hIBDpos.makeProf();
    for(int i=0; i<3; i++) {
        hIBDpos.hProf[i]->Scale(1./D.genTime);
        hIBDpos.hProf[i]->GetXaxis()->SetTitle("event position [m]");
        hIBDpos.hProf[i]->GetYaxis()->SetTitle("rate [Hz/m]");
    }
    hIBDpos.hProf[1]->Draw();
    hIBDpos.hProf[0]->Draw("Same");
    hIBDpos.hProf[2]->Draw("Same");
    gPad->Print((outpath+"/IBDPos.pdf").c_str());
    
    gPad->SetLogx(true);
    hPrimE->Scale(1000./D.genTime);
    hPrimE->GetYaxis()->SetTitle("rate [mHz/bin]");
    hPrimE->Draw();
    gPad->Print((outpath+"/nPrimE.pdf").c_str());
    
    return 0;
}
