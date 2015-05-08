/// \file SelectH5Events.hh \brief Selects HDF5 data from directory of simulations, based on directory of event listings, into one combined file.
#include "HDF5Reader.hh"
#include "XMLInfo.hh"

// ./SelectH5Events P2k_nBG

int main(int argc, char** argv) {
    string inDir, evtDir, outFile;
    if(argc == 4) {
        inDir = argv[1];
        evtDir = argv[2];
        outFile = argv[3];
    } else if(argc == 2) {
        evtDir = inDir = getEnvSafe("PG4_OUTDIR")+"/"+argv[1];
        outFile = evtDir + "/SelectedEvents.h5";
    } else {
        printf("Arguments: <sim name> | <event list input directory> <simulation .h5 output directory> <output .h5 filename>\n");
        return 0;
    }
    
   
    OutDirLoader ODL(inDir);
    auto rlist = ODL.getRunlist();
    if(!rlist.size()) { printf("No files found!\n"); return 0; }
    
    SimEventSelector SES;
    SES.setOutfile(outFile);
    
    printf("Preparing to load events from %zu files...\n", rlist.size());
    double totalTime = 0;
    for(auto it = rlist.begin(); it != rlist.end(); it++) {
        SES.setInfile(evtDir+"/Run_"+to_str(*it)+".h5");
        printf("(%i)\t", *it);
        SES.transfer(inDir+"/Run_"+to_str(*it)+"_EvtList.txt");
        totalTime += ODL.getXML(*it)->getGenTime()*1e-9;
    }
    printf("Selected %lli events from %.2f h simulated time.\n", SES.nTransferred, totalTime/3600);
    SES.setTotalTime(totalTime);
    
    return 0;
}
