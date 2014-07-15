// Unrestricted Use - Property of AECL
//
// ProtonHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Proton Hit Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef ProtonHit_H			// Only creates if object is not yet defined
#define ProtonHit_H 1			// Defines object

#include "G4VHit.hh"			// Specifies base class or parent class

#include "G4THitsCollection.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>			// Specifies classes defining all global parameters and variable types
#include <map>
#include "globals.hh"

class G4AttDef;				// Structures necessary for class definition
class G4AttValue;

/* -------- Class Definition --------- */

class ProtonHit : public G4VHit		// This class inherits functions from G4VHit
{
  public:	// Constructors and Destructors

    ProtonHit();			// Constructor
    virtual ~ProtonHit();		// Destructor

    ProtonHit(const ProtonHit &right);	// Overloaded Constructor - initialize with data from another ProtonHit object

  public:	// Accessible Methods

    const ProtonHit& operator=(const ProtonHit &right);		// Function called when a ProtonHit object is assigned to another similar object
    int operator==(const ProtonHit &right);			// Function called when a ProtonHit object is compared with another similar object

    inline void* operator new(size_t);					// Function called when new operator is used
    inline void operator delete(void* aHit);				// Function called when delete operator is used

    virtual void Draw();						// Used to display stored data
    virtual void Print();

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;	// Used to store incoming data
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetEnergy(G4double E) { energy = E; };				// Set functions
    void SetTrackID(G4int id) { trackID = id; };

    G4double GetEnergy() const { return energy; };			// Get functions
    G4int GetTrackID() const { return trackID; };

  private:	// Member Data

    G4double energy;
    G4int trackID;
};

/* ----------------------------------- */

typedef G4THitsCollection<ProtonHit> ProtonHitsCollection;	// Provides the type definition to create a hit collection

extern G4Allocator<ProtonHit> ProtonHitAllocator;		// Creates an allocator for this type of object

	// Inline functions are substituted right where they are called, rather than passing control over to this class

inline void* ProtonHit::operator new(size_t)			// Allocates memory space for a ProtonHit object
{
  void* aHit;
  aHit = (void*)ProtonHitAllocator.MallocSingle();
  return aHit;
}

inline void ProtonHit::operator delete(void* aHit)			// De-allocates memory space previously allocated for ProtonHit objects
{
  ProtonHitAllocator.FreeSingle((ProtonHit*) aHit);
}

#endif		// End of the if clause

// EOF
