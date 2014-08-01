#include "CRYModule.hh"

#include "CRYSetup.h"
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYUtils.h"
#include "RNGWrapper.hh"

#include "DetectorConstruction.hh"
#include <G4SystemOfUnits.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleTable.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithoutParameter.hh>

CRYModule::CRYModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "CRY"),
CRY_generator(NULL), inputState(false), primpoint(false) {
   
    const DetectorConstruction* detect = dynamic_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    zOffset = detect->myBuilding.dim[2]/2.;
    
    CRYDir = new G4UIdirectory("/CRY/");
    CRYDir->SetGuidance("CRY initialization");
    
    cryPointCmd = new G4UIcmdWithABool("/CRY/toggleCryPointing",this);
    cryPointCmd->SetGuidance("Toggles ehether CRY preselwects particles pointing towards the detector");
    cryPointCmd->SetGuidance("  true: Enable CRY pointing");
    cryPointCmd->SetGuidance("  false: Disable CRY pointing");
    cryPointCmd->SetParameterName("cryPoint",false);
    cryPointCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    cryZCmd = new G4UIcmdWithADoubleAndUnit("/CRY/zOffset",this);
    cryZCmd->SetGuidance("sets the Z offset for the CRY generator (default is world maximum Z)");
    cryZCmd->SetParameterName("cryZ",false);
    cryZCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    cryInputCmd = new G4UIcmdWithAString("/CRY/input",this);
    cryInputCmd->SetGuidance("CRY input lines");
    cryInputCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    cryUpdateCmd = new G4UIcmdWithoutParameter("/CRY/update",this);
    cryUpdateCmd->SetGuidance("Update CRY definition.");
    cryUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
    cryUpdateCmd->SetGuidance("if you changed the CRY definition.");
    cryUpdateCmd->AvailableForStates(G4State_Idle);
}

CRYModule::~CRYModule() {
    delete CRYDir;
    delete cryPointCmd;
    delete cryZCmd;
    delete cryInputCmd;
    delete cryUpdateCmd;
    if(CRY_generator) delete CRY_generator;
}

void CRYModule::SetNewValue(G4UIcommand* command, G4String newValue) {
    static std::string messageInput;
    if(command == cryInputCmd) { 
        if(myPGA->GetVerbosity() >= 1) G4cerr << "CRY input command: " << newValue << G4endl;
        inputState = true;
        messageInput.append(newValue);
        messageInput.append(" ");
    } else if(command == cryUpdateCmd) { 
        initCRY(messageInput); 
        messageInput = "";
    } else if(command == cryPointCmd) primpoint = cryPointCmd->GetNewBoolValue(newValue);
    else if(command == cryZCmd) zOffset = cryZCmd->GetNewDoubleValue(newValue);
}

G4double CRYModule::GetGeneratorTime() const {
    if(!CRY_generator) return 0;
    return CRY_generator->timeSimulated()*s;
}

void CRYModule::GeneratePrimaries(G4Event* anEvent) {
    
    // verify successful CRY initialization
    if(inputState || !CRY_generator) {
        G4String* str = new G4String("CRY library was not successfully initialized");
        G4Exception("PrimaryGeneratorAction", "1", RunMustBeAborted, *str);
    }
    
    // loop until generating at least one primary
    std::vector<primaryPtcl> v;
    do {
        vect.clear();
        CRY_generator->genEvent(&vect);
        
        uint n_muons = 0;
        uint n_neutrons = 0;
        
        for ( unsigned j=0; j<vect.size(); j++) {
            
            // determine whether to process trajectory
            bool good_point = true;
            /*
             *            i f(fCRYpoint) {    *
             *            //find pointing vector
             *            G4double ray = sqrt((*vect)[j]->x()*(*vect)[j]->x()+(*vect)[j]->y()*(*vect)[j]->y()+(cryZoffset*cryZoffset));
             *       G4double px = (*vect)[j]->x()+ray*(*vect)[j]->u();
             *       G4double py = (*vect)[j]->y()+ray*(*vect)[j]->v();
             *       G4double pz = cryZoffset+ray*(*vect)[j]->w();
             *       
             *       good_point = (sqrt(px*px+py*py+pz*pz)<=(0.0015*length));
             }
             */
            
            if(good_point) {
                primaryPtcl p;
                p.PDGid = vect[j]->PDGid();
                p.KE = vect[j]->ke()*MeV;
                p.pos = G4ThreeVector(vect[j]->x()*m, vect[j]->y()*m, zOffset);
                p.mom = G4ThreeVector(vect[j]->u(), vect[j]->v(), vect[j]->w());
                p.t = vect[j]->t()*s;
                v.push_back(p);
                
                n_muons += (abs(p.PDGid) == 13);
                n_neutrons += (p.PDGid == 2112);
            }
            
            delete vect[j];
        }
        
        //if(!n_neutrons) v.clear(); // only keep neutron events
        
    } while(!v.size());
    
    throwPrimaries(v, anEvent);
}

void CRYModule::initCRY(const std::string& S) {
    if(myPGA->GetVerbosity() >= 1) G4cerr << "Initializing CRY generator..." << G4endl;
    
    CRYSetup *setup = new CRYSetup(S, getenv("CRYDATA"));
    if(CRY_generator) delete CRY_generator;
    CRY_generator = new CRYGenerator(setup);
    
    // set random number generator
    RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(),&CLHEP::HepRandomEngine::flat);
    setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
    inputState = false;
}
