/// \file HDF5_IO.hh \brief HDF5 format IO classes.
#ifndef HDF5_IO_HH
#define HDF5_IO_HH

#ifdef WITH_HDF5

#include "FileIO.hh"
#include "hdf5.h"
#include "hdf5_hl.h"

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
    
    hid_t outfile_id = 0;       ///< output HDF5 file ID
    hid_t vec3_tid = 0;         ///< type ID for double[3]
    int nchunk = 100;           ///< "chunk" size for tables
    int compress = 9;           ///< compression level for tables
};



// ///////////////////////////////
// info on structs for HDF5 I/O
// ///////////////////////////////

/// element offsets for s_IoniCluster
const size_t IoniCluster_offsets[11] = {    
    HOFFSET(s_IoniCluster,E),
    HOFFSET(s_IoniCluster,t),
    HOFFSET(s_IoniCluster,dt),
    HOFFSET(s_IoniCluster,x),
    HOFFSET(s_IoniCluster,dx),
    HOFFSET(s_IoniCluster,EdEdx),
    HOFFSET(s_IoniCluster,EdEdx2),
    HOFFSET(s_IoniCluster,l),
    HOFFSET(s_IoniCluster,vol),
    HOFFSET(s_IoniCluster,PID),
    HOFFSET(s_IoniCluster,evt)
};

/// element sizes for s_IoniCluster
const size_t IoniCluster_sizes[11] = {
    sizeof(s_IoniCluster::E),
    sizeof(s_IoniCluster::t),
    sizeof(s_IoniCluster::dt),
    sizeof(s_IoniCluster::x),
    sizeof(s_IoniCluster::dx),
    sizeof(s_IoniCluster::EdEdx),
    sizeof(s_IoniCluster::EdEdx2),
    sizeof(s_IoniCluster::l),
    sizeof(s_IoniCluster::vol),
    sizeof(s_IoniCluster::PID),
    sizeof(s_IoniCluster::evt)
};

/// element offsets for s_ParticleVertex
const size_t ParticleVertex_offsets[6] {
    HOFFSET(s_ParticleVertex,PID),
    HOFFSET(s_ParticleVertex,x),
    HOFFSET(s_ParticleVertex,p),
    HOFFSET(s_ParticleVertex,E),
    HOFFSET(s_ParticleVertex,t),
    HOFFSET(s_ParticleVertex,evt)
};

/// element sizes for s_ParticleVertex
const size_t ParticleVertex_sizes[6] = {
    sizeof(s_ParticleVertex::PID),
    sizeof(s_ParticleVertex::x),
    sizeof(s_ParticleVertex::p),
    sizeof(s_ParticleVertex::E),
    sizeof(s_ParticleVertex::t),
    sizeof(s_ParticleVertex::evt)
};


/// element offsets for s_NCapt
const size_t NCapt_offsets[10] {
    HOFFSET(s_NCapt,t),
    HOFFSET(s_NCapt,E),
    HOFFSET(s_NCapt,x),
    HOFFSET(s_NCapt,Ngamma),
    HOFFSET(s_NCapt,Egamma),
    HOFFSET(s_NCapt,Nprod),
    HOFFSET(s_NCapt,capt_A),
    HOFFSET(s_NCapt,capt_Z),
    HOFFSET(s_NCapt,vol),
    HOFFSET(s_NCapt,evt)
};

/// element sizes for s_NCapt
const size_t NCapt_sizes[10] = {
    sizeof(s_NCapt::t),
    sizeof(s_NCapt::E),
    sizeof(s_NCapt::x),
    sizeof(s_NCapt::Ngamma),
    sizeof(s_NCapt::Egamma),
    sizeof(s_NCapt::Nprod),
    sizeof(s_NCapt::capt_A),
    sizeof(s_NCapt::capt_Z),
    sizeof(s_NCapt::vol),
    sizeof(s_NCapt::evt)
};

/// element offsets for s_Event
const size_t Event_offsets[4] {
    HOFFSET(s_Event,N),
    HOFFSET(s_Event,t),
    HOFFSET(s_Event,ct),
    HOFFSET(s_Event,flg)
};

/// element sizes for s_Event
const size_t Event_sizes[4] = {
    sizeof(s_Event::N),
    sizeof(s_Event::t),
    sizeof(s_Event::ct),
    sizeof(s_Event::flg)
};

#endif
#endif
