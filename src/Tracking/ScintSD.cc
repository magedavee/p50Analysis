#include "ScintSD.hh"

#include "RootIO.hh"
#include "RunAction.hh"
#include "ScintTankBuilder.hh"

#include <cassert>
#include <vector>

#include <G4SystemOfUnits.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>
#include <G4TouchableHandle.hh>
#include <G4Neutron.hh>
#include <G4Gamma.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4ProcessType.hh>
#include <G4HadronicProcessType.hh>
#include <G4ios.hh>

////////////////////////////////////////////////////////////////

void IonisationHit::Display() const {
    G4cerr << "Hit E=" << G4BestUnit(sum_w,"Energy") 
    << " at t=" << G4BestUnit(GetTime(),"Time") << "( " << G4BestUnit(GetDTime(),"Time")
    << ")\tx=[ " << G4BestUnit(GetPos(),"Length") << "] { " << G4BestUnit(GetDPos(),"Length") << "}" << G4endl;
}

////////////////////////////////////////////////////////////////

ScintSD::ScintSD(G4String name, ScintTankBuilder& T): G4VSensitiveDetector(name),
time_gap(20*ns), edep_threshold(100*keV), verbose(0), nclusters(0), myTank(T) { }

void ScintSD::Initialize(G4HCofThisEvent*) {
    verbose = G4RunManager::GetRunManager()->GetVerboseLevel();
    hit_history.clear();
    secondaries_counter.clear();
    nclusters = 0;
}

G4bool ScintSD::ProcessHits(G4Step* aStep, G4TouchableHistory* H) {
    
    G4TouchableHandle hitVol = aStep->GetPreStepPoint()->GetTouchableHandle();
    // check that we are in scintillator proper, not sub-assemblies therein
    G4LogicalVolume* log = hitVol->GetVolume()->GetLogicalVolume();
    if(log != myTank.scint_log) return false;
    
    // world and local position
    worldPos = aStep->GetPreStepPoint()->GetPosition();
    localPos = hitVol->GetHistory()->GetTopTransform().TransformPoint(worldPos);
    seg_id = myTank.getSegmentNum(localPos);
    
    G4bool notable = false;
    notable |= ProcessNeutronHits(aStep, H);
    notable |= ProcessIoniHits(aStep, H);
    secondaries_counter[aStep->GetTrack()->GetTrackID()] = aStep->GetSecondary()->size();
    
    return notable;
}

G4bool ScintSD::ProcessNeutronHits(G4Step* aStep, G4TouchableHistory*) {
    // Check that this is a neutron
    if(aStep->GetTrack()->GetDefinition() != G4Neutron::NeutronDefinition()) return false;
    
    // Check that this is a hadronic capture (or special case for 6Li inelastic scattering)
    const G4VProcess* PDS = aStep->GetPostStepPoint()->GetProcessDefinedStep();
    bool isCapt = PDS->GetProcessSubType() == fCapture;
    if(!(PDS && PDS->GetProcessType() == fHadronic && (isCapt || PDS->GetProcessSubType() == fHadronInelastic))) return false;
    
    if(verbose >= 2) PDS->DumpInfo();
    
    EventNCapt nc;
    for(uint i=0; i<3; i++) nc.x[i] = localPos[i];
    nc.t = aStep->GetPostStepPoint()->GetGlobalTime();
    nc.vol = seg_id;
    nc.E = aStep->GetPreStepPoint()->GetKineticEnergy();
    
    // tally secondaries; check for recoiling nucleus
    nc.Ngamma = 0;
    nc.Egamma = 0;
    nc.capt_Z = nc.capt_A = 0;
    nc.Nprod = 0;
    const G4TrackVector* secondaries = aStep->GetSecondary();
    assert(secondaries);
    G4TrackVector::const_iterator it = secondaries->begin() + secondaries_counter[aStep->GetTrack()->GetTrackID()];
    for(; it != secondaries->end(); it++) {
        nc.Nprod++;
        G4ParticleDefinition* pt = (*it)->GetDefinition();
        assert(pt);
        if(pt == G4Gamma::GammaDefinition()) {
            nc.Egamma += (*it)->GetKineticEnergy();
            nc.Ngamma++;
        } else {
            G4int Z = pt->GetAtomicNumber();
            G4int A = pt->GetAtomicMass();
            if(verbose >= 2) {
                G4cerr << "\tFragment: " << pt->GetParticleType() << "/" << pt->GetParticleSubType()
                << " (" << Z << "," << A << ") " << G4BestUnit((*it)->GetKineticEnergy(),"Energy") << G4endl;
            }
            nc.capt_Z += Z;
            nc.capt_A += A;
        }
    }
    
    if(isCapt || (nc.capt_Z == 3 && nc.capt_A == 7)) {
        RootIO::GetInstance()->GetEvent().AddNCapt(nc);
    
        if(verbose >= 2) {
            G4cerr << "Neutron ( KE=" << G4BestUnit(nc.E,"Energy") << ") capture at [ " << G4BestUnit(localPos,"Length") << "] with "
                << nc.Ngamma << " gammas ( E=" << G4BestUnit(nc.Egamma,"Energy")
                << ") of " << nc.Nprod << " final products; total (A,Z)=(" << nc.capt_A << "," << nc.capt_Z << ")" << G4endl;
        }
    
        return true;
        
    } else return false;
}

G4bool ScintSD::ProcessIoniHits(G4Step* aStep, G4TouchableHistory*) {
    
    // check whether this is ionizing process
    if(!(aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0
        //    || aStep->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition()
        || aStep->GetTrack()->GetDefinition() == G4Gamma::GammaDefinition() ) ) return false;
    
    G4double E = aStep->GetTotalEnergyDeposit()-aStep->GetNonIonizingEnergyDeposit();
    if(!E) return false;
    
    IonisationHit* aHit = new IonisationHit();
    aHit->SetVolume(seg_id);
    
    // Record total energy deposit if particle is charged (e-, e+, etc.)
    //if(aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0)
    aHit->SetEnergy(E);
    aHit->SetLength(aStep->GetStepLength());
    aHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime()+0.5*aStep->GetDeltaTime());
    aHit->SetPos(localPos);
    
    /* TODO: Need to implement direct energy deposit by gamma in case of high production threshold for e- */
    
    // If step leaves volume, record the particle energy at point of exit as the escape energy
    /*
     *   if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) { 
     *       if(aStep->GetTrack()->GetDefinition() == G4Positron::PositronDefinition())
     *           aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetTotalEnergy());
     *       else
     *           aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetKineticEnergy());
     }
     */
    
    // record hits to volume listing
    aHit->record();
    hit_history[aHit->GetVolume()].push_back(aHit);
    
    return true;
}

void ScintSD::RegisterIoniHit(IonisationHit* h) {
    if(verbose >= 2) h->Display();
    nclusters++;
    
    // add to ROOT output
    EventIoniCluster c;
    c.E = h->GetEnergyDeposit();
    c.t = h->GetTime();
    c.dt = h->GetDTime();
    for(uint i=0; i<3; i++) { c.x[i] = h->GetPos()[i]; c.dx[i] = h->GetDPos()[i]; }
    c.vol = h->GetVolume();
    RootIO::GetInstance()->GetEvent().AddIoniCluster(c);
}

bool compare_hit_times(const IonisationHit* a, const IonisationHit* b) { return a->GetTime() < b->GetTime(); }

void ScintSD::EndOfEvent(G4HCofThisEvent*) {
    
    /////////////////////////////////////////////
    // organize ionizing deposition into clusters
    
    if(verbose >= 2 && hit_history.size())
        G4cerr << "Processing ionization hits in " << hit_history.size() << " volumes." << G4endl;
    for(std::map< G4int, std::vector<IonisationHit*> >::iterator it = hit_history.begin(); it != hit_history.end(); it++) {
        
        // time-order hit events
        std::sort(it->second.begin(), it->second.end(), compare_hit_times);
        
        // group into timing clusters
        nclusters = 0;
        std::vector<IonisationHit*>::iterator ihit = it->second.begin();
        IonisationHit* prevHit = *ihit;
        ihit++;
        for(; ihit != it->second.end(); ihit++) {
            if((*ihit)->GetTime() > prevHit->GetTime() + time_gap) {
                if(prevHit->GetEnergyDeposit() > edep_threshold) RegisterIoniHit(prevHit);
                else delete prevHit;
                prevHit = *ihit;
            } else {
                *prevHit += **ihit;
                delete *ihit;
            }
        }
        if(prevHit->GetEnergyDeposit() > edep_threshold) RegisterIoniHit(prevHit);
        else delete prevHit;
    }
}
