#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

//#define EMPHYSICS

#include "XMLProvider.hh"

#ifndef EMPHYSICS

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
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
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

#else

#include <G4VModularPhysicsList.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmLivermorePhysics.hh>
#include <G4EmPenelopePhysics.hh>

/// Low-energy electromagnetic physics list
class PhysicsList: public G4VModularPhysicsList, public XMLProvider {
public:
    /// Constructor
    PhysicsList(): XMLProvider("EMPhysics") { SetDefaultCutValue(0.001); emPhysicsList = new G4EmStandardPhysics(); }
    virtual void ConstructParticle() { emPhysicsList->ConstructParticle(); }
    virtual void ConstructProcess() { AddTransportation(); emPhysicsList->ConstructProcess(); AddStepMax(); }
    virtual void SetCuts();
    virtual void AddStepMax();
    G4VPhysicsConstructor* emPhysicsList;
};

#endif

#endif
