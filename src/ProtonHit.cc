// Unrestricted Use - Property of AECL
//
// ProtonHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Proton Hit Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "ProtonHit.hh"		// Specifies the file which contain the class structure

#include "G4RunManager.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"
#include "G4Point3D.hh"
#include "G4Circle.hh"

#include <iomanip>		// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"		// Specifies class defining all global parameters and variable types
#include <vector>

G4Allocator<ProtonHit> ProtonHitAllocator;	// Creates an allocator for the NeutronHit class - allocates memory according to size of data to be stored

	// ****** Constructor ****** //
ProtonHit::ProtonHit()
{
  energy = 0.*MeV;
  trackID = -1;
}

	// ****** Destructor ****** //
ProtonHit::~ProtonHit()
{;}

	// ****** Overloaded Constructor ****** //
ProtonHit::ProtonHit(const ProtonHit &right)
  :  G4VHit()		// This is included to ensure that the final object has inherited the functions from G4VHit class
{
	// Assigns old object data values as new object data values
  energy = right.energy;
  trackID = right.trackID;
}

	// The following are functions called when an operator is applied to an object of this class

const ProtonHit& ProtonHit::operator=(const ProtonHit &right)	// Assignment operator function
{
  energy = right.energy;
  trackID = right.trackID;
  return *this;
}

int ProtonHit::operator==(const ProtonHit &right)		// Equality operator function
{
  return (trackID == right.trackID);
}

	// ****** Draw Hit Function ****** //
void ProtonHit::Draw()		// Function to draw out hit locations on the visualization
{;}

	// ****** Print Hit Function ****** //
void ProtonHit::Print()		// Function to print out hit locations on designated output
{;}

	// The following functions specify and assemble the memory allocations required/variables stored for an object of this class

const std::map<G4String,G4AttDef>* ProtonHit::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("IonisationHit",isNew);
  if(isNew)
  {
    G4String hitType("Hit Type");
    (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");

    G4String erg("Proton Energy");
    (*store)[erg] = G4AttDef(erg,"Proton Energy","Physics","G4BestUnit","G4double");

    G4String id("Track ID");
    (*store)[id] = G4AttDef(id,"Track ID","Simulation","","G4int");
  }
  return store;
}

std::vector<G4AttValue>* ProtonHit::CreateAttValues() const
{
  std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

  values->push_back(G4AttValue("Hit Type","Proton Hit",""));
  values->push_back(G4AttValue("Proton Energy",G4BestUnit(energy,"Energy"),""));
  values->push_back(G4AttValue("Track ID",trackID,""));

  return values;
}

// EOF
