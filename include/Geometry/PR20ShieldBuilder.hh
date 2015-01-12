#ifndef PR20SHIELDBUILDER_HH
#define PR20SHIELDBUILDER_HH

#include "Builder.hh"

/// Builder for PROSPECT-20 inner shielding cave
class PR20InnerShieldBuilder: public ShellLayerBuilder {
public:
    /// constructor
    PR20InnerShieldBuilder(): ShellLayerBuilder("PROSPECT20_Inner") { expand_to_contents = false; place_centered = false; }
    
    /// Construct geometry
    void _construct();
};

/// Builder for PROSPECT-20 inner+outer cave
class PR20ShieldBuilder: public ShellLayerBuilder {
public:
    /// constructor
    PR20ShieldBuilder(): ShellLayerBuilder("PROSPECT20") { myContents = &myInnerShield; place_centered = false; }
    
    /// Construct geometry
    void _construct();
    
    PR20InnerShieldBuilder myInnerShield;       ///< inner shield layers
};

#endif
