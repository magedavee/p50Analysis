#!/usr/bin/python

from JobLauncher import *

if __name__=="__main__":
    
    parser = OptionParser()
    parser.add_option("-k", "--kill", dest="kill", action="store_true", default=False, help="kill running jobs")
    parser.add_option("--p2k", dest="p2k", action="store_true", default=False, help="PROSPECT-2k simulations");
    parser.add_option("--p20", dest="p20", action="store_true", default=False, help="PROSPECT-20 simulations");
    parser.add_option("--dima", dest="dima", action="store_true", default=False, help="DIMA detector");
    parser.add_option("--h5resp", dest="h5resp", action="store", type="string", help="HDF5-based detector response");
    parser.add_option("--np", dest="np", action="store_true", default=False, help="non-parallel execution");
    parser.add_option("--misc", dest="misc", action="store_true", default=False, help="Miscellaneous sims");
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)
    
    if options.p20:
        # nBG: 2e5 in 50min in building+WB (=40s sim); 4e5 in 22min open building (=500s sim)
        # muBG: 4e6 in 33min in building+WB (=37m sim); 1e7 in XX min open building (= sim)
        L = SB_MC_Launcher("P20_muBG_bld", 4e6)
        L.template = "Templates/P20_muBG.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(96*10)
        
    if options.p2k:
        # IBD: 1e4 = 15min
        # 1e6 = ~15min for CRY
        # nBG: 1e6 ~20min without building; ~4.4h with building (=138s sim)
        L = SB_MC_Launcher("P2k_nBG_bld", 1e6)
        L.template = "Templates/P2k_nBG.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(36*30)
        
    if options.dima:
        L = SB_MC_Launcher("DIMA-Co60", 1e6)
        L.template = "Templates/DIMA_Co60.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(96)

    if options.misc:
        L = SB_MC_Launcher("AlphaSi", 1e6)
        L.template = "Analysis/Private/AlphaSi_Template.mac"
        L.settings["out_sfx"] = "h5"
        L.launch_sims(10*4) 
    
    if options.h5resp:
        L = H5_DetResponse_Launcher(options.h5resp, options.np)
        L.launch_converter()
