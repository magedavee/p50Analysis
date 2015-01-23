#include "FissionAntiNuModule.hh"

#include "DetectorConstruction.hh"
#include "FissionAntiNuMessenger.hh"

#include <G4SystemOfUnits.hh>
#include <G4AntiNeutrinoE.hh>
#include <Randomize.hh>
#include <G4ParticleGun.hh>

FissionAntiNuModule::FissionAntiNuModule(PrimaryGeneratorAction* P): PrimaryGeneratorModule(P, "FissionAntiNu"),
U235(1.06), U238(0.057), Pu239(0.0), Pu241(0.0), fiss_messenger(new FissionAntiNuMessenger(this)) { 

  char header[200];
  G4double dummy;
  G4int cnt=0;
  char pnd = '#';
  ifstream infile("antineutrinoSpectra_DwyerLangford_1keV_v0.txt",std::ifstream::in);
  if(!infile.is_open()) G4cerr<<"Problem opening input file"<<G4endl;
  while (infile.peek()==int(pnd)){
    infile.getline(header,200);
  }
  while(infile>>dummy>>Dwyer235[cnt]>>Dwyer238[cnt]>>Dwyer239[cnt]>>Dwyer241[cnt]){
    cnt++;
  }
  infile.close();
  dwyer = false;
}

FissionAntiNuModule::~FissionAntiNuModule() { delete fiss_messenger; }

void FissionAntiNuModule::GeneratePrimaries(G4Event* anEvent) {    
    G4ParticleGun* gun = myPGA->GetParticleGun();
    assert(gun);
    gun->SetParticleDefinition(G4AntiNeutrinoE::AntiNeutrinoEDefinition());
    
    /// neutrinos coming straight up from under floor
    G4ThreeVector wsize = myPGA->GetDetector()->myBuilding.getDimensions();
    G4double positionZ = -wsize[0]/2.;
    G4double positionX = wsize[1]*(G4UniformRand()-0.5);
    G4double positionY = wsize[2]*(G4UniformRand()-0.5);
    gun->SetParticlePosition(G4ThreeVector(positionX,positionY,positionZ));
    if(dwyer)  gun->SetParticleEnergy(GenerateDwyerAntiNeutrinoEnergy());
    else gun->SetParticleEnergy(GenerateAntiNeutrinoEnergy());
    gun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
    gun->GeneratePrimaryVertex(anEvent);
}

G4double FissionAntiNuModule::GenerateAntiNeutrinoEnergy() const {
  G4double energy, randNo, Probability;
  if(dwyer) {
    energy = GenerateDwyerAntiNeutrinoEnergy();
  }
  else{
    do {
      
      // Generate acceptance parameter
      randNo = G4UniformRand();
      
      // Generate energy
      energy = 8.*G4UniformRand() + 1.8;
      
      // Calculate acceptance value - NormFactor is the maximum value of distribution to obtain a fraction
      G4double Phi = 0.; G4double NormFactor = 0.0;  G4double xsec = 0.0;
      Phi += U235*CalculateU235Spectrum(energy);
      Phi += Pu239*CalculatePu239Spectrum(energy);
      Phi += Pu241*CalculatePu241Spectrum(energy);
      Phi += U238*CalculateU238Spectrum(energy);
      
      //  NormFactor += U235*2.07217;
      NormFactor += U235*2.01612;
      NormFactor += Pu239*1.49546;     // equation 9 in Vogel and Beacom
      NormFactor += Pu241*1.98477;
      NormFactor += U238*2.82543;
      xsec = (energy-1.294)*(sqrt((energy-1.294)*(energy-1.294)-0.511*0.511));
      
      Probability = xsec*Phi / NormFactor;
    } while (randNo > Probability);               // Acceptance condition
   }
   return energy*MeV;
}

G4double FissionAntiNuModule::GenerateDwyerAntiNeutrinoEnergy() const {
    G4double energy, randNo, Probability;
    G4int intE;
    do {
        // Generate acceptance parameter
        randNo = G4UniformRand();
        
        // Generate energy
        energy = 10.199*G4UniformRand() + 1.8;
        
        // Calculate acceptance value - NormFactor is the maximum value of distribution to obtain a fraction
        G4double Phi = 0.; G4double NormFactor = 0.0;  G4double xsec = 0.0;
	intE = 1000*energy-1800;
        Phi += U235*Dwyer235[G4int(intE)];
        Phi += Pu239*Dwyer239[G4int(intE)];
        Phi += Pu241*Dwyer241[G4int(intE)];
        Phi += U238*Dwyer238[G4int(intE)];

	NormFactor += U235*2.04e-03;
	NormFactor += Pu239*1.58e-03;     // equation 9 in Vogel and Beacom
	NormFactor += Pu241*2.00e-03;
	NormFactor += U238*2.76e-03;
	xsec = (energy-1.294)*(sqrt((energy-1.294)*(energy-1.294)-0.511*0.511));
	
	Probability = xsec*Phi / NormFactor;
    } while (randNo > Probability);               // Acceptance condition
    return energy*MeV;
}

G4double FissionAntiNuModule::CalculateU235Spectrum(G4double eNu) const {
  //  G4double a_1 = 3.519e+0, a_2 = -3.517e+0, a_3 = 1.595e+0, a_4 = -4.171e-1, a_5 = 5.004e-2, a_6 = -2.303e-3;
  G4double a_1 = 1.418, a_2 = -0.6078, a_3 = 8.955e-3, a_4 = -6.69e-3, a_5 = 6.933e-5, a_6 = 0;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double FissionAntiNuModule::CalculatePu239Spectrum(G4double eNu) const {
    G4double a_1 = 2.560e+0, a_2 = -2.654e+0, a_3 = 1.256e+0, a_4 = -3.617e-1, a_5 = 4.547e-2, a_6 = -2.143e-3;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double FissionAntiNuModule::CalculatePu241Spectrum(G4double eNu) const {
    G4double a_1 = 1.487e+0, a_2 = -1.038e+0, a_3 = 4.130e-1, a_4 = -1.423e-1, a_5 = 1.866e-2, a_6 = -9.229e-4;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double FissionAntiNuModule::CalculateU238Spectrum(G4double eNu) const {
    G4double a_1 = 9.760e-1, a_2 = -1.620e-1, a_3 = -7.900e-2;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2));
    return phi;
}

void FissionAntiNuModule::SetAntiNeutrinoSpectrum(G4double U5, G4double U8, G4double P3, G4double P4) {
    if(U5 <= 0. && U8 <= 0. && P3 <= 0. && P4 <= 0.) { G4cout << "*** ATTENTION: Invalid composition of fission material for antineutrino spectrum. Previous composition kept. ***" << G4endl; }
    else {
     
        if(U5 >= 0.) { U235 = U5; } else { U235 = 0.; G4cout << "*** WARNING: Uranium-235 content (first entry) of composition is negative. Uranium-235 content set to zero. ***" << G4endl; }
        if(U8 >= 0.) { U238 = U8; } else { U238 = 0.; G4cout << "*** WARNING: Uranium-238 content (second entry) of composition is negative. Uranium-238 content set to zero. ***" << G4endl; }
        if(P3 >= 0.) { Pu239 = P3;  } else { Pu239 = 0.; G4cout << "*** WARNING: Plutonium-239 content (third entry) of composition is negative. Plutonium-239 content set to zero. ***" << G4endl; }
        if(P4 >= 0.) { Pu241 = P4;} else { Pu241 = 0.; G4cout << "*** WARNING: Plutonium-241 content (fourth entry) of composition is negative. Plutonium-241 content set to zero. ***" << G4endl; }

    /*    if(myPGA->GetVerbosity() > 1) {
            G4cout << "Simulation set to use antineutrino spectrum from fissions in the following split \n" 
            << "--- U235: " << U235 << " U238: " << U238 << " Pu239: " << Pu239 << " Pu241: " << Pu241 << " ---" << G4endl;
        }*/

    }
}

void FissionAntiNuModule::PrintAllParameters() const {
    G4cout << "\n"
    << "******* FissionAntiNuModule currently specified at the following parameters *******\n"
    << "\t   Uranium-235 Content:               " << 100*(GetUranium235Content(true)) << "%" << "\n"
    << "\t   Uranium-238 Content:               " << 100*(GetUranium238Content(true)) << "%" << "\n"
    << "\t   Plutonium-239 Content:     " << 100*(GetPlutonium239Content(true)) << "%" << "\n"
    << "\t   Plutonium-241-Content:     " << 100*(GetPlutonium241Content(true)) << "%" << "\n"
    << "**************************************************************************************\n"
    << G4endl;
}

void FissionAntiNuModule::SetDwyer(G4bool val){
  dwyer = val;
}
