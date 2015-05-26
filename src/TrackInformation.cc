#include "TrackInformation.hh"
#include "G4ios.hh"

G4Allocator<TrackInformation> aTrackInformationAllocator;

TrackInformation::TrackInformation()
{
    originPID = 0;
    originalTrackID = -1;
    particleDefinition = NULL;
    originalPosition = G4ThreeVector(0.,0.,0.);
    originalMomentum = G4ThreeVector(0.,0.,0.);
    originalEnergy = 0.;
    originalTime = 0.;

    sourcePID = 0;
}

TrackInformation::TrackInformation(const G4Track* aTrack)
{
    originPID = aTrack->GetDefinition()->GetPDGEncoding();
    originalTrackID = aTrack->GetTrackID();
    particleDefinition = aTrack->GetDefinition();
    originalPosition = aTrack->GetPosition();
    originalMomentum = aTrack->GetMomentum();
    originalEnergy = aTrack->GetTotalEnergy();
    originalTime = aTrack->GetGlobalTime();

    sourcePID = 0;
}

TrackInformation::TrackInformation(const TrackInformation* aTrackInfo)
{
    originPID = aTrackInfo->originPID;
    originalTrackID = aTrackInfo->originalTrackID;
    particleDefinition = aTrackInfo->particleDefinition;
    originalPosition = aTrackInfo->originalPosition;
    originalMomentum = aTrackInfo->originalMomentum;
    originalEnergy = aTrackInfo->originalEnergy;
    originalTime = aTrackInfo->originalTime;
	
    sourcePID = aTrackInfo->sourcePID;
}

void TrackInformation::SetSourceTrackInformation(const G4Track* aTrack)
{
    sourcePID = aTrack->GetDefinition()->GetPDGEncoding();
}

TrackInformation::~TrackInformation(){;}

void TrackInformation::Print() const
{
    G4cout << "Original PID " << originPID << G4endl;
    G4cout << "Original track ID " << originalTrackID << G4endl;
    G4cout << "Original particleDefinition " << particleDefinition->GetPDGEncoding() << G4endl;
    G4cout << "Original position " << originalPosition << G4endl;
    G4cout << "Original momentum " << originalMomentum << G4endl;
    G4cout << "Original total energy " << originalEnergy << G4endl;
    G4cout << "Original global time " << originalTime << G4endl;

    G4cout << "Source PID " << sourcePID << G4endl;
}
