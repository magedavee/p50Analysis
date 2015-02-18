/// \file FileIO.hh \brief ROOT output trees setup.
#ifndef INCLUDE_ROOTIO_HH
/// Assure this header file is loaded only once
#define INCLUDE_ROOTIO_HH

#include "FileIO.hh"
class TFile;
class TTree;

/// Class with singleton instance for managing MC output via ROOT
class RootIO: public FileIO {
public: 
    /// Constructor
    RootIO();
    /// Destructor
    virtual ~RootIO() { }
    
    /// Open named output file
    virtual void SetFileName(G4String filename);
    /// Finalize/close file output
    virtual void WriteFile();
    /// Add current Event to output file
    virtual void SaveEvent();
    
    /// record event numbering
    virtual void addEvtBranch();
    /// record primary particles
    virtual void addPrimBranch();
    /// record scintillator ionization
    virtual void addScIoniBranch();
    /// record neutron captures
    virtual void addNCaptBranch();
    /// record fluxCounter contents
    virtual void addFluxBranch();
    /// record muon veto ionization
    virtual void addVetoIoniBranch();
    
private:
    
    int writecount;     ///< count number of events recorded to file
    TFile* outfile;     ///< output file
    TTree* dataTree;    ///< output events TTree
};

#endif
