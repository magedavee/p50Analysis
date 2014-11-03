// Unrestricted Use - Property of AECL
//
// OpticalHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Optical Photon Hit Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef OpticalHit_H
#define OpticalHit_H

#include <vector>
#include <map>

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>

class G4AttDef;
class G4AttValue;
using std::vector;

/* -------- Class Definition --------- */

class OpticalHit : public G4VHit		// This class inherits functions from G4VHit
{
  public:	// Constructors and Destructors

    OpticalHit();		// Constructor
    virtual ~OpticalHit();	// Destructor

    OpticalHit(const OpticalHit &right);			// Overloaded Constructor - initialize with data from another OpticalHit object

  public:	// Accessible Methods

    const OpticalHit& operator=(const OpticalHit &right);		// Function called when a SimpleNeutronHit object is assigned to another similar object
    int operator==(const OpticalHit &right) const;			// Function called when a SimpleNeutronHit object is equated to another similar object

    inline void* operator new(size_t);					// Function called when new operator is used
    inline void operator delete(void* aHit);				// Function called when delete operator is used

    virtual void Draw();						// Used to display stored data
    virtual void Print();

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;	// Used to store incoming data
    virtual vector<G4AttValue>* CreateAttValues() const;

    void SetVolume(G4String);						// Set functions
    void SetEnergy(G4double);
    void SetTime(G4double);

    G4String GetVolumeName() const { return volume; };			// Get functions
    G4int GetCopyNumber() const { return copyNo; };
    vector<G4double>* GetEnergy() const { return energy; };
    vector<G4double>* GetTime() const { return time; };

  private:	// Member Data

    G4String volume;
    G4int copyNo;
    vector<G4double>* energy;
    vector<G4double>* time;
};

/* ----------------------------------- */

typedef G4THitsCollection<OpticalHit> OpticalHitsCollection;	// Provides the type definition to create a hit collection

extern G4Allocator<OpticalHit> OpticalHitAllocator;		// Creates an allocator for this type of object

	// Inline functions are substituted right where they are called, rather than passing control over to this class

inline void* OpticalHit::operator new(size_t)			// Allocates memory space for an OpticalHit object
{
  void* aHit;
  aHit = (void*)OpticalHitAllocator.MallocSingle();
  return aHit;
}

inline void OpticalHit::operator delete(void* aHit)		// De-allocates memory space previously allocated for an OpticalHit object
{
  OpticalHitAllocator.FreeSingle((OpticalHit*) aHit);
}

#endif		// End of the if clause

// EOF
