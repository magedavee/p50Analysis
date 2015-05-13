#include "DetectorResponse.hh"
#include "XMLInfo.hh"
#include <cmath>
#include <TRandom3.h>
#include <cassert>

/// comparison function for time-sorting hits
bool compare_hit_times(const s_PhysPulse& a, const s_PhysPulse& b) { return a.t < b.t; }

/// plot pre-mapping PSD parameter
void plot_PSD(const string& basedir) {
    TH2F* hEnergyPSD = new TH2F("hEnergyPSD","PSD approximation input", 100, 0, 10, 100, 0, 1);
    hEnergyPSD->GetXaxis()->SetTitle("quenched energy [MeVee]"); 
    hEnergyPSD->GetYaxis()->SetTitle("PSD proxy");
    
    OutDirLoader ODL(getEnvSafe("PG4_OUTDIR") + "/" + basedir);
    auto flist = ODL.getFileList(".h5");
    
    DetectorResponse DR;
    DR.psd_a = 0.10;
    DR.PSD_gamma0 = DR.PSD_gamma = 0;
    DR.PSD_ncapt0 = DR.PSD_ncapt = 1;
    
    for(auto itf = flist.begin(); itf != flist.end(); itf++) {
        
        SimIoniReader SIR(*itf);
        while(SIR.ioni_reader.loadEvent()) {
            // calculate detector response
            DR.event_response.clear();
            for(auto it = SIR.ioni_reader.event_read.begin(); it != SIR.ioni_reader.event_read.end(); it++) {
                if(it->vol < 1000) DR.addIoni(*it); // skip muon veto hits
            }
            DR.clearWindow();
            for(auto it = DR.event_response.begin(); it != DR.event_response.end(); it++) {
                hEnergyPSD->Fill(it->E, it->PSD);
            }
        }
    }
    
    gStyle->SetOptStat("");
    hEnergyPSD->Draw("Col Z");
    gPad->SetLogz(true);
    gPad->Print("hEnergyPSD.pdf");
}

int main(int argc, char** argv) {
    
    if(argc < 2) {
        printf("Arguments: <PROSPECT-G4 HDF5 filename>\n");
        return -1;
    }
    
    if(argc == 3) {
        plot_PSD(argv[1]);
        return 0;
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
