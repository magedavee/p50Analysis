// Unrestricted Use - Property of AECL
//
// CosmicMuonGenerator.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Cosmic Muon Energy Spectrum Generator
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "CosmicMuonGenerator.hh"		// Specifies the file which contains the class structure

#include "CosmicMuonMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "CosmicCosineGenerator.hh"

#include <G4SystemOfUnits.hh>
#include "Randomize.hh"				// Specifies the classes which contain structures called upon in this class
#include "G4UnitsTable.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies classes defining all global parameters and variable types
#include <math.h>
#include <map>

void CosmicMuonGenerator::setDefaults() {
    muMass = G4MuonPlus::MuonPlusDefinition()->GetPDGMass();      // Placed in new variable for ease of coding
    
    // Defines hard limits for each energy spectrum range
    minLipari_e = 1.0e0*GeV;
    maxLipari_e = 1.0e6*GeV;
    minBESS_p = 0.576*GeV;
    maxBESS_p = 20.552*GeV;
    
    // Default minimum and maximum energies sampled with Lipari limits, default mono energy
    min_val = minLipari_e;
    max_val = maxLipari_e;
    if(fBESS) { min_val = minBESS_p; max_val = maxBESS_p; }
    muMono = 1.*GeV;
    
    // Define other default values
    fMonoEnergy = false;
    fPlusOnly = false;
    fMinusOnly = false;
    ratio = 0.0;
    testDir = G4ThreeVector(0.,1.,0.);
}

// ****** Constructor ******* //
CosmicMuonGenerator::CosmicMuonGenerator(const G4String target): CosmicCosineGenerator(target), fBESS(false) {
    setDefaults();
    
  if(verbose > 1) { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }

  GenerateLipariPDFTable();		// Intention to code CDF function later for faster simulation
  GenerateBESSPDFTable();

  muon_messenger = new CosmicMuonMessenger(this);

	// Outputs default parameter list to terminal
  std::stringstream stream;
  if(ratio <= 0.0) { stream << "Energy-Dependent"; }
  else { stream << ratio; }
  G4cout << "\n"
         << "********* CosmicMuonGenerator initiated with the following default parameters. *********\n"
         << "\t   Mu+ to Mu- Ratio:		" << stream.str() << "\n"
         << "\t   Minimum Sample Energy:	" << G4BestUnit(min_val,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_val,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "****************************************************************************************\n"
         << G4endl;
}

// ****** Overloaded Constructor ******* //
CosmicMuonGenerator::CosmicMuonGenerator(G4bool BESS, G4double PlusMinus): CosmicCosineGenerator(), fBESS(BESS) {
    setDefaults();
   
	// Determine user-specified muon ratio
  if(PlusMinus == -1.0)		// An input of -1 in constructor uses mu+ only
  {
    fPlusOnly = true;
    if(verbose > 1)
    { G4cout << "\tSimulation will now only generate mu+." << G4endl; }
  }
  else if(PlusMinus == -2.0)	// An input of -2 in constructor uses mu- only
  {
    fMinusOnly = true;
    if(verbose > 1)
    { G4cout << "\tSimulation will now only generate mu-." << G4endl; }
  }
  else if(PlusMinus <= 0.0) 	// All other negative inputs (including 0) in constructor indicates the spectrum-defined muon ratios
  {
    if(verbose > 1)
    { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }
  }
  ratio = PlusMinus;		// All other positive inputs (except 0) are accepted as user-defined muon ratios

  GenerateLipariPDFTable();		// Intention to code CDF function later for faster simulation
  GenerateBESSPDFTable();

  muon_messenger = new CosmicMuonMessenger(this);

	// Outputs user-specified parameter list to terminal
  std::stringstream stream;
  if(fPlusOnly) { stream << "Only mu+"; }
  else if(fMinusOnly) { stream << " Only mu-"; }
  else if(ratio <= 0.0) { stream << "Energy-Dependent"; }
  else { stream << ratio; }
  G4cout << "\n"
         << "********* CosmicMuonGenerator initiated with the following default parameters. *********\n"
         << "\t   Mu+ to Mu- Ratio:		" << stream.str() << "\n";
  if(fBESS)
{ G4cout << "\t   Minimum Sample Momentum:	" << G4BestUnit(min_val,"Energy") << "/c" << "\n"
         << "\t   Maximum Sample Momentum:	" << G4BestUnit(max_val,"Energy") << "/c" << "\n"; }
  else
{ G4cout << "\t   Minimum Sample Energy:	" << G4BestUnit(min_val,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_val,"Energy") << "\n"; }
  G4cout << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "****************************************************************************************\n"
         << G4endl;
}

// ****** Overloaded Constructor ****** //
CosmicMuonGenerator::CosmicMuonGenerator(G4double monoE, G4double PlusMinus): CosmicCosineGenerator(), fBESS(false) {
    setDefaults();
    muMono = monoE;
    fMonoEnergy = true;

	// Determine user-specified muon ratio
  if(PlusMinus == -1.0)		// An input of -1 in constructor uses mu+ only
  {
    fPlusOnly = true;
    if(verbose > 1)
    { G4cout << "\tSimulation will now only generate mu+." << G4endl; }
  }
  else if(PlusMinus == -2.0)	// An input of -2 in constructor uses mu- only
  {
    fMinusOnly = true;
    if(verbose > 1)
    { G4cout << "\tSimulation will now only generate mu-." << G4endl; }
  }
  else if(PlusMinus <= 0.0) 	// All other negative inputs (including 0) in constructor indicates the spectrum-defined muon ratios
  {
    if(verbose > 1)
    { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }
  }
  ratio = PlusMinus;		// All other positive inputs (except 0) are accepted as user-defined muon ratios

  GenerateLipariPDFTable();		// Intention to code CDF function later for faster simulation
  GenerateBESSPDFTable();

  muon_messenger = new CosmicMuonMessenger(this);

	// Outputs user-specified parameter list to terminal
  std::stringstream stream;
  if(fPlusOnly) { stream << "Only mu+"; }
  else if(fMinusOnly) { stream << " Only mu-"; }
  else if(ratio <= 0.0) { stream << "Energy-Dependent"; }
  else { stream << ratio; }
  G4cout << "\n"
         << "********* CosmicMuonGenerator initiated with the following default parameters. *********\n"
         << "\t   Mu+ to Mu- Ratio:		" << stream.str() << "\n"
         << "\t   Muon Energy:			" << G4BestUnit(muMono,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "****************************************************************************************\n"
         << G4endl;
}

CosmicMuonGenerator::~CosmicMuonGenerator() {
  delete muon_messenger;
}


	// The following functions allow the user to adjust the sample range of the momentum spectrum

	// ****** Change Range Minimum ****** //
void CosmicMuonGenerator::SetMinRange(G4double min) {
  if(fBESS)
  {
    if(min < minBESS_p || min > maxBESS_p)
    {
      min = min_val;	// If specified minimum is not within pre-defined hard limits, keep old value
      G4cout << "*** ATTENTION: Invalid minimum sample momentum parameter entered. Must be a number between 0.576 GeV and 20.552 GeV. Previous value kept. ***" << G4endl;
    }
    else if(min > max_val)
    {
      min = max_val;	// If specified minimum is larger than current maximum, set to current maximum
      G4cout << "*** ATTENTION: Invalid minimum sample momentum parameter entered. Must be smaller than the maximum sample momentum. Minimum set equal to maximum. ***" << G4endl;
    }
  }
  else
  {
    if(min < minLipari_e || min > maxLipari_e)
    {
      min = min_val;	// If specified minimum is not within pre-defined hard limits, keep old value
      G4cout << "*** ATTENTION: Invalid minimum sample energy parameter entered. Must be a number between 1 GeV and 1000 TeV. Previous value kept. ***" << G4endl;
    }
    else if(min > max_val)
    {
      min = max_val;	// If specified minimum is larger than current maximum, set to current maximum
      G4cout << "*** ATTENTION: Invalid minimum sample energy parameter entered. Must be smaller than the maximum sample energy. Minimum set equal to maximum. ***" << G4endl;
    }
  }
  min_val = min; 
  if(verbose > 1)
  {
    if(fBESS) { G4cout << "\tMinimum sample momentum parameter set to " << G4BestUnit(min_val,"Energy") << "/c." << G4endl; }
    else      { G4cout << "\tMinimum sample energy parameter set to " << G4BestUnit(min_val,"Energy") << "." << G4endl; }
  }
}

	// ****** Change Range Maximum ****** //
void CosmicMuonGenerator::SetMaxRange(G4double max)
{
  if(fBESS)
  {
    if(max < minBESS_p || max > maxBESS_p)
    {
      max = max_val;	// If specified maximum is not within pre-defined hard limits, keep old value
      G4cout << "*** ATTENTION: Invalid maximum sample momentum parameter entered. Must be a number between 0.576 GeV and 20.552 GeV. Previous value kept. ***" << G4endl;
    }
    if(max < min_val)
    {
      max = min_val;	// If specified maximum is smaller than current minimum, set to current mininum
      G4cout << "*** ATTENTION: Invalid maximum sample momentum parameter entered. Must be greater than the minimum sample momentum. Maximum set equal to minimum. ***" << G4endl;
    }
  }
  else
  {
    if(max < minLipari_e || max > maxLipari_e)
    {
      max = max_val;	// If specified maximum is not within pre-defined hard limits, keep old value
      G4cout << "*** ATTENTION: Invalid maximum sample energy parameter entered. Must be a number between 1 GeV and 1000 TeV. Previous value kept. ***" << G4endl;
    }
    if(max < min_val)
    {
      max = min_val;	// If specified maximum is smaller than current minimum, set to current minimum
      G4cout << "*** ATTENTION: Invalid maximum sample energy parameter entered. Must be greater than the minimum sample energy. Maximum set equal to minimum. ***" << G4endl;
    }
  }
  max_val = max;
  if(verbose > 1)
  {
    if(fBESS) { G4cout << "\tMaximum sample momentum parameter set to " << G4BestUnit(max_val,"Energy") << "/c." << G4endl; }
    else      { G4cout << "\tMaximum sample energy parameter set to " << G4BestUnit(max_val,"Energy") << "." << G4endl; }
  }
}

	// The following function allows the user to specify the neutron mono-energy

	// ****** Set Monoenergetic Neutrons ****** //
void CosmicMuonGenerator::SetMuonMonoEnergy(G4double E)
{
  if(E < 0.) { G4cout << "*** ATTENTION: Invalid mono-energy value. Must be a positive number. Previous value kept. ***"; return; }
  muMono = E;
  if(!fMonoEnergy && verbose > 0)
  { G4cout << "*** CAUTION: Cosmic Muon Module has not been primed to fire mono-energetic muons. Parameter will not be used until this option is activated. ***" << G4endl; }
}

	// ****** Change Muon Ratio ****** //
void CosmicMuonGenerator::SetPlusMinusRatio(G4double PlusMinus)
{
	// Removes single-type-generation flags
  fPlusOnly = false;	
  fMinusOnly = false;

  if(PlusMinus <= 0.0) 			// If input is negative (including 0), spectrum-defined ratios will be used
  {
    PlusMinus = 0.0;
    if(verbose > 1)
    { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }
  }
  if(verbose > 1 && ratio > 0.0)	// All other inputs are accpeted as a muon ratio
  {
    G4cout << "\tMu+/Mu- ratio currently set to " << ratio << "." << G4endl;
  }
  ratio = PlusMinus;
}

	// ****** Set Mu+ Flag ****** //
void CosmicMuonGenerator::SetMuonPlusOnlyFlag(G4bool flag)
{
	// Removes mu- flag before setting mu+ flag
  if(fMinusOnly && verbose > 1) { G4cout << "\tMu- flag has been previously set. Resetting mu- flag." << G4endl; }
  fMinusOnly = false;

	// Sets mu+ flag as requested (accepts both true AND false)
  fPlusOnly = flag;
  if(fPlusOnly) { G4cout << "\tSimulation will now only generate mu+." << G4endl; }
  else
  {
    if(verbose > 1 && ratio <= 0.0) { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }
    else if(verbose > 1) { G4cout << "\tMu+/Mu- ratio has been reset to " << ratio << "." << G4endl; }
  }
}

	// ****** Set Mu-  Flag ****** //
void CosmicMuonGenerator::SetMuonMinusOnlyFlag(G4bool flag)
{
	// Removes mu+ flag before setting mu- flag
  if(fPlusOnly && verbose > 1) { G4cout << "\tMu+ flag has been previously set. Resetting mu+ flag." << G4endl; }
  fPlusOnly = false;

	// Sets mu- flag as requested (accepts both true AND false)
  fMinusOnly = flag;
  if(fPlusOnly) { G4cout << "\tSimulation will now only generate mu-." << G4endl; }
  else
  {
    if(verbose > 1 && ratio <= 0.0) { G4cout << "\tEnergy-dependent ratio values are currently being used." << G4endl; }
    else if(verbose > 1) { G4cout << "\tMu+/Mu- ratio has been reset to " << ratio << "." << G4endl; }
  }
}

	// ****** Set BESS Spectrum ****** //
void CosmicMuonGenerator::SetBESSSpectrumFlag(G4bool flag)
{
  fBESS = flag;		// Set flag which specifies use of BESS spectrum
  if(fBESS)
  {
    min_val = minBESS_p; max_val = maxBESS_p;
    if(verbose > 0) { G4cout << "\tBESS Spectrum enabled. Sampled parameter is now the particle momentum." << G4endl; }
    if(verbose > 1) { G4cout << "\tMinimum and maximum values have been changed to the default values for this spectrum." << G4endl; }
  }
  else
  {
    min_val = minLipari_e; max_val = maxLipari_e;
    if(verbose > 0) { G4cout << "\tBESS Spectrum disabled. Sampled parameter is now the particle kinetic energy." << G4endl; }
    if(verbose > 1) { G4cout << "\tMinimum and maximum values have been changed to the default values for this spectrum." << G4endl; }
  }
}

	// ****** Set Test Angle ****** //
void CosmicMuonGenerator::SetTestAngle(G4double angle)
{
  G4double theta = 90.*deg;
  G4double phi = 90.*deg - angle;
  testDir.setRThetaPhi(1,theta,phi);
}

	// The following functions allow the user to specify the source and location for the angular distribution generator


// -------------------------------------------------------------------------------------------------- //

	// ****** Spectrum Test Function ****** //
void CosmicMuonGenerator::GenerateEnergiesWithoutSimulation(const G4int n) const		// Used to test the shape and statistics of the spectrum generator
{
	// Aborts procedure if mono-energetic neutrons have been activated
  if(fMonoEnergy) { G4cout << "*** WARNING: Mono-energetic muons enabled. Spectrum contains only one energy value. Operation aborted. ***" << G4endl; return; }

	// Test function chosen based on which spectrum is specified for use
  if(fBESS) { GenerateBESSEnergiesWithoutSimulation(n); }
  else { GenerateLipariEnergiesWithoutSimulation(n); }
}

	// ****** Test Implementation for Lipari Spectrum ****** //
void CosmicMuonGenerator::GenerateLipariEnergiesWithoutSimulation(const G4int n) const		// Used to test the shape and statistics of the Lipari spectrum generator
{
	// Generates histograms for particle kinetic energies
  std::map<G4double,G4int>* thePlusHistogram = new std::map<G4double,G4int>();
  std::map<G4double,G4int>* theMinusHistogram = new std::map<G4double,G4int>();
  G4double n_bins = 500;
  G4double incrementer = (log10(max_val) - log10(min_val))/n_bins;
  for(int i = 0; i < n_bins; i++)
  {
    G4double lowerlimit = pow(10.0,(log10(min_val) + i*incrementer));
    (*thePlusHistogram)[lowerlimit] = 0;
    (*theMinusHistogram)[lowerlimit] = 0;
  }

	// Uses random number generator to sample from spectrum n times
  G4int reps = 0;
  G4int muPlusCount = 0;
  G4int muMinusCount = 0;
  while(reps < n)
  {
    if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
    G4double muEnergy = GenerateLipariMuonEnergy(testDir);
    G4int muType = GenerateLipariMuonType(muEnergy,testDir);

	// Immediately sorts sampled energy into histogram for ease of storage and memory efficiency
    if(muType) 
    { 
      std::map<G4double,G4int>::iterator itr = theMinusHistogram->begin();
      while(muEnergy > itr->first && itr != theMinusHistogram->end()) { itr++; }
      itr--; (*theMinusHistogram)[itr->first]++;
      muMinusCount++;
    }
    else
    {
      std::map<G4double,G4int>::iterator itr = thePlusHistogram->begin();
      while(muEnergy > itr->first && itr != thePlusHistogram->end()) { itr++; }
      itr--; (*thePlusHistogram)[itr->first]++;
      muPlusCount++;
    }
    reps++;
  }
}

	// ****** Test Implementation for BESS Spectrum ****** //
void CosmicMuonGenerator::GenerateBESSEnergiesWithoutSimulation(const G4int n) const		// Used to test the shape and statistics of the BESS spectrum generator
{
	// Generates histograms for particle kinetic energies
  std::map<G4double,G4int>* thePlusHistogram = new std::map<G4double,G4int>();
  std::map<G4double,G4int>* theMinusHistogram = new std::map<G4double,G4int>();
  G4double n_bins = 500;
  G4double incrementer = (max_val - min_val)/n_bins;
  for(int i = 0; i < n_bins; i++)
  {
    G4double lowerlimit = min_val + i*incrementer;
    lowerlimit = sqrt(lowerlimit*lowerlimit + muMass*muMass) - muMass;		// Transformation from momentum to kinetic energy
    (*thePlusHistogram)[lowerlimit] = 0;
    (*theMinusHistogram)[lowerlimit] = 0;
  }

	// Uses random number generator to sample from spectrum n times
  G4int reps = 1;
  G4int muPlusCount = 0;
  G4int muMinusCount = 0;
  while(reps <= n)
  {
    G4double muEnergy = GenerateBESSMuonEnergy();
    G4int muType = GenerateBESSMuonType(muEnergy);

	// Immediately sorts sampled energy into histogram for ease of storage and memory efficiency
    if(muType) 
    { 
      std::map<G4double,G4int>::iterator itr = theMinusHistogram->begin();
      while(muEnergy > itr->first && itr != theMinusHistogram->end()) { itr++; }
      itr--; (*theMinusHistogram)[itr->first]++;
      muMinusCount++;
    }
    else
    {
      std::map<G4double,G4int>::iterator itr = thePlusHistogram->begin();
      while(muEnergy > itr->first && itr != thePlusHistogram->end()) { itr++; }
      itr--; (*thePlusHistogram)[itr->first]++;
      muPlusCount++;
    }

	// Print status to show test progress
    if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
    reps++;
  }
}

	// ****** Generate Initial Particle Energy ****** //
G4double CosmicMuonGenerator::GenerateMuonEnergy(G4ThreeVector muDir) const	// Generation of muon energy - for use in PrimaryGeneratorAction
{
	// Output as a kinetic energy
  G4double result = 0.;
  if(fMonoEnergy) { result = muMono; }			// Returns the monoenergetic neutron if specified
  else if(fBESS) { result = GenerateBESSMuonEnergy(); }
  else { result = GenerateLipariMuonEnergy(muDir); }

  return result;
}

	// ****** Implementation of Lipari Spectrum ****** //
G4double CosmicMuonGenerator::GenerateLipariMuonEnergy(G4ThreeVector muDir) const	// Uses Lipari table of values indicated below to generate spectral shape
{
	// Determine the bin that muon zenith angle falls into
  G4double muAngle = std::fabs(muDir.angle(G4ThreeVector(0.,1.,0.)));
  G4int angBin = 0;
  while(angBin < 6 && muAngle > acos(Ang[angBin+1])) { angBin++; }

	// Generates vector of probabilities corresponding to specific angle
  G4double AngFlx[13];
  for(int i = 0; i < 13; i++)
  {
	// Uses quadratic spline interpolation on probabilities bounding the determined angle
    G4double a, b, c;
    a = log10(Flx[i][angBin]);
    b = ZAng[i][angBin];
    c = (ZAng[i][angBin+1]-ZAng[i][angBin])/(2.0*(acos(Ang[angBin+1])-acos(Ang[angBin])));
    AngFlx[i] = a + b*(muAngle - acos(Ang[angBin])) + c*pow((muAngle - acos(Ang[angBin])),2);
  }

	// Generates Z-values for quadratic spline interpolation along energy axis
  G4double ZEn[13];
  ZEn[0] = (AngFlx[1]-AngFlx[0])/(En[1]-En[0])/2.0;
  for(int j = 1; j < 13; j++)
  {
    ZEn[j] = -ZEn[j-1] + 2.0*(AngFlx[j]-AngFlx[j-1])/(En[j]-En[j-1]);
  }

  G4double muEnergy, Prob;
  do {
	// Determine the bin that muon energy falls into
    muEnergy = 6.0*G4UniformRand();
    G4int enBin = 0;
    while (enBin < 11 && muEnergy > En[enBin+1]) { enBin++; }

	// Uses quadratic spline interpolation on probabilities bounding the determined energy
    G4double a, b, c;
    a = AngFlx[enBin];
    b = ZEn[enBin];
    c = (ZEn[enBin+1]-ZEn[enBin])/(2.0*(En[enBin+1]-En[enBin]));
    G4double TotalFlx = a + b*(muEnergy - En[enBin]) + c*pow((muEnergy - En[enBin]),2);

    Prob = pow(10.0,TotalFlx);	// Probability at sampled muon zenith angle and energy
  }
  while(Flx[0][0]*G4UniformRand() > Prob);		// Repeat final energy sampling procedure if value not accepted

  muEnergy = pow(10.0,muEnergy)*GeV;	// Final transformation from exponent to kinetic energy
  return muEnergy;
}

	// ****** Implementation of BESS Spectrum ****** //
G4double CosmicMuonGenerator::GenerateBESSMuonEnergy() const			// Uses BESS table of values indicated below to generate spectral shape
{
  // Test Rejection sampling to create Bess µ+ Atmospheric Flux distribution - written by M. Thompson
  // Total muon energy range 0.576 GeV -> 20.552 GeV
  // N.B. 20.552 - 0.576 = 19.976
  //
  // SampX Range: 0 -> 1 with uniform distribution
  //
  // generate SampX and muon energy (MuonEnergy) pairs 
  //
  // if SampX > normalized acceptance cutoff value (Normalized differential flux) for the range reject it;

  G4double MuonMomentum;
  G4double SampX, Acceptance;
  G4int Bin = 0;
  do
  {
    MuonMomentum = (G4UniformRand()*(max_val/GeV - min_val/GeV)) + min_val/GeV;

	// Calculate the Bin that MuonMomentum falls into
    for(int i = 0; i < 48; i++)
    {
      if((MuonMomentum >= BinRangeMin[i]) && (MuonMomentum <= BinRangeMax[i])) { Bin = i; break; }
    }

	// Calculate the probability at particular MuonMomentum
    G4double muPlusProb = 0.0, muMinusProb = 0.0;
    if(MuonMomentum <= BinRangeAvg[Bin])
    { 
      if(Bin == 0) { muPlusProb = BinAcceptanceCutoff[0][Bin]; muMinusProb = BinAcceptanceCutoff[1][Bin]; }
      else
      {
        muPlusProb = (BinSlope[0][Bin-1]*(MuonMomentum - BinRangeAvg[Bin]) + BinAcceptanceCutoff[0][Bin]);
        muMinusProb = (BinSlope[1][Bin-1]*(MuonMomentum - BinRangeAvg[Bin]) + BinAcceptanceCutoff[1][Bin]);
      }
    }
    else
    {
      muPlusProb = (BinSlope[0][Bin]*(MuonMomentum - BinRangeAvg[Bin]) + BinAcceptanceCutoff[0][Bin]);
      muMinusProb = (BinSlope[1][Bin]*(MuonMomentum - BinRangeAvg[Bin]) + BinAcceptanceCutoff[1][Bin]);
    }

	// Determine which distributions are necessary
    if(fPlusOnly) { Acceptance = muPlusProb; SampX = G4UniformRand()*(BinAcceptanceCutoff[0][0]); }
    else if(fMinusOnly) { Acceptance = muMinusProb; SampX = G4UniformRand()*(BinAcceptanceCutoff[1][1]); }
    else if(ratio <= 0.0) { Acceptance = muPlusProb + muMinusProb; SampX = G4UniformRand()*(BinAcceptanceCutoff[0][0] + BinAcceptanceCutoff[1][1]); }
    else
    {
      G4double plusFraction = ratio / (ratio + 1.0); G4double oldFraction = muPlusProb / muMinusProb;
      Acceptance = (muPlusProb*plusFraction / oldFraction) + (muMinusProb*(1.0 - plusFraction) / (1.0 - oldFraction));
      SampX = G4UniformRand()*((BinAcceptanceCutoff[0][0]*plusFraction / oldFraction) + (BinAcceptanceCutoff[1][1]*(1.0 - plusFraction) / (1.0 - oldFraction)));
    }
  }
  while (SampX > Acceptance);

	// Final transformation from momentum to kinetic energy
  MuonMomentum = MuonMomentum*GeV;
  G4double MuonEnergy = sqrt(MuonMomentum*MuonMomentum + muMass*muMass) - muMass;
  return MuonEnergy;
}

	// ****** Generate Muon Type ****** //
G4int CosmicMuonGenerator::GenerateMuonType(G4double keMu,G4ThreeVector muDir) const
{
  G4int result;
  if(ratio > 0.0)
  {
    G4double plusFraction = ratio / (ratio + 1.0);
    if(G4UniformRand() < plusFraction) { result = 0; }
    else                               { result = 1; }
  }
  else if(fBESS) { result = GenerateBESSMuonType(keMu); }
  else { result = GenerateLipariMuonType(keMu,muDir); }
  return result;
}

	// ****** Implementation of Lipari Spectrum Muon Type ****** //
G4int CosmicMuonGenerator::GenerateLipariMuonType(G4double keMu,G4ThreeVector muDir) const
{
	// Return the specified type if a type flag is set
  if(fPlusOnly) { return 0; }
  if(fMinusOnly) { return 1; }

  G4double muAngle = cos(std::fabs(muDir.angle(G4ThreeVector(0.,1.,0.)))); 
  G4double muEnergy = log10(keMu);
  G4int angBin = 0, enBin = 0;
  while(muAngle < Ang[angBin+1] && angBin < 7) { angBin++; }		// Find corresponding angle bin
  while(muEnergy > En[enBin+1] && enBin < 12) { enBin++; }		// Find corresponding energy bin

	// Uses three linear interpolations to determine type ratio from spectrum table
  G4double lowEnRatio = (muAngle - Ang[angBin])*(Ratio[enBin][angBin+1] - Ratio[enBin][angBin])/(Ang[angBin+1] - Ang[angBin]) + Ratio[enBin][angBin];
  G4double highEnRatio = (muAngle - Ang[angBin])*(Ratio[enBin+1][angBin+1] - Ratio[enBin+1][angBin])/(Ang[angBin+1] - Ang[angBin]) + Ratio[enBin+1][angBin];
  G4double newRatio = (muEnergy - En[enBin])*(highEnRatio - lowEnRatio)/(En[enBin+1] - En[enBin]) + lowEnRatio;
  G4double plusFraction = newRatio / (newRatio + 1.0);

	// Randomly samples muon type using the type fraction
  if(G4UniformRand() < plusFraction) { return 0; }	// Return of zero to be interpreted as a mu+
  else { return 1; }					// Return of one to be interpreted as a mu-
}

	// ****** Implementation of BESS Spectrum Muon Type ****** //
G4int CosmicMuonGenerator::GenerateBESSMuonType(G4double keMu) const
{
	// Return the specified type if a type flag is set
  if(fPlusOnly) { return 0; }
  if(fMinusOnly) { return 1; }

  G4double pMu = sqrt((keMu + muMass)*(keMu + muMass) - muMass*muMass)/GeV;
  G4int Bin = 0;
  for(int i = 0; i < 48; i++)		// Searches BESS table and uses linear interpolation
  {
    if((pMu >= BinRangeMin[i]) && (pMu <= BinRangeMax[i])) { Bin = i; break; }
  }

  G4double muPlusProb = 0.0, muMinusProb = 0.0;
  if(pMu <= BinRangeAvg[Bin])
  { 
    if(Bin == 0) { muPlusProb = BinAcceptanceCutoff[0][Bin]; muMinusProb = BinAcceptanceCutoff[1][Bin]; }
    else
    {
      muPlusProb = (BinSlope[0][Bin-1]*(pMu - BinRangeAvg[Bin]) + BinAcceptanceCutoff[0][Bin]);
      muMinusProb = (BinSlope[1][Bin-1]*(pMu - BinRangeAvg[Bin]) + BinAcceptanceCutoff[1][Bin]); 
    }
  }
  else
  {
    muPlusProb = (BinSlope[0][Bin]*(pMu - BinRangeAvg[Bin]) + BinAcceptanceCutoff[0][Bin]);
    muMinusProb = (BinSlope[1][Bin]*(pMu - BinRangeAvg[Bin]) + BinAcceptanceCutoff[1][Bin]);
  }
  G4double relative = muPlusProb / muMinusProb;
  G4double plusFraction = relative / (relative + 1.0);

	// Randomly samples muon type using the type fraction
  if(G4UniformRand() < plusFraction) { return 0; }	// Return of zero to be interpreted as a mu+
  else { return 1; }					// Return of one to be interpreted as a mu-
}

	// The following functions construct the tables used to generate cosmic muon energy and angular distributions

	// ****** Lipari Spectrum Data Tables ****** //
void CosmicMuonGenerator::GenerateLipariPDFTable()
{
  // See Lepton Spectra in the Earth's Atmosphere
  // P. Lipari, August 1992, Table 6 - Flux of mu+ and mu- at sea level

  		Ang[0]=1.0;		Ang[1]=0.6;		Ang[2]=0.4;		Ang[3]=0.3;		Ang[4]=0.2;		Ang[5]=0.1;		Ang[6]=0.05;		Ang[7]=0.0;
  En[0]=0.0;	Flx[0][0]=2.24e-3;	Flx[0][1]=4.04e-4;	Flx[0][2]=1.19e-4;	Flx[0][3]=3.62e-5;	Flx[0][4]=9.00e-6;	Flx[0][5]=1.08e-6;	Flx[0][6]=2.37e-7;	Flx[0][7]=5.15e-8;
  En[1]=0.5;	Flx[1][0]=8.16e-4;	Flx[1][1]=2.80e-4;	Flx[1][2]=1.06e-4;	Flx[1][3]=4.42e-5;	Flx[1][4]=1.33e-5;	Flx[1][5]=1.88e-6;	Flx[1][6]=4.23e-7;	Flx[1][7]=6.45e-8;
  En[2]=1.0;	Flx[2][0]=1.10e-4;	Flx[2][1]=6.78e-5;	Flx[2][2]=3.95e-5;	Flx[2][3]=2.37e-5;	Flx[2][4]=1.05e-5;	Flx[2][5]=2.28e-6;	Flx[2][6]=6.16e-7;	Flx[2][7]=8.88e-8;
  En[3]=1.5;	Flx[3][0]=6.80e-6;	Flx[3][1]=6.09e-6;	Flx[3][2]=5.07e-6;	Flx[3][3]=4.11e-6;	Flx[3][4]=2.80e-6;	Flx[3][5]=1.17e-6;	Flx[3][6]=4.75e-7;	Flx[3][7]=9.54e-8;
  En[4]=2.0;	Flx[4][0]=2.48e-7;	Flx[4][1]=2.81e-7;	Flx[4][2]=2.91e-7;	Flx[4][3]=2.84e-7;	Flx[4][4]=2.59e-7;	Flx[4][5]=1.87e-7;	Flx[4][6]=1.21e-7;	Flx[4][7]=4.55e-8;
  En[5]=2.5;	Flx[5][0]=6.06e-9;	Flx[5][1]=8.12e-9;	Flx[5][2]=9.78e-9;	Flx[5][3]=1.08e-8;	Flx[5][4]=1.18e-8;	Flx[5][5]=1.18e-8;	Flx[5][6]=1.04e-8;	Flx[5][7]=6.78e-9;
  En[6]=3.0;	Flx[6][0]=1.14e-10;	Flx[6][1]=1.69e-10;	Flx[6][2]=2.24e-10;	Flx[6][3]=2.67e-10;	Flx[6][4]=3.28e-10;	Flx[6][5]=4.02e-10;	Flx[6][6]=4.19e-10;	Flx[6][7]=3.69e-10;
  En[7]=3.5;	Flx[7][0]=1.87e-12;	Flx[7][1]=2.91e-12;	Flx[7][2]=4.06e-12;	Flx[7][3]=5.06e-12;	Flx[7][4]=6.65e-12;	Flx[7][5]=9.14e-12;	Flx[7][6]=1.04e-11;	Flx[7][7]=1.05e-11;
  En[8]=4.0;	Flx[8][0]=2.81e-14;	Flx[8][1]=4.50e-14;	Flx[8][2]=6.47e-14;	Flx[8][3]=8.26e-14;	Flx[8][4]=1.13e-13;	Flx[8][5]=1.64e-13;	Flx[8][6]=1.95e-13;	Flx[8][7]=2.07e-13;
  En[9]=4.5;	Flx[9][0]=4.06e-16;	Flx[9][1]=6.58e-16;	Flx[9][2]=9.60e-16;	Flx[9][3]=1.24e-15;	Flx[9][4]=1.72e-15;	Flx[9][5]=2.60e-15;	Flx[9][6]=3.16e-15;	Flx[9][7]=3.42e-15;
  En[10]=5.0;	Flx[10][0]=5.78e-18;	Flx[10][1]=9.41e-18;	Flx[10][2]=1.38e-17;	Flx[10][3]=1.79e-17;	Flx[10][4]=2.51e-17;	Flx[10][5]=3.85e-17;	Flx[10][6]=4.72e-17;	Flx[10][7]=5.16e-17;
  En[11]=5.5;	Flx[11][0]=8.18e-20;	Flx[11][1]=1.33e-19;	Flx[11][2]=1.96e-19;	Flx[11][3]=2.55e-19;	Flx[11][4]=3.59e-19;	Flx[11][5]=5.53e-19;	Flx[11][6]=6.81e-19;	Flx[11][7]=7.47e-19;
  En[12]=6.0;	Flx[12][0]=1.16e-21;	Flx[12][1]=1.89e-21;	Flx[12][2]=2.78e-21;	Flx[12][3]=3.61e-21;	Flx[12][4]=5.08e-21;	Flx[12][5]=7.86e-21;	Flx[12][6]=9.68e-21;	Flx[12][7]=1.06e-20;

  // P. Lipari, August 1992, Table 9 - Ratio mu+/mu-

  	//	Ang[0]			Ang[1]			Ang[2]			Ang[3]			Ang[4]			Ang[5]			Ang[6]			Ang[7]
/*  En[0]  */	Ratio[0][0]=1.28;	Ratio[0][1]=1.28;	Ratio[0][2]=1.29;	Ratio[0][3]=1.29;	Ratio[0][4]=1.29;	Ratio[0][5]=1.29;	Ratio[0][6]=1.29;	Ratio[0][7]=1.30;
/*  En[1]  */	Ratio[1][0]=1.29;	Ratio[1][1]=1.29;	Ratio[1][2]=1.29;	Ratio[1][3]=1.29;	Ratio[1][4]=1.29;	Ratio[1][5]=1.29;	Ratio[1][6]=1.30;	Ratio[1][7]=1.30;
/*  En[2]  */	Ratio[2][0]=1.30;	Ratio[2][1]=1.30;	Ratio[2][2]=1.30;	Ratio[2][3]=1.30;	Ratio[2][4]=1.30;	Ratio[2][5]=1.29;	Ratio[2][6]=1.30;	Ratio[2][7]=1.30;
/*  En[3]  */	Ratio[3][0]=1.31;	Ratio[3][1]=1.31;	Ratio[3][2]=1.30;	Ratio[3][3]=1.30;	Ratio[3][4]=1.30;	Ratio[3][5]=1.30;	Ratio[3][6]=1.30;	Ratio[3][7]=1.30;
/*  En[4]  */	Ratio[4][0]=1.34;	Ratio[4][1]=1.32;	Ratio[4][2]=1.32;	Ratio[4][3]=1.31;	Ratio[4][4]=1.31;	Ratio[4][5]=1.31;	Ratio[4][6]=1.30;	Ratio[4][7]=1.31;
/*  En[5]  */	Ratio[5][0]=1.39;	Ratio[5][1]=1.36;	Ratio[5][2]=1.35;	Ratio[5][3]=1.34;	Ratio[5][4]=1.33;	Ratio[5][5]=1.32;	Ratio[5][6]=1.32;	Ratio[5][7]=1.32;
/*  En[6]  */	Ratio[6][0]=1.45;	Ratio[6][1]=1.42;	Ratio[6][2]=1.40;	Ratio[6][3]=1.39;	Ratio[6][4]=1.37;	Ratio[6][5]=1.35;	Ratio[6][6]=1.34;	Ratio[6][7]=1.34;
/*  En[7]  */	Ratio[7][0]=1.50;	Ratio[7][1]=1.49;	Ratio[7][2]=1.47;	Ratio[7][3]=1.45;	Ratio[7][4]=1.43;	Ratio[7][5]=1.41;	Ratio[7][6]=1.39;	Ratio[7][7]=1.39;
/*  En[8]  */	Ratio[8][0]=1.53;	Ratio[8][1]=1.52;	Ratio[8][2]=1.51;	Ratio[8][3]=1.51;	Ratio[8][4]=1.49;	Ratio[8][5]=1.47;	Ratio[8][6]=1.46;	Ratio[8][7]=1.46;
/*  En[9]  */	Ratio[9][0]=1.53;	Ratio[9][1]=1.53;	Ratio[9][2]=1.53;	Ratio[9][3]=1.53;	Ratio[9][4]=1.52;	Ratio[9][5]=1.51;	Ratio[9][6]=1.51;	Ratio[9][7]=1.51;
/*  En[10] */	Ratio[10][0]=1.54;	Ratio[10][1]=1.54;	Ratio[10][2]=1.54;	Ratio[10][3]=1.54;	Ratio[10][4]=1.53;	Ratio[10][5]=1.53;	Ratio[10][6]=1.53;	Ratio[10][7]=1.53;
/*  En[11] */	Ratio[11][0]=1.54;	Ratio[11][1]=1.54;	Ratio[11][2]=1.54;	Ratio[11][3]=1.54;	Ratio[11][4]=1.54;	Ratio[11][5]=1.54;	Ratio[11][6]=1.54;	Ratio[11][7]=1.54;
/*  En[12] */	Ratio[12][0]=1.54;	Ratio[12][1]=1.54;	Ratio[12][2]=1.54;	Ratio[12][3]=1.54;	Ratio[12][4]=1.54;	Ratio[12][5]=1.54;	Ratio[12][6]=1.54;	Ratio[12][7]=1.54;

  // Creating Z-values for quadratic spline interpolation along angular axis

  G4double customZ[13] = { 1.03e-4, 4.48e-3, 3.88e-3, 7.50e-4, 1.00e-5, 1.00e-6, 0.00e+0, 0.00e+0, 0.00e+0, 0.00e+0, 0.00e+0, 0.00e+0, 0.00e+0 };
  for(int i = 0; i < 13; i++)
  {
//    ZAng[i][0] = (log10(Flx[i][1])-log10(Flx[i][0]))/(Ang[1]-Ang[0]);
    ZAng[i][0] = customZ[i];
    for(int j = 1; j < 8; j++)
    {
      ZAng[i][j] = -ZAng[i][j-1] + 2.0*(log10(Flx[i][j])-log10(Flx[i][j-1]))/(acos(Ang[j])-acos(Ang[j-1]));
    }
  }
/*
  for(int i = 0; i < 13; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      G4cout << ZAng[i][j] << " "; if(j==7) { G4cout << G4endl; }
    }
  }
*/
}

	// ****** BESS Spectrum Data Tables ****** //
void CosmicMuonGenerator::GenerateBESSPDFTable()
{
  // See Precise Measurements of Atmospheric Muon Fluxes with the BESS Spectrometer
  // M. Motoki et al, February 2008, Table 1 - Positive Muon Flux (Tsukuba '95) and Negative Muon Flux (Tsukuba '95)

//	Mu+ Bin Acceptance Cutoff		Mu- Bin Acceptance Cutoff		Bin Range Min			Bin Range Max			Bin Range Avg
	BinAcceptanceCutoff[0][0]=13.86;	BinAcceptanceCutoff[1][0]=12.45;	BinRangeMin[0]=0.576;		BinRangeMax[0]=0.621;		BinRangeAvg[0]=0.598;
	BinAcceptanceCutoff[0][1]=13.75;	BinAcceptanceCutoff[1][1]=12.62;	BinRangeMin[1]=0.621;		BinRangeMax[1]=0.669;		BinRangeAvg[1]=0.645;
	BinAcceptanceCutoff[0][2]=13.10;	BinAcceptanceCutoff[1][2]=12.15;	BinRangeMin[2]=0.669;		BinRangeMax[2]=0.72;		BinRangeAvg[2]=0.695;
	BinAcceptanceCutoff[0][3]=13.32;	BinAcceptanceCutoff[1][3]=11.86;	BinRangeMin[3]=0.72;		BinRangeMax[3]=0.776;		BinRangeAvg[3]=0.748;
	BinAcceptanceCutoff[0][4]=12.87;	BinAcceptanceCutoff[1][4]=11.49;	BinRangeMin[4]=0.776;		BinRangeMax[4]=0.836;		BinRangeAvg[4]=0.806;
	BinAcceptanceCutoff[0][5]=12.36;	BinAcceptanceCutoff[1][5]=11.13;	BinRangeMin[5]=0.836;		BinRangeMax[5]=0.901;		BinRangeAvg[5]=0.868;
	BinAcceptanceCutoff[0][6]=12.23;	BinAcceptanceCutoff[1][6]=10.86;	BinRangeMin[6]=0.901;		BinRangeMax[6]=0.97;		BinRangeAvg[6]=0.936;
	BinAcceptanceCutoff[0][7]=11.92;	BinAcceptanceCutoff[1][7]=10.21;	BinRangeMin[7]=0.97;		BinRangeMax[7]=1.045;		BinRangeAvg[7]=1.008;
	BinAcceptanceCutoff[0][8]=11.37;	BinAcceptanceCutoff[1][8]=10.12;	BinRangeMin[8]=1.045;		BinRangeMax[8]=1.126;		BinRangeAvg[8]=1.086;
	BinAcceptanceCutoff[0][9]=10.91;	BinAcceptanceCutoff[1][9]=9.572;	BinRangeMin[9]=1.126;		BinRangeMax[9]=1.213;		BinRangeAvg[9]=1.17;
	BinAcceptanceCutoff[0][10]=10.55;	BinAcceptanceCutoff[1][10]=8.920;	BinRangeMin[10]=1.213;		BinRangeMax[10]=1.307;		BinRangeAvg[10]=1.26;
	BinAcceptanceCutoff[0][11]=9.551;	BinAcceptanceCutoff[1][11]=8.722;	BinRangeMin[11]=1.307;		BinRangeMax[11]=1.408;		BinRangeAvg[11]=1.357;
	BinAcceptanceCutoff[0][12]=9.390;	BinAcceptanceCutoff[1][12]=8.039;	BinRangeMin[12]=1.408;		BinRangeMax[12]=1.517;		BinRangeAvg[12]=1.463;
	BinAcceptanceCutoff[0][13]=8.989;	BinAcceptanceCutoff[1][13]=7.590;	BinRangeMin[13]=1.517;		BinRangeMax[13]=1.634;		BinRangeAvg[13]=1.575;
	BinAcceptanceCutoff[0][14]=8.613;	BinAcceptanceCutoff[1][14]=7.317;	BinRangeMin[14]=1.634;		BinRangeMax[14]=1.76;		BinRangeAvg[14]=1.697;
	BinAcceptanceCutoff[0][15]=7.962;	BinAcceptanceCutoff[1][15]=6.662;	BinRangeMin[15]=1.76;		BinRangeMax[15]=1.896;		BinRangeAvg[15]=1.828;
	BinAcceptanceCutoff[0][16]=7.519;	BinAcceptanceCutoff[1][16]=6.234;	BinRangeMin[16]=1.896;		BinRangeMax[16]=2.043;		BinRangeAvg[16]=1.969;
	BinAcceptanceCutoff[0][17]=7.094;	BinAcceptanceCutoff[1][17]=5.787;	BinRangeMin[17]=2.043;		BinRangeMax[17]=2.201;		BinRangeAvg[17]=2.121;
	BinAcceptanceCutoff[0][18]=6.543;	BinAcceptanceCutoff[1][18]=5.421;	BinRangeMin[18]=2.201;		BinRangeMax[18]=2.371;		BinRangeAvg[18]=2.285;
	BinAcceptanceCutoff[0][19]=6.000;	BinAcceptanceCutoff[1][19]=4.966;	BinRangeMin[19]=2.371;		BinRangeMax[19]=2.555;		BinRangeAvg[19]=2.462;
	BinAcceptanceCutoff[0][20]=5.596;	BinAcceptanceCutoff[1][20]=4.510;	BinRangeMin[20]=2.555;		BinRangeMax[20]=2.752;		BinRangeAvg[20]=2.653;
	BinAcceptanceCutoff[0][21]=5.139;	BinAcceptanceCutoff[1][21]=4.075;	BinRangeMin[21]=2.752;		BinRangeMax[21]=2.965;		BinRangeAvg[21]=2.857;
	BinAcceptanceCutoff[0][22]=4.622;	BinAcceptanceCutoff[1][22]=3.757;	BinRangeMin[22]=2.965;		BinRangeMax[22]=3.194;		BinRangeAvg[22]=3.078;
	BinAcceptanceCutoff[0][23]=4.212;	BinAcceptanceCutoff[1][23]=3.353;	BinRangeMin[23]=3.194;		BinRangeMax[23]=3.441;		BinRangeAvg[23]=3.315;
	BinAcceptanceCutoff[0][24]=3.742;	BinAcceptanceCutoff[1][24]=3.041;	BinRangeMin[24]=3.441;		BinRangeMax[24]=3.707;		BinRangeAvg[24]=3.573;
	BinAcceptanceCutoff[0][25]=3.417;	BinAcceptanceCutoff[1][25]=2.694;	BinRangeMin[25]=3.707;		BinRangeMax[25]=3.993;		BinRangeAvg[25]=3.847;
	BinAcceptanceCutoff[0][26]=3.089;	BinAcceptanceCutoff[1][26]=2.393;	BinRangeMin[26]=3.993;		BinRangeMax[26]=4.302;		BinRangeAvg[26]=4.145;
	BinAcceptanceCutoff[0][27]=2.719;	BinAcceptanceCutoff[1][27]=2.090;	BinRangeMin[27]=4.302;		BinRangeMax[27]=4.635;		BinRangeAvg[27]=4.465;
	BinAcceptanceCutoff[0][28]=2.419;	BinAcceptanceCutoff[1][28]=1.878;	BinRangeMin[28]=4.635;		BinRangeMax[28]=4.993;		BinRangeAvg[28]=4.809;
	BinAcceptanceCutoff[0][29]=2.060;	BinAcceptanceCutoff[1][29]=1.649;	BinRangeMin[29]=4.993;		BinRangeMax[29]=5.379;		BinRangeAvg[29]=5.182;
	BinAcceptanceCutoff[0][30]=1.873;	BinAcceptanceCutoff[1][30]=1.430;	BinRangeMin[30]=5.379;		BinRangeMax[30]=5.795;		BinRangeAvg[30]=5.583;
	BinAcceptanceCutoff[0][31]=1.628;	BinAcceptanceCutoff[1][31]=1.270;	BinRangeMin[31]=5.795;		BinRangeMax[31]=6.243;		BinRangeAvg[31]=6.016;
	BinAcceptanceCutoff[0][32]=1.448;	BinAcceptanceCutoff[1][32]=1.104;	BinRangeMin[32]=6.243;		BinRangeMax[32]=6.726;		BinRangeAvg[32]=6.478;
	BinAcceptanceCutoff[0][33]=1.248;	BinAcceptanceCutoff[1][33]=0.9449;	BinRangeMin[33]=6.726;		BinRangeMax[33]=7.246;		BinRangeAvg[33]=6.983;
	BinAcceptanceCutoff[0][34]=1.101;	BinAcceptanceCutoff[1][34]=0.8736;	BinRangeMin[34]=7.246;		BinRangeMax[34]=7.806;		BinRangeAvg[34]=7.519;
	BinAcceptanceCutoff[0][35]=0.8934;	BinAcceptanceCutoff[1][35]=0.6926;	BinRangeMin[35]=7.806;		BinRangeMax[35]=8.409;		BinRangeAvg[35]=8.099;
	BinAcceptanceCutoff[0][36]=0.7962;	BinAcceptanceCutoff[1][36]=0.5978;	BinRangeMin[36]=8.409;		BinRangeMax[36]=9.059;		BinRangeAvg[36]=8.728;
	BinAcceptanceCutoff[0][37]=0.6731;	BinAcceptanceCutoff[1][37]=0.5188;	BinRangeMin[37]=9.059;		BinRangeMax[37]=9.76;		BinRangeAvg[37]=9.399;
	BinAcceptanceCutoff[0][38]=0.5634;	BinAcceptanceCutoff[1][38]=0.4598;	BinRangeMin[38]=9.76;		BinRangeMax[38]=10.514;		BinRangeAvg[38]=10.126;
	BinAcceptanceCutoff[0][39]=0.4923;	BinAcceptanceCutoff[1][39]=0.3713;	BinRangeMin[39]=10.514;		BinRangeMax[39]=11.327;		BinRangeAvg[39]=10.907;
	BinAcceptanceCutoff[0][40]=0.3982;	BinAcceptanceCutoff[1][40]=0.3101;	BinRangeMin[40]=11.327;		BinRangeMax[40]=12.203;		BinRangeAvg[40]=11.754;
	BinAcceptanceCutoff[0][41]=0.3521;	BinAcceptanceCutoff[1][41]=0.2625;	BinRangeMin[41]=12.203;		BinRangeMax[41]=13.146;		BinRangeAvg[41]=12.652;
	BinAcceptanceCutoff[0][42]=0.2790;	BinAcceptanceCutoff[1][42]=0.2335;	BinRangeMin[42]=13.146;		BinRangeMax[42]=14.163;		BinRangeAvg[42]=13.649;
	BinAcceptanceCutoff[0][43]=0.2465;	BinAcceptanceCutoff[1][43]=0.1958;	BinRangeMin[43]=14.163;		BinRangeMax[43]=15.258;		BinRangeAvg[43]=14.693;
	BinAcceptanceCutoff[0][44]=0.2016;	BinAcceptanceCutoff[1][44]=0.1599;	BinRangeMin[44]=15.258;		BinRangeMax[44]=16.437;		BinRangeAvg[44]=15.826;
	BinAcceptanceCutoff[0][45]=0.1752;	BinAcceptanceCutoff[1][45]=0.1320;	BinRangeMin[45]=16.437;		BinRangeMax[45]=17.708;		BinRangeAvg[45]=17.054;
	BinAcceptanceCutoff[0][46]=0.1440;	BinAcceptanceCutoff[1][46]=0.1126;	BinRangeMin[46]=17.708;		BinRangeMax[46]=19.077;		BinRangeAvg[46]=18.378;
	BinAcceptanceCutoff[0][47]=0.1227;	BinAcceptanceCutoff[1][47]=0.09271;	BinRangeMin[47]=19.077;		BinRangeMax[47]=20.552;		BinRangeAvg[47]=19.791;

  // Addition of linear interpolation to smooth out the curve

  for(int i = 0; i < 47; i++)
  {
    BinSlope[0][i] = (BinAcceptanceCutoff[0][i+1] - BinAcceptanceCutoff[0][i]) / (BinRangeAvg[i+1] - BinRangeAvg[i]);
    BinSlope[1][i] = (BinAcceptanceCutoff[1][i+1] - BinAcceptanceCutoff[1][i]) / (BinRangeAvg[i+1] - BinRangeAvg[i]);
  }
  BinSlope[0][47] = -BinAcceptanceCutoff[0][47] / (BinRangeMax[47] - BinRangeAvg[47]);
  BinSlope[1][47] = -BinAcceptanceCutoff[1][47] / (BinRangeMax[47] - BinRangeAvg[47]);
}

// -------------------------------------------------------------------------------------------------- //

	// ****** Print Function ****** //
void CosmicMuonGenerator::PrintAllParameters() const		// Displays the last set of parameters given to this object - for use anywhere
{
	// Outputs current parameter list to terminal
  std::stringstream stream;
  if(fPlusOnly) { stream << "Only mu+"; }
  else if(fMinusOnly) { stream << " Only mu-"; }
  else if(ratio <= 0.0) { stream << "Energy-Dependent"; }
  else { stream << ratio; }
  G4cout << "\n"
         << "********* CosmicMuonGenerator initiated with the following default parameters. *********\n"
         << "\t   Mu+ to Mu- Ratio:		" << stream.str() << "\n";
  if(fMonoEnergy)
{ G4cout << "\t   Muon Energy:			" << G4BestUnit(muMono,"Energy") << "\n"; }
  else if(fBESS)
{ G4cout << "\t   Minimum Sample Momentum:	" << G4BestUnit(min_val,"Energy") << "/c" << "\n"
         << "\t   Maximum Sample Momentum:	" << G4BestUnit(max_val,"Energy") << "/c" << "\n"; }
  else
{ G4cout << "\t   Minimum Sample Energy:	" << G4BestUnit(min_val,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_val,"Energy") << "\n"; }
  G4cout << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "****************************************************************************************\n"
         << G4endl;
}
