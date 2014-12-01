#ifndef MATERIALSHELPER_HH
/// Assure this header is loaded only once
#define MATERIALSHELPER_HH

#include <map>
#include <string>

#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4MaterialPropertiesTable.hh>

using std::string;
using std::map;

/// Class with singleton instance providing materials properties
class MaterialsHelper {
public:
    /// get instance reference
    static MaterialsHelper& M();
    
    G4NistManager* nist;        ///< NIST materials DB
    double room_T;              ///< materials "room temperature"
    
    G4Element* el6Li;           ///< pure 6Li
    G4Element* el3He;           ///< pure 3He
    
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
    G4Material* nat_Cu;         ///< natural copper
    G4Material* nat_Zn;         ///< natural zinc
    G4Material* nat_Cr;         ///< natural chromium
    G4Material* nat_Mo;         ///< natural molybdenum
    G4Material* nat_Pb;         ///< natural lead
    G4Material* nat_W;          ///< natural tungsten
    
    G4Material* Vacuum;         ///< vacuum
    G4Material* Air;            ///< room air
    G4Material* MinOil;         ///< Mineral oil
    G4Material* Mylar;          ///< Mylar
    G4Material* PMMA;           ///< plexiglass, transparent
    G4Material* PMMA_black;     ///< plexiglass, black
    G4Material* PMMA_white;     ///< plexiglass, white
    G4Material* PEEK;           ///< PEEK O3 C19 H12
    G4Material* Polyeth;        ///< Polyethylene
    G4Material* Polystyrene;    ///< Polystyrene
    G4Material* PTFE;           ///< PTFE (Teflon)
    G4Material* BPoly5;         ///< Borated (5% by mass) polyethylene
    G4Material* BPoly30;        ///< Borated (30% by mass) polyethylene
    G4Material* LiPoly;         ///< Lithiated (5% by mass) polyethlyene
    G4Material* Water;          ///< Water
    G4Material* PVT;            ///< Polyvinyl Toluene scintillator
    G4Material* RawPsiCumene;   ///< un-doped pseudocumene scintillator base
    G4Material* UG_AB;          ///< Ultima Gold AB scintillator
    G4Material* EJ309;          ///< Eljen EJ-309 liquid scintillator
    G4Material* SS444;          ///< Stainless Steel 444
    G4Material* Quartz;         ///< Quartz
    G4Material* Pyrex;          ///< Pyrex borosilicate glass
    G4Material* Concrete;       ///< Concrete
    G4Material* Dirt;           ///<  "Dirt,"  From PNNL-20401
    
    /// get 6Li-loaded version of scintillator (option for natural or enriched 6Li)
    G4Material* get6LiLS(G4Material* base, double loading, bool enriched = true);
    /// get 3He gas at specified density
    G4Material* get3He(double dens);
    
protected:
    /// Constructor
    MaterialsHelper();
    /// set up material optical properties
    void setupOptical();
    
    static MaterialsHelper* theMatHelper;       ///< singleton instance
    map<string, G4Material*> xmats;             ///< extra on-demand materials like 6Li-loaded liquid scintillator concoctions
};


#endif
