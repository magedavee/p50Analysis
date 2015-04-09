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
    G4cout << "HDF5_IO initialized. Remember to specify output filename." << G4endl;
}

HDF5_IO::~HDF5_IO() {
    evt_writer.setFile(0);
    ioni_writer.setFile(0);
    prim_writer.setFile(0);
    ncapt_writer.setFile(0);
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
    
    makeScIoniTable(outfile_id, nchunk, compress);
}

void HDF5_IO::addPrimBranch() {
    if(pprimPtcls) return; // already set up
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'Prim' output branch...\n";
    subObjs.push_back(pprimPtcls = &primPtcls);
        
    makePrimTable(outfile_id, nchunk, compress);
}

void HDF5_IO::addNCaptBranch() {
    if(pscintNCapt) return;
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'NCapt' output branch...\n";
    subObjs.push_back(pscintNCapt = &scintNCapt);
      
    makeNCaptTable(outfile_id, nchunk, compress);
}

void HDF5_IO::addEvtBranch() {
    if(pmcevent) return; // already set up
    assert(outfile_id);
    G4cout << "HDF5_IO Setting up 'Evt' output branch...\n";
    subObjs.push_back(pmcevent = &mcevent);
  
    makeEvtTable(outfile_id, nchunk, compress);
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
