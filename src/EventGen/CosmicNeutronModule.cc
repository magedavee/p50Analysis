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
PrimaryGeneratorModule(P, "CosmicNeutron"), myDist(NULL),
cosn_dir("/generator/cosmic_n/"),
tscale_cmd("/generator/cosmic_n/thermal",this),
sscale_cmd("/generator/cosmic_n/scale",this),
smod_cmd("/generator/cosmic_n/solar_mod",this),
rc_cmd("/generator/cosmic_n/rc",this),
d_cmd("/generator/cosmic_n/depth",this),
w_cmd("/generator/cosmic_n/water",this),
t_cmd("/generator/cosmic_n/Ethermal",this){
    
    setParameters(0.5*GV, 3.47*GV, 1016*g/cm2, 0.2);        // Nashville, TN
    // setParameters(s_min, 2.00*GV, 1010.9*g/cm2, 0.2);    // Goldhagen "Watson Roof"
    // setParameters(s_min, 2.7*GV, 1030*g/cm2, 0.2);       // Sato-Niita fig. 1A bottom
    
    tscale_cmd.SetGuidance("Set scale factor for thermal neutron peak.");
    tscale_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    sscale_cmd.SetGuidance("Set scale factor for non-thermal spectrum.");
    sscale_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    smod_cmd.SetGuidance("Set solar modulation in GV.");
    smod_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    rc_cmd.SetGuidance("Set rigidity cutoff in GV.");
    rc_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    d_cmd.SetGuidance("Set atmospheric depth in g/cm^2.");
    d_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    w_cmd.SetGuidance("Set ground water fraction.");
    w_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    t_cmd.SetGuidance("Set thermal neutron characteristic energy.");
    t_cmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
}

void CosmicNeutronModule::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &tscale_cmd) { scale_T =  tscale_cmd.GetNewDoubleValue(newValue); resetDistribution(); }
    else if(command == &sscale_cmd) { scale_S =  sscale_cmd.GetNewDoubleValue(newValue); resetDistribution(); }
    else if(command == &smod_cmd) { s_mod = smod_cmd.GetNewDoubleValue(newValue)*GV; resetDistribution(); }
    else if(command == &rc_cmd) { r_c = rc_cmd.GetNewDoubleValue(newValue)*GV; resetDistribution(); }
    else if(command == &d_cmd) { depth = d_cmd.GetNewDoubleValue(newValue)*g/cm2; resetDistribution(); }
    else if(command == &w_cmd) { waterFrac = w_cmd.GetNewDoubleValue(newValue); resetDistribution(); }
    else if(command == &t_cmd) { E_T = t_cmd.GetNewDoubleValue(newValue); resetDistribution(); }
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
    calcFluxNorm();
    
    const unsigned int nBins = 1000;
    const double eMin = 1e-3*eV;
    const double eMax = 1e10*eV;
    Double_t binEdges[nBins+1];
    for(unsigned int i=0; i<=nBins; i++)
        binEdges[i] = exp((nBins-i)*log(eMin)/nBins + i*log(eMax)/nBins);
    myDist = new TH1F("hSatoNiita","Neutron energy spectrum",nBins,binEdges);
    
    thermalFlux = 0;
    for(unsigned int i=1; i<=nBins; i++) {
        double E = myDist->GetBinCenter(i);
        double EdPhidE = calcGroundSpectrum(E);
        assert(EdPhidE >= 0.);
        myDist->SetBinContent(i, EdPhidE*myDist->GetBinWidth(i)/E);
        thermalFlux += phi_T_scaled*myDist->GetBinWidth(i)/E;
    }
    netFlux = myDist->Integral();
    
    myDist->Write();
}

G4double CosmicNeutronModule::GetGeneratorTime() const {
    return myPGA->GetCosineThrower()->getAttempts() / myPGA->GetCosineThrower()->getOriginArea() / netFlux;
}

void CosmicNeutronModule::fillNode(TXMLEngine& E) {
    addAttr(E, "flux", to_str(netFlux*(1000*s*cm2))+" mHz/cm2");
    addAttr(E, "thermalFlux", to_str(thermalFlux*(1000*s*cm2))+" mHz/cm2");
    addAttr(E, "s_mod", G4BestUnit(s_mod,"Electric potential"));
    addAttr(E, "depth", G4BestUnit(depth,"Mass/Surface"));
    addAttr(E, "r_c", G4BestUnit(r_c,"Electric potential"));
    addAttr(E, "phi_L", to_str(phi_L*(1000*s*cm2))+" mHz/cm2");
    addAttr(E, "w", waterFrac);
    addAttr(E,"E_T",G4BestUnit(E_T,"Energy"));
    if(scale_S != 1) addAttr(E, "scale_S", scale_S);
    if(scale_T != 1) addAttr(E, "scale_T", scale_T);
}
