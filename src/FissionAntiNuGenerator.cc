// Unrestricted Use - Property of AECL
//
// FissionAntiNuGenerator.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Fission Antineutrino Energy Spectrum Generator
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "FissionAntiNuGenerator.hh"		// Specifies the file which contains the class structure

#include "FissionAntiNuMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"

#include "Randomize.hh"				// Specifies the classes which contain structures called upon in this class
#include "G4UnitsTable.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies classes defining all global parameters and variable types
#include <math.h>
#include <map>

	// ****** Constructor ****** //
FissionAntiNuGenerator::FissionAntiNuGenerator(G4bool M, G4int v)
{
  verbose = v;		// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }

	// Define default values
  U235 = 1.06; U238 = 0.057; Pu239 = 0.0; Pu241 = 0.0;

	// Creates messenger class only if not instantiated as part of InverseBetaKinematics module
  if(M) { fiss_messenger = new FissionAntiNuMessenger(this); }
  else  { fiss_messenger = 0; }

	// Outputs default parameter list to terminal only if not instantiated as part of InverseBetaKinematics module
  if(M)
  {
    G4cout << "\n"
           << "******* FissionAntiNuGenerator initiated with the following default parameters *******\n"
           << "\t   Uranium-235 Content:		" << 100*(GetUranium235Content(true)) << "%" << "\n"
           << "\t   Uranium-238 Content:		" << 100*(GetUranium238Content(true)) << "%" << "\n"
           << "\t   Plutonium-239 Content:	" << 100*(GetPlutonium239Content(true)) << "%" << "\n"
           << "\t   Plutonium-241-Content:	" << 100*(GetPlutonium241Content(true)) << "%" << "\n"
           << "**************************************************************************************\n"
           << G4endl;
  }
}

	// ****** Destructor ****** //
FissionAntiNuGenerator::~FissionAntiNuGenerator()
{
  if(fiss_messenger) { delete fiss_messenger; }
}

	// ****** Change Verbosity ****** //
void FissionAntiNuGenerator::SetVerbosity(G4int v)
{
  verbose = v;
  if(verbose > 1) { G4cout << "FissionAntiNuGenerator verbosity set to " << v << "." << G4endl; }
  if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: FissionAntiNuGenerator raw data will now be output. ***" << G4endl; }
  else            { RawData = false; }
}

	// ****** Change Fuel Composition ****** //
void FissionAntiNuGenerator::SetAntiNeutrinoSpectrum(G4double U5, G4double U8, G4double P3, G4double P4)
{
  if(U5 <= 0. && U8 <= 0. && P3 <= 0. && P4 <= 0.) { G4cout << "*** ATTENTION: Invalid composition of fission material for antineutrino spectrum. Previous composition kept. ***" << G4endl; }
  else
  {
    if(U5 >= 0.) { U235 = U5; } else { U235 = 0.; G4cout << "*** WARNING: Uranium-235 content (first entry) of composition is negative. Uranium-235 content set to zero. ***" << G4endl; }
    if(U8 >= 0.) { U238 = U8; } else { U238 = 0.; G4cout << "*** WARNING: Uranium-238 content (second entry) of composition is negative. Uranium-238 content set to zero. ***" << G4endl; }
    if(P3 >= 0.) { Pu239 = P3; } else { Pu239 = 0.; G4cout << "*** WARNING: Plutonium-239 content (third entry) of composition is negative. Plutonium-239 content set to zero. ***" << G4endl; }
    if(P4 >= 0.) { Pu241 = P4; } else { Pu241 = 0.; G4cout << "*** WARNING: Plutonium-241 content (fourth entry) of composition is negative. Plutonium-241 content set to zero. ***" << G4endl; }
    if(verbose > 1)
    {
      G4cout << "Simulation set to use antineutrino spectrum from fissions in the following split \n" 
             << "--- U235: " << U235 << " U238: " << U238 << " Pu239: " << Pu239 << " Pu241: " << Pu241 << " ---" << G4endl;
    }
  }
}

	// The following functions allow the user to retrieve individual fuel composition percentages

G4double FissionAntiNuGenerator::GetUranium235Content(G4bool frac) const
{
  if(frac) { return CalculateFractionComposition(U235); }
  else	   { return U235; }
}

G4double FissionAntiNuGenerator::GetPlutonium239Content(G4bool frac) const
{
  if(frac) { return CalculateFractionComposition(Pu239); }
  else	   { return Pu239; }
}

G4double FissionAntiNuGenerator::GetPlutonium241Content(G4bool frac) const
{
  if(frac) { return CalculateFractionComposition(Pu241); }
  else	   { return Pu241; }
}

G4double FissionAntiNuGenerator::GetUranium238Content(G4bool frac) const
{
  if(frac) { return CalculateFractionComposition(U238); }
  else	   { return U238; }
}

// -------------------------------------------------------------------------------------------------- //

	// ****** Generate Antineutrino Energy ****** //
G4double FissionAntiNuGenerator::GenerateAntiNeutrinoEnergy() const
{
  G4double energy, randNo, Probability;
  do
  {
	// Generate acceptance parameter
    randNo = G4UniformRand();

	// Generate energy
    energy = 8.*G4UniformRand() + 1.5;

	// Calculate acceptance value - NormFactor is the maximum value of distribution to obtain a fraction
    G4double Phi = 0.; G4double NormFactor = 0.0;
    Phi += U235*CalculateU235Spectrum(energy); NormFactor += U235*1.936;
    Phi += Pu239*CalculatePu239Spectrum(energy); NormFactor += Pu239*1.489;
    Phi += Pu241*CalculatePu241Spectrum(energy); NormFactor += Pu241*1.490;
    Phi += U238*CalculateU238Spectrum(energy); NormFactor += U238*1.742;
    Probability = Phi / NormFactor;
  }
  while (randNo > Probability);		// Acceptance condition
  energy = energy*MeV;

	// Writes individual energies into file if requested
  if(RawData)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName("GeneratedAntiNeutrinoEnergies.txt");
    log->OpenFile(false,true);
    (*log) << energy/MeV << std::endl;
    log->CloseFile();
  }

  return energy;
}

	// ****** Spectrum Test Function ****** //
void FissionAntiNuGenerator::GenerateEnergiesWithoutSimulation(G4int n) const
{
	// Generate histogram for particle kinetic energy
  std::map<G4double,G4int>* theTotalHistogram = new std::map<G4double,G4int>();
  G4double n_bins = 80;
  G4double maxE = 9.5*MeV; G4double minE = 1.5*MeV;
  G4double Incrementer = (maxE - minE) / n_bins;
  for(int j = 1; j <= n_bins; j++)
  {
    G4double upperlimit = minE + j*Incrementer;
    (*theTotalHistogram)[upperlimit] = 0;
  }

	// Uses random number generator to sample from spectrum n times
  G4int reps = 1;
  while(reps <= n)
  {
    G4double energy = GenerateAntiNeutrinoEnergy();

	// Immediately sorts sampled energy into histogram for ease of storage and memory efficiency
    std::map<G4double,G4int>::iterator itr = theTotalHistogram->begin();
    while(energy >= itr->first && itr != theTotalHistogram->end()) { itr++; }
    (*theTotalHistogram)[itr->first]++;

	// Print status to show progress
    if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
    reps++;
  }

	// Outputs resulting histograms into text file FissionAntiNuSpectrumHistogram.txt
  LogSession* log = LogSession::GetLogSessionPointer();
  log->SetOutputFileName("FissionAntiNuSpectrumHistogram.txt");
  log->OpenFile();

	// Prints out spectrum parameters first
  (*log) << "\t   Uranium-235 Content:		" << 100*(GetUranium235Content(true)) << "%" << "\n"
         << "\t   Uranium-238 Content:		" << 100*(GetUranium238Content(true)) << "%" << "\n"
         << "\t   Plutonium-239 Content:	" << 100*(GetPlutonium239Content(true)) << "%" << "\n"
         << "\t   Plutonium-241-Content:	" << 100*(GetPlutonium241Content(true)) << "%" << "\n"
         << G4endl;

	// Then prints the antineutrino histogram
  (*log) << "Antineutrino Energies" << std::endl;
  (*log) << "Energy Bin Upper Limit: (MeV)\t\tCount:" << std::endl;
  std::map<G4double,G4int>::iterator ittr = theTotalHistogram->begin();
  for( ; ittr != theTotalHistogram->end(); ittr++)
  {
    (*log) << (ittr->first)/MeV << "\t\t" << ittr->second << std::endl;
  }
  log->CloseFile();
}

	// ****** Calculate Fraction of Total Composition ****** //
G4double FissionAntiNuGenerator::CalculateFractionComposition(G4double value) const
{
  G4double fraction = value / (U235 + U238 + Pu239 + Pu241);
  return fraction;
}

	// ****** Calculate Contribution from Uranium-235 ****** //
G4double FissionAntiNuGenerator::CalculateU235Spectrum(G4double eNu) const
{
  G4double a_1 = 3.519e+0, a_2 = -3.517e+0, a_3 = 1.595e+0, a_4 = -4.171e-1, a_5 = 5.004e-2, a_6 = -2.303e-3;
  G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
  return phi;
}

	// ****** Calculate Contribution from Plutonium-239 ****** //
G4double FissionAntiNuGenerator::CalculatePu239Spectrum(G4double eNu) const
{
  G4double a_1 = 2.560e+0, a_2 = -2.654e+0, a_3 = 1.256e+0, a_4 = -3.617e-1, a_5 = 4.547e-2, a_6 = -2.143e-3;
  G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
  return phi;
}

	// ****** Calculate Contribution from Plutonium-241 ****** //
G4double FissionAntiNuGenerator::CalculatePu241Spectrum(G4double eNu) const
{
  G4double a_1 = 1.487e+0, a_2 = -1.038e+0, a_3 = 4.130e-1, a_4 = -1.423e-1, a_5 = 1.866e-2, a_6 = -9.229e-4;
  G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
  return phi;
}

	// ****** Calculate Contribution from Uranium-238 ****** //
G4double FissionAntiNuGenerator::CalculateU238Spectrum(G4double eNu) const
{
  G4double a_1 = 9.760e-1, a_2 = -1.620e-1, a_3 = -7.900e-2;
  G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2));
  return phi;
}

	// ****** Print Function ****** //
void FissionAntiNuGenerator::PrintAllParameters() const		// Displays the last set of parameters given to this object - for use anywhere
{
  G4cout << "\n"
	 << "******* FissionAntiNuGenerator currently specified at the following parameters *******\n"
         << "\t   Uranium-235 Content:		" << 100*(GetUranium235Content(true)) << "%" << "\n"
         << "\t   Uranium-238 Content:		" << 100*(GetUranium238Content(true)) << "%" << "\n"
         << "\t   Plutonium-239 Content:	" << 100*(GetPlutonium239Content(true)) << "%" << "\n"
         << "\t   Plutonium-241-Content:	" << 100*(GetPlutonium241Content(true)) << "%" << "\n"
         << "**************************************************************************************\n"
         << G4endl;
}

// EOF
