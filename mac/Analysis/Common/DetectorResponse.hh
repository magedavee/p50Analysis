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
    
    /// quenched energy approximation
    virtual double Equench(const s_IoniCluster& evt) const;
    /// PSD approximation
    virtual double PSD(const s_IoniCluster& evt) const;
    /// calculate detector response to ionization event
    virtual s_PhysPulse genResponse(const s_IoniCluster& evt) const;
};
