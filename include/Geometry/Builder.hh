#ifndef BUILDER_HH
#define BUILDER_HH

#include "XMLProvider.hh"
#include "MaterialsHelper.hh"
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4AssemblyVolume.hh>
#include <G4RotationMatrix.hh>

class ContainerBuilder;

/// Base class for "buildable" detector sub-assemblies
class Builder: public XMLProvider {
public:
    /// Constructor
    Builder(const string& n);
    /// Destructor
    virtual ~Builder() { }
    
    /// get outer dimensions
    G4ThreeVector getDimensions() const { return dim; }
    
    /// Construct geometry: subclass me! to define main_log or myAssembly
    virtual void construct() = 0;
    /// place into a container 
    virtual void placeInto(ContainerBuilder& B);
    
    bool place_centered = true;                 ///< whether to place centered or on "floor"
    G4LogicalVolume* main_log = NULL;           ///< main constructed volume for placement; if NULL after construct(), use myAssembly instead
    G4VPhysicalVolume* main_phys = NULL;        ///< main volume physical placement
    G4AssemblyVolume myAssembly;                ///< optional detector assembly for placement
    
    static G4RotationMatrix* rot_X_90;          ///< 90 degree rotation around X axis
    static G4RotationMatrix* rot_Y_90;          ///< 90 degree rotation around Y axis
    static G4RotationMatrix* rot_Z_90;          ///< 90 degree rotation around Z axis
    static G4RotationMatrix* rot_X_90_Z_90;     ///< 90 deg around X followed by 90 deg around Z
    
protected:
    G4ThreeVector dim;          ///< outer dimensions
    static double in;           ///< conveniece definition for inch
};

/// Base class for "container" assemblies with inner volume for placing components
class ContainerBuilder: public Builder {
public:
    /// Constructor
    ContainerBuilder(const string& n): Builder(n) { }
    
    /// Construct and place contents; subclass _construct() below for specifics
    virtual void construct();
    /// construction called between setting/building and placing contents
    virtual void _construct() = 0;
    
    Builder* myContents = NULL;                 ///< contents to be constructed
    G4LogicalVolume* inside_log = NULL;         ///< interior logical volume for placing contents
    G4ThreeVector floor_pos;                    ///< Position for items resting on "floor"
    G4ThreeVector center_pos;                   ///< Position for items suspended at "center"
    G4RotationMatrix* placementRot = NULL;      ///< rotation for placed contents
};

/// Specification for a rectangular material shell around contents
class ShellLayerSpec: public XMLProvider {
public:
    /// Constructor with thicknesses
    ShellLayerSpec(G4ThreeVector u, G4ThreeVector l, G4Material* M = NULL, const G4Colour& c = G4Colour(0.5,0.5,0.2)):
    XMLProvider("ShellLayer"), uthick(u), lthick(l), mat(M), vis(c) { }
    /// Constructor
    ShellLayerSpec(double t, G4Material* M = NULL, const G4Colour& c = G4Colour(0.5,0.5,0.2)):
    XMLProvider("ShellLayer"), uthick(t, t, t), lthick(t, t, 0), mat(M), vis(c) { }
    
    G4ThreeVector uthick;       ///< thickness on upper (positive) side on each axis
    G4ThreeVector lthick;       ///< thickness on lower (negative) side on each axis
    G4Material* mat;            ///< material
    G4VisAttributes vis;        ///< layer visualization attributes
    
    /// offset of internal volume
    G4ThreeVector offset() const { return (lthick-uthick)/2; }
    /// set thickness on 4 sides
    void setSideThick(double t) { uthick[0] = uthick[1] = lthick[0] = lthick[1] = t; }
    /// set uniform thickness in all directions
    void setThick(double t) { uthick = lthick = G4ThreeVector(t,t,t); }
    /// "wrap" child volume in specified layers; return physical placed child volume
    G4VPhysicalVolume* wrap(G4LogicalVolume*& child, G4ThreeVector& dim, const G4String& nm) const;
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// Builder specified by nested layers
class ShellLayerBuilder: public ContainerBuilder {
public:
    /// Constructor
    ShellLayerBuilder(const string& n): ContainerBuilder(n) { }
    
    /// add shield layer
    virtual void addLayer(const ShellLayerSpec& sh) { layers.push_back(sh); }
    
    /// get layer logical volume
    G4LogicalVolume* getLayerLog(unsigned int n) { assert(n<layer_log.size()); return layer_log[n]; }
    /// get layer dimensions
    G4ThreeVector getLayerDim(unsigned int n) { assert(n<layer_dim.size()); return layer_dim[n]; }
    
    /// construct layers (call after defining layers in _construct())
    virtual void construct_layers();
    
protected:
    bool expand_to_contents = true;     ///< whether to expand to fit contents or keep constant dimensions
    vector<ShellLayerSpec> layers;      ///< descriptions of each layer
    vector<G4LogicalVolume*> layer_log; ///< logical volumes at each layer
    vector<G4ThreeVector> layer_dim;    ///< dimensions of each layer
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};


#endif
