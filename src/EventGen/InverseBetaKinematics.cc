#include "InverseBetaKinematics.hh"
#include "InverseBetaMessenger.hh"

#include <G4SystemOfUnits.hh>
#include <Randomize.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include <G4RotationMatrix.hh>
#include <G4ios.hh>

#include <vector>
#include <math.h>
#include <fstream>
#include <map>
using std::map;

InverseBetaKinematics::InverseBetaKinematics(G4int v):
dwyer(false), U235(1.06), U238(0.057), Pu239(0.0), Pu241(0.0),
pMass(G4Proton::ProtonDefinition()->GetPDGMass()),
nMass(G4Neutron::NeutronDefinition()->GetPDGMass()),
ePlusMass(G4Positron::PositronDefinition()->GetPDGMass()),
verbose(v), antiNuMonoEnergy(0*MeV),
inv_messenger(this) {
    RawData = verbose > 2;
    // load Dwyer spectrum data
    char header[200];
    G4double dummy;
    G4int cnt=0;
    char pnd = '#';
    std::ifstream infile("antineutrinoSpectra_DwyerLangford_1keV_v0.txt",std::ifstream::in);
    if(!infile.is_open()) G4cerr << "Problem opening Dwyer spectrum file" << G4endl;
    while (infile.peek()==int(pnd)){
        infile.getline(header,200);
    }
    while(infile >> dummy >> Dwyer235[cnt] >> Dwyer238[cnt] >> Dwyer239[cnt] >> Dwyer241[cnt]){
        cnt++;
    }
    infile.close();
    
    PrintAllParameters();
}

void InverseBetaKinematics::SetVerbosity(G4int v) {
    verbose = v;
    if(verbose > 1) { G4cout << "InverseBetaKinematics verbosity set to " << v << "." << G4endl; }
    if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: InverseBetaKinematics raw data will now be output. ***" << G4endl; }
    else            { RawData = false; }
}

void InverseBetaKinematics::SetAntiNeutrinoSpectrum(G4double U5, G4double U8, G4double P3, G4double P4) {
    // Re-enables monoenergetic antineutrinos if all compositions are zero
    if(U5 == 0. && U8 == 0. && P3 == 0. && P4 == 0.) {
        G4cout << "*** WARNING: Spectrum total composition adds to zero. Mono-energetic antineutrinos with energy " << G4BestUnit(antiNuMonoEnergy,"Energy") << " used. ***" << G4endl;
    } else {
        antiNuMonoEnergy = 0;
        if(U5 >= 0.) { U235 = U5; } else { U235 = 0.; G4cout << "*** WARNING: Uranium-235 content (first entry) of composition is negative. Uranium-235 content set to zero. ***" << G4endl; }
        if(U8 >= 0.) { U238 = U8; } else { U238 = 0.; G4cout << "*** WARNING: Uranium-238 content (second entry) of composition is negative. Uranium-238 content set to zero. ***" << G4endl; }
        if(P3 >= 0.) { Pu239 = P3;  } else { Pu239 = 0.; G4cout << "*** WARNING: Plutonium-239 content (third entry) of composition is negative. Plutonium-239 content set to zero. ***" << G4endl; }
        if(P4 >= 0.) { Pu241 = P4;} else { Pu241 = 0.; G4cout << "*** WARNING: Plutonium-241 content (fourth entry) of composition is negative. Plutonium-241 content set to zero. ***" << G4endl; }
        
        G4double pcU5 = 100*GetUranium235Content(true);  
        G4double pcU8 = 100*GetUranium238Content(true);  
        G4double pcP3 = 100*GetPlutonium239Content(true);
        G4double pcP4 = 100*GetPlutonium241Content(true);
        
        if(verbose > 0)
            G4cout << "Spectrum currently set to --- Uranium-235: " << pcU5 << "%, Uranium-238: " << pcU8 << "%, Plutonium-239: " << pcP3 << "%, Plutonium-241: " << pcP4 << "%" << G4endl;
    }
}

void InverseBetaKinematics::SetAntiNeutrinoMonoEnergy(G4double E) {
    if(E > 1.8*MeV && E < 9.5*MeV) {
        antiNuMonoEnergy = E;
        if(verbose > 0) { G4cout << "Spectrum set to use mono-energetic antineutrinos at " << G4BestUnit(E,"Energy") << "." << G4endl; }
    } else {
        antiNuMonoEnergy = 2.*MeV;
        G4cout << "*** WARNING: Input mono-energetic antineutrinos are outside acceptable range. Switching to default energy of 2.0 MeV. ***" << G4endl;
    }
}

vector<G4double> InverseBetaKinematics::GenerateReactionKinematics() const {    
    // Generates an antineutrino energy according to chosen fuel composition
    G4double eNu = GenerateAntiNeutrinoEnergy();
    
    // Computes the various energies and angles using the four-vector solution
    G4double thetaPos = GeneratePositronAngle(eNu);
    if(thetaPos < 0.*deg || thetaPos > 180.*deg) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, positron angle out of range. ***" << G4endl; return GenerateReactionKinematics(); }
    G4double ePos = CalculatePositronEnergy(thetaPos, eNu);
    if(ePos < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative positron momentum/energy. ***" << G4endl; return GenerateReactionKinematics(); }
    G4double eN = CalculateNeutronEnergy(ePos, eNu);
    if(eN < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative neutron energy/momentum. ***" << G4endl; return GenerateReactionKinematics(); }
    G4double thetaN = CalculateNeutronAngle(eN, eNu);
    if(thetaN < 0. || thetaN > 180.*deg) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron angle out of range. ***" << G4endl; return GenerateReactionKinematics(); }
    G4double phiPos = 360.*G4UniformRand()*deg;
    G4double phiN = 0.*deg;
    phiN = (phiPos < 180.*deg)? phiPos + 180.*deg : phiPos - 180.*deg;
    
    // try again if not kinematically allowed
    if(!CheckFourVectorSolution(eNu,ePos,eN,thetaPos,thetaN)) return GenerateReactionKinematics();
    
    // Assembles kinematics results into ThreeVectors to obtain <x,y,z> from <r,theta,phi>
    G4ThreeVector pHatPos(0.,0.,0.); pHatPos.setRThetaPhi(1.,thetaPos,phiPos);
    G4ThreeVector pHatN(0.,0.,0.); pHatN.setRThetaPhi(1.,thetaN,phiN);
    
    // TODO rotate to incident antinu frame
    
    // Convert total energy to kinetic energy for neutron and positron
    G4double kePos = ePos - ePlusMass;
    G4double keN = eN - nMass;
    
    // Placed into vector as { Positron: E, px, py, pz ; Neutron: E, px, py, pz ; Extra: positron angle wrt nu, neutron angle wrt nu } - 10 entries
    vector<G4double> kinematics;
    kinematics.push_back(kePos); kinematics.push_back(pHatPos.x()); kinematics.push_back(pHatPos.y()); kinematics.push_back(pHatPos.z());
    kinematics.push_back(keN); kinematics.push_back(pHatN.x()); kinematics.push_back(pHatN.y()); kinematics.push_back(pHatN.z());
    kinematics.push_back(thetaPos); kinematics.push_back(-thetaN);
    return kinematics;
}

void InverseBetaKinematics::GenerateKinematicsWithoutSimulation(G4int n) const {
    // Generates histograms for particle kinetic energies and relative directions
    map<G4double,G4int>* thePosEHistogram = new map<G4double,G4int>();
    map<G4double,G4int>* theNEHistogram = new map<G4double,G4int>();
    map<G4double,G4int>* thePosAngHistogram = new map<G4double,G4int>();
    map<G4double,G4int>* theNAngHistogram = new map<G4double,G4int>();
    G4double minPos_e = 0.0*MeV;
    G4double maxPos_e = 8.2*MeV;
    G4double pos_bins = 164;
    G4double minN_e = 0.0*MeV;
    G4double maxN_e = 125.*keV;
    G4double n_bins = 1250;
    if(antiNuMonoEnergy)	// Redefines histogram values if monoenergetic particles are selected
    {
        minPos_e = CalculatePositronEnergy(180.*deg, antiNuMonoEnergy) - ePlusMass;
        maxPos_e = CalculatePositronEnergy(0.*deg, antiNuMonoEnergy) - ePlusMass;
        pos_bins = 150;
        minN_e = CalculateNeutronEnergy((maxPos_e + ePlusMass), antiNuMonoEnergy) - nMass - 10.*eV;
        maxN_e = CalculateNeutronEnergy((minPos_e + ePlusMass), antiNuMonoEnergy) - nMass + 10.*eV;
        n_bins = 150;
    }
    G4double posIncrementer = (maxPos_e - minPos_e) / pos_bins;
    G4double nIncrementer = (maxN_e - minN_e) / n_bins;
    for(int i = 1; i <= pos_bins; i++)
    {
        G4double upperlimit = minPos_e + i*posIncrementer;
        (*thePosEHistogram)[upperlimit] = 0;
    }
    for(int i = 1; i <= n_bins; i++)
    {
        G4double upperlimit = minN_e + i*nIncrementer;
        (*theNEHistogram)[upperlimit] = 0;
    }
    G4double ang_bins = 180;
    for(int j = 1; j <= ang_bins; j++)
    {
        G4double upperlimit = j*1.*deg;
        (*thePosAngHistogram)[upperlimit] = 0;
        upperlimit = -upperlimit + 1.*deg;
        (*theNAngHistogram)[upperlimit] = 0;
    }
    
    // Uses random number generator to sample from spectrum n times
    G4int reps = 1;
    while(reps <= n) {
        vector<G4double> kinematics = GenerateReactionKinematics();
        
        // Immediately sorts sampled kinematics into histogram for ease of storage and memory efficiency
        map<G4double,G4int>::iterator itr = thePosEHistogram->begin();
        while(kinematics[0] >= itr->first && itr != thePosEHistogram->end()) { itr++; }
        (*thePosEHistogram)[itr->first]++;
        itr = theNEHistogram->begin();
        while(kinematics[4] >= itr->first && itr != theNEHistogram->end()) { itr++; }
        (*theNEHistogram)[itr->first]++;
        itr = thePosAngHistogram->begin();
        while(kinematics[8] > itr->first && itr != thePosAngHistogram->end()) { itr++; }
        (*thePosAngHistogram)[itr->first]++;
        itr = theNAngHistogram->begin();
        while(kinematics[9] > itr->first && itr != theNAngHistogram->end()) { itr++; }
        (*theNAngHistogram)[itr->first]++;
        
        // Print status to show progress
        if(reps % 100000 == 0) { G4cout << "Completed " << reps << " samples." << G4endl; }
        reps++;
    }
}

G4double InverseBetaKinematics::GenerateAntiNeutrinoEnergy() const {
    if(antiNuMonoEnergy) return antiNuMonoEnergy;
    
    G4double energy = 0;
    if(dwyer) {
        energy = GenerateDwyerAntiNeutrinoEnergy();
    } else {
        G4double Probability;
        do {
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
        } while (G4UniformRand() > Probability); // Acceptance condition
    }
    if(energy <= 1.81 || energy > 9.5) return GenerateAntiNeutrinoEnergy();  // try again if out of range
    return energy*MeV;
}

G4double InverseBetaKinematics::GenerateDwyerAntiNeutrinoEnergy() const {
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

G4double InverseBetaKinematics::GeneratePositronAngle(G4double eNu) const {
    // Uses coded distribution to generate a positron angle according to theory
    G4double ePlusAngle = -1.;
    G4double max_dist = 1.0;
    G4double Forward = CalculateCrossSectionWRTAngle(0.*deg, eNu);
    G4double Backward = CalculateCrossSectionWRTAngle(180.*deg, eNu);
    
    // Although tendency is backwards for all fission antineutrino ranges, this code is capable of evaluating E(nu) > 15 MeV where the tendency flips
    if(Forward > Backward) max_dist = Forward;
    else max_dist = Backward;
    G4double xSeed, ySeed, Probability;
    do {
        // Positron angle
        xSeed = 180.*G4UniformRand()*deg;
        
        // Positron angle with 4*PI correction
        //    xSeed = 2*G4UniformRand() - 1.0;
        //    xSeed = acos(xSeed);
        
        // Acceptance parameter
        ySeed = G4UniformRand();
        
        // Acceptance value
        Probability = CalculateCrossSectionWRTAngle(xSeed, eNu) / max_dist;
    }
    while (ySeed > Probability); // Acceptance condition
    ePlusAngle = xSeed;
    
    return ePlusAngle;
}

G4double InverseBetaKinematics::CalculatePositronEnergy(G4double ePlusAngle, G4double nuEnergy) const {
    // Calculates positron energy using the generated positron angle and the four-vector equation
    G4double t1, t2, t3, t4;
    
    // Four-vector equation solution - solves for positron momentum
    t1 = 4*ePlusMass*ePlusMass*pow((nuEnergy + pMass),2);
    t2 = pow(pMass,4) + pow(ePlusMass,4) + pow(nMass,4) + 4*nuEnergy*nuEnergy*pMass*pMass;
    t3 = 2*(pMass*pMass*ePlusMass*ePlusMass - pMass*pMass*nMass*nMass - ePlusMass*ePlusMass*nMass*nMass);
    t4 = 4*nuEnergy*pMass*(pMass*pMass + ePlusMass*ePlusMass - nMass*nMass);
    
    G4double a, b, cc, Determinant;
    
    a = 4*pow((nuEnergy + pMass),2) - 4*(nuEnergy*nuEnergy)*pow(cos(ePlusAngle),2);
    b = -4*nuEnergy*cos(ePlusAngle)*(pMass*pMass + ePlusMass*ePlusMass - nMass*nMass + 2*nuEnergy*pMass);
    cc = t1 - t2 - t3 - t4;
    Determinant = b*b - 4*a*cc;
    
    // Determine validity of calculated momentum
    G4double ePlusEnergy;
    if(Determinant < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, negative determinant in quadratic formula. ***" << G4endl; ePlusEnergy = -1.; }
    else // It may be still possible to use the negative determinant by taking the real part, but this is not proven to be the case yet
    {
        G4double ePlusMomentum = (-b + sqrt(Determinant)) / (2*a);
        if(ePlusMomentum < 0.) { ePlusMomentum = (-b - sqrt(Determinant)) / (2*a); }
        if(ePlusMomentum < 0.) { ePlusMomentum = 0.; if(verbose > 1) { G4cout << "*** WARNING: Positron momentum is negative. Momentum set to zero. ***" << G4endl; } }
        
        // Conversion of momentum to total energy
        ePlusEnergy = sqrt(ePlusMomentum*ePlusMomentum + ePlusMass*ePlusMass);
    }
    
    return ePlusEnergy;
}

G4double InverseBetaKinematics::CalculateNeutronEnergy(G4double ePlusEnergy, G4double nuEnergy) const {
    // Calculates neutron total energy using positron total energy and conservation of mass/energy
    G4double nEnergy = nuEnergy + pMass - ePlusEnergy;
    
    // Determine validity of calculated total energy
    G4double rootCheck = nEnergy*nEnergy - nMass*nMass;
    if(rootCheck < 0.) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron total energy less than rest mass energy. ***" << G4endl; nEnergy = -1.; }
    
    return nEnergy;
}

G4double InverseBetaKinematics::CalculateNeutronAngle(G4double nEnergy, G4double nuEnergy) const {
    // Calculates neutron angle using its total energy and the four-vector equation
    G4double nAngle;
    
    // Conversion from total energy to momentum
    G4double nMomentum = sqrt(nEnergy*nEnergy - nMass*nMass);
    G4double t1, t2, t3, cosNAngle;
    
    // Four-vector equation solution
    t1 = 2*nEnergy*(nuEnergy + pMass);
    t2 = ePlusMass*ePlusMass - pMass*pMass - nMass*nMass - 2*nuEnergy*pMass;
    t3 = 2*nuEnergy*nMomentum;
    cosNAngle = (t1 + t2) / t3;
    if(cosNAngle > 1.0 && (cosNAngle - 1.0) < 1.e-8)   { cosNAngle = 1.; }	// This accounts for some slight mathematical deviations due to computation
    
    // Negative angles shown to mathematically equate to corresponding positive angle
    nAngle = std::fabs(acos(cosNAngle));
    
    // Determine validity of calculated neutron angle
    if(std::fabs(cosNAngle) > 1.0) { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics, neutron angle cosine is out of acceptable bounds. ***" << G4endl; nAngle = 10.; }
    
    return nAngle;
}

G4bool InverseBetaKinematics::CheckFourVectorSolution(G4double nuEnergy, G4double posE, G4double nE, G4double posAng, G4double nAng) const {
    // Check conservation of mass-energy
    G4double eBalance = nuEnergy + pMass - posE - nE;
    if(eBalance/MeV > 1.e-6)
    { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of mass-energy test. ***" << G4endl; return false; }
    
    // Check conservation of momentum in antineutrino axial direction
    G4double axialBalance = nuEnergy - cos(posAng)*sqrt(posE*posE - ePlusMass*ePlusMass) - cos(nAng)*sqrt(nE*nE - nMass*nMass);
    if(axialBalance/MeV > 1.e-6)
    { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of axial momentum test. ***" << G4endl; return false; }
    
    // Check conservation of momentum in antineutrino radial direction
    G4double radialBalance = sin(posAng)*sqrt(posE*posE - ePlusMass*ePlusMass) - sin(nAng)*sqrt(nE*nE - nMass*nMass);
    if(radialBalance/MeV > 1.e-3)
    { G4cout << "*** ERROR: Unexpected error in InverseBetaKinematics. Kinematics solution fails conservation of radial momentum test. ***" << G4endl; return false; }
    
    return true;
}

G4double InverseBetaKinematics::CalculateCrossSectionWRTAngle(G4double angle, G4double nuEnergy) const {
    // Parameterization found in Angular Distribtuion of Neutron Inverse beta Decay
    // P. Vogel and J. F. Beacon, July 1999 - Equation (12)
    
    // Calculates probability distribution of event based on angle
    G4double delta = nMass - pMass;
    G4double M = (nMass + pMass)/2;
    G4double ySq = (delta*delta - ePlusMass*ePlusMass)/2;
    G4double F = 1., G = 1.26, F2 = 3.706;
    G4double fermiConst = 2.301562e-22*cm/MeV;
    G4double cosC = 0.974, innerRad = 0.024;
    
    G4double sigmaZero, dSigmaByDCosTheta;
    G4double eZero, pZero, vZero;
    G4double eOne, pOne, vOne;
    
    sigmaZero = (pow(fermiConst,2)*cosC*cosC / M_PI)*(1 + innerRad);
    eZero = nuEnergy - delta;
    pZero = sqrt(eZero*eZero - ePlusMass*ePlusMass);
    vZero = pZero / eZero;
    eOne = eZero*(1 - (nuEnergy / M)*(1 - vZero*cos(angle))) - (ySq / M);
    pOne = sqrt(eOne*eOne - ePlusMass*ePlusMass);
    vOne = pOne / eOne;
    
    G4double T1, T2, T3, T4, Gamma;
    
    T1 = 2*(F + F2)*G*((2*eZero + delta)*(1 - vZero*cos(angle)) - (ePlusMass*ePlusMass / eZero));
    T2 = (F*F + G*G)*(delta*(1 + vZero*cos(angle)) + (ePlusMass*ePlusMass / eZero));
    T3 = (F*F + 3*G*G)*((eZero + delta)*(1 - (cos(angle) / vZero)) - delta);
    T4 = (F*F - G*G)*((eZero + delta)*(1 - (cos(angle) / vZero)) - delta)*vZero*cos(angle);
    Gamma = T1 + T2 + T3 + T4;
    
    dSigmaByDCosTheta = (sigmaZero/2)*((F*F + 3*G*G) + (F*F - G*G)*vOne*cos(angle))*eOne*pOne - (sigmaZero/2)*(Gamma / M)*eZero*pZero;
    return dSigmaByDCosTheta;
}

void InverseBetaKinematics::PrintAllParameters() const {
    G4cout << "\n******* InverseBetaKinematics currently specified at the following parameters *******\n";
    if(antiNuMonoEnergy) {
        G4cout << "\t   Fission Spectrum:\t\tInactive" << "\n"
        << "\t   Antineutrino Mono Energy:\t" << G4BestUnit(antiNuMonoEnergy,"Energy") << "\n" ;
    } else {
        G4cout << "\t   Fission Spectrum:\t\tActive" << "\n"
        << "\t   Uranium-235 Content:\t\t" << 100*GetUranium235Content(true) << "%" << "\n"
        << "\t   Uranium-238 Content:\t\t" << 100*GetUranium238Content(true) << "%" << "\n"
        << "\t   Plutonium-239 Content:\t" << 100*GetPlutonium239Content(true) << "%" << "\n"
        << "\t   Plutonium-241-Content:\t" << 100*GetPlutonium241Content(true) << "%" << "\n";
    }
    G4cout << "*************************************************************************************\n" << G4endl;
}

G4double InverseBetaKinematics::CalculateU235Spectrum(G4double eNu) const {
    //  G4double a_1 = 3.519e+0, a_2 = -3.517e+0, a_3 = 1.595e+0, a_4 = -4.171e-1, a_5 = 5.004e-2, a_6 = -2.303e-3;
    G4double a_1 = 1.418, a_2 = -0.6078, a_3 = 8.955e-3, a_4 = -6.69e-3, a_5 = 6.933e-5, a_6 = 0;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double InverseBetaKinematics::CalculatePu239Spectrum(G4double eNu) const {
    G4double a_1 = 2.560e+0, a_2 = -2.654e+0, a_3 = 1.256e+0, a_4 = -3.617e-1, a_5 = 4.547e-2, a_6 = -2.143e-3;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double InverseBetaKinematics::CalculatePu241Spectrum(G4double eNu) const {
    G4double a_1 = 1.487e+0, a_2 = -1.038e+0, a_3 = 4.130e-1, a_4 = -1.423e-1, a_5 = 1.866e-2, a_6 = -9.229e-4;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2) + a_4*pow(eNu,3) + a_5*pow(eNu,4) + a_6*pow(eNu,5));
    return phi;
}

G4double InverseBetaKinematics::CalculateU238Spectrum(G4double eNu) const {
    G4double a_1 = 9.760e-1, a_2 = -1.620e-1, a_3 = -7.900e-2;
    G4double phi = exp(a_1 + a_2*eNu + a_3*pow(eNu,2));
    return phi;
}
