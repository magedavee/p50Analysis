#ifndef InverseBetaMessenger_H
#define InverseBetaMessenger_H

class InverseBetaKinematics;

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIparameter.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

/// UI Messenger class for InverseBetaKinematics settings
class InverseBetaMessenger: public G4UImessenger {
public:
    /// Constructor
    InverseBetaMessenger(InverseBetaKinematics*);
    /// Destructor
    virtual ~InverseBetaMessenger();
    
    /// respond to UI input
    void SetNewValue(G4UIcommand*,G4String);
    
private:
    /// Removes commands related to Inverse Beta Module when module is destroyed
    void ResetModuleCommands();
    
    InverseBetaKinematics* generator;   ///< generator being controlled
    
    G4UIdirectory* fissNuDir;           ///< Directory holding commands
    
    G4UIcommand* fisProdCmd;
    G4UIcmdWithABool* fisDwyerCmd;
    G4UIcmdWithoutParameter* fisPrintCmd;
    
    G4UIcmdWithADoubleAndUnit* invMonoECmd;
    G4UIcommand* invProdCmd;
    G4UIcmdWith3Vector* invDirectCmd;
    G4UIcmdWithAnInteger* invVerbCmd;
};

#endif
