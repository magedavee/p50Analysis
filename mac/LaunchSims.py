#!/usr/bin/python

from optparse import OptionParser
import os
from math import *
from random import shuffle

class parallelsubmitter:
    def __init__(self,nm):
        self.jobname = nm
    
    def start_index(self):
        return 0
    
    def run_jobs(self, jcmd, r0, r1):
        parallel_jobfile = "jobs.txt"
        jobsout = open(parallel_jobfile,"w")
        for rn in range(r0,r1):
            jobsout.write(jcmd%{"jobnum":"%i"%rn} + "\n")
        jobsout.close()
        print "Running simulation jobs..."
        os.system("cat "+parallel_jobfile)
        os.system("nice -n 15 parallel < "+parallel_jobfile)
        os.system("rm "+parallel_jobfile)
        
class qsubmitter:
    def __init__(self,nm):
        self.settings = {"jobname":nm, "xcmds":""}
        self.setup = """#!/bin/bash
#PBS -j oe
#PBS -N %(jobname)s"""
        for e in ["PG4_AUX"]:
            self.setup += "\n#PBS -v %s=%s"%(e,os.environ[e])
    
    def start_index(self):
        return 1
    
    def run_jobs(self, jcmd, r0, r1):
        open("job_submit","w").write((self.setup + "\n#PBS -t %i-%i"%(r0+1, r1) + "\n%(xcmds)s\n")%self.settings + jcmd%{"jobnum":"${PBS_ARRAYID}"} + "\n")
        os.system("cat job_submit")
        os.system("qsub -p -500 job_submit")
        os.system("rm job_submit")
            
class SB_MC_Launcher:
    
    def __init__(self, simname, nevt):
        self.settings = {"nevents":nevt, "run_num":0}
        self.settings["simName"] = simname
        self.settings["preinit"] = ""
        self.settings["reclevel"] = 2
        self.template = "CRY_Template.mac"
        self.vary_E = None
        self.bin_name = os.environ["PG4_BIN"]
        if not os.system("which qsub"):
            self.submitter = qsubmitter(self.settings["simName"])
        else:
            self.submitter = parallelsubmitter(self.settings["simName"])
        
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
        run_name = "Run_%(jobnum)s"
        i0 = self.submitter.start_index()
         
        # set up macros
        for rn in range(rnmin, nruns):
            
            self.settings["run_num"]= rn+i0
    
            if self.vary_E:
                self.settings["gun_energy"] = self.vary_E[rn]
            
            rname = run_name%{"jobnum":"%i"%self.settings["run_num"]}
            self.settings["outfile"] = "%s/%s.root"%(self.outdir, rname)
            
            # fill in macro template
            macrodat = open(self.template,"r").read()%self.settings
            open(os.path.expanduser("%s/%s.mac"%(self.macro_dir,rname)),"w").write(macrodat)
            
        # make job command and launch
        jcmd = "%s %s/%s.mac > %s/%s.txt 2>&1"%(self.bin_name, self.macro_dir, run_name, self.log_dir, run_name)
        self.submitter.run_jobs(jcmd,rnmin,nruns)


def logrange(n,x0,x1):
    return [exp(log(x0)*(1-l)+log(x1)*l) for l in [x/float(n-1) for x in range(n)]]
    
if __name__=="__main__":
    
    parser = OptionParser()
    parser.add_option("-k", "--kill", dest="kill", action="store_true", default=False, help="kill running jobs")
    parser.add_option("--p2", dest="p2", action="store_true", default=False, help="PROSPECT-2 backgrounds");
    parser.add_option("--p20yale", dest="p20yale", action="store_true", default=False, help="Bare PROSPECT-20 'Yale' cell");
    
    options, args = parser.parse_args()
    if options.kill:
        os.system("killall -9 parallel")
        exit(0)
        
    if options.p2:
        # 1e7 for CRY, 1e6 for P2/P20 neutrons (no building)
        L = SB_MC_Launcher("Test", 1e6)
        L.template = "Analysis/Private/PR2_Template.mac"
        L.launch_sims(100)
        
    if options.p20yale:
        L = SB_MC_Launcher("P20-Yale-Cf252_n", 1e5)
        L.template = "Analysis/Private/P20-Yale.mac"
        L.launch_sims(40)
