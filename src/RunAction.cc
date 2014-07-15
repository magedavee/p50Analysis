// Unrestricted Use - Property of AECL
//
// RunAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Run Controller and Processing
//	Contains definitions of functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "RunAction.hh"			// Specifies the file which contains the class structure

#include "RunMessenger.hh"		// Specifies user-defined classes which are called upon in this class
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4Run.hh"			// Specifies all the classes which contain structures called upon in this class
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output
#include "globals.hh"			// Specifies the class defining all global parameters and variable types
#include <map>
#include <vector>

	// ****** Constructor ****** //
RunAction::RunAction()
{
	// Pointers
  gen = 0;
  run_messenger = new RunMessenger(this);

	// Run statistics
  nEnergy = 0;
  nTime = 0;
  nTherm = 0;
  nDist = 0;
  pEnergy = 0;
  nEntry = 0;
  nZenith = 0;
  photCount = 0;
  photCount2 = 0;
  RunTime = 0;
  vetoCount = 0;
  ionDep = 0;
  ionEsc = 0;
  ionVDep = 0;

	// First Print Setting
  fFirstPrint = true;
  fFirstNCPos = true;
  fFirstNEPos = true;

	// Print Modifiers
  scint = "";
  prefix = "";
  suffix = "";
  geomI = "Inactive";
  geomO = "Inactive";
  geomB = "Inactive";
  geomW = "Inactive";
  geomV = "Inactive";
  fRawNeutronCapture = false;
  fRawOpticalPhoton = false;
  fRawPhotonCount = false;
  fRawVetoCount = false;
  fSegregation = false;
  EvtSeparationBin = 999.*s;

	// Output Table Modifiers
  nEnergyBin = 0.025;
  nEnergyRef = 1.e-8*MeV;
  nEnergyNum = 480;
  nEnergyScale = true;

  nTimeBin = 1.*microsecond;
  nTimeRef = 0.*microsecond;
  nTimeNum = 250;

  nThermBin = 1.*microsecond;
  nThermRef = 0.*microsecond;
  nThermNum = 250;

  nDistBin = 1.*cm;
  nDistRef = 0.*cm;
  nDistNum = 200;

  pEnergyBin = 10.*keV;
  pEnergyRef = 0.*keV;
  pEnergyNum = 1000;

  nEntryBin = 0.025;
  nEntryRef = 1.e-8*MeV;
  nEntryNum = 480;
  nEntryScale = true;

  photCountBin = 10;
  photCountRef = 0;
  photCountNum = 300;

  vetoCountBin = 50;
  vetoCountRef = 0;
  vetoCountNum = 100;

  ionDepBin = 10.*keV;
  ionDepRef = 0.*keV;
  ionDepNum = 1000;

  ionEscBin = 10.*keV;
  ionEscRef = 0.*keV;
  ionEscNum = 1000;

  ionVDepBin = 1.*MeV;
  ionVDepRef = 0.*MeV;
  ionVDepNum = 1000;

}

	// ****** Destructor ****** //
RunAction::~RunAction()
{
  if(nEnergy) { delete nEnergy; }
  if(nTime) { delete nTime; }
  if(nTherm) { delete nTherm; }
  if(nDist) { delete nDist; }
  if(pEnergy) { delete pEnergy; }
  if(nZenith) { delete nZenith; }
  if(photCount) { delete photCount; }
  if(photCount2) { delete photCount2; }
  if(RunTime) { delete RunTime; }
  if(vetoCount) { delete vetoCount; }
  if(ionDep) { delete ionDep; }
  if(ionEsc) { delete ionEsc; }
  if(ionVDep) { delete ionVDep; }
  delete run_messenger;
}

	// ****** Pre-Run Processing ****** //
void RunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cerr << "~~~~~ Run Number " << (aRun->GetRunID() + 1) << " Initiated ~~~~~\n" << G4endl;

  // G4RunManager::GetRunManager()->SetRandomNumberStore(true);	// Saves random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);	// Saves random number seed
  if(!gen) { gen = (PrimaryGeneratorAction*)(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()); }
  DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

	// Re-initialize event variables
  n_count = -1;
  ncap_count = 0;
  sumGamma = 0;
  p_count = -1;

  geomI = "Inactive";
  geomO = "Inactive";
  geomB = "Inactive";
  geomW = "Inactive";
  geomV = "Inactive";
  fFirstPrint = true;
  fFirstNCPos = true;
  fFirstNEPos = true;

	// Obtain run modifiers
  scint = detector->GetScintillatorComposition();
  prefix = gen->GetRunSettings();

  std::stringstream runID; runID << (aRun->GetRunID()+1);

  char rnumstr[10];
  sprintf(rnumstr,"r%04i",nRunNumber);
  G4String rnumg4str(rnumstr);
  suffix = runID.str() + rnumg4str;

  if(detector->GetInnerTankInclusion())   { geomI = "Active"; } else { scint = "N"; }
  if(detector->GetOuterTankInclusion())   { geomO = "Active"; }
  if(detector->GetBPolyInclusion())       { geomB = "Active"; }
  if(detector->GetShieldInclusion()) { geomW = "Active"; }
  if(detector->GetMuonVetoInclusion())    { geomV = "Active"; }
  prefix += scint;

  if(scint == "N")      { scint = "N/A"; }
  else if(scint == "S") { scint = "LAB Gd0.00%"; }
  else if(scint == "L") { scint = "LAB Gd0.01%"; }
  else if(scint == "T") { scint = "LAB Gd0.10%"; }
  else if(scint == "H") { scint = "LAB Gd1.00%"; }
  else if(scint == "M") { scint = "LAB Gd2.00%"; }
  else if(scint == "G") { scint = "LAB Gd100%"; }

 
  if(nEnergy)	// Neutron capture energy
  {
    for(int i = 1; i <= nEnergyNum; i++)
    {
      G4double eUpper = 0.0;
      if(nEnergyScale) { eUpper = nEnergyRef * pow(10.,(i*nEnergyBin)); }
      else             { eUpper = nEnergyRef + i*nEnergyBin; }
      (*nEnergy)[eUpper] = 0;
    }
    if(!nEnergyScale) { (*nEnergy)[999.*GeV] = 0; }
  }

  if(nTime)	// Neutron capture time
  {
    for(int i = 1; i <= nTimeNum; i++)
    {
      G4double tUpper = nTimeRef + i*nTimeBin;
      (*nTime)[tUpper] = 0;
    }
    (*nTime)[999.*s] = 0;
  }

  if(nTherm)	// Neutron thermalization time
  {
    for(int i = 1; i <= nThermNum; i++)
    {
      G4double thermUpper = nThermRef + i*nThermBin;
      (*nTherm)[thermUpper] = 0;
    }
    (*nTherm)[999.*s] = 0;
  }

  if(nDist)	// Neutron capture distance
  {
    for(int i = 1; i <= nDistNum; i++)
    {
      G4double distUpper =  nDistRef + i*nDistBin;
      (*nDist)[distUpper] = 0;
    }
    (*nDist)[999.*m] = 0;
  }

  if(pEnergy)	// Recoil proton energy
  {
    for(int i = 1; i <= pEnergyNum; i++)
    {
      G4double eUpper = pEnergyRef + i*pEnergyBin;
      (*pEnergy)[eUpper] = 0;
    }
    (*pEnergy)[999.*GeV] = 0;
  }

  if(nEntry)	// Neutron entry energy
  {
    for(int i = 1; i <= nEntryNum; i++)
    {
      G4double eUpper = 0.0;
      if(nEntryScale) { eUpper = nEntryRef * pow(10.,(i*nEntryBin)); }
      else            { eUpper = nEntryRef + i*nEntryBin; }
      (*nEntry)[eUpper] = 0;
    }
    if(!nEntryScale) { (*nEntry)[999.*GeV] = 0; }
  }

  if(nZenith)	// Neutron entry zenith angle
  {
    G4double bin = 1.*deg;
    G4double ref = 0.*deg;
    for(int i = 1; i <= 180; i++)
    {
      G4double zUpper = ref + i*bin;
      (*nZenith)[zUpper] = 0;
    }
  }

  if(photCount)		// Liquid scintillator photon count
  {
    for(int i = 1; i <= photCountNum; i++)
    {
      G4int cUpper = photCountRef + i*photCountBin;
      (*photCount)[cUpper] = 0;
      (*photCount2)[cUpper] = 0;
    }
    (*photCount)[999999] = 0;
    (*photCount2)[999999] = 0;
  }

  if(vetoCount)		// Veto panel photon count
  {
    for(int i = 1; i <= vetoCountNum; i++)
    {
      G4int cUpper = vetoCountRef + i*vetoCountBin;
      (*vetoCount)[cUpper] = 0;
    }
    (*vetoCount)[999999] = 0;
  }

  if(ionDep)		// Ionisation energy deposit
  {
    for(int i = 1; i <= ionDepNum; i++)
    {
      G4double eUpper = ionDepRef + i*ionDepBin;
      (*ionDep)[eUpper] = 0;
    }
    (*ionDep)[999.*GeV] = 0;
  }

  if(ionEsc)		// Ionisation energy escape
  {
    for(int i = 1; i <= ionEscNum; i++)
    {
      G4double eUpper = ionDepRef + i*ionDepBin;
      (*ionEsc)[eUpper] = 0;
    }
    (*ionEsc)[999.*GeV] = 0;
  }

  if(ionVDep)		// Veto ionisation energy deposit
  {
    for(int i = 1; i <= ionVDepNum; i++)
    {
      G4double eUpper = ionVDepRef + i*ionVDepBin;
      (*ionVDep)[eUpper] = 0;
    }
    (*ionVDep)[9999.*GeV] = 0;
  }
}

	// The following functions allow the EventAction class to pass event statistics for accumulation and processing

void RunAction::AddNCount(G4int count)
{
  if(n_count == -1 && count != 0)
  {
    n_count = 0;
  }
  n_count += count;
}

void RunAction::AddNCapCount(G4int count)
{
  ncap_count += count;
}

void RunAction::AddPCount(G4int count)
{
  if(p_count == -1 && count != 0)
  {
    p_count = 0;
  }
  p_count += count;
}

void RunAction::PassNCEnergy(G4double E)
{
  if(nEnergy)
  {
    std::map<G4double,G4int>::iterator itr = nEnergy->begin();
    while(E > itr->first && itr != nEnergy->end()) { itr++; }
    (*nEnergy)[itr->first]++;
  }
}

void RunAction::PassNCTime(G4double t)
{
  if(nTime)
  {
    std::map<G4double,G4int>::iterator itr = nTime->begin();
    while(t > itr->first && itr != nTime->end()) { itr++; }
    (*nTime)[itr->first]++;
  }
}

void RunAction::PassNThermTime(G4double tt)
{
  if(nTherm)
  {
    std::map<G4double,G4int>::iterator itr = nTherm->begin();
    while(tt > itr->first && itr != nTherm->end()) { itr++; }
    (*nTherm)[itr->first]++;
  }
}

void RunAction::PassNCDistance(G4double d)
{
  if(nDist)
  {
    std::map<G4double,G4int>::iterator itr = nDist->begin();
    while(d > itr->first && itr != nDist->end()) { itr++; }
    (*nDist)[itr->first]++;
  }
}

void RunAction::PassNEnergy(G4double E)
{
  if(nEntry)
  {
    std::map<G4double,G4int>::iterator itr = nEntry->begin();
    while(E > itr->first && itr != nEntry->end()) { itr++; }
    (*nEntry)[itr->first]++;
  }
}

void RunAction::PassNZenithAngle(G4double angle)
{
  if(nZenith)
  {
    std::map<G4double,G4int>::iterator itr = nZenith->begin();
    while(angle > itr->first && itr != nZenith->end()) { itr++; }
    (*nZenith)[itr->first]++;
  }
}

void RunAction::PassGammasCreated(G4int gamma)
{
  sumGamma += gamma;
}

void RunAction::PassEventTiming(std::map<G4double,G4int>* data)
{
  if(RunTime)
  {
    if(RunTime->empty()) { *RunTime = *data; }
    else
    {
      std::map<G4double,G4int>::iterator itr = data->begin();
      for( ; itr != data->end(); itr++) { (*RunTime)[itr->first] += itr->second; }
    }
  }
}

void RunAction::PassPhotonCount(G4int photons, G4int exPhotons)
{
  if(photCount && photons)
  {
    std::map<G4int,G4int>::iterator itr = photCount->begin();
    while(photons > itr->first && itr != photCount->end()) { itr++; }
    (*photCount)[itr->first]++;
  }
  if(photCount2 && exPhotons)
  {
    std::map<G4int,G4int>::iterator itr = photCount2->begin();
    while(exPhotons > itr->first && itr != photCount2->end()) { itr++; }
    (*photCount2)[itr->first]++;
  }
}

void RunAction::PassVetoCount(G4int photons)
{
  if(vetoCount)
  {
    std::map<G4int,G4int>::iterator itr = vetoCount->begin();
    while(photons > itr->first && itr != vetoCount->end()) { itr++; }
    (*vetoCount)[itr->first]++;
  }
}

void RunAction::PassIonEDeposit(G4double dep)
{
  if(ionDep)
  {
    std::map<G4double,G4int>::iterator itr = ionDep->begin();
    while(dep > itr->first && itr != ionDep->end()) { itr++; }
    (*ionDep)[itr->first]++;
  }
}

void RunAction::PassIonEEscape(G4double esc)
{
  if(ionEsc)
  {
    std::map<G4double,G4int>::iterator itr = ionEsc->begin();
    while(esc > itr->first && itr != ionEsc->end()) { itr++; }
    (*ionEsc)[itr->first]++;
  }
}

void RunAction::PassIonVDeposit(G4double dep)
{
  if(ionVDep)
  {
    std::map<G4double,G4int>::iterator itr = ionVDep->begin();
    while(dep > itr->first && itr != ionVDep->end()) { itr++; }
    (*ionVDep)[itr->first]++;
  }
}

void RunAction::PassProtonEnergy(G4double pEn)
{
  if(pEnergy)
  {
    std::map<G4double,G4int>::iterator itr = pEnergy->begin();
    while(pEn > itr->first && itr != pEnergy->end()) { itr++; }
    (*pEnergy)[itr->first]++;
  }
}

	// The following functions allow the EventAction class to print event data with corresponding run modifiers

void RunAction::WriteNCPosition(G4ThreeVector pos, G4int evtNo)
{
    if(fNCPosition)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronCapturePosition"+suffix+".txt");
    log->OpenFile(fFirstNCPos,!fFirstNCPos);
    (*log) << "Event Number: " << evtNo << "\t" << pos.x()/cm << "\t" << pos.y()/cm << "\t" << pos.z()/cm << " cm" << std::endl;
    log->CloseFile();
    fFirstNCPos = false;
    }
}

void RunAction::WriteNEPosition(G4ThreeVector pos, G4int evtNo)
{
    if(fNEPosition)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronEntryPosition"+suffix+".txt");
    log->OpenFile(fFirstNEPos,!fFirstNEPos);
    (*log) << "Event Number: " << evtNo << "\t" << pos.x()/cm << "\t" << pos.y()/cm << "\t" << pos.z()/cm << " cm" << std::endl;
    log->CloseFile();
    fFirstNEPos = false;
  }
}

	// ****** Post-Run Processing ****** //
void RunAction::EndOfRunAction(const G4Run* aRun)
{
    
	// Print run number
  G4int numOfEvents = aRun->GetNumberOfEvent();
  if (numOfEvents == 0)
  {
    return;
  }

  RootIO::GetInstance()->WriteFile();

  std::stringstream stream;	// This string stream buffers the end of run terminal output
  LogSession* log = 0;
  G4bool FilesRequested = false;
  std::stringstream files;	// This string stream buffers the file names for the run info

  files << G4endl;

  stream
    << "\n--------------- End of Run " << (aRun->GetRunID()+1) << " ----------------" 
    << G4endl << G4endl;

	// Determines distribution used in run and displays distribution name
  G4double particle_energy;
  if(gen->GetDistribution() == "No Distribution")
  {
    if(gen->GetGunSettings()) { particle_energy = gen->GetParticleGun()->GetParticleEnergy(); }
    else                      { particle_energy = gen->GetParticleSource()->GetParticleEnergy(); }
  }
  else
  {
    particle_energy = -1.;
  }

  if(particle_energy < 0.)
  {
    stream
    << " Particle Energy Distribution		: " << (gen->GetDistribution())
    << G4endl << G4endl;
  }
  else
  {
    stream
    << " Particle Energy			: " << G4BestUnit(particle_energy,"Energy")
    << G4endl << G4endl;
  }

	// Displays overview of neutron capture data in run
  if(n_count != -1)
  {
    stream
    << " Neutron Count in Inner Tank		: " << n_count << "\n"
    << " Neutron Capture Count in Inner Tank	: " << ncap_count
    << G4endl;

    if(sumGamma > 0)
    {
      stream
      << " Avg. No. of Gammas Per N-Capture	: " << (G4double) sumGamma / (G4double) n_count
      << G4endl;
    }
    stream << G4endl;
  }

	// Prints out requested histograms into respective output files
   if(nTime)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronCaptureTime"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Time (us):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = nTime->begin();
    for( ; itr != nTime->end(); itr++)
    {
      (*log) << (itr->first / microsecond) << "\t" << itr->second << std::endl;
    }
    stream << " Neutron capture time details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
    if(nEnergy)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronCaptureEnergy"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy (MeV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator ittr = nEnergy->begin();
    for( ; ittr != nEnergy->end(); ittr++)
    {
      (*log) << (ittr->first / MeV) << "\t" << ittr->second << std::endl;
    }
    stream << " Neutron capture energy details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(nDist)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronCaptureDistance"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Distance (cm):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator iter = nDist->begin();
    for( ; iter != nDist->end(); iter++)
    {
      (*log) << (iter->first / cm) << "\t" << iter->second << std::endl;
    }
    stream << " Neutron capture distance details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }
   if(fNCPosition)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronCapturePosition"+suffix+".txt");
    log->OpenFile(fFirstNCPos,!fFirstNCPos);
    stream << " Neutron capture position details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }
  if(!nEnergy && !nTime && !nDist && !fNCPosition)
  {
    stream
    << " No neutron capture details were requested, text file(s) not created."
    << G4endl;
    }
  stream << G4endl;

   if(nTherm)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronThermalization"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Time (us):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = nTherm->begin();
    for( ; itr != nTherm->end(); itr++)
    {
      (*log) << (itr->first / microsecond) << "\t" << itr->second << std::endl;
    }
    stream << " Neutron thermalization details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
    else
  {
	// Message if no neutron data was requested
    stream
    << " No neutron thermalization details were requested, text file(s) not created."
    << G4endl;
    }
   stream << G4endl;

  if(nEntry)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronEntryEnergy"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy (MeV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = nEntry->begin();
    for( ; itr != nEntry->end(); itr++)
    {
      (*log) << (itr->first / MeV) << "\t" << itr->second << std::endl;
    }
    stream << " Neutron entry energy details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(fNEPosition)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronEntryPosition"+suffix+".txt");
    log->OpenFile(fFirstNEPos,!fFirstNEPos);
    stream << " Neutron entry position details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(nZenith)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"NeutronEntryAngle"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Angle (deg):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = nZenith->begin();
    for( ; itr != nZenith->end(); itr++)
    {
      (*log) << (itr->first / deg) << "\t" << itr->second << std::endl;
    }
    stream << " Neutron entry angle details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(!fNEPosition && !nZenith)
  {
	// Message if no background neutron data was requested
    stream
    << " No background neutron entry details were requested, text file(s) not created."
    << G4endl;
    }
  stream << G4endl;

	// Displays overview of recoil proton data in run
  if(p_count != -1)
  {
    stream
    << " Proton Count in Inner Tank		: " << p_count
    << G4endl << G4endl;
  }

   if(pEnergy)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"RecoilProtonEnergies"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = pEnergy->begin();
    for( ; itr != pEnergy->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    stream << " Recoil proton energy details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }
  else
  {
	// Message if no proton data was requested
    stream
    << " No recoil proton details were requested, text file(s) not created."
    << G4endl;
  }
  stream << G4endl;

    if(RunTime && !RunTime->empty())
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"PhotonTimeProfile"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Time Bin (us):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = RunTime->begin();
    for( ; itr != RunTime->end(); itr++)
    {
      (*log) << itr->first / microsecond << "\t" << (G4double)(itr->second) / numOfEvents << std::endl;
    }
    stream << " Liquid scintillator photon arrival details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
    if(photCount)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"OpticalPhotonCount"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Count Bin:\tCount:" << std::endl;
    std::map<G4int,G4int>::iterator itr = photCount->begin();
    for( ; itr != photCount->end(); itr++)
    {
      (*log) << itr->first << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    (*log) << "***** The following is the count of photons arriving after " << G4BestUnit(EvtSeparationBin,"Time") << "*****" << std::endl;
    (*log) << std::endl;
    (*log) << "Count Bin:\tCount:" << std::endl;
    itr = photCount2->begin();
    for( ; itr != photCount2->end(); itr++)
    {
      (*log) << itr->first << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Liquid scintillator photon count details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
    if(!RunTime && !photCount)
  {
	// Message if no optical photon data was requested
    stream
    << " No liquid scintillator optical photon details were requested, text file(s) not created."
    << G4endl;
    }
    stream << G4endl;

  if(vetoCount)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"VetoPhotonCount"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Count Bin:\tCount:" << std::endl;
    std::map<G4int,G4int>::iterator itr = vetoCount->begin();
    for( ; itr != vetoCount->end(); itr++)
    {
      (*log) << itr->first << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Veto panel photon count details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(!vetoCount)
  {
	// Message if no background optical photon data was requested
    stream
    << " No veto panel optical photon details were requested, text file(s) not created."
    << G4endl;
  }
  stream << G4endl;

  if(ionDep)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"IonisationDeposit"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionDep->begin();
    for( ; itr != ionDep->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Ionisation energy deposit details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(ionEsc)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"IonisationEscape"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionEsc->begin();
    for( ; itr != ionEsc->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Ionisation energy escape details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
    if(ionVDep)
  {
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"VetoIonisationDeposit"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionEsc->begin();
    for( ; itr != ionEsc->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Veto ionisation energy deposit details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
    }
   if(!ionDep && !ionEsc && !ionVDep)
  {
	// Message if no ionisation data was requested
    stream
    << " No ionizing particle energy deposit details were requested, text file(s) not created."
    << G4endl;
  }
  stream << G4endl;
  
  // Print run info
  /*  log = LogSession::GetLogSessionPointer();
  log->SetOutputFileName(prefix+"RunInfo"+suffix+".txt");
  log->OpenFile(fFirstPrint);
  (*log) << "----------------------- Run Info ---------------------------" << std::endl << std::endl
         << "  Number of Events:                " << aRun->GetNumberOfEvent() << std::endl
         << "  Scintillator:                    " << scint << std::endl
         << "  Particle:                        " << gen->GetParticleSource()->GetCurrentSource()->GetParticleDefinition()->GetParticleName() << std::endl
         << "  Particle Spectrum:               " << gen->GetDistribution() << std::endl;
  if(particle_energy >= 0.)
    { (*log) << "  Particle Energy:                 " << G4BestUnit(particle_energy,"Energy") << std::endl; }
  (*log) << "  Inner Tank Status:               " << geomI << std::endl
         << "  Outer Tank Status:               " << geomO << std::endl
         << "  Borated Polyethylene Status:     " << geomB << std::endl
         << "  Shielding Status:     " << geomW << std::endl
         << "  Muon Veto Panel Status:          " << geomV << std::endl;
  if(FilesRequested)
    { (*log) << "  Home Directory:                  " << LogSession::GetHomeDirectory() << std::endl
	     << "  Files Created:" << std::endl << files.str() << std::endl;				}
  (*log) << std::endl
         << "------------------------------------------------------------" << std::endl;
  log->CloseFile();
  log = 0;*/
    
    // Print directory in which output files for this run may be found
    if(LogSession::GetHomeDirectory() != "" && FilesRequested)
      {
	stream 
	  << " Generated files placed into directory: " << LogSession::GetHomeDirectory()
	  << G4endl;
      }
    
    stream
      << "\n---------------------------------------------"
      << G4endl;
    
    // *** Leave the following at the END of this method, always! ***
    
    // prints string stream buffer into an output file or into the terminal depending on initial settings
    G4String endOfRunStats = stream.str();
    if(LogSession::GetLogSessionStatus())
      {
	log = LogSession::GetLogSessionPointer();
	log->OpenFile(fFirstPrint);
	(*log) << endOfRunStats;
	log->CloseFile();
	log = 0;
      }
    else
      {
	G4cout << endOfRunStats;
      }
    
  // Resets all output tables and run modifiers - they are re-established in BeginOfRunAction() if requested/necessary
  prefix = "";
  suffix = "";
  scint = "";
  if(nEnergy) { nEnergy->clear(); }
  if(nTime) { nTime->clear(); }
  if(nTherm) { nTherm->clear(); }
  if(nDist) { nDist->clear(); }
  if(pEnergy) { pEnergy->clear(); }
  if(nZenith) { nZenith->clear(); }
  if(photCount) { photCount->clear(); photCount2->clear(); }
  if(RunTime) { RunTime->clear(); }
  if(vetoCount) { vetoCount->clear(); }
  if(ionDep) { ionDep->clear(); }
  if(ionEsc) { ionEsc->clear(); }
  if(ionVDep) { ionVDep->clear(); }
  fFirstPrint = false;
  fFirstNCPos = false;
  fFirstNEPos = false;
}

	// ****** Print Run Data at Current State ****** //
void RunAction::PrintCurrentData()
{
   G4int numOfEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEvent();
  if (numOfEvents == 0)
  {
    return;
  }

  LogSession* log = 0;
  G4bool FilesRequested = false;
  std::stringstream files; files << G4endl;

  if(n_count != -1)
  {
    if(nTime)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronCaptureTime"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Time (us):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator itr = nTime->begin();
      for( ; itr != nTime->end(); itr++)
      {
        (*log) << (itr->first / microsecond) << "\t" << itr->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current neutron capture time details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
    if(nEnergy)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronCaptureEnergy"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Energy (MeV):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator ittr = nEnergy->begin();
      for( ; ittr != nEnergy->end(); ittr++)
      {
        (*log) << (ittr->first / MeV) << "\t" << ittr->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current neutron capture energy details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
    if(nDist)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronCaptureDistance"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Distance (cm):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator iter = nDist->begin();
      for( ; iter != nDist->end(); iter++)
      {
        (*log) << (iter->first / cm) << "\t" << iter->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current neutron capture distance details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    } 
    if(nTherm)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronThermalization"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Time (us):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator itr = nTherm->begin();
      for( ; itr != nTherm->end(); itr++)
      {
        (*log) << (itr->first / microsecond) << "\t" << itr->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current neutron thermalization details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
    if(nEntry)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronEntryEnergy"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Energy (MeV):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator itr = nEntry->begin();
      for( ; itr != nEntry->end(); itr++)
      {
        (*log) << (itr->first / MeV) << "\t" << itr->second << std::endl;
      }
      G4cout << " Current neutron entry energy details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
    if(nZenith)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"NeutronEntryAngle"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Angle (deg):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator itr = nZenith->begin();
      for( ; itr != nZenith->end(); itr++)
      {
        (*log) << (itr->first / deg) << "\t" << itr->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current neutron entry angle details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
  }

  if(p_count != -1)
  {
    if(pEnergy)
    {
      log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName(prefix+"RecoilProtonEnergies"+suffix+".txt");
      log->OpenFile(fFirstPrint);
      (*log) << "Energy (keV):\tCount:" << std::endl;
      std::map<G4double,G4int>::iterator itr = pEnergy->begin();
      for( ; itr != pEnergy->end(); itr++)
      {
        (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
      }
      (*log) << std::endl;
      G4cout << " Current recoil proton energy details available in " << log->GetOutputFilePath() << G4endl;
      FilesRequested = true;
      files << "\t" << log->GetOutputFilePath() << G4endl;
      log->CloseFile();
    }
  }

  if(RunTime)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"PhotonTimeProfile"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Time Bin (us):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = RunTime->begin();
    for( ; itr != RunTime->end(); itr++)
    {
      (*log) << (itr->first / microsecond) << "\t\t" << (G4double)(itr->second) / numOfEvents << std::endl;
    }
    (*log) << std::endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    G4cout << " Current liquid scintillator photon arrival details available in " << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }

  if(photCount)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"OpticalPhotonCount"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Count Bin:\tCount:" << std::endl;
    std::map<G4int,G4int>::iterator itr = photCount->begin();
    for( ; itr != photCount->end(); itr++)
    {
      (*log) << itr->first << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Current liquid scintillator photon count details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }

  if(vetoCount)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"VetoPhotonCount"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Count Bin:\tCount:" << std::endl;
    std::map<G4int,G4int>::iterator itr = vetoCount->begin();
    for( ; itr != vetoCount->end(); itr++)
    {
      (*log) << itr->first << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Current veto panel photon count details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }

  if(ionDep)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"IonisationDeposit"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionDep->begin();
    for( ; itr != ionDep->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Current ionisation energy deposit details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }
  if(ionEsc)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"IonisationEscape"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionEsc->begin();
    for( ; itr != ionEsc->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Current ionisation energy escape details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }
  if(ionVDep)
  {
    log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName(prefix+"VetoIonisationDeposit"+suffix+".txt");
    log->OpenFile(fFirstPrint);
    (*log) << "Energy Bin (keV):\tCount:" << std::endl;
    std::map<G4double,G4int>::iterator itr = ionEsc->begin();
    for( ; itr != ionEsc->end(); itr++)
    {
      (*log) << (itr->first / keV) << "\t" << itr->second << std::endl;
    }
    (*log) << std::endl;
    G4cout << " Current veto ionisation energy deposit details available in " << log->GetOutputFilePath() << G4endl;
    FilesRequested = true;
    files << "\t" << log->GetOutputFilePath() << G4endl;
    log->CloseFile();
  }

  G4double particle_energy;
  if(gen->GetDistribution() == "No Distribution")
  {
    if(gen->GetGunSettings()) { particle_energy = gen->GetParticleGun()->GetParticleEnergy(); }
    else                      { particle_energy = gen->GetParticleSource()->GetParticleEnergy(); }
  }
  else
  {
    particle_energy = -1.;
  }

	// Print run info
  log = LogSession::GetLogSessionPointer();
  log->SetOutputFileName(prefix+"RunInfo"+suffix+".txt");
  log->OpenFile(fFirstPrint);
  (*log) << "----------------------- Run Info ---------------------------" << std::endl
         << "  Number of Events:                " << numOfEvents << std::endl
         << "  Scintillator:                    " << scint << std::endl
         << "  Particle:                        " << gen->GetParticleSource()->GetCurrentSource()->GetParticleDefinition()->GetParticleName() << std::endl
         << "  Particle Spectrum:               " << gen->GetDistribution() << std::endl;
  if(particle_energy >= 0.)
{ (*log) << "  Particle Energy:	                " << G4BestUnit(particle_energy,"Energy") << std::endl; }
  (*log) << "  Inner Tank Status:               " << geomI << std::endl
         << "  Outer Tank Status:               " << geomO << std::endl
         << "  Borated Polyethylene Status:     " << geomB << std::endl
         << "  Shielding Status:     " << geomW << std::endl
         << "  Muon Veto Panel Status:          " << geomV << std::endl;
  if(FilesRequested)
{ (*log) << "  Home Directory:                  " << LogSession::GetHomeDirectory() << std::endl
         << "  Files Created:" << std::endl << files.str() << std::endl;				}
  log->CloseFile();
  log = 0;

  fFirstPrint = false;
}

	// The following functions allow the user to specify which output tables to activate for the current simulation run

	// ****** Neutron Capture Energy Output Switch ****** //
void RunAction::SetNeutronCaptureEnergyTable(G4bool activate)
{
  if(nEnergy) { delete nEnergy; nEnergy = 0; }
  if(activate)
  {
    nEnergy = new std::map<G4double,G4int>();
    G4cout << "Neutron capture energy in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    nEnergyBin = 0.025;
    nEnergyRef = 1.e-8*MeV;
    nEnergyNum = 480;
    nEnergyScale = true;
    G4cout << "Neutron capture energy in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Capture Time Output Switch ****** //
void RunAction::SetNeutronCaptureTimeTable(G4bool activate)
{
  if(nTime) { delete nTime; nTime = 0; }
  if(activate)
  {
    nTime = new std::map<G4double,G4int>();
    G4cout << "Neutron capture time in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    nTimeBin = 1.*microsecond;
    nTimeRef = 0.*microsecond;
    nTimeNum = 250;
    G4cout << "Neutron capture time in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Thermalization Time Output Switch ****** //
void RunAction::SetNeutronThermalizationTimeTable(G4bool activate)
{
  if(nTherm) { delete nTherm; nTherm = 0; }
  if(activate)
  {
    nTherm = new std::map<G4double,G4int>();
    G4cout << "Neutron thermalization time from generation will be recorded in future runs." << G4endl;
  }
  else
  {
    nThermBin = 1.*microsecond;
    nThermRef = 0.*microsecond;
    nThermNum = 250;
    G4cout << "Neutron thermalization time from generation will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Capture Distance Output Switch ****** //
void RunAction::SetNeutronCaptureDistanceTable(G4bool activate)
{
  if(nDist) { delete nDist; nDist = 0; }
  if(activate)
  {
    nDist = new std::map<G4double,G4int>();
    G4cout << "Neutron capture distance in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    nDistBin = 1.*cm;
    nDistRef = 0.*cm;
    nDistNum = 200;
    G4cout << "Neutron capture distance in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Capture Position Output Switch ****** //
void RunAction::SetNeutronCapturePositionTable(G4bool activate)
{
  if(activate)
  {
    G4cout << "Neutron capture position in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Neutron capture position in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
  fNCPosition = activate;
}

	// ****** Neutron Capture Raw Data Output Switch ****** //
void RunAction::SetNeutronCaptureRawDataOutput(G4bool activate)
{
  if(activate)
  {
    G4cout << "Raw data for neutron capture in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Raw data for neutron capture in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
  fRawNeutronCapture = activate;
}

void RunAction::SetOpticalPhotonRawDataOutput(G4bool activate)
{
  if(activate)
  {
    G4cout << "Raw data for optical photons hitting the PMT's will be recorded." << G4endl;
  }
  else
  {
    G4cout << "Raw data for optical photons hitting the PMT's will no longer be recorded." << G4endl;
  }
  fRawOpticalPhoton = activate;
}

	// ****** Recoil Proton Energy Output Switch ****** //
void RunAction::SetRecoilProtonEnergyTable(G4bool activate)
{
  if(pEnergy) { delete pEnergy; pEnergy = 0; }
  if(activate)
  {
    pEnergy = new std::map<G4double,G4int>();
    G4cout << "Recoil proton energy in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    pEnergyBin = 10.*keV;
    pEnergyRef = 0.*keV;
    pEnergyNum = 1000;
    G4cout << "Recoil proton energy in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Entry Energy Output Switch ****** //
void RunAction::SetNeutronEntryEnergyTable(G4bool activate)
{
  if(nEntry) { delete nEntry; nEntry = 0; }
  if(activate)
  {
    nEntry = new std::map<G4double,G4int>();
    G4cout << "Neutron entry energy in liquid scintillator tank will be recorded in future runs." << G4endl;
  }
  else
  {
    nEntryBin = 0.025;
    nEntryRef = 1.e-8*MeV;
    nEntryNum = 480;
    nEntryScale = true;
    G4cout << "Neutron entry energy in liquid scintillator tank will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Neutron Entry Position Output Switch ****** //
void RunAction::SetNeutronEntryPositionTable(G4bool activate)
{
  if(activate)
  {
    G4cout << "Neutron entry position into scintillator tank will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Neutron entry position into scintillator tank will no longer be recorded in future runs." << G4endl;
  }
  fNEPosition = activate;
}

	// ****** Neutron Entry Zenith Angle Output Switch ****** //
void RunAction::SetNeutronEntryAngleTable(G4bool activate)
{
  if(nZenith) { delete nZenith; nZenith = 0; }
  if(activate)
  {
    nZenith = new std::map<G4double,G4int>();
    G4cout << "Neutron entry zenith angle into scintillator tank will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Neutron entry zenith angle into scintillator tank will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Liquid Scintillator Photon Count Output Switch ****** //
void RunAction::SetLSPhotonCountTable(G4bool activate)
{
  if(photCount) { delete photCount; photCount = 0; }
  if(photCount2) { delete photCount2; photCount2 = 0; }
  if(activate)
  {
    photCount = new std::map<G4int,G4int>();
    photCount2 = new std::map<G4int,G4int>();
    G4cout << "Photons detected in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    photCountBin = 10;
    photCountRef = 0;
    photCountNum = 300;
    G4cout << "Photons detected in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Change Event Separation Timing ****** //
void RunAction::SetLSPhotonCountEventCut(G4double cut)
{
  EvtSeparationBin = cut;
  G4cout << "Optical photon time separation in liquid scintillator set to " << G4BestUnit(EvtSeparationBin,"Time") << "." << G4endl;
}

	// ****** Liquid Scintillator Photon Raw Data Output Switch ****** //
void RunAction::SetLSPhotonCountRawDataOutput(G4bool activate)
{
  if(activate)
  {
    G4cout << "Raw data for photons detected in liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Raw data for photons detected in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
  fRawPhotonCount = activate;
}

	// ****** Event-By-Event Photon Time Output Switch ****** //
void RunAction::SetPhotonTravelTimeTable(G4bool activate)
{
  if(RunTime) { delete RunTime; }
  EventAction* event = (EventAction*)(G4RunManager::GetRunManager()->GetUserEventAction());
  event->SetPhotonTravelTimeTable(activate);
  if(activate)
  {
    RunTime = new std::map<G4double,G4int>();
  }
  if(!activate)
  {
    RunTime = 0;
    travelBin = 1.*microsecond;
    travelRef = 0.*microsecond;
    travelNum = 500;
  }
}

	// ****** Veto Panel Photon Count Output Switch ****** //
void RunAction::SetVetoPanelCountTable(G4bool activate)
{
  if(vetoCount) { delete vetoCount; vetoCount = 0; }
  if(activate)
  {
    vetoCount = new std::map<G4int,G4int>();
    G4cout << "Photons detected in muon veto panels will be recorded in future runs." << G4endl;
  }
  else
  {
    vetoCountBin = 50;
    vetoCountRef = 0;
    vetoCountNum = 100;
    G4cout << "Photons detected in muon veto panels will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Veto Panel Photon Raw Data Output Switch ****** //
void RunAction::SetVetoPanelCountRawDataOutput(G4bool activate)
{
  if(activate)
  {
    G4cout << "Raw data for photons detected in muon veto panels will be recorded in future runs." << G4endl;
  }
  else
  {
    G4cout << "Raw data for photons detected in muon veto panels will no longer be recorded in future runs." << G4endl;
  }
  fRawVetoCount = activate;
}

	// ****** Ionisation Energy Deposit Output Switch ****** //
void RunAction::SetIonisationDepositTable(G4bool activate)
{
  DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if(ionDep) { delete ionDep; ionDep = 0; }
  if(activate)
  {
    ionDep = new std::map<G4double,G4int>();
    G4cout << "Ionisation energy deposit in liquid scintillator will be recorded in future runs." << G4endl;
    if(detector->IsOpticalTransported() && G4RunManager::GetRunManager()->GetVerboseLevel() > 0)
    { G4cout << "*** CAUTION: Optical photon generation/transport has been specified. Requested data will be inaccurate as scintillation replaces energy deposition processes. ***" << G4endl; }
  }
  else
  {
    ionDepBin = 10.*keV;
    ionDepRef = 0.*keV;
    ionDepNum = 1000;
    G4cout << "Ionisation energy deposit in liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Ionisation Energy Escape Output Switch ****** //
void RunAction::SetIonisationEscapeTable(G4bool activate)
{
  if(ionEsc) { delete ionEsc; ionEsc = 0; }
  if(activate)
  {
    ionEsc = new std::map<G4double,G4int>();
    G4cout << "Ionizing energy escape from liquid scintillator will be recorded in future runs." << G4endl;
  }
  else
  {
    ionEscBin = 10.*keV;
    ionEscRef = 0.*keV;
    ionEscNum = 1000;
    G4cout << "Ionizing energy escape from liquid scintillator will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Veto Panel Ionisation Energy Deposit Output Switch ****** //
void RunAction::SetVetoIonisationDepositTable(G4bool activate)
{
  DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if(ionVDep) { delete ionVDep; ionVDep = 0; }
  if(activate)
  {
    ionVDep = new std::map<G4double,G4int>();
    G4cout << "Ionisation energy deposit in veto panels will be recorded in future runs." << G4endl;
    if(detector->IsOpticalTransported() && G4RunManager::GetRunManager()->GetVerboseLevel() > 0)
    { G4cout << "*** CAUTION: Optical photon generation/transport has been specified. Requested data will be inaccurate as scintillation replaces energy deposition processes. ***" << G4endl; }
  }
  else
  {
    ionVDepBin = 1.*MeV;
    ionVDepRef = 0.*MeV;
    ionVDepNum = 1000;
    G4cout << "Ionisation energy deposit in veto panels will no longer be recorded in future runs." << G4endl;
  }
}

	// ****** Print Output Status ****** //
void RunAction::PrintOutputList() const
{
  G4cout << "& nCapEnergy   ";
  if(nEnergy)     { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nCapTime   ";
  if(nTime)       { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nThermTime   ";
  if(nTherm)      { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nCapDist   ";
  if(nDist)       { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nCapPos   ";
  if(fNCPosition) { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& rpEnergy   ";
  if(pEnergy)     { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nEntryEnergy   ";
  if(nEntry)      { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nEntryPos   ";
  if(fNEPosition) { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& nEntryAngle   ";
  if(nZenith)     { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& opLiquidHit   ";
  if(photCount)   { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& opTravelTime   ";
  if(RunTime)     { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& opVetoHit   ";
  if(vetoCount)   { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& ionEDep   ";
  if(ionDep)      { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& ionEEsc   ";
  if(ionEsc)      { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
  G4cout << "& ionVEDep   ";
  if(ionVDep)     { G4cout << "*** Active" << G4endl; } else { G4cout << "XXX Inactive" << G4endl; }
}


// EOF
