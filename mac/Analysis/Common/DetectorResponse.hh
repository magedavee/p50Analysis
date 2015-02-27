/// \file DetectorResponse.hh \brief Applies "detector response" to (HDF5) simulation output to produce data-matching result

#include "HDF5Reader.hh"

/// comparison function for time-sorting hits
bool compare_hit_times(const s_PhysPulse& a, const s_PhysPulse& b) { return a.t < b.t; }

const hsize_t n_PhysPulse_fields = 7;

/// element offsets for s_PhysPulse
const size_t PhysPulse_offsets[n_PhysPulse_fields] = {
    HOFFSET(s_PhysPulse,evt),
    HOFFSET(s_PhysPulse,seg),
    HOFFSET(s_PhysPulse,E),
    HOFFSET(s_PhysPulse,t),
    HOFFSET(s_PhysPulse,y),
    HOFFSET(s_PhysPulse,PSD),
    HOFFSET(s_PhysPulse,PID)
};

/// element sizes for s_PhysPulse
const size_t PhysPulse_sizes[n_PhysPulse_fields] = {
    sizeof(s_PhysPulse::evt),
    sizeof(s_PhysPulse::seg),
    sizeof(s_PhysPulse::E),
    sizeof(s_PhysPulse::t),
    sizeof(s_PhysPulse::y),
    sizeof(s_PhysPulse::PSD),
    sizeof(s_PhysPulse::PID)
};

const char* PhysPulse_field_names[n_PhysPulse_fields] = { "evt", "seg", "E", "t", "y", "PSD", "PID" };
const hid_t PhysPulse_field_types[n_PhysPulse_fields] = { H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT };

/// "Detector response" base class for converting s_IoniCluster to s_PhysPulse
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
