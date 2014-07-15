// Unrestricted Use - Property of AECL
// 
// DetectorMessenger.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Geometry and Optical Module Messenger
//  Contains class functions/variables
//
// --------------------------------------------------------
//  Version 1.01 - 2011/04/29 - A. Ho
//  Modified 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef DetectorMessenger_H
/// Assure header file is loaded only once
#define DetectorMessenger_H

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;

class DetectorMessenger: public G4UImessenger {
public:
    /// constructor
    DetectorMessenger(DetectorConstruction*);
    /// destructor
    virtual ~DetectorMessenger();
    
    void SetNewValue(G4UIcommand*,G4String);
    G4String GetCurrentValue(G4UIcommand*);

private:
    
    DetectorConstruction* detector;
    
    // Directories holding commands
    G4UIdirectory* scintDir;    
    G4UIdirectory* detectDir;
    
    // Commands - for use in the terminal during run-time
    G4UIcmdWithABool* opticalCmd;
    G4UIcmdWithADouble* birksCmd;
    G4UIcmdWithADouble* lobeCmd;
    G4UIcmdWithADouble* sigalCmd;
    G4UIcmdWithADouble* spikeCmd;
    G4UIcmdWithADouble* reflCmd;
    G4UIcmdWithADouble* effCmd;
    G4UIcmdWithADouble* backCmd;
    G4UIcmdWithADouble* QECmd;
    G4UIcmdWithAnInteger* segXCmd;
    G4UIcmdWithAnInteger* segYCmd;
    G4UIcmdWithADoubleAndUnit* segLCmd;
    G4UIcmdWithADoubleAndUnit* segWCmd;
    G4UIcmdWithADoubleAndUnit* segHCmd;
    G4UIcmdWithADoubleAndUnit* shLeadCmd;
    G4UIcmdWithADoubleAndUnit* shBPolyCmd;
    G4UIcmdWithADoubleAndUnit* shLiPolyCmd;
    G4UIcmdWithADoubleAndUnit* scinLCmd;
    G4UIcmdWithADoubleAndUnit* scinWCmd;
    G4UIcmdWithADoubleAndUnit* scinHCmd;
    G4UIcmdWithADoubleAndUnit* segBuffCmd;
    G4UIcmdWithADoubleAndUnit* segAirCmd;
    G4UIcmdWithADoubleAndUnit* segWrapGapCmd;
    G4UIcmdWithADoubleAndUnit* segWrapCmd;
    G4UIcmdWithADoubleAndUnit* segAcrylCmd;
    G4UIcmdWithADoubleAndUnit* segPMTCmd;
    G4UIcmdWithAString* scintCompCmd;
    G4UIcmdWithABool* innerCmd;
    G4UIcmdWithABool* outerCmd;
    G4UIcmdWithABool* polyCmd;
    G4UIcmdWithABool* shieldCmd;
    G4UIcmdWithABool* cylinderCmd;
    G4UIcmdWithABool* vetoCmd;
    G4UIcmdWithABool* innerVCmd;
    G4UIcmdWithABool* outerVCmd;
    G4UIcmdWithABool* polyVCmd;
    G4UIcmdWithABool* shieldVCmd;
    G4UIcmdWithABool* vetoVCmd;
    G4UIcmdWithABool* vertCmd;
    G4UIcommand* modCmd;
    G4UIcommand* conCmd;
    G4UIcmdWithABool* explodeCmd;
    G4UIcmdWithoutParameter* printCmd;
    G4UIcmdWithAString *optSurfCmd;
};

#endif
