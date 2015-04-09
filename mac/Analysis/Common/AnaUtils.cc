#include "AnaUtils.hh"
#include <TArray.h>
#include <TMath.h>
#include <algorithm>

bool isIsolatedSegment(const map<Int_t,double>& volIoni) {
    int nThresh = 0;
    int nHi = 0;
    for(auto it = volIoni.begin(); it != volIoni.end(); it++) {
        nThresh += it->second > 0.2;
        nHi += it->second > 20;
    }
    return (nThresh == 1 && nHi == 0);
}

void scale_times_bin(TH1* f) {
    for(int i=1; i<=f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = sqrt(A->GetBinLowEdge(i)*A->GetBinUpEdge(i));
        f->SetBinContent(i, f->GetBinContent(i)*scale);
        f->SetBinError(i, f->GetBinError(i)*scale);
    }
    f->Scale(1.0);
}

void normalize_to_bin_width(TH1* f, double xscale) {
    for(int i=1; i<=f->GetNbinsX(); i++) {
        TAxis* A = f->GetXaxis();
        double scale = 1./A->GetBinWidth(i);
        f->SetBinContent(i, f->GetBinContent(i)*scale);
        f->SetBinError(i, f->GetBinError(i)*scale);
    }
    f->Scale(xscale);
}

TH1F* logHist(const string& n, const string& descrip, unsigned int nbins, double bmin, double bmax) {
    vector<Double_t> binEdges(nbins+1);
    for(unsigned int i=0; i<=nbins; i++)
        binEdges[i] = exp((nbins-i)*log(bmin)/nbins + i*log(bmax)/nbins);
    return new TH1F(n.c_str(), descrip.c_str(), nbins, &binEdges[0]);
}

void fill_interp(TH1* h, double x, double w) {
    TAxis* Ax = h->GetXaxis();
    int b0 = Ax->FindBin(x);
    if(b0 < 1 || b0 > h->GetNbinsX()) { h->Fill(x); return; }
    double c0 = Ax->GetBinCenter(b0);
    int b1 = x > c0? b0+1 : b0-1;
    double c1 = Ax->GetBinCenter(b1);
    double a = (c1-x)/(c1-c0);
    h->Fill(c0, a*w);
    h->Fill(c1, (1-a)*w);
}

void describe_event(TClonesArray* clusts, TClonesArray* nCapts) {
    Int_t nIoni = clusts? clusts->GetEntriesFast() : 0;
    if(nIoni) printf("Event with %i ionization clusters\n------- vol\tPID\tKE\tt\n", nIoni);
    for(Int_t i=0; i<nIoni; i++) {
        IoniCluster* ei = dynamic_cast<IoniCluster*>(clusts->At(i));
        printf("\t%i\t%i\t%.3f\t%f\n",ei->vol,ei->PID,ei->E,ei->t);
    }
    Int_t nNCapt = nCapts? nCapts->GetEntriesFast() : 0;
    if(nNCapt) printf("-nCapt- A\tZ\tEgamma\tNprod\tt\n");
    for(Int_t i=0; i<nNCapt; i++) {
        NCapt* nc = dynamic_cast<NCapt*>(nCapts->At(i));
        string ntp = (nc->capt_A == 1 && nc->capt_Z == 0)? "new" :
                     (nc->capt_A == 2 && nc->capt_Z == 1)? "1H" :
                     (nc->capt_A == 7 && nc->capt_Z == 3)? "6Li" :
                     (nc->capt_A == 36 && nc->capt_Z == 17)? "35Cl" : "";
        printf("%s\t%i\t%i\t%.2f\t%i\t%f\n", ntp.c_str(), nc->capt_A, nc->capt_Z, nc->Egamma, nc->Nprod, nc->t);
    }
    if(nNCapt+nIoni) printf("--------------------------\n");
}

void printHisto(const TH1* h) {
    printf("----- Histogram '%s' -----\nx: '%s'\ny: '%s'\nbin_lo\tbin_hi\tcontents\n---------------------------\n",
           h->GetName(), h->GetXaxis()->GetTitle(), h->GetYaxis()->GetTitle());
    const TAxis* A = h->GetXaxis();
    for(int i=1; i<=h->GetNbinsX(); i++)
        printf("%.5g\t%.5g\t%.5g\n",A->GetBinLowEdge(i),A->GetBinUpEdge(i),h->GetBinContent(i));
}

double integralAndError(TH1* h, double x0, double x1, Double_t& err, const string& option) {
    if(!h) {
        err = 0;
        return 0;
    }
    return h->IntegralAndError(h->FindBin(x0), h->FindBin(x1), err, option.c_str());
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////


ProfileHistos::ProfileHistos(int nbins, double r, const string& n, const string& ttl, const string& u) {
    h_xy = new TH2F((n+"_xy").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_xy->GetXaxis()->SetTitle(("x position "+u).c_str());
    h_xy->GetYaxis()->SetTitle(("y position "+u).c_str());
    h[0] = h_xy;
        
    h_xz = new TH2F((n+"_xz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_xz->GetXaxis()->SetTitle(("x position "+u).c_str());
    h_xz->GetYaxis()->SetTitle(("z position "+u).c_str());
    h[1] = h_xz;
    
    h_yz = new TH2F((n+"_yz").c_str(), ttl.c_str(), nbins,-r,r, nbins,-r,r);
    h_yz->GetXaxis()->SetTitle(("y position "+u).c_str());
    h_yz->GetYaxis()->SetTitle(("z position "+u).c_str());
    h[2] = h_yz;
    
    for(int i=0; i<3; i++) {
        h[i]->GetYaxis()->SetTitleOffset(1.4);
        hProf[i] = NULL;
    }
}

void ProfileHistos::Fill(double x, double y, double z, double w) {
    h_xy->Fill(x,y,w);
    h_xz->Fill(x,z,w);
    h_yz->Fill(y,z,w);
}

void ProfileHistos::Print(const char* opt, const string& bpath) {
    h_xy->Draw(opt);
    gPad->Print((bpath+"_xy.pdf").c_str());
    h_xz->Draw(opt);
    gPad->Print((bpath+"_xz.pdf").c_str());
    h_yz->Draw(opt);
    gPad->Print((bpath+"_yz.pdf").c_str());
}

void ProfileHistos::makeProf() {
    hProf[0] = h_xy->ProjectionX();
    hProf[1] = h_yz->ProjectionX();
    hProf[2] = h_xz->ProjectionY();
    for(int i=0; i<3; i++) {
        hProf[i]->Scale(1./hProf[i]->GetXaxis()->GetBinWidth(1));
        hProf[i]->SetLineColor(2+i);
    }
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////

TH1* poisson_smear(const TH1& hIn, double NperX, TH1* hOut) {
    if(!hOut) {
        hOut = (TH1*)hIn.Clone((string(hIn.GetName())+"_Smeared").c_str());
        hOut->Reset();
    }
    for(int i=1; i<=hIn.GetNbinsX(); i++) {
        double c0 = hIn.GetBinContent(i);
        if(!c0) continue;
        double X = hIn.GetBinCenter(i);
        double n0 = X*NperX;
        double nrm = 0;
        for(int j=1; j<=hOut->GetNbinsX(); j++) nrm += TMath::Poisson(hOut->GetBinCenter(j)*NperX, n0);
        for(int j=1; j<=hOut->GetNbinsX(); j++) {
            double X1 = hOut->GetBinCenter(j);
            hOut->Fill(X1, c0 * TMath::Poisson(X1*NperX, n0)/nrm);
        }
    }
    return hOut;
}
  