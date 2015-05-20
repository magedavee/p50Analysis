/// \file CalcQuenchingCurve.cc \brief Fitting for simulated quenching curve; see DocDB 345.

#include "AnaUtils.hh"
#include <Math/Minimizer.h>
#include <Math/Factory.h>
#include <Math/Functor.h>

// make CalcQuenchingCurve; ./CalcQuenchingCurve

// in MeV/mm / z^2 / n/(n_A/cm^3)
double bethe(double b, double I=100e-6) {
    double bb = b*b;
    return 0.0307/bb*(log(1.022*bb/(1-bb)/I)-bb);
}

double bethe_x(double x, double I=100e-6) { return bethe(sqrt(1.-1./(x+1)/(x+1)),I); }
double bethe_x_extended(double x) { return x>9.59e-5? bethe_x(x) : 5701.7*pow(x,0.4290); }
double bxe_sq(double x) { return pow(bethe_x_extended(x), 2); }

/// Simpson's Rule numerical integration
double nintegrate(double (*f)(double), double x1, double x2, int npts=1000) {
    double s = 0;
    for(int i=0; i<npts; i++) {
        double x = x1 + (x2-x1)*(i+0.5)/npts;
        double w = i%2? 2 : 4;
        if(i==0 || i==npts-1) w = 1;
        s += (*f)(x)*w;
    }
    return s*(x2-x1)/(3.*npts);
}
    
/// Minimizer class for quenching parameters fit
class QuenchMin {
public:
    /// Constructor
    QuenchMin() { 
        const double scint_edens = 0.524;
        
        for(int i=0; i<npts; i++) {
            double x = Etrue_p[i]/938.3;
            dEdx_p[i] = nintegrate(&bethe_x_extended,0,x)/x*scint_edens;
            dEdx2_p[i] = nintegrate(&bxe_sq,0,x)/x*pow(scint_edens,2);
            printf("E = %1f:\tproton dEdx = %g MeV/mm\tsqrt(dEdx2) = %g\n", Etrue_p[i], dEdx_p[i], sqrt(dEdx2_p[i]));
       
            x = Etrue_e[i]/0.511;
            dEdx_e[i] = nintegrate(&bethe_x_extended,0,x)/x*scint_edens;
            dEdx2_e[i] = nintegrate(&bxe_sq,0,x)/x*pow(scint_edens,2);
            printf("E = %1f:\telectron dEdx = %g MeV/mm\tsqrt(dEdx2) = %g\n", Etrue_e[i], dEdx_e[i], sqrt(dEdx2_e[i]));
            
            Etrue_C[i] = 0.5 + 1.5*i;
            double x_C = Etrue_C[i]/12000.;
            dEdx_C[i] = 6*nintegrate(&bethe_x_extended, 0, x_C)/x_C*scint_edens;
            dEdx2_C[i] = 36*nintegrate(&bxe_sq, 0, x_C)/x_C*scint_edens;
            printf("C12 dEdx = %g MeV/mm\tsqrt(dEdx2) = %g\n", dEdx_C[i], sqrt(dEdx2_C[i]));
        }
        
        double x_a = E_a/3727.;
        dEdx_a = 4*nintegrate(&bethe_x_extended, 0, x_a)/x_a*scint_edens;
        dEdx2_a = 16*nintegrate(&bxe_sq, 0, x_a)/x_a*scint_edens;
        printf("Alpha dEdx = %g MeV/mm\tsqrt(dEdx2) = %g\n", dEdx_a, sqrt(dEdx2_a));
        
        double x_t = E_t/2809.;
        dEdx_t = nintegrate(&bethe_x_extended, 0, x_t)/x_t*scint_edens;
        dEdx2_t = nintegrate(&bxe_sq, 0, x_t)/x_t*scint_edens;
        printf("Triton dEdx = %g MeV/mm\tsqrt(dEdx2) = %g\n", dEdx_t, sqrt(dEdx2_t));

    }

    static const int npts = 11; ///< number of proton recoil datapoints
    /// proton recoil datapoints true energy
    double Etrue_p[npts] =        {1,      1.5,    2,      2.5,    3,      3.5,    4,      4.5,    5,      5.5,    6 };
    double dEdx_p[npts];          ///< <dE/dx> for each proton point
    double dEdx2_p[npts];         ///< <(dE/dx)^2> for each proton point
    
    double Etrue_e[npts] =        { 0.5, 1, 2, 3, 4.9, 7, 9, 12, 15, 19, 23 };
    double dEdx_e[npts];          ///< <dE/dx> for each electron point
    double dEdx2_e[npts];         ///< <(dE/dx)^2> for each electron point
    
    const double E_a = 2.05;    ///< alpha kinetic energy
    const double E_t = 2.73;    ///< triton kinetic energy
    double dEdx_a, dEdx2_a, dEdx_t, dEdx2_t;    ///< alpha, triton energy deposition parameters
    double Etrue_C[npts];
    double dEdx_C[npts], dEdx2_C[npts];         ///< heavy nucleus recoil deposition
    const double ncaptE = 0.55;                 ///< visible energy for 6Li neutron capture peak [MeVee]
    
    /// proton visible energy empirical parametrization
    double Evis_p(double E) const { return 0.838*E*E/(E+6.19); }
    //double Evis(double E) const { return 0.748*E-2.41*(1-exp(-0.298*E)); }
    
    /// 2-parameter empirical fit form quenching factor
    //double qfactor(double d, double dd, const double* x) const { return 1./(1 + x[0]*d + x[1]*dd); }
    
    /// 3-parameter empirical fit form quenching factor
    //double qfactor(double d, double dd, const double* x) const { return (1. + x[1]*d*d*d)/(1 + x[0]*d + x[2]*d*d); }
    
    /// 4-parameter empirical fit form quenching factor
    double qfactor(double d, const double* x) const { return (1. + x[3]*d*d)/(1 + x[0]*d + x[1]*d*d + x[2]*d*d*d); }
    
    /// Quenched energy of 6Li alpha + triton
    double alphatritonquench(const double* x) const { return E_t*qfactor(dEdx_t, x) + E_a*qfactor(dEdx_a, x); }
    
    /// Minimization function for quenching fit
    double quenchMin(const double* x) {
        double s_err = 0;
        
        // proton points
        for(int i=0; i<npts; i++) {
            double Ev = Evis_p(Etrue_p[i]); 
            double Eq = Etrue_p[i]*qfactor(dEdx_p[i], x);
            s_err += 10*(Ev-Eq)*(Ev-Eq)/(Etrue_p[i]*Etrue_p[i]*Etrue_p[i]);
        }
        
        // electron points
        for(int i=0; i<npts; i++) {
            double Eq = Etrue_e[i]*qfactor(dEdx_e[i], x);
            s_err += 0.2*(Etrue_e[i]-Eq)*(Etrue_e[i]-Eq);
        }
        
        // 12C points
        for(int i=0; i<npts; i++) {
            if(i != 2) continue;
            double Ev = Etrue_C[i]*0.4/6.; 
            double Eq = Etrue_C[i]*qfactor(dEdx_C[i], x);
            s_err += 10*(Ev-Eq)*(Ev-Eq);
        }
        
        // alpha, triton heavily weighted
        double Eq = alphatritonquench(x);
        s_err += 100*(ncaptE-Eq)*(ncaptE-Eq);
        
        return s_err;
    }
    
    void plotFit(const double* x) {
        TGraph g1(npts);
        TGraph g2(npts);
        TGraph ge(npts);
        TGraph ge_q(npts);
        TGraph gC(npts);
        TGraph gC_q(npts);
        for(int i=0; i<npts; i++) {
            g1.SetPoint(i, Etrue_p[i], Evis_p(Etrue_p[i]));
            g2.SetPoint(i, Etrue_p[i], Etrue_p[i]*qfactor(dEdx_p[i], x));
            ge.SetPoint(i, Etrue_e[i], Etrue_e[i]);
            ge_q.SetPoint(i, Etrue_e[i], Etrue_e[i]*qfactor(dEdx_e[i], x));
            gC.SetPoint(i, Etrue_C[i], Etrue_C[i]*0.4/6.);
            gC_q.SetPoint(i, Etrue_C[i], Etrue_C[i]*qfactor(dEdx_C[i], x));
        }
        g1.SetTitle("Quenching estimate");
        g1.SetMarkerStyle(22);
        g1.SetMaximum(5);
        g1.Draw("AP");
        //gPad->SetLogy(true);
        g1.GetXaxis()->SetRangeUser(0,10);
        g1.Draw("AP");
        g1.GetXaxis()->SetTitle("True energy [MeV]");
        g1.GetYaxis()->SetTitle("Observed energy [MeVee]");
        g2.SetLineStyle(2);
        g2.Draw("C");
        gC_q.SetLineStyle(3);
        gC_q.Draw("C");
        
        ge.Draw("L");
        ge_q.SetMarkerStyle(20);
        ge_q.Draw("P");
        gC.SetMarkerStyle(33);
        gC.Draw("P");
        
        TGraph g3(2);
        g3.SetPoint(0, E_a+E_t, ncaptE);
        g3.SetPoint(1, E_a+E_t, alphatritonquench(x));
        printf("alpha+triton reconstructed at %.2f + %.2f = %.2f\n", 
               E_a*qfactor(dEdx_a, x), E_t*qfactor(dEdx_t, x), alphatritonquench(x));
        g3.SetMarkerStyle(32);
        g3.Draw("P");
        
        gPad->Print("QuenchedFit.pdf");
        
        
        TGraph gq_e;
        TGraph gq_p;
        TGraph gq_C;
        for(int i=0; i<npts; i++) {
            gq_e.SetPoint(i, dEdx_e[i], 1.);
            gq_p.SetPoint(i, dEdx_p[i], Evis_p(Etrue_p[i])/Etrue_p[i]);
            gq_C.SetPoint(i, dEdx_C[i], 0.0666);
        }
        TGraph gq_at;
        gq_at.SetPoint(0, dEdx_t, qfactor(dEdx_t, x));
        gq_at.SetPoint(1, dEdx_a, qfactor(dEdx_a, x));
        gq_at.SetMarkerStyle(32);
        
        TGraph gqq;
        for(int i=0; i<101; i++) {
            double l = i/100.;
            double dedx = exp(log(0.2)*(1-l) + log(1000)*l);
            gqq.SetPoint(i, dedx, qfactor(dedx, x));
        }
        
        gqq.SetTitle("Quenching curve fit");
        gqq.Draw("AC");
        gqq.GetXaxis()->SetTitle("dE/dx [MeV/mm]");
        gqq.GetXaxis()->SetRangeUser(0.2, 300);
        gqq.GetYaxis()->SetTitle("quenching factor");
        gPad->SetLogx(true);
        
        gq_e.SetMarkerStyle(20);
        gq_p.SetMarkerStyle(22);
        gq_C.SetMarkerStyle(33);
        
        gq_p.Draw("P");
        gq_e.Draw("P");
        gq_at.Draw("P");
        gq_C.Draw("P");
        gPad->Print("QuenchFactor.pdf");
    }
};



int fitQuenchData() {
    // Choose method upon creation between:
    // kConjugateFR, kConjugatePR, kVectorBFGS,
    // kVectorBFGS2, kSteepestDescent
    
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "");
    assert(min);

    
    min->SetMaxFunctionCalls(1000000);
    min->SetMaxIterations(100000);
    min->SetTolerance(0.00001);
    
    QuenchMin QM;
    const int nvar = 4;
    ROOT::Math::Functor f(&QM,&QuenchMin::quenchMin,nvar); 
    double step[] = {0.0001, 0.0001, 1e-9, 1e-11 };
    double variable[] = { -0.0026114, 0.00416283, 1.11055e-08, 0 };
    min->SetFunction(f);
    
    // Set the free variables to be minimized!
    min->SetVariable(0,"x",variable[0], step[0]);
    if(nvar >= 2) min->SetVariable(1,"y",variable[1], step[1]);
    if(nvar >= 3) min->SetVariable(2,"z",variable[2], step[2]);
    if(nvar >= 4) min->SetVariable(3,"t",variable[3], step[3]);
    
    min->Minimize(); 
    
    const double *xs = min->X();
    cout << "Minimum: f( ";
    for(int i=0; i<nvar; i++) cout << xs[i] << " ";\
    cout << "): " << QM.quenchMin(xs) << "\n";
    QM.plotFit(xs);
    
    return 0;
}

int main(int, char**) {
    return fitQuenchData();
}
