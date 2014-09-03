#include "EventAction.hh"
#include "RootIO.hh"

#include "RunAction.hh"
#include "Event.hh"

#include <G4Event.hh>
#include <G4RunManager.hh>
#include <Randomize.hh>

#include <G4ios.hh>

void EventAction::BeginOfEventAction(const G4Event* anEvent) {
    
    // Displays event number and random seed for select events - gives user a progress report
    G4int eventNumber = anEvent->GetEventID();
    if(!(eventNumber % 1000)) {
        G4cout << "   Beginning of event: " << eventNumber << G4endl;
        CLHEP::HepRandom::showEngineStatus();
    }
    
    // Clear prior event data
    RootIO::GetInstance()->Clear();
    RootIO::GetEvent().N = eventNumber;
}

void EventAction::EndOfEventAction(const G4Event* anEvent) {
    // Save event data
    RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
    G4int reclevel = run_action->GetRecordLevel();
    if(reclevel >= 3 || (reclevel >= 2 &&  RootIO::GetNCapt().nNCapts +  RootIO::GetScIoni().nIoniClusts > 0)) {
        
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
        
        RootIO::GetInstance()->FillTree();
    }
}
