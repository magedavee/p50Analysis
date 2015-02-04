/// \file PrimaryGeneratorAction.hh \brief Geant4 primary generator action user class.
#ifndef PrimaryGeneratorAction_H
/// Assure header is only loaded once during compilation
#define PrimaryGeneratorAction_H

#include "XMLProvider.hh"
#include "VertexPositioner.hh"
#include "CosineThrower.hh"
#include "DirectionThrower.hh"

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ThreeVector.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4ParticleGun.hh>

class G4Event;
class G4GeneralParticleSource;
class DetectorConstruction;

class CRYModule;
class IBDModule;
class CosmicMuonModule;
class CosmicNeutronModule;
class Cf252Module;
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
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction,
public XMLProvider, public G4UImessenger {
public:
    /// constructor
    PrimaryGeneratorAction();
    /// destructor
    virtual ~PrimaryGeneratorAction();

    /// Respond to UI commands
    void SetNewValue(G4UIcommand*,G4String);
    
    /// Generates a particle and launches it into the geometry
    void GeneratePrimaries(G4Event* anEvent);

    G4ParticleGun* GetParticleGun() const { return particle_gun; };
    G4GeneralParticleSource* GetParticleSource() const { return particle_source; };
    DetectorConstruction* GetDetector() const { return detect; }
    VertexPositioner* GetPositioner() { return myPositioner; }
    /// Get (create if needed) CosineThrower positioner
    CosineThrower* GetCosineThrower();
    /// Get (create if needed) DirectionThrower positioner
    DirectionThrower* GetDirectionThrower();
    
    G4int GetVerbosity() const { return verbose; }
    
    /// load CRY as current generator
    void loadCRYModule();              
    /// load Inverse Beta Decay as current generator
    void loadIBDModule();
    /// load cosmic muons as current generator
    void loadCosmicMuonModule();
    /// load cosmic neutrons as current generator
    void loadCosmicNeutronModule();
    /// load Cf252 neutron source as current generator
    void loadCf252Module();
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
    CosineThrower* myCosineThrower;             ///< positioner defined by source and target surfaces
    DirectionThrower* myDirThrower;             ///< positioner for directional flux from target surface
    
    PrimaryGeneratorModule* genModule;          ///< generator module currently in use
    CRYModule* myCRYModule;                     ///< CRY generator module
    IBDModule* myIBDModule;                     ///< antineutrino and Inverse Beta Decay generator
    CosmicMuonModule* myCosmicMuonModule;       ///< Cosmic muons generator
    CosmicNeutronModule* myCosmicNeutronModule; ///< Cosmic neutrons generator
    Cf252Module* myCf252Module;                 ///< Cf252 neutron source generator
    GenPtclModule* myGPSModule;                 ///< G4GeneralParticleSource grnerator
    DecaySourceModule* myDecaySourceModule;     ///< nuclear decay event genertor
    HistogramModule* myHistogramModule;         ///< throw from histogram module
    
    G4int verbose;                              ///< Verbosity (0 = silent, 1 = minimal, 2 = loud)

    G4ParticleGun* particle_gun;
    G4GeneralParticleSource* particle_source;
    
    PrimaryGeneratorAction* generator;          ///< generator being controlled
    G4UIdirectory genDir;                       ///< UI directory for generator commands
    G4UIdirectory modDir;                       ///< UI directory for module commands
    G4UIdirectory posDir;                       ///< UI directory for vertex selection commands
    
    G4UIcmdWithAnInteger verbCmd;               ///< UI command for setting verbosity
    G4UIcmdWithoutParameter moduleGuncmd;       ///< UI command for using "particle gun" generator
    G4UIcmdWithoutParameter moduleCRYcmd;       ///< UI command for using CRY cosmic ray generator
    G4UIcmdWithoutParameter moduleIBDcmd;       ///< UI command for using IBD generator
    G4UIcmdWithoutParameter moduleCosMucmd;     ///< UI command for using cosmic muon generator
    G4UIcmdWithoutParameter moduleCosNcmd;      ///< UI command for using cosmic neutron generator
    G4UIcmdWithoutParameter moduleCf252cmd;     ///< UI command for using Cf252 neutron generator
    G4UIcmdWithoutParameter moduleGPScmd;       ///< UI command for using the G4GeneralParticleSource generator
    G4UIcmdWithoutParameter moduleDecaySrccmd;  ///< UI command for using the nuclear decay source generator
    G4UIcmdWithoutParameter moduleHistocmd;     ///< UI command for using the nuclear decay source generator
    
    G4UIcmdWith3VectorAndUnit ptPosCmd;         ///< UI command for setting isotropic point source positioner
    G4UIcmdWithoutParameter isotFluxCmd;        ///< UI command for isotropic flux from world volume surface
    G4UIcmdWithoutParameter srcTargCmd;         ///< UI command for isotropic flux from source to target volume
    G4UIcmdWithoutParameter scintSrcCmd;        ///< UI command for isotropic source in scintillator volume
    G4UIcmdWithADouble cosFluxCmd;              ///< UI command for cos^x-weighted flux
    G4UIcmdWith3Vector dirFluxCmd;              ///< UI command for directional flux
};

#endif
