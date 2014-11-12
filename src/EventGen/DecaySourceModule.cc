#include "DecaySourceModule.hh"
#include "PathUtils.hh"

#include <G4SystemOfUnits.hh>
#include <G4ParticleTable.hh>

DecaySourceModule::DecaySourceModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "DecaySource"),
NDL(getEnvSafe("PG4_AUX")+"/NuclearDecays/"),
decaySrcDir("/generator/decaySource/"),
genNameCmd("/generator/decaySource/type",this),
ptclCmd("/generator/decaySource/particle",this),
singleCmd("/generator/decaySource/throwSingle",this) {
    
    genNameCmd.SetGuidance("Set event generator type.");
    genNameCmd.AvailableForStates(G4State_Idle);
    
    ptclCmd.SetGuidance("Restrict particle types thrown.");
    ptclCmd.AvailableForStates(G4State_Idle);
    
    singleCmd.SetGuidance("Whether to only throw one particle at a time.");
    singleCmd.AvailableForStates(G4State_Idle);
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
    } else if(command == &ptclCmd) ptypes.insert(ptclCmd.GetNewIntValue(newValue));
    else if(command == &singleCmd) throwSingle = singleCmd.GetNewBoolValue(newValue);
}

void DecaySourceModule::GeneratePrimaries(G4Event* anEvent) {
    
    if(!gen_name.size()) return;
    
    vector<primaryPtcl> v;

    while(!v.size()) {
        if(!evtq.size()) {
            NDL.getGenerator(gen_name).genDecayChain(evtq);
            nDecays++;
        }
        
        while(evtq.size()) {
            primaryPtcl p;
            p.PDGid = evtq.back().d;
            p.KE = evtq.back().E*keV;
            p.t = evtq.back().t;
            evtq.pop_back();
            
            if(ptypes.size() && !ptypes.count(p.PDGid)) continue;
            v.push_back(p);
            if(throwSingle) break;
        }
        
        if(v.size() && !tryVertex(v)) v.clear();
    }
    
    throwPrimaries(v, anEvent);
}

G4double DecaySourceModule::GetGeneratorTime() const { return nDecays*s; }

void DecaySourceModule::fillNode(TXMLEngine& E) {
    addAttr(E, "type", gen_name);
    addAttrI(E, "nDecays", nDecays);
    for(auto it = ptypes.begin(); it != ptypes.end(); it++)
        addAttrI(E,"pType",*it);
    if(throwSingle) addAttr(E, "single", "true");
}
