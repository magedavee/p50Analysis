
#ifndef INCLUDE_ROOTIO_HH 
#define INCLUDE_ROOTIO_HH 1

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TBranch.h"
#include "Event.hh"
#include "G4ios.hh"
#include "G4String.hh"
#include <vector>
#include <cassert>

using namespace std;

/** @class rootio rootio.hh include/rootio.hh
 *   
 *
 *  @author Witold POKORSKI
 *  @date   2005-10-27
 */

class RootIO {
public: 
  
    static RootIO* GetInstance();
    void WriteFile();
    void FillTree();
    Event& GetEvent() { return mcevent; }
    
    void Clear() { mcevent.Clear(); }
    void SetFileName(G4String);
    G4String GetFileName() const { return filename; }

protected:
    /// Constructor; protected for singleton instantiation
    RootIO(); 
  
private:
    int writecount;
    TFile* outfile;
    TTree* dataTree;
    Event mcevent;      ///< event data fill point
    Event* pmcevent;    ///< pointer to mcevent, for TTree setup
    Run* mcrun;
    int Nevents;
    int Ntracks;
    G4String filename;
};


#endif
