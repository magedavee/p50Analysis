// Unrestricted Use - Property of AECL
//
// IonisationHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Ionizing Radiation Hit Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "IonisationHit.hh"	// Specifies the file which contain the class structure

#include "G4RunManager.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4VVisManager.hh"
#include "G4PhysicalVolumeStore.hh"
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

G4Allocator<IonisationHit> IonisationHitAllocator;	// Creates an allocator for the NeutronHit class - allocates memory according to size of data to be stored

	// ****** Constructor ****** //
IonisationHit::IonisationHit()
{
  volName = "";
  copyNo = -1;
  totEDep = 0.*MeV;
  totEEsc = 0.*MeV;
  trackIDs = new std::vector<G4int>();
  left = new std::vector<G4int>();
}

	// ****** Destructor ****** //
IonisationHit::~IonisationHit()
{
  delete trackIDs;
  delete left;
}

	// ****** Overloaded Constructor ****** //
IonisationHit::IonisationHit(const IonisationHit &right)
  :  G4VHit()					// This is included to ensure that the final object has inherited the functions from G4VHit class
{
	// Assigns old object data values as new object data values
  volName = right.volName;
  copyNo = right.copyNo;
  totEDep = right.totEDep;
  totEEsc = right.totEEsc;
  *trackIDs = *(right.trackIDs);
  *left = *(right.left);
}

	// The following are functions called when an operator is applied to an object of this class

const IonisationHit& IonisationHit::operator=(const IonisationHit &right)	// Assignment operator function
{
  volName = right.volName;
  copyNo = right.copyNo;
  totEDep = right.totEDep;
  totEEsc = right.totEEsc;
  *trackIDs = *(right.trackIDs);
  *left = *(right.left);
  return *this;
}

int IonisationHit::operator>(const IonisationHit &right) const			// Greater-than operator function
{
  return (totEDep > right.totEDep);
}

int IonisationHit::operator<(const IonisationHit &right) const			// Less-than operator function
{
  return (totEDep < right.totEDep);
}

int IonisationHit::operator>=(const IonisationHit &right) const			// Greater-or-equal operator function
{
  return (totEDep >= right.totEDep);
}

int IonisationHit::operator<=(const IonisationHit &right) const			// Less-or-equal operator function
{
  return (totEDep <= right.totEDep);
}

int IonisationHit::operator==(const IonisationHit &right) const			// Comparative operator function
{
  return (copyNo==right.copyNo && volName == right.volName);
}

void IonisationHit::operator+=(const IonisationHit &right)			// Increment operator function
{
  totEDep += right.totEDep;
  totEEsc += right.totEEsc;
  G4bool IsNew = true;
  std::vector<G4int>::iterator itr = trackIDs->begin();
  while(itr != trackIDs->end())
  {
    if((*itr) == (*(right.trackIDs))[0]) { IsNew = false; break; }
    itr++;
  }
  std::vector<G4int>::iterator ittr = left->begin();
  while(ittr != left->end())
  {
    if((*ittr) == (*(right.trackIDs))[0]) { left->erase(ittr); break; }
    ittr++;
  }
  if(IsNew) { trackIDs->push_back((*(right.trackIDs))[0]); }
  if(!((right.left)->empty())) { left->push_back((*(right.left))[0]); }
}

	// ****** Draw Hit Function ****** //
void IonisationHit::Draw()
{;}

	// ****** Print Hit Function ****** //
void IonisationHit::Print()
{;}

	// The following functions specify and assemble the memory allocations required/variables stored for an object of this class

const std::map<G4String,G4AttDef>* IonisationHit::GetAttDefs() const
{
  G4bool isNew;
  std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("IonisationHit",isNew);
  if(isNew)
  {
    G4String hitType("Hit Type");
    (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");

    G4String volume("Volume Name");
    (*store)[volume] = G4AttDef(volName,"Volume Name","Simulation","","G4String");

    G4String erg("Energy Deposited");
    (*store)[erg] = G4AttDef(erg,"Energy Deposited","Physics","G4BestUnit","G4double");

    G4String escape("Energy Escaped");
    (*store)[escape] = G4AttDef(escape,"Energy Escaped","Physics","G4BestUnit","G4double");

    G4String id("Track ID");
    (*store)[id] = G4AttDef(id,"Track ID","Simulation","","std::vector");

    G4String track("Tracker");
    (*store)[track] = G4AttDef(track,"Tracker","Simulation","","std::vector");
  }
  return store;
}

std::vector<G4AttValue>* IonisationHit::CreateAttValues() const
{
  std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

  values->push_back(G4AttValue("Hit Type","Ionisation Hit",""));
  values->push_back(G4AttValue("Copy Number",copyNo,""));
  values->push_back(G4AttValue("Volume Name",volName,""));
  values->push_back(G4AttValue("Energy Deposited",G4BestUnit(totEDep,"Energy"),""));
  values->push_back(G4AttValue("Energy Escaped",G4BestUnit(totEEsc,"Energy"),""));
  values->push_back(G4AttValue("Track ID","Vector",""));
  values->push_back(G4AttValue("Tracker","Vector",""));

  return values;
}

void IonisationHit::SetVolume(G4String volume)
{
  volName = volume;
  G4VPhysicalVolume* detVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName);
  G4int numCopy = -1;
  if(detVolume)
  {
    numCopy = detVolume->GetCopyNo();
  }
  if(numCopy >= 0) { copyNo = numCopy; }
}

// EOF
