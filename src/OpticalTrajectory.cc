// Unrestricted Use - Property of AECL
//
// OpticalTrajectory.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Optical Photon Trajectory Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "OpticalTrajectory.hh"		// Specifies the file which contains the class structure

#include "G4TrajectoryPoint.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4Trajectory.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ThreeVector.hh"
#include "G4Polyline.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4VVisManager.hh"
#include "G4Polymarker.hh"
#include "G4OpticalPhoton.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"			// Specifies class defining all global parameters and variable types

G4Allocator<OpticalTrajectory> OpticalTrajectoryAllocator;

	// ****** Constructor ****** //
OpticalTrajectory::OpticalTrajectory()
  :  G4Trajectory()
{
  part_def = 0;
  isHit = false;
}

	// ****** Overloaded Constructor ****** //
OpticalTrajectory::OpticalTrajectory(const G4Track* aTrack)
  : G4Trajectory(aTrack)
{
  part_def = aTrack->GetDefinition();
  isHit = false;
}

	// ****** Overloaded Constructor ****** //
OpticalTrajectory::OpticalTrajectory(OpticalTrajectory &right)
  : G4Trajectory(right)
{
  part_def = right.part_def;
  isHit = right.isHit;
}

	// ****** Destructor ****** //
OpticalTrajectory::~OpticalTrajectory()
{;}

	// ****** Draw Trajectory Function ****** //
void OpticalTrajectory::DrawTrajectory(G4int i_mode) const
{
  //  if(!isHit) { return; }
  G4VVisManager* vis_manager = G4VVisManager::GetConcreteInstance();
  if(!vis_manager) { return; }

	// Determine line and marker visualizations
  const G4double markerSize = std::abs(i_mode)/1000;
  G4bool lineRequired(i_mode >= 0);
  G4bool markersRequired(markerSize > 0.);

  G4Polyline trajLine;
  G4Polymarker stepPoints;
  G4Polymarker auxPoints;

	// Determines which points are necessary to draw the trajectory
  for(int i = 0; i < GetPointEntries(); i++)
  {
    G4VTrajectoryPoint* aTrajectoryPoint = GetPoint(i);
    const std::vector<G4ThreeVector>* auxiliaries = aTrajectoryPoint->GetAuxiliaryPoints();
    if(auxiliaries)
    {
      for(size_t j = 0; j < auxiliaries->size(); j++)
      {
        const G4ThreeVector pos((*auxiliaries)[j]);
        if(lineRequired)    { trajLine.push_back(pos); }
        if(markersRequired) { auxPoints.push_back(pos); }
      }
    }
    const G4ThreeVector pos(aTrajectoryPoint->GetPosition());
    if(lineRequired)    { trajLine.push_back(pos); }
    if(markersRequired) { stepPoints.push_back(pos); }
  }

	// Draws the trajectory line if requested
  if(lineRequired)
  {
    G4VisAttributes* line_vis;
    if(isHit) line_vis = new G4VisAttributes(G4Colour(1.0,0.5,0.0));
    //    else line_vis = new G4VisAttributes(G4Colour(0.0,1.0,0.5));   //visualize missed photons
    trajLine.SetVisAttributes(line_vis);
    vis_manager->Draw(trajLine);
  }

	// Draws the markers at trajectory changes if requested
  if(markersRequired)
  {
    G4VisAttributes* aux_vis = new G4VisAttributes(G4Colour(1.0,0.0,0.5));
    auxPoints.SetMarkerType(G4Polymarker::squares);
    auxPoints.SetScreenSize(markerSize);
    auxPoints.SetFillStyle(G4VMarker::filled);
    auxPoints.SetVisAttributes(aux_vis);
    vis_manager->Draw(auxPoints);

    G4VisAttributes* step_vis = new G4VisAttributes(G4Colour(1.0,0.5,0.0));
    auxPoints.SetMarkerType(G4Polymarker::circles);
    auxPoints.SetScreenSize(markerSize);
    auxPoints.SetFillStyle(G4VMarker::filled);
    auxPoints.SetVisAttributes(step_vis);
    vis_manager->Draw(stepPoints);
  }
}

// EOF
