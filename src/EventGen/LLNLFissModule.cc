#include "LLNLFissModule.hh"

#include <G4SystemOfUnits.hh>
#include <G4LLNLFission.hh>

LLNLFissModule::LLNLFissModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "LLNLFiss"),
fissDir("/fissgen/"),
isotCmd("/fissgen/isot",this) {
    fissDir.SetGuidance("LLNL fission event generator commands.");
    isotCmd.SetGuidance("Set fission isotope, e.g. Cf252 = 98252.");
    isotCmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    // use FREYA correlated neutron/gamma generation, maybe...
    // unclear whether or not Geant4.10 actually includes FREYA interface
    G4int correlation_mode = 3;
    setcorrel_(&correlation_mode);
}

void LLNLFissModule::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &isotCmd) isotope =  isotCmd.GetNewIntValue(newValue);
}

void LLNLFissModule::GeneratePrimaries(G4Event* anEvent) {

    vector<primaryPtcl> v;
        
    // generate fission event
    G4double time = 0;
    genspfissevt_(&isotope, &time);
    int nn = getnnu_(); // neutron multiplicity
    int np = getpnu_(); // photon multiplicity
    if(nn < 0 || np < 0) return; // bad isotope!
    
    // fill neutrons
    for(G4int i = 0; i < nn; i++) {
        primaryPtcl p;
        p.PDGid = 2112;
        p.KE = getneng_(&i);
        p.t = getnage_ (&i);
        p.mom = G4ThreeVector(getndircosu_(&i), getndircosv_(&i), getndircosw_(&i));
        v.push_back(p);
    }
    // fill photons
    for(G4int i = 0; i < np; i++) {
        primaryPtcl p;
        p.PDGid = 22;
        p.KE = getpeng_(&i);
        p.t = getpage_ (&i);
        p.mom = G4ThreeVector(getpdircosu_(&i), getpdircosv_(&i), getpdircosw_(&i));
        v.push_back(p);
    }
    
    setVertices(v);
    throwPrimaries(v, anEvent);
}

void LLNLFissModule::fillNode(TXMLEngine& E) {
    addAttrI(E, "isot", isotope);
}
