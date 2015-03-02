/// \file AnalysisTypes.hh \brief Core structs for data/simulation analysis
#ifndef ANALYSISTYPES_HH
#define ANALYSISTYPES_HH

#include <Rtypes.h> // ROOT data types

// ///////////////////////
// Simulation truth values
// ///////////////////////

/// Simulated particle vertex information
struct s_ParticleVertex {
    Int_t PID = 0;      ///< PDG particle ID code
    Double_t x[3];      ///< vertex position
    Double_t p[3];      ///< momentum direction
    Double_t E = 0;     ///< kinetic energy
    Double_t t = 0;     ///< initial time
    Long64_t evt = 0;   ///< event number
};

/// underlying struct for NCapt data
struct s_NCapt {
    Double_t t = 0;     ///< time of capture
    Double_t E = 0;     ///< kinetic energy at capture
    Double_t x[3];      ///< position of capture
    Int_t Ngamma = 0;   ///< number of gammas produced
    Double_t Egamma = 0;///< total energy of gammas produced
    Int_t Nprod = 0;    ///< total number of secondary products
    Int_t capt_A = 0;   ///< capture nucleus A
    Int_t capt_Z = 0;   ///< capture nucleus Z
    Int_t vol = 0;      ///< volume
    Long64_t evt = 0;   ///< event number
};

/// Simulation event info
struct s_Event {
    Long64_t N = 0;     ///< event number
    Double_t t = 0;     ///< event time [ns]
    Double_t ct = 0;    ///< computer time to calculate event
    Int_t flg = 0;      ///< event flags
};

// ////////////////////////
// Data or Simulation types
// ////////////////////////

/// Pulse shape analysis information for a single digitized detector pulse
struct s_DetPulse {
    Long64_t evt = 0;   ///< DAQ event/trigger number
    Int_t det = 0;      ///< detector ID
    Double_t t = 0;     ///< arrival time [ns]
    Float_t h = 0;      ///< height [ADC counts]
    Float_t a = 0;      ///< area [ADC sum]
    Float_t b = 0;      ///< baseline [ADC counts]
    Float_t rise = 0;   ///< rise time [ns]
    Float_t PSD = 0;    ///< PSD parameter [arb.]
};

/// "Physics" calibrated data/simulation, indicating energy deposition in one detector segment
struct s_PhysPulse {
    Long64_t evt = 0;   ///< DAQ/sim event number
    Int_t seg = 0;      ///< segment/channel ID number
    Double_t E = 0;     ///< deposited energy [MeVee]
    Double_t t = 0;     ///< event time [ns] since run start
    Double_t y = 0;     ///< position along scintillator long axis [mm] 
    Double_t PSD = 0;   ///< Pulse-shape-discrimination value [arb]
    Int_t PID = 0;      ///< Particle ID flags
};

/// hit PID classifications that can be used in s_PhysPulse
enum HitTypeID {
    CRAZY_HIT = 0,      ///< unclassifiable event
    IONI_HIT = 1,       ///< electron/gamma/muon ionization
    NCAPT_HIT = 2,      ///< neutron capture (on 6Li)
    RECOIL_HIT = 3,     ///< nucleus recoil (fast neutron)
    VETO_HIT = 4,       ///< veto detector hit
    DEAD_HIT = 5        ///< dead volume or non-detected hit (simulation only)
};

#endif
