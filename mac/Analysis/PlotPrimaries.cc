///////////////////////////////////////////////////////////
// Standalone program for plotting MC primaries information

#include "AnaUtils.hh"

/// Collection of histograms to generate for a primary particle type
class PrimaryHistograms {
public:
    /// Constructor
    PrimaryHistograms(FileKeeper& F, int pid): PID(pid) {
        string pn = to_str(PID);
        
        p_dir = (TH2F*)F.add(new TH2F(("prim_p_"+pn).c_str(), "Momentum direction", 100,-1.2,1.2, 100,-1.2,1.2));
        
        Espec = (TH1F*)F.add(logHist("Espec_"+pn, "Energy spectrum", 300, 1e-6, 1e8));
        Espec->GetXaxis()->SetTitle("kinetic energy [MeV]");
        Espec->GetYaxis()->SetTitle("rate [Hz/bin]");
        Espec->GetYaxis()->SetTitleOffset(1.45);
    }
    
    /// Fill from primaries data
    void Fill(ParticleVertex* pp) {
        p_dir->Fill(pp->p[0], pp->p[1]);
        Espec->Fill(pp->E);
    }
    
    /// Process and draw
    void Draw(const string& outpath, double t) {
        
        string pn = to_str(PID);
        
        gStyle->SetOptStat("");
        
        Espec->Scale(1./t);
        Espec->Draw();
        gPad->SetLogx(true);
        gPad->SetLogy(true);
        gPad->Print((outpath+"/Espec_"+pn+".pdf").c_str());
       
        if(0) {
            gPad->SetLogy(false);
            gPad->SetCanvasSize(700,700);
            
            p_dir->Draw("Col Z");
            gPad->Print((outpath+"/p_Direction_"+pn+".pdf").c_str());
        }

    }
    
    int PID;            ///< PDG particle ID
    TH2F* p_dir;        ///< momentum direction
    TH1F* Espec;        ///< Energy spectrum
};

int main(int argc, char** argv) {
    // load library describing data classes
    gSystem->Load("libEventLib.so");
    
    string inPath = ".";
    if(argc == 2) inPath = argv[1];
    string outpath = inPath + "/Plots/";
    
    mkdir(outpath.c_str(), 0755);
    FileKeeper f(outpath+"PrimariesOut.root");
    
    // load data into TChain
    OutDirLoader D(inPath);
    TChain* T = D.makeTChain();
    // set readout branches
    ParticleEvent* prim = new ParticleEvent();
    T->SetBranchAddress("Prim",&prim);
    
    map<Int_t, PrimaryHistograms> primHists;
    
    // scan events
    
    Long64_t nentries = T->GetEntries();
    map<int,int> primSingles;
    map<map<int,int>, int> primCombos;
    std::cout << "Scanning " << nentries << " events...\n";
    for (Long64_t ev=0; ev<nentries; ev++) {
        prim->Clear();
        T->GetEntry(ev);
        
        // primaries
        map<int,int> primCounter;
        Int_t nPrim = prim->particles->GetEntriesFast();
        for(Int_t i=0; i<nPrim; i++) {
            ParticleVertex* pp = (ParticleVertex*)prim->particles->At(i);
            Int_t PID = pp->PID;
            primCounter[PID]++;
            primSingles[PID]++;
            map<Int_t, PrimaryHistograms>::iterator it = primHists.find(PID);
            if(it == primHists.end())
                it = primHists.insert(std::pair<Int_t, PrimaryHistograms>(PID,PrimaryHistograms(f,PID))).first;
            it->second.Fill(pp);
        }
        primCombos[primCounter]++;
    }
    cout << "\n\nSingles:\n";
    display_map(primSingles);
    cout << "\n\nCombinations:\n";
    display_map(primCombos,-1,1e-4);
    
    // produce output
    for(map<Int_t, PrimaryHistograms>::iterator it = primHists.begin(); it !=primHists.end(); it++)
        it->second.Draw(outpath, D.genTime);

    return 0;
}