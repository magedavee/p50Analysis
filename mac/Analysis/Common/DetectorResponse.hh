/// \file DetectorResponse.hh \brief Applies "detector response" to (HDF5) simulation output to produce data-matching result

#include "HDF5Reader.hh"

/// comparison function for time-sorting hits
bool compare_hit_times(const s_PhysPulse& a, const s_PhysPulse& b) { return a.t < b.t; }

/// "Detector response" base class for converting s_IoniCluster to s_PhysPulse
class DetectorResponse {
public:
    /// Constructor
    DetectorResponse() { }
    /// Destructor
    virtual ~DetectorResponse() { }
    
    /// combined PSD and quenching approximation calculation
    virtual void quenchPSD(const s_IoniCluster& evt, double& Equench, double& PSD) const;

    /// calculate detector response to ionization event
    virtual s_PhysPulse genResponse(const s_IoniCluster& evt) const;
};
