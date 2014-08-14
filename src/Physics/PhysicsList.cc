#include "PhysicsList.hh"

PhysicsList::PhysicsList(): QGSP_BERT_HP(), XMLProvider("Physics"),
myMessenger(new PhysicsListMessenger(this)), myOptPhys(NULL) {
}

PhysicsList::~PhysicsList() {
    delete myMessenger;
    // Do not delete myOptPhys --- automatically handled
}

void PhysicsList::fillNode(TXMLEngine& E) {
    addAttr(E, "optical", myOptPhys?"on":"off");
}

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* p): 
phys(p), physDir(new G4UIdirectory("/phys/")) {    
    opticalCmd = new G4UIcmdWithoutParameter("/phys/enableOptical",this);
    opticalCmd->SetGuidance("Enables optical photon physics.");
    opticalCmd->AvailableForStates(G4State_PreInit,G4State_Init);
}

PhysicsListMessenger::~PhysicsListMessenger() {
    delete physDir;
    delete opticalCmd;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String) {
    if(command == opticalCmd) {
        if(!phys->myOptPhys) {
            phys->myOptPhys = new G4OpticalPhysics(0);
            phys->RegisterPhysics(phys->myOptPhys);
        }
    }
}
