#include "PrimaryGeneratorAction.hh"

/// Placeholder until Tim Classen uploads the real version
class ThermalNModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    ThermalNModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P,"ThermalN") { }
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent) { }
};
