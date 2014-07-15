
#ifndef INCLUDE_ROOTIO_HH 
#define INCLUDE_ROOTIO_HH 1

// Include files
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

class RootIO 
{
public: 
  virtual ~RootIO();
  
  static RootIO* GetInstance();
  void WriteFile();
  void FillTree();
  Event* GetEvent();
  void AddTrack(Int_t);
  void Clear(){mctrack.clear();mcevent->Clear();};
  Track* GetTrack(Int_t);
  G4String filename;
  void SetFileName(G4String);
  G4String GetFileName(){return filename;};
  int GetFill(){return fill;};
  void SetFill(Int_t f){fill=f;};

protected:
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
  int fill;
};


#endif // INCLUDE_ROOTIO_HH
