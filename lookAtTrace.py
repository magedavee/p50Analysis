from initROOT import initROOT
import ROOT
from ROOT import gROOT, TCanvas, TF1,TFile,TTree
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
from scipy.stats import norm 
from array import array
initROOT()
name="s005_f00000_ts1459345732.root"
print "looking at "+name
pmt=ROOT.PmtData(name)
pmt.CalIntegral(0)
pmt.CalIntegral(1)
ent=pmt.GetEntries()
integral=[]
for i in xrange(0,ent):
    pmt.SetEntry(i)
    charge=pmt.GetPulseIntegral(0,i)
    if charge>250000 and charge<500000:
	integral.append(charge)
	print charge
	pmt.GetTrace(0).Draw()
	raw_input()
# # pmt.GetIntegral(1).Draw()
# # pmt.GetIntegral(2).Draw()
print ""+str(len(integral))
plt.hist(integral,bins=1000)
plt.show()
