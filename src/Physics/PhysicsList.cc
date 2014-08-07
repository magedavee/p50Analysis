#include "PhysicsList.hh"
#include "G4OpticalPhysics.hh"

PhysicsList::PhysicsList(): QGSP_BERT_HP() {
    // Add optical photon physics
    //RegisterPhysics( new G4OpticalPhysics );
}
