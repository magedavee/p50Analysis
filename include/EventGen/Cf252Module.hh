/// \file Cf252Module.hh \brief Cf252 neutrons event generator
#ifndef CF252MODULE_HH
#define CF252MODULE_HH

#include "PrimaryGeneratorAction.hh"

class TH1F;

/// Cosmic neutron event generator module
class Cf252Module: public PrimaryGeneratorModule {
public:
    /// Constructor
    Cf252Module(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    double netRate;     ///< total decay rate
    
protected:
    const double a;     ///< distribution parameter 'a'
    const double b;     ///< distribution parameter 'b'
    TH1F* myDist;       ///< energy distribution
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
