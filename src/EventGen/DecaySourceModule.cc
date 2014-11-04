#include "DecaySourceModule.hh"
#include "PathUtils.hh"

#include <G4SystemOfUnits.hh>
#include <G4ParticleTable.hh>

DecaySourceModule::DecaySourceModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "DecaySource"),
NDL(getEnvSafe("PG4_AUX")+"/NuclearDecays/"),
decaySrcDir("/generator/decaySource/"),
genNameCmd("/generator/decaySource/type",this) {
    genNameCmd.SetGuidance("Set event generator type.");
    genNameCmd.AvailableForStates(G4State_Idle);
}

void DecaySourceModule::SetNewValue(G4UIcommand* command, G4String newValue) {
    static string messageInput;
    if(command == &genNameCmd) { 
        gen_name = newValue;
        printf("Setting up decay generator '%s'...\n", gen_name.c_str());
        if(NDL.hasGenerator(gen_name)) {
            NDL.getGenerator(gen_name).display();
        } else {
            printf("ERROR: Generator '%s' not found!\n", gen_name.c_str());
            gen_name = "";
        }
    }
}

void DecaySourceModule::GeneratePrimaries(G4Event* anEvent) {
    
    if(!gen_name.size()) return;
    
    vector<primaryPtcl> v;
    primaryPtcl p;
    p.pos = G4ThreeVector(0,0,-1*mm);
    p.mom = G4ThreeVector(0,0,1);
    
    while(!v.size()) {
        NDL.getGenerator(gen_name).genDecayChain(evtq);
        while(evtq.size()) {
            p.PDGid = evtq.back().d;
            p.KE = evtq.back().E*keV;
            p.t = evtq.back().t;
            v.push_back(p);
            evtq.pop_back();
        }
    }
    
    throwPrimaries(v, anEvent);
}

void DecaySourceModule::fillNode(TXMLEngine& E) {
    addAttr(E, "type", gen_name);
}
