#!/usr/bin/python

from JobLauncher import *

if __name__=="__main__":
    
    parser = OptionParser()
    parser.add_option("-k", "--kill", dest="kill", action="store_true", default=False, help="kill running jobs")
    parser.add_option("--p2k", dest="p2k", action="store_true", default=False, help="PROSPECT-2k simulations");
    parser.add_option("--p20", dest="p20", action="store_true", default=False, help="PROSPECT-20 simulations");
    parser.add_option("--dima", dest="dima", action="store_true", default=False, help="DIMA detector");
    parser.add_option("--h5resp", dest="h5resp", action="store", type="string", help="HDF5-based detector response");
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)
    
    #if options.p20:
    #    L = SB_MC_Launcher("P20_IBD", 2e4)
    #    L.template = "Templates/P2k_IBD.mac"
    #    L.settings["out_sfx"] = "h5"
    #    L.launch_sims(40)
        
    if options.p2k:
        # IBD: 1e4 = 15min
        # 1e6 = ~15min for CRY, ~20min for neutrons
        L = SB_MC_Launcher("P2k_IBD", 1e6)
        L.template = "Templates/P2k_IBD.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(36*10)
        
    if options.dima:
        L = SB_MC_Launcher("DIMA-Co60", 1e6)
        L.template = "Templates/DIMA_Co60.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(96)
    
    if options.h5resp:
        L = H5_DetResponse_Launcher(options.h5resp)
        L.launch_converter()
