#ifndef XSDUMP_HH
#define XSDUMP_HH

#include <vector>

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcommand.hh>
#include <G4UIcmdWithAString.hh>
#include <G4ParticleDefinition.hh>
#include <G4Element.hh>
#include <G4Material.hh>

/// Utility for dumping cross-section plots to output
class XSDump: public G4UImessenger {
public:
    /// constructor
    XSDump();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Generate cross section curves
    void makeXSdata();
    
    double Emin;        ///< energy output range minimum
    double Emax;        ///< energy output range maximum
    unsigned int npts;  ///< number of points
    
protected:
    G4ParticleDefinition* particle;             ///< particle for cross sections
    G4ParticleDefinition* neutronDef;           ///< neutron particle definition
    G4UIdirectory xsdir;                        ///< UI directory for cross-section dump commands
    G4UIcmdWithAString ptclCmd;                 ///< UI command for setting particle
    G4UIcommand xsCmd;                          ///< UI command for adding isotope to output list
    
    /// add isotope to output listing
    void addIsot(int Z, int A);
    
    std::vector<int> Zs;                        ///< isotope Zs
    std::vector<int> As;                        ///< isotope As
    std::vector<G4Element*> isots;              ///< isotopes to generate cross-sections
    std::vector<G4Material*> isotmats;          ///< materials to generate cross-sections
};

#endif
