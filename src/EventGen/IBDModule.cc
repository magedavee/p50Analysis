#include "IBDModule.hh"
#include "strutils.hh"
#include <cassert>
#include <G4UnitsTable.hh>

IBDModule::IBDModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "IBD"),
InverseBetaKinematics(P->GetVerbosity()),
invNeutCmd("/generator/module/IBD/setNeutrons",this),
invPosiCmd("/generator/module/IBD/setPositrons",this),
invSeqCmd("/generator/module/IBD/setSequential",this),
invNuCmd("/generator/module/IBD/setAntinus",this),
Antinus(false), Neutrons(true), Positrons(true), Sequential(false) {
    invNeutCmd.SetGuidance("Set IBD module to generate neutrons.");
    invNeutCmd.SetParameterName("neutrons",true);
    invNeutCmd.SetDefaultValue(true);
    invNeutCmd.AvailableForStates(G4State_Idle);
    
    invPosiCmd.SetGuidance("Set IBD module to generate positrons.");
    invPosiCmd.SetParameterName("positrons",true);
    invPosiCmd.SetDefaultValue(true);
    invPosiCmd.AvailableForStates(G4State_Idle);
    
    invSeqCmd.SetGuidance("Set IBD module to alternately generate positrons and neutrons.");
    invSeqCmd.SetParameterName("sequential",true);
    invSeqCmd.SetDefaultValue(true);
    invSeqCmd.AvailableForStates(G4State_Idle);
    
    invNuCmd.SetGuidance("Set IBD module to generate antineutrinos.");
    invNuCmd.SetParameterName("antinus",true);
    invNuCmd.SetDefaultValue(false);
    invNuCmd.AvailableForStates(G4State_Idle);
}

void IBDModule::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &invNeutCmd) ToggleNeutronGeneration(invNeutCmd.GetNewBoolValue(newValue));
    else if(command == &invPosiCmd) TogglePositronGeneration(invPosiCmd.GetNewBoolValue(newValue));
    else if(command == &invSeqCmd) ToggleSequentialGeneration(invSeqCmd.GetNewBoolValue(newValue));
    else if(command == &invNuCmd) ToggleAntinuGeneration(invNuCmd.GetNewBoolValue(newValue));
}

void IBDModule::GeneratePrimaries(G4Event* anEvent) {
    
    while(!vToThrow.size()) {
        VertexPositioner* VP = myPGA->GetPositioner();
        VP->proposePosition();
        nuDirection = (VP->pos - VP->originPoint).unit();
        kinematics = GenerateReactionKinematics();
        
        if(Positrons) {
            primaryPtcl p;
            p.PDGid = -11; // positron
            p.mom = G4ThreeVector(kinematics[1],kinematics[2],kinematics[3]);
            p.KE = kinematics[0];
            p.t = 0;
            vToThrow.push_back(p);
        }
        if(Neutrons){
            primaryPtcl p;
            p.PDGid = 2112; // neutron
            p.mom = G4ThreeVector(kinematics[5], kinematics[6], kinematics[7]);
            p.KE = kinematics[4];
            p.t = 0;
            vToThrow.push_back(p);
        }
        
        if(!tryVertex(vToThrow)) {
            vToThrow.clear();
            continue;
        }
        
        if(Antinus) {
            vector<primaryPtcl> v = vToThrow;
            vToThrow.clear();
            
            primaryPtcl p;
            p.PDGid = -12; // electron antineutrino
            p.KE = kinematics[10];
            p.t = 0;
            p.pos = VP->originPoint;
            p.mom = nuDirection;
            
            vToThrow.push_back(p);
            vToThrow.insert(vToThrow.end(), v.begin(), v.end());
        }
    }
    
    if(Sequential) {
        vector<primaryPtcl> onePtcl;
        onePtcl.push_back(vToThrow.back());
        throwPrimaries(onePtcl, anEvent);
        vToThrow.pop_back();
    } else {
        throwPrimaries(vToThrow, anEvent);
        vToThrow.clear();
    }
}

void IBDModule::ToggleNeutronGeneration(G4bool neutron) {
    Neutrons = neutron;
    if(Neutrons) { G4cout << "Neutron generation enabled. Simulation will shoot neutrons according to this module's distribution." << G4endl; }
    else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
    else { G4cout << "Neutron generation disabled. Simulation will not shoot neutrons generated through this module's distribution." << G4endl; }
}

void IBDModule::TogglePositronGeneration(G4bool positron) {
    Positrons = positron;
    if(Positrons) { G4cout << "Positron generation enabled. Simulation will shoot positrons according to this module's distribution." << G4endl; }
    else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
    else { G4cout << "Positron generation disabled. Simulation will not shoot positrons generated through this module's distribution." << G4endl; }
}

void IBDModule::ToggleSequentialGeneration(G4bool sequential) {
    Sequential = sequential;
    if(Sequential) { G4cout << "Sequential generation enabled. Simulation will shoot positrons and neutrons according to this module's distribution." << G4endl;}
    else if(!Neutrons && ! Positrons && !Sequential) { G4cout << "*** WARNING: Simulation currently does not shoot any particles. Program may crash if run in this state! ***" << G4endl; }
    else { G4cout << "Sequential generation disabled. Simulation will not shoot particles generated through this module's distribution." << G4endl; }
}

void IBDModule::ToggleAntinuGeneration(G4bool b) {
    Antinus = b;
    if(Antinus)  G4cout << "IBD module will generate un-captured antineutrinos as dummies to save the primary antineutrino information." << G4endl;
    else G4cout << "IBD module will generate neutrino IBD captures." << G4endl;
}

void IBDModule::fillNode(TXMLEngine& E) {
    vector<string> throwing;
    if(Antinus) throwing.push_back("antinu");
    else {
        if(Sequential || Positrons) throwing.push_back("positron");
        if(Sequential || Neutrons) throwing.push_back("neutron");
    }
    addAttr(E, "throwing", join(throwing,","));
    if(Sequential) addAttr(E, "mode", "sequential");
    if(antiNuMonoEnergy) {
        addAttr(E, "Enu", G4BestUnit(antiNuMonoEnergy,"Energy"));
    } else {
        if(U235) addAttr(E, "U235", U235);
        if(U238) addAttr(E, "U238", U238);
        if(Pu239) addAttr(E, "Pu239", Pu239);
        if(Pu241) addAttr(E, "Pu241", Pu241);
    }
}
