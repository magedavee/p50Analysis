#ifndef INCLUDE_ROOTIO_HH
/// Assure this header file is loaded only once
#define INCLUDE_ROOTIO_HH

#include "Event.hh"

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
    /// Get reference for flux counter
    static ParticleEvent& GetFlux() { return GetInstance()->fluxCounter; }
    
    /// Write data to file
    void WriteFile();
    /// Add current Event to output TTree
    void FillTree();
    
    /// Reset data to initial values
    void Clear() { mcevent.Clear(); fluxCounter.Clear(); }
    /// Set output file
    void SetFileName(G4String filename);
    /// Get output filename
    G4String GetFileName() const { return fname; }

    Event mcevent;              ///< event data fill point
    ParticleEvent fluxCounter;  ///< (optional) flux counter fill point
    
    /// record fluxCounter contents
    void addFluxBranch();
    
protected:
    /// Constructor; protected for singleton instantiation
    RootIO();
  
private:
    
    int writecount;                     ///< count number of events recorded to file
    G4String fname;                     ///< file name
    TFile* outfile;                     ///< output file
    TTree* dataTree;                    ///< output events TTree
    Event* pmcevent;                    ///< pointer to mcevent, for TTree setup
    ParticleEvent* pfluxCounter;        ///< pointer to fluxCounter, for TTree setup

};

#endif
