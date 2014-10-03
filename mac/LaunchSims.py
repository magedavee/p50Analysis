#!/usr/bin/python

from optparse import OptionParser
import os
from math import *
from random import shuffle

class SB_MC_Launcher:
    
    def __init__(self, simname, nevt):
        self.settings = {"nevents":nevt, "run_num":0}
        self.n_parallel = 4
        self.settings["simName"] = simname
        self.settings["preinit"] = ""
        self.settings["reclevel"] = 2
        self.template = "CRY_Template.mac"
        self.vary_E = None
        
    def set_dirs(self):
        self.bin_name = os.environ["SBMC_BIN"]
        self.outdir = os.environ["SBMC_OUTDIR"]+"/"+self.settings["simName"]
        self.auxout = os.environ["SBMC_AUXOUT"]
        self.macro_dir = self.auxout+"/mac/"+self.settings["simName"]
        self.log_dir = self.auxout+"/log/"+self.settings["simName"]
        
        os.system("mkdir -p %s"%self.outdir)
        os.system("mkdir -p %s"%self.macro_dir)
        os.system("mkdir -p %s"%self.log_dir)
        
        
    def launch_sims(self, nruns, rnmin=0):
        
        self.set_dirs()
        
        # set up macros for each job
        parallel_jobfile = "%s/jobs.txt"%self.log_dir
        jobsout = open(parallel_jobfile,"w")
        
        for rn in range(nruns):
            
            self.settings["run_num"] += 1
            if rn < rnmin:
                continue
            
            if self.vary_E:
                self.settings["gun_energy"] = self.vary_E[rn]
            
            run_name = "Run_%i"%self.settings["run_num"]
            self.settings["outfile"] = "%s/%s.root"%(self.outdir, run_name)
            
            # fill in macro template
            macrodat = open(self.template,"r").read()%self.settings
            open(os.path.expanduser("%s/%s.mac"%(self.macro_dir,run_name)),"w").write(macrodat)
            
            # make job command
            onejob = "%s %s/%s.mac"%(self.bin_name, self.macro_dir, run_name)
            jobsout.write(onejob+" > %s/%s.txt 2>&1\n"%(self.log_dir, run_name))
        
        jobsout.close()
        
        print "Running simulation jobs..."
        os.system("cat "+parallel_jobfile)
        os.system("nice -n 15 parallel -P %i < "%(self.n_parallel)+parallel_jobfile)
        os.system("rm "+parallel_jobfile)


def logrange(n,x0,x1):
    return [exp(log(x0)*(1-l)+log(x1)*l) for l in [x/float(n-1) for x in range(n)]]
    
if __name__=="__main__":
    
    parser = OptionParser()
    parser.add_option("-k", "--kill", dest="kill", action="store_true", default=False, help="kill running jobs")
    parser.add_option("--cry", dest="cry", action="store_true", default=False, help="CRY cosmic ray simulations")
    parser.add_option("--muveto", dest="muveto", action="store_true", default=False, help="Muon veto layer simulations")
    parser.add_option("--testcell", dest="testcell", action="store_true", default=False, help="Scintillator test cell")
    parser.add_option("--nscatter", dest="nscatter", action="store_true", default=False, help="neutron scattering tests")
    parser.add_option("--proton", dest="proton", action="store_true", default=False, help="proton interactions tests")
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)

    if options.cry:
        L = SB_MC_Launcher("CRY_TinyPinwheeled", 1e6)
        L.settings["preinit"] += "/geom/tank/nSegX 3\n"
        L.settings["preinit"] += "/geom/tank/nSegY 3\n"
        L.settings["preinit"] += "/geom/building/makeBare\n"
        L.settings["preinit"] += "/geom/shield/clear\n"
        L.settings["preinit"] += "/geom/pwrod/width 4 cm\n"
        L.settings["preinit"] += "/geom/pwrod/r_hole 1.5 cm\n"
        L.settings["preinit"] += "/geom/separator/thick 1 cm\n"
        L.launch_sims(4*6*10)
        
    if options.muveto:
        L = SB_MC_Launcher("CRY_MuVeto", 1e5)
        #L.settings["reclevel"] = 3
        L.settings["preinit"] += "/geom/shield/muveto 4 cm\n"
        L.launch_sims(4*6*10)
    
    if options.testcell:
        L = SB_MC_Launcher("TestCell", 1e5)
        L.template = "TestCell_Template.mac"
        L.launch_sims(4*6*5)
        
    if options.nscatter:
        for E in ["10 MeV","1 keV","1 eV","0.02 eV"]:
            for (m,t) in [("Lithium6", "1 mm")]: # ("G4_WATER","1 cm"), ("Lithium6","1 um"), ("solidCl", "1 cm"), ("UG_AB", "1 cm")]:
                L = SB_MC_Launcher(("nScatter %s %s %s"%(m,t,E)).replace(" ","_"), 1e5)
                L.template = "ScatterSlab_Template.mac"
                L.settings["gun_energy"] = E
                L.settings["slab_mat"] = m
                L.settings["slab_thick"] = t
                L.settings["particle"] = "neutron"
                L.settings["reclevel"] = 3
                L.launch_sims(4*10)
                
    if options.proton:
        nruns = 4*100
        L = SB_MC_Launcher("protonScint", 5e4)
        L.template = "ScatterSlab_Template.mac"
        L.settings["particle"] = "proton"
        L.settings["slab_mat"] = "EJ309-0.1wt%-6Li"
        L.settings["slab_thick"] = "2 cm"
        L.vary_E = ["%.3f MeV"%x for x in logrange(nruns,1,2000)]
        shuffle(L.vary_E)
        L.launch_sims(nruns)
        