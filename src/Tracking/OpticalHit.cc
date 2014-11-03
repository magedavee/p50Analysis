// Unrestricted Use - Property of AECL
//
// OpticalHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Optical Photon Hit Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "OpticalHit.hh"

#include <G4SystemOfUnits.hh>
#include <G4RunManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VVisManager.hh>
#include <G4Colour.hh>
#include <G4AttDefStore.hh>
#include <G4AttDef.hh>
#include <G4AttValue.hh>
#include <G4UnitsTable.hh>
#include <G4VisAttributes.hh>
#include <G4ThreeVector.hh>
#include <G4Point3D.hh>
#include <G4Circle.hh>
#include <G4VSolid.hh>

#include <G4ios.hh>

#include <iomanip>

G4Allocator<OpticalHit> OpticalHitAllocator;	// Creates an allocator for the OpticalHit class - allocates memory according to size of data to be stored

	// ****** Constructor ****** //
OpticalHit::OpticalHit()
{
  volume = "N/A";
  copyNo = -1;
  energy = new vector<G4double>();
  time = new vector<G4double>();
}

	// ****** Destructor ****** //
OpticalHit::~OpticalHit()
{
  delete energy;
  delete time;
}

	// ****** Overloaded Constructor ****** //
OpticalHit::OpticalHit(const OpticalHit &right)
  :  G4VHit()		// This is included to ensure that the final object has inherited the functions from G4VHit class
{
	// Assigns old object data values as new object data values
  volume = right.volume;
  copyNo = right.copyNo;
  *energy = *(right.energy);
  *time = *(right.time);
}

	// The following are functions called when an operator is applied to an object of this class

const OpticalHit& OpticalHit::operator=(const OpticalHit &right)	// Assignment operator function
{
  volume = right.volume;
  copyNo = right.copyNo;
  *energy = *(right.energy);
  *time = *(right.time);
  return *this;
}

int OpticalHit::operator==(const OpticalHit &right) const		// Equality operator function
{
  return (copyNo==right.copyNo && volume==right.volume);
}

	// ****** Draw Hit Function ****** //
void OpticalHit::Draw()
{
  G4VVisManager* vis_manager = G4VVisManager::GetConcreteInstance();
  if(vis_manager)
  {
    // Identify target physical volume

    G4VPhysicalVolume* detVol = G4PhysicalVolumeStore::GetInstance()->GetVolume(volume,false);
    
    if(detVol && !(energy->empty()))
    {
	// Obtain the shape of the physical volume
      G4VSolid* detSolid = detVol->GetLogicalVolume()->GetSolid();
      G4RotationMatrix detRot = detVol->GetObjectRotationValue();
      G4ThreeVector detTrans = detVol->GetObjectTranslation();
      G4Transform3D transform(detRot, detTrans);

	// Draws dark red, then red, then orange, then yellow, then white depending on number of hits recorded
      G4double red = ((double)energy->size()) / 200.0; red = (red/2.) + 0.5;
      G4double green = 0.; G4double blue = 0.;
      if(red > 1.)
      {
        red = 1.;
        green = ((double)energy->size() - 200.0) / 500.0;
      }
      if(green > 1.)
      { 
        green = 1.;
        blue = ((double)energy->size() - 700.) / 1300.0;
      }
      if(blue > 1.) { blue = 1.; }
      G4VisAttributes* detect_vis = new G4VisAttributes(G4Colour(red,green,blue));
      detect_vis->SetForceSolid(true);

      vis_manager->Draw(*detSolid,*detect_vis,transform);
    }
    else { G4cout << "*** ERROR: Nothing to draw! ***" << G4endl; }
  }
}

	// ****** Print Hit Function ****** //
void OpticalHit::Print()
{
  G4cout << "\tCopy No.: " << copyNo << G4endl;
  vector<G4double>::iterator itr = energy->begin();
  G4cout << "\tEnergy: " << G4endl;
  while(itr != energy->end())
  {
    G4cout << "\t" << G4BestUnit(*itr,"Energy") << G4endl;
  }
  itr = time->begin();
  G4cout << "\tTime: " << G4endl;
  while(itr != time->end())
  {
    G4cout << G4BestUnit(*itr,"Time") << G4endl;
  }
}

	// The following functions specify and assemble the memory allocations required/variables stored for an object of this class

const map<G4String,G4AttDef>* OpticalHit::GetAttDefs() const
{
  G4bool IsNew;
  map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("OpticalHit",IsNew);
  if(IsNew)
  {
    G4String hitType("Hit Type");
    (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");

    G4String vol("Volume");
    (*store)[vol] = G4AttDef(vol,"Physical Volume Name","Geometry","","G4String");

    G4String copy("Copy Number");
    (*store)[copy] = G4AttDef(copy,"Volume Copy Number","Geometry","","G4int");

    G4String photEnergy("Energy");
    (*store)[photEnergy] = G4AttDef(photEnergy,"Photon Energy","Physics","","vector<G4double>");

    G4String detectTime("Time");
    (*store)[detectTime] = G4AttDef(detectTime,"Detection Time from Generation","Physics","","vector<G4double>");
  }
  return store;
}

vector<G4AttValue>* OpticalHit::CreateAttValues() const
{
  vector<G4AttValue>* values = new vector<G4AttValue>;

  values->push_back(G4AttValue("Hit Type","Optical Hit",""));
  values->push_back(G4AttValue("Volume",volume,""));
  values->push_back(G4AttValue("Copy Number",copyNo,""));
  values->push_back(G4AttValue("Energy","G4double vector",""));
  values->push_back(G4AttValue("Time","G4double vector",""));

  return values;
}

	// The following functions allow the storage containers to be modified in other ways than simple assignment - as used in header file

	// ****** Change Volume Name ****** //
void OpticalHit::SetVolume(G4String volName)
{
  volume = volName;
  G4VPhysicalVolume* detVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName);
  G4int numCopy = -1;
  if(detVolume)
  {
    numCopy = detVolume->GetCopyNo();
  }
  if(numCopy >= 0) { copyNo = numCopy; }
}

	// ****** Attach Energy Value ****** //
void OpticalHit::SetEnergy(G4double E)
{
  energy->push_back(E);
}

	// ****** Attach Time Value ****** //
void OpticalHit::SetTime(G4double t)
{
  time->push_back(t);
}

// EOF
