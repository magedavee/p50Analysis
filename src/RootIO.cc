#include <sstream>

#include "RootIO.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"
//
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4ios.hh"
//

static RootIO* instance = 0;

RootIO::RootIO():Nevents(0)
{
  RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
  fill = 0;
  if(run_action->GetRecordLevel()==3) fill = 1;
  writecount=0;
}

RootIO::~RootIO()
{}

RootIO* RootIO::GetInstance()
{
  if (instance == 0 )
  {
    instance = new RootIO();
  }
  return instance;
}

void RootIO::WriteFile()
{
  RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
  if(run_action->GetRecordLevel()>=0){
    Nevents++;
    
    std::ostringstream os;
    os << Nevents;
    std::string stevt = "Event_" + os.str(); 
    
    std::cout << "writing " << stevt << std::endl;
    dataTree->Write();
    std::cout << "closing " <<std::endl;
    outfile->Write();
    outfile->Close();
    std::cout << "closed " <<std::endl;
    delete outfile;
    std::cout << "deleted " <<std::endl;
  }
}

void RootIO::FillTree(){
  RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
  if(run_action->GetRecordLevel()>=0){

    mcevent->fNph=0;
    mcevent->fSph=0;
    for(int i=0;i<mcevent->fNSeg;i++){
      mcevent->fNph+=mcevent->fNpmt[i];
      mcevent->fSph+=mcevent->fSpmt[i];
    }
    dataTree->Fill(); 
    if(run_action->GetRecordLevel()==3) fill=1;
    else fill=0;
    writecount++;
    if(writecount==500){
      dataTree->AutoSave("SaveSelf");
      writecount=0;
    }
    Nevents++;
    mcevent->Clear();
  }
  else mcevent->Clear();
}

Event* RootIO::GetEvent(){
  return mcevent; 
}

Track* RootIO::GetTrack(Int_t tn){
  for(uint count = 0; count < mctrack.size(); count++) {
    if(mctrack[count]->tID == tn)
      return mctrack[count];
  }
  G4cerr << "ERROR: track not found " << tn << "\t" << mctrack.size() << G4endl;
  return 0; 
}

void RootIO::SetFileName(G4String fname)
{
 RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
 DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());   
 mcevent = new Event(detector->GetNSegX(),detector->GetNSegY());
 mcevent->Clear();
  if(run_action->GetRecordLevel()>=0){
    G4cerr << "ROOTIO: setting filename to " <<fname<< G4endl;
    filename = fname;
    G4cerr << "Root output files is set to " << filename << G4endl;
    TSystem ts;
    gDebug = 0;
    outfile = new TFile(filename,"RECREATE");  // set up output file
    dataTree = new TTree("sblmc","Short Baseline MC data");
    dataTree->SetDirectory(outfile);
    dataTree->Branch("MCEvent",&mcevent);
    
    mcrun = new Run();
    mcrun->Clear();
    mcrun->NSegX = detector->GetNSegX();
    mcrun->NSegY = detector->GetNSegY();
    for(int i=0;i< mcrun->NSegX*mcrun->NSegY;i++){
      mcrun->PositionX[i] = detector->GetPositionX(i);
      mcrun->PositionY[i] = detector->GetPositionY(i);
    }
    mcrun->AirGap = detector->GetAirGap();
    mcrun->WrapGap = detector->GetWrapGap();
    mcrun->WrapThickness = detector->GetWrapThickness();		
    mcrun->AcrylThickness = detector->GetAcrylThickness();       
    mcrun->SegBuffer = detector->GetSegBuffer();	
    mcrun->ScintLength = detector->GetScintLength();
    mcrun->ScintHeight = detector->GetScintHeight();
    mcrun->ScintWidth = detector->GetScintWidth();
    mcrun->SegLength = detector->GetSegLength();	
    mcrun->SegWidth = detector->GetSegWidth();	
    mcrun->SegHeight = detector->GetSegHeight();	
    mcrun->PMTscale = detector->GetPMTscale();                 
    mcrun->ShieldLead = detector->GetShieldLead();
    mcrun->ShieldPolyB = detector->GetShieldPolyB();
    mcrun->ShieldPolyLi = detector->GetShieldPolyLi();
    mcrun->Vertical = detector->GetVertical();
    // mcrun->Scint = detector->GetScint();
    mcrun->Shielded = detector->GetShieldInclusion();
    mcrun->Reflectivity = detector->GetReflectivity();
    mcrun->Write("RunInfo");
  }
}

void RootIO::AddTrack(Int_t tn){
  Track *temp = new Track();
  temp->Clear();
  temp->tID = tn;
  mctrack.push_back(temp);
  }


