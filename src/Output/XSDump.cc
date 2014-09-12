#include "XSDump.hh"
#include "Utilities.hh"
#include "MaterialsHelper.hh"

#include <cmath>
#include <TGraph.h>

#include <G4SystemOfUnits.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4HadronicProcessStore.hh>
#include <G4ParticleTable.hh>
#include <G4UIcommand.hh>
#include <G4Tokenizer.hh>

XSDump::XSDump():
Emin(0.001*eV), Emax(100*MeV), npts(1000),
xsdir("/output/xsdump/"),
ptclCmd("/output/xsdump/particle",this),
xsCmd("/output/xsdump/isotZA",this) {
    ptclCmd.SetGuidance("Set particle for cross-sections dump");
    ptclCmd.AvailableForStates(G4State_Idle);
    
    xsCmd.SetGuidance("Dump cross-sections for isotope Z,A");
    xsCmd.AvailableForStates(G4State_PreInit);
    G4UIparameter* Zparam = new G4UIparameter("Z", 'i', false);
    xsCmd.SetParameter(Zparam);
    G4UIparameter* Aparam = new G4UIparameter("A", 'i', false);
    xsCmd.SetParameter(Aparam);
    
    neutronDef = particle = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
}


void XSDump::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &ptclCmd) {
        particle = G4ParticleTable::GetParticleTable()->FindParticle(newValue);
    } else if(command == &xsCmd) {
        G4Tokenizer tk(newValue);
        int Z = StoI(tk());
        int A = StoI(tk());
        addIsot(Z,A);
    }
}

// export G4PhysListDocDir=${HOME}/Documents/PROSPECT/G4Phys/
// export G4PhysListName=PG4

void XSDump::addIsot(int Z, int A) {
    Zs.push_back(Z);
    As.push_back(A);
    
    std::string isotnm = "Z"+to_str(Z)+"A"+to_str(A);
    G4Isotope* myIsot = new G4Isotope("isot_"+isotnm, Z, A, A*g/mole);
    G4Element* myEl  = new G4Element("ele_"+isotnm, isotnm, 1);
    myEl->AddIsotope(myIsot,100.*perCent);
    isots.push_back(myEl);
    
    G4Material* myMat = new G4Material("mat_"+isotnm, 1.0*g/cm3, 1, kStateSolid, MaterialsHelper::M().room_T);
    myMat->AddElement(myEl, 100.*perCent);
    isotmats.push_back(myMat);
}

void XSDump::makeXSdata() {
    
    if(!particle) {
        G4cout << "XSDump: undefined particle!\n";
        return;
    }
    G4HadronicProcessStore* store = G4HadronicProcessStore::Instance();
    if(!store) {
        G4cout << "XSDump: no hadronic process store!\n";
        return;
    }
    
    for(unsigned int ii=0; ii<Zs.size(); ii++) {
        int Z = Zs[ii];
        int A = As[ii];
        std::string isotnm = "Z"+to_str(Z)+"A"+to_str(A);
    
        G4cout << "Dumping cross sections for " << particle->GetParticleName() << " interactions with Z,A = " << Z << "," << A << "\n";
        
        G4Element* myEl = isots[ii];
        G4Material* myMat = isotmats[ii];
    
        TGraph gEl(npts);
        TGraph gInEl(npts);
        TGraph gCEx(npts);
        TGraph gCap(npts);
        TGraph gFis(npts);
        for(unsigned int i=0; i<npts; i++) {
            double l = double(i)/(npts-1);
            double E = exp(log(Emin)*(1-l)+log(Emax)*l);
            /*
            gEl.SetPoint(  i, E, store->GetElasticCrossSectionPerIsotope(particle,E,Z,A)/barn);
            gInEl.SetPoint(i, E, store->GetInelasticCrossSectionPerIsotope(particle,E,Z,A)/barn);
            gCEx.SetPoint( i, E, store->GetChargeExchangeCrossSectionPerIsotope(particle,E,Z,A)/barn);
            if(particle == neutronDef) {
                gCap.SetPoint(i, E, store->GetCaptureCrossSectionPerIsotope(particle,E,Z,A)/barn);
                gFis.SetPoint(i, E, store->GetFissionCrossSectionPerIsotope(particle,E,Z,A)/barn);
            }
            */
            gEl.SetPoint(  i, E, store->GetElasticCrossSectionPerAtom(particle,E,myEl,myMat)/barn);
            gInEl.SetPoint(i, E, store->GetInelasticCrossSectionPerAtom(particle,E,myEl,myMat)/barn);
            gCEx.SetPoint( i, E, store->GetChargeExchangeCrossSectionPerAtom(particle,E,myEl,myMat)/barn);
            if(particle == neutronDef) {
                gCap.SetPoint(i, E, store->GetCaptureCrossSectionPerAtom(particle,E,myEl,myMat)/barn);
                gFis.SetPoint(i, E, store->GetFissionCrossSectionPerAtom(particle,E,myEl,myMat)/barn);
            }
        }

        std::string gnm = "gXS_"+particle->GetParticleName()+"_"+isotnm;
        
        gEl.SetName((gnm+"_Elastic").c_str());
        gEl.Write(gEl.GetName());
        gInEl.SetName((gnm+"_Inelastic").c_str());
        gInEl.Write(gInEl.GetName());
        gCEx.SetName((gnm+"_ChgEx").c_str());
        gCEx.Write(gCEx.GetName());
        if(particle == neutronDef) {
            gCap.SetName((gnm+"_Capture").c_str());
            gCap.Write(gCap.GetName());
            gFis.Write((gnm+"_Fission").c_str());
            gFis.Write(gFis.GetName());
        }
    }
}
