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
    /// Write data to file
    void WriteFile();
    /// Add current Event to output TTree
    void FillTree();
    /// Get reference for current event
    Event& GetEvent() { return mcevent; }
    
    /// Reset data to initial values
    void Clear() { mcevent.Clear(); }
    /// Set output file
    void SetFileName(G4String filename);
    /// Get output filename
    G4String GetFileName() const { return fname; }

    Event mcevent;      ///< event data fill point
    
protected:
    /// Constructor; protected for singleton instantiation
    RootIO();
  
private:
    
    int writecount;     ///< count number of events recorded to file
    G4String fname;     ///< file name
    TFile* outfile;     ///< output file
    TTree* dataTree;    ///< output events TTree
    Event* pmcevent;    ///< pointer to mcevent, for TTree setup

};

#endif
