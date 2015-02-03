/// \file PlotSingles.cc \brief Example analyzer for plotting scintillator uncorrelated ionization response

#include "AnaUtils.hh"

int main(int argc, char** argv) {
    gSystem->Load("libEventLib.so"); // load library describing data classes
    gStyle->SetOptStat("");
    
    // set input path; "." if not specified as command line argument
    string inPath = ".";
    if(argc == 2) inPath = argv[1];
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    Long64_t nentries = T->GetEntries();
    std::cout << "Scanning " << nentries << " events...\n";
    if(!nentries) return -1;
    // set event read point
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
    
    // make output directory, file
    string outpath = inPath + "/Plots/";
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"Singles.root");
    
    ////////////////////
    // set up histograms

    // energy vs. pseudo-PSD plot
    TH2F* hSingles = (TH2F*)f.add(new TH2F("hSingles","Scintillator events", 200, 0, 5, 200, 0, 6));
    hSingles->GetXaxis()->SetTitle("Visible energy [MeVee]");
    hSingles->GetYaxis()->SetTitle("1/<dE/dx> [mm/MeV]");
    
    // "electron-like" energy deposition
    TH1F* hSinglesE = (TH1F*)f.add(new TH1F("hSinglesE", "Singles energy spectrum", 400, 0, 10));
    hSinglesE->GetXaxis()->SetTitle("Visible energy [MeVee]");
    hSinglesE->GetYaxis()->SetTitle("Event rate [/neutron/MeV]");
    
    // segment multiplicity
    TH1F* hMult = (TH1F*)f.add(new TH1F("hMult", "Hit multiplicity", 16, 0.5, 16.5));
    hMult->GetXaxis()->SetTitle("number of segments triggered");
    hMult->GetYaxis()->SetTitle("Event rate [Hz]");
    
    //////////////
    // scan events
    
    for (Long64_t ev=0; ev<nentries; ev++) {
        T->GetEntry(ev);
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); } // progress bar
        
        // merge ionization tracks (all particles) into observable scintillator hits
        vector<IoniCluster> scintHits;
        mergeIoniHits(sion->clusts, scintHits, 100.);

        int nseg = 0;
        for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
            if(its->vol < 0 || its->vol > 1000) continue; // exclude dead and veto volumes
            double E = its->Equench();
            if(E>0.1) nseg++; // trigger threshold > 0.1 MeV
            hSinglesE->Fill(E);
            hSingles->Fill(E,its->E/its->EdEdx);
        }
        if(nseg) hMult->Fill(nseg);
        
    }
    cout << " Done.\n\n";
    
    //////////////////////////////////////////////////
    // normalize from raw counts to differential rates
    
    normalize_to_bin_width(hSinglesE, 1./D.genTime);
    hMult->Scale(1./D.genTime);
    hSingles->Scale(1./D.genTime);
    
    ///////////////
    // plot results
    
    hSinglesE->Draw("HIST");
    gPad->SetLogy(true);
    gPad->Print((outpath+"/Singles_Energy.pdf").c_str());
    
    gPad->SetLogy(false);
    gPad->SetLogz(true);
    hSingles->SetMinimum(1e-6);
    hSingles->Draw("Col");
    gPad->Print((outpath+"/Singles_PseudoPSD.pdf").c_str());
    
    gPad->SetLogy(true);
    hMult->Draw();
    gPad->Print((outpath+"/Multiplicity.pdf").c_str());
    
    return 0;
}
