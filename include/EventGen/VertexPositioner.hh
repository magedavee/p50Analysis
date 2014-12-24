#ifndef VERTEXPOSITIONER_HH
#define VERTEXPOSITIONER_HH

#include "XMLProvider.hh"

#include <vector>
using std::vector;

#include <G4ThreeVector.hh>

/// Specification for a primary particle to throw
struct primaryPtcl {
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
    VertexPositioner(const string& nm = "VertexPositioner"): XMLProvider(nm) { }
    /// Destructor
    virtual ~VertexPositioner() { }
    
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
    
protected:
    
    int nAttempts = 0;          ///< number of proposed throws
};

/// Vertex positioner for isotropic momenta from a fixed point
class IsotPtPositioner: public VertexPositioner {
public:
    /// Constructor
    IsotPtPositioner(const G4ThreeVector& x = G4ThreeVector()): VertexPositioner("IsotPtPositioner"), x0(x) { }
    
    /// Set position, momentum for list of particles
    virtual bool tryVertex(vector<primaryPtcl>& v);
    
    G4ThreeVector x0;   ///< vertex position
 
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
