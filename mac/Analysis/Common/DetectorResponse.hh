/// \file DetectorResponse.hh \brief Applies "detector response" to (HDF5) simulation output to produce data-matching result

#include "HDF5Reader.hh"
#include "TimeStructure.hh"
#include "IoniTiming.hh"

/// "Detector response" base class for converting s_IoniCluster to s_PhysPulse
class DetectorResponse: public ClustersWindow {
public:
    /// Constructor
    DetectorResponse() { window_time = 0; }
    /// Destructor
    virtual ~DetectorResponse() { }
    
    /// add ionization hit
    void addIoni(const s_IoniCluster& h);
    
    /// non-PSD-interpolated quenching calculation
    virtual double calcQuench(const s_IoniCluster& evt) const;
    /// combined PSD and quenching approximation calculation
    virtual void quenchPSD(const s_IoniCluster& evt, double& Equench, double& PSD) const;

    /// calculate detector response to ionization event
    virtual s_PhysPulse genResponse(const s_IoniCluster& evt) const;
    
    /// calculate pre-mapping PSD f(evt; a)
    virtual double psd_f(const s_IoniCluster& evt) const;
    
    vector<s_PhysPulse> event_response;         ///< detector response pulses generated
    size_t cellaxis = 2;                        ///< coordinate for cell long dimension; 2 for multi-cell PROSPECTS; 1 for P20 and DIMA
    bool P20reflectorless = false;              ///< special mode for "reflectorless" P20 volume merging
    
    double psd_a = 0.10;                        ///< PSD remapping stretch factor
    double PSD_gamma0 = 0.02;                   ///< input "gamma-like" PSD value
    double PSD_gamma = 0.22;                    ///< target "gamma-like" PSD value
    double PSD_ncapt0 = 0.935;                  ///< input neutron capture PSD value
    double PSD_ncapt = 0.363;                   ///< target neutron capture PSD value
    
protected:
    /// create new cluster object of appropriate sub-class type; will be managed in pool.
    virtual TimedCluster* makeCluster() { return new TimedCluster(20); }
    
    /// processing hook for each object as it passes through middle of window
    virtual void processMid(TimedObject* O);
};
