
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
    Event* GetEvent();
    void AddTrack(Int_t);
    void Clear() { mctrack.clear(); mcevent->Clear(); }
    Track* GetTrack(Int_t);
    void SetFileName(G4String);
    G4String GetFileName() const { return filename; }
    bool GetFill() const { return fill; }
    void SetFill(bool f){ fill=f; }

protected:
    /// Constructor; protected for singleton instantiation
    RootIO(); 
  
private:
    int writecount;
    TFile* outfile;
    TTree* dataTree;
    Event* mcevent;
    vector<Track*> mctrack;
    Run* mcrun;
    int Nevents;
    int Ntracks;
    G4String filename;
    bool fill;
};


#endif
