#include "IBDModule.hh"

#include <G4Positron.hh>
#include <G4Neutron.hh>
#include <G4ParticleGun.hh>
#include <G4SystemOfUnits.hh>

#include <cassert>

IBDModule::IBDModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "IBD"),
inverse_beta(P->GetVerbosity(), "ScintTank_scint_phys") { 
    primary=true;
}

void IBDModule::GeneratePrimaries(G4Event* anEvent) {
    nIBD++;
    G4ParticleGun* gn = myPGA->GetParticleGun();
    assert(gn);
    if( inverse_beta.IsSequentialGenerated()){
        if(primary){
            primary=false;
            position = inverse_beta.GenerateReactionPosition();
            kinematics = inverse_beta.GenerateReactionKinematics();
            
            G4ThreeVector positronMomentum((*kinematics)[1],(*kinematics)[2],(*kinematics)[3]);
            gn->SetParticlePosition(position);
            gn->SetParticleDefinition(G4Positron::PositronDefinition());
            gn->SetParticleEnergy((*kinematics)[0]);
            gn->SetParticleMomentumDirection(positronMomentum);
            
            gn->GeneratePrimaryVertex(anEvent);
            
        }
        else{
            primary=true;
            G4ThreeVector neutronMomentum((*kinematics)[5],(*kinematics)[6],(*kinematics)[7]);
            gn->SetParticlePosition(position);
            gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
            gn->SetParticleEnergy((*kinematics)[4]);
            gn->SetParticleMomentumDirection(neutronMomentum);
            
            gn->GeneratePrimaryVertex(anEvent);
        }
    }
    else{
        gn->SetParticlePosition(inverse_beta.GenerateReactionPosition());
        
        kinematics = inverse_beta.GenerateReactionKinematics();
        G4ThreeVector positronMomentum((*kinematics)[1],(*kinematics)[2],(*kinematics)[3]);
        G4ThreeVector neutronMomentum((*kinematics)[5],(*kinematics)[6],(*kinematics)[7]);
        
        if( inverse_beta.IsPositronGenerated()){
            gn->SetParticleDefinition(G4Positron::PositronDefinition());
            gn->SetParticleEnergy((*kinematics)[0]);
            gn->SetParticleMomentumDirection(positronMomentum);
            gn->GeneratePrimaryVertex(anEvent);
        }
        if( inverse_beta.IsNeutronGenerated()){
            gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
            gn->SetParticleEnergy((*kinematics)[4]);
            gn->SetParticleMomentumDirection(neutronMomentum);
            gn->GeneratePrimaryVertex(anEvent);
        }
    }
}

G4double IBDModule::GetGeneratorTime() const {
    return nIBD*s;
}
