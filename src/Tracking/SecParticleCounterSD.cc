#include "SecParticleCounterSD.hh"
#include "Event.hh"
#include "FileIO.hh"
#include "G4RunManager.hh"
#include "TrackInformation.hh"

SecParticleCounterSD::SecParticleCounterSD(G4String name): G4VSensitiveDetector(name) {
	FileIO::GetInstance()->addSecParticleBranch();
}

G4bool SecParticleCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
	collectHitInfo(aStep);

	if ( parenttrackID!=0 ) {

		G4StepPoint* preStepPoint = aStep->GetPreStepPoint(); //track occurred in this volume
		//G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

		G4Track* track=aStep->GetTrack();
		SecondaryParticleVertex Vertex;
		Vertex.PID = PID;

		Vertex.E = preStepPoint->GetKineticEnergy();
		Vertex.t = preStepPoint->GetGlobalTime();
		G4ThreeVector x = worldPrePos;
		for(int i=0; i<3; i++) Vertex.x[i] = x[i];
		G4ThreeVector p = preStepPoint->GetMomentumDirection();
		for(int i=0; i<3; i++) Vertex.p[i] = p[i];
		Vertex.parenttrackID=parenttrackID;
		Vertex.trackID=track->GetTrackID();
		Vertex.stepID=track->GetCurrentStepNumber();
		G4String volname=preStepPoint->GetPhysicalVolume()->GetName();
		G4int copyno=preStepPoint->GetPhysicalVolume()->GetCopyNo();
		if ( volname.contains("PMT_capsule_phys") ) {
			Vertex.vol=2;
		}
		else if ( volname.contains("ScintTank_scint_phys") ) {
			Vertex.vol=0;
		}
		else if ( volname.contains("ScintTank_sepX_phys") ) {
			Vertex.vol=1;
		}
		else {
			Vertex.vol=-1;
		}
		Vertex.vol=10*Vertex.vol+copyno;

		Vertex.evt=G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
		TrackInformation* info = (TrackInformation*)(track->GetUserInformation());
		Vertex.originPID = info->GetOriginPID();
		Vertex.sourcePID = info->GetSourcePID();
		Vertex.isSourceInsideSepx = info->GetIsSourceInsideSepx();

		//if ( 1 ) {
		//	printf("eventID=%llu, parenttrackID=%d, trackID=%d, stepID=%d, pre-step volume=%s, copy no=%d, Vertex E=%g, t=%g, x(%g,%g,%g), p(%g,%g,%g), vol=%d, PID=%d\n",Vertex.evt,Vertex.parenttrackID,Vertex.trackID,Vertex.stepID, volname.data(), copyno,Vertex.E, Vertex.t, x[0],Vertex.x[1],Vertex.x[2],Vertex.p[0],Vertex.p[1],Vertex.p[2],Vertex.vol,Vertex.PID);
		//	printf("local x(%g,%g,%g)\n",localPrePos[0],localPrePos[1],localPrePos[2]);
		//}
		////printf("eventID=%llu, parenttrackID=%d, trackID=%d, stepID=%d, pre-step volume=%s, copy no=%d, Vertex (E=%g, t=%g, x(%g,%g,%g), p(%g,%g,%g), vol=%d, PID=%d\n",Vertex.evt,Vertex.parenttrackID,Vertex.trackID,Vertex.stepID, volname.data(), copyno,Vertex.E, Vertex.t, x[0],Vertex.x[1],Vertex.x[2],Vertex.p[0],Vertex.p[1],Vertex.p[2],Vertex.vol,Vertex.PID);

		if ( volname.contains("PMT_capsule_phys") ) {
			FileIO::GetSecondaryParticle().AddParticle(Vertex);
		}
		if ( volname.contains("ScintTank_scint_phys") && Vertex.stepID==1 ) {
			if ( Vertex.isSourceInsideSepx==1 ) {
				FileIO::GetSecondaryParticle().AddParticleGenInsideSepx();
			}
			FileIO::GetSecondaryParticle().AddParticleGen();
		}

	}
	return true;
}
