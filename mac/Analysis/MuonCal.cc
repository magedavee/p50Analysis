#include "AnaUtils.hh"

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    gStyle->SetOptStat("");
    
    std::string inPath = ".";
    if(argc == 2) inPath = argv[1];
    std::string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"MuonCal.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    double simtime = D.getTotalGenTime();
    // set readout branches
    ParticleEvent* prim = new ParticleEvent();
    T->SetBranchAddress("Prim",&prim);
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);

    // set up histograms
    TH1F* hSpec = (TH1F*)f.add(new TH1F("hSpec","Scintillator ionization spectrum", 400, 0, 200));
    hSpec->GetXaxis()->SetTitle("Ionizing deposition [MeV]");
    hSpec->GetYaxis()->SetTitle("Event rate [Hz/MeV]");
    hSpec->GetYaxis()->SetTitleOffset(1.45);
    
    TH1F* hSpec2 = (TH1F*)f.add(new TH1F("hSpec2","Scintillator ionization spectrum", 400, 0, 200));
    hSpec->SetLineColor(2);
    
    TH1F* hdz = (TH1F*)f.add(new TH1F("hdz","Ionization position spread over cell", 200, 0, 200));
    hdz->GetXaxis()->SetTitle("RMS position spread [mm]");
    hdz->GetYaxis()->SetTitle("Event rate [Hz/mm]");
    
    TH1F* hdx = (TH1F*)f.add(new TH1F("hdx","Ionization position spread over cell", 200, 0, 200));
    hdx->GetXaxis()->SetTitle("RMS position spread [mm]");
    hdx->GetYaxis()->SetTitle("Event rate [Hz/mm]");
    hdx->SetLineColor(3);
    
    TH1F* hdy = (TH1F*)f.add(new TH1F("hdy","Ionization position spread over cell", 200, 0, 200));
    hdy->GetXaxis()->SetTitle("RMS spread [mm]");
    hdy->GetYaxis()->SetTitle("Event rate [Hz/mm]");
    hdy->SetLineColor(2);
    
    TH1F* hMult10 = (TH1F*)f.add(new TH1F("hMult10","Cell multiplicity", 8,0.5,8.5));
    hMult10->GetXaxis()->SetTitle("number of cells");
    hMult10->GetYaxis()->SetTitle("Event rate [Hz]");
    
    TH1F* hMult40 = (TH1F*)f.add(new TH1F("hMult40","Cell multiplicity", 8,0.5,8.5));
    hMult40->GetXaxis()->SetTitle("number of cells");
    hMult40->GetYaxis()->SetTitle("Event rate [Hz]");
    hMult40->SetLineColor(2);
    
    // scan events
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        prim->Clear();
        sion->Clear();
        T->GetEntry(ev);
    
        Int_t nScint = sion->clusts->GetEntriesFast();
        
        // primaries
        /*
        Int_t nPrim = prim->particles->GetEntriesFast();
        map<int,int> primCounter;
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)prim->particles->At(i);
            primCounter[pp->PID]++;
        }
        */
        
        // scintillator hits
        map<Int_t, double> volIoni; // Ionization accumulator by volume
        for(Int_t i=0; i<nScint; i++) {
            IoniCluster* ei = (IoniCluster*)sion->clusts->At(i);
            if(ei->vol < 0) continue;
            volIoni[ei->vol] += ei->E;
            if(40 < ei->E && ei->E < 100) { 
                hdz->Fill(ei->dx[2]);
                hdx->Fill(ei->dx[0]);
                hdy->Fill(ei->dx[1]);
            }
        }
        int nThresh10 = 0;
        int nThresh40 = 0;
        for(auto it = volIoni.begin(); it != volIoni.end(); it++) {
            if(it->second > 10) nThresh10++;
            if(it->second > 40) nThresh40++;
            if(it->first == 12) hSpec->Fill(it->second);
            int vol = it->first;
            if(vol == 12
                && !(volIoni.count(vol-1) && volIoni[vol-1]>1)
                && !(volIoni.count(vol+1) && volIoni[vol+1]>1) ) hSpec2->Fill(it->second);
        }
        if(nThresh10) hMult10->Fill(nThresh10);
        if(nThresh40) hMult40->Fill(nThresh40);

    }
    
    hMult10->Scale(1./simtime);
    hMult40->Scale(1./simtime);
    hMult40->Draw();
    hMult10->Draw("Same");
    gPad->Print((outpath+"/CellMult.pdf").c_str());
    
    hSpec->Sumw2();
    hSpec->Scale(1./hSpec->GetBinWidth(1)/simtime);
    //hSpec2->Sumw2();
    hSpec2->Scale(1./hSpec2->GetBinWidth(1)/simtime);
    
    TF1 lFit("lFit","landau",43,100);
    lFit.SetLineColor(1);
    hSpec->Fit(&lFit,"RE");
    
    double mpv = lFit.GetParameter(1);
    double d_mpv = lFit.GetParError(1);
    double res_per_hr = (d_mpv/mpv)*sqrt(simtime/3600);
    cout << "dE/E = " << res_per_hr << " per hour\n";
    
    hSpec->SetMinimum(0);
    hSpec->SetMaximum(1);
    hSpec->Draw();
    hSpec2->Draw("Same");
    cout << "Total rate " << hSpec->Integral("width") << " Hz\n";
    gPad->Print((outpath+"/ScintSpec.pdf").c_str());
    
    hdz->Scale(1./hdz->GetBinWidth(1)/simtime/32);
    hdx->Scale(1./hdx->GetBinWidth(1)/simtime/32);
    hdy->Scale(1./hdy->GetBinWidth(1)/simtime/32);
    hdy->Draw();
    hdz->Draw("Same");
    hdx->Draw("Same");
    gPad->Print((outpath+"/PosSpread.pdf").c_str());
}
