#include "PhysicsList.hh"

PhysicsList::PhysicsList(): QGSP_BERT_HP(), XMLProvider("Physics"),
myOptPhys(NULL), physDir(new G4UIdirectory("/phys/")) {    
    opticalCmd = new G4UIcmdWithoutParameter("/phys/enableOptical",this);
    opticalCmd->SetGuidance("Enables optical photon physics.");
    opticalCmd->AvailableForStates(G4State_PreInit,G4State_Init);
}

void PhysicsList::SetNewValue(G4UIcommand* command, G4String) {
    if(command == opticalCmd) {
        if(!myOptPhys) {
            myOptPhys = new G4OpticalPhysics(0);
            RegisterPhysics(myOptPhys);
        }
    }
}


