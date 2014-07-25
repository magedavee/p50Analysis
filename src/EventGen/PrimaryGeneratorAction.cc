// Unrestricted Use - Property of AECL
//
// PrimaryGeneratorAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Source and Initial Kinematics Specifications
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "PrimaryGeneratorAction.hh"		// Specifies the file which contains the class structure

#include "PrimaryGeneratorMessenger.hh" 
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include "LogSession.hh"
#include "InputSession.hh"

#include "CRYModule.hh"
#include "IBDModule.hh"
#include "FissionAntiNuModule.hh"
#include "CosmicMuonModule.hh"
#include "CosmicNeutronModule.hh"

#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>

#include <G4ios.hh>

#include "globals.hh"
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>

//////////////////////////////////////

void PrimaryGeneratorModule::throwPrimaries(const std::vector<primaryPtcl>& v, G4Event* anEvent) {
    if(myPGA->GetVerbosity() >= 2) G4cerr << "Throwing " << v.size() << " particles:" << G4endl;
    for(std::vector<primaryPtcl>::const_iterator it = v.begin(); it != v.end(); it++) {
        if(myPGA->GetVerbosity() >= 2) G4cerr << "\tPDG ID " << it->PDGid << "\tKE=" << G4BestUnit(it->KE,"Energy") << " at t=" << G4BestUnit(it->t,"Time") << G4endl;
        
        G4ParticleGun* g = myPGA->GetParticleGun();
        assert(g);
        g->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(it->PDGid));
        g->SetParticleEnergy(it->KE);
        g->SetParticlePosition(it->pos);
        g->SetParticleMomentumDirection(it->mom);
        g->SetParticleTime(it->t);
        g->GeneratePrimaryVertex(anEvent);
    }
}

////////////////////////////////////////

PrimaryGeneratorAction::PrimaryGeneratorAction():
genModule(NULL), myCRYModule(NULL), myIBDModule(NULL),
myCosmicMuonModule(NULL), myCosmicNeutronModule(NULL) {
    
    verbose = 0;
    
    // Particle Gun
    particle_gun = new G4ParticleGun(1);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
    
    // Source-related constants
    numSources = 2;
    calSources = 5;
    
    // Define default values
    min_energy = 0.0*MeV;
    max_energy = 1.0*MeV;
    max_dist = 0.;
    eBin_width = 0.1*MeV;
    mTemp = 273.*kelvin;
    
    Energy = 0;
    Dist = 0;
    Histogram = 0;
    
    fCalibration = 0;
    fCalFunction = 0;
    fDistribution = "No Distribution";
    fDistStore = "";
    customDist = "No Distribution";
    fFunction = 0;
    fCustom = false;
    
    // Initialize global pointers
    detect = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    gun_messenger = new PrimaryGeneratorMessenger(this);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particle_gun;
    delete gun_messenger;
    
    if(myCRYModule) delete myCRYModule;
    if(myIBDModule) delete myIBDModule;
    if(myFisAntNuModule) delete myFisAntNuModule;
    if(myCosmicMuonModule) delete myCosmicMuonModule;
    if(myCosmicNeutronModule) delete myCosmicNeutronModule;
    
    delete Energy;
    delete Dist;
    delete Histogram;
}

void PrimaryGeneratorAction::loadCRYModule() {
    if(!myCRYModule) myCRYModule = new CRYModule(this);
    G4cerr << "Using CRY event generator module; remember to provide setup!" << G4endl; 
    genModule = myCRYModule;
}

void PrimaryGeneratorAction::loadIBDModule() {
    if(!myIBDModule) myIBDModule = new IBDModule(this);
    G4cerr << "Using Inverse Beta Decay event generator." << G4endl; 
    genModule = myIBDModule;
}

void PrimaryGeneratorAction::loadFisAntNuModule() {
    if(!myFisAntNuModule) myFisAntNuModule = new FissionAntiNuModule(this);
    G4cerr << "Using Fission Anti-neutrino event generator." << G4endl; 
    genModule = myFisAntNuModule;
}

void PrimaryGeneratorAction::loadCosmicMuonModule() {
    if(!myCosmicMuonModule) myCosmicMuonModule = new CosmicMuonModule(this);
    G4cerr << "Using cosmic muon event generator." << G4endl; 
    genModule = myCosmicMuonModule;
}

void PrimaryGeneratorAction::loadCosmicNeutronModule() {
    if(!myCosmicNeutronModule) myCosmicNeutronModule = new CosmicNeutronModule(this);
    G4cerr << "Using cosmic neutron event generator." << G4endl; 
    genModule = myCosmicNeutronModule;
}


// ****** Generate User-Specified Particle Kinematics ****** //
void PrimaryGeneratorAction::GenerateUserParticleKinematics(G4int evtNo) {
    // Sets the kinematics of the particle every event according to user-defined distributions
    
    if(fCustom) {       // User-defined spectrum table
      if(!evtNo && fFunction) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified custom distribution is replacing function distribution. ***" << G4endl; }
      } else if(!evtNo) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified custom distribution is replacing module distribution. ***" << G4endl; }
      }
      GenerateCustomParticleEnergy();
  } else if(fFunction) {        // Function-defined spectrum
      if(!evtNo) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified function distribution is replacing module distribution. ***" << G4endl; }
      }
      GenerateFunctionParticleEnergy();
  }
}

void PrimaryGeneratorAction::SetVerbosity(G4int v) {
    verbose = v;
    if(verbose > 1) G4cout << "PrimaryGeneratorAction verbosity set to " << v << "." << G4endl;
}

// ****** Generate Energy from Custom Spectrum Table ****** //
void PrimaryGeneratorAction::GenerateCustomParticleEnergy() {
    // Sets the energy of the particle generated every event according to user-defined distributions
    if(fDistribution != "No Distribution") {
        G4double seedX, seedY;
        G4bool IsAcceptable = false;
        do
        {
            seedX = (log10(Energy->back()) - log10(Energy->front()))*(G4UniformRand()) + log10(Energy->front());
            // Energy value
            seedX = pow(10.0, seedX)*MeV;
            // Acceptance parameter
            seedY = max_dist*(G4UniformRand());
            // Acceptance condition
            IsAcceptable = NumericSpectrumGenerator(seedX, seedY);
        }
        while(!IsAcceptable);
        
        // Set sampled energy as next particle energy
        particle_gun->SetParticleEnergy(seedX);
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(seedX);
    }
    else
    {
        G4cout << "*** WARNING: Spectrum settings are flawed. Continuing without any energy spectrum data. ***" << G4endl;
        fCustom = false;
    }
}

// ****** Generate Energy from Defined Function ****** //
void PrimaryGeneratorAction::GenerateFunctionParticleEnergy() {
    G4double seedX, seedY;
    G4bool IsAcceptable = false;
    G4bool IsLog = false;
    
    // If histogram is not already created, build new histogram
    if(min_energy != 0. && max_energy/min_energy >= 1000.) { IsLog = true; }
    if(Energy->empty())
    {
        for(G4int t = 0; t < (G4int)(max_energy/eBin_width); t++)
        {
            Energy->push_back(min_energy + (t+1)*eBin_width);
        }
    }
    if(Histogram->empty())
    {
        std::vector<G4double>::iterator itr = Energy->begin();
        G4int binNo = 0;
        while(itr != Energy->end())
        { (*Histogram)[binNo] = 0; binNo++; itr++; }
    }
    
    switch(fFunction)	// Switch structure to change functions on-the-fly
  {
        case 1:		// Cf-252 Spontaneous Neutron Source
   {
       particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
       particle_source->SetCurrentSourceto(0);
       particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
       
       do
       {
           // Energy value
           if(IsLog) { seedX = pow(10.0,(log10(max_energy/MeV) - log10(min_energy/MeV))*G4UniformRand() + (min_energy/MeV))*MeV; }
           else      { seedX = (max_energy - min_energy)*G4UniformRand() + min_energy; }
           // Acceptance parameter
           seedY = max_dist*G4UniformRand();
           // Acceptance condition
           IsAcceptable = Cf252Function(seedX, seedY);
       }
       while (!IsAcceptable);
           
           // Set sampled energy as next particle energy
       particle_gun->SetParticleEnergy(seedX);
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(seedX);
       
       // Store sampled energy in histogram by locating correct bin
       G4int index = (G4int)(seedX / eBin_width);
       if(Histogram->find(index) != Histogram->end()) {
           (*Histogram)[index]++;
       } else {
           (*Histogram)[index] = 1;
       }
       break;
   }
        case 2:		// Maxwell Temperature-Dependent Spectrum
   {
       do
       {
           // Energy value
           if(IsLog) { seedX = pow(10.0,(log10(max_energy/eV) - log10(min_energy/eV))*G4UniformRand() + (min_energy/eV))*eV; }
           else      { seedX = (max_energy - min_energy)*G4UniformRand() + min_energy; }
           // Acceptance parameter
           seedY = max_dist*G4UniformRand();
           // Acceptance condition
           IsAcceptable = MaxwellFunction(seedX, seedY);
       }
       while (!IsAcceptable);
           
           // Set sampled energy as next particle energy
       particle_gun->SetParticleEnergy(seedX);
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(seedX);
       
       // Store sampled energy in histogram by locating correct bin
       G4int index = (G4int)(seedX / eBin_width);
       if(Histogram->find(index) != Histogram->end()) {
           (*Histogram)[index]++;
       } else {
           (*Histogram)[index] = 1;
       }
       break;
   }
        default:	// Desired Spectrum Not Coded
   {
       G4cout << "*** WARNING: Spectrum settings are flawed. Continuing without any energy spectrum data. ***" << G4endl;
       fFunction = 0;
       break;
   }
  }
}

// ****** Generate Energy from Calibration Source ****** //
void PrimaryGeneratorAction::GenerateCalibratedSourceEnergy()
{
    switch(fCalFunction)
    {
        case 1:		// Cf-252 Neutron Source
   {
       if(particle_source->GetCurrentSourceIndex() != (numSources+1)) { particle_source->SetCurrentSourceto(numSources); }
       ResetAllSourceIntensities();
       particle_source->SetCurrentSourceIntensity(1.0); 
       
       G4double seedX, seedY;
       G4bool IsAcceptable = false;
       do
       {
           seedX = 8.5*G4UniformRand()*MeV;
           seedY = G4UniformRand();
           IsAcceptable = Cf252Function(seedX, seedY);
       }
       while (!IsAcceptable);
           particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(seedX);
       break;
   }
        case 2:		// Green LED Source
   {
       if(particle_source->GetCurrentSourceIndex() != (numSources+3)) { particle_source->SetCurrentSourceto(numSources+3); }
       ResetAllSourceIntensities();
       particle_source->SetCurrentSourceIntensity(1.0);
       
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(2.1944*eV);
       break;
   }
        default:
        {
            G4cout << "*** WARNING: Calibration spectrum settings are flawed. Continuing with default calibration energy settings. ***" << G4endl;
            fCalFunction = 0;
            break;
        }
    }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    
    if(genModule) {
        genModule->GeneratePrimaries(anEvent);
        return;
    }
    
    // Check for Calibration Mode
    if(fCalibration) {
        if(anEvent->GetEventID() == 0) {
            if(verbose > 0) G4cout << "*** CAUTION: Simulation is in calibration mode. Using calibration sources. ***" << G4endl;
           if(particle_source->GetCurrentSourceIndex() < numSources) {
               G4cout << "*** ATTENTION: Simulation set to use a GPS Source #0 or #1. Resetting GPS to last chosen calibration source. ***" << G4endl;
           }
           SetCalibrationSource(fCalibration);
        }
        if(fCalFunction) { GenerateCalibratedSourceEnergy(); }
    } else if(fCustom || fFunction) {
        GenerateUserParticleKinematics(anEvent->GetEventID());
    }
    
    if(fGun) {
        if(anEvent->GetEventID() == 0 && verbose > 0) G4cout << "*** CAUTION: G4ParticleGun being used instead of the default G4GeneralParticleSource. ***" << G4endl;
        particle_gun->GeneratePrimaryVertex(anEvent);
    } else {
        particle_source->GeneratePrimaryVertex(anEvent);
    }
}





// ****** Calibration Mode Switch ****** //
void PrimaryGeneratorAction::ToggleCalibrationMode(G4bool flipOn) {
    
    if(flipOn) {
        ResetAllSourceIntensities();
        fDistStore = fDistribution;
        
        // Calibration Source - Neutrons
        particle_source->AddaSource(1.0);
        particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
        particle_source->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");		// Momentum in isotropic distribution
        particle_source->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(1.*MeV);
    
        // Calibration Source - Cf252 Neutrons
        particle_source->AddaSource(0.0);
        particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
        particle_source->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");		// Momentum in isotropic distribution
        particle_source->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(10.*MeV);
        
        // Calibration Source - Co60
        G4ParticleDefinition* co60 = G4ParticleTable::GetParticleTable()->GetIon(27,60,0.0);
        particle_source->AddaSource(0.0);
        particle_source->SetParticleDefinition(co60);
        particle_source->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");		// Momentum in isotropic distribution
        particle_source->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.*eV);
        
        // Calibration Source - Cs137
        G4ParticleDefinition* cs137 = G4ParticleTable::GetParticleTable()->GetIon(55,137,0.0);
        particle_source->AddaSource(0.0);
        particle_source->SetParticleDefinition(cs137);
        particle_source->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");		// Momentum in isotropic distribution
        particle_source->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.*eV);
        
        // Calibration Source - Optical Photons  
        particle_source->AddaSource(0.0);
        particle_source->SetParticleDefinition(G4OpticalPhoton::OpticalPhotonDefinition());
        particle_source->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");		// Momentum in isotropic distribution
        particle_source->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
        particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(1.*eV);
        
        SetCalibrationSource(fCalibration);
    }
}

// ****** Change Calibration Source ****** //
void PrimaryGeneratorAction::SetCalibrationSource(G4int source)
{
    if(!fCalibration) { return; }
    G4int previous = fCalibration;
    ResetAllSourceIntensities();
    switch(source)
    {
        case 1:		// Co-60
   {
       fCalFunction = 0;
       particle_source->SetCurrentSourceto(numSources+2);
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.*eV);
       break;
   }
        case 2:		// Cs-137
   {
       fCalFunction = 0;
       particle_source->SetCurrentSourceto(numSources+3);
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.*eV);
       break;
   }
        case 3:		// Cf-252
   {
       fCalFunction = 1;
       particle_source->SetCurrentSourceto(numSources+1);
       fDistribution = "Calibration Cf-252 Neutron Spectrum";
       break;
   }
        case 4:		// Green LED
   {
       fCalFunction = 2;
       particle_source->SetCurrentSourceto(numSources+4);
       fDistribution = "Green LED at 565 nm";
       break; }
        case 999:											// 1 MeV Neutrons
   {
       fCalFunction = 0;
       particle_source->SetCurrentSourceto(numSources);
       particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(1.*MeV);
       break;
   }
        default:
        {
            G4cout << "*** ATTENTION: Invalid calibration source specified. Previous source kept. ***" << G4endl;
            SetCalibrationSource(previous);
            return;
        }
    }
    fCalibration = source;
    particle_source->SetCurrentSourceIntensity(1.0);
}

void PrimaryGeneratorAction::SetCalibrationPosition(G4ThreeVector pos)
{
    if(!fCalibration) { return; }
    if(particle_source->GetCurrentSourceIndex() < numSources)
    { G4cout << "*** WARNING: Particle source is not specified to a calibration source. Source position not registered. ***" << G4endl; }
    else if(particle_source->GetCurrentSourceIndex() == numSources)
    { G4cout << "*** WARNING: Please use the /gps commands when specifying a custom calibration source. Source position not registered. ***" << G4endl; }
    else
    { particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos); }
}

// ****** Reset All Source Intensities ****** //
void PrimaryGeneratorAction::ResetAllSourceIntensities()
{
    G4int source = particle_source->GetCurrentSourceIndex();
    for(int i = 0; i < numSources; i++)
    {
        particle_source->SetCurrentSourceto(i);
        particle_source->SetCurrentSourceIntensity(0.0);
    }
    if(fCalibration)	// Resets calibration sources if they have been activated
  {
      for(int i = numSources; i < (numSources+calSources); i++)
      {
          particle_source->SetCurrentSourceto(i);
          particle_source->SetCurrentSourceIntensity(0.0);
      }
  }
  particle_source->SetCurrentSourceto(source);		// Set current source back to initial source
}

// ****** Specify Spectrum Function ****** //
void PrimaryGeneratorAction::SetFunctionEnergySpectrum(G4int function)		// Called by Messenger class to set a custom energy spectrum
{
    // Warning message for spectrum priority sequence
    if(fDistribution != "No Distribution" && verbose > 0)
    {
        G4cout << "*** CAUTION: Overridding " << fDistribution << ". ***" << G4endl;
    }
    
    // Initialize information holders
    Energy = new std::vector<G4double>();
    Histogram = new std::map<G4int,G4int>();
    
    switch(function)
    {
        case 1:		// Cf-252 Neutron Source
   {
       fFunction = function;
       fDistribution = "Cf-252 Spontaneous Fission Neutron Spectrum";
       max_dist = 1.0;
       eBin_width = 0.1*MeV;
       break;
   }
        case 2:		// Maxwell Temperature-Dependent
   {
       fFunction = function;
       fDistribution = "Temperature-Dependent Maxwellian Spectrum";
       G4double k = 8.617343e-5*eV/kelvin;
       max_dist = exp(-1.0) / (k*mTemp);
       eBin_width = 0.001*eV;
       break;
   }
        default:		// Unspecified
   {
       fFunction = 0;
       fDistribution = "No Distribution"; fFunction = false;
       delete Energy; Energy = 0;
       delete Histogram; Histogram = 0;
       G4cout << "*** WARNING: Specified spectrum definition has not been programmed within PrimaryGeneratorAction.cc. Function spectrum not applied. ***" << G4endl;
       break;
   }
    }
    G4cout << "Currently using: " << fDistribution << G4endl;	// User notification
}

// ****** Specify Custom Spectrum ****** //
void PrimaryGeneratorAction::SetCustomEnergySpectrum(G4String custom)		// Called by Messenger class to set a custom energy spectrum
{
    // Warning message for spectrum priority sequence
    if(fDistribution != "No Distribution" && verbose > 0)
    {
        G4cout << "*** CAUTION: Overridding " << fDistribution << ". ***" << G4endl;
    }
    
    // Initialize information holders
    Energy = new std::vector<G4double>();
    Dist = new std::vector<G4double>();
    Histogram = new std::map<G4int,G4int>();
    
    // Uses information in input file to create spectrum - requires FULL PATH!!!
    if(!custom.empty())
    {
        G4String directory = custom;
        InputSession* input = InputSession::GetInputSessionPointer();
        
        // Assembles probability vector of distribution
        if(directory[directory.length() - 1] != '/') { directory.append("/"); }
        input->SetInputFileName(directory+"SourceDist.txt");	// Must be named SourceDist.txt
        input->OpenFile();
        
        // First line should contain distribution name
        std::getline(*input, customDist);
        fDistribution = customDist;
        
        // Lines after should contain probabilities - one entry per line
        G4double value;
        while(!(input->eof()))
        {
            (*input) >> value;
            if(value > max_dist) { max_dist = value; }
            Dist->push_back(value);
        }
        input->CloseFile();
        
        // Assembles energy vector of distribution
        input->SetInputFileName(directory+"SourceEnergy.txt");	// Must be named SourceEnergy.txt
        input->OpenFile();
        
        // First line should contain distribution name - must match that found in SourceDist.txt
        G4String MatchTest;
        std::getline(*input, MatchTest);
        
        // If there is a correct match, lines after should contain energies in MeV - one entry per line
        if(MatchTest == customDist)
        {
            while(!(input->eof()))
            {
                (*input) >> value;
                value = value*MeV;
                Energy->push_back(value);
            }
        }
        else
        {
            // If match fails, nothing is registered
            G4cout << "*** ATTENTION: Particle Source Spectrum files do not match. Spectrum not registered. ***" << G4endl;
        }
        input->CloseFile();
        
        // Ensures both energy and probability vector are of equal size by adjusting the probability vector
        while(Energy->size() < Dist->size()) { Dist->pop_back(); }
        while(Energy->size() > Dist->size()) { Dist->push_back(0.); }
    }
    
    // Produces histogram for recording sampled distribution
    if(!Energy->empty())
    {
        for(unsigned int i = 0; i < Energy->size(); i++)
        {
            (*Histogram)[i] = 0;
        }
        max_energy = Energy->back();
        fCustom = true;
    }
    else
    {
        // If energy vector turns out to be empty, everything is reverted back to its original state
        fDistribution = "No Distribution";
        customDist = "No Distribution";
        delete Energy; Energy = 0;
        delete Dist; Dist = 0;
        delete Histogram; Histogram = 0;
        G4cout << "*** ATTENTION: Spectrum Definition files are insufficient. Please ensure the files contain the correct information. ***" << G4endl;
    }
}

// ****** Change Energy Range Minimum ****** //
void PrimaryGeneratorAction::SetMinimumEnergyRange(G4double minE)
{
    if(fFunction) { min_energy = minE; }
    else { G4cout << "*** ATTENTION: Function-based spectrum not activated. Specified energy limit not set. ***" << G4endl; }
}

// ****** Change Energy Range Maximum ****** //
void PrimaryGeneratorAction::SetMaximumEnergyRange(G4double maxE)
{
    if(fFunction) { max_energy = maxE; }
    else { G4cout << "*** ATTENTION: Function-based spectrum not activated. Specified energy limit not set. ***" << G4endl; }
}

// ****** Change Temperature of Maxwell Distribution ****** //
void PrimaryGeneratorAction::SetMaxwellianTemperature(G4double T)
{
    mTemp = T;
    if(fDistribution != "Temperature-Dependent Maxwellian Spectrum" && verbose > 0)
    { G4cout << "*** CAUTION: Maxwellian spectrum function not specified yet. Temperature setting will have no effect until spectrum is set. ***" << G4endl; }
}

// ****** Destroy Custom or Function Energy Spectrum ****** //
void PrimaryGeneratorAction::InactivateUserEnergySpectrum(G4bool) {
    delete Energy; Energy = NULL;
    delete Dist; Dist = NULL;
    delete Histogram; Histogram = NULL;
    fDistribution = "No Distribution";
    fFunction = 0;
    customDist = "No Distribution";
    fCustom = false;
}


// The following functions are for coding energy spectrum sampling and distributions

// ****** Custom Energy Spectrum Sampler ****** //
G4bool PrimaryGeneratorAction::NumericSpectrumGenerator(G4double xSeed, G4double ySeed)
{
    // Only accepts energy values within range of energy vector
    G4bool accept = false;
    if(xSeed < Energy->front() || xSeed > Energy->back())
    {
        return accept;
    }
    
    // Finds the correspinding bin for the accepted energy value
    G4int index = 0;
    G4double bin_max = 0.*MeV;
    do
    {
        index++;
        bin_max = (*Energy)[index];
    }
    while(xSeed > bin_max);
                                                     
                                                     // Calculates acceptance parameter from vector using linear interpolation
    G4double bin_pos = xSeed - bin_max;
    G4double slope = ((*Dist)[index] - (*Dist)[index-1])/((*Energy)[index] - (*Energy)[index-1]);
    G4double limit = (*Dist)[index] + slope*bin_pos;
    
    // Acceptance condition
    if(ySeed < limit)
    {
        (*Histogram)[index-1]++;
        accept = true;
    }
    return accept;
}

/* -------------------------------------------------------------------------------------------------------------------------------------
 *   If an analytic function is to be inserted into the simulation, simply add a method below this that defines the equation of the
 *   desired function. Be sure to give the function a descriptive name and to insert the associated call function into the .hh file and
 *   modify the GeneratePrimaries function above. The Cf-252 Neutron Spectrum source is provided as an example of how to do this.
 *   Alternatively, you may modify the contents of the provided function but it would be harder to keep track of everything you've done.
 *   ------------------------------------------------------------------------------------------------------------------------------------- */

G4bool PrimaryGeneratorAction::Cf252Function(G4double xSeed, G4double ySeed)	// Cf-252 Spontaneous Fission Neutron Energy Spectrum
{
    // Calculate acceptance parameter using function
    G4double T = 1.3;
    G4double dN = (pow((xSeed/MeV),0.5))*(exp(-1.*((xSeed/MeV)/T)));
    
    // Acceptance condition
    G4bool accept = false;
    if(ySeed <= dN) { accept = true; }
    return accept;
}

G4bool PrimaryGeneratorAction::MaxwellFunction(G4double xSeed, G4double ySeed)	// Temperature-Dependent Maxwellian Spectrum
{
    // Calculate acceptance parameter using function
    G4double k = 8.617343e-5*eV/kelvin;
    G4double Prob = (xSeed / pow((k*mTemp),2))*exp(-xSeed / (k*mTemp));
    
    // Accpetance condition
    G4bool accept = false;
    if (ySeed <= Prob) { accept = true; }
    return accept;
}

//----------------------------------------------------------------------------//




