#ifndef TrackInformation_h
#define TrackInformation_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"

class TrackInformation : public G4VUserTrackInformation
{
	public:
		TrackInformation();
		TrackInformation(const G4Track* aTrack);
		TrackInformation(const TrackInformation* aTrackInfo);
		virtual ~TrackInformation();

		void SetSourceTrackInformation(const G4Track* aTrack);

		inline void *operator new(size_t);
		inline void operator delete(void *aTrackInfo);
		inline int operator ==(const TrackInformation& right) const
		{return (this==&right);}

		void Print() const;

	private:
		G4int                 originPID;
		G4int                 originalTrackID;
		G4ParticleDefinition* particleDefinition;
		G4ThreeVector         originalPosition;
		G4ThreeVector         originalMomentum;
		G4double              originalEnergy;
		G4double              originalTime;

		G4int                 sourcePID;
		G4int                isSourceInsideSepx; // is the source (vertex) inside the sep box
	public:
		inline G4int GetOriginPID() const {return originPID;}
		inline G4int GetOriginalTrackID() const {return originalTrackID;}
		inline G4ParticleDefinition* GetOriginalParticle() const {return particleDefinition;}
		inline G4ThreeVector GetOriginalPosition() const {return originalPosition;}
		inline G4ThreeVector GetOriginalMomentum() const {return originalMomentum;}
		inline G4double GetOriginalEnergy() const {return originalEnergy;}
		inline G4double GetOriginalTime() const {return originalTime;}

		inline G4int GetSourcePID() const {return sourcePID;}
		inline G4int GetIsSourceInsideSepx() const {return isSourceInsideSepx;}
};

extern G4Allocator<TrackInformation> aTrackInformationAllocator;

inline void* TrackInformation::operator new(size_t)
{ void* aTrackInfo;
	aTrackInfo = (void*)aTrackInformationAllocator.MallocSingle();
	return aTrackInfo;
}

inline void TrackInformation::operator delete(void *aTrackInfo)
{ aTrackInformationAllocator.FreeSingle((TrackInformation*)aTrackInfo);}

#endif
