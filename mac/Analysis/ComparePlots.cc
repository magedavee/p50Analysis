#include "AnaUtils.hh"

void CompareMultiFiles() {
    vector<string> fnames;
    
    fnames.push_back("${PG4_OUTDIR}/P20_nBG_IBD/Plots/PROSPECT-2.root");
    fnames.push_back("${PG4_OUTDIR}/P20_WB_nBG_IBD/Plots/PROSPECT-2.root");
    fnames.push_back("${PG4_OUTDIR}/P20_2WB_nBG_IBD/Plots/PROSPECT-2.root");
    fnames.push_back("${PG4_OUTDIR}/P20_WBPb_nBG_IBD/Plots/PROSPECT-2.root");
    
    //fnames.push_back("${PG4_OUTDIR}/P20_muBG/Plots/PROSPECT-2.root");
    //fnames.push_back("${PG4_OUTDIR}/P20_WB_muBG/Plots/PROSPECT-2.root");
    
    //fnames.push_back("${PG4_OUTDIR}/P20_Aug28P/Plots/PROSPECT-2.root");
    //fnames.push_back("${PG4_OUTDIR}/P20_WB_Aug28P/Plots/PROSPECT-2.root");
    //fnames.push_back("${PG4_OUTDIR}/P20_WBPb_Aug28P/Plots/PROSPECT-2.root");
    
    vector<string> hnames;
    hnames.push_back("hIBDEnergy");
    //hnames.push_back("hSinglesE");
    
    for(auto ithn = hnames.begin(); ithn != hnames.end(); ithn++) {
        
        vector<TH1*> hs;
        double mx =0;
        for(size_t i=0; i<fnames.size(); i++) {
            cout << fnames[i] << "\n";
            TFile* f = new TFile(fnames[i].c_str(),"READ");
            assert(f);
            TH1* h = (TH1*)f->Get(ithn->c_str());
            assert(h);
            
            if(h->GetMaximum()>mx) mx = h->GetMaximum();
            hs.push_back(h);
            continue;
            
            char projname[1024];
            sprintf(projname,"px_%i",i);
            h = ((TH2F*)h)->ProjectionX(projname);
            string fitname = "fit_";
            fitname += projname;
            TF1* expFit = new TF1(fitname.c_str(),"expo",-20.,-0.25);
            expFit->SetLineColor(4-i);
            h->SetMaximum(4000);
            h->SetLineColor(4-i);
            h->Fit(expFit,"0R"); // "0" = do NOT automatically draw "hist"
            h->GetFunction(fitname.c_str())->ResetBit(TF1::kNotDraw); // make "projname" visible (= 1<<9)
            
        }
        
        for(size_t i=0; i<hs.size(); i++) {
            //hs[i]->SetLineColor(i? (i==1?4:2): 1);
            //hs[i]->SetLineColor(2+2*i);
            hs[i]->SetLineColor(1+i);
            
            hs[i]->SetMaximum(20);
            //hs[i]->SetTitle("gamma singles");
            //hs[i]->GetYaxis()->SetTitle("rate [arb. units]");
            //hs[i]->GetYaxis()->SetTitleOffset(1.5);
            
            //hs[i]->Draw(i?"HIST E1 X0 Same":"HIST E1 X0");
            hs[i]->Draw(i?"HIST Same":"HIST");
        }
        
        //gPad->SetLogy(true);
        //gPad->SetLogx(true);
        gPad->Print(("~/Desktop/"+*ithn+".pdf").c_str());
    }
}

void CompareMultiHistograms() {
    
    TFile* f = new TFile("$PG4_AUX/BGSpectra/Gammas.root","READ");
    
    vector<string> hnames;
    hnames.push_back("gamma_Jun30_unfold");
    hnames.push_back("gamma_Aug28P_unfold");

    TH1* hAxes = new TH1F("hAxes","ambient gamma spectrum", 10, 0, 8);
    hAxes->GetYaxis()->SetTitle("flux [Hz/MeV/cm^{2}]");
    hAxes->GetYaxis()->SetTitleOffset(1.35);
    hAxes->SetMaximum(1000);
    hAxes->Draw();
    
    int i = 0;
    for(auto ithn = hnames.begin(); ithn != hnames.end(); ithn++) {
        TH1* h = (TH1*)f->Get(ithn->c_str());
        assert(h);
        
        h->SetLineColor(2+2*i);
        normalize_to_bin_width(h);
        hAxes->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
        //hAxes->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
        h->Draw("HIST Same");
        
        i++;
    }
    
    //gPad->SetLogy(true);
    //gPad->SetLogx(true);
    gPad->Print("PlotCompare.pdf");
    
}

void ComparePlots() {
    gStyle->SetOptStat("");
    CompareMultiFiles();
    //CompareMultiHistograms();
}

int main(int, char**) { ComparePlots(); }
