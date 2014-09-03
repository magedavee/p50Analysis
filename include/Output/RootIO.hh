#ifndef INCLUDE_ROOTIO_HH
/// Assure this header file is loaded only once
#define INCLUDE_ROOTIO_HH

#include "Event.hh"
#include <vector>

#include <G4String.hh>
class TFile;
class TTree;

/// Class with singleton instance for managing MC output via ROOT
class RootIO {
public: 
    /// Get class singleton instance
    static RootIO* GetInstance();
    
    /// Get reference for current event
    static Event& GetEvent() { return GetInstance()->mcevent; }
    /// Get reference for current event primaries
    static ParticleEvent& GetPrim() { return GetInstance()->primPtcls; }
    /// Get reference for flux counter
    static ParticleEvent& GetFlux() { return GetInstance()->fluxCounter; }
    /// Get reference for current event neutron captures
    static NCaptEvent& GetNCapt() { return GetInstance()->scintNCapt; }
    /// Get reference for current event scintillator ionization
    static IoniClusterEvent& GetScIoni() { return GetInstance()->scintIoni; }
    /// Get reference for current event muon veto ionization
    static IoniClusterEvent& GetVetoIoni() { return GetInstance()->vetoIoni; }
    
    /// Write data to file
    void WriteFile();
    /// Add current Event to output TTree
    void FillTree();
    
    /// Reset data to initial values
    void Clear();
    /// Set output file
    void SetFileName(G4String filename);
    /// Get output filename
    G4String GetFileName() const { return fname; }

    Event mcevent;              ///< event basic data fill point
    ParticleEvent primPtcls;    ///< event primary particles fill point
    ParticleEvent fluxCounter;  ///< (optional) flux counter fill point
    NCaptEvent scintNCapt;      ///< liquid scintillator neutron captures fill point
    IoniClusterEvent scintIoni; ///< liquid scintillator ionization fill point
    IoniClusterEvent vetoIoni;  ///< muon veto panels ionization fill point
    
    /// record event numbering
    void addEvtBranch();
    /// record primary particles
    void addPrimBranch();
    /// record scintillator ionization
    void addScIoniBranch();
    /// record neutron captures
    void addNCaptBranch();
    /// record fluxCounter contents
    void addFluxBranch();
    /// record muon veto ionization
    void addVetoIoniBranch();
    
protected:
    /// Constructor; protected for singleton instantiation
    RootIO();
  
private:
    
    int writecount;                     ///< count number of events recorded to file
    G4String fname;                     ///< file name
    TFile* outfile;                     ///< output file
    TTree* dataTree;                    ///< output events TTree
    
    Event* pmcevent = NULL;             ///< pointer to mcevent, for TTree setup
    ParticleEvent* pprimPtcls = NULL;   ///< pointer to primPtcls, for TTree setup
    ParticleEvent* pfluxCounter = NULL; ///< pointer to fluxCounter, for TTree setup
    NCaptEvent* pscintNCapt = NULL;     ///< pointer to scintNCapt, for TTree setup
    IoniClusterEvent* pscintIoni = NULL;///< pointer to scintIoni, for TTree setup
    IoniClusterEvent* pvetoIoni = NULL; ///< pointer to vetoIoni, for TTree setup
    
    std::vector<TObject*> subObjs;      ///< list of pointers to write points to Clear()
};

#endif
