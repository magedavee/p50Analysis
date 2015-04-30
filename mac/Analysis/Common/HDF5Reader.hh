/// \file HDF5Reader.hh \brief Readers for simulation HDF5 data
#ifndef HDF5READER_HH
#define HDF5READER_HH

#include "AnaUtils.hh"
#include "HDF5_IO.hh"

/// Simulation reader and time-series merger for ionization events
class SimIoniReader: protected HDF5_StructInfo {
public:
    /// Constructor
    SimIoniReader(const string& f_in);
    /// Destructor
    virtual ~SimIoniReader();
    
    /// get number of ionization records read
    hsize_t getNRead() const { return ioni_reader.getNRead(); }
    /// get recorded table attribute
    double getAttr(const string& tbl, const string& attr);
    /// get run time
    double getRuntime() { return getAttr("ScIoni", "runtime"); }
    
    /// Load next merged event from file; return whether event good, or file finished.
    bool loadMergedIoni();
    vector<s_IoniCluster> merged;       ///< most recently completed merged ionization event
    
    double dt_max = 50;                 ///< time to separate events [ns]
    hsize_t nrecords = 0;               ///< number of table records
 
    HDF5_Table_Cache<s_IoniCluster> ioni_reader;        ///< cached output for s_IoniCluster
    HDF5_Table_Cache<s_ParticleVertex> prim_reader;     ///< event primaries information reader
    HDF5_Table_Cache<s_NCapt> ncapt_reader;             ///< neutron capture information reader

protected:    
    hid_t infile_id = 0;                ///< input HDF5 file ID
    hsize_t nfields = 0;                ///< number of table fields
};

/// PROSPECT-G4 HDF5 event sub-selection file transfer
class SimEventSelector: protected HDF5_StructInfo {
public:
    /// Constructor
    SimEventSelector();
    /// Setup output file
    void setOutfile(const string& filename);
    /// Set input file
    void setInfile(const string& filename);
    /// Transfer events in (sorted ascending) list
    void transfer(const vector<int64_t>& ids);
    /// Transfer events in file-specified list
    void transfer(const string& evtfile);
    /// Set total time attribute
    void setTotalTime(double t);
    
    int64_t nTransferred = 0;                           ///< total number of events transferred

protected:
    hid_t outfile_id = 0;                               ///< output HDF5 file ID
    int nchunk = 1024;                                  ///< "chunk" size for tables
    int compress = 9;                                   ///< compression level for tables
    hid_t infile_id = 0;                                ///< input HDF5 file ID
    HDF5_Table_Transfer<s_Event> evt_xfer;              ///< event transferrer for s_Event
    HDF5_Table_Transfer<s_IoniCluster> ioni_xfer;       ///< event transferrer for s_IoniCluster
    HDF5_Table_Transfer<s_ParticleVertex> prim_xfer;    ///< event transferrer for primaries s_ParticleVertex
    HDF5_Table_Transfer<s_NCapt> ncapt_xfer;            ///< event transferrer for s_NCapt
};

#endif
