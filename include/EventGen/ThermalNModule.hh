#ifndef THERMALNMODULE_HH
#define THERMALNMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "SurfaceThrower.hh"

#include "G4PhysicalVolumeStore.hh"


class TH1F;

/// Cosmic neutron event generator module
class ThermalNModule: public PrimaryGeneratorModule, public SurfaceThrower {
public:
  /// Constructor
  ThermalNModule(PrimaryGeneratorAction* P);
  
  /// throw event particles
  virtual void GeneratePrimaries(G4Event* anEvent);
  
  /// get total "real time" for primaries thrown
  virtual G4double GetGeneratorTime() const;
  
  double netRate;     ///< total decay rate
  
protected:
  
  /// XML output contents
  virtual void fillNode(TXMLEngine& E);
};

#endif
=======
#include "PrimaryGeneratorAction.hh"

/// Placeholder until Tim Classen uploads the real version
class ThermalNModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    ThermalNModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P,"ThermalN") { }
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent) { }
};
