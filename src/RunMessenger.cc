// Unrestricted Use - Property of AECL
//
// RunMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Run Action Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "RunMessenger.hh"			// Specifies the file which contains the class structure

#include "RunAction.hh"				// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4UIdirectory.hh"			// Specifies the classes which contain structures called upon in this class
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UImanager.hh"
#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies class defining all global parameters and variable types

// ****** Constructor ****** //
RunMessenger::RunMessenger(RunAction* run_action)
:  run(run_action)		// Allows retrieval of RunAction to set user-defined values
{
    // Specifies all commands available for EventMessenger.cc
    
    outputDir = new G4UIdirectory("/output/");
    outputDir->SetGuidance("Custom simulation output settings.");
    outputDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    G4UIparameter* param;
    
    outSetCmd = new G4UIcommand("/output/set",this);
    outSetCmd->SetGuidance("Enable/disable requested output table");
    param = new G4UIparameter("name",'s',false);
    param->SetParameterCandidates("nCapEnergy nCapTime nThermTime nCapDist nCapPos rpEnergy nEntryEnergy nEntryPos nEntryAngle opLiquidHit opTravelTime opVetoHit ionEDep ionEEsc ionVEDep");
    outSetCmd->SetParameter(param);
    param = new G4UIparameter("yn",'b',true);
    param->SetDefaultValue(true);
    outSetCmd->SetParameter(param);
    outSetCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    outRawCmd = new G4UIcommand("/output/setRawData",this);
    outRawCmd->SetGuidance("Enable/disable requested raw data output");
    param = new G4UIparameter("raw",'s',false);
    param->SetParameterCandidates("nCapture opLiquid opPhoton opVeto");
    outRawCmd->SetParameter(param);
    param = new G4UIparameter("yn",'b',true);
    param->SetDefaultValue(true);
    outRawCmd->SetParameter(param);
    outRawCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    outNumCmd = new G4UIcommand("/output/setRunNum",this);
    outNumCmd->SetGuidance("Set the run number");
    param = new G4UIparameter("rnum",'i',false);
    param->SetDefaultValue(1);
    outNumCmd->SetParameter(param);
    outNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    evNumCmd = new G4UIcommand("/output/setEvNumOffset",this);
    evNumCmd->SetGuidance("Set the event number offset");
    param = new G4UIparameter("rnum",'i',false);
    param->SetDefaultValue(0);
    evNumCmd->SetParameter(param);
    evNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    G4String testDir = getenv("HOME"); testDir += "/Desktop/";
    outputCmd = new G4UIcmdWithAString("/output/setOutputDirectory",this);
    outputCmd->SetGuidance("Change the home directory for output files");
    outputCmd->SetParameterName("homeDir",true);
    outputCmd->SetDefaultValue(testDir);
    outputCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    G4String rootDef = getenv("PWD");  rootDef += "/SBLMCData.root";
    rootOutputCmd = new G4UIcmdWithAString("/output/setRootOutput",this);
    rootOutputCmd->SetGuidance("set the root output file name and directory");
    rootOutputCmd->SetParameterName("rootFile",true);
    rootOutputCmd->SetDefaultValue(rootDef);
    rootOutputCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    recLevCmd = new G4UIcommand("/output/setRecordLevel",this);
    recLevCmd->SetGuidance("Set the level at which events are recorded:\n0: when a photon is detected by the PMTs\n1: when energy is deposited in the scintillator\n2: When a particle enters the shield\n3: All events");
    param = new G4UIparameter("recL",'i',false);
    param->SetDefaultValue(0);
    recLevCmd->SetParameter(param);
    recLevCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    listCmd = new G4UIcmdWithoutParameter("/output/list",this);
    listCmd->SetGuidance("Lists all output tables and current corresponding status");
    listCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    
    nEnergyDir = 0;
    nEnBinCmd = 0;
    nEnRefCmd = 0;
    nEnNumCmd = 0;
    
    nTimeDir = 0;
    nTmBinCmd = 0;
    nTmRefCmd = 0;
    nTmNumCmd = 0;
    
    nThermDir = 0;
    nThBinCmd = 0;
    nThRefCmd = 0;
    nThNumCmd = 0;
    
    nDistDir = 0;
    nDsBinCmd = 0;
    nDsRefCmd = 0;
    nDsNumCmd = 0;
    
    pEnergyDir = 0;
    pEnBinCmd = 0;
    pEnRefCmd = 0;
    pEnNumCmd = 0;
    
    nEntryDir = 0;
    nEEBinCmd = 0;
    nEERefCmd = 0;
    nEENumCmd = 0;
    
    photHitDir = 0;
    phBinCmd = 0;
    phRefCmd = 0;
    phNumCmd = 0;
    phCutCmd = 0;
    phPMTCmd = 0;
    
    photTimeDir = 0;
    phTBinCmd = 0;
    phTRefCmd = 0;
    phTNumCmd = 0;
    
    vetoHitDir = 0;
    phVBinCmd = 0;
    phVRefCmd = 0;
    phVNumCmd = 0;
    
    ionDepDir = 0;
    iEDBinCmd = 0;
    iEDRefCmd = 0;
    iEDNumCmd = 0;
    
    ionEscDir = 0;
    iEEBinCmd = 0;
    iEERefCmd = 0;
    iEENumCmd = 0;
    
    ionVDepDir = 0;
    iVDBinCmd = 0;
    iVDRefCmd = 0;
    iVDNumCmd = 0;
}

// ****** Destructor ****** //
RunMessenger::~RunMessenger() {
    
    delete outputDir;
    delete outputCmd;
    delete rootOutputCmd;
    delete listCmd;
    
    delete nEnergyDir;
    delete nEnBinCmd;
    delete nEnRefCmd;
    delete nEnNumCmd;
    
    delete nTimeDir;
    delete nTmBinCmd;
    delete nTmRefCmd;
    delete nTmNumCmd;
    
    delete nThermDir;
    delete nThBinCmd;
    delete nThRefCmd;
    delete nThNumCmd;
    
    delete nDistDir;
    delete nDsBinCmd;
    delete nDsRefCmd;
    delete nDsNumCmd;
    
    delete pEnergyDir;
    delete pEnBinCmd;
    delete pEnRefCmd;
    delete pEnNumCmd;
    
    delete nEntryDir;
    delete nEEBinCmd;
    delete nEERefCmd;
    delete nEENumCmd;
    
    delete photHitDir;
    delete phBinCmd;
    delete phRefCmd;
    delete phNumCmd;
    delete phCutCmd;
    delete phPMTCmd;
    
    delete photTimeDir;
    delete phTBinCmd;
    delete phTRefCmd;
    delete phTNumCmd;
    
    delete vetoHitDir;
    delete phVBinCmd;
    delete phVRefCmd;
    delete phVNumCmd;
    
    delete ionDepDir;
    delete iEDBinCmd;
    delete iEDRefCmd;
    delete iEDNumCmd;
    
    delete ionEscDir;
    delete iEEBinCmd;
    delete iEERefCmd;
    delete iEENumCmd;
    
    delete ionVDepDir;
    delete iVDBinCmd;
    delete iVDRefCmd;
    delete iVDNumCmd;
}

// ****** Initialize Neutron Capture Energy Commands ****** //
void RunMessenger::InitializeNEnergyCommands(G4bool set) {
    if(set) {
        nEnergyDir = new G4UIdirectory("/output/nCapEnergy/");
        nEnergyDir->SetGuidance("Neutron capture energy table settings.");
        nEnergyDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEnBinCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapEnergy/setBinWidth",this);
        nEnBinCmd->SetGuidance("Set the bin width of the neutron capture energy table");
        nEnBinCmd->SetGuidance("   Entry of positive value will indicate use of linear scale");
        nEnBinCmd->SetGuidance("   Entry of negative value will indicate use of logarithmic scale");
        nEnBinCmd->SetGuidance("      Logarithmic scale option will use entered number (no attached unit)");
        nEnBinCmd->SetParameterName("nEBin",false);
        nEnBinCmd->SetRange("nEBin!=0.");
        nEnBinCmd->SetDefaultUnit("keV");
        nEnBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEnRefCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapEnergy/setLowerBound",this);
        nEnRefCmd->SetGuidance("Set the lower reference energy of the neutron capture energy table");
        nEnRefCmd->SetGuidance("   Energy value to be converted to log scale after conversion to MeV");
        nEnRefCmd->SetParameterName("nERef",false);
        nEnRefCmd->SetRange("nERef>=0.");
        nEnRefCmd->SetDefaultUnit("keV");
        nEnRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEnNumCmd = new G4UIcmdWithAnInteger("/output/nCapEnergy/setNumberOfBins",this);
        nEnNumCmd->SetGuidance("Set the number of bins in the neutron capture energy table");
        nEnNumCmd->SetParameterName("nENum",false);
        nEnNumCmd->SetRange("nENum>0");
        nEnNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    } else {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(nEnBinCmd) { UI->RemoveCommand(nEnBinCmd); nEnBinCmd = 0; }
        if(nEnRefCmd) { UI->RemoveCommand(nEnRefCmd); nEnRefCmd = 0; }
        if(nEnNumCmd) { UI->RemoveCommand(nEnNumCmd); nEnNumCmd = 0; }
        if(nEnergyDir) { UI->RemoveCommand(nEnergyDir); nEnergyDir = 0; }
    }
}

// ****** Initialize Neutron Capture Time Commands ****** //
void RunMessenger::InitializeNTimeCommands(G4bool set)
{
    if(set)
    {
        nTimeDir = new G4UIdirectory("/output/nCapTime/");
        nTimeDir->SetGuidance("Neutron capture time table settings.");
        nTimeDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nTmBinCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapTime/setBinWidth",this);
        nTmBinCmd->SetGuidance("Set the bin width of the neutron capture time table");
        nTmBinCmd->SetParameterName("tBin",false);
        nTmBinCmd->SetRange("tBin>0.");
        nTmBinCmd->SetDefaultUnit("microsecond");
        nTmBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nTmRefCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapTime/setLowerBound",this);
        nTmRefCmd->SetGuidance("Set the lower reference time of the neutron capture time table");
        nTmRefCmd->SetParameterName("tRef",false);
        nTmRefCmd->SetRange("tRef>=0.");
        nTmRefCmd->SetDefaultUnit("microsecond");
        nTmRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nTmNumCmd = new G4UIcmdWithAnInteger("/output/nCapTime/setNumberOfBins",this);
        nTmNumCmd->SetGuidance("Set the number of bins in the neutron capture time table");
        nTmNumCmd->SetParameterName("tNum",false);
        nTmNumCmd->SetRange("tNum>0");
        nTmNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(nTmBinCmd) { UI->RemoveCommand(nTmBinCmd); nTmBinCmd = 0; }
        if(nTmRefCmd) { UI->RemoveCommand(nTmRefCmd); nTmRefCmd = 0; }
        if(nTmNumCmd) { UI->RemoveCommand(nTmNumCmd); nTmNumCmd = 0; }
        if(nTimeDir) { UI->RemoveCommand(nTimeDir); nTimeDir = 0; }
    }
}

// ****** Initialize Neutron Thermalization Time Commands ****** //
void RunMessenger::InitializeNThermalizationCommands(G4bool set)
{
    if(set)
    {
        nThermDir = new G4UIdirectory("/output/nThermTime/");
        nThermDir->SetGuidance("Neutron thermalization time table settings.");
        nThermDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nThBinCmd = new G4UIcmdWithADoubleAndUnit("/output/nThermTime/setBinWidth",this);
        nThBinCmd->SetGuidance("Set the bin width of the neutron thermalization time table");
        nThBinCmd->SetParameterName("thBin",false);
        nThBinCmd->SetRange("thBin>0.");
        nThBinCmd->SetDefaultUnit("microsecond");
        nThBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nThRefCmd = new G4UIcmdWithADoubleAndUnit("/output/nThermTime/setLowerBound",this);
        nThRefCmd->SetGuidance("Set the lower reference time of the neutron thermalization time table");
        nThRefCmd->SetParameterName("thRef",false);
        nThRefCmd->SetRange("thRef>=0.");
        nThRefCmd->SetDefaultUnit("microsecond");
        nThRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nThNumCmd = new G4UIcmdWithAnInteger("/output/nThermTime/setNumberOfBins",this);
        nThNumCmd->SetGuidance("Set the number of bins in the neutron thermalization time table");
        nThNumCmd->SetParameterName("thNum",false);
        nThNumCmd->SetRange("thNum>0");
        nThNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(nThBinCmd) { UI->RemoveCommand(nThBinCmd); nThBinCmd = 0; }
        if(nThRefCmd) { UI->RemoveCommand(nThRefCmd); nThRefCmd = 0; }
        if(nThNumCmd) { UI->RemoveCommand(nThNumCmd); nThNumCmd = 0; }
        if(nThermDir) { UI->RemoveCommand(nThermDir); nThermDir = 0; }
    }
}

// ****** Initialize Neutron Capture Distance Commands ****** //
void RunMessenger::InitializeNDistanceCommands(G4bool set)
{
    if(set)
    {
        nDistDir = new G4UIdirectory("/output/nCapDist/");
        nDistDir->SetGuidance("Neutron capture distance table settings.");
        nDistDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nDsBinCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapDist/setBinWidth",this);
        nDsBinCmd->SetGuidance("Set the bin width of the neutron capture distance table");
        nDsBinCmd->SetParameterName("dBin",false);
        nDsBinCmd->SetRange("dBin>0.");
        nDsBinCmd->SetDefaultUnit("mm");
        nDsBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nDsRefCmd = new G4UIcmdWithADoubleAndUnit("/output/nCapDist/setLowerBound",this);
        nDsRefCmd->SetGuidance("Set the lower reference time of the neutron capture distance table");
        nDsRefCmd->SetParameterName("dRef",false);
        nDsRefCmd->SetRange("dRef>=0.");
        nDsRefCmd->SetDefaultUnit("mm");
        nDsRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nDsNumCmd = new G4UIcmdWithAnInteger("/output/nCapDist/setNumberOfBins",this);
        nDsNumCmd->SetGuidance("Set the number of bins in the neutron capture distance table");
        nDsNumCmd->SetParameterName("dNum",false);
        nDsNumCmd->SetRange("dNum>0");
        nDsNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(nDsBinCmd) { UI->RemoveCommand(nDsBinCmd); nDsBinCmd = 0; }
        if(nDsRefCmd) { UI->RemoveCommand(nDsRefCmd); nDsRefCmd = 0; }
        if(nDsNumCmd) { UI->RemoveCommand(nDsNumCmd); nDsNumCmd = 0; }
        if(nDistDir) { UI->RemoveCommand(nDistDir); nDistDir = 0; }
    }
}

// ****** Initialize Recoil Proton Energy Commands ****** //
void RunMessenger::InitializePEnergyCommands(G4bool set)
{
    if(set)
    {
        pEnergyDir = new G4UIdirectory("/output/rpEnergy/");
        pEnergyDir->SetGuidance("Recoil proton energy table settings.");
        pEnergyDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        pEnBinCmd = new G4UIcmdWithADoubleAndUnit("/output/rpEnergy/setBinWidth",this);
        pEnBinCmd->SetGuidance("Set the bin width of the recoil proton energy table");
        pEnBinCmd->SetParameterName("pEBin",false);
        pEnBinCmd->SetRange("pEBin>0.");
        pEnBinCmd->SetDefaultUnit("keV");
        pEnBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        pEnRefCmd = new G4UIcmdWithADoubleAndUnit("/output/rpEnergy/setLowerBound",this);
        pEnRefCmd->SetGuidance("Set the lower reference time of the recoil proton energy table");
        pEnRefCmd->SetParameterName("pERef",false);
        pEnRefCmd->SetRange("pERef>=0.");
        pEnRefCmd->SetDefaultUnit("keV");
        pEnRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        pEnNumCmd = new G4UIcmdWithAnInteger("/output/rpEnergy/setNumberOfBins",this);
        pEnNumCmd->SetGuidance("Set the number of bins in the recoil proton energy table");
        pEnNumCmd->SetParameterName("pENum",false);
        pEnNumCmd->SetRange("pENum>0");
        pEnNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(pEnBinCmd) { UI->RemoveCommand(pEnBinCmd); pEnBinCmd = 0; }
        if(pEnRefCmd) { UI->RemoveCommand(pEnRefCmd); pEnRefCmd = 0; }
        if(pEnNumCmd) { UI->RemoveCommand(pEnNumCmd); pEnNumCmd = 0; }
        if(pEnergyDir) { UI->RemoveCommand(pEnergyDir); pEnergyDir = 0; }
    }
}

// ****** Initialize Neutron Entry Energy Commands ****** //
void RunMessenger::InitializeEntryEnergyCommands(G4bool set)
{
    if(set)
    {
        nEntryDir = new G4UIdirectory("/output/nEntryEnergy/");
        nEntryDir->SetGuidance("Neutron entry energy table settings.");
        nEntryDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEEBinCmd = new G4UIcmdWithADoubleAndUnit("/output/nEntryEnergy/setBinWidth",this);
        nEEBinCmd->SetGuidance("Set the bin width of the neutron entry energy table");
        nEEBinCmd->SetGuidance("   Entry of positive value will indicate use of linear scale");
        nEEBinCmd->SetGuidance("   Entry of negative value will indicate use of logarithmic scale");
        nEEBinCmd->SetGuidance("      Logarithmic scale option will use entered number (no attached unit)");
        nEEBinCmd->SetParameterName("enBin",false);
        nEEBinCmd->SetRange("enBin!=0.");
        nEEBinCmd->SetDefaultUnit("keV");
        nEEBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEERefCmd = new G4UIcmdWithADoubleAndUnit("/output/nEntryEnergy/setLowerBound",this);
        nEERefCmd->SetGuidance("Set the lower reference time of the neutron entry energy table");
        nEERefCmd->SetGuidance("   Energy value to be converted to log scale after conversion to MeV");
        nEERefCmd->SetParameterName("enRef",false);
        nEERefCmd->SetRange("enRef>=0.");
        nEERefCmd->SetDefaultUnit("keV");
        nEERefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        nEENumCmd = new G4UIcmdWithAnInteger("/output/nEntryEnergy/setNumberOfBins",this);
        nEENumCmd->SetGuidance("Set the number of bins in the neutron entry energy table");
        nEENumCmd->SetParameterName("enNum",false);
        nEENumCmd->SetRange("enNum>0");
        nEENumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(nEEBinCmd) { UI->RemoveCommand(nEEBinCmd); nEEBinCmd = 0; }
        if(nEERefCmd) { UI->RemoveCommand(nEERefCmd); nEERefCmd = 0; }
        if(nEENumCmd) { UI->RemoveCommand(nEENumCmd); nEENumCmd = 0; }
        if(nEntryDir) { UI->RemoveCommand(nEntryDir); nEntryDir = 0; }
    }
}

// ****** Initialize Liquid Scintillator Photon Count Commands ****** //
void RunMessenger::InitializeLSPhotonHitCommands(G4bool set)
{
    if(set)
    {
        photHitDir = new G4UIdirectory("/output/opLiquidHit/");
        photHitDir->SetGuidance("Optical photon hit per event table settings.");
        photHitDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phBinCmd = new G4UIcmdWithAnInteger("/output/opLiquidHit/setBinWidth",this);
        phBinCmd->SetGuidance("Set the bin width of the optical photon hit table");
        phBinCmd->SetParameterName("lBin",false);
        phBinCmd->SetRange("lBin>0");
        phBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phRefCmd = new G4UIcmdWithAnInteger("/output/opLiquidHit/setLowerBound",this);
        phRefCmd->SetGuidance("Set the lower reference time of the optical photon hit table");
        phRefCmd->SetParameterName("lRef",false);
        phRefCmd->SetRange("lRef>=0");
        phRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phNumCmd = new G4UIcmdWithAnInteger("/output/opLiquidHit/setNumberOfBins",this);
        phNumCmd->SetGuidance("Set the number of bins in the optical photon hit table");
        phNumCmd->SetParameterName("lNum",false);
        phNumCmd->SetRange("lNum>0");
        phNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phCutCmd = new G4UIcmdWithADoubleAndUnit("/output/opLiquidHit/setSeparationTime",this);
        phCutCmd->SetGuidance("Set the event separation time cut - data will be split into above and below the cut");
        phCutCmd->SetParameterName("lCut",false);
        phCutCmd->SetRange("lCut>0.");
        phCutCmd->SetDefaultUnit("microsecond");
        phCutCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phPMTCmd = new G4UIcmdWithABool("/output/opLiquidHit/setSegregationByPMT",this);
        phPMTCmd->SetGuidance("Set output segregation by PMT number");
        phPMTCmd->SetParameterName("lSeg",false);
        phPMTCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(phBinCmd) { UI->RemoveCommand(phBinCmd); phBinCmd = 0; }
        if(phRefCmd) { UI->RemoveCommand(phRefCmd); phRefCmd = 0; }
        if(phNumCmd) { UI->RemoveCommand(phNumCmd); phNumCmd = 0; }
        if(phCutCmd) { UI->RemoveCommand(phCutCmd); phCutCmd = 0; }
        if(phPMTCmd) { UI->RemoveCommand(phPMTCmd); phPMTCmd = 0; }
        if(photHitDir) { UI->RemoveCommand(photHitDir); photHitDir = 0; }
    }
}

// ****** Initialize Photon Travel Time Commands ****** //
void RunMessenger::InitializePhotonTravelTimeCommands(G4bool set)
{
    if(set)
    {
        photTimeDir = new G4UIdirectory("/output/opTravelTime/");
        photTimeDir->SetGuidance("Photon travel time table settings.");
        photTimeDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phTBinCmd = new G4UIcmdWithADoubleAndUnit("/output/opTravelTime/setBinWidth",this);
        phTBinCmd->SetGuidance("Set the bin width of the photon travel time table");
        phTBinCmd->SetParameterName("tBin",false);
        phTBinCmd->SetRange("tBin>0.");
        phTBinCmd->SetDefaultUnit("microsecond");
        phTBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phTRefCmd = new G4UIcmdWithADoubleAndUnit("/output/opTravelTime/setLowerBound",this);
        phTRefCmd->SetGuidance("Set the lower reference time of the photon travel time table");
        phTRefCmd->SetParameterName("tRef",false);
        phTRefCmd->SetRange("tRef>=0.");
        phTRefCmd->SetDefaultUnit("microsecond");
        phTRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phTNumCmd = new G4UIcmdWithAnInteger("/output/opTravelTime/setNumberOfBins",this);
        phTNumCmd->SetGuidance("Set the number of bins in the neutron capture distance table");
        phTNumCmd->SetParameterName("tNum",false);
        phTNumCmd->SetRange("tNum>0");
        phTNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(phTBinCmd) { UI->RemoveCommand(phTBinCmd); phTBinCmd = 0; }
        if(phTRefCmd) { UI->RemoveCommand(phTRefCmd); phTRefCmd = 0; }
        if(phTNumCmd) { UI->RemoveCommand(phTNumCmd); phTNumCmd = 0; }
        if(photTimeDir) { UI->RemoveCommand(photTimeDir); photTimeDir = 0; }
    }
}

// ****** Initialize Veto Panel Photon Count Commands ****** //
void RunMessenger::InitializeVetoPhotonHitCommands(G4bool set)
{
    if(set)
    {
        vetoHitDir = new G4UIdirectory("/output/opVetoHit/");
        vetoHitDir->SetGuidance("Veto optical photon hit per event table settings.");
        vetoHitDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phVBinCmd = new G4UIcmdWithAnInteger("/output/opVetoHit/setBinWidth",this);
        phVBinCmd->SetGuidance("Set the bin width of the veto optical photon hit table");
        phVBinCmd->SetParameterName("vBin",false);
        phVBinCmd->SetRange("vBin>0");
        phVBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phVRefCmd = new G4UIcmdWithAnInteger("/output/opVetoHit/setLowerBound",this);
        phVRefCmd->SetGuidance("Set the lower reference time of the veto optical photon hit table");
        phVRefCmd->SetParameterName("vRef",false);
        phVRefCmd->SetRange("vRef>=0");
        phVRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        phVNumCmd = new G4UIcmdWithAnInteger("/output/opVetoHit/setNumberOfBins",this);
        phVNumCmd->SetGuidance("Set the number of bins in the veto optical photon hit table");
        phVNumCmd->SetParameterName("vNum",false);
        phVNumCmd->SetRange("vNum>0");
        phVNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(phVBinCmd) { UI->RemoveCommand(phVBinCmd); phVBinCmd = 0; }
        if(phVRefCmd) { UI->RemoveCommand(phVRefCmd); phVRefCmd = 0; }
        if(phVNumCmd) { UI->RemoveCommand(phVNumCmd); phVNumCmd = 0; }
        if(vetoHitDir) { UI->RemoveCommand(vetoHitDir); vetoHitDir = 0; }
    }
}

// ****** Initialize Ionisation Energy Deposit Commands ****** //
void RunMessenger::InitializeIonEnergyDepositCommands(G4bool set)
{
    if(set)
    {
        ionDepDir = new G4UIdirectory("/output/ionEDep/");
        ionDepDir->SetGuidance("Ionisation energy deposit table settings.");
        ionDepDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEDBinCmd = new G4UIcmdWithADoubleAndUnit("/output/ionEDep/setBinWidth",this);
        iEDBinCmd->SetGuidance("Set the bin width of the ionisation energy deposit table");
        iEDBinCmd->SetParameterName("iEDBin",false);
        iEDBinCmd->SetRange("iEDBin>0.");
        iEDBinCmd->SetDefaultUnit("keV");
        iEDBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEDRefCmd = new G4UIcmdWithADoubleAndUnit("/output/ionEDep/setLowerBound",this);
        iEDRefCmd->SetGuidance("Set the lower reference time of the ionisation energy deposit table");
        iEDRefCmd->SetParameterName("iEDRef",false);
        iEDRefCmd->SetRange("iEDRef>=0.");
        iEDRefCmd->SetDefaultUnit("keV");
        iEDRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEDNumCmd = new G4UIcmdWithAnInteger("/output/ionEDep/setNumberOfBins",this);
        iEDNumCmd->SetGuidance("Set the number of bins in the ionisation energy deposit table");
        iEDNumCmd->SetParameterName("iEDNum",false);
        iEDNumCmd->SetRange("iEDNum>0");
        iEDNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(iEDBinCmd) { UI->RemoveCommand(iEDBinCmd); iEDBinCmd = 0; }
        if(iEDRefCmd) { UI->RemoveCommand(iEDRefCmd); iEDRefCmd = 0; }
        if(iEDNumCmd) { UI->RemoveCommand(iEDNumCmd); iEDNumCmd = 0; }
        if(ionDepDir) { UI->RemoveCommand(ionDepDir); ionDepDir = 0; }
    }
}

// ****** Initialize Ionisation Energy Escape Commands ****** //
void RunMessenger::InitializeIonEnergyEscapeCommands(G4bool set)
{
    if(set)
    {
        ionEscDir = new G4UIdirectory("/output/ionEEsc/");
        ionEscDir->SetGuidance("Ionisation energy energy table settings.");
        ionEscDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEEBinCmd = new G4UIcmdWithADoubleAndUnit("/output/ionEEsc/setBinWidth",this);
        iEEBinCmd->SetGuidance("Set the bin width of the ionisation energy escape table");
        iEEBinCmd->SetParameterName("iEEBin",false);
        iEEBinCmd->SetRange("iEEBin>0.");
        iEEBinCmd->SetDefaultUnit("keV");
        iEEBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEERefCmd = new G4UIcmdWithADoubleAndUnit("/output/ionEEsc/setLowerBound",this);
        iEERefCmd->SetGuidance("Set the lower reference time of the ionisation energy escape table");
        iEERefCmd->SetParameterName("iEERef",false);
        iEERefCmd->SetRange("iEERef>=0.");
        iEERefCmd->SetDefaultUnit("keV");
        iEERefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iEENumCmd = new G4UIcmdWithAnInteger("/output/ionEEsc/setNumberOfBins",this);
        iEENumCmd->SetGuidance("Set the number of bins in the ionisation energy escape table");
        iEENumCmd->SetParameterName("iEENum",false);
        iEENumCmd->SetRange("iEENum>0");
        iEENumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(iEDBinCmd) { UI->RemoveCommand(iEDBinCmd); iEDBinCmd = 0; }
        if(iEDRefCmd) { UI->RemoveCommand(iEDRefCmd); iEDRefCmd = 0; }
        if(iEDNumCmd) { UI->RemoveCommand(iEDNumCmd); iEDNumCmd = 0; }
        if(ionDepDir) { UI->RemoveCommand(ionDepDir); ionDepDir = 0; }
    }
}

// ****** Initialize Veto Panel Ionisation Energy Deposit Commands ****** //
void RunMessenger::InitializeVetoEnergyDepositCommands(G4bool set)
{
    if(set)
    {
        ionVDepDir = new G4UIdirectory("/output/ionVEDep/");
        ionVDepDir->SetGuidance("Veto ionisation energy deposit table settings.");
        ionVDepDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iVDBinCmd = new G4UIcmdWithADoubleAndUnit("/output/ionVEDep/setBinWidth",this);
        iVDBinCmd->SetGuidance("Set the bin width of the ionisation energy deposit table");
        iVDBinCmd->SetParameterName("iVDBin",false);
        iVDBinCmd->SetRange("iVDBin>0.");
        iVDBinCmd->SetDefaultUnit("MeV");
        iVDBinCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iVDRefCmd = new G4UIcmdWithADoubleAndUnit("/output/ionVEDep/setLowerBound",this);
        iVDRefCmd->SetGuidance("Set the lower reference time of the ionisation energy deposit table");
        iVDRefCmd->SetParameterName("iVDRef",false);
        iVDRefCmd->SetRange("iVDRef>=0.");
        iVDRefCmd->SetDefaultUnit("MeV");
        iVDRefCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
        
        iVDNumCmd = new G4UIcmdWithAnInteger("/output/ionVEDep/setNumberOfBins",this);
        iVDNumCmd->SetGuidance("Set the number of bins in the ionisation energy deposit table");
        iVDNumCmd->SetParameterName("iVDNum",false);
        iVDNumCmd->SetRange("iVDNum>0");
        iVDNumCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle,G4State_GeomClosed);
    }
    else
    {
        G4UImanager* UI = G4UImanager::GetUIpointer();
        if(iVDBinCmd) { UI->RemoveCommand(iVDBinCmd); iVDBinCmd = 0; }
        if(iVDRefCmd) { UI->RemoveCommand(iVDRefCmd); iVDRefCmd = 0; }
        if(iVDNumCmd) { UI->RemoveCommand(iVDNumCmd); iVDNumCmd = 0; }
        if(ionVDepDir) { UI->RemoveCommand(ionVDepDir); ionVDepDir = 0; }
    }
}

void RunMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    
    if(command == outNumCmd) {
        G4Tokenizer next(newValue);		// Tokenizer splits input string into delimited sections
        G4int rnum = StoI(next()); 
        run->SetRunNumber(rnum); 
        G4cout << "Run number is " <<rnum<< G4endl;
    } else if(command == evNumCmd) {
        G4Tokenizer next(newValue);		// Tokenizer splits input string into delimited sections
        G4int rnum = StoI(next()); 
        run->SetEventNumberOffset(rnum); 
        G4cout << "Event number offset is " <<rnum<< G4endl;
    } else if(command == recLevCmd) {
        G4Tokenizer next(newValue);		// Tokenizer splits input string into delimited sections
        G4int recL = StoI(next()); 
        run->SetRecordLevel(recL); 
        G4cout << "Record Level is " <<recL<< G4endl;
    } else if(command == outSetCmd) {
        G4Tokenizer next(newValue);
        G4String output = next(); G4bool condition = StoB(next());
        if(output == "nCapEnergy") {
            run->SetNeutronCaptureEnergyTable(condition);
            InitializeNEnergyCommands(condition);
        } else if(output == "nCapTime") {
            run->SetNeutronCaptureTimeTable(condition);
            InitializeNTimeCommands(condition);
        } else if(output == "nThermTime") {
            run->SetNeutronThermalizationTimeTable(condition);
            InitializeNThermalizationCommands(condition);
        } else if(output == "nCapDist") {
            run->SetNeutronCaptureDistanceTable(condition);
            InitializeNDistanceCommands(condition);
        } else if(output == "nCapPos") {
            run->SetNeutronCapturePositionTable(condition);
        } else if(output == "rpEnergy") {
            run->SetRecoilProtonEnergyTable(condition);
            InitializePEnergyCommands(condition);
        } else if(output == "nEntryEnergy") {
            run->SetNeutronEntryEnergyTable(condition);
            InitializeEntryEnergyCommands(condition);
        }  else if(output == "nEntryPos") {
            run->SetNeutronEntryPositionTable(condition);
        } else if(output == "nEntryAngle") {
            run->SetNeutronEntryAngleTable(condition);
        } else if(output == "opLiquidHit") {
            run->SetLSPhotonCountTable(condition);
            InitializeLSPhotonHitCommands(condition);
        } else if(output == "opTravelTime") {
            run->SetPhotonTravelTimeTable(condition);
            InitializePhotonTravelTimeCommands(condition);
        } else if(output == "opVetoHit") {
            run->SetVetoPanelCountTable(condition);
            InitializeVetoPhotonHitCommands(condition);
        } else if(output == "ionEDep") {
            run->SetIonisationDepositTable(condition);
            InitializeIonEnergyDepositCommands(condition);
        } else if(output == "ionEEsc") {
            run->SetIonisationEscapeTable(condition);
            InitializeIonEnergyEscapeCommands(condition);
        } else if(output == "ionVEDep") {
            run->SetVetoIonisationDepositTable(condition);
            InitializeVetoEnergyDepositCommands(condition);
        }
    } else if(command == outRawCmd) {
        G4Tokenizer next(newValue);
        G4String output = next(); G4bool condition = StoB(next());
        if(output == "nCapture") run->SetNeutronCaptureRawDataOutput(condition);
        else if(output == "opPhoton") run->SetOpticalPhotonRawDataOutput(condition);
        else if(output == "opLiquid") run->SetLSPhotonCountRawDataOutput(condition);
        else if(output == "opVeto") run->SetVetoPanelCountRawDataOutput(condition);
    } else if(command == outputCmd) {
        LogSession* log = LogSession::GetLogSessionPointer();
        log->SetHomeDirectory(newValue);
    } else if(command == rootOutputCmd) {
        RootIO* rr = RootIO::GetInstance();
        rr->SetFileName(newValue);
    } else if(command == listCmd){
        run->PrintOutputList();
    } else if(command == nEnBinCmd) {
        if(nEnBinCmd->GetNewDoubleValue(newValue) < 0.) {
            run->SetNeutronCaptureEnergyBinWidth(std::fabs(nEnBinCmd->GetNewDoubleRawValue(newValue)));
            run->SetNeutronCaptureEnergyLogScale(true);
        } else {
            run->SetNeutronCaptureEnergyBinWidth(nEnBinCmd->GetNewDoubleValue(newValue));
            run->SetNeutronCaptureEnergyLogScale(false);
        }
    } else if(command == nEnRefCmd) run->SetNeutronCaptureEnergyLowerRef(nEnRefCmd->GetNewDoubleValue(newValue));
    else if(command == nEnNumCmd) run->SetNeutronCaptureEnergyBinCount(nEnNumCmd->GetNewIntValue(newValue));
    else if(command == nTmBinCmd) run->SetNeutronCaptureTimeBinWidth(nTmBinCmd->GetNewDoubleValue(newValue));
    else if(command == nTmRefCmd) run->SetNeutronCaptureTimeLowerRef(nTmRefCmd->GetNewDoubleValue(newValue));
    else if(command == nTmNumCmd) run->SetNeutronCaptureTimeBinCount(nTmNumCmd->GetNewIntValue(newValue));
    else if(command == nThBinCmd) run->SetNeutronThermalizationTimeBinWidth(nThBinCmd->GetNewDoubleValue(newValue));
    else if(command == nThRefCmd) run->SetNeutronThermalizationTimeLowerRef(nThRefCmd->GetNewDoubleValue(newValue));
    else if(command == nThNumCmd) run->SetNeutronThermalizationTimeBinCount(nThNumCmd->GetNewIntValue(newValue));
    else if(command == nDsBinCmd) run->SetNeutronCaptureDistanceBinWidth(nDsBinCmd->GetNewDoubleValue(newValue));
    else if(command == nDsRefCmd) run->SetNeutronCaptureDistanceLowerRef(nDsBinCmd->GetNewDoubleValue(newValue));
    else if(command == nDsNumCmd) run->SetNeutronCaptureDistanceBinCount(nDsNumCmd->GetNewIntValue(newValue));
    else if(command == pEnBinCmd) run->SetRecoilProtonEnergyBinWidth(pEnBinCmd->GetNewDoubleValue(newValue));
    else if(command == pEnRefCmd) run->SetRecoilProtonEnergyLowerRef(pEnRefCmd->GetNewDoubleValue(newValue));
    else if(command == pEnNumCmd) run->SetRecoilProtonEnergyBinCount(pEnNumCmd->GetNewIntValue(newValue));
    else if(command == nEEBinCmd) {
        if(nEnBinCmd->GetNewDoubleValue(newValue) < 0.) {
            run->SetNeutronEntryEnergyBinWidth(std::fabs(nEEBinCmd->GetNewDoubleRawValue(newValue)));
            run->SetNeutronEntryEnergyLogScale(true);
        } else {
            run->SetNeutronEntryEnergyBinWidth(nEEBinCmd->GetNewDoubleValue(newValue));
            run->SetNeutronEntryEnergyLogScale(false);
        }
    } else if(command == nEERefCmd) run->SetNeutronEntryEnergyLowerRef(nEERefCmd->GetNewDoubleValue(newValue));
    else if(command == nEENumCmd) run->SetNeutronEntryEnergyBinCount(nEENumCmd->GetNewIntValue(newValue));
    else if(command == phBinCmd) run->SetLSPhotonCountBinWidth(phBinCmd->GetNewIntValue(newValue));
    else if(command == phRefCmd) run->SetLSPhotonCountLowerRef(phRefCmd->GetNewIntValue(newValue));
    else if(command == phNumCmd) run->SetLSPhotonCountBinCount(phNumCmd->GetNewIntValue(newValue));
    else if(command == phCutCmd) run->SetLSPhotonCountEventCut(phCutCmd->GetNewDoubleValue(newValue));
    else if(command == phPMTCmd) run->SetLSPhotonCountPMTSegregation(phPMTCmd->GetNewBoolValue(newValue));
    else if(command == phTBinCmd) run->SetPhotonTravelTimeBinWidth(phTBinCmd->GetNewDoubleValue(newValue));
    else if(command == phTRefCmd) run->SetPhotonTravelTimeLowerRef(phTRefCmd->GetNewDoubleValue(newValue));
    else if(command == phTNumCmd) run->SetPhotonTravelTimeBinCount(phTNumCmd->GetNewIntValue(newValue));
    else if(command == phVBinCmd) run->SetVetoPanelCountBinWidth(phVBinCmd->GetNewIntValue(newValue));
    else if(command == phVRefCmd) run->SetVetoPanelCountLowerRef(phVRefCmd->GetNewIntValue(newValue));
    else if(command == phVNumCmd) run->SetVetoPanelCountBinCount(phVNumCmd->GetNewIntValue(newValue));
    else if(command == iEDBinCmd) run->SetIonisationDepositBinWidth(iEDBinCmd->GetNewDoubleValue(newValue));
    else if(command == iEDRefCmd) run->SetIonisationDepositLowerRef(iEDRefCmd->GetNewDoubleValue(newValue));
    else if(command == iEDNumCmd) run->SetIonisationDepositBinCount(iEDNumCmd->GetNewIntValue(newValue));
    else if(command == iEEBinCmd) run->SetIonisationEscapeBinWidth(iEEBinCmd->GetNewDoubleValue(newValue));
    else if(command == iEERefCmd) run->SetIonisationEscapeLowerRef(iEERefCmd->GetNewDoubleValue(newValue));
    else if(command == iEENumCmd) run->SetIonisationEscapeBinCount(iEENumCmd->GetNewIntValue(newValue));
    else if(command == iVDBinCmd) run->SetVetoIonisationDepositBinWidth(iVDBinCmd->GetNewDoubleValue(newValue));
    else if(command == iVDRefCmd) run->SetVetoIonisationDepositLowerRef(iVDRefCmd->GetNewDoubleValue(newValue));
    else if(command == iVDNumCmd) run->SetVetoIonisationDepositBinCount(iVDNumCmd->GetNewIntValue(newValue));
    else G4cout << "Command not found." << G4endl;
}

G4String RunMessenger::GetCurrentValue(G4UIcommand*) {
    G4String cv;
    cv = "Command not implemented.";
    return cv;
}

void RunMessenger::ResetOutputCommands() {
    InitializeNEnergyCommands(false); 
    InitializeNTimeCommands(false); 
    InitializeNThermalizationCommands(false); 
    InitializeNDistanceCommands(false); 
    InitializePEnergyCommands(false); 
    InitializeEntryEnergyCommands(false); 
    InitializeLSPhotonHitCommands(false); 
    InitializePhotonTravelTimeCommands(false);
    InitializeVetoPhotonHitCommands(false); 
    InitializeIonEnergyDepositCommands(false); 
    InitializeIonEnergyEscapeCommands(false); 
    InitializeVetoEnergyDepositCommands(false); 
}
