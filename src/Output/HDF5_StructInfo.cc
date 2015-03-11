#ifdef WITH_HDF5

#include "HDF5_StructInfo.hh"

hsize_t const HDF5_StructInfo::array_dim_3 = 3;
hid_t const HDF5_StructInfo::vec3_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, &HDF5_StructInfo::array_dim_3);

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::DetPulse_offsets[n_DetPulse_fields] = {    
    HOFFSET(s_DetPulse,evt),
    HOFFSET(s_DetPulse,det),
    HOFFSET(s_DetPulse,t),
    HOFFSET(s_DetPulse,h),
    HOFFSET(s_DetPulse,a),
    HOFFSET(s_DetPulse,b),
    HOFFSET(s_DetPulse,rise),
    HOFFSET(s_DetPulse,PSD)
};

size_t const HDF5_StructInfo::DetPulse_sizes[n_DetPulse_fields] = {
    sizeof(s_DetPulse::evt),
    sizeof(s_DetPulse::det),
    sizeof(s_DetPulse::t),
    sizeof(s_DetPulse::h),
    sizeof(s_DetPulse::a),
    sizeof(s_DetPulse::b),
    sizeof(s_DetPulse::rise),
    sizeof(s_DetPulse::PSD)
};

const char* HDF5_StructInfo::DetPulse_field_names[n_DetPulse_fields] = { "evt", "det", "t", "h", "a", "b", "rise", "PSD" };

hid_t const HDF5_StructInfo::DetPulse_field_types[n_DetPulse_fields] = {
    H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_FLOAT,
    H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT, H5T_NATIVE_FLOAT
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::PhysPulse_offsets[n_PhysPulse_fields] = {
    HOFFSET(s_PhysPulse,evt),
    HOFFSET(s_PhysPulse,seg),
    HOFFSET(s_PhysPulse,E),
    HOFFSET(s_PhysPulse,t),
    HOFFSET(s_PhysPulse,y),
    HOFFSET(s_PhysPulse,PSD),
    HOFFSET(s_PhysPulse,PID)
};

size_t const HDF5_StructInfo::PhysPulse_sizes[n_PhysPulse_fields] = {
    sizeof(s_PhysPulse::evt),
    sizeof(s_PhysPulse::seg),
    sizeof(s_PhysPulse::E),
    sizeof(s_PhysPulse::t),
    sizeof(s_PhysPulse::y),
    sizeof(s_PhysPulse::PSD),
    sizeof(s_PhysPulse::PID)
};

const char* HDF5_StructInfo::PhysPulse_field_names[n_PhysPulse_fields] = { "evt", "seg", "E", "t", "y", "PSD", "PID" };

hid_t const HDF5_StructInfo::PhysPulse_field_types[n_PhysPulse_fields] = {
    H5T_NATIVE_INT64, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::IoniCluster_offsets[n_IoniCluster_fields] = {    
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

size_t const HDF5_StructInfo::IoniCluster_sizes[n_IoniCluster_fields] = {
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

const char* HDF5_StructInfo::IoniCluster_field_names[n_IoniCluster_fields] = { "E", "t", "dt", "x", "dx", "EdEdx", "EdEdx2", "l", "vol", "PID", "evt" };

hid_t const HDF5_StructInfo::IoniCluster_field_types[n_IoniCluster_fields] = {
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::ParticleVertex_offsets[n_ParticleVertex_fields] {
    HOFFSET(s_ParticleVertex,PID),
    HOFFSET(s_ParticleVertex,x),
    HOFFSET(s_ParticleVertex,p),
    HOFFSET(s_ParticleVertex,E),
    HOFFSET(s_ParticleVertex,t),
    HOFFSET(s_ParticleVertex,evt)
};

size_t const HDF5_StructInfo::ParticleVertex_sizes[n_ParticleVertex_fields] = {
    sizeof(s_ParticleVertex::PID),
    sizeof(s_ParticleVertex::x),
    sizeof(s_ParticleVertex::p),
    sizeof(s_ParticleVertex::E),
    sizeof(s_ParticleVertex::t),
    sizeof(s_ParticleVertex::evt)
};

const char* HDF5_StructInfo::ParticleVertex_field_names[n_ParticleVertex_fields] = { "PID", "x", "p", "E", "t", "evt" };

hid_t const HDF5_StructInfo::ParticleVertex_field_types[n_ParticleVertex_fields] = {
    H5T_NATIVE_INT, vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64
};

/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::NCapt_offsets[n_NCapt_fields] {
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

size_t const HDF5_StructInfo::NCapt_sizes[n_NCapt_fields] = {
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

const char* HDF5_StructInfo::NCapt_field_names[n_NCapt_fields] = { "t", "E", "x", "Ngamma", "Egamma", "Nprod", "capt_A", "capt_Z", "vol", "evt" };

hid_t const HDF5_StructInfo::NCapt_field_types[n_NCapt_fields] = {
    H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, vec3_tid, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE,
    H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64
};
    
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

size_t const HDF5_StructInfo::Event_offsets[n_Event_fields] {
    HOFFSET(s_Event,N),
    HOFFSET(s_Event,t),
    HOFFSET(s_Event,ct),
    HOFFSET(s_Event,flg)
};

size_t const HDF5_StructInfo::HDF5_StructInfo::Event_sizes[n_Event_fields] = {
    sizeof(s_Event::N),
    sizeof(s_Event::t),
    sizeof(s_Event::ct),
    sizeof(s_Event::flg)
};

const char* HDF5_StructInfo::Event_field_names[n_Event_fields] = { "N", "t", "ct", "flg" };

hid_t const HDF5_StructInfo::Event_field_types[n_Event_fields] = { H5T_NATIVE_INT64, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT };

#endif
