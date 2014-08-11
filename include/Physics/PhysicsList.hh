#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "XMLProvider.hh"

#include <G4UImessenger.hh>
#include <QGSP_BERT_HP.hh>
#include <G4OpticalPhysics.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>

class PhysicsListMessenger;

/// Physics list, derived from QGSP_BERT_HP
class PhysicsList: public QGSP_BERT_HP, public XMLProvider {
    friend class PhysicsListMessenger;
public:
    /// Constructor
    PhysicsList();
    /// Destructor
    ~PhysicsList();
        
protected:
    PhysicsListMessenger* myMessenger;  ///< UI for this PhysicsList
    G4OpticalPhysics* myOptPhys;        ///< optical physics processes
};

/// UI messenger class for physics list
class PhysicsListMessenger: public G4UImessenger {
public:
    /// Constructor
    PhysicsListMessenger(PhysicsList* p);
    /// Destructor
    ~PhysicsListMessenger();
    
    /// Response to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
protected:
    PhysicsList* phys;                  ///< physics list being controlled
    G4UIdirectory* physDir;             ///< UI directory for physics commands
    G4UIcmdWithoutParameter* opticalCmd;///< command for enabling optical physics
};

#endif
