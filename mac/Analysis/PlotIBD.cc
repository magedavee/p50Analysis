/// \file PlotIBD.cc \brief Example using TimeStructureAnalyzer to plot IBD-like event distributions

#include "AnaUtils.hh"
#include "TimeStructure.hh"

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
    
    // make output directory, file
    string outpath = inPath + "/Plots/";
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"IBDLike.root");
    
    // set up TimeStructureAnalyzer
    TimeStructureAnalyzer TSA;
    // set data tree to read scintillator ionization into TSA input
    T->SetBranchAddress("ScIoni",&TSA.ionc);
    
    // set up histograms
    TH1F* hIBDEnergy = (TH1F*)f.add(new TH1F("hIBDEnergy", "IBD-like event energy spectrum", 100, 0, 8));
    hIBDEnergy->GetXaxis()->SetTitle("scintillator signal [MeVee]");
    hIBDEnergy->GetYaxis()->SetTitle("Event rate [mHz/MeV]");
    
    TH1F* hIBD1Energy = (TH1F*)f.add(new TH1F("hIBD1Energy", "IBD-like (multiplicity 1) energy spectrum", 100, 0, 8));
    hIBD1Energy->GetXaxis()->SetTitle("scintillator signal [MeVee]");
    hIBD1Energy->GetYaxis()->SetTitle("Event rate [mHz/MeV]");
    hIBD1Energy->SetLineColor(2);
    
    TH1F* hIBDTiming = (TH1F*)f.add(new TH1F("hIBDTiming", "IBD-like coincidence timing", 100, 0, 200));
    hIBDTiming->GetXaxis()->SetTitle("t_{n}-t_{e} [#mus]");
    hIBDTiming->GetYaxis()->SetTitle("Event rate [#muHz/#mus]");
    
    // scan events
    for (Long64_t ev=0; ev<nentries; ev++) {
        T->GetEntry(ev);
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); } // progress bar
        
        TSA.classifyHits(); // generate time-structured event analysis
        // scan over coincidence groups
        for(auto it = TSA.coincs.begin(); it != TSA.coincs.end(); it++) {
            int ibdsegs = it->isIBDlike(); // 0 if not IBD-like; electron-like segment multiplicity otherwise.
            if(!ibdsegs) continue;
            hIBDTiming->Fill((it->promptClusters[1].t_median - it->promptClusters[0].t_median)/1000);
            double Eioni = it->promptClusters[1].Eioni; // prompt ionization signal, summed over all segments [MeVee]
            hIBDEnergy->Fill(Eioni);
            if(ibdsegs==1) hIBD1Energy->Fill(Eioni);
        }
    }
    cout << " Done.\n\n";
    
    // normalize from raw counts to differential rates. Factor of 1000 to get mHz instead of Hz units; 1e6 for uHz.
    normalize_to_bin_width(hIBDEnergy, 1000./D.genTime);
    normalize_to_bin_width(hIBD1Energy, 1000./D.genTime);
    normalize_to_bin_width(hIBDTiming, 1e6/D.genTime);
    
    // plot results
    hIBDEnergy->Draw("HIST");
    hIBD1Energy->Draw("HIST Same");
    gPad->Print((outpath+"/IBDEnergy.pdf").c_str());
    
    hIBDTiming->Draw("HIST");
    gPad->Print((outpath+"/IBDTiming.pdf").c_str());
    
    return 0;
}
