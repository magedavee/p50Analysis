#ifndef BUILDER_HH
#define BUILDER_HH

#include "XMLProvider.hh"
#include "MaterialsHelper.hh"
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VisAttributes.hh>

/// Base class for "buildable" detector sub-assemblies
class Builder: public XMLProvider {
public:
    /// Constructor
    Builder(const string& n): XMLProvider(n), main_log(NULL), dim() { }
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

/// Specification for a rectangular material shell around contents
class ShellLayerSpec: public XMLProvider {
public:
    /// Constructor
    ShellLayerSpec(double t, G4Material* M = NULL, const G4Colour& c = G4Colour(0.5,0.5,0.2)):
    XMLProvider("ShellLayer"), top_thick(t), side_thick(t), bottom_thick(0), mat(M), vis(c) { }
    
    double top_thick;           ///< thickness on top
    double side_thick;          ///< thickness on sides
    double bottom_thick;        ///< thickness on bottom
    G4Material* mat;            ///< material
    G4VisAttributes vis;        ///< layer visualization attributes
    
    /// "wrap" child volume in specified layers
    G4VPhysicalVolume* wrap(G4LogicalVolume*& child, G4ThreeVector& dim, const G4String& nm) const;
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// Builder specified by nested layers
class ShellLayerBuilder: public Builder {
public:
    /// Constructor
    ShellLayerBuilder(const string& n): Builder(n) { }
    
    /// add shield layer
    virtual void addLayer(const ShellLayerSpec& sh) { layers.push_back(sh); }
    
protected:
    /// construct layers
    void constructLayers(Builder& contents);
    
    vector<ShellLayerSpec> layers;         ///< descriptions of each layer
};


#endif
