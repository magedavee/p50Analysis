#include "VisSetup.hh"

VisSetup::VisSetup(G4VisManager* vis_manager): pidModel("vpid") {
    pidModel.Set("e+", G4Colour(1., 0., 0.));
    pidModel.Set("e-", G4Colour(0., 0., 1.));
    pidModel.Set("gamma", G4Colour(0.5, 1., 0.5));
    pidModel.Set("opticalphoton", G4Colour(1., 1., 1., 0.05));
    pidModel.Set("n", G4Colour(0., 1., 0.));
    pidModel.Set("p", G4Colour(1., 1., 0.));
    pidModel.Set("mu+", G4Colour(0.7, 0.5, 0.));
    pidModel.Set("mu-", G4Colour(0., 0.5, 0.7));
    pidModel.Set("pi+", G4Colour(0.4, 0.2, 0.));
    pidModel.Set("pi-", G4Colour(0., 0.2, 0.4));
    
    vis_manager->RegisterModel(&pidModel);
    vis_manager->SelectTrajectoryModel(pidModel.Name());
}
