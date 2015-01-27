#ifndef IBDMODULE_HH
/// Assure headers are only loaded once
#define IBDMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include "InverseBetaKinematics.hh"

/// Inverse Beta Decay event generator module
class IBDModule: public PrimaryGeneratorModule, public InverseBetaKinematics, public G4UImessenger {
public:
    /// Constructor
    IBDModule(PrimaryGeneratorAction* P);
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);
    
    /// respond to UI input
    void SetNewValue(G4UIcommand*,G4String);
    
    /// Set whether to generate neutrons
    void ToggleNeutronGeneration(G4bool);
    /// Set whether to generate positrons
    void TogglePositronGeneration(G4bool);
    /// Set whether to generate (uncaptured) antineutrinos
    void ToggleAntinuGeneration(G4bool);
    /// Set whether to alternately generate positron, neutron in separate events
    void ToggleSequentialGeneration(G4bool);
    
protected:
    
    G4UIcmdWithABool invNeutCmd;        ///< command to set neutrons generation
    G4UIcmdWithABool invPosiCmd;        ///< command to set positrons generation
    G4UIcmdWithABool invSeqCmd;         ///< command to set "sequential" mode
    G4UIcmdWithABool invNuCmd;          ///< command to set antineutrino generation
    
    vector<G4double> kinematics;        ///< generated kinematics vector
    G4bool Antinus;     ///< Flag to specify antineutrino (only) generation
    G4bool Neutrons;    ///< Flag to specify neutron production
    G4bool Positrons;   ///< Flag to specify positron production
    G4bool Sequential;  ///< Flag to specify sequential positron and neutron production
    G4bool primary;     ///< whether this is positron in sequential generation
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
