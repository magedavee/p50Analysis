#!/usr/bin/python

from optparse import OptionParser
import os
from math import *
from random import shuffle

class dumbsubmitter:
    def __init__(self,nm):
        self.jobname = nm
    def start_index(self):
        return 0
    def run_jobs(self, jcmd, r0, r1):
        for rn in range(r0,r1):
            cmd = jcmd%{"jobnum":"%i"%rn}
            print cmd
            os.system(cmd)

class parallelsubmitter:
    def __init__(self,nm):
        self.jobname = nm
    
    def start_index(self):
        return 0
    
    def run_jobs(self, jcmd, r0, r1):
        parallel_jobfile = "jobs_%s.txt"%self.jobname
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
#PBS -N %(jobname)s
#PBS -q exclusive"""
    
    def start_index(self):
        return 1
    
    def run_jobs(self, jcmd, r0, r1):
        subcmd = (self.setup + "\n#PBS -t %i-%i"%(r0+1, r1) + "\n%(xcmds)s\n")%self.settings
        subcmd += "source ${HOME}/.bashrc\n"
        subcmd += jcmd%{"jobnum":"${PBS_ARRAYID}"} + "\n"
        open("job_submit","w").write(subcmd)
        os.system("cat job_submit")
        os.system("qsub -p -500 job_submit")
        os.system("rm job_submit")
            
class SB_MC_Launcher:
    
    def __init__(self, simname, nevt):
        self.settings = {"nevents":nevt, "run_num":0}
        self.settings["simName"] = simname
        self.settings["preinit"] = ""
        self.settings["reclevel"] = 2
        self.settings["out_sfx"] = "root"
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
            self.settings["outfile"] = "%s/%s.%s"%(self.outdir, rname, self.settings["out_sfx"])
            
            # fill in macro template
            macrodat = open(self.template,"r").read()%self.settings
            open(os.path.expanduser("%s/%s.mac"%(self.macro_dir,rname)),"w").write(macrodat)
            
        # make job command and launch
        jcmd = "%s %s/%s.mac > %s/%s.txt 2>&1"%(self.bin_name, self.macro_dir, run_name, self.log_dir, run_name)
        self.submitter.run_jobs(jcmd,rnmin,nruns)

class H5_DetResponse_Launcher:
    def __init__(self, simname, forceParallel = False):
        self.settings = {"simName": simname}
        self.settings["xargs"] = ""
        if not forceParallel and not os.system("which qsub"):
            self.submitter = qsubmitter(self.settings["simName"]+"_Response")
        elif not os.system("which parallel"):
            self.submitter = parallelsubmitter(self.settings["simName"]+"_Response")
        else:
            self.submitter = dumbsubmitter(self.settings["simName"]+"_Response")
    def launch_converter(self):
        rmin = 10000000
        rmax = -1
        basedir = os.environ["PG4_OUTDIR"]+"/"+self.settings["simName"]
        flist = [f for f in os.listdir(basedir) if f[:4] == "Run_" and f[-3:] == ".h5" and f.split("_")[-1] != "DetSim.h5"]
        for f in flist:
            rnum = int(f.split("_")[-1].split(".")[0])
            if rnum < rmin:
                rmin = rnum
            if rnum > rmax:
                rmax = rnum
        rmin = 0
        cmdinfo = {"basedir":basedir, "p2xdir":os.environ["MPM_P2X_ANALYSIS"], "xargs":self.settings["xargs"]}
        jcmd = "if test -f %(basedir)s/Run_%%(jobnum)s.h5.xml; then %(p2xdir)s/Examples/CalcDetectorResponse %(basedir)s/Run_%%(jobnum)s.h5 %(xargs)s; fi"%cmdinfo
        self.submitter.run_jobs(jcmd,rmin,rmax-rmin+1)

def logrange(n,x0,x1):
    return [exp(log(x0)*(1-l)+log(x1)*l) for l in [x/float(n-1) for x in range(n)]]
