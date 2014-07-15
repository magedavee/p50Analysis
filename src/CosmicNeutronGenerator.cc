// Unrestricted Use - Property of AECL
//
// CosmicNeutronGenerator.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Cosmic Neutron Energy Spectrum Generator
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "CosmicNeutronGenerator.hh"		// Specifies the file which contains the class structure

#include "CosmicNeutronMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "CosmicCosineGenerator.hh"
#include "LogSession.hh"

#include "Randomize.hh"				// Specifies the classes which contain structures called upon in this class
#include "G4UnitsTable.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies classes defining all global parameters and variable types
#include <math.h>
#include <map>

	// ****** Constructor ****** //
CosmicNeutronGenerator::CosmicNeutronGenerator(G4int v, const G4String target)
{
  verbose = v;			// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }

	// Defines hard limits on solar modulation parameter
  s_min = 465.*megavolt;
  s_max = 1700.*megavolt;

	// Defines default values
  fMonoEnergy = false;
  nMono = 1.*MeV;
  s = s_max;
  rc = 10000.*megavolt;
  d = 1030.*g/cm2;
  w = 1;
  min_e = 1.E-10*MeV;
  max_e = 1.E+4*MeV;
  CalculateMaximumFluxValue();

  GenerateCDFTable();		// CDF generation method is flawed... DO NOT USE!

  cos_gen = new CosmicCosineGenerator(verbose,target);
  neutron_messenger = new CosmicNeutronMessenger(this);

	// Conversion factors for atmospheric depth to altitude
  G4double slope = -0.066044/km;
  G4double unlogb = 1030.;
  G4double LogDepthGCm2 = log10(d*cm2/g);
  G4double dd = (LogDepthGCm2 - log10(unlogb)) / slope;

	// Outputs default parameter list to terminal
  G4cout << "\n"
         << "******* CosmicNeutronGenerator initiated with the following default parameters. *******\n"
         << "\t   Solar Modulation:		" << G4BestUnit(s,"Electric potential") << "\n"
         << "\t   Cut-off Rigidity:		" << G4BestUnit(rc,"Electric potential") << "\n"
         << "\t   Atmospheric Depth:		" << G4BestUnit(d,"Mass/Surface") << "\n"
         << "\t   Atmospheric Height in km:	" << std::fabs(dd)/km << " km" << "\n"
         << "\t   Water-equivalent Content:	" << w << "\n"
         << "\t   Minimum Sample Energy:	" << G4BestUnit(min_e,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_e,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "***************************************************************************************\n"
         << G4endl;
}

	// ****** Overloaded Constructor ****** //
CosmicNeutronGenerator::CosmicNeutronGenerator(G4double s_mod, G4double rigid, G4double depth, G4double water, G4int v)		// Specify cosmic parmaeters
{
  verbose = v;			// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }

	// Defines hard limits on solar modulation parameter
  fMonoEnergy = false;
  nMono = 1.*MeV;
  s_min = 465.*megavolt;
  s_max = 1700.*megavolt;

	// Instantiates object with user-defined parameter values - subject to hard lmiits
  if(s_mod < s_min || s_mod > s_max) 
  {
    s_mod = s_max;
    G4cout << "*** ERROR: Invalid solar modulation parameter entered. Must be between 465 and 1700 MV. Defaulted to 1700 MV. ***" << G4endl;
  }
  if(rigid < 0.)
  {
    rigid = 10000.*megavolt;
    G4cout << "*** ERROR: Invalid cut-off rigidity parameter entered. Must be a positive number. Defaulted to 10 GV. ***" << G4endl;
  }
  if(depth < 0.)
  {
    depth = 1030.*g/cm2;
    G4cout << "*** ERROR: Invalid atmospheric depth parameter entered. Must be a positive number. Defaulted to 1030 g/cm2. ***" << G4endl;
  }
  if(water < 0. || water > 1.)
  {
    water = 1.;
    G4cout << "*** ERROR: Invalid water-equivalent content parameter entered. Must be between 0 and 1. Defaulted to 1. ***" << G4endl;
  }

	// Defines the user-defined values for parameters as defaults
  s = s_mod;
  rc = rigid;
  d = depth;
  w = water;
  min_e = 1.E-10*MeV;
  max_e = 1.E+4*MeV;
  CalculateMaximumFluxValue();

  GenerateCDFTable();		// CDF generation method is flawed... DO NOT USE!!!

  cos_gen = new CosmicCosineGenerator(verbose);
  neutron_messenger = new CosmicNeutronMessenger(this);

	// Conversion factors for atmospheric depth to altitude
  G4double slope = -0.066044/km;
  G4double unlogb = 1030.;
  G4double LogDepthGCm2 = log10(d*cm2/g);
  G4double dd = (LogDepthGCm2 - log10(unlogb)) / slope;

	// Outputs user-defined parameter list to terminal
  G4cout << "\n"
         << "*********** CosmicNeutronGenerator initiated with the following parameters. ***********\n"
         << "\t   Solar Modulation:		" << G4BestUnit(s,"Electric potential") << "\n"
         << "\t   Cut-off Rigidity:		" << G4BestUnit(rc,"Electric potential") << "\n"
         << "\t   Atmospheric Depth:		" << G4BestUnit(d,"Mass/Surface") << "\n"
         << "\t   Atmospheric Height in km:	" << std::fabs(dd)/km << " km" << "\n"
         << "\t   Water-equivalent Content:	" << w << "\n"
         << "\t   Minimum Sample Energy:	" << G4BestUnit(min_e,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_e,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "***************************************************************************************\n"
         << G4endl;
}

	// ****** Overloaded Constructor ****** //
CosmicNeutronGenerator::CosmicNeutronGenerator(G4double monoE, G4int v)		// Specify mono-energy
{
  verbose = v;			// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }

	// Defines hard limits on solar modulation parameter
  s_min = 465.*megavolt;
  s_max = 1700.*megavolt;

	// Defines default values
  fMonoEnergy = true;
  nMono = monoE;
  s = s_max;
  rc = 10000.*megavolt;
  d = 1030.*g/cm2;
  w = 1;
  min_e = 1.E-10*MeV;
  max_e = 1.E+4*MeV;
  CalculateMaximumFluxValue();

  GenerateCDFTable();		// CDF generation method is flawed... DO NOT USE!

  cos_gen = new CosmicCosineGenerator(verbose);
  neutron_messenger = new CosmicNeutronMessenger(this);

	// Outputs user-defined parameter list to terminal
  G4cout << "\n"
         << "*********** CosmicNeutronGenerator initiated with the following parameters. ***********\n"
         << "\t   Neutron Mono Energy:		" << G4BestUnit(nMono,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "***************************************************************************************\n"
         << G4endl;
}

	// ****** Destructor ****** //
CosmicNeutronGenerator::~CosmicNeutronGenerator()
{
  delete theCDF;
  delete cos_gen;
  delete neutron_messenger;
}

	// ****** Change Verbosity ******* //
void CosmicNeutronGenerator::SetVerbosity(G4int v)
{
  verbose = v;
  if(verbose > 1) { G4cout << "CosmicMuonGenerator verbosity set to " << v << "." << G4endl; }
  if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: CosmicNeutronGenerator raw data will now be output. ***" << G4endl; }
  else            { RawData = false; }
  cos_gen->SetVerbosity(verbose);
}

	// The following functions allow the user to set the global condition parameters of this analytical spectrum function

	// ****** Change Solar Modulation ******* //
void CosmicNeutronGenerator::SetSolarModulation(G4double s_mod)
{
  if(s_mod < s_min || s_mod > s_max) 
  {
    s_mod = s;
    G4cout << "*** ATTENTION: Invalid solar modulation parameter entered. Must be between 465 and 1700 MV. Previous value kept. ***" << G4endl;
  }
  s = s_mod;
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tSolar modulation parameter currently set to " << G4BestUnit(s,"Electric potential") << "." << G4endl; }
}

	// ****** Change Cutoff Rigidity of Magnetosphere ******* //
void CosmicNeutronGenerator::SetCutoffRigidity(G4double rigid)
{
  if(rigid < 0.)
  {
    rigid = rc;
    G4cout << "*** ATTENTION: Invalid cut-off rigidity parameter entered. Must be a positive number. Previous value kept. ***" << G4endl;
  }
  rc = rigid;
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tCut-off rigidity parameter currently set to " << G4BestUnit(rc,"Electric potential") << "." << G4endl; }
}

	// ****** Change Atmospheric Depth of Detector ******* //
void CosmicNeutronGenerator::SetAtmosphericDepth(G4double depth,G4bool IsKm)
{
  if(depth < 0.)
  {
    depth = d;
    G4cout << "*** ATTENTION: Invalid atmospheric depth parameter entered. Must be a positive number. Previous value kept. ***" << G4endl;
  }
  else if(IsKm)		// Conversion required if enetered in kilometres
  {
    depth = ConvertToGramPerSqCm(depth);
  }
  d = depth;
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tAtmospheric depth parameter currently set to " << G4BestUnit(d,"Mass/Surface") << "." << G4endl; }
}

	// The following function allows the user to set the local condition parameters of this analytical spectrum function

	// ****** Change Water-Equivalent Content of Environment ******* //
void CosmicNeutronGenerator::SetWaterEquivalence(G4double water)
{
  if(water < 0. || water > 1.)
  {
    water = w;
    G4cout << "*** ATTENTION: Invalid water-equivalent content parameter entered. Must be between 0 and 1. Previous value kept. ***" << G4endl;
  }
  w = water;
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tWater-equivalent content parameter set to " << w << "." << G4endl; }
}

	// The following functions allow the user to adjust the sample range of the energy spectrum

	// ****** Change Range Minimum ****** //
void CosmicNeutronGenerator::SetMinEnergy(G4double Emin)
{
  if(Emin < 0.)
  {
    Emin = 0.*MeV;
    G4cout << "*** ATTENTION: Invalid minimum sample energy parameter entered. Must be a positive number. Previous value kept. ***" << G4endl;
  }
  else if(Emin > max_e)
  {
    Emin = max_e;
    G4cout << "*** ATTENTION: Invalid minimum sample energy parameter entered. Must be smaller than the maximum sample energy. Previous value kept. ***" << G4endl;
  }
  min_e = Emin; 
  CalculateMaximumFluxValue();
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tMinimum sample energy parameter set to " << G4BestUnit(min_e,"Energy") << "." << G4endl; }
}

	// ****** Change Range Maximum ****** //
void CosmicNeutronGenerator::SetMaxEnergy(G4double Emax)
{
  if(Emax < min_e)
  {
    Emax = min_e;
    G4cout << "*** ATTENTION: Invalid maximum sample energy parameter entered. Must be greater than the minimum sample energy. Previous value kept. ***" << G4endl;
  }
  else if(Emax < 0.)
  {
    Emax = 0.*MeV;
    G4cout << "*** ATTENTION: Invalid maximum sample energy parameter entered. Must be a positive number. Previous value kept. ***" << G4endl;
  }
  max_e = Emax;
  CalculateMaximumFluxValue();
  GenerateCDFTable();
  if(verbose > 1)
  { G4cout << "\tMaximum sample energy parameter set to " << G4BestUnit(max_e,"Energy") << "." << G4endl; }
}

	// The following function allows the user to specify the neutron mono-energy

	// ****** Set Monoenergetic Neutrons ****** //
void CosmicNeutronGenerator::SetNeutronMonoEnergy(G4double E)
{
  if(E < 0.) { G4cout << "*** ATTENTION: Invalid mono-energy value. Must be a positive number. Previous value kept. ***"; return; }
  nMono = E;
  if(!fMonoEnergy && verbose > 0)
  { G4cout << "*** CAUTION: Cosmic Neutron Module has not been primed to fire mono-energetic neutrons. Parameter will not be used until this option is activated. ***" << G4endl; }
}

	// The following functions allow the user to specify the source and location for the angular distribution generator

	// ****** Set Target Volume ****** //
void CosmicNeutronGenerator::SetTargetVolume(G4String vol)
{
  cos_gen->SetTargetVolume(vol);
}

	// ****** Set Source Radius ****** //
void CosmicNeutronGenerator::SetSourceRadius(G4double r)
{
  cos_gen->SetSourceRadius(r);
}

	// The following functions return the angular distribution generator settings for output

G4String CosmicNeutronGenerator::GetTargetVolume() const
{
  return cos_gen->GetTargetVolume();
}

G4double CosmicNeutronGenerator::GetSourceRadius() const
{
  return cos_gen->GetSourceRadius();
}

// -------------------------------------------------------------------------------------------------- //

	// ****** Calculate Flux in Lethargy ****** //
G4double CosmicNeutronGenerator::CalculateLethargyValue(const G4double energy) const		// Used to obtain the lethargy flux value of a certain energy
{
	// Calls other methods which calculate the terms individually, then assembles them to create the lethargy flux
  G4double phi_L, phi_B, f_G, phi_T, dPhiBydLethargy;
  phi_L = CalculateLowEnergyFluxTerm();
  phi_B = CalculateBasicSpectrumTerm(energy);
  f_G = CalculateGroundEffectTerm(energy);
  phi_T = CalculateThermalSpectrumTerm(energy);
  dPhiBydLethargy = phi_L*(phi_B*f_G + phi_T);

	// The lethargy flux value was coded as a separate function in case a future class is coded which prefers input in lethargy.

  return dPhiBydLethargy;
}

	// ****** Calculate Flux ****** //
G4double CosmicNeutronGenerator::CalculateFluxValue(const G4double energy) const		// Used to obtain the flux of a certain energy
{
	// Calls the lethargy flux value above and converts to a flux
  G4double Lethargy, dPhiBydE;
  Lethargy = CalculateLethargyValue(energy);
  dPhiBydE = Lethargy / (energy/MeV);
  return dPhiBydE;
}

	// ****** Spectrum Test Function ****** //
void CosmicNeutronGenerator::GenerateEnergiesWithoutSimulation(const G4int n) const		// Used to test the shape and statistics of the spectrum generator
{
	// Aborts procedure if mono-energetic neutrons have been activated
  if(fMonoEnergy) { G4cout << "*** WARNING: Mono-energetic neutrons enabled. Spectrum contains only one energy value. Operation aborted. ***" << G4endl; return; }

	// Generates histograms for particle kinetic energies
  std::map<G4double,G4int>* theHistogram = new std::map<G4double,G4int>();
  G4double n_bins = 300;
  if((log10(max_e/MeV) - log10(min_e/MeV)) < 2.)
  {
    G4double incrementer = (max_e - min_e)/n_bins;
    for(int i = 0; i < n_bins; i++)
    {
      G4double lowerlimit = min_e + i*incrementer;
      (*theHistogram)[lowerlimit] = 0;
    }
  }
  else
  {
    G4double incrementer = (log10(max_e/MeV) - log10(min_e/MeV))/n_bins;
    for(int i = 0; i < n_bins; i++)
    {
      G4double lowerlimit = pow(10,(log10(min_e/MeV) + i*incrementer))*MeV;
      (*theHistogram)[lowerlimit] = 0;
    }
  }
 
	// Uses random number generator to sample from spectrum n times
  G4int reps = 1;
  while(reps <= n)
  {
    G4double nEnergy = GenerateNeutronEnergy();						// For slower simulation, but using the analytical function
//    G4double nEnergy = GenerateNeutronEnergyWithCDF();				// For quicker, less accurate simulation

	// Immediately sorts sampled energy into histogram for ease of storage and memory efficiency
    std::map<G4double,G4int>::iterator itr = theHistogram->begin();
    while(nEnergy > itr->first && itr != theHistogram->end()) { itr++; }
    itr--; (*theHistogram)[itr->first]++;

	// Print status to show progress
    if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
    reps++;
  }
  G4double dd = ConvertToKm(d);		// Conversion for output purposes

	// Outputs resulting histograms into text file CosmicNeutronSpectrumHistogram.txt
  LogSession* log = LogSession::GetLogSessionPointer();
  log->SetOutputFileName("CosmicNeutronSpectrumHistogram.txt");
  log->OpenFile();

	// Prints out spectrum parameters first
  (*log) << "\t   Solar Modulation:		" << G4BestUnit(s,"Electric potential") << "\n"
         << "\t   Cut-off Rigidity:		" << G4BestUnit(rc,"Electric potential") << "\n"
         << "\t   Atmospheric Depth:		" << G4BestUnit(d,"Mass/Surface") << "\n"
         << "\t   Atmospheric Height in km:	" << std::fabs(dd)/km << " km" << "\n"
         << "\t   Water-equivalent Content:	" << w << "\n"
         << "\t   Minimum Sample Energy:	" << G4BestUnit(min_e,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_e,"Energy") << "\n"
         << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << std::endl;

	// Then prints the neutron histogram
  (*log) << "Energy Bin Lower Limit: (MeV)\t\tCount:" << std::endl;
  G4int totalCount = 0;
  std::map<G4double,G4int>::iterator ittr = theHistogram->begin();
  for( ; ittr != theHistogram->end(); ittr++)
  {
    totalCount += ittr->second;
    (*log) << (ittr->first)/MeV << "\t\t\t" << ittr->second << std::endl;
  }
  (*log) << "Total Number of Energies Generated: " << totalCount << std::endl;
  log->CloseFile();
}

	// ****** Generate Initial Particle Vector ******  //
std::vector<G4double>* CosmicNeutronGenerator::GenerateSourceLocation() const	// Generation of neutron position and momentum - for use in PrimaryGeneratorAction
{
	// Output as <x,y,z,px,py,pz> - p* indicates momentum in the * direction
  std::vector<G4double>* kinematics = cos_gen->GenerateSourceLocation(true);

	// Writes individual momenta into file if requested
  if(RawData)
  {
    G4ThreeVector pHat((*kinematics)[3],(*kinematics)[4],(*kinematics)[5]);
    G4double angle = std::fabs(pHat.angle(G4ThreeVector(0.,1.,0.)));
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName("GeneratedNeutronMomenta.txt");
    log->OpenFile(false,true);
    (*log) << pHat.x() << "\t" << pHat.y() << "\t" << pHat.z() << "\t" << angle/deg << std::endl;
    log->CloseFile();
  }

  return kinematics;
}

	// ****** Generate Initial Particle Energy ****** //
G4double CosmicNeutronGenerator::GenerateNeutronEnergy() const		// Generation of neutron energy value - for use in PrimaryGeneratorAction
{
	// Output as a kinetic energy
  G4double ySeed, xSeed, Probability;
  if(fMonoEnergy) { xSeed = nMono; }		// Returns the monoenergetic neutron if specified
  else
  {
    do
    {
	// Generate acceptance parameter
      ySeed = max_dist*G4UniformRand();

	// Generate energy
      if(log10(max_e/MeV) - log10(min_e/MeV) < 2.)	// Linear scale
      {
        xSeed = ((max_e/MeV - min_e/MeV)*G4UniformRand() + min_e/MeV)*MeV;
      }
      else						// Logarithmic scale
      {
        xSeed = pow(10,((log10(max_e/MeV) - log10(min_e/MeV))*G4UniformRand() + log10(min_e/MeV)))*MeV;
      }
      Probability = CalculateFluxValue(xSeed);	// Calculate acceptance value
    }
    while (ySeed > Probability);	// Acceptance condition
  }

	// Writes individual energies into file if requested
  if(RawData)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName("GeneratedNeutronEnergies.txt");
    log->OpenFile(false,true);
    (*log) << xSeed/MeV << std::endl;
    log->CloseFile();
  }

  return xSeed;
}

	// ****** Generate Initial Particle Energy CDF ****** //
G4double CosmicNeutronGenerator::GenerateNeutronEnergyWithCDF() const		// Function doesn't work properly yet, but method proved to be at least 10 times faster in generating energies
{
  if(fMonoEnergy) { return nMono; }
  G4double xSeed;
  do
  {
    G4double random = G4UniformRand(); G4int index = -1;
    std::map<G4double,G4double>::iterator threshold = theCDF->find(theCDF->find((G4double)(index - 1))->second); threshold--;
    while(random >= threshold->second && index != (-nCDFSplits)) { random = G4UniformRand(); index--;
                                                                   threshold = theCDF->find(theCDF->find((G4double)(index - 1))->second); threshold--; }
    G4double limitlower = theCDF->find((G4double)(index))->second;
    std::map<G4double,G4double>::iterator limitupper = theCDF->find((G4double)(index - 1));
    G4double cdf_high, cdf_low;
    std::map<G4double,G4double>::iterator itr = theCDF->find(limitlower);
    std::map<G4double,G4double>::iterator itrr = itr; itrr++;
    if(random < itr->second) { cdf_low = 0.; cdf_high = itr->second; itr--; itrr--; }
    else
    {
      for( ; itr != limitupper; itr++)
      {
        itrr = itr; itrr++;
        G4double seamCheck = itr->second;
        if(seamCheck >= itrr->second) { seamCheck = 0.; }
        if(random >= seamCheck && random < itrr->second) { break; }
      }
      cdf_low = itr->second; cdf_high = itrr->second;
    }
    if(itr->first < 0.) { xSeed = (random - cdf_low)*(itrr->first) / (cdf_high - cdf_low); }
    else 		{ xSeed = ((random - cdf_low)*(itrr->first - itr->first) / (cdf_high - cdf_low)) + itr->first; }
  }
  while (xSeed < min_e || xSeed > max_e);
  return xSeed;
}

	// ****** Generate CDF Energy Table ****** //
void CosmicNeutronGenerator::GenerateCDFTable()		// Sets up the table for fast generation of energies
{
  theCDF = new std::map<G4double,G4double>();
  std::map<G4double,G4double>* pdf = new std::map<G4double,G4double>();
  G4double range = 0., sum = 0., cumulative = 0., binWidth = 0.;
  G4int nBins = 0;
  if(log10(max_e/MeV) - log10(min_e/MeV) < 2.)  { range = max_e/MeV - min_e/MeV; binWidth = range/100.; nBins = (G4int)(range/binWidth); }
  else						{ range = log10(max_e/MeV) - log10(min_e/MeV); binWidth = 0.1; nBins = (G4int)(range/binWidth); }
  for(G4int i = 0; i < nBins; i++)
  {
    G4double Ebin;
    if(log10(max_e/MeV) - log10(min_e/MeV) < 2.) { Ebin = min_e + (i*binWidth)*MeV; }
    else					 { Ebin = pow(10,(log10(min_e/MeV) + i*binWidth))*MeV; }
    (*pdf)[Ebin] = CalculateFluxValue(Ebin);
    sum += (*pdf)[Ebin];
  }
  G4double index = -1.;
  std::map<G4double,G4double>::iterator itr = pdf->begin();
  (*theCDF)[index] = itr->first;
  for( ; itr != pdf->end(); itr++)
  {
    (*theCDF)[itr->first] = cumulative;
    if(cumulative >= 0.9999)
    {
      sum = 0.; cumulative = 0.; index = index - 1.;
      std::map<G4double,G4double>::iterator ittr = itr;
      for( ; ittr != pdf->end(); ittr++) { sum += ittr->second; }
      (*theCDF)[index] = itr->first;
    }
    cumulative += itr->second / sum;
  }
  (*theCDF)[max_e] = 1.0;
  nCDFSplits = (G4int)(-index + 1);
}

/* ---------------------------- Conversion Factors for Atmospheric Depth Parameter ------------------------- */

G4double CosmicNeutronGenerator::ConvertToGramPerSqCm(const G4double depthKm) const		// Implements atmospheric depth conversion factor from km to g/cm2 
{
  G4double slope = -0.066044/km;
  G4double unlogb = 1030.;
  G4double LogDepthGCm2 = slope*depthKm + log10(unlogb);
  G4double depthGCm2 = pow(10,LogDepthGCm2)*g/cm2;
  return depthGCm2;
}

G4double CosmicNeutronGenerator::ConvertToKm(const G4double depthGCm2) const			// Reverse conversion factor from above
{
  G4double slope = -0.066044/km;
  G4double unlogb = 1030.;
  G4double LogDepthGCm2 = log10(depthGCm2*cm2/g);
  G4double depthKm = (LogDepthGCm2 - log10(unlogb)) / slope;
  return depthKm;
}

	// ****** Spectral Shape Calculator ****** //
void CosmicNeutronGenerator::CalculateMaximumFluxValue()				// Calculates the maximum of the function in the given range to make process more efficient.
{
	// Note: ONLY works for this function due to its shape!!! Find other methods for other functions.

  G4double MaxFlux = -1./cm2/s;
  if(log10(max_e/MeV) - log10(min_e/MeV) < 2.)		// Uses linear scaling if range is less than two orders of magnitude
  {
    G4int precision = 500;
    G4bool flag = false;
    G4double CurrentEnergy = min_e;
    G4double Separation = ((max_e/MeV - min_e/MeV)/precision)*MeV;

	// Loop to determine maximum flux value
    while(CurrentEnergy < max_e)
    {
      G4double CurrentFlux = CalculateFluxValue(CurrentEnergy);
      if(CurrentFlux > MaxFlux)		{ MaxFlux = CurrentFlux; }
      else if(!flag)			{ flag = true; }		// Forces loop to evaluate two points after maximum as a safety
      else				{ CurrentEnergy = 2*max_e; }
      CurrentEnergy += Separation;
    }
  }
  else							// Uses log scaling if range is greater than two orders of magnitude
  {
    G4int precision = 150;
    G4bool flag = false;
    G4double CurrentEnergy = log10(min_e/MeV);
    G4double Separation = ((log10(max_e/MeV) - log10(min_e/MeV))/precision);
    while(CurrentEnergy < log10(max_e/MeV))
    {
      G4double CurrentFlux = CalculateFluxValue(pow(10,CurrentEnergy*MeV));
      if(CurrentFlux > MaxFlux)		{ MaxFlux = CurrentFlux; }
      else if(!flag)			{ flag = true; }		// Forces loop to evaluate two points after maximum as a safety
      else				{ CurrentEnergy = max_e/MeV; }
      CurrentEnergy += Separation;
    }
  }
  if(MaxFlux < 0.)	// Catch clause in case of unforeseen error
  {
    MaxFlux = 1./cm2/s;
    G4cout << "*** WARNING: Error occured in calculating maximum flux value for sampling. Defaulted to 1.0 cm^-2 s^-1. This may result in a slower total simulation time. ***" << G4endl;
  }
  max_dist = MaxFlux;
}


/* --------- The following functions are implementations of the analytical functions described by Sato and Niita ---------- */

	// ****** Calculates PHI_L ****** //
G4double CosmicNeutronGenerator::CalculateLowEnergyFluxTerm() const
{
  G4double b_1_1_min = 13.9/cm2/s, b_1_1_max = 12.9/cm2/s;
  G4double b_1_2_min = 25.5/cm2/s, b_1_2_max = 15.7/cm2/s;
  G4double b_1_3 = 5620.*megavolt;
  G4double b_1_4 = 1790.*megavolt;

  G4double b_2_1 = 0.00706*cm2/g;
  G4double b_2_2_min = 6.73e-4*cm2/g, b_2_2_max = 5.70e-4*cm2/g;
  G4double b_2_3 = 5990.*megavolt;
  G4double b_2_4 = 1940.*megavolt;

  G4double b_3_1 = 0.975;
  G4double b_3_2_min = -0.292, b_3_2_max = -0.210;		// The minimum value has been changed from 0.292, the value stated in the paper by Sato and Niita
  G4double b_3_3 =  990.*megavolt;
  G4double b_3_4 = 2240.*megavolt;

  G4double b_4_1 = 0.00840*cm2/g;
  G4double b_4_2_min = 0.00582*cm2/g, b_4_2_max = 0.00441*cm2/g;
  G4double b_4_3 = 2240.*megavolt;
  G4double b_4_4 = 2660.*megavolt;

  G4double a_1, a_2, a_3, a_4, phi_L;
  G4double b_1_1, b_1_2, b_2_2, b_3_2, b_4_2;

  b_1_1 = (b_1_1_min*(s_max - s) + b_1_1_max*(s - s_min))/(s_max - s_min);
  b_1_2 = (b_1_2_min*(s_max - s) + b_1_2_max*(s - s_min))/(s_max - s_min);
  b_2_2 = (b_2_2_min*(s_max - s) + b_2_2_max*(s - s_min))/(s_max - s_min);
  b_3_2 = (b_3_2_min*(s_max - s) + b_3_2_max*(s - s_min))/(s_max - s_min);
  b_4_2 = (b_4_2_min*(s_max - s) + b_4_2_max*(s - s_min))/(s_max - s_min);

  a_1 = b_1_1 + (b_1_2 / (1 + exp((rc - b_1_3)/b_1_4)));
  a_2 = b_2_1 + (b_2_2 / (1 + exp((rc - b_2_3)/b_2_4)));
  a_3 = b_3_1 + (b_3_2 / (1 + exp((rc - b_3_3)/b_3_4)));
  a_4 = b_4_1 + (b_4_2 / (1 + exp((rc - b_4_3)/b_4_4)));

  phi_L = a_1*(exp(-a_2*d) - a_3*exp(-a_4*d));
  return phi_L;
}

	// ****** Calculates PHI_B ****** //
G4double CosmicNeutronGenerator::CalculateBasicSpectrumTerm(const G4double E) const
{
  G4double b_5_1 = -0.00701,   b_5_2 = 0.0258,     b_5_3 = 10900.*megavolt, b_5_4 =  2380.*megavolt;		// Lethargy units
  G4double b_9_1 = 642.*MeV,	   b_9_2 = -189.*MeV,      b_9_3 =  2320.*megavolt, b_9_4 =   897.*megavolt;
  G4double b_10_1 = 0.00112*cm2/g, b_10_2 = 1.81e-4*cm2/g, b_10_3 = 8840.*megavolt, b_10_4 =  587.*megavolt;
  G4double b_11_1 = 1.26,	   b_11_2 = -0.958,	   b_11_3 = 3180.*megavolt, b_11_4 = 1470.*megavolt;

  G4double a_6 = 1.71e-4*cm2/g;		// Lethargy units
  G4double a_7 = 0.530;
  G4double a_8 = 0.00136*cm2/g;
  G4double a_12 = 0.0133*cm2/g;

  G4double c_1 = 0.229;		// Lethargy units
  G4double c_2 = 2.31*MeV;
  G4double c_3 = 0.5;		// Sato and Niita paper uses 0.721, but they recommend 0.5 for basic evaporation spectrum
  G4double c_5 = 126.*MeV;
  G4double c_6 = 2.17*MeV;
  G4double c_7 = 0.00108;	// Lethargy units
  G4double c_8 = 3.33e-12*MeV;
  G4double c_9 = 1.62;
  G4double c_10 = 9.59e-8*MeV;
  G4double c_11 = 1.48;

  G4double T1, T2, T3, T4, phi_B;
  G4double c_4, c_12;
  G4double a_5, a_9, a_10, a_11;

  a_5 = b_5_1 + (b_5_2 / (1 + exp((rc - b_5_3)/b_5_4)));
  a_9 = b_9_1 + (b_9_2 / (1 + exp((rc - b_9_3)/b_9_4)));
  a_10 = b_10_1 + (b_10_2 / (1 + exp((rc - b_10_3)/b_10_4)));
  a_11 = b_11_1 + (b_11_2 / (1 + exp((rc - b_11_3)/b_11_4)));

  c_4 = a_5 + (a_6*d / (1 + a_7*exp(a_8*d)));
  c_12 = a_9*(exp(-a_10*d) + a_11*exp(-a_12*d));

  T1 = c_1*pow((E/c_2),c_3)*exp(-E/c_2);
  T2 = c_4*exp(-pow((log10(E) - (log10(c_5))),2) / (2*pow(log10(c_6),2)));
  T3 = c_7*log10(E/c_8)*(1 + tanh(c_9*log10(E/c_10)));
  T4 = 1 - tanh(c_11*log10(E/c_12));

  phi_B = T1 + T2 + T3*T4;
  return phi_B;
}

	// ****** Calculates PHI_G ****** //
G4double CosmicNeutronGenerator::CalculateGroundEffectTerm(const G4double E) const
{
	// Units taken off these parameters, but g_3 is in units of MeV
  G4double h_3_1 = -25.2, h_3_2 = 2.73, h_3_3 = 0.0715;	
  G4double h_5_1 = 0.348*MeV, h_5_2 = 3.35*MeV, h_5_3 = -1.57*MeV;

  G4double g_1 = -0.0235, g_2 = -0.0129, g_4 = 0.969;

  G4double LogF, f_G;
  G4double LogG3, g_3, g_5;

  LogG3 = h_3_1 + (h_3_2 / (w + h_3_3));
  g_3 = pow(10,LogG3)*MeV;
  g_5 = h_5_1 + h_5_2*w + h_5_3*w*w;

  LogF = g_1 + g_2*(log10(E) - log10(g_3))*(1 - tanh(g_4*log10(E/g_5)));

  f_G = pow(10,LogF);
  return f_G;
}

	// ****** Calculate PHI_T ****** //
G4double CosmicNeutronGenerator::CalculateThermalSpectrumTerm(const G4double E) const
{
	// All h_6 in lethargy units
  G4double h_6_1 = 0.118;
  G4double h_6_2 = 0.144;
  G4double h_6_3 = 3.87;
  G4double h_6_4 = 0.653;
  G4double h_6_5 = 42.8;

  G4double E_therm = 2.53e-8*MeV;

  G4double g_6, phi_T;

  g_6 = (h_6_1 + h_6_2*exp(-h_6_3*w)) / (1 + h_6_4*exp(-h_6_5*w));

  phi_T = g_6*pow((E/E_therm),2)*exp(-E/E_therm);
  return phi_T;
}

	// ****** Print Function ****** //
void CosmicNeutronGenerator::PrintAllParameters()		// Displays the last set of parameters given to this object - for use anywhere
{
  G4double dd = ConvertToKm(d);
  G4cout << "\n"
         << "*********** CosmicNeutronGenerator currently has the following parameters. ************\n";
  if(fMonoEnergy)
{ G4cout << "\t   Neutron Mono Energy:		" << G4BestUnit(nMono,"Energy") << "\n"; }
  else
{ G4cout << "\t   Solar Modulation:		" << G4BestUnit(s,"Electric potential") << "\n"
         << "\t   Cut-off Rigidity:		" << G4BestUnit(rc,"Electric potential") << "\n"
         << "\t   Atmospheric Depth:		" << G4BestUnit(d,"Mass/Surface") << "\n"
         << "\t   Atmospheric Height in km:	" << std::fabs(dd)/km << " km" << "\n"
         << "\t   Water-equivalent Content:	" << w << "\n"
         << "\t   Minimum Sample Energy:	" << G4BestUnit(min_e,"Energy") << "\n"
         << "\t   Maximum Sample Energy:	" << G4BestUnit(max_e,"Energy") << "\n"; }
  G4cout << "\t   Target Volume Name:		" << GetTargetVolume() << "\n"
         << "\t   Spherical Source Radius:	" << G4BestUnit(GetSourceRadius(),"Length") << "\n"
         << "***************************************************************************************\n"
         << G4endl;
}

// EOF
