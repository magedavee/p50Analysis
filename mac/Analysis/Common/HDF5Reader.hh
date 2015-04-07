/// \file HDF5Reader.hh \brief Readers for simulation HDF5 data
#ifndef HDF5READER_HH
#define HDF5READER_HH

#include "AnaUtils.hh"
#include "HDF5_IO.hh"

/// Simulation reader and time-series merger
class SimIoniReader: protected HDF5_StructInfo {
public:
    /// Constructor
    SimIoniReader(const string& f_in);
    /// Destructor
    virtual ~SimIoniReader();
    
    /// Load next single ionization record from file; return whether event good, or file finished.
    bool loadIoni();
    /// Load next merged event from file; return whether event good, or file finished.
    bool loadMergedIoni();
    
    s_IoniCluster ioni;                 ///< next un-merged event loaded
    vector<s_IoniCluster> merged;       ///< most recently completed merged event
    double dt_max = 50;                 ///< time to separate events [ns]
    hsize_t nRead = 0;                  ///< number of records read so far
    hsize_t nrecords = 0;               ///< number of table records
    bool P20reflectorless = false;      ///< special mode for merging reflectorless P20 outer volume events
 
protected:
    HDF5_Table_Cache<s_IoniCluster> ioni_reader;       ///< cached output for s_IoniCluster
    Long64_t current_evt = 0;           ///< current event number
    hid_t infile_id = 0;                ///< input HDF5 file ID
    hsize_t nfields = 0;                ///< number of table fields
};

#endif
