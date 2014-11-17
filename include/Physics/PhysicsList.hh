#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "XMLProvider.hh"
#include "StepMax.hh"

#include <G4UImessenger.hh>
#include <QGSP_BERT_HP.hh>
#include <G4VModularPhysicsList.hh>
#include <G4OpticalPhysics.hh>

#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

class PhysicsListMessenger;

/// Physics list
class PhysicsList: public G4VModularPhysicsList, public G4UImessenger, public XMLProvider {
public:
    /// Constructor
    PhysicsList();
    /// Destructor
    ~PhysicsList();
    
    /// Response to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
        
    virtual void ConstructParticle();
    virtual void ConstructProcess();
    virtual void SetCuts();
    
protected:
    G4OpticalPhysics* myOptPhys = NULL;         ///< optical physics processes
    G4VPhysicsConstructor* myEMPhys = NULL;     ///< low-energy electromagnetic physics
    QGSP_BERT_HP* myHadronic;                   ///< neutron physics
    StepMax* myStepMax = NULL;                  ///< optional step-size limit
    
    G4UIdirectory physDir;                      ///< UI directory for physics commands
    G4UIcmdWithoutParameter opticalCmd;         ///< command for enabling optical physics
    G4UIcmdWithAString emCmd;                   ///< command for enabling precision electromagnetic physics
    G4UIcmdWithADoubleAndUnit stepMaxCmd;       ///< command for step-size limiter
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
