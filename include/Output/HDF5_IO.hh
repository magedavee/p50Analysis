/// \file HDF5_IO.hh \brief HDF5 format IO classes.
#ifndef HDF5_IO_HH
#define HDF5_IO_HH

#ifdef WITH_HDF5

#include "FileIO.hh"
#include "HDF5_Table_Cache.hh"

/// HDF5-formatted output file
class HDF5_IO: public FileIO {
public:
    /// Constructor
    HDF5_IO();
    
    /// Destructor
    virtual ~HDF5_IO();
    
    /// record event numbering
    virtual void addEvtBranch();
    /// record scintillator ionization
    virtual void addScIoniBranch();
    /// record primary particles
    virtual void addPrimBranch();
    /// record neutron captures
    virtual void addNCaptBranch();
    
    /// Open named output file
    virtual void SetFileName(const string& filename);
    /// Add current Event to output file
    virtual void SaveEvent();
    /// Finalize/close file output
    virtual void WriteFile();
    
protected:
    
    HDF5_Table_Writer<s_Event> evt_writer;              ///< buffered output for s_Event
    HDF5_Table_Writer<s_IoniCluster> ioni_writer;       ///< cached output for s_IoniCluster
    HDF5_Table_Writer<s_ParticleVertex> prim_writer;    ///< cached output for primaries s_ParticleVertex
    HDF5_Table_Writer<s_NCapt> ncapt_writer;            ///< buffered output for s_NCapt
    
    hid_t outfile_id = 0;       ///< output HDF5 file ID
    int nchunk = 1024;          ///< "chunk" size for tables
    int compress = 9;           ///< compression level for tables
};

#endif
#endif
