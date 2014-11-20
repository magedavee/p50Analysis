#ifndef PrimaryGeneratorAction_H
/// Assure header is only loaded once during compilation
#define PrimaryGeneratorAction_H

#include "XMLProvider.hh"
#include "VertexPositioner.hh"
#include "CosineThrower.hh"

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ThreeVector.hh>

class G4Event;
class G4ParticleGun;
class G4GeneralParticleSource;
class PrimaryGeneratorMessenger;
class DetectorConstruction;

class CRYModule;
class IBDModule;
class FissionAntiNuModule;
class CosmicMuonModule;
class CosmicNeutronModule;
class Cf252Module;
class SimpleBGModule;
class ThermalNModule;
class GenPtclModule;
class DecaySourceModule;
class HistogramModule;

class PrimaryGeneratorAction;

/// Base class interface for alternate generator modules
class PrimaryGeneratorModule: public XMLProvider {
public:
    /// Constructor
    PrimaryGeneratorModule(PrimaryGeneratorAction* P, const string& nm): XMLProvider(nm), myPGA(P) { }
    /// Destructor
    virtual ~PrimaryGeneratorModule() { }
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent) = 0;
    /// get total "real time" for primaries thrown
    virtual G4double GetGeneratorTime() const;
    
protected:
    /// convenence function for setting vertices using PrimaryGeneratorAction default
    int setVertices(vector<primaryPtcl>& v);
    /// convenience function for attempting a vertex using PrimaryGeneratorAction default
    bool tryVertex(vector<primaryPtcl>& v);
    /// convenience function for throwing listed primaries
    void throwPrimaries(const vector<primaryPtcl>& v, G4Event* anEvent);
    
    PrimaryGeneratorAction* myPGA;      ///< PrimaryGeneratorAction this module runs for
};


/// Class for generating primary particles for event
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction, public XMLProvider {
    
friend class PrimaryGeneratorMessenger;

public:
    /// constructor
    PrimaryGeneratorAction();
    /// destructor
    virtual ~PrimaryGeneratorAction();

    /// Generates a particle and launches it into the geometry
    void GeneratePrimaries(G4Event* anEvent);

    G4ParticleGun* GetParticleGun() const { return particle_gun; };
    G4GeneralParticleSource* GetParticleSource() const { return particle_source; };
    DetectorConstruction* GetDetector() const { return detect; }
    VertexPositioner* GetPositioner() { return myPositioner; }
    /// Get (create if needed) CosineThrower positioner
    CosineThrower* GetCosineThrower();
    G4int GetVerbosity() const { return verbose; }
    
    /// load particle gun as current generator
    void loadGunModule();
    /// load CRY as current generator
    void loadCRYModule();              
    /// load Inverse Beta Decay as current generator
    void loadIBDModule();
    /// load fission anti-neutrinos as current generator
    void loadFisAntNuModule();
    /// load cosmic muons as current generator
    void loadCosmicMuonModule();
    /// load cosmic neutrons as current generator
    void loadCosmicNeutronModule();
    /// load Cf252 neutron source as current generator
    void loadCf252Module();
    /// load SimpleBG gamma source as current generator
    void loadSimpleBGModule();
    /// load ThermalN neutron source as current generator
    void loadThermalNModule();
    /// load G4GeneralParticleSource
    void loadGPSModule();
    /// load nuclear decay source as current generator
    void loadDecaySourceModule();
    /// load histogram module as current generator
    void loadHistogramModule();
    
protected:
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    /// Set event generator log verbosity
    void SetVerbosity(G4int);
    
    DetectorConstruction* detect;               ///< pointer to detector construction
    
    VertexPositioner* myPositioner;             ///< positioner for vertex and momentum direction
    IsotPtPositioner myIsotPt;                  ///< positioner for isotropic point source
    CosineThrower* myCosineThrower;            ///< positioner defined by source and target surfaces
    
    PrimaryGeneratorModule* genModule;          ///< generator module currently in use
    CRYModule* myCRYModule;                     ///< CRY generator module
    IBDModule* myIBDModule;                     ///< Inverse Beta Decay generator
    FissionAntiNuModule* myFisAntNuModule;      ///< Fission anti-neutrinos generator
    CosmicMuonModule* myCosmicMuonModule;       ///< Cosmic muons generator
    CosmicNeutronModule* myCosmicNeutronModule; ///< Cosmic neutrons generator
    Cf252Module* myCf252Module;                 ///< Cf252 neutron source generator
    SimpleBGModule* mySimpleBGModule;           ///< SimpleBG gamma source generator
    ThermalNModule* myThermalNModule;           ///< ThermalN neutron source generator
    GenPtclModule* myGPSModule;                 ///< G4GeneralParticleSource grnerator
    DecaySourceModule* myDecaySourceModule;     ///< nuclear decay event genertor
    HistogramModule* myHistogramModule;         ///< throw from histogram module
    
    G4int verbose;                              ///< Verbosity (0 = silent, 1 = minimal, 2 = loud)

    G4ParticleGun* particle_gun;
    G4GeneralParticleSource* particle_source;
    PrimaryGeneratorMessenger* myMessenger;
};

#endif
