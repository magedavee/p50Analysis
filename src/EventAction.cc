#include "EventAction.hh"
#include "RootIO.hh"
#include <TClonesArray.h>

#include "RunAction.hh"
#include "SteppingAction.hh"
#include "Event.hh"

#include <G4Event.hh>
#include <G4RunManager.hh>
#include <Randomize.hh>
#include <G4ios.hh>

void EventAction::BeginOfEventAction(const G4Event* anEvent) {
    timer.Start();
    
    // Displays event number and random seed for select events - gives user a progress report
    G4int eventNumber = anEvent->GetEventID();
    if( (eventNumber <= 1e5 && !(eventNumber % 1000))
        || (eventNumber <= 1e6 && !(eventNumber % 10000))
        || !(eventNumber % 100000)) {
        G4cout << "   Beginning of event: " << eventNumber << G4endl;
        CLHEP::HepRandom::showEngineStatus();
    }
    
    // Clear prior event data (keeping time set by event generator)
    double tGen = RootIO::GetEvent().t;
    RootIO::GetInstance()->Clear();
    RootIO::GetEvent().N = eventNumber;
    RootIO::GetEvent().t = tGen;
}

bool EventAction::keepEvent() const {
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    G4int reclevel = run_action->GetRecordLevel();
    if(reclevel >= 5
        || (reclevel == 3 && RootIO::GetFlux().nParticles > 0)
        || (reclevel >= 2 && RootIO::GetScIoni().nIoniClusts > 0)) return true;
    if(reclevel == -1 && RootIO::GetScIoni().nIoniClusts > 0) { // IBD candidates only
        vector<IoniCluster> scintHits;
        mergeIoniHits(RootIO::GetScIoni().clusts, scintHits, 100);
        int nNcapt = 0;
        int nIoni = 0;
        for(auto its = scintHits.begin(); its != scintHits.end(); its++) {
            HitTypeID tp = classifyHit(*its);
            if(tp==NCAPT_HIT) nNcapt++;
            if(tp==IONI_HIT) nIoni++;
        }
        return nNcapt && nIoni;
    }
    return false;
}

void EventAction::EndOfEventAction(const G4Event* anEvent) {
    timer.Stop();
    
    // Save event data
    if(keepEvent()) {
        
        // record event primaries information
        ParticleEvent& prim = RootIO::GetPrim();
        for(G4int i=0; i<anEvent->GetNumberOfPrimaryVertex(); i++) {
            const G4PrimaryVertex* v = anEvent->GetPrimaryVertex(i); 
            assert(v);
            ParticleVertex p;
            for(unsigned int j=0; j<3; j++) p.x[j] = v->GetPosition()[j];
            p.t = v->GetT0();
            for(G4int pn = 0; pn < v->GetNumberOfParticle(); pn++) {
                const G4PrimaryParticle* pp = v->GetPrimary(pn);
                assert(pp);
                G4ThreeVector mom = pp->GetMomentum();
                G4double mass = pp->GetMass();
                p.E = sqrt(mom.mag2()+mass*mass)-mass;
                mom = mom.unit();
                for(unsigned int j=0; j<3; j++) p.p[j] = mom[j];
                p.PID = pp->GetPDGcode();
                prim.AddParticle(p);
            }
        }
        
        // sort neutrons by time
        RootIO::GetNCapt().nCapts->Sort();
        
        // record computation time and flags
        SteppingAction* sa = (SteppingAction*)(G4RunManager::GetRunManager()->GetUserSteppingAction());
        if(sa->isTrapped) RootIO::GetEvent().flg |= Event::EVT_TRAPPED;
        RootIO::GetEvent().ct = sa->timeSpentSoFar;
            
        RootIO::GetInstance()->FillTree();
    }
}

double EventAction::getCPUTime() {
    timer.Stop();
    double t = timer.CpuTime();
    timer.Continue();
    return t;
}
