/// \file PR20ShieldBuilder.hh \brief PROSPECT-20 inner and outer shield geometry.
#ifndef PR20SHIELDBUILDER_HH
#define PR20SHIELDBUILDER_HH

#include "Builder.hh"
#include "PR2ShieldBuilder.hh"

/// Builder for PROSPECT-20 inner shielding cave
class PR20InnerShieldBuilder: public WaterbrickerBuilder {
public:
    /// constructor
    PR20InnerShieldBuilder();
    
    /// Construct geometry
    void _construct();
};

/// Builder for PROSPECT-20 inner+outer cave
class PR20ShieldBuilder: public WaterbrickerBuilder {
public:
    /// constructor
    PR20ShieldBuilder();
    
    /// Construct geometry
    void _construct();
    
    PR20InnerShieldBuilder myInnerShield;       ///< inner shield layers
};

#endif
