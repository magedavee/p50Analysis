#ifndef CRYMODULE_HH
/// Assure header is only loaded once
#define CRYMODULE_HH

#include "PrimaryGeneratorAction.hh"
#include <G4UImessenger.hh>

class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;
class CRYGenerator;
class CRYParticle;

/// Event generator module with interface to CRY cosmic ray shower generator
class CRYModule: public PrimaryGeneratorModule, public G4UImessenger {
public:
    /// Constructor
    CRYModule(PrimaryGeneratorAction* P);
    /// Destructor
    ~CRYModule();
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

    /// UI response
    void SetNewValue(G4UIcommand*,G4String);

    /// get event generator elapsed "real" time
    double getElapsedTime() const;
    
protected:
    
    /// initialize CRY generator with input string
    void initCRY(const std::string& S);
    
    G4double zOffset;                   ///< z offset in world volume for CRY event plane
    CRYGenerator* CRY_generator;        ///< CRY event generator
    G4bool inputState;                  ///< flag indicating CRY currently receiving input commands
    G4bool primpoint;                   ///< flag to determine whether particles are generated from the CRY package selectively point at the detector
    std::vector<CRYParticle*> vect;     ///< vector of generated particles
    
    G4UIdirectory* CRYDir;              ///< UI directory for CRY-related commands
    G4UIcmdWithABool* cryPointCmd;      ///< UI command for limiting primaries to those aimed at detector
    G4UIcmdWithADoubleAndUnit* cryZCmd; ///< UI command for setting height of event vertex plane
    G4UIcmdWithAString* cryInputCmd;    ///< UI command for direct commands to CRY
    G4UIcmdWithoutParameter* cryUpdateCmd;      ///< UI command for executing cached CRY commands
};

#endif
