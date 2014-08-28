#ifndef BUILDER_HH
#define BUILDER_HH

#include "XMLProvider.hh"
#include <G4LogicalVolume.hh>

/// Base class for "buildable" detector sub-assemblies
class Builder: public XMLProvider {
public:
    /// Constructor
    Builder(const std::string& n): XMLProvider(n), main_log(NULL), dim() { }
    /// Destructor
    virtual ~Builder() { }
    
    /// get outer dimensions
    G4ThreeVector getDimensions() const { return dim; }
    
    /// Construct geometry: subclass me!
    virtual void construct() = 0;
    
    G4LogicalVolume* main_log;  ///< main constructed volume for placement
    
protected:
    G4ThreeVector dim;          ///< outer dimensions
    
};

#endif
