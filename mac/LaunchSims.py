#!/usr/bin/python

from optparse import OptionParser
import os

class SB_MC_Launcher:
    
    def __init__(self, simname, nevt):
        self.settings = {"nevents":nevt, "run_num":0}
        self.n_parallel = 4
        self.settings["simName"] = simname
        
    def set_dirs(self):
        self.bin_name = os.environ["SBMC_BIN"]
        self.outdir = os.environ["SBMC_OUTDIR"]+"/"+self.settings["simName"]
        self.auxout = os.environ["SBMC_AUXOUT"]
        self.macro_dir = self.auxout+"/mac/"+self.settings["simName"]
        self.log_dir = self.auxout+"/log/"+self.settings["simName"]
        
        os.system("mkdir -p %s"%self.outdir)
        os.system("mkdir -p %s"%self.macro_dir)
        os.system("mkdir -p %s"%self.log_dir)
        
        
    def launch_sims(self, nruns):
        
        self.set_dirs()
        
        # set up macros for each job
        parallel_jobfile = "%s/jobs.txt"%self.log_dir
        jobsout = open(parallel_jobfile,"w")
        for rn in range(nruns):
            self.settings["run_num"] += 1
            run_name = "Run_%i"%self.settings["run_num"]
            self.settings["outfile"] = "%s/%s.root"%(self.outdir, run_name)
            
            # fill in macro template
            macrodat = open("CRY_Template.mac","r").read()%self.settings
            open(os.path.expanduser("%s/%s.mac"%(self.macro_dir,run_name)),"w").write(macrodat)
            
            # make job command
            onejob = "%s %s/%s.mac"%(self.bin_name, self.macro_dir, run_name)
            jobsout.write(onejob+" > %s/%s.txt 2>&1\n"%(self.log_dir, run_name))
        
        jobsout.close()
        
        print "Running simulation jobs..."
        os.system("cat "+parallel_jobfile)
        os.system("nice -n 15 parallel -P %i < "%(self.n_parallel)+parallel_jobfile)
        os.system("rm "+parallel_jobfile)



if __name__=="__main__":
    
    parser = OptionParser()
    parser.add_option("-k", "--kill", dest="kill", action="store_true", default=False, help="kill running jobs")
    parser.add_option("--cry", dest="cry", action="store_true", default=False, help="CRY cosmic ray simulations")
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)

    if options.cry:
        L = SB_MC_Launcher("CRY_nCapt", 1e5)
        L.launch_sims(120)
        