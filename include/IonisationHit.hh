// Unrestricted Use - Property of AECL
//
// IonisationHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Ionizing Radiation Hit Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef IonisationHit_H			// Only creates if object is not yet defined
#define IonisationHit_H 1		// Defines object

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

class IonisationHit : public G4VHit		// This class inherits functions from G4VHit
{
  public:	// Constructors and Destructors

    IonisationHit();		// Constructor
    virtual ~IonisationHit();	// Destructor

    IonisationHit(const IonisationHit &right);	// Overloaded Constructor - initialize with data from another IonisationHit object

  public:	// Accessible Methods

    const IonisationHit& operator=(const IonisationHit &right);		// Function called when a IonisationHit object is assigned to another similar object
    int operator>(const IonisationHit &right) const;			// Functions called when a IonisationHit object is compared to another similar object
    int operator<(const IonisationHit &right) const;
    int operator>=(const IonisationHit &right) const;
    int operator<=(const IonisationHit &right) const;
    int operator==(const IonisationHit &right) const;
    void operator+=(const IonisationHit &right);			// Function called when a IonisationHit object is incremented with another similar object

    inline void* operator new(size_t);					// Function called when new operator is used
    inline void operator delete(void* aHit);				// Function called when delete operator is used

    virtual void Draw();						// Used to display stored data
    virtual void Print();

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;	// Used to store incoming data
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetVolume(G4String);			// Set functions
    void SetEnergyDeposit(G4double E) { totEDep = E; };
    void SetEnergyEscaped(G4double Esc) { totEEsc = Esc; };
    void SetTrackID(G4int id) { trackIDs->push_back(id); };
    void SetLeft(G4bool l) { left->push_back(l); }; 
  G4int GetCopyNumber() const { return copyNo; };
  
    G4String GetVolumeName() const { return volName; };				// Get functions
    G4double GetEnergyDeposit() const { return totEDep; };
    G4double GetEnergyEscaped() const { return totEEsc; };
    std::vector<G4int>* GetTrackID() const { return trackIDs; };
    std::vector<G4int>* GetLeft() const { return left; };

  private:	// Member Data

    G4String volName;
    G4int copyNo;
    G4double totEDep;
    G4double totEEsc;
    std::vector<G4int>* trackIDs;
    std::vector<G4int>* left;
};

/* ----------------------------------- */

typedef G4THitsCollection<IonisationHit> IonisationHitsCollection;	// Provides the type definition to create a hit collection

extern G4Allocator<IonisationHit> IonisationHitAllocator;		// Creates an allocator for this type of object

	// Inline functions are substituted right where they are called, rather than passing control over to this class

inline void* IonisationHit::operator new(size_t)			// Allocates memory space for a IonisationHit object
{
  void* aHit;
  aHit = (void*)IonisationHitAllocator.MallocSingle();
  return aHit;
}

inline void IonisationHit::operator delete(void* aHit)			// De-allocates memory space previously allocated for IonisationHit objects
{
  IonisationHitAllocator.FreeSingle((IonisationHit*) aHit);
}

#endif		// End of the if clause

// EOF
