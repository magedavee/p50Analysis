////////////////////////////////////////////////////////////////
// Standalone program for plotting scintillator event positions
// ./PlotPos $PG4_OUTDIR/<directory>

#include "AnaUtils.hh"

int main(int argc, char** argv) {
    gSystem->Load("libEventLib.so"); // load library describing data classes
    gStyle->SetOptStat("");
        
    string inPath = ".";
    if(argc == 2) inPath = argv[1];
    string outpath = inPath + "/Plots/";

    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"Out.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    IoniClusterEvent* sion = new IoniClusterEvent();
    T->SetBranchAddress("ScIoni",&sion);
    NCaptEvent* scn = new NCaptEvent();
    T->SetBranchAddress("ScN",&scn);
    
    // set up histograms
    ProfileHistos hIPos(500,1.0,"hIPos","ionization average positions","[m]");
    ProfileHistos hnPos(500,1.0,"hnPos","neutron capture positions","[m]");
    
    // scan events
    Long64_t nentries = T->GetEntries();
    cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        if(!(ev % (nentries/20))) { cout << "*"; cout.flush(); }
        T->GetEntry(ev);

        // scintillator hits
        Int_t nScint = sion->clusts->GetEntriesFast();
        for(Int_t i=0; i<nScint; i++) {
            IoniCluster* ei = (IoniCluster*)sion->clusts->At(i);
            hIPos.Fill(ei->x[0]/1000., ei->x[1]/1000., ei->x[2]/1000.);
        }
        
        // neutron captures
        Int_t nNCapt = scn->nCapts->GetEntriesFast();
        for(Int_t i=0; i<nNCapt; i++) {
            NCapt* nc = (NCapt*)scn->nCapts->At(i);
            hnPos.Fill(nc->x[0]/1000., nc->x[1]/1000., nc->x[2]/1000.);
        }
    }
    
    hnPos.h_xy->Draw();
    gPad->SetCanvasSize(700,700);
    
    hnPos.ScaleBinsize();
    hnPos.Scale(1./D.genTime);
    //hnPos.SetMaximum(80);
    hnPos.Print("Col",outpath+"/nCapPos");
    
    gPad->SetLogz(true);
    hIPos.ScaleBinsize();
    hIPos.Scale(1./D.genTime);
    //hIPos.SetMinimum(100);
    //hIPos.SetMaximum(100000);
    hIPos.Print("Col Z",outpath+"/IoniPos");
    
    return 0;
}
