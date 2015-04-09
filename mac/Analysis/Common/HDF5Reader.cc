#include "HDF5Reader.hh"
#include "HDF5_StructInfo.hh"

SimIoniReader::SimIoniReader(const string& f_in): ioni_reader("ScIoni",IoniCluster_offsets, IoniCluster_sizes,1024) {
    infile_id = H5Fopen(f_in.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    assert(infile_id > 0);
    ioni.evt = -1;
    
    herr_t err = H5TBget_table_info(infile_id, "ScIoni", &nfields, &nrecords );
    assert(err >= 0);
    printf("Reading table 'ScIoni' with %llu records from '%s'.\n", nrecords, f_in.c_str());
    ioni_reader.setFile(infile_id);
}

SimIoniReader::~SimIoniReader() { 
    if(infile_id > 0) {
        herr_t err = H5Fclose(infile_id);
        assert(err >= 0);
    }
}

/// comparison function for time-sorting hits
bool compare_hit_times(const s_IoniCluster& a, const s_IoniCluster& b) { return a.t < b.t; }

bool SimIoniReader::loadIoni() {
    if(ioni_reader.next(ioni)) {
        nRead++;
        if(P20reflectorless && ioni.vol == 1) ioni.vol = 0;
        return true;
    } else return false;
}

bool SimIoniReader::loadMergedIoni() {
    map<Int_t, vector<s_IoniCluster> > volClusts;
    
    if(ioni.evt > 0) volClusts[ioni.vol].push_back(ioni); // use event from previous read
    else if(ioni_reader.getNRead()) return false; // file is exhausted
    
    current_evt = ioni.evt; // = -1 on the first time
    
    while(true) {
        if(!loadIoni()) {
            ioni.evt = -1;
            break;
        }
        if(current_evt < 0) current_evt = ioni.evt;
        if(ioni.evt != current_evt) break;
        
        Int_t V = ioni.vol;
        if(!volClusts.count(V)) volClusts[V].push_back(ioni);
        else {
            if(ioni.t > volClusts[V].back().t + dt_max)
                volClusts[V].push_back(ioni);
            else
                volClusts[V].back() += ioni;
        }
    }
    
    if(!volClusts.size()) return false; // out of data!
    
    // merge and sort into single detector history
    merged.clear();
    for(auto it = volClusts.begin(); it != volClusts.end(); it++)
        merged.insert(merged.end(), it->second.begin(), it->second.end());
    std::sort(merged.begin(), merged.end(), compare_hit_times);
    
    return true;
}
