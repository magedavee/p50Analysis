#ifndef PR2SHIELDBUILDER_HH
#define PR2SHIELDBUILDER_HH

#include "Builder.hh"
#include "ScintSegVol.hh"

/// Builder for PROSPECT-2 shielding cave
class PR2ShieldBuilder: public ShellLayerBuilder {
public:
    /// constructor
    PR2ShieldBuilder(): ShellLayerBuilder("PROSPECT2") { expand_to_contents = false; place_centered = false; }
    
    /// Construct geometry
    void _construct();
};

/// Builder for PROSPECT-2 muon veto
class PR2MuVetoBuilder: public ScintChunk {
public:
    /// Constructor
    PR2MuVetoBuilder(int n);
};

#endif
