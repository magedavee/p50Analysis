#include "DetectorResponse.hh"
#include "XMLInfo.hh"
#include <cmath>
#include <TRandom3.h>

double DetectorResponse::Equench(const s_IoniCluster& evt) const {
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    return evt.E / (1 + c_1*evt.EdEdx/evt.E + c_2*evt.EdEdx2/evt.E);
}

double DetectorResponse::PSD(const s_IoniCluster& evt) const {
    return evt.E/evt.EdEdx; 
}

s_PhysPulse DetectorResponse::genResponse(const s_IoniCluster& evt) const {
    s_PhysPulse p;
    
    p.evt = evt.evt;
    p.seg = evt.vol;
    p.E = Equench(evt);
    p.t = evt.t;
    p.y = evt.x[1];
    p.PSD = PSD(evt);
    
    return p;
}

int main(int argc, char** argv) {

    if(argc != 2) {
        printf("Argument: <PROSPECT-G4 HDF5 filename>\n");
        return -1;
    }
    
    string f_in = argv[1];
    string fext = split(f_in,".").back(); // file extension
    assert(fext == "h5");
    string fbase = f_in.substr(0, f_in.size()-fext.size()-1); // base filename less extension
    string f_out = fbase+"_DetSim."+fext;
    
    bool fullsort = true; // whether to cache all events in memory and time-sort
    
    // open input file
    SimIoniReader SIR(f_in);
    XMLInfo XI(f_in+".xml");
    double runtime = XI.getGenTime(); // [ns]
    size_t runthrows = XI.getEvts();
    printf("Input file for %zu primary events in %.1f seconds simulated time.\n", runthrows, runtime*1e-9);
    
    // set up output file
    hid_t outfile_id = H5Fcreate(f_out.c_str(), // file name
                                 H5F_ACC_TRUNC, // access_mode : overwrite old file with new data
                                 H5P_DEFAULT,   // create_ID
                                 H5P_DEFAULT    // access_ID
    );
    assert(outfile_id >= 0);
    herr_t err = H5TBmake_table("Simulated detector response", outfile_id, "PhysDat",
                                n_PhysPulse_fields, 0, sizeof(s_PhysPulse),
                                PhysPulse_field_names, PhysPulse_offsets, PhysPulse_field_types,
                                100, NULL, 9, NULL);
    assert(err >= 0);
    
    // convert events to detector response
    DetectorResponse DR;
    size_t nMerged = 0;
    vector<s_PhysPulse> allPulses;
    TRandom3 r;
    while(SIR.loadMergedIoni()) {
        if(!(SIR.nRead % (SIR.nrecords/20))) { cout << "*"; cout.flush(); }
        double evttime = r.Rndm()*runtime; // uniform random time offset for event cluster
        for(auto it = SIR.merged.begin(); it != SIR.merged.end(); it++) {
            s_PhysPulse p = DR.genResponse(*it);
            if(!fullsort) {
                err = H5TBappend_records(outfile_id, "PhysDat", 1, sizeof(s_PhysPulse), PhysPulse_offsets, PhysPulse_sizes, &p);
                assert(err >= 0);
            } else {
                p.t += evttime;
                while(p.t > runtime) p.t -= runtime*floor(p.t/runtime); // wrap around long-delayed events to start of run
                allPulses.push_back(p);
            }
            nMerged++;
        }
    }
    // optional full time sorting
    if(allPulses.size()) {
        printf("\nMaster time merge and output... "); fflush(stdout);
        std::sort(allPulses.begin(), allPulses.end(), compare_hit_times);
        err = H5TBappend_records(outfile_id, "PhysDat", allPulses.size(), sizeof(s_PhysPulse), PhysPulse_offsets, PhysPulse_sizes, allPulses.data());
        assert(err >= 0);
        printf("Done.");
    }
    printf("\nRead %llu ionizations into %zu merged events.\nOutput '%s'\n", SIR.nRead, nMerged, f_out.c_str());
    
    H5Fclose(outfile_id);
    return 0;
}
