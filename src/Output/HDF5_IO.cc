#ifdef WITH_HDF5

#include "HDF5_IO.hh"
#include <TClonesArray.h>
#include <G4ios.hh>
#include <cassert>

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

HDF5_IO::HDF5_IO() {
    const hsize_t array_dim = 3;
    vec3_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, &array_dim); // double[3] array type
    
    //addPrimBranch();
    //addEvtBranch();
    G4cout << "HDF5_IO initialized. Remember to specify output filename." << G4endl;
}

HDF5_IO::~HDF5_IO() {
    H5Tclose(vec3_tid);
    //H5Sclose(space);
    //H5Dclose(dataset);
    H5Fclose(outfile_id);
}

void HDF5_IO::WriteFile() {
    if(!outfile_id) {
        G4cout << "No HDF5 output file opened! Data not saved!" << G4endl;
        return;
    }
    std::cout << "Writing events to HDF5 file '" << fname << "' and closing..." << std::endl;
    H5Fclose(outfile_id);
    outfile_id = 0;
}

void HDF5_IO::SaveEvent() {
    if(!outfile_id) return;
    
    herr_t err;
    
    if(pscintIoni) {
        Int_t nIoni = pscintIoni->clusts->GetEntriesFast();
        for(Int_t i=0; i<nIoni; i++) {
            IoniCluster* ei = (IoniCluster*)pscintIoni->clusts->At(i);
            ei->evt = pmcevent? pmcevent->N : 0;
            err = H5TBappend_records(outfile_id, "ScIoni", 1, sizeof(s_IoniCluster), IoniCluster_offsets, IoniCluster_sizes, (s_IoniCluster*)ei);
            assert(err >= 0);
        }
    }
}

void HDF5_IO::addScIoniBranch() {
    if(pscintIoni) return; // already set up
    assert(outfile_id);
    
    G4cout << "HDF5_IO Setting up 'ScIoni' output branch...\n";
    subObjs.push_back(pscintIoni = &scintIoni);
    
    const hsize_t n_fields = 11;    
    const char* field_names[n_fields] = { "E", "t", "dt", "x", "dx", "EdEdx", "EdEdx2", "l", "vol", "PID", "evt" };
    const hid_t field_types[n_fields] = {
        H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
        vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE,
        H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64
    };
    
    //for(hsize_t i=0; i<n_fields; i++) G4cout << field_names[i] << ": size = " << IoniCluster_sizes[i] << ", offset = " << IoniCluster_offsets[i] << G4endl;
    
    herr_t err = H5TBmake_table("Scintillator Ionization", outfile_id, "ScIoni",
                                n_fields, 0, sizeof(s_IoniCluster), field_names, IoniCluster_offsets, field_types,
                                100, NULL, 6, NULL);
    assert(err >= 0);
}

void HDF5_IO::SetFileName(G4String filename) {
    assert(!outfile_id);
    fname = filename;
    outfile_id = H5Fcreate(fname.c_str(), // file name
                           H5F_ACC_TRUNC, // access_mode : overwrite old file with new data
                           H5P_DEFAULT,   // create_ID
                           H5P_DEFAULT    // access_ID
                          );
}

#endif
