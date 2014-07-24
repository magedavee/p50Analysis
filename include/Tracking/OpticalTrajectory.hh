// Unrestricted Use - Property of AECL
// 
// OpticalTrajectory.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Optical Photon Trajectory Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 by A. Ho
// --------------------------------------------------------

#ifndef OpticalTrajectory_H		// Only carries out if object is undefined
#define OpticalTrajectory_H 1		// Defines object

#include "G4Trajectory.hh"		// Specifies base class or parent class

#include "G4Allocator.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4ParticleDefinition.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include <vector>			// Specifies classes defining all global parameters and variable types
#include <map>
#include "globals.hh"

class G4Polyline;			// Structures necessary for class definition

/* -------- Class Definition --------- */

class OpticalTrajectory : public G4Trajectory			// This class inherits functions from G4Trajectory
{
  public:	// Constructors and Destructors

    OpticalTrajectory();				// Constructor
    OpticalTrajectory(const G4Track*);			// Overloaded Constructor - initialize with a G4Track
    OpticalTrajectory(OpticalTrajectory&);		// Overloaded Constructor - initialize with another OpticalTrajectory

    virtual ~OpticalTrajectory();			// Destructor

  public:	// Accessible Methods

    virtual void DrawTrajectory(G4int i_mode = 0) const;

    inline void* operator new(size_t);			// Function called when new operator is used
    inline void operator delete(void*);			// Function called when delete operator is used

  void SetDrawTrajectory(G4bool hit) { isHit = hit; };

  private:	// Member Data

    G4ParticleDefinition* part_def;
    G4bool isHit;
};

/* ----------------------------------- */

extern G4Allocator<OpticalTrajectory> OpticalTrajectoryAllocator;	// Creates an allocator for this type of object

	// Inline functions are substituted right where they are called, rather than passing control over to this class

inline void* OpticalTrajectory::operator new(size_t)			// Allocates memory for an OpticalTrajectory object
{
  void* aTrajectory;
  aTrajectory = (void*)OpticalTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void OpticalTrajectory::operator delete(void* aTrajectory)	// De-allocates memory previously allocated for an OpticalTrajectory object
{
  OpticalTrajectoryAllocator.FreeSingle((OpticalTrajectory*) aTrajectory);
}

#endif					// End of the if clause

// EOF
