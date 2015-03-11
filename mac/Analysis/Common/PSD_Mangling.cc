#include "HDF5Reader.hh"
#include "XMLInfo.hh"
#include <cmath>

// make PSD_Mangling -j4; ./PSD_Mangling; evince InputPSD.pdf 

double Equench(const s_IoniCluster& evt) {
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    return evt.E / (1 + c_1*evt.EdEdx/evt.E + c_2*evt.EdEdx2/evt.E);
}

TH1* loadPSD(const string& basedir) {
    
    // input PSD histogram
    double k0 = 0.2;
    TH1F* inputPSD = new TH1F(("inputPSD_"+basedir).c_str(), "Simulation PSD proxy", 100, 0, 1);
    inputPSD->GetXaxis()->SetTitle("2/#pi atan(dE/dx)"); 
    inputPSD->GetYaxis()->SetTitle("rate [mHz/bin]"); 
    
    OutDirLoader ODL(getEnvSafe("PG4_OUTDIR") + "/" + basedir);
    auto flist = ODL.getFileList(".h5");
    double totalTime = 0;
    for(auto itf = flist.begin(); itf != flist.end(); itf++) {
        
        // open input file
        SimIoniReader SIR(*itf);
        XMLInfo XI(*itf+".xml");
        double runtime = 1e-9*XI.getGenTime(); // [ns] -> [s]
        printf("Input file for %.1f seconds simulated time.\n", runtime);
        totalTime += runtime;
        
        // load PSD for neutron capture energy region
        while(SIR.loadMergedIoni()) {
            if(!(SIR.nRead % (SIR.nrecords/20))) { cout << "*"; cout.flush(); }
            for(auto it = SIR.merged.begin(); it != SIR.merged.end(); it++) {
                if(it->vol < 0 || it->vol >= 1000) continue; // main scintillator only
                double Eq = Equench(*it);
                if(Eq < 0.4 || Eq > 0.6) continue; // neutron capture region only
                //if(!(Eq < 0.4 || Eq > 0.6)) continue;
                inputPSD->Fill(atan(k0*it->EdEdx/it->E)*2/M_PI); 
            }
        }
    }
    
    inputPSD->Scale(1000./totalTime);
    return inputPSD;
}

int main(int, char**) {
    TH1* hMuBG = loadPSD("P2B_muBG");
    TH1* hNBG = loadPSD("P2B_nBG");
   
    hNBG->SetLineColor(2);
    hMuBG->Draw();
    hNBG->Draw("Same");
    gPad->Print("InputPSD.pdf");
    
    // calculate function to convert sim to data PSD
    TGraph gConvert(2);
    
    return 0;
}

