/// \file VertexPositioner.hh \brief Base class for specifying event generator vertex/direction distributions.
#ifndef VERTEXPOSITIONER_HH
#define VERTEXPOSITIONER_HH

#include "XMLProvider.hh"

#include <vector>
using std::vector;

#include <G4ThreeVector.hh>

/// Specification for a primary particle to throw
class primaryPtcl {
public:
    /// Constructor
    primaryPtcl() { }
    
    int PDGid;          ///< PDG particle ID enumeration
    double KE;          ///< particle kinetic energy
    G4ThreeVector pos;  ///< vertex position
    G4ThreeVector mom;  ///< momentum direction
    double t;           ///< particle time
};

/// Base class for setting event vertex position and momentum direction
class VertexPositioner: public XMLProvider {
public:
    /// Constructor
    VertexPositioner(const string& n = "VertexPositioner"): XMLProvider(n) { }
    /// Destructor
    virtual ~VertexPositioner() { }
    
    /// propose a vertex position
    virtual void proposePosition() = 0;
    /// Attempt to set position/momentum for particles; return whether attempt passed
    virtual bool tryVertex(vector<primaryPtcl>& v) = 0;
    /// Set position, momentum for list of particles; return number of vertices tried
    virtual int setVertex(vector<primaryPtcl>& v);
    
    /// utility function for generating isotropic random direction
    static G4ThreeVector randomDirection();
    
    /// Get "attempted" number of throws before discarding for surface normal, target conditions
    virtual int getAttempts() const { return nAttempts; }
    /// Get "normalized" (to volume, surface, etc.) number of attempts
    virtual double getAttemptsNormalized() const { return getAttempts(); }
    
    G4ThreeVector pos;                  ///< proposed vertex position
    G4ThreeVector originPoint;          ///< origin point for 1/r^2 capture generators
    
protected:
    
    int nAttempts = 0;                  ///< number of proposed throws
};

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithADouble.hh>

/// Vertex positioner for isotropic momenta from a fixed point
class IsotPtPositioner: public VertexPositioner, public G4UImessenger {
public:
    /// Constructor
    IsotPtPositioner(const G4ThreeVector& x = G4ThreeVector());
    
    /// Set position, momentum for list of particles
    virtual bool tryVertex(vector<primaryPtcl>& v);
    /// propose a vertex position: keep fixed pos
    virtual void proposePosition() { }
    /// set origin position
    void setPos(G4ThreeVector x) { originPoint = pos = x; }
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    G4ThreeVector dxn;          ///< direction to measure cos theta from
    double costheta_min = -1;   ///< minimum direction cos theta
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    G4UIdirectory isotptDir;    ///< UI directory for IsotPtPositioner commands
    G4UIcmdWithADouble cthCmd;  ///< UI command for costheta_min setting
};

#endif
