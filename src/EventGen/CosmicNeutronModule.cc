#include <TH1F.h>
#include "CosmicNeutronModule.hh"
#include "DetectorConstruction.hh"

#include <G4Neutron.hh>
#include <G4ParticleGun.hh>
#include <G4ios.hh>
#include <G4UnitsTable.hh>

#include <cassert>

CosmicNeutronModule::CosmicNeutronModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "CosmicNeutron"),
SurfaceThrower(myPGA->GetDetector()->theWorld),
myDist(NULL), waterFrac(0.01) {
    setSourceTarget(myPGA->GetDetector()->ptclSrc, myPGA->GetDetector()->myBuilding.det_phys);
}

void CosmicNeutronModule::GeneratePrimaries(G4Event* anEvent) {    
    
    G4ParticleGun* gn = myPGA->GetParticleGun();
    assert(gn);
    gn->SetParticleDefinition(G4Neutron::NeutronDefinition());
    
    while(!genThrow()) { }
    gn->SetParticlePosition(pos);
    gn->SetParticleMomentumDirection(mom);
    
    if(!myDist) makeDistribution();
    double E = myDist->GetRandom();
    gn->SetParticleEnergy(E);
    
    gn->GeneratePrimaryVertex(anEvent);
}

void CosmicNeutronModule::makeDistribution() {
    if(myDist) delete myDist;
    
    const unsigned int nBins = 1000;
    const double eMin = 1e-3*eV;
    const double eMax = 1e10*eV;
    Double_t binEdges[nBins+1];
    for(unsigned int i=0; i<=nBins; i++)
        binEdges[i] = exp((nBins-i)*log(eMin)/nBins + i*log(eMax)/nBins);
    myDist = new TH1F("hSatoNiita","Neutron energy spectrum",nBins,binEdges);
    
    /*
    for(int i=1; i<=11; i++) G4cout << "a_" << i << " = " << a[i] << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "1 = " << b_i1[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "2 = " << b_i2[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "3 = " << b_i3[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "4 = " << b_i4[i]; } G4cout << "\n";
    */
    
    for(unsigned int i=1; i<=nBins; i++) {
        double E = myDist->GetBinCenter(i);
        double EdEdPhi = calcGroundSpectrum(E,waterFrac);
        //G4cout << g_1 << "\t" << g_2 << "\t" << G4BestUnit(g_3,"Energy") << "\t" << g_4 << "\t"<< G4BestUnit(g_5,"Energy") << "\n";
        assert(EdEdPhi >= 0.);
        myDist->SetBinContent(i,EdEdPhi/E*myDist->GetBinWidth(i));
        //G4cout << "E = " << G4BestUnit(E,"Energy") << "\tphi_B = " << phi_B << "\tphi_L = " << phi_L
        //    << "\tphi_T = " << phi_T << "\tf_G = " << f_G << "\tE*dPhi/dE = " << EdEdPhi << G4endl;
    }
}

G4ThreeVector CosmicNeutronModule::proposeDirection() {
    return G4ThreeVector(-1./sqrt(3.), -1./sqrt(3.), -1./sqrt(3.));
}
