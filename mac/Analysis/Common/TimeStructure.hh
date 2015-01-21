#ifndef TIMESTRUCTURE_HH
#define TIMESTRUCTURE_HH

#include "AnaUtils.hh"

/// identified hit classifications
enum HitTypeID {
    IONI_HIT,       ///< electron/gamma ionization
    NCAPT_HIT,      ///< neutron capture hit
    RECOIL_HIT,     ///< nucleus recoil
    VETO_HIT,       ///< veto detector hit
    DEAD_HIT,       ///< dead volume or non-detected hit
    CRAZY_HIT       ///< unclassifiable event
};

/// Prompt time-correlated cluster of hits
class HitCluster {
public:
    /// Constructor
    HitCluster() { }
    /// tally hit types and times from myHits
    void tallyHits();
    /// whether this is a simple neutron capture
    bool isNcapt() const { return hitTypes.size() == 1 && hitTypes.count(NCAPT_HIT) == 1; }
    /// whether this is a simple ionization hit (with volume multiplicity)
    int isIoni() const { return hitTypes.size() == 1 ? hitTypes.count(IONI_HIT) : 0; }
    /// whether this cluster includes a veto signal
    bool hasVeto() const { return hitTypes.count(VETO_HIT); }
    
    vector<IoniCluster> myHits;         ///< hits in cluster
    map<HitTypeID,int> hitTypes;        ///< counter for each hit type
    double t_median;                    ///< representative median event time
    double Eioni = 0;                   ///< total ionization hit energy
};

/// Delayed coincident events group
class CoincidenceEvent {
public:
    /// Constructor
    CoincidenceEvent() { }
    
    /// add a hit if timescale allows; otherwise, return false
    bool addHit(IoniCluster h, double prompt_timescale, double delayed_timescale);
    /// finalize calculations after last hit is added
    void finalize();
    /// general IBD-like: isolated neutron capture followed by (multi-segment) electron-like
    int isIBDlike() const { return (promptClusters.size() == 2 && promptClusters[0].isNcapt())? promptClusters[1].isIoni() : 0; }
    
    /// get all (electron ionization, ncapt) pairs
    vector< pair<HitCluster,HitCluster> > getIBDPairs() const;
    
    vector<HitCluster> promptClusters;  ///< clusters of promptly-time-correlated events
};

/// Class for analyzing time-correlated event observables
class TimeStructureAnalyzer {
public:
    /// Constructor
    TimeStructureAnalyzer(): ionc(new IoniClusterEvent()) { }
    /// Destructor
    virtual ~TimeStructureAnalyzer() { if(ionc) delete ionc; }
    
    /// classify scintillator hits into identified event types
    virtual void classifyHits();
  
    IoniClusterEvent* ionc;             ///< event being analyzed (set to TTree branch readpoint)
    
    // analysis settings
    double prompt_timescale = 100;      ///< timescale for grouping hits into prompt correlated clusters [ns]
    double delayed_timescale = 300e3;   ///< timescale for grouping events into delayed correlated systems [ns]
    double ncapt_Emin = 0.40;           ///< lower bound for identifying neutron capture peak [MeVee]
    double ncapt_Emax = 0.60;           ///< upper bound for identifying neutron capture peak [MeVee]
    double trig_thresh = 0.10;          ///< effective segment trigger threshold [MeVee]; ignore lesser ionizations
    double veto_thresh = 0.50;          ///< muon veto trigger threshold
    
    // time-structured results
    vector<CoincidenceEvent> coincs;    ///< events in delayed-coincidence groups
};

#endif
