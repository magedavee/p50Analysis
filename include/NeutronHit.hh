// Unrestricted Use - Property of AECL
//
// NeutronHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Neutron Hit Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef NeutronHit_H			// Only creates if object is not yet defined
#define NeutronHit_H 1			// Defines object

#include "G4VHit.hh"			// Specifies base class or parent class

#include "G4THitsCollection.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>			// Specfies classes defining all global parameters and variable types
#include <map>
#include "globals.hh"

class G4AttDef;				// Structures necessary for class definition
class G4AttValue;

/* -------- Class Definition --------- */

class NeutronHit : public G4VHit		// This class inherits functions from G4VHit
{
  public:	// Constructors and Destructors

    NeutronHit();		// Constructor
    virtual ~NeutronHit();	// Destructor

    NeutronHit(const NeutronHit &right);	// Overloaded Constructor - initialize with data from another NeutronHit object

  public:	// Accessible Methods

    const NeutronHit& operator=(const NeutronHit &right);		// Function called when a NeutronHit object is assigned to another similar object
    int operator==(const NeutronHit &right) const;			// Function called when a NeutronHit object is equated to another similar object
    void operator+=(const NeutronHit &right);				// Function called when a NeutronHit object is incremented with another similar object

    inline void* operator new(size_t);					// Function called when new operator is used
    inline void operator delete(void* aHit);				// Function called when delete operator is used

    virtual void Draw();						// Used to display stored data
    virtual void Print();

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;	// Used to store incoming data
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetEnergy(G4double E) { energy = E; };				// Set functions
    void SetInitialPosition(G4ThreeVector pos) { positionI = pos; };
    void SetFinalPosition(G4ThreeVector pos) { positionF = pos; };
    void SetEntryTime(G4double bt) { bTime = bt; };
    void SetTrackTime(G4double t) { time = t; };
    void SetTrackID(G4int id) { trackID = id; };
    void SetThermalizationTime(G4double tt) { tTherm = tt; };
    void SetGammasGenerated(G4int gam) { gamma = gam; };
    void SetLeft(G4bool l) { left = l; }; 
    void SetCaptured(G4bool cap) { captured = cap; }; 
    void SetForeign(G4bool ext) { foreign = ext; }; 
    void SetZenithAngle(G4double ang) { zenith = ang; };

    G4double GetEnergy() const { return energy; };			// Get functions
    G4ThreeVector GetInitialPosition() const { return positionI; };
    G4ThreeVector GetFinalPosition() const { return positionF; };
    G4double GetEntryTime() const { return bTime; };
    G4double GetTrackTime() const { return (time - bTime); };
    G4int GetTrackID() const { return trackID; };
    G4double GetThermalizationTime() const { return tTherm; };
    G4int GetGammasGenerated() const { return gamma; };
    G4bool GetLeft() const { return left; };
    G4bool GetCaptured() const { return captured; };
    G4bool GetForeign() const { return foreign; };
    G4double GetZenithAngle() const { return zenith; };

  private:	// Member Data

    G4double energy;
    G4ThreeVector positionI;
    G4ThreeVector positionF;
    G4double bTime;
    G4double time;
    G4int trackID;
    G4double tTherm;
    G4int gamma;
    G4bool left;
    G4bool captured;
    G4bool foreign;
    G4double zenith;
};

/* ----------------------------------- */

typedef G4THitsCollection<NeutronHit> NeutronHitsCollection;	// Provides the type definition to create a hit collection

extern G4Allocator<NeutronHit> NeutronHitAllocator;			// Creates an allocator for this type of object

	// Inline functions are substituted right where they are called, rather than passing control over to this class

inline void* NeutronHit::operator new(size_t)				// Allocates memory space for a NeutronHit object
{
  void* aHit;
  aHit = (void*)NeutronHitAllocator.MallocSingle();
  return aHit;
}

inline void NeutronHit::operator delete(void* aHit)			// De-allocates memory space previously allocated for NeutronHit objects
{
  NeutronHitAllocator.FreeSingle((NeutronHit*) aHit);
}

#endif		// End of the if clause

// EOF
