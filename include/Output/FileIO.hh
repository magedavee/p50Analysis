/// \file FileIO.hh \brief Base class for setting up output files
#ifndef FILEIO_HH
#define FILEIO_HH

#include "Event.hh"
#include <vector>
using std::vector;
#include <string>
using std::string;

/// Base class for managing file IO; subclassed for ROOT or HDF5 output
class FileIO {
public: 
    /// Destructor
    virtual ~FileIO() { }
    
    /// Get class singleton instance
    static FileIO* GetInstance();
    
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
    
    /// Open named output file
    virtual void SetFileName(const string& filename) = 0;
    /// Finalize/close file output
    virtual void WriteFile() = 0;
    /// Add current Event to output file
    virtual void SaveEvent() = 0;
    
    /// Reset data to initial values
    void Clear();
    /// Get output filename
    string GetFileName() const { return fname; }
    
    Event mcevent;              ///< event basic data fill point
    ParticleEvent primPtcls;    ///< event primary particles fill point
    ParticleEvent fluxCounter;  ///< (optional) flux counter fill point
    NCaptEvent scintNCapt;      ///< liquid scintillator neutron captures fill point
    IoniClusterEvent scintIoni; ///< liquid scintillator ionization fill point
    IoniClusterEvent vetoIoni;  ///< muon veto panels ionization fill point
    
    /// record event numbering
    virtual void addEvtBranch() { }
    /// record primary particles
    virtual void addPrimBranch() { }
    /// record scintillator ionization
    virtual void addScIoniBranch() { }
    /// record neutron captures
    virtual void addNCaptBranch() { }
    /// record fluxCounter contents
    virtual void addFluxBranch() { }
    /// record muon veto ionization
    virtual void addVetoIoniBranch() { }
    
protected:
    /// Constructor; protected for singleton instantiation
    FileIO() { }
    
    string fname;                       ///< output file name
    
    Event* pmcevent = NULL;             ///< pointer to mcevent, for TTree setup
    ParticleEvent* pprimPtcls = NULL;   ///< pointer to primPtcls, for TTree setup
    ParticleEvent* pfluxCounter = NULL; ///< pointer to fluxCounter, for TTree setup
    NCaptEvent* pscintNCapt = NULL;     ///< pointer to scintNCapt, for TTree setup
    IoniClusterEvent* pscintIoni = NULL;///< pointer to scintIoni, for TTree setup
    IoniClusterEvent* pvetoIoni = NULL; ///< pointer to vetoIoni, for TTree setup
    
    vector<TObject*> subObjs;           ///< list of pointers to write points to Clear()
};

#endif
