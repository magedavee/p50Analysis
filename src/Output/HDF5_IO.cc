#ifdef WITH_HDF5

#include "HDF5_IO.hh"
#include <TClonesArray.h>
#include <G4ios.hh>
#include <cassert>

HDF5_IO::HDF5_IO():
evt_writer("Evt", Event_offsets, Event_sizes, nchunk),
ioni_writer("ScIoni", IoniCluster_offsets, IoniCluster_sizes, nchunk),
prim_writer("Prim", ParticleVertex_offsets, ParticleVertex_sizes, nchunk),
ncapt_writer("NCapt", NCapt_offsets, NCapt_sizes, nchunk) {
    const hsize_t array_dim = 3;
    vec3_tid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, &array_dim); // double[3] array type
    G4cout << "HDF5_IO initialized. Remember to specify output filename." << G4endl;
}

HDF5_IO::~HDF5_IO() {
    evt_writer.setFile(0);
    ioni_writer.setFile(0);
    prim_writer.setFile(0);
    ncapt_writer.setFile(0);
    H5Tclose(vec3_tid);
    if(outfile_id) H5Fclose(outfile_id);
}

void HDF5_IO::WriteFile() {
    if(!outfile_id) {
        G4cout << "No HDF5 output file opened! Data not saved!" << G4endl;
        return;
    }
    std::cout << "Writing events to HDF5 file '" << fname << "' and closing..." << std::endl;
    H5Fclose(outfile_id);
    outfile_id = 0;
    
    evt_writer.setFile(0);
    ioni_writer.setFile(0);
    prim_writer.setFile(0);
    ncapt_writer.setFile(0);
}

void HDF5_IO::SaveEvent() {
    if(!outfile_id) return;
    
    if(pmcevent) evt_writer.write(*pmcevent);
    
    if(pscintIoni) {
        Int_t n = pscintIoni->clusts->GetEntriesFast();
        for(Int_t i=0; i<n; i++) {
            IoniCluster* x = (IoniCluster*)pscintIoni->clusts->At(i);
            x->evt = mcevent.N;
            ioni_writer.write(*x);
        }
    }
    
    if(pprimPtcls) {
        Int_t n = pprimPtcls->particles->GetEntriesFast();
        for(Int_t i=0; i<n; i++) {
            ParticleVertex* x = (ParticleVertex*)pprimPtcls->particles->At(i);
            x->evt = mcevent.N;
            prim_writer.write(*x);
        }
    }
    
    if(pscintNCapt) {
        Int_t n = pscintNCapt->nCapts->GetEntriesFast();
        for(Int_t i=0; i<n; i++) {
            NCapt* x = (NCapt*)pscintNCapt->nCapts->At(i);
            x->evt = mcevent.N;
            ncapt_writer.write(*x);
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
        
    herr_t err = H5TBmake_table("Scintillator Ionization", outfile_id, "ScIoni",
                                n_fields, 0, sizeof(s_IoniCluster), field_names, IoniCluster_offsets, field_types,
                                nchunk, NULL, compress, NULL);
    assert(err >= 0);
}

void HDF5_IO::addPrimBranch() {
    if(pprimPtcls) return; // already set up
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'Prim' output branch...\n";
    subObjs.push_back(pprimPtcls = &primPtcls);
    
    const hsize_t n_fields = 6;    
    const char* field_names[n_fields] = { "PID", "x", "p", "E", "t", "evt" };
    const hid_t field_types[n_fields] = { H5T_NATIVE_INT, vec3_tid, vec3_tid, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64 };
    
    herr_t err = H5TBmake_table("Primary particles", outfile_id, "Prim",
                                n_fields, 0, sizeof(s_ParticleVertex), field_names, ParticleVertex_offsets, field_types,
                                nchunk, NULL, compress, NULL);
    assert(err >= 0);
}

void HDF5_IO::addNCaptBranch() {
    if(pscintNCapt) return;
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'NCapt' output branch...\n";
    subObjs.push_back(pscintNCapt = &scintNCapt);
    
    const hsize_t n_fields = 10;    
    const char* field_names[n_fields] = { "t", "E", "x", "Ngamma", "Egamma", "Nprod", "capt_A", "capt_Z", "vol", "evt" };
    const hid_t field_types[n_fields] = { H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, vec3_tid, H5T_NATIVE_INT, H5T_NATIVE_DOUBLE,
        H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT, H5T_NATIVE_INT64 };
    herr_t err = H5TBmake_table("Neutron captures", outfile_id, "NCapt",
                                n_fields, 0, sizeof(s_NCapt), field_names, NCapt_offsets, field_types,
                                nchunk, NULL, compress, NULL);
    assert(err >= 0);
}

void HDF5_IO::addEvtBranch() {
    if(pmcevent) return; // already set up
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'Evt' output branch...\n";
    subObjs.push_back(pmcevent = &mcevent);

    const hsize_t n_fields = 4;    
    const char* field_names[n_fields] = { "N", "t", "ct", "flg" };
    const hid_t field_types[n_fields] = { H5T_NATIVE_INT64, H5T_NATIVE_DOUBLE, H5T_NATIVE_DOUBLE, H5T_NATIVE_INT };

    herr_t err = H5TBmake_table("Event calculation info", outfile_id, "Evt",
                                n_fields, 0, sizeof(s_Event), field_names, Event_offsets, field_types,
                                nchunk, NULL, compress, NULL);
    assert(err >= 0);
}

void HDF5_IO::SetFileName(const string& filename) {
    assert(!outfile_id);
    fname = filename;
    outfile_id = H5Fcreate(fname.c_str(), // file name
                           H5F_ACC_TRUNC, // access_mode : overwrite old file with new data
                           H5P_DEFAULT,   // create_ID
                           H5P_DEFAULT    // access_ID
                          );
    addEvtBranch();
    addPrimBranch();
    
    evt_writer.setFile(outfile_id);
    ioni_writer.setFile(outfile_id);
    prim_writer.setFile(outfile_id);
    ncapt_writer.setFile(outfile_id);
}

#endif
