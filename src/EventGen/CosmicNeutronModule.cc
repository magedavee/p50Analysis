#include <TH1F.h>
#include "CosmicNeutronModule.hh"
#include "DetectorConstruction.hh"

#include <G4Neutron.hh>
#include <G4ParticleGun.hh>
#include <G4ios.hh>
#include <G4UnitsTable.hh>
#include <G4VSolid.hh>
#include <Randomize.hh>

#include <cassert>

CosmicNeutronModule::CosmicNeutronModule(PrimaryGeneratorAction* P):
PrimaryGeneratorModule(P, "CosmicNeutron"), myDist(NULL) {
    setParameters(0.5*GV, 3.47*GV, 1016*g/cm2, 0.2);        // Nashville, TN
    // setParameters(s_min, 2.00*GV, 1010.9*g/cm2, 0.2);       // Goldhagen "Watson Roof"
    // setParameters(s_min, 2.7*GV, 1030*g/cm2, 0.2);       // Sato-Niita fig. 1A bottom
}

void CosmicNeutronModule::GeneratePrimaries(G4Event* anEvent) {    
    primaryPtcl p;
    p.PDGid = 2112;
    if(!myDist) makeDistribution();
    p.KE = myDist->GetRandom();
    p.t = 0;
    
    vector<primaryPtcl> v;
    v.push_back(p);
    setVertices(v);
    throwPrimaries(v,anEvent);
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
    
 
    //G4cout << "a_1 = " << a[1]*cm*cm*s << "/cm^2/s\n";
    //G4cout << "a_2 = " << a[2]/(cm*cm/g) << " cm^2/g\n";
    //G4cout << "a_3 = " << a[3] << "\n";
    //G4cout << "a_4 = " << a[4]/(cm*cm/g) << " cm^2/g\n";
    
    /*
    for(int i=1; i<=11; i++) G4cout << "a_" << i << " = " << a[i] << "\t"; G4cout << "\n";*
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "1 = " << b_i1[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "2 = " << b_i2[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "3 = " << b_i3[i]; } G4cout << "\n";
    for(int i=1; i<=5; i++) { G4cout << "\tb_" << i << "4 = " << b_i4[i]; } G4cout << "\n";
    */
    
    for(unsigned int i=1; i<=nBins; i++) {
        double E = myDist->GetBinCenter(i);
        double EdPhidE = calcGroundSpectrum(E);
        //G4cout << "g_i = " << g_1 << "\t" << g_2 << "\t" << G4BestUnit(g_3,"Energy") << "\t" << g_4 << "\t"<< G4BestUnit(g_5,"Energy") << "\n";
        assert(EdPhidE >= 0.);
        myDist->SetBinContent(i, EdPhidE*myDist->GetBinWidth(i)/E);
        //G4cout << "E = " << G4BestUnit(E,"Energy") << "\tphi_B = " << phi_B << "\tphi_L = " << phi_L*cm*cm*s
        //    << "/cm2/s\tphi_T = " << phi_T << "\tf_G = " << f_G << "\tE*dPhi/dE = " << EdPhidE << G4endl;
    }
    netFlux = myDist->Integral();
}

G4double CosmicNeutronModule::GetGeneratorTime() const {
    return myPGA->GetCosineThrower()->getAttempts() / myPGA->GetCosineThrower()->getOriginArea() / netFlux;
}

void CosmicNeutronModule::fillNode(TXMLEngine& E) {
    addAttr(E, "flux", to_str(netFlux*(1000*s*cm2))+" mHz/cm2");
    addAttr(E, "s_mod", G4BestUnit(s_mod,"Electric potential"));
    addAttr(E, "depth", G4BestUnit(depth,"Mass/Surface"));
    addAttr(E, "r_c", G4BestUnit(r_c,"Energy"));
    addAttr(E, "w", waterFrac);
}
