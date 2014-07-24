// Unrestricted Use - Property of AECL
//
// PrimaryGeneratorAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Source and Initial Kinematics Specifications
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
//  Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "PrimaryGeneratorAction.hh"		// Specifies the file which contains the class structure

#include "PrimaryGeneratorMessenger.hh"		// Specifies user-defined classes which are called upon in this class 
#include "DetectorConstruction.hh"
#include "LogSession.hh"
#include "InputSession.hh"
#include "CRYModule.hh"
#include "CosmicNeutronGenerator.hh"
#include "InverseBetaKinematics.hh"
#include "FissionAntiNuGenerator.hh"
#include "CosmicMuonGenerator.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include <G4UnitsTable.hh>
#include "Randomize.hh"
#include <G4RunManager.hh>
#include <G4UImanager.hh>

#include "G4DistributionGenerator.hh"		// Keep this around for future use, it may be handy for more complicated distributions

#include "G4ios.hh"				// Specifies classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies classes defining all global parameters and variable types
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;
// ****** Constructor ****** //
PrimaryGeneratorAction::PrimaryGeneratorAction() {
    
    verbose = 0;
    genModule = NULL;
    RawData = false;
    
    // Particle Gun
    G4int n_particle = 1;
    particle_gun = new G4ParticleGun(n_particle);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
    
    // Particle Source
    // Geantino (Default) Source
    particle_source = new G4GeneralParticleSource();
    particle_source->SetNumberOfParticles(n_particle);
    particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
    
    // Positron Source for Inverse Beta Module
    particle_source->AddaSource(0.0);
    particle_source->SetParticleDefinition(G4Positron::PositronDefinition());
    
    // Set source to default
    particle_source->SetCurrentSourceto(0);
    
    // Source-related constants
    numSources = 2;
    calSources = 5;
    
    // Define default values
    min_energy = 0.0*MeV;
    max_energy = 1.0*MeV;
    max_dist = 0.;
    eBin_width = 0.1*MeV;
    mTemp = 273.*kelvin;
    sModes = "";
    
    Energy = 0;
    Dist = 0;
    Histogram = 0;
    
    fCalibration = 0;
    fCalFunction = 0;
    fDistribution = "No Distribution";
    fDistStore = "";
    customDist = "No Distribution";
    fModule = None;
    fFunction = 0;
    fCustom = false;
    fFile = false;
    fGun = false;
    
    // Initialize global pointers
    detect = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    gun_messenger = new PrimaryGeneratorMessenger(this);
    neutron_generator = 0;
    inverse_beta = 0;
    fission_spec = 0;
    muon_generator = 0;
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particle_gun;
    delete particle_source;
    delete gun_messenger;
    
    if(myCRYModule) delete myCRYModule;
    if(neutron_generator) delete neutron_generator;
    if(inverse_beta) delete inverse_beta;
    if(fission_spec) delete fission_spec;
    if(muon_generator) delete muon_generator;
    
    delete Energy;
    delete Dist;
    delete Histogram;
}

void PrimaryGeneratorAction::loadCRYModule() {
    if(!myCRYModule) myCRYModule = new CRYModule(this);
    G4cerr << "Using CRY event generator module; remember to provide setup!" << G4endl; 
    genModule = myCRYModule;
}

// ****** Generate User-Specified Particle Kinematics ****** //
void PrimaryGeneratorAction::GenerateUserParticleKinematics(G4int evtNo) {
    // Sets the kinematics of the particle every event according to user-defined distributions
    
    if(fCustom) {       // User-defined spectrum table
      if(!evtNo && fFunction) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified custom distribution is replacing function distribution. ***" << G4endl; }
      } else if(!evtNo && fModule != None) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified custom distribution is replacing module distribution. ***" << G4endl; }
      }
      GenerateCustomParticleEnergy();
  } else if(fFunction) {        // Function-defined spectrum
      if(!evtNo && fModule != None) {
          if(verbose > 0) { G4cout << "*** CAUTION: Specified function distribution is replacing module distribution. ***" << G4endl; }
      }
      GenerateFunctionParticleEnergy();
  } else if(fFile) {
      GenerateFileParticleEnergy();
  }
}

void PrimaryGeneratorAction::SetVerbosity(G4int v) {
    verbose = v;
    if(verbose > 1) { G4cout << "PrimaryGeneratorAction verbosity set to " << v << "." << G4endl; }
    if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: PrimaryGeneratorAction raw data will now be output. ***" << G4endl; }
    else            { RawData = false; }
    if(neutron_generator) { neutron_generator->SetVerbosity(verbose); }
    if(inverse_beta) { inverse_beta->SetVerbosity(verbose); }
    if(fission_spec) { fission_spec->SetVerbosity(verbose); }
    if(muon_generator) { muon_generator->SetVerbosity(verbose); }
}

// ****** Generate Module-Specified Particle Kinematics ****** //
void PrimaryGeneratorAction::GenerateModuleParticleKinematics(G4int evtNo) {
    // Sets the kinematics of the particle every event according to module-defined distributions
    
    switch(fModule) {
        case InverseBeta: {
            if(evtNo == 0) {
                particle_source->SetCurrentSourceto(0);
                if(!(particle_source->GetParticleDefinition() == G4Neutron::NeutronDefinition())) {
                    particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
                    G4cout << "*** WARNING: GPS Source #0 was not specified to fire neutrons. The correct particle definition has now been assigned to Source #0. ***" << G4endl;
                }
                particle_source->SetCurrentSourceto(1);
                if(!(particle_source->GetParticleDefinition() == G4Positron::PositronDefinition())) {
                    particle_source->SetParticleDefinition(G4Positron::PositronDefinition());
                    G4cout << "*** WARNING: GPS Source #1 was not specified to fire positrons. The correct particle definition has now been assigned to Source #1. ***" << G4endl;
                }
                if(!(particle_gun->GetParticleDefinition() == G4Neutron::NeutronDefinition())) {
                    particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
                    if(fGun) { G4cout << "*** WARNING: G4ParticleGun was not specified to fire neutrons. The correct particle definition has now been assigned to G4ParticleGun. ***" << G4endl; }
                }
            }
            
            G4ThreeVector position = inverse_beta->GenerateReactionPosition();
            std::vector<G4double>* kinematics = inverse_beta->GenerateReactionKinematics();
            G4ThreeVector positronMomentum((*kinematics)[1],(*kinematics)[2],(*kinematics)[3]);
            G4ThreeVector neutronMomentum((*kinematics)[5],(*kinematics)[6],(*kinematics)[7]);
            
            // Specify positron kinematics
            particle_source->SetCurrentSourceto(1);
            particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);
            particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(positronMomentum);
            particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy((*kinematics)[0]);
            
            // Specify neutron kinematics
            particle_source->SetCurrentSourceto(0);
            particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);
            particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(neutronMomentum);
            particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy((*kinematics)[4]);
            
            // Particle Gun will fire neutrons with correct kinematics - NOT positron
            particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
            particle_gun->SetParticlePosition(position);
            particle_gun->SetParticleMomentumDirection(neutronMomentum);
            particle_gun->SetParticleEnergy((*kinematics)[4]);
            
            break;
        }
        case FissionAntinu: {
            if(evtNo == 0) {
                particle_source->SetCurrentSourceto(0);
                if(!(particle_source->GetParticleDefinition() == G4AntiNeutrinoE::AntiNeutrinoEDefinition()))
                {
                    particle_source->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
                    if(!fGun) { G4cout << "*** WARNING: GPS Source #0 was not specified to fire antineutrinos. The correct particle definition has now been assigned to Source #0. ***" << G4endl; }
                }
                if(!(particle_gun->GetParticleDefinition() == G4AntiNeutrinoE::AntiNeutrinoEDefinition()))
                {
                    particle_gun->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
                    if(fGun) { G4cout << "*** WARNING: G4ParticleGun was not specified to fire antineutrinos. The correct particle definition has now been assigned to G4ParticleGun. ***" << G4endl; }
                }
            }
            
            G4double energy = fission_spec->GenerateAntiNeutrinoEnergy();
            
            G4double positionZ = -(detect->GetWorldSizeZ());
            G4double positionX = 2.0*(detect->GetWorldSizeX())*(G4UniformRand()-0.5);
            G4double positionY = 2.0*(detect->GetWorldSizeY())*(G4UniformRand()-0.5);
            
            particle_gun->SetParticlePosition(G4ThreeVector(positionX,positionY,positionZ));		// Gun placement at random places on xy-plane
            particle_gun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));			// Momentum along positive z-axis
            particle_gun->SetParticleEnergy(energy);
            
            particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(positionX,positionY,positionZ));	// Source placement at random places on xy-plane
            particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));		// Momentum along positive z-axis
            particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(energy);
            
            break;
        }
        case CosmicNeutron: {
            if(evtNo == 0) {
                particle_source->SetCurrentSourceto(0);
                if(!(particle_source->GetParticleDefinition() == G4Neutron::NeutronDefinition()))
                {
                    particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
                    if(!fGun) { G4cout << "*** WARNING: GPS Source #0 was not specified to fire neutrons. The correct particle definition has now been assigned to Source #0. ***" << G4endl; }
                }
                if(!(particle_gun->GetParticleDefinition() == G4Neutron::NeutronDefinition()))
                {
                    particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
                    if(fGun) { G4cout << "*** WARNING: G4ParticleGun was not specified to fire neutrons. The correct particle definition has now been assigned to G4ParticleGun. ***" << G4endl; }
                }
            }
            
            std::vector<G4double>* location = neutron_generator->GenerateSourceLocation();
            G4ThreeVector nCosmicPosition((*location)[0],(*location)[1],(*location)[2]);
            G4ThreeVector nCosmicMomentum((*location)[3],(*location)[4],(*location)[5]);
            G4double theEnergy = neutron_generator->GenerateNeutronEnergy();
            //    G4double theEnergy = neutron_generator->GenerateNeutronEnergyWithCDF();		// Quicker generation, but slightly less accurate
            
            particle_gun->SetParticlePosition(nCosmicPosition);
            particle_gun->SetParticleMomentumDirection(nCosmicMomentum);
            particle_gun->SetParticleEnergy(theEnergy);
            
            particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(nCosmicPosition);
            particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(nCosmicMomentum);
            particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(theEnergy);
            
            break;
        }
        case CosmicMuon: {
            std::vector<G4double>* location = muon_generator->GenerateSourceLocation();
            G4ThreeVector muCosmicPosition((*location)[0],(*location)[1],(*location)[2]);
            G4ThreeVector muCosmicMomentum((*location)[3],(*location)[4],(*location)[5]);
            G4double theEnergy = muon_generator->GenerateMuonEnergy();
            G4int type = muon_generator->GenerateMuonType(theEnergy);
            
            if(type) {
                // Change to mu- source
                particle_gun->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
                particle_source->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
            } else {
                // Change to mu+ source
                particle_gun->SetParticleDefinition(G4MuonPlus::MuonPlusDefinition());
                particle_source->SetParticleDefinition(G4MuonPlus::MuonPlusDefinition());
            }
            
            particle_gun->SetParticlePosition(muCosmicPosition);
            particle_gun->SetParticleMomentumDirection(muCosmicMomentum);
            particle_gun->SetParticleEnergy(theEnergy);
            
            particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(muCosmicPosition);
            particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(muCosmicMomentum);
            particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(theEnergy);
            
            break;
        }
        default: break;
    }
}

// ****** Display Current Particle Kinematics ****** //
void PrimaryGeneratorAction::DisplayKinematicsInfo() const {
    G4cout << "*** WARNING: No model selected. Shooting isotropically with 10 MeV geantinos uniformly over inner tank. ***" << G4endl;
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

void PrimaryGeneratorAction::GenerateFileParticleEnergy()
{
    G4int pdg=0;
    G4ThreeVector pos; 
    G4ThreeVector mom;
    G4double E=0, T=0;
    char *line = new char[200];
    G4cerr << "generating a file event" << G4endl;
    if(textfile->getline(line,200)){
        //>>pdg>>E>>pos[0]>>pos[1]>>pos[2]>>mom[0]>>mom[1]>>mom[2]>>T;
        pdg = atoi(strtok(line," \t"));
        E = atof(strtok(0," \t"))*MeV;
        pos[0] = atof(strtok(0," \t"))*cm;
        pos[1] = atof(strtok(0," \t"))*cm;
        pos[2] = atof(strtok(0," \t"))*cm;
        mom[0] = atof(strtok(0," \t"));
        mom[1] = atof(strtok(0," \t"));
        mom[2] = atof(strtok(0," \t"));
        T = atof(strtok(0," \t"))*s;
        line = new char[200];
        G4ParticleTable* partab = G4ParticleTable::GetParticleTable();
        // particle_gun->SetParticleDefinition(partab->FindParticle(pdg));
        particle_source->SetCurrentSourceto(0);
        particle_source->SetParticleDefinition(partab->FindParticle(pdg));
    }
    else{
        pdg = 0;
        E = 0.1*eV;
        for(int i=0;i<3;i++){
            pos[i]=0;
            mom[i]=0.5;
        }
        T=0;
        particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
        particle_source->SetCurrentSourceto(0);
        particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
    }
    particle_gun->SetParticleEnergy(E);
    particle_source->GetCurrentSource()->GetEneDist()->SetMonoEnergy(E);
    
    particle_gun->SetParticlePosition(pos);
    particle_source->GetCurrentSource()->SetParticlePosition(pos);
    particle_source->SetParticlePosition(pos);
    particle_source->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    
    particle_gun->SetParticleMomentumDirection(mom);
    particle_source->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(mom);
    
    particle_gun->SetParticleTime(T);
    particle_source->GetCurrentSource()->SetParticleTime(T);
    
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
       std::map<G4int,G4int>::iterator itr = Histogram->begin();
       if(Histogram->find(index) != Histogram->end())
       {
           (*Histogram)[index]++;
       }
       else
       {
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
       std::map<G4int,G4int>::iterator itr = Histogram->begin();
       if(Histogram->find(index) != Histogram->end())
       {
           (*Histogram)[index]++;
       }
       else
       {
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


void PrimaryGeneratorAction::throwPrimaries(const std::vector<primaryPtcl>& v, G4Event* anEvent) {
    if(verbose >= 2) G4cerr << "Throwing " << v.size() << " particles:" << G4endl;
    for(std::vector<primaryPtcl>::const_iterator it = v.begin(); it != v.end(); it++) {
        if(verbose >= 2) G4cerr << "\tPDG ID " << it->PDGid << "\tKE=" << G4BestUnit(it->KE,"Energy") << " at t=" << G4BestUnit(it->t,"Time") << G4endl;
        
        assert(particle_gun);
        particle_gun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(it->PDGid));
        particle_gun->SetParticleEnergy(it->KE);
        particle_gun->SetParticlePosition(it->pos);
        particle_gun->SetParticleMomentumDirection(it->mom);
        particle_gun->SetParticleTime(it->t);
        particle_gun->GeneratePrimaryVertex(anEvent);
        
        // record primary to ROOT output
        EventPrimaryPtcl p;
        for(uint i=0; i<3; i++) {
            p.x[i] = it->pos[i];
            p.p[i] = it->mom[i];
        }
        p.t = it->t;
        p.E = it->KE;
        p.PID = it->PDGid;
        RootIO::GetInstance()->GetEvent().AddPrimary(p);
    }
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    
    if(genModule) {
        throwPrimaries(genModule->gen(), anEvent);
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
    } else if(fCustom || fFunction || fFile) {
        GenerateUserParticleKinematics(anEvent->GetEventID());
    } else if(fModule != None) {
        GenerateModuleParticleKinematics(anEvent->GetEventID());
    }
    
    if(fGun) {
        if(anEvent->GetEventID() == 0 && verbose > 0) G4cout << "*** CAUTION: G4ParticleGun being used instead of the default G4GeneralParticleSource. ***" << G4endl;
           
           // Writes individual momentums and energies into file if requested
        if(RawData) {
            LogSession* log = LogSession::GetLogSessionPointer();
            
            G4ThreeVector pHat = particle_gun->GetParticleMomentumDirection();
            log->SetOutputFileName("GeneratedSourceMomentum.txt");
            log->OpenFile(false,true);
            (*log) << pHat.x() << "\t" << pHat.y() << "\t" << pHat.z() << std::endl;
            log->CloseFile();
            
            G4double energy = particle_gun->GetParticleEnergy();
            log->SetOutputFileName("GeneratedSourceEnergies.txt");
            log->OpenFile(false,true);
            (*log) << energy/MeV << std::endl;
            log->CloseFile();
        }
        
        particle_gun->GeneratePrimaryVertex(anEvent);
    } else if(fModule == InverseBeta) {
        // If using Inverse Beta Module, ensure one neutron and one positron each are generated
        if(inverse_beta->IsPositronGenerated()) {
            ResetAllSourceIntensities();
            particle_source->SetCurrentSourceto(1);
            particle_source->SetCurrentSourceIntensity(1.0);
            particle_source->GeneratePrimaryVertex(anEvent);
        }
        if(inverse_beta->IsNeutronGenerated()) {
            ResetAllSourceIntensities();
            particle_source->SetCurrentSourceto(0);
            particle_source->SetCurrentSourceIntensity(1.0);
            particle_source->GeneratePrimaryVertex(anEvent);
        }
        particle_source->SetCurrentSourceto(0);
    } else if(fModule != None) {        
        particle_source->GeneratePrimaryVertex(anEvent);
    } else {
        // Writes individual momentums and energies into file if requested
        if(RawData) {
            LogSession* log = LogSession::GetLogSessionPointer();
            
            G4ThreeVector pHat = particle_source->GetParticleMomentumDirection();
            log->SetOutputFileName("GeneratedSourceMomentum.txt");
            log->OpenFile(false,true);
            (*log) << pHat.x() << "\t" << pHat.y() << "\t" << pHat.z() << std::endl;
            log->CloseFile();
            
            G4double energy = particle_source->GetParticleEnergy();
            log->SetOutputFileName("GeneratedSourceEnergies.txt");
            log->OpenFile(false,true);
            (*log) << energy/MeV << std::endl;
            log->CloseFile();
        }
        particle_source->GeneratePrimaryVertex(anEvent);
    }
    
        /*
        Event* ev = RootIO::GetInstance()->GetEvent();
        
        ev->fGenPos[0] = anEvent->GetPrimaryVertex()->GetPosition()[0]/cm;
        ev->fGenPos[1] = anEvent->GetPrimaryVertex()->GetPosition()[1]/cm;
        ev->fGenPos[2] = anEvent->GetPrimaryVertex()->GetPosition()[2]/cm;
        
        ev->fGenMom[0] = anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum()[0];
        ev->fGenMom[1] = anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum()[1];
        ev->fGenMom[2] = anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum()[2];
        
        ev->fEventTime = anEvent->GetPrimaryVertex()->GetT0();
        
        ev->fEnergy = particle_source->GetParticleEnergy();
        ev->fPDGcode = anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
        */
}





// ****** Calibration Mode Switch ****** //
void PrimaryGeneratorAction::ToggleCalibrationMode(G4bool flipOn) {
    
    if(flipOn)
    {
        ResetAllSourceIntensities();
        fDistStore = fDistribution;
        sModes = "C";
        fCalibration = 999;
        
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
    else
    {
        particle_source->SetCurrentSourceto(0);
        
        // Remove all calibration sources from particle source
        for(int i = numSources; i < (numSources+calSources); i++)
        {
            G4int index = numSources;
            particle_source->DeleteaSource(index);		// Note: Removal of a source drops indices of higher index sources by 1
        }
        fDistribution = fDistStore; fDistStore = "";
        fCalibration = 0;
        
        switch(fModule)
        {
            case InverseBeta: { sModes = "I"; break ;}
            case FissionAntinu: { sModes = "F"; break; }
            case CosmicMuon: { sModes = "Z"; break; }
            case CosmicNeutron: { sModes = "Z"; break; }
            default: { sModes = ""; break; }
        }
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

void PrimaryGeneratorAction::SetFile(G4String filename) {
    G4cerr << "set file" << filename<< G4endl;
    textfile = new std::ifstream();
    G4cerr << "set file" << textfile->is_open() << G4endl;
    fFile = false;
    textfile->open(filename);
    G4cerr << "opened file" << filename<< G4endl;
    if(textfile->is_open()){
        G4cerr << "really opened " << filename << G4endl;
        fFile = true;
        fCustom = false;
        fFunction = 0;
    }
    G4cerr << "file set " << fFile << G4endl;
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
        fFile = false;
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
void PrimaryGeneratorAction::InactivateUserEnergySpectrum(G4bool reset)
{
    delete Energy; Energy = 0;
    delete Dist; Dist = 0;
    delete Histogram; Histogram = 0;
    if(reset)
    {
        switch(fModule)
        {
            case InverseBeta:   { fDistribution = "Inverse Beta Product Spectrum"; break; }
            case FissionAntinu: { fDistribution = "Fission Antinuetrino Spectrum"; break; }
            case CosmicNeutron: { fDistribution = "Cosmic Neutron Energy Spectrum"; break; }
            case CosmicMuon:    { fDistribution = "Cosmic Muon Energy Spectrum"; break; }
            default:            { fDistribution = "No Distribution"; break; }
        }
    }
    fFunction = 0;
    customDist = "No Distribution";
    fCustom = false;
}

// The following functions are called by the Messenger class to switch the kinematics module being used. 

// ****** Cosmic Neutron Module Switch ****** //
void PrimaryGeneratorAction::SetCosmicNeutronFlag(G4bool flag)
{
    if(flag)
    {
        neutron_generator = new CosmicNeutronGenerator(verbose,"Outer Tank");
        fDistribution = "Cosmic Neutron Energy Spectrum"; sModes = "Z";
        particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
        particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
    }
    else
    {
        delete neutron_generator; neutron_generator = 0;
        fDistribution = "No Distribution"; sModes = "";
        particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
        G4cout << "\tCosmicNeutronGenerator destroyed. Previously stored values erased and reset to default." << G4endl;
    }
    if(fFunction == 1) { fDistribution = "Cf-252 Spontaneous Fission Neutron Spectrum"; }
    if(fFunction == 2) { fDistribution = "Temperature-Dependent Maxwellian Spectrum"; }
    if(fCustom) { fDistribution = customDist; }
    fModule = CosmicNeutron;
}

// ****** Inverse Beta Module Switch ****** //
void PrimaryGeneratorAction::SetInverseBetaFlag(G4bool flag)
{
    if(flag)
    {
        inverse_beta = new InverseBetaKinematics(verbose,"Scintillator Volume");
        fDistribution = "Inverse Beta Product Spectrum"; sModes = "I";
        particle_gun->SetParticleDefinition(G4Neutron::NeutronDefinition());
        particle_source->SetParticleDefinition(G4Neutron::NeutronDefinition());
    }
    else
    {
        delete inverse_beta; inverse_beta = 0;
        fDistribution = "No Distribution"; sModes = "";
        particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
        G4cout << "\tInverseBetaKinematics destroyed. Previously stored values erased and reset to default." << G4endl;
    }
    if(fFunction == 1) { fDistribution = "Cf-252 Spontaneous Fission Neutron Spectrum"; }
    if(fFunction == 2) { fDistribution = "Temperature-Dependent Maxwellian Spectrum"; }
    if(fCustom) { fDistribution = customDist; }
    fModule = InverseBeta;
}

// ****** Fission Antineutrino Module Switch ****** //
void PrimaryGeneratorAction::SetFissionAntiNuFlag(G4bool flag)
{
    if(fModule == InverseBeta)
    { G4cout << "*** WARNING: Inverse Beta Module is still active and already contains this module within it. Fission Antineutrino Module not activated. ***" << G4endl; return; }
    if(flag)
    {
        fission_spec = new FissionAntiNuGenerator(true,verbose);
        fDistribution = "Fission Antineutrino Spectrum"; sModes = "F";
        particle_gun->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
        particle_source->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
    }
    else
    {
        delete fission_spec; fission_spec = 0;
        fDistribution = "No Distribution"; sModes = "";
        particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
        G4cout << "\tFissionAntiNuGenerator destroyed. Previously stored values erased and reset to default." << G4endl;
    }
    if(fFunction == 1) { fDistribution = "Cf-252 Spontaneous Fission Neutron Spectrum"; }
    if(fFunction == 2) { fDistribution = "Temperature-Dependent Maxwellian Spectrum"; }
    if(fCustom) { fDistribution = customDist; }
    fModule = FissionAntinu;
}

// ****** Cosmic Muon Module Switch ****** //
void PrimaryGeneratorAction::SetCosmicMuonFlag(G4bool flag)
{
    if(flag)
    {
        muon_generator = new CosmicMuonGenerator(verbose,"Detector Shell");
        fDistribution = "Cosmic Muon Energy Spectrum"; sModes = "Z";
        particle_gun->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
        particle_source->SetParticleDefinition(G4MuonMinus::MuonMinusDefinition());
    }
    else
    {
        delete muon_generator; muon_generator = 0;
        fDistribution = "No Distribution"; sModes = "";
        particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());
        G4cout << "\tCosmicMuonGenerator destroyed. Previously stored values erased and reset to default." << G4endl;
    }
    if(fFunction == 1) { fDistribution = "Cf-252 Spontaneous Fission Neutron Spectrum"; }
    if(fFunction == 2) { fDistribution = "Temperature-Dependent Maxwellian Spectrum"; }
    if(fCustom) { fDistribution = customDist; }
    fModule = CosmicMuon;
}

// ****** Module Reset Switch ****** //
void PrimaryGeneratorAction::ResetSimulationModule()
{
    switch(fModule)
    {
        case InverseBeta: { SetInverseBetaFlag(false); break; }
        case FissionAntinu: { SetFissionAntiNuFlag(false); break; }
        case CosmicNeutron: { SetCosmicNeutronFlag(false); break; }
        case CosmicMuon: { SetCosmicMuonFlag(false); break; }
        default: { break; }
    }
    fModule = None;
    ResetAllSourceIntensities();
}

// ****** Spectrum Test Function ****** //
void PrimaryGeneratorAction::GenerateEnergySpectrumWithoutSimulation(G4int n)		// Used to test the shape and statistics of the chosen energy spectrum
{
    if(fCustom)	// For custom distributions
  {
      // Sample custom energy/probability vectors n times
      for(int m = 0; m < n; m++)
      {
          GenerateCustomParticleEnergy();
          
          // Print status to show progress
          if(m % 100000 == 0) { G4cout << "Generated " << m << " samples." << G4endl; }
      }
      
      // Outputs resulting histograms into text file CustomEnergyHistogram.txt
      LogSession* log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName("CustomEnergyHistogram.txt");
    log->OpenFile();
    
    // Prints the distribution parameters first
    (*log) << "\t   Distribution Name:      " << fDistribution << "\n";
    (*log) << "\t   Minimum Energy:         " << G4BestUnit(Energy->front(),"Energy") << "\n";
    (*log) << "\t   Maximum Energy:         " << G4BestUnit(Energy->back(),"Energy") << "\n";
    
    // Then prints the energy histogram
    (*log) << "Max Bin Energy\tCount\n";
    std::map<G4int,G4int>::iterator itr = Histogram->begin();
    for( ; itr != Histogram->end(); itr++)
    {
        (*log) << (*Energy)[itr->first] << "\t" << itr->second << std::endl;
    }
    log->CloseFile();
  }
  else if(fFunction)
  {
      for(int m = 0; m < n; m++)
      {
          GenerateFunctionParticleEnergy();
          if(m % 100000 == 0) { G4cout << "Generated " << m << " samples." << G4endl; }
      }
      LogSession* log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName("EnergyDistribution.txt");
        log->OpenFile();
        (*log) << "Max Bin Energy\tCount\n";
        std::map<G4int,G4int>::iterator itr = Histogram->begin();
        for( ; itr != Histogram->end(); itr++)
        {
            (*log) << (*Energy)[itr->first] << "\t" << itr->second << std::endl;
        }
        log->CloseFile();
  }
  else if(fFile)
  {
      for(int m = 0; m < n; m++)
      {
          GenerateFileParticleEnergy();
      }
  }
  else if(fModule != None)
  {
      switch(fModule)
      {
          case InverseBeta: { inverse_beta->GenerateKinematicsWithoutSimulation(n); break; }
          case FissionAntinu: { fission_spec->GenerateEnergiesWithoutSimulation(n); break; }
          case CosmicNeutron: { neutron_generator->GenerateEnergiesWithoutSimulation(n); break; }
          case CosmicMuon: { muon_generator->GenerateEnergiesWithoutSimulation(n); break; }
          default: { break; }
      }
  }
  else if(fGun)
  {
      G4cout << "\tParticle Gun:\t" << particle_gun->GetParticleDefinition()->GetParticleName() << G4endl;
      G4cout << "\tParticle Energy:\t" << G4BestUnit(particle_gun->GetParticleEnergy(),"Energy") << G4endl;
      G4cout << "\tNo energy spectrum specified. Histogram not generated." << G4endl;
  }
  else
  {
      particle_source->ListSource();
      G4cout << "\tParticle Energy:\t" << G4BestUnit(particle_source->GetCurrentSource()->GetParticleEnergy(),"Energy") << G4endl;
      G4cout << "\tNo energy spectrum specified. Histogram not generated." << G4endl;
  }
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




