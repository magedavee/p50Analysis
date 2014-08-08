#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH

#include "XMLProvider.hh"

#include <G4UImessenger.hh>
#include <QGSP_BERT_HP.hh>
#include <G4OpticalPhysics.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>

/// Physics list, derived from QGSP_BERT_HP
class PhysicsList: public QGSP_BERT_HP, public G4UImessenger, public XMLProvider {
public:
    /// Constructor
    PhysicsList();
    
    /// Response to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
        
protected:
    G4OpticalPhysics* myOptPhys;        ///< optical physics processes
    
    G4UIdirectory* physDir;             ///< UI directory for physics commands
    G4UIcmdWithoutParameter* opticalCmd;///< command for enabling optical physics
};

#endif
