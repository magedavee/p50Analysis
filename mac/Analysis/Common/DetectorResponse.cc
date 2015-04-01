#include "DetectorResponse.hh"
#include "HDF5_StructInfo.hh"
#include "XMLInfo.hh"
#include <cmath>
#include <TRandom3.h>

void DetectorResponse::quenchPSD(const s_IoniCluster& evt, double& Equench, double& PSD) const {
    // PSD
    const double u = atan(0.2*evt.EdEdx/evt.E)*2/M_PI;
    const double PSD_gamma = 0.14;
    const double PSD_ncapt = 0.31;
    PSD = PSD_gamma + (u-0.04)/(0.98-0.04)*(PSD_ncapt - PSD_gamma);
    
    // quenched energy
    const double c_1 = 0.1049;
    const double c_2 = -8.72117e-05;
    Equench = evt.E / (1 + c_1*evt.EdEdx/evt.E + c_2*evt.EdEdx2/evt.E);
    
    // interpolate between quenched and unquenched energy
    const double u_upper = 0.9;
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
    p.t = evt.t;
    p.y = evt.x[2];
    p.PSD = PSD;
    
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
    size_t nMerged = 0;
    vector<s_PhysPulse> allPulses;
    TRandom3 r;
    while(SIR.loadMergedIoni()) {
        if(!(SIR.nRead % (SIR.nrecords/20))) { cout << "*"; cout.flush(); }
        double evttime = r.Rndm()*runtime; // uniform random time offset for event cluster
        for(auto it = SIR.merged.begin(); it != SIR.merged.end(); it++) {
            s_PhysPulse p = DR.genResponse(*it);
            if(!fullsort) pulse_writer.write(p);
            else {
                p.t += evttime;
                while(p.t > runtime) p.t -= runtime*floor(p.t/runtime); // wrap around long-delayed events to start of run
                allPulses.push_back(p);
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
    printf("\nRead %llu ionizations into %zu merged events.\nOutput '%s'\n", SIR.nRead, nMerged, f_out.c_str());
    
    runtime /= 1e9; // convert to seconds
    err = H5LTset_attribute_double(outfile_id, "PhysPulse", "runtime", &runtime, 1);
    assert(err >= 0);
    
    pulse_writer.setFile(0);
    H5Fclose(outfile_id);
    return 0;
}
