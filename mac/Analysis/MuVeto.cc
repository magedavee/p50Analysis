// export LD_LIBRARY_PATH=../../../PG4/lib/:$LD_LIBRARY_PATH

#include "AnaUtils.hh"

bool checkVeto(double t, const std::vector<double>& vt, double dt0, double dt1) {
    for(auto itv = vt.begin(); itv != vt.end(); itv++)
        if(dt0 < t-*itv && t-*itv < dt1) return true;
    return false;
}

void dispVFrac(int* vcounts, const std::string& suffix) {
    cout << "Vetoed " << vcounts[true] << "/" << vcounts[false] << " ("
        << double(vcounts[true])/vcounts[false] << ") " << suffix << "\n";
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
    
    TH2F* h_xy;
    TH2F* h_xz;
    TH2F* h_yz;
    TH2F* h[3];
};

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
    double simtime = D.getTotalGenTime();
    // set readout branches
    ParticleEvent* prim = new ParticleEvent();
    T->SetBranchAddress("Prim",&prim);
    IoniClusterEvent* vion = new IoniClusterEvent();
    T->SetBranchAddress("VetoIoni",&vion);
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
    NCaptEvent* snc = new NCaptEvent();
    T->SetBranchAddress("ScN",&snc);
    
    // set up histograms
    TH1F* hVetoSpec = (TH1F*)f.add(new TH1F("hVetoSpec","Muon Veto ionization", 400, 0, 50));
    hVetoSpec->GetXaxis()->SetTitle("Ionizing deposition [MeV]");
    hVetoSpec->GetYaxis()->SetTitle("Event rate [Hz/MeV]");
    hVetoSpec->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* htMuToScint = (TH1F*)f.add(new TH1F("htMuToScint","Timing between veto and LS segment ionization", 400, -10, 20));
    htMuToScint->GetXaxis()->SetTitle("t_{scint}-t_{veto} [ns]");
    htMuToScint->GetYaxis()->SetTitle("Event rate [Hz/ns]");
    
    TH1F* htMuToN = (TH1F*)f.add(new TH1F("htMuToN","Timing between veto ionization and neutron capture", 100, -10, 200));
    htMuToN->GetXaxis()->SetTitle("t_{ncapt}-t_{veto} [#mus]");
    htMuToN->GetYaxis()->SetTitle("Event rate [Hz/#mus]");
    
    TH1F* htMuToIBD = (TH1F*)f.add(new TH1F("htMuToIBD","Timing between veto ionization and `IDB-like' capture", 100, -10, 200));
    htMuToIBD->GetXaxis()->SetTitle("t_{ncapt}-t_{veto} [#mus]");
    htMuToIBD->GetYaxis()->SetTitle("Event rate [Hz/#mus]");
    htMuToIBD->SetLineColor(2);
    
    ProfileHistos hvpos(200,2,"hvpos","Veto ionization positions","[m]");
    ProfileHistos hvnpos(50,2,"hvnpos","Veto ionization positions","[m]");
    
    TH1F* posOff_x = (TH1F*)f.add(new TH1F("posOff_x","Position offset between veto and detector", 100, -4, 4));
    posOff_x->GetXaxis()->SetTitle("x_{veto}-x_{det} [m]");
    posOff_x->GetYaxis()->SetTitle("Event rate [Hz/m]");
    posOff_x->SetLineColor(2);
    //
    TH1F* posOff_y = (TH1F*)f.add(new TH1F("posOff_y","Position offset between veto and detector", 100, -4, 4));
    posOff_y->GetXaxis()->SetTitle("y_{veto}-y_{det} [m]");
    posOff_y->GetYaxis()->SetTitle("Event rate [Hz/m]");
    
    
    double veto_thresh = 5;     //< muon veto trigger threshold energy, MeV
    int nIoniV[2] = {0,0};
    int nNCaptV[2] = {0,0};
    int nIBDV[2] = {0,0};
    
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        prim->Clear();
        vion->Clear();
        sion->Clear();
        snc->Clear();
        T->GetEntry(ev);
        
        Int_t nPrim = prim->particles->GetEntriesFast();
        Int_t nVeto = vion->clusts->GetEntriesFast();
        Int_t nNCapts = snc->nCapts->GetEntriesFast();
        Int_t nScint = sion->clusts->GetEntriesFast();
        
        // primaries
        map<int,int> primCounter;
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)prim->particles->At(i);
            primCounter[pp->PID]++;
        }
        //if(primCounter[2112]) continue; // non-neutron only
        //if(nPrim>1 || !primCounter[2112]) continue; // neutron events only
        
        if(vion->EIoni) hVetoSpec->Fill(vion->EIoni);
        
        // scintillator hits
        map<Int_t, double> volIoni; // Ionization accumulator by volume
        for(Int_t i=0; i<nScint; i++) {
            IoniCluster* ei = (IoniCluster*)sion->clusts->At(i);
            volIoni[ei->vol] += ei->E;
            
            if(ei->vol < 0 || ei->E < 0.2) continue;
            nIoniV[false]++;
            
            // coincident veto hits
            bool isVetoed = false;
            for(Int_t iv=0; iv<nVeto; iv++) {
                IoniCluster* eiv = (IoniCluster*)vion->clusts->At(iv);
                if(eiv->E < veto_thresh) continue;
                double dtime = ei->t - eiv->t;
                isVetoed |= (-3 <= dtime && dtime <= 12);
                htMuToScint->Fill(dtime);
                if(eiv->x[2] > 900.) {
                    posOff_x->Fill((eiv->x[0]-ei->x[0])/1000.);
                    posOff_y->Fill((eiv->x[1]-ei->x[2])/1000.);
                }
            }
            nIoniV[true] += isVetoed;
        }
        bool isolhit = isIsolatedSegment(volIoni);
        
        // veto hits
        for(Int_t iv=0; iv<nVeto; iv++) {
            IoniCluster* eiv = (IoniCluster*)vion->clusts->At(iv);
            hvpos.Fill(eiv->x[0]/1000,eiv->x[1]/1000,eiv->x[2]/1000);
            if(nNCapts) hvnpos.Fill(eiv->x[0]/1000,eiv->x[1]/1000,eiv->x[2]/1000);
            
            if(eiv->E < veto_thresh) continue;
            
            // coincident neutron captures
            for(int in = 0; in < nNCapts; in++) {
                NCapt* nc = (NCapt*)snc->nCapts->At(in);
                if(nc->vol < 0) continue;
                htMuToN->Fill((nc->t - eiv->t)/1000.);
                if(isolhit) htMuToIBD->Fill((nc->t - eiv->t)/1000.);
            }
        }
        
        // neutron captures
        for(Int_t i=0; i<nNCapts; i++) {
            NCapt* nc = (NCapt*)snc->nCapts->At(i);
            if(nc->vol < 0) continue;
            
            nNCaptV[false]++;
            nIBDV[false] += isolhit;
            
            // coincident veto hits
            bool isVetoed = false;
            for(Int_t iv=0; iv<nVeto; iv++) {
                IoniCluster* eiv = (IoniCluster*)vion->clusts->At(iv);
                if(eiv->E < veto_thresh) continue;
                double dtime = nc->t - eiv->t;
                isVetoed |= (-3e3 <= dtime && dtime <= 12e3);
                htMuToN->Fill(dtime);
                if(isolhit) htMuToIBD->Fill(dtime);
            }
            nNCaptV[true] += isVetoed;
            nIBDV[true] += isolhit && isVetoed;
        }
    
    }
    
    dispVFrac(nIoniV,"ionization events");
    dispVFrac(nNCaptV,"neutron captures");
    dispVFrac(nIBDV,"IBD-like");
    
    ///////////////
    // draw results
    
    hVetoSpec->Scale(1./hVetoSpec->GetBinWidth(1)/simtime);
    hVetoSpec->SetMaximum(500.);
    hVetoSpec->Draw();
    cout << "Total coincidence rate " << hVetoSpec->Integral("width") << " Hz\n";
    gPad->Print((outpath+"/VetoSpec.pdf").c_str());

    htMuToScint->Scale(1./htMuToScint->GetBinWidth(1)/simtime);
    htMuToScint->Draw();
    gPad->Print((outpath+"/IoniTiming.pdf").c_str());
    
    posOff_x->Scale(1./posOff_x->GetBinWidth(1)/simtime);
    posOff_y->Scale(1./posOff_y->GetBinWidth(1)/simtime);
    posOff_x->Draw();
    posOff_y->Draw("Same");
    gPad->Print((outpath+"/PosOffset.pdf").c_str());
    
    gPad->SetLogy(true);
    htMuToN->Scale(1./htMuToN->GetBinWidth(1)/simtime);
    htMuToIBD->Scale(1./htMuToIBD->GetBinWidth(1)/simtime);
    htMuToN->SetMinimum(1e-4);
    htMuToN->Draw();
    htMuToIBD->Draw("Same");
    cout << "Neutron capture coincidence rate " << htMuToN->Integral("width") << " Hz\n";
    cout << "IBD-like capture coincidence rate " << htMuToIBD->Integral("width") << " Hz\n";
    gPad->Print((outpath+"/NCaptTiming.pdf").c_str());
    
    gPad->SetLogy(false);
    gPad->SetCanvasSize(700,700);
    gPad->SetRightMargin(0.15);
    
    hvpos.ScaleBinsize();
    hvpos.Scale(1./simtime);
    hvpos.SetMaximum(100);
    hvpos.Print("Col Z",outpath+"/VetoPos");
    
    hvnpos.ScaleBinsize();
    hvnpos.Scale(1./simtime);
    hvnpos.SetMaximum(4);
    hvnpos.Print("Col Z",outpath+"/VetoPosN");
}
