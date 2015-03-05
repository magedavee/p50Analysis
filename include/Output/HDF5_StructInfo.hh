/// \file HDF5_StructInfo.hh \brief Struct layout information for HDF5 tables
#ifndef HDF5_STRUCTINFO_HH
#define HDF5_STRUCTINFO_HH

#include "AnalysisTypes.hh"
#include "hdf5.h"
#include "hdf5_hl.h"

// double[3] array type
const hsize_t array_dim_3 = 3;
const hid_t vec3_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, &array_dim_3);

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_DetPulse struct
const hsize_t n_DetPulse_fields = 8;

/// element offsets for s_DetPulse
const size_t DetPulse_offsets[n_DetPulse_fields] = {    
    HOFFSET(s_DetPulse,evt),
    HOFFSET(s_DetPulse,det),
    HOFFSET(s_DetPulse,t),
    HOFFSET(s_DetPulse,h),
    HOFFSET(s_DetPulse,a),
    HOFFSET(s_DetPulse,b),
    HOFFSET(s_DetPulse,rise),
    HOFFSET(s_DetPulse,PSD)
};

/// element sizes for s_DetPulse
const size_t DetPulse_sizes[n_DetPulse_fields] = {
    sizeof(s_DetPulse::evt),
    sizeof(s_DetPulse::det),
    sizeof(s_DetPulse::t),
    sizeof(s_DetPulse::h),
    sizeof(s_DetPulse::a),
    sizeof(s_DetPulse::b),
    sizeof(s_DetPulse::rise),
    sizeof(s_DetPulse::PSD)
};

/// field names for s_DetPulse
const char* DetPulse_field_names[n_DetPulse_fields] = { "evt", "det", "t", "h", "a", "b", "rise", "PSD" };
/// field types for s_DetPulse
const hid_t DetPulse_field_types[n_DetPulse_fields] = {
    H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_FLOAT,
    H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT
};


/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_PhysPulse struct
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

/// field names for s_PhysPulse
const char* PhysPulse_field_names[n_PhysPulse_fields] = { "evt", "seg", "E", "t", "y", "PSD", "PID" };
/// field types for s_PhysPulse
const hid_t PhysPulse_field_types[n_PhysPulse_fields] = {
    H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_IoniCluster struct
const hsize_t n_IoniCluster_fields = 11;

/// element offsets for s_IoniCluster
const size_t IoniCluster_offsets[n_IoniCluster_fields] = {    
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
const size_t IoniCluster_sizes[n_IoniCluster_fields] = {
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

/// field names for s_IoniCluster
const char* IoniCluster_field_names[n_IoniCluster_fields] = { "E", "t", "dt", "x", "dx", "EdEdx", "EdEdx2", "l", "vol", "PID", "evt" };
/// field types for s_IoniCluster
const hid_t IoniCluster_field_types[n_IoniCluster_fields] = {
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_ParticleVertex struct
const hsize_t n_ParticleVertex_fields = 6;

/// element offsets for s_ParticleVertex
const size_t ParticleVertex_offsets[n_ParticleVertex_fields] {
    HOFFSET(s_ParticleVertex,PID),
    HOFFSET(s_ParticleVertex,x),
    HOFFSET(s_ParticleVertex,p),
    HOFFSET(s_ParticleVertex,E),
    HOFFSET(s_ParticleVertex,t),
    HOFFSET(s_ParticleVertex,evt)
};

/// element sizes for s_ParticleVertex
const size_t ParticleVertex_sizes[n_ParticleVertex_fields] = {
    sizeof(s_ParticleVertex::PID),
    sizeof(s_ParticleVertex::x),
    sizeof(s_ParticleVertex::p),
    sizeof(s_ParticleVertex::E),
    sizeof(s_ParticleVertex::t),
    sizeof(s_ParticleVertex::evt)
};

/// field names for s_ParticleVertex
const char* ParticleVertex_field_names[n_ParticleVertex_fields] = { "PID", "x", "p", "E", "t", "evt" };
/// field types for s_ParticleVertex
const hid_t ParticleVertex_field_types[n_ParticleVertex_fields] = {
    H5T_NATIVE_INT, vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_NCapt struct
const hsize_t n_NCapt_fields = 10;

/// element offsets for s_NCapt
const size_t NCapt_offsets[n_NCapt_fields] {
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
const size_t NCapt_sizes[n_NCapt_fields] = {
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

/// field names for s_NCapt
const char* NCapt_field_names[n_NCapt_fields] = { "t", "E", "x", "Ngamma", "Egamma", "Nprod", "capt_A", "capt_Z", "vol", "evt" };
/// field types for s_NCapt
const hid_t NCapt_field_types[n_NCapt_fields] = {
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, vec3_tid, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64
};
    
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

/// number of members in s_Event struct
const hsize_t n_Event_fields = 4;

/// element offsets for s_Event
const size_t Event_offsets[n_Event_fields] {
    HOFFSET(s_Event,N),
    HOFFSET(s_Event,t),
    HOFFSET(s_Event,ct),
    HOFFSET(s_Event,flg)
};

/// element sizes for s_Event
const size_t Event_sizes[n_Event_fields] = {
    sizeof(s_Event::N),
    sizeof(s_Event::t),
    sizeof(s_Event::ct),
    sizeof(s_Event::flg)
};

/// field names for s_Event
const char* Event_field_names[n_Event_fields] = { "N", "t", "ct", "flg" };
/// field types for s_Event
const hid_t Event_field_types[n_Event_fields] = { H5T_NATIVE_INT64, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT };

#endif
