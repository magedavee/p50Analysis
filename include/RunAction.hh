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

class G4Run;
class RunMessenger;
class PrimaryGeneratorAction;

class RunAction : public G4UserRunAction {
    friend class RunMessenger;
    
public:
    
    /// Constructor
    RunAction();
    /// Destructor
    virtual ~RunAction();
    
    /// Called at the beginning of a run (BeamOn) to instantiate variables/objects
    void BeginOfRunAction(const G4Run*);
    /// Called at the end of a run to calculate required values
    void EndOfRunAction(const G4Run*);
    
    G4int GetRecordLevel() const { return nRecLev; }
    
    void PrintCurrentData();
    void PrintOutputList() const;
    
protected:

    void SetRunNumber(G4int rnum);
    void SetRecordLevel(G4int rnum) { nRecLev = rnum; }
    
private:
    
    RunMessenger* run_messenger;
    PrimaryGeneratorAction* gen;
    
    G4int nRunNumber;           ///< run number
    G4int nRecLev;              ///< flag for amount of data to record
};

#endif
