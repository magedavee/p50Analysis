// Unrestricted Use - Property of AECL
//
// RunAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Run Controller and Processing
// 	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef RunAction_H		// Only carries out if object is undefined
#define RunAction_H 1		// Defines object

#include "G4UserRunAction.hh"	// Specifies base class or parent class

#include <map>			// Specifies classes defining all global parameters and variable types
#include <vector>
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4Run;			// Structures necessary for class definition
class RunMessenger;
class PrimaryGeneratorAction;

/* -------- Class Definition --------- */

class RunAction : public G4UserRunAction	// This class inherits functions from G4UserRunAction
{
  friend class RunMessenger;

  public:	// Constructors and Destructors

    RunAction();			// Constructor
    virtual ~RunAction();		// Destructor

  public:	// Accessible Methods

    void BeginOfRunAction(const G4Run*);		// Called at the beginning of a run (BeamOn) to instantiate variables/objects
    void EndOfRunAction(const G4Run*);			// Called at the end of a run to calculate required values - fluxes in this simulation

    void AddNCount(G4int);				// Allows passing of cumulative statistics from other classes
    void AddNCapCount(G4int);
    void AddPCount(G4int);

    void PassNCEnergy(G4double);			// Allows passing of histogram statistics from other classes
    void PassNCTime(G4double);
    void PassNThermTime(G4double);
    void PassNCDistance(G4double);
    void PassNEnergy(G4double);
    void PassNZenithAngle(G4double);
    void PassGammasCreated(G4int);
    void PassEventTiming(std::map<G4double,G4int>*);
    void PassPhotonCount(G4int,G4int);
    void PassVetoCount(G4int);
    void PassProtonEnergy(G4double);
    void PassIonEDeposit(G4double);
    void PassIonEEscape(G4double);
    void PassIonVDeposit(G4double);
    void PassMuEnergy(G4double);
    void WriteNCPosition(G4ThreeVector,G4int);
    void WriteNEPosition(G4ThreeVector,G4int);
    void PassSeparationTime(G4double t) { EvtSeparationBin = t; };

    void SetFilePrefix(G4String label) { prefix = label; };	// Get/set function for output file labels to quickly identify critical runs
    void SetFileSuffix(G4String label) { suffix = label; };
    G4String GetFilePrefix() const { return prefix; };
    G4String GetFileSuffix() const { return suffix; };

    G4bool GetNeutronCaptureEnergyLogScale() const { return nEnergyScale; };
    G4bool GetNeutronEntryEnergyLogScale() const { return nEntryScale; };
    G4bool GetNeutronCaptureRawDataOutput() const { return fRawNeutronCapture; };
    G4bool GetOpticalPhotonRawDataOutput() const { return fRawOpticalPhoton; };
    G4double GetLSPhotonCountEventCut() const { return EvtSeparationBin; };
    G4bool GetLSPhotonCountRawDataOutput() const { return fRawPhotonCount; };
    G4bool GetVetoPanelCountRawDataOutput() const { return fRawVetoCount; };

    G4double GetPhotonTravelTimeBinWidth() const { return travelBin; };		// Return photon travel time output table
    G4double GetPhotonTravelTimeLowerRef() const { return travelRef; };
    G4double GetPhotonTravelTimeBinCount() const { return travelNum; };

  G4int GetRunNumber(){return nRunNumber ;};
  G4int GetEventNumberOffset(){return nEvNumOffset ;};
  G4int GetRecordLevel(){return nRecLev ;};


    void PrintCurrentData();				// Data output functions
    void PrintOutputList() const;


  protected:	// Internal Methods

    void SetNeutronCaptureEnergyTable(G4bool);			// These functions activate the various output tables in the simulation
    void SetNeutronCaptureTimeTable(G4bool);
    void SetNeutronThermalizationTimeTable(G4bool);
    void SetNeutronCaptureDistanceTable(G4bool);
    void SetNeutronCapturePositionTable(G4bool);
    void SetNeutronCaptureRawDataOutput(G4bool);
    void SetOpticalPhotonRawDataOutput(G4bool);
    void SetRecoilProtonEnergyTable(G4bool);
    void SetNeutronEntryEnergyTable(G4bool);
    void SetNeutronEntryPositionTable(G4bool);
    void SetNeutronEntryAngleTable(G4bool);
    void SetLSPhotonCountTable(G4bool);
    void SetLSPhotonCountRawDataOutput(G4bool);
    void SetPhotonTravelTimeTable(G4bool);
    void SetVetoPanelCountTable(G4bool);
    void SetVetoPanelCountRawDataOutput(G4bool);
    void SetIonisationDepositTable(G4bool);
    void SetIonisationEscapeTable(G4bool);
    void SetVetoIonisationDepositTable(G4bool);
//    void SetMuonEntryEnergyTable(G4bool);

	// The following functions modify the output tables to display data with desired resolution

  void SetRunNumber(G4int rnum){nRunNumber = rnum;};
  void SetEventNumberOffset(G4int rnum){nEvNumOffset = rnum;};
  void SetRecordLevel(G4int rnum){nRecLev = rnum;};

    void SetNeutronCaptureEnergyBinWidth(G4double bin) { nEnergyBin = bin; };
    void SetNeutronCaptureEnergyLowerRef(G4double ref) { nEnergyRef = ref; };
    void SetNeutronCaptureEnergyBinCount(G4int num) { nEnergyNum = num; };
    void SetNeutronCaptureEnergyLogScale(G4bool log) { nEnergyScale = log; };

    void SetNeutronCaptureTimeBinWidth(G4double bin) { nTimeBin = bin; };
    void SetNeutronCaptureTimeLowerRef(G4double ref) { nTimeRef = ref; };
    void SetNeutronCaptureTimeBinCount(G4int num) { nTimeNum = num; };

    void SetNeutronThermalizationTimeBinWidth(G4double bin) { nThermBin = bin; };
    void SetNeutronThermalizationTimeLowerRef(G4double ref) { nThermRef = ref; };
    void SetNeutronThermalizationTimeBinCount(G4int num) { nThermNum = num; };

    void SetNeutronCaptureDistanceBinWidth(G4double bin) { nDistBin = bin; };
    void SetNeutronCaptureDistanceLowerRef(G4double ref) { nDistRef = ref; };
    void SetNeutronCaptureDistanceBinCount(G4int num) { nDistNum = num; };

    void SetRecoilProtonEnergyBinWidth(G4double bin) { pEnergyBin = bin; };
    void SetRecoilProtonEnergyLowerRef(G4double ref) { pEnergyRef = ref; };
    void SetRecoilProtonEnergyBinCount(G4int num) { pEnergyNum = num; };

    void SetNeutronEntryEnergyBinWidth(G4double bin) { nEntryBin = bin; };
    void SetNeutronEntryEnergyLowerRef(G4double ref) { nEntryRef = ref; };
    void SetNeutronEntryEnergyBinCount(G4int num) { nEntryNum = num; };
    void SetNeutronEntryEnergyLogScale(G4bool log) { nEntryScale = log; };

    void SetLSPhotonCountBinWidth(G4int bin) { photCountBin = bin; };
    void SetLSPhotonCountLowerRef(G4int ref) { photCountRef = ref; };
    void SetLSPhotonCountBinCount(G4int num) { photCountNum = num; };
    void SetLSPhotonCountEventCut(G4double);
    void SetLSPhotonCountPMTSegregation(G4bool seg) { fSegregation = seg; };

    void SetPhotonTravelTimeBinWidth(G4double bin) { travelBin = bin; };
    void SetPhotonTravelTimeLowerRef(G4double ref) { travelRef = ref; };
    void SetPhotonTravelTimeBinCount(G4int num) { travelNum = num; };

    void SetVetoPanelCountBinWidth(G4int bin) { vetoCountBin = bin; };
    void SetVetoPanelCountLowerRef(G4int ref) { vetoCountRef = ref; };
    void SetVetoPanelCountBinCount(G4int num) { vetoCountNum = num; };

    void SetIonisationDepositBinWidth(G4double bin) { ionDepBin = bin; };
    void SetIonisationDepositLowerRef(G4double ref) { ionDepRef = ref; };
    void SetIonisationDepositBinCount(G4int num) { ionDepNum = num; };

    void SetIonisationEscapeBinWidth(G4double bin) { ionEscBin = bin; };
    void SetIonisationEscapeLowerRef(G4double ref) { ionEscRef = ref; };
    void SetIonisationEscapeBinCount(G4int num) { ionEscNum = num; };

    void SetVetoIonisationDepositBinWidth(G4double bin) { ionVDepBin = bin; };
    void SetVetoIonisationDepositLowerRef(G4double ref) { ionVDepRef = ref; };
    void SetVetoIonisationDepositBinCount(G4int num) { ionVDepNum = num; };
    
   private:	// Member Data
    
    G4int n_count;		// Overall run data storage
    G4int ncap_count;
    G4int pmtHit;
    G4int sumGamma;
    G4int p_count;
    G4double EvtSeparationBin;

    std::map<G4double,G4int>* nEnergy;		// Table storage pointers
    std::map<G4double,G4int>* nTime;
    std::map<G4double,G4int>* nTherm;
    std::map<G4double,G4int>* nDist;
    std::map<G4double,G4int>* pEnergy;
    std::map<G4double,G4int>* nEntry;
    std::map<G4double,G4int>* nZenith;
    std::map<G4int,G4int>* photCount;
    std::map<G4int,G4int>* photCount2;
    std::map<G4double,G4int>* RunTime;
    std::map<G4int,G4int>* vetoCount;
    std::map<G4double,G4int>* ionDep;
    std::map<G4double,G4int>* ionEsc;
    std::map<G4double,G4int>* ionVDep;

    G4bool fNCPosition;		// lag indicating capture position to be recorded
    G4bool fNEPosition;		// Flag indicating entry position to be recorded

    G4String scint;		// Print modifiers
    G4String prefix;
    G4String suffix;
    G4String geomI, geomO, geomB, geomW, geomV;

    G4bool fFirstPrint;		// First print modifiers
    G4bool fFirstNCPos;
    G4bool fFirstNEPos;

    RunMessenger* run_messenger;
    PrimaryGeneratorAction* gen;

    G4double nEnergyBin, nEnergyRef;
    G4int nEnergyNum;
    G4bool nEnergyScale;
    G4bool fRawNeutronCapture;
    G4bool fRawOpticalPhoton;

  G4int nRunNumber;
  G4int nEvNumOffset;
  G4int nRecLev;
  
  G4double nTimeBin, nTimeRef;
    G4int nTimeNum;

    G4double nThermBin, nThermRef;
    G4int nThermNum;

    G4double nDistBin, nDistRef;
    G4int nDistNum;

    G4double pEnergyBin, pEnergyRef;
    G4int pEnergyNum;

    G4double nEntryBin, nEntryRef;
    G4int nEntryNum;
    G4bool nEntryScale;

    G4int photCountBin, photCountRef;
    G4int photCountNum;
    G4bool fSegregation;
    G4bool fRawPhotonCount;

    G4double travelBin, travelRef;
    G4int travelNum;

    G4int vetoCountBin, vetoCountRef;
    G4int vetoCountNum;
    G4bool fRawVetoCount;

    G4double ionDepBin, ionDepRef;
    G4int ionDepNum;

    G4double ionEscBin, ionEscRef;
    G4int ionEscNum;

    G4double ionVDepBin, ionVDepRef;
    G4int ionVDepNum;
};

/* ------------------------------------ */

#endif				// End of the if clause

// EOF
