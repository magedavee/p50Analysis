// Unrestricted Use - Property of AECL
//
// InverseBetaKinematics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Inverse Beta Decay Reaction Kinematics Generator
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "InverseBetaKinematics.hh"		// Specifies the file which contains the class structure

#include "InverseBetaMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "FissionAntiNuModule.hh"

#include <G4SystemOfUnits.hh>
#include "Randomize.hh"				// Specifies the classes which contain structures called upon in this class
#include "G4UnitsTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Tubs.hh"
#include "G4VisExtent.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies classes defining all global parameters and variable types
#include <vector>
#include <math.h>

using std::map;

	// ****** Constructor ****** //
InverseBetaKinematics::InverseBetaKinematics(G4int v, const G4String target)
{

  G4cout << "Physical Volume Names:" << G4endl;
  vector<G4VPhysicalVolume*>* volumes = G4PhysicalVolumeStore::GetInstance();
  vector<G4VPhysicalVolume*>::iterator itr = volumes->begin();
  for( ; itr != volumes->end(); itr++)
  {
    G4cout << (*itr)->GetName() << G4endl;
  }


  verbose = v;		// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }

	// Define default values
  c = 299792458*m/s;
  pMass = G4Proton::ProtonDefinition()->GetPDGMass();
  nMass = G4Neutron::NeutronDefinition()->GetPDGMass();
  ePlusMass = G4Positron::PositronDefinition()->GetPDGMass();
  antiNuDir.set(0.0,0.0,1.0);
  antiNuMonoEnergy = 2.*MeV;
  fMonoEnergy = false;
  Neutrons = true;
  Positrons = true;
  Sequential=false;

  fission_gen = new FissionAntiNuModule(NULL); //assert(false);
  inv_messenger = new InverseBetaMessenger(this);

	// Establish initial target volume - defaults to world volume
  worldVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume("world_phys");
  targetName = target;
  G4cout << "TARGET IS " <<target<< "   " <<targetName<< G4endl;
  if(!(targetName == "")) { targetVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(target); }
  else                    { targetVolume = 0; }
  if(!targetVolume)
  {
    targetVolume = worldVolume;
    targetName = targetVolume->GetName();
    DToWRotation = new G4RotationMatrix(0.,0.,0.);
    DToWTranslation = new G4ThreeVector(0.,0.,0.);
    G4cout << "*** ERROR: Specified target volume not found. Inverse Beta Target defaulted to the world volume, " << targetName << ". ***" << G4endl;
  }
  else
  {
    DToWRotation = FindTargetRotationWRTWorld();
    DToWTranslation = FindTargetTranslationWRTWorld();
    G4cout<< " Target input is " <<target<< "." << G4endl;
    G4cout << "Inverse Beta Target set to " << targetName << "." << G4endl;
  }

	// Outputs default parameter list to terminal
  G4String IsActive;
  if(fMonoEnergy) { IsActive = "Inactive"; } else { IsActive = "Active"; }
  G4cout << "\n"
	 << "******* InverseBetaKinematics initiated with the following default parameters *******\n"
         << "\t   Fission Spectrum:		" << IsActive << "\n"
         << "\t   Uranium-235 Content:		" << 100*(fission_gen->GetUranium235Content(true)) << "%" << "\n"
         << "\t   Uranium-238 Content:		" << 100*(fission_gen->GetUranium238Content(true)) << "%" << "\n"
         << "\t   Plutonium-239 Content:	" << 100*(fission_gen->GetPlutonium239Content(true)) << "%" << "\n"
         << "\t   Plutonium-241-Content:	" << 100*(fission_gen->GetPlutonium241Content(true)) << "%" << "\n"
         << "\t   Antineutrino Flux Direction:	" << "x = " << antiNuDir.x() << ", y = " << antiNuDir.y() << ", z = " << antiNuDir.z() << "\n"
         << "\t   Target Volume Name:		" << targetName << "\n"
	 << "\t   Target Volume Dimensions:     X: " <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetXmax() << "\tY: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetYmax() << "\tZ: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetZmax() << "\n"
	 << "\t   Target Volume Center:     X: " <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[0] << "\tY: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[1] << "\tZ: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[2] << "\n"
	 << "*************************************************************************************\n"
         << G4endl;
}

	// ****** Destructor ****** //
InverseBetaKinematics::~InverseBetaKinematics()
{
  delete fission_gen;
  delete inv_messenger;
}

	// The following functions allow the user to retrieve individual fuel composition percentages

G4double InverseBetaKinematics::GetSpectrumU235Content(G4bool frac) const
{
  return fission_gen->GetUranium235Content(frac);
}

G4double InverseBetaKinematics::GetSpectrumU238Content(G4bool frac) const
{
  return fission_gen->GetUranium238Content(frac);
}

G4double InverseBetaKinematics::GetSpectrumPu239Content(G4bool frac) const
{
  return fission_gen->GetPlutonium239Content(frac);
}

G4double InverseBetaKinematics::GetSpectrumPu241Content(G4bool frac) const
{
  return fission_gen->GetPlutonium241Content(frac);
}

	// ****** Change Verbosity ******* //
void InverseBetaKinematics::SetVerbosity(G4int v)
{
  verbose = v;
  if(verbose > 1) { G4cout << "InverseBetaKinematics verbosity set to " << v << "." << G4endl; }
  if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: InverseBetaKinematics raw data will now be output. ***" << G4endl; }
  else            { RawData = false; }
  //fission_gen->SetVerbosity(verbose);
}

	// ****** Change Target Volume ****** //
void InverseBetaKinematics::SetReactionVolume(G4String target)			// Change interacting volume without altering PrimaryGeneratorAction directly
{
  G4VPhysicalVolume* theVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(target);
  if(theVolume)
  {
    targetName = target;
    targetVolume = theVolume;
    DToWRotation = FindTargetRotationWRTWorld();
    DToWTranslation = FindTargetTranslationWRTWorld();
    G4cout << "Inverse Beta Target set to " << targetName << "." << G4endl;
  }
  else { G4cout << "*** ATTENTION: Specified target volume not found. Inverse Beta Target kept as " << targetName << ". ***" << G4endl; }
}

	// ****** Change Fuel Composition ****** //
void InverseBetaKinematics::SetAntiNeutrinoSpectrum(G4double U5, G4double U8, G4double P3, G4double P4)
{
	// Disables monoenergetic antineutrinos
  fMonoEnergy = false;

	// Re-enables monoenergetic antineutrinos if all compositions are zero
  if(U5 == 0. && U8 == 0. && P3 == 0. && P4 == 0.)
  {
    fMonoEnergy = true; 
    G4cout << "*** WARNING: Spectrum total composition adds to zero. Mono-energetic antineutrinos with energy " << G4BestUnit(antiNuMonoEnergy,"Energy") << " used. ***" << G4endl;
  }
  else
  {
	// Passes values to FissionAntiNuModule object otherwise
    fission_gen->SetAntiNeutrinoSpectrum(U5,U8,P3,P4);
    G4double pcU5 = 100*(fission_gen->GetUranium235Content(true));  
    G4double pcU8 = 100*(fission_gen->GetUranium238Content(true));  
    G4double pcP3 = 100*(fission_gen->GetPlutonium239Content(true));
    G4double pcP4 = 100*(fission_gen->GetPlutonium241Content(true));
 

    if(verbose > 0)
    { G4cout << "Spectrum currently set to --- Uranium-235: " << pcU5 << "%, Uranium-238: " << pcU8 << "%, Plutonium-239: " << pcP3 << "%, Plutonium-241: " << pcP4 << "%" << G4endl; }
  }
}

	// ****** Change Mono-Energy ****** //
void InverseBetaKinematics::SetAntiNeutrinoMonoEnergy(G4double E)		// User functionality to specify a single antineutrino energy rather than use the spectrum
{
  if(E > 1.8*MeV && E < 9.5*MeV)
  {
    antiNuMonoEnergy = E;
    if(verbose > 0) { G4cout << "Spectrum set to use mono-energetic antineutrinos at " << G4BestUnit(E,"Energy") << "." << G4endl; }
  }
  else
  {
    antiNuMonoEnergy = 2.*MeV;
    G4cout << "*** WARNING: Input mono-energetic antineutrinos are outside acceptable range. Switching to default energy of 2.0 MeV. ***" << G4endl;
  }

	// Set monoenergetic particle option by changing fuel composition to 0,0,0,0
  if(!fMonoEnergy && verbose > 0)
  { G4cout << "*** CAUTION: Mono-energetic antineutrino option has not been selected. Running simulation before setting this option will not use this parameter. ***" << G4endl; }
}

	// ****** Neutron Generation Switch ****** //
void InverseBetaKinematics::ToggleNeutronGeneration(G4bool neutron)
{
  Neutrons = neutron;
  if(Neutrons) { G4cout << "Neutron generation enabled. Simulation will shoot neutrons according to this module's distribution." << G4endl; }
  else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
  else { G4cout << "Neutron generation disabled. Simulation will not shoot neutrons generated through this module's distribution." << G4endl; }
}

	// ****** Positron Generation Switch ****** //
void InverseBetaKinematics::TogglePositronGeneration(G4bool positron)
{
  Positrons = positron;
  if(Positrons) { G4cout << "Positron generation enabled. Simulation will shoot positrons according to this module's distribution." << G4endl; }
  else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
  else { G4cout << "Positron generation disabled. Simulation will not shoot positrons generated through this module's distribution." << G4endl; }
}

void InverseBetaKinematics::ToggleSequentialGeneration(G4bool sequential)
{
  Sequential = sequential;
  if(Sequential) { G4cout << "Sequential generation enabled. Simulation will shoot positrons and neutrons according to this module's distribution." << G4endl;}
  else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
  else { G4cout << "Sequential generation disabled. Simulation will not shoot particles generated through this module's distribution." << G4endl; }
}

	// ****** Generate Initial Particle Vector ****** //
G4ThreeVector InverseBetaKinematics::GenerateReactionPosition() const
{
	// Obtains target volume shape
  G4VSolid* solid = targetVolume->GetLogicalVolume()->GetSolid();
  G4double xRange, yRange, zRange;
  if(solid->GetEntityType() == "G4Tubs")
  {
    G4Tubs* tube = dynamic_cast<G4Tubs*>(solid);	// G4Tubs does not contain a GetExtent() function so this will suffice
    xRange = tube->GetOuterRadius();
    yRange = tube->GetOuterRadius();
    zRange = tube->GetZHalfLength();
  }
  else
  {
    G4VisExtent ranges = solid->GetExtent();
    xRange = ranges.GetXmax();
    yRange = ranges.GetYmax();
    zRange = ranges.GetZmax();
  }

	// Uniformly samples within volume shape
  G4ThreeVector posPropose;
  do
  {
    G4double xPropose = 2*(G4UniformRand() - 0.5)*xRange;
    G4double yPropose = 2*(G4UniformRand() - 0.5)*yRange;
    G4double zPropose = 2*(G4UniformRand() - 0.5)*zRange;
    posPropose.set(xPropose,yPropose,zPropose);
  }
  while (solid->Inside(posPropose) == kOutside);	// Double-checks that point lies within specified volume shape

	// Rotate and translate point into physical placement
  posPropose = (*DToWRotation)*posPropose;
  posPropose += *DToWTranslation;
 	// Return ThreeVector with interaction coordinates
  
  return posPropose;
}

	// ****** Generate Initial Particle Energy ****** //
vector<G4double>* InverseBetaKinematics::GenerateReactionKinematics() const	// Generates neutron and positron kinematics - for use in PrimaryGeneratorAction
{
	// Generates a zero vector for null returns
  vector<G4double>* zeroVector = new vector<G4double>();
  for(int i = 0; i < 10; i++) { zeroVector->push_back(0.0); }

	// Generates an antineutrino energy according to chosen fuel composition
  G4double eNu = GenerateAntiNeutrinoEnergy();

	// Computes the various energies and angles using the four-vector solution
  G4double thetaPos = GeneratePositronAngle(eNu);
  if(thetaPos < 0.*deg || thetaPos > 180.*deg) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, positron angle out of range. ***" << G4endl; return GenerateReactionKinematics(); }
  G4double ePos = CalculatePositronEnergy(thetaPos, eNu);
  if(ePos < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative positron momentum/energy. ***" << G4endl; return GenerateReactionKinematics(); }
  G4double eN = CalculateNeutronEnergy(ePos, eNu);
  if(eN < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative neutron energy/momentum. ***" << G4endl; return GenerateReactionKinematics(); }
  G4double thetaN = CalculateNeutronAngle(eN, eNu);
  if(thetaN < 0. || thetaN > 180.*deg) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron angle out of range. ***" << G4endl; return GenerateReactionKinematics(); }
  G4double phiPos = 360.*G4UniformRand()*deg; G4double phiN = 0.*deg;
  if(phiPos < 180.*deg) { phiN = phiPos + 180.*deg; }
  else			{ phiN = phiPos - 180.*deg; }

	// Assembles kinematics results into ThreeVectors to obtain <x,y,z> from <r,theta,phi>
  vector<G4double>* kinematics = new vector<G4double>();
  G4ThreeVector pHatPos(0.,0.,0.); pHatPos.setRThetaPhi(1.,thetaPos,phiPos);
  G4ThreeVector pHatN(0.,0.,0.); pHatN.setRThetaPhi(1.,thetaN,phiN);

	// Rotates reaction frame into physical frame using pHatNu
  G4ThreeVector zAxis(0.,0.,1.);
  G4ThreeVector crossNorm = zAxis.cross(antiNuDir);
  if(crossNorm.mag() > 1e-5)
  {
    G4double rotAngle = zAxis.angle(antiNuDir);
    pHatPos.rotate(crossNorm,rotAngle);
    pHatN.rotate(crossNorm,rotAngle);
  }

	// Convert total energy to kinetic energy for neutron and positron
  G4double kePos = ePos - ePlusMass;
  G4double keN = eN - nMass;

	// Placed into vector as { Positron: E, px, py, pz ; Neutron: E, px, py, pz ; Extra: positron angle wrt nu, neutron angle wrt nu } - 10 entries
  kinematics->push_back(kePos); kinematics->push_back(pHatPos.x()); kinematics->push_back(pHatPos.y()); kinematics->push_back(pHatPos.z());
  kinematics->push_back(keN); kinematics->push_back(pHatN.x()); kinematics->push_back(pHatN.y()); kinematics->push_back(pHatN.z());
  kinematics->push_back(thetaPos); kinematics->push_back(-thetaN);

	// Returns the vector of kinematics values
  if(!CheckFourVectorSolution(eNu,ePos,eN,thetaPos,thetaN)) { return GenerateReactionKinematics(); }

  return kinematics;
}

	// ****** Spectrum Test Function ****** //
void InverseBetaKinematics::GenerateKinematicsWithoutSimulation(G4int n) const
{
	// Generates histograms for particle kinetic energies and relative directions
  map<G4double,G4int>* thePosEHistogram = new map<G4double,G4int>();
  map<G4double,G4int>* theNEHistogram = new map<G4double,G4int>();
  map<G4double,G4int>* thePosAngHistogram = new map<G4double,G4int>();
  map<G4double,G4int>* theNAngHistogram = new map<G4double,G4int>();
  G4double minPos_e = 0.0*MeV;
  G4double maxPos_e = 8.2*MeV;
  G4double pos_bins = 164;
  G4double minN_e = 0.0*MeV;
  G4double maxN_e = 125.*keV;
  G4double n_bins = 1250;
  if(fMonoEnergy)	// Redefines histogram values if monoenergetic particles are selected
  {
    minPos_e = CalculatePositronEnergy(180.*deg, antiNuMonoEnergy) - ePlusMass;
    maxPos_e = CalculatePositronEnergy(0.*deg, antiNuMonoEnergy) - ePlusMass;
    pos_bins = 150;
    minN_e = CalculateNeutronEnergy((maxPos_e + ePlusMass), antiNuMonoEnergy) - nMass - 10.*eV;
    maxN_e = CalculateNeutronEnergy((minPos_e + ePlusMass), antiNuMonoEnergy) - nMass + 10.*eV;
    n_bins = 150;
  }
  G4double posIncrementer = (maxPos_e - minPos_e) / pos_bins;
  G4double nIncrementer = (maxN_e - minN_e) / n_bins;
  for(int i = 1; i <= pos_bins; i++)
  {
    G4double upperlimit = minPos_e + i*posIncrementer;
    (*thePosEHistogram)[upperlimit] = 0;
  }
  for(int i = 1; i <= n_bins; i++)
  {
    G4double upperlimit = minN_e + i*nIncrementer;
    (*theNEHistogram)[upperlimit] = 0;
  }
  G4double ang_bins = 180;
  for(int j = 1; j <= ang_bins; j++)
  {
    G4double upperlimit = j*1.*deg;
    (*thePosAngHistogram)[upperlimit] = 0;
    upperlimit = -upperlimit + 1.*deg;
    (*theNAngHistogram)[upperlimit] = 0;
  }

	// Uses random number generator to sample from spectrum n times
  G4int reps = 1;
  while(reps <= n)
  {
    vector<G4double>* kinematics = GenerateReactionKinematics();

	// Immediately sorts sampled kinematics into histogram for ease of storage and memory efficiency
    map<G4double,G4int>::iterator itr = thePosEHistogram->begin();
    while((*kinematics)[0] >= itr->first && itr != thePosEHistogram->end()) { itr++; }
    (*thePosEHistogram)[itr->first]++;
    itr = theNEHistogram->begin();
    while((*kinematics)[4] >= itr->first && itr != theNEHistogram->end()) { itr++; }
    (*theNEHistogram)[itr->first]++;
    itr = thePosAngHistogram->begin();
    while((*kinematics)[8] > itr->first && itr != thePosAngHistogram->end()) { itr++; }
    (*thePosAngHistogram)[itr->first]++;
    itr = theNAngHistogram->begin();
    while((*kinematics)[9] > itr->first && itr != theNAngHistogram->end()) { itr++; }
    (*theNAngHistogram)[itr->first]++;

	// Print status to show progress
    if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
    reps++;
  }
}

	// ****** Generate Reaction Antineutrino Energy ****** //
G4double InverseBetaKinematics::GenerateAntiNeutrinoEnergy() const
{
  G4double energy = antiNuMonoEnergy;
  if(!fMonoEnergy)
  { 
    do { energy = fission_gen->GenerateAntiNeutrinoEnergy(); }		// Uses FissionAntiNuModule to generate antineutrino energy if specified
    while (energy <= 1.81*MeV || energy > 9.5*MeV);
  }
  return energy;
}

	// ****** Calculate Target Volume Rotation ****** //
G4RotationMatrix* InverseBetaKinematics::FindTargetRotationWRTWorld() const		// Determines if logical volume has been rotated into place in the world - obtains rotation matrix
{
	// Generate null rotation matrix to start the stack
  G4RotationMatrix* rotation = new G4RotationMatrix(0.,0.,0.);
  G4VPhysicalVolume* currentVolume = worldVolume;

	// Works inward from world volume applying rotations of daughter volumes until it reaches the target volume
  if(targetVolume)
  {
    do
    {
      *rotation *= currentVolume->GetObjectRotationValue();	// Applies current volume rotation matrix to stack
      G4LogicalVolume* currentLogical = currentVolume->GetLogicalVolume();
      G4int noDaughters = currentLogical->GetNoDaughters();

	// Grabs each daughter volume and checks if target volume is contained within it
      if(noDaughters != 0)
      {
        for(G4int i = 0; i < noDaughters; i++)
        {
          currentVolume = currentLogical->GetDaughter(i);
          if(currentVolume->GetLogicalVolume()->IsAncestor(targetVolume) || currentVolume == targetVolume) break;
        }
      }
      else { currentVolume = targetVolume; }	// Assumes current volume is target volume if it has no daughter volumes
    }
    while (!(currentVolume == targetVolume));
    *rotation *= currentVolume->GetObjectRotationValue();
  }

  return rotation;
}

	// ****** Calculate Target Volume Translation ****** //
G4ThreeVector* InverseBetaKinematics::FindTargetTranslationWRTWorld() const		// Determines if logical volume has been translated into place in the world - obtains translation vector
{
	// Generate null translation vector to start the stack
  G4ThreeVector* translation = new G4ThreeVector(0.,0.,0.);
  G4VPhysicalVolume* currentVolume = worldVolume;
	// Works inward from world volume apply translations of daughter volumes until it reaches the target volume
  if(targetVolume) {
    do {
      *translation += currentVolume->GetObjectTranslation();	// Applies current volume translation vector to stack

	// Grabs each daughter volume and checks if target volume is contained within it
      G4LogicalVolume* currentLogical = currentVolume->GetLogicalVolume();
        G4int noDaughters = currentLogical->GetNoDaughters();
        if(noDaughters != 0)
      {
        for(G4int i = 0; i < noDaughters; i++)
        {
          currentVolume = currentLogical->GetDaughter(i);
          if(currentVolume->GetLogicalVolume()->IsAncestor(targetVolume) || currentVolume == targetVolume) break;
        }
      }
      else { currentVolume = targetVolume; }	// Assumes current volume is target volume if it has no daughter volumes
       //   currentVolume = targetVolume; 
    } while (!(currentVolume == targetVolume));
    *translation += currentVolume->GetObjectTranslation();
  }
  return translation;
}

	// ****** Generate Positron Angle ****** //
G4double InverseBetaKinematics::GeneratePositronAngle(G4double eNu) const
{
	// Uses coded distribution to generate a positron angle according to theory
  G4double ePlusAngle = -1.;
  G4double max_dist = 1.0;
  G4double Forward = CalculateCrossSectionWRTAngle(0.*deg, eNu);
  G4double Backward = CalculateCrossSectionWRTAngle(180.*deg, eNu);

	// Although tendency is backwards for all fission antineutrino ranges, this code is capable of evaluating E(nu) > 15 MeV where the tendency flips
  if(Forward > Backward) { max_dist = Forward; }
  else			 { max_dist = Backward; }
  G4double xSeed, ySeed, Probability;
  do
  {
	// Positron angle
    xSeed = 180.*G4UniformRand()*deg;

	// Positron angle with 4*PI correction
//    xSeed = 2*G4UniformRand() - 1.0;
//    xSeed = acos(xSeed);

	// Acceptance parameter
    ySeed = G4UniformRand();

	// Acceptance value
    Probability = CalculateCrossSectionWRTAngle(xSeed, eNu) / max_dist;
  }
  while (ySeed > Probability);	// Acceptance condition
  ePlusAngle = xSeed;

  return ePlusAngle;
}

	// ****** Calculate Positron Total Energy ****** //
G4double InverseBetaKinematics::CalculatePositronEnergy(G4double ePlusAngle, G4double nuEnergy) const
{
	// Calculates positron energy using the generated positron angle and the four-vector equation
  G4double t1, t2, t3, t4;

	// Four-vector equation solution - solves for positron momentum
  t1 = 4*ePlusMass*ePlusMass*pow((nuEnergy + pMass),2);
  t2 = pow(pMass,4) + pow(ePlusMass,4) + pow(nMass,4) + 4*nuEnergy*nuEnergy*pMass*pMass;
  t3 = 2*(pMass*pMass*ePlusMass*ePlusMass - pMass*pMass*nMass*nMass - ePlusMass*ePlusMass*nMass*nMass);
  t4 = 4*nuEnergy*pMass*(pMass*pMass + ePlusMass*ePlusMass - nMass*nMass);

  G4double a, b, cc, Determinant;
  
  a = 4*pow((nuEnergy + pMass),2) - 4*(nuEnergy*nuEnergy)*pow(cos(ePlusAngle),2);
  b = -4*nuEnergy*cos(ePlusAngle)*(pMass*pMass + ePlusMass*ePlusMass - nMass*nMass + 2*nuEnergy*pMass);
  cc = t1 - t2 - t3 - t4;
  Determinant = b*b - 4*a*cc;

	// Determine validity of calculated momentum
  G4double ePlusEnergy;
  if(Determinant < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative determinant in quadratic formula. ***" << G4endl; ePlusEnergy = -1.; }
  else		// It may be still possible to use the negative determinant by taking the real part, but this is not proven to be the case yet
  {
    G4double ePlusMomentum = (-b + sqrt(Determinant)) / (2*a);
    if(ePlusMomentum < 0.) { ePlusMomentum = (-b - sqrt(Determinant)) / (2*a); }
    if(ePlusMomentum < 0.) { ePlusMomentum = 0.; if(verbose > 1) { G4cout << "*** WARNING: Positron momentum is negative. Momentum set to zero. ***" << G4endl; } }

	// Conversion of momentum to total energy
    ePlusEnergy = sqrt(ePlusMomentum*ePlusMomentum + ePlusMass*ePlusMass);
  }

  return ePlusEnergy;
}

	// ****** Calculate Neutron Total Energy ****** //
G4double InverseBetaKinematics::CalculateNeutronEnergy(G4double ePlusEnergy, G4double nuEnergy) const
{
	// Calculates neutron total energy using positron total energy and conservation of mass/energy
  G4double nEnergy = nuEnergy + pMass - ePlusEnergy;

	// Determine validity of calculated total energy
  G4double rootCheck = nEnergy*nEnergy - nMass*nMass;
  if(rootCheck < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron total energy less than rest mass energy. ***" << G4endl; nEnergy = -1.; }

  return nEnergy;
}

	// ****** Calculate Neutron Angle ****** //
G4double InverseBetaKinematics::CalculateNeutronAngle(G4double nEnergy, G4double nuEnergy) const
{
	// Calculates neutron angle using its total energy and the four-vector equation
  G4double nAngle;

	// Conversion from total energy to momentum
  G4double nMomentum = sqrt(nEnergy*nEnergy - nMass*nMass);
  G4double t1, t2, t3, cosNAngle;

	// Four-vector equation solution
  t1 = 2*nEnergy*(nuEnergy + pMass);
  t2 = ePlusMass*ePlusMass - pMass*pMass - nMass*nMass - 2*nuEnergy*pMass;
  t3 = 2*nuEnergy*nMomentum;
  cosNAngle = (t1 + t2) / t3;
  if(cosNAngle > 1.0 && (cosNAngle - 1.0) < 1.e-8)   { cosNAngle = 1.; }	// This accounts for some slight mathematical deviations due to computation

	// Negative angles shown to mathematically equate to corresponding positive angle
  nAngle = std::fabs(acos(cosNAngle));

	// Determine validity of calculated neutron angle
  if(std::fabs(cosNAngle) > 1.0) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron angle cosine is out of acceptable bounds. ***" << G4endl; nAngle = 10.; }

  return nAngle;
}

	// ****** Four-Vector Solution Check ****** //
G4bool InverseBetaKinematics::CheckFourVectorSolution(G4double nuEnergy, G4double posE, G4double nE, G4double posAng, G4double nAng) const
{
	// Check conservation of mass-energy
  G4double eBalance = nuEnergy + pMass - posE - nE;
  if(eBalance/MeV > 1.e-6)
  { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of mass-energy test. ***" << G4endl; return false; }

	// Check conservation of momentum in antineutrino axial direction
  G4double axialBalance = nuEnergy - cos(posAng)*sqrt(posE*posE - ePlusMass*ePlusMass) - cos(nAng)*sqrt(nE*nE - nMass*nMass);
  if(axialBalance/MeV > 1.e-6)
  { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of axial momentum test. ***" << G4endl; return false; }

	// Check conservation of momentum in antineutrino radial direction
  G4double radialBalance = sin(posAng)*sqrt(posE*posE - ePlusMass*ePlusMass) - sin(nAng)*sqrt(nE*nE - nMass*nMass);
  if(radialBalance/MeV > 1.e-3)
  { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of radial momentum test. ***" << G4endl; return false; }

  return true;
}

	// ****** Calculate Interaction Cross Section ****** //
G4double InverseBetaKinematics::CalculateCrossSectionWRTAngle(G4double angle, G4double nuEnergy) const
{
  // Parameterization found in Angular Distribtuion of Neutron Inverse beta Decay
  // P. Vogel and J. F. Beacon, July 1999 - Equation (12)

	// Calculates probability distribution of event based on angle
  G4double delta = nMass - pMass;
  G4double M = (nMass + pMass)/2;
  G4double ySq = (delta*delta - ePlusMass*ePlusMass)/2;
  G4double F = 1., G = 1.26, F2 = 3.706;
  G4double fermiConst = 2.301562e-22*cm/MeV;
  G4double cosC = 0.974, innerRad = 0.024;

  G4double sigmaZero, dSigmaByDCosTheta;
  G4double eZero, pZero, vZero;
  G4double eOne, pOne, vOne;

  sigmaZero = (pow(fermiConst,2)*cosC*cosC / M_PI)*(1 + innerRad);
  eZero = nuEnergy - delta;
  pZero = sqrt(eZero*eZero - ePlusMass*ePlusMass);
  vZero = pZero / eZero;
  eOne = eZero*(1 - (nuEnergy / M)*(1 - vZero*cos(angle))) - (ySq / M);
  pOne = sqrt(eOne*eOne - ePlusMass*ePlusMass);
  vOne = pOne / eOne;

  G4double T1, T2, T3, T4, Gamma;

  T1 = 2*(F + F2)*G*((2*eZero + delta)*(1 - vZero*cos(angle)) - (ePlusMass*ePlusMass / eZero));
  T2 = (F*F + G*G)*(delta*(1 + vZero*cos(angle)) + (ePlusMass*ePlusMass / eZero));
  T3 = (F*F + 3*G*G)*((eZero + delta)*(1 - (cos(angle) / vZero)) - delta);
  T4 = (F*F - G*G)*((eZero + delta)*(1 - (cos(angle) / vZero)) - delta)*vZero*cos(angle);
  Gamma = T1 + T2 + T3 + T4;

  dSigmaByDCosTheta = (sigmaZero/2)*((F*F + 3*G*G) + (F*F - G*G)*vOne*cos(angle))*eOne*pOne - (sigmaZero/2)*(Gamma / M)*eZero*pZero;
  return dSigmaByDCosTheta;
}

	// ****** Print Function ****** //
void InverseBetaKinematics::PrintAllParameters() const
{
  G4cout << "\n"
	 << "******* InverseBetaKinematics currently specified at the following parameters *******\n" ;
  if(fMonoEnergy)
 {G4cout << "\t   Fission Spectrum:		" << "Inactive" << "\n"
         << "\t   Antineutrino Mono Energy:	" << G4BestUnit(antiNuMonoEnergy,"Energy") << "\n" ;}
  else
 {G4cout << "\t   Fission Spectrum:		" << "Active" << "\n"
         << "\t   Uranium-235 Content:		" << 100*(fission_gen->GetUranium235Content(true)) << "%" << "\n"
         << "\t   Uranium-238 Content:		" << 100*(fission_gen->GetUranium238Content(true)) << "%" << "\n"
         << "\t   Plutonium-239 Content:	" << 100*(fission_gen->GetPlutonium239Content(true)) << "%" << "\n"
         << "\t   Plutonium-241-Content:	" << 100*(fission_gen->GetPlutonium241Content(true)) << "%" << "\n" ;}
  G4cout << "\t   Antineutrino Flux Direction:	" << "x = " << antiNuDir.x() << ", y = " << antiNuDir.y() << ", z = " << antiNuDir.z() << "\n"
         << "\t   Target Volume Name:		" << targetName << "\n"
	 << "\t   Target Volume Dimensions:     X: " <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetXmax() - targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetXmin() << "\tY: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetYmax()  - targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetYmin() << "\tZ: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetZmax()  - targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetZmin() << "\n"
	 << "\t   Target Volume Center:     X: " <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[0] << "\tY: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[1] << "\tZ: "
	 <<  targetVolume->GetLogicalVolume()->GetSolid()->GetExtent().GetExtentCenter()[2] << "\n"
         << "*************************************************************************************\n"
         << G4endl;
}

// EOF
