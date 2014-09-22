#ifndef MATERIALSHELPER_HH
/// Assure this header is loaded only once
#define MATERIALSHELPER_HH

#include <map>
#include <string>

#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4MaterialPropertiesTable.hh>

/// Class with singleton instance providing materials properties
class MaterialsHelper {
public:
    /// get instance reference
    static MaterialsHelper& M();
    
    G4NistManager* nist;        ///< NIST materials DB
    double room_T;              ///< materials "room temperature"
    
    G4Element* elLi6;           ///< pure 6Li
    
    G4Material* nat_H;          ///< natural hydrogen
    G4Material* nat_Li;         ///< natural lithium
    G4Material* Li6;            ///< isotopically pure 6Li
    G4Material* nat_B;          ///< natural boron
    G4Material* nat_C;          ///< natural carbon
    G4Material* nat_N;          ///< natural nitrogen
    G4Material* nat_O;          ///< natural oxygen
    G4Material* nat_Al;         ///< natural aluminum
    G4Material* nat_Si;         ///< natural silicon
    G4Material* nat_P;          ///< natural phosphorus
    G4Material* nat_Cl;         ///< natural chlorine
    G4Material* nat_Ca;         ///< natural calcium
    G4Material* nat_Fe;         ///< natural iron
    G4Material* nat_Cr;         ///< natural chromium
    G4Material* nat_Mo;         ///< natural molybdenum
    G4Material* nat_Pb;         ///< natural lead
    
    G4Material* Vacuum;         ///< vacuum
    G4Material* Air;            ///< room air
    G4Material* MinOil;         ///< Mineral oil
    G4Material* PMMA;           ///< plexiglass, transparent
    G4Material* PMMA_black;     ///< plexiglass, black
    G4Material* PMMA_white;     ///< plexiglass, white
    G4Material* PEEK;           ///< PEEK O3 C19 H12
    G4Material* Polyeth;        ///< Polyethylene
    G4Material* BPoly;          ///< Borated (5% by mass) polyethylene
    G4Material* LiPoly;         ///< Lithiated (5% by mass) polyethlyene
    G4Material* Water;          ///< Water
    G4Material* PVT;            ///< Polyvinyl Toluene scintillator
    G4Material* RawPsiCumene;   ///< un-doped pseudocumene scintillator base
    G4Material* UG_AB;          ///< Ultima Gold AB scintillator
    G4Material* EJ309;          ///< Eljen EJ-309 liquid scintillator
    G4Material* SS444;          ///< Stainless Steel 444
    G4Material* Quartz;         ///< Quartz
    G4Material* Concrete;       ///< Concrete
    G4Material* Dirt;           ///<  "Dirt,"  From PNNL-20401
    
    /// get 6Li-loaded version of scintillator (option for natural or enriched 6Li)
    G4Material* get6LiLS(G4Material* base, double loading, bool enriched = true);
    
protected:
    /// Constructor
    MaterialsHelper();
    /// set up material optical properties
    void setupOptical();
    
    static MaterialsHelper* theMatHelper;       ///< singleton instance
    std::map<std::string, G4Material*> xmats;   ///< extra on-demand materials like 6Li-loaded liquid scintillator concoctions
    
    G4MaterialPropertiesTable* mptUG_AB;        ///< optical properties for Ultima Gold AB scintillator
    double birksPC;                             ///< Birk's Law quenching constant for pseudocumene
    double birksPVT;                            ///< Birk's Law quenching constant for PVT
    double birksUG_AB;                          ///< Birk's Law quenching constant for Ultima Gold AB
};


#endif
