#ifndef RunAction_H
/// Assure this header is only loaded once
#define RunAction_H

#include "XMLProvider.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include <map>
#include <vector>
#include <time.h>

#include <G4UserRunAction.hh>
#include <G4ThreeVector.hh>

class G4Run;
class RunMessenger;

class RunAction : public G4UserRunAction, public XMLProvider {
    friend class RunMessenger;
    
public:
    
    /// Constructor
    RunAction(PrimaryGeneratorAction* g, DetectorConstruction* d);
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
    
    time_t start_time;  ///< time stamp for start of run
    time_t end_time;    ///< time stamp for end of run

    /// fill in XML data
    virtual void fillNode(TXMLEngine& E);
    
private:
    
    RunMessenger* run_messenger;
    PrimaryGeneratorAction* gen;
    DetectorConstruction* det;
    
    G4int nRunNumber;           ///< run number
    G4int nRecLev;              ///< flag for amount of data to record
};

#endif
