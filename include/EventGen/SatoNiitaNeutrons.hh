#ifndef SATONIITANEUTRONS_HH
#define SATONIITANEUTRONS_HH

#include <G4SystemOfUnits.hh>

/// Implementation of neutron spectra from Sato, Niita, RADIATION RESEARCH 166, 544–555 (2006)
/// which parametrizes simulation results for <20km altitudes
class SatoNiitaNeutrons {
public:
    
    /// Constructor
    SatoNiitaNeutrons() { setParameters(1.*GV, 4.*GeV, 1035*g/cm2); }
    
    /// Set solar modulation potential ss, cutoff rigidity rc, atmospheric depth d
    void setParameters(double ss, double rc, double d);
    
    /// Calcuate energy-dependent spectrum terms at energy E
    double calcAirSpectrum(double E);
    /// Calculate ground-level spectrum, for water weight fraction w (calls calcAirSpectrum)
    double calcGroundSpectrum(double E, double w);
    
    double phi_L;       ///< Low-energy neutron flux (constant in E)
    double phi_B;       ///< "basic" neutron spectrum shape, 1/Lethargy
    
    double phi_inf;     ///< semi-infinite atmospheric flux, 1/Lethargy
    double phi_T;       ///< thermal neutron spectrum
    double f_G;         ///< ground enhancement factor
    double phi_G;       ///< ground-level spectrum
    
protected:  
    const double GV = megavolt; /// convenience shorthand for GigaVolt unit
    
    const double s_max = 1.700*GV;      ///< solar modulation potential maximum
    const double s_min = 0.465*GV;      ///< solar modulation potential minimum
    const double E_T = 0.025*eV;        ///< thermal neutron energy

    // Table 1, except b_i2mn[3], which has been changed from 0.292 to -0.292         | Table 3
    //                        0  1           2               3        4               | 5         6               7      8              9         10              11        12
    /***/ double a[13]    = { 0, 0.,         0.,             0.,      0.,               0.,       1.71e-4*cm2/g,  0.530, 0.00136*cm2/g, 0.,       0.,             0.,       0.0133*cm2/g };
    /***/ double b_i1[13] = { 0, 0.,         0.00706*cm2/g,  0.975,   0.00840*cm2/g,   -0.00701,  1.71e-4*cm2/g,  0.530, 0.00136*cm2/g, 642*MeV,  0.00112*cm2/g,  1.26,     0.0133*cm2/g };
    const double b_11mn   =      13.9/cm2/s;
    const double b_11mx   =      12.9/cm2/s;
    /***/ double b_i2[12] = { 0, 0,          0,              0,       0,                0.0258,    0,             0,     0,            -189*MeV,  1.81e-4*cm2/g, -0.958   };
    const double b_i2mn[5]= { 0, 25.5/cm2/s, 6.73e-4*cm2/g, -0.292,   0.00582*cm2/g };
    const double b_i2mx[5]= { 0, 15.7/cm2/s, 5.70e-4*cm2/g, -0.210,   0.00441*cm2/g };
    const double b_i3[12] = { 0, 5.62*GV,    5.99*GV,        0.99*GV, 2.24*GV,          10.9*GV,   0,             0,     0,             2.32*GV,  8.84*GV,        3.18*GV };
    const double b_i4[12] = { 0, 1.79*GV,    1.94*GV,        2.24*GV, 2.66*GV,          2.38*GV,   0,             0,     0,             0.897*GV, 0.587*GV,       1.47*GV };
    
    // Table 2 for c_i
    const double c_1 = 0.229;           // 1/Lethargy units
    const double c_2 = 2.31*MeV;
    const double c_3 = 0.5;             // Sato and Niita paper uses 0.721, but they recommend 0.5 for basic evaporation spectrum
    /***/ double c_4 = 0.0516;          // 1/Lethargy, redefined in Eq. (8)
    const double c_5 = 126.*MeV;
    const double c_6 = 2.17*MeV;
    const double c_7 = 0.00108;         // 1/Lethargy units
    const double c_8 = 3.33e-12*MeV;
    const double c_9 = 1.62;
    const double c_10 = 9.59e-8*MeV;
    const double c_11 = 1.48;
    /***/ double c_12 = 299*MeV;        ///< redefined in Eq. (9)
    
    // Table 4 for g
    const double g_1 = -0.0235;
    const double g_2 = -0.0129;
    /***/ double g_3;                   ///< from Eq. (12)
    const double g_4 = 0.969;
    /***/ double g_5;                   ///< from Eq. (13)
    /***/ double g_6;                   ///< from Eq. (16)
    
    // Table 5 for h
    const double h_31 = -25.2;
    const double h_32 =  2.73;
    const double h_33 =  0.0715;
    const double h_51 =  0.348;
    const double h_52 =  3.35;
    const double h_53 = -1.57;
    const double h_61 = 0.118;
    const double h_62 = 0.144;
    const double h_63 = 3.87;
    const double h_64 = 0.653;
    const double h_65 = 42.8;
};

#endif
