#ifndef SIMPLEBGMODULE_HH
#define SIMPLEBGMODULE_HH

#include "PrimaryGeneratorAction.hh"

#include "G4PhysicalVolumeStore.hh"


class TH1F;

/// Cosmic neutron event generator module
class SimpleBGModule: public PrimaryGeneratorModule {
public:
  /// Constructor
  SimpleBGModule(PrimaryGeneratorAction* P);
  
  /// throw event particles
  virtual void GeneratePrimaries(G4Event* anEvent);
  
  double netRate;     ///< total decay rate
  
protected:
  TH1F* Aug28P;       ///< energy distribution
  TH1F* Aug28R;       ///< energy distribution
  TH1F* Jun30;       ///< energy distribution
  
  /// XML output contents
  virtual void fillNode(TXMLEngine& E);
};

#endif
