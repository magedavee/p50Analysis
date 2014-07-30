#ifndef MATERIALSHELPER_HH
/// Assure this header is loaded only once
#define MATERIALSHELPER_HH

#include <map>

#include <G4Material.hh>
#include <G4NistManager.hh>

class MaterialsHelper {
public:
    
    /// initialize material properties
    static void init();
    
    static G4NistManager* nist;         ///< NIST materials DB
    
    static G4Material* nat_H;           ///< natural hydrogen
    static G4Material* Li6;             ///< isotopically pure 6Li
    static G4Material* nat_C;           ///< natural carbon
    static G4Material* nat_O;           ///< natural oxygen
    static G4Material* nat_Ca;          ///< natural calcium
    static G4Material* nat_Si;          ///< natural silicon
    static G4Material* nat_Al;          ///< natural aluminum
    static G4Material* nat_Fe;          ///< natural iron
    static G4Material* nat_Cr;          ///< natural chromium
    static G4Material* nat_Mo;          ///< natural molybdenum
    
    static G4Material* Vacuum;          ///< vacuum
    static G4Material* Air;             ///< room air
    static G4Material* PMMA;            ///< plexiglass
    static G4Material* PEEK;            ///< PEEK O3 C19 H12
    static G4Material* RawPsiCumene;    ///< un-doped pseudocumene scintillator base
    static G4Material* SS444;           ///< Stainless Steel 444
    static G4Material* Quartz;          ///< Quartz
    static G4Material* Concrete;        ///< Concrete
    
    /// get 6Li-loaded liquid scintillator material
    static G4Material* get6LiLS(double loading = 0.007);
    
protected:
    
    static std::map<double, G4Material*> LiLSs; ///< 6Li-loaded liquid scintillator concoctions
};


#endif
