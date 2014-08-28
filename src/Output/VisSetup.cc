#include "VisSetup.hh"
#include <G4TrajectoryDrawByParticleID.hh>
#include <G4ios.hh>

VisSetup::VisSetup(G4VisManager* vis_manager) {
    
    G4TrajectoryDrawByParticleID* pidModel = new G4TrajectoryDrawByParticleID("vpid");
    
    pidModel->Set("e+", G4Colour(1., 0., 0.));          // red
    pidModel->Set("e-", G4Colour(0., 0., 1.));          // blue
    pidModel->Set("gamma", G4Colour(0.5, 1., 0.5));     // pale green
    pidModel->Set("opticalphoton", G4Colour(1., 1., 1., 0.05)); // transparent white
    pidModel->Set("n", G4Colour(0., 1., 0.));           // green
    pidModel->Set("p", G4Colour(1., 1., 0.));           // yellow
    pidModel->Set("mu+", G4Colour(1.0, 0., 1.0));       // magenta
    pidModel->Set("mu-", G4Colour(0., 1.0, 1.0));       // cyan
    pidModel->Set("pi+", G4Colour(0.4, 0.2, 0.));       // dark orange-bown
    pidModel->Set("pi-", G4Colour(0., 0.2, 0.4));       // dark cyan
    
    vis_manager->RegisterModel(pidModel);
    vis_manager->SelectTrajectoryModel(pidModel->Name());

    vis_manager->RegisterModel(new OpticalPhotonReducer());
    
    G4cout << "Track visualization setup complete.\n";
}

bool OpticalPhotonReducer::Evaluate (const G4VTrajectory& T) const {
    if(T.GetParticleName() != "opticalphoton") return true;
    accum += pAccept;
    if(accum >= 1) {
        accum = 0;
        return true;
    }
    return false;
}
