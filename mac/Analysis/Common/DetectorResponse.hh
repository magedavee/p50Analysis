/// \file DetectorResponse.hh \brief Applies "detector response" to (HDF5) simulation output to produce data-matching result

#include "HDF5Reader.hh"

/// "Physics" calibrated pulse data or simulation struct
struct s_PhysPulse {
    Long64_t evt = 0;   ///< DAQ/sim event number
    Int_t seg = 0;      ///< segment ID number
    Double_t E = 0;     ///< deposited energy [MeV]
    Double_t t = 0;     ///< event time [ns]
    Double_t y = 0;     ///< position along scintillator long axis, from timing [mm] 
    Double_t PSD = 0;   ///< Pulse-shape-discrimination value [arb]
};

const hsize_t n_PhysPulse_fields = 6;

/// element offsets for s_PhysPulse
const size_t PhysPulse_offsets[n_PhysPulse_fields] = {
    HOFFSET(s_PhysPulse,evt),
    HOFFSET(s_PhysPulse,seg),
    HOFFSET(s_PhysPulse,E),
    HOFFSET(s_PhysPulse,t),
    HOFFSET(s_PhysPulse,y),
    HOFFSET(s_PhysPulse,PSD)
};

/// element sizes for s_PhysPulse
const size_t PhysPulse_sizes[n_PhysPulse_fields] = {
    sizeof(s_PhysPulse::evt),
    sizeof(s_PhysPulse::seg),
    sizeof(s_PhysPulse::E),
    sizeof(s_PhysPulse::t),
    sizeof(s_PhysPulse::y),
    sizeof(s_PhysPulse::PSD)
};

const char* PhysPulse_field_names[n_PhysPulse_fields] = { "evt", "seg", "E", "t", "y", "PSD" };
const hid_t PhysPulse_field_types[n_PhysPulse_fields] = { H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE };

/// "Detector response" base class
class DetectorResponse {
public:
    /// Constructor
    DetectorResponse() { }
    /// Destructor
    virtual ~DetectorResponse() { }
    
    /// quenched energy approximation
    virtual double Equench(const s_IoniCluster& evt) const;
    /// PSD approximation
    virtual double PSD(const s_IoniCluster& evt) const;
    /// calculate detector response to ionization event
    virtual s_PhysPulse genResponse(const s_IoniCluster& evt) const;
};
