/// \file HDF5_StructInfo.hh \brief Struct layout information for HDF5 tables
#ifndef HDF5_STRUCTINFO_HH
#define HDF5_STRUCTINFO_HH

#ifdef WITH_HDF5

#include "AnalysisTypes.hh"
#include "hdf5.h"
#include "hdf5_hl.h"

/// Class providing struct information for HDF5 table structures
class HDF5_StructInfo {
public:
    /// Constructor
    HDF5_StructInfo() { }
    
    /// set up s_IoniCluster table
    void makeScIoniTable(hid_t outfile_id, int nchunk, int compress) const;
    /// set up s_ParticleVertex primaries table
    void makePrimTable(hid_t outfile_id, int nchunk, int compress) const;
    /// set up s_NCapt table
    void makeNCaptTable(hid_t outfile_id, int nchunk, int compress) const;
    /// set up s_Event table
    void makeEvtTable(hid_t outfile_id, int nchunk, int compress) const;
    
    // double[3] array type
    static hsize_t const array_dim_3;
    static hid_t const vec3_tid;

    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_DetPulse struct
    static hsize_t const n_DetPulse_fields = 8;
    /// element offsets for s_DetPulse
    static size_t const DetPulse_offsets[n_DetPulse_fields];
    /// element sizes for s_DetPulse
    static size_t const DetPulse_sizes[n_DetPulse_fields];
    /// field names for s_DetPulse
    static const char* DetPulse_field_names[n_DetPulse_fields];
    /// field types for s_DetPulse
    static hid_t const DetPulse_field_types[n_DetPulse_fields];

    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_PhysPulse struct
    static hsize_t const n_PhysPulse_fields = 7;
    /// element offsets for s_PhysPulse
    static size_t const PhysPulse_offsets[n_PhysPulse_fields];
    /// element sizes for s_PhysPulse
    static size_t const PhysPulse_sizes[n_PhysPulse_fields];
    /// field names for s_PhysPulse
    static const char* PhysPulse_field_names[n_PhysPulse_fields];
    /// field types for s_PhysPulse
    static hid_t const PhysPulse_field_types[n_PhysPulse_fields];

    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_IoniCluster struct
    static hsize_t const n_IoniCluster_fields = 11;
    /// element offsets for s_IoniCluster
    static size_t const IoniCluster_offsets[n_IoniCluster_fields];
    /// element sizes for s_IoniCluster
    static size_t const IoniCluster_sizes[n_IoniCluster_fields];
    /// field names for s_IoniCluster
    static const char* IoniCluster_field_names[n_IoniCluster_fields];
    /// field types for s_IoniCluster
    static hid_t const IoniCluster_field_types[n_IoniCluster_fields];

    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_ParticleVertex struct
    static hsize_t const n_ParticleVertex_fields = 6;
    /// element offsets for s_ParticleVertex
    static size_t const ParticleVertex_offsets[n_ParticleVertex_fields];
    /// element sizes for s_ParticleVertex
    static size_t const ParticleVertex_sizes[n_ParticleVertex_fields];
    /// field names for s_ParticleVertex
    static const char* ParticleVertex_field_names[n_ParticleVertex_fields];
    /// field types for s_ParticleVertex
    static hid_t const ParticleVertex_field_types[n_ParticleVertex_fields];

    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_NCapt struct
    static hsize_t const n_NCapt_fields = 10;
    /// element offsets for s_NCapt
    static size_t const NCapt_offsets[n_NCapt_fields];
    /// element sizes for s_NCapt
    static size_t const NCapt_sizes[n_NCapt_fields];
    /// field names for s_NCapt
    static const char* NCapt_field_names[n_NCapt_fields];
    /// field types for s_NCapt
    static hid_t const NCapt_field_types[n_NCapt_fields];
        
    /////////////////////////////////
    /////////////////////////////////
    /////////////////////////////////

    /// number of members in s_Event struct
    static hsize_t const n_Event_fields = 4;
    /// element offsets for s_Event
    static size_t const Event_offsets[n_Event_fields];
    /// element sizes for s_Event
    static size_t const Event_sizes[n_Event_fields];
    /// field names for s_Event
    static const char* Event_field_names[n_Event_fields];
    /// field types for s_Event
    static hid_t const Event_field_types[n_Event_fields];

};

#endif
#endif
