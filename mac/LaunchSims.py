#!/usr/bin/python

from optparse import OptionParser
import os
from math import *
from random import shuffle

class SB_MC_Launcher:
    
    def __init__(self, simname, nevt):
        self.settings = {"nevents":nevt, "run_num":0}
        self.settings["simName"] = simname
        self.settings["preinit"] = ""
        self.settings["reclevel"] = 2
        self.template = "CRY_Template.mac"
        self.vary_E = None
        self.bin_name = os.environ["PG4_BIN"]
        
    def set_dirs(self):
        self.outdir = os.environ["PG4_OUTDIR"]+"/"+self.settings["simName"]
        self.auxout = os.environ["PG4_AUXOUT"]
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
        os.system("nice -n 15 parallel < "+parallel_jobfile)
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
    parser.add_option("--sphere", dest="sphere", action="store_true", default=False);
    parser.add_option("--acorn", dest="acorn", action="store_true", default=False);
    parser.add_option("--p2", dest="p2", action="store_true", default=False, help="PROSPECT-2 backgrounds");
    parser.add_option("--dima", dest="dima", action="store_true", default=False);
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)

    if options.cry:
        L = SB_MC_Launcher("CRY_TinyPinwheeled", 1e6)
        L.settings["preinit"] += "/geom/tank/nSegX 3\n"
        L.settings["preinit"] += "/geom/tank/nSegY 4\n"
        L.settings["preinit"] += "/geom/building/makeBare\n"
        L.settings["preinit"] += "/geom/shield/clear\n"
        L.settings["preinit"] += "/geom/pwrod/width 4 cm\n"
        L.settings["preinit"] += "/geom/pwrod/r_hole 1.5 cm\n"
        L.settings["preinit"] += "/geom/separator/thick 1 cm\n"
        L.launch_sims(4)
        
    if options.muveto:
        L = SB_MC_Launcher("CRY_MuVeto_dEdx", 1e6)
        #L.settings["reclevel"] = 3
        L.settings["preinit"] += "/geom/shield/muveto 4 cm\n"
        L.launch_sims(4*6*200,4*6*100)
    
    if options.testcell:
        L = SB_MC_Launcher("TestCell_Quenched", 1e4)
        L.template = "TestCell_Template.mac"
            
        L.settings["generator"] = "/generator/module/Cf252"
            
        #L.settings["preinit"] += "/geom/testcell/radius 6.35 cm\n"
        #L.settings["preinit"] += "/geom/testcell/length 20 cm\n"
        #L.settings["preinit"] += "/geom/testcell/loading 0\n"
            
        L.launch_sims(4*2)
        
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
                L.launch_sims(4*2)
                
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
        
    if options.sphere:
        L = SB_MC_Launcher("Sphere_Poly", 1e4)
        L.template = "spheretest.mac"
        L.launch_sims(4*4)
        
    if options.p2:
        # 1e7 for CRY, 1e6 for P2/P20 neutrons (no building)
        L = SB_MC_Launcher("P2_nBG", 1e6)
        L.template = "Analysis/Private/PR2_Template.mac"
        L.launch_sims(400)
        
        #L = SB_MC_Launcher("PROSPECT-2_gamma_Aug28P-Bare-Isot", 1e6)
        #L.template = "Analysis/Private/PR2_Gamma_Template.mac"
        #L.launch_sims(4)
        
    if options.acorn:
        for src in ["Bi207", "Sn113", "Cd109"][:1]:
            for ptcl in [("e", 11, 1e5), ("gamma", 22, 2e5)][:1]:
                L = SB_MC_Launcher("aCORN_"+src+"_"+ptcl[0], ptcl[2])
                L.bin_name = "/home/mpmendenhall/Applications/PG4-aCORN/bin/PROSPECT-G4"
                L.template = "aCORN_Template.mac"
                L.settings["src"] = src
                L.settings["PID"] = ptcl[1]
                L.launch_sims(4*10)
    
    if options.dima:
        L = SB_MC_Launcher("DIMA_Co60", 1e6)
        L.template = "Analysis/Private/DIMA_Template.mac"
        L.launch_sims(100)
        