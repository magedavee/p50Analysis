/// \file DisplayHDF5Events.cc \brief Print out contents of PG4 output HDF5 file
#include "AnaUtils.hh"
#include "HDF5_IO.hh"
#include <cassert>

class PG4_HDF5_Reader: protected HDF5_StructInfo {
public:
    /// Constructor
    PG4_HDF5_Reader(const string& f_in):
    prim_reader("Prim", ParticleVertex_offsets, ParticleVertex_sizes, 1024),
    ioni_reader("ScIoni", IoniCluster_offsets, IoniCluster_sizes, 1024) {
        infile_id = H5Fopen(f_in.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
        assert(infile_id > 0);
        
        prim_reader.setFile(infile_id);
        ioni_reader.setFile(infile_id);
    }
    hid_t infile_id = 0;                                ///< input HDF5 file ID
    HDF5_Table_Cache<s_ParticleVertex> prim_reader;     ///< cached output for s_IoniCluster
    HDF5_Table_Cache<s_IoniCluster> ioni_reader;        ///< cached output for s_IoniCluster
};

int main(int argc, char** argv) {
    if(argc != 2) return -1;
    
    PG4_HDF5_Reader R(argv[1]);
    int64_t current_evt = 0;
    
    s_ParticleVertex prim;      ///< primary event loaded
    s_IoniCluster ioni;         ///< IoniCluster event loaded
    prim.evt = ioni.evt = -1;
    
    while(true) {
        printf("\n--- Event %lli ---\n", current_evt);
        do {
            if(prim.evt == current_evt) printf("Primary PID = %i,\tE = %g\n", prim.PID, prim.E);
        } while(R.prim_reader.next(prim) && prim.evt <= current_evt);
        
        do {
            if(ioni.evt == current_evt) printf("Ionization PID = %i,\tvol = %i,\tE = %g\n", ioni.PID, ioni.vol, ioni.E);
        }  while(R.ioni_reader.next(ioni) && ioni.evt <= current_evt);
        
        if(prim.evt <= current_evt && ioni.evt <= current_evt) break;
        if(prim.evt <= current_evt) current_evt = ioni.evt;
        else if(ioni.evt <= current_evt) current_evt = prim.evt;
        else current_evt = std::min(prim.evt, ioni.evt);
    }
    
    return 0;
}
