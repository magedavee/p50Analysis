#include "DetectorResponse.hh"
#include "HDF5_StructInfo.hh"
#include "XMLInfo.hh"
#include <cmath>
#include <TRandom3.h>
#include <cassert>

void DetectorResponse::addIoni(const s_IoniCluster& h) {
    if(!h.E) return; // skip zero-energy neutrino tags
    TimedIoniCluster* tc = new TimedIoniCluster(h);
    if(P20reflectorless && h.vol == 1) tc->vol = 0;
    addSingle(tc);
}

double DetectorResponse::calcQuench(const s_IoniCluster& evt) const {
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    return evt.E / (1 + c_1*evt.EdEdx/evt.E + c_2*evt.EdEdx2/evt.E);
}

void DetectorResponse::quenchPSD(const s_IoniCluster& evt, double& Equench, double& PSD) const {
    // PSD, scaling from raw variable u to data-like values
    // a = 0.06 -> .01, .89
    // a = 0.08 -> .02, .92
    // a = 0.10 -> .02, .935
    const double a = 0.10;
    const double u = atan(a*evt.EdEdx/evt.E)*2/M_PI;
    const double PSD_gamma = 0.22;      // target "gamma-like" PSD value
    const double PSD_ncapt = 0.363;     // target neutron capture PSD value
    PSD = PSD_gamma + (u-0.02)/(0.935-0.02)*(PSD_ncapt - PSD_gamma);

    // interpolate between quenched and unquenched energy
    Equench = calcQuench(evt);
    const double u_upper = 0.25;
    const double u_lower = 0.1;
    if(u <= u_lower) Equench = evt.E;
    else if(u < u_upper) Equench = Equench*(u-u_lower)/(u_upper - u_lower) + evt.E*(u_upper-u)/(u_upper - u_lower);
}

s_PhysPulse DetectorResponse::genResponse(const s_IoniCluster& evt) const {
    s_PhysPulse p;
    double PSD, Equench;
    
    p.evt = evt.evt;
    p.seg = evt.vol;
    quenchPSD(evt,Equench,PSD);
    p.E = Equench;
    assert(evt.t == evt.t); // NaN check
    p.t = evt.t;
    p.y = evt.x[cellaxis];
    p.PSD = PSD;
    
    return p;
}

void DetectorResponse::processMid(TimedObject* O) {
    TimedCluster* C = dynamic_cast<TimedCluster*>(O);
    assert(C);
    
    /// sum hits grouped by volume and particle ID
    map<int, map<int, s_IoniCluster> > volHits;
    for(auto it = C->objs().begin(); it != C->objs().end(); it++) {
        TimedIoniCluster* h = dynamic_cast<TimedIoniCluster*>(*it);
        assert(h);
        // special case: combine alpha, triton due to current quenching calculation. TODO: fix quenching calc so these are separate.
        if(h->PID == 1000020040) h->PID = 1000010030;
        
        map<int, s_IoniCluster>& m = volHits[h->vol];
        auto prevHit = m.find(h->PID);
        if(prevHit == m.end()) m.insert(std::pair<int,s_IoniCluster>(h->PID, *h));
        else prevHit->second += *h;
    }
    
    /// quench and merge hits in each volume
    double PSD, Equench;
    for(auto it = volHits.begin(); it != volHits.end(); it++) {
        s_IoniCluster volIoni;  // quenched-energy-summed response from different particles
        s_PhysPulse volPhys;    // detector physics response
        double EqPSD = 0;       // quenched-energy-weighted average PSD
        for(auto ith = it->second.begin(); ith != it->second.end(); ith++) {
            volPhys.evt = ith->second.evt;              // preserve MC event number (should be same for all *ith)
            quenchPSD(ith->second, Equench, PSD);       // calculate individual particle quenching/PSD
            EqPSD += Equench*PSD;
            ith->second.E = Equench;                    // replace energy with quenched energy for averaging different particles
            volIoni += ith->second;
        }
        if(!volIoni.E) continue;        // ignore zero-energy hits
        volPhys.seg = it->first;
        volPhys.E = volIoni.E;          // summed quenched energy
        volPhys.t = volIoni.t;
        volPhys.y = volIoni.x[cellaxis];
        volPhys.PSD = EqPSD/volIoni.E;
        event_response.push_back(volPhys);
    }
}

int main(int argc, char** argv) {

    if(argc != 2) {
        printf("Arguments: <PROSPECT-G4 HDF5 filename>\n");
        return -1;
    }
    
    string f_in = argv[1];
    string fext = split(f_in,".").back(); // file extension
    auto pathels = split(f_in,"/");
    assert(pathels.size() >= 2);
    string detname = split(pathels[pathels.size()-2],"_-")[0]; // simulation detector
    assert(detname == "P20" || detname == "P2k" || detname == "DIMA");
    assert(fext == "h5");
    string fbase = f_in.substr(0, f_in.size()-fext.size()-1); // base filename less extension
    string f_out = fbase+"_DetSim."+fext;
    
    bool fullsort = true; // whether to cache all events in memory and time-sort
    
    // open input file
    SimIoniReader SIR(f_in);
    XMLInfo XI(f_in+".xml");
    double runtime = XI.getGenTime(); // [ns]
    assert(runtime && runtime==runtime);
    size_t runthrows = XI.getEvts();
    printf("Input file for %zu primary events in %.1f seconds simulated time.\n", runthrows, runtime*1e-9);
    
    // set up output file
    hid_t outfile_id = H5Fcreate(f_out.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    assert(outfile_id >= 0);
    int nchunk = 1024;
    herr_t err = H5TBmake_table("Simulated detector response", outfile_id, "PhysPulse",
                                HDF5_StructInfo::n_PhysPulse_fields, 0, sizeof(s_PhysPulse),
                                HDF5_StructInfo::PhysPulse_field_names, HDF5_StructInfo::PhysPulse_offsets, HDF5_StructInfo::PhysPulse_field_types,
                                nchunk, NULL, 9, NULL);
    assert(err >= 0);
    HDF5_Table_Writer<s_PhysPulse> pulse_writer("PhysPulse", HDF5_StructInfo::PhysPulse_offsets, HDF5_StructInfo::PhysPulse_sizes, nchunk);
    pulse_writer.setFile(outfile_id);
    
    // convert events to detector response
    DetectorResponse DR;
    if(detname == "P20" || detname == "DIMA") DR.cellaxis = 1;
    DR.P20reflectorless = (detname == "P20");
    size_t nMerged = 0;
    vector<s_PhysPulse> allPulses;
    TRandom3 r;
    while(SIR.ioni_reader.loadEvent()) {
        //if(!(SIR.getNRead() % (SIR.nrecords/20))) { cout << "*"; cout.flush(); }
        // calculate detector response
        DR.event_response.clear();
        for(auto it = SIR.ioni_reader.event_read.begin(); it != SIR.ioni_reader.event_read.end(); it++) {
            if(it->vol < 1000) DR.addIoni(*it); // skip muon veto hits
        }
        DR.clearWindow();
        // record results, with optional global time offset
        double evttime = r.Rndm()*runtime; // uniform random time offset for event cluster
        for(auto it = DR.event_response.begin(); it != DR.event_response.end(); it++) {
            if(!fullsort) pulse_writer.write(*it);
            else {
                it->t += evttime;
                while(it->t > runtime) it->t -= runtime*floor(it->t/runtime); // wrap around long-delayed events to start of run
                allPulses.push_back(*it);
            }
            nMerged++;
        }
    }
    // optional full time sorting
    if(allPulses.size()) {
        printf("\nMaster time merge and output of %zu pulses... ", allPulses.size()); fflush(stdout);
        std::sort(allPulses.begin(), allPulses.end(), compare_hit_times);
        pulse_writer.write(allPulses);
        printf("Done.");
    }
    printf("\nRead %llu ionizations into %zu merged events.\nOutput '%s'\n", SIR.getNRead(), nMerged, f_out.c_str());
    
    runtime /= 1e9; // convert to seconds
    err = H5LTset_attribute_double(outfile_id, "PhysPulse", "runtime", &runtime, 1);
    assert(err >= 0);
    
    pulse_writer.setFile(0);
    H5Fclose(outfile_id);
    return 0;
}
