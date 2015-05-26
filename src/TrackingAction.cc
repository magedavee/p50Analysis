#include "TrackingAction.hh"
#include "FileIO.hh"

#include <G4Track.hh>
#include <G4SystemOfUnits.hh>
#include "TrackInformation.hh"
#include "G4TrackingManager.hh"

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
	// record neutron generation
	if(aTrack->GetDefinition()->GetPDGEncoding() == 2112 && aTrack->GetGlobalTime() > 0) {
		NCapt nc;
		nc.t = aTrack->GetGlobalTime();
		nc.E = aTrack->GetKineticEnergy();
		for(int i=0; i<3; i++) nc.x[i] = aTrack->GetPosition()[i];
		nc.capt_A = 1;
		nc.vol = -1000;
		FileIO::GetNCapt().AddNCapt(nc);
	}

	//for Tracking/SecParticleCounterSD.cc
	if(aTrack->GetParentID()==0 && aTrack->GetUserInformation()==0)
	{
		TrackInformation* anInfo = new TrackInformation(aTrack);
		G4Track* theTrack = (G4Track*)aTrack;
		theTrack->SetUserInformation(anInfo);
	}
	//if ( aTrack->GetParentID()==0 ) {
	//	if ( aTrack->GetVolume() ) {
	//		printf("track status=%d, trackID=%d, E=%g, t=%g, vol=%s \n",aTrack->GetTrackStatus(),aTrack->GetTrackID(), aTrack->GetTotalEnergy(),aTrack->GetGlobalTime(),aTrack->GetVolume()->GetName().data());
	//	}
	//	else {
	//		printf("track status=%d, trackID=%d, E=%g, t=%g \n",aTrack->GetTrackStatus(),aTrack->GetTrackID(), aTrack->GetTotalEnergy(),aTrack->GetGlobalTime());
	//	}
	//	if ( aTrack->GetCreatorProcess() ) {
	//		printf("processname=%s\n",aTrack->GetCreatorProcess()->GetProcessName().data());
	//	}
	//}
	
	
	
	
	// immediate secondaries from long-lived primaries: doesn't work!
	//if(aTrack->GetParentID()==1 && aTrack->GetGlobalTime() > 0.1*s) {
	//    G4Track* tr = (G4Track*)aTrack;
	//    tr->SetGlobalTime(0);
	//}
}

//for Tracking/SecParticleCounterSD.cc
void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
	G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
	if(secondaries)
	{
		TrackInformation* info = (TrackInformation*)(aTrack->GetUserInformation());
		if ( aTrack->GetParentID()==0 ) {
			info->SetSourceTrackInformation(aTrack);
			//if ( aTrack->GetVolume() ) {
			//	printf("parentid=%d,trackid=%d,pid=%d, isSourceInsideSepx=%d, vol=%s\n",aTrack->GetParentID(),aTrack->GetTrackID(),aTrack->GetDefinition()->GetPDGEncoding(),info->GetIsSourceInsideSepx(),aTrack->GetVolume()->GetName().data());
			//}
			//else {
			//	printf("parentid=%d,trackid=%d,pid=%d, isSourceInsideSepx=%d\n",aTrack->GetParentID(),aTrack->GetTrackID(),aTrack->GetDefinition()->GetPDGEncoding(),info->GetIsSourceInsideSepx());
			//}
		}
		size_t nSeco = secondaries->size();
		if(nSeco>0)
		{
			for(size_t i=0;i<nSeco;i++)
			{
				TrackInformation* infoNew = new TrackInformation(info);
				infoNew->SetSourceTrackInformation(aTrack);
				(*secondaries)[i]->SetUserInformation(infoNew);
			}
		}
	}
}
