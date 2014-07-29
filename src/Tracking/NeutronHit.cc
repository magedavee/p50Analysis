// Unrestricted Use - Property of AECL
//
// NeutronHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Neutron Hit Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "NeutronHit.hh"	// Specifies the file which contain the class structure

#include <G4SystemOfUnits.hh>
#include "G4RunManager.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"
#include "G4Point3D.hh"
#include "G4Circle.hh"

#include <iomanip>		// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"		// Specifies class defining all global parameters and variable types

G4Allocator<NeutronHit> NeutronHit::NeutronHitAllocator;

NeutronHit::NeutronHit(): G4VHit() {
    positionI = G4ThreeVector(0.,0.,0.);
    positionF = G4ThreeVector(0.,0.,0.);
    energy = 0.*MeV;
    time = -1.*s;
    bTime = 0.*s;
    trackID = -1;
    tTherm = -1.*s;
    nGamma = 0;
    left = false;
    captured = false;
    foreign = false;
    zenith = 0.*deg;
}

void NeutronHit::operator+=(const NeutronHit &right) {
    positionF = right.positionF;
    time = right.time;
    if(left && right.tTherm >= 0.) { tTherm = (tTherm + right.tTherm) / 2; }
    else if (right.tTherm >= 0.)   { tTherm = right.tTherm; }
    nGamma += right.nGamma;
    left = right.left;
    captured = right.captured;
    foreign = right.foreign;
}

void NeutronHit::Draw() {
    G4VVisManager* vis_manager = G4VVisManager::GetConcreteInstance();
    if(!vis_manager) return;
    
    static  G4VisAttributes* attributesI = NULL;
    if(!attributesI) {
        attributesI = new G4VisAttributes();
        G4Colour* colourI = new G4Colour(G4Colour::Yellow());
        attributesI->SetColour(*colourI);
    }
    static G4VisAttributes* attributesF = NULL;
    if(!attributesF) {
        attributesF = new G4VisAttributes();
        G4Colour* colourF = new G4Colour(G4Colour::Red());
        attributesF->SetColour(*colourF);
    }

    // Entry Position
    if(foreign) {
        G4Point3D* pointI = new G4Point3D(positionI);
        G4Circle* circleI = new G4Circle(*pointI);
        circleI->SetScreenSize(5);
        
        circleI->SetVisAttributes(attributesI);
            
        // Draw circle into visualization
        vis_manager->Draw(*circleI);
    }
    
    // Capture Position
    if(captured) {
        G4Point3D* pointF = new G4Point3D(positionF);
        G4Circle* circleF = new G4Circle(*pointF);
        circleF->SetScreenSize(5);

        circleF->SetVisAttributes(attributesF);
        
        vis_manager->Draw(*circleF);
    }
}

const std::map<G4String,G4AttDef>* NeutronHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("NeutronHit",isNew);
    if(isNew) {
        G4String hitType("Hit Type");
        (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");
        
        G4String erg("Energy");
        (*store)[erg] = G4AttDef(erg,"Energy","Physics","G4BestUnit","G4double");
        
        G4String xyzi("Initial Position");
        (*store)[xyzi] = G4AttDef(xyzi,"Initial Position","Physics","G4BestUnit","G4ThreeVector");
        
        G4String xyzf("Final Position");
        (*store)[xyzf] = G4AttDef(xyzf,"Final Position","Physics","G4BestUnit","G4ThreeVector");
        
        G4String timing("Time");
        (*store)[timing] = G4AttDef(timing,"Time","Physics","G4BestUnit","G4double");
        
        G4String baseT("Entry Time");
        (*store)[baseT] = G4AttDef(baseT,"Entry Time","Physics","G4BestUnit","G4double");
        
        G4String id("Track ID");
        (*store)[id] = G4AttDef(id,"Track ID","Simulation","","G4int");
        
        G4String therm("Thermalization Time");
        (*store)[therm] = G4AttDef(therm,"Thermalization Time","Physics","G4BestUnit","G4double");
        
        G4String em("Gamma Count");
        (*store)[em] = G4AttDef(em,"Gamma Count","Physics","","G4int");
        
        G4String track("Tracker");
        (*store)[track] = G4AttDef(track,"Tracker","Simulation","","G4bool");
        
        G4String capt("Capture Tag");
        (*store)[capt] = G4AttDef(capt,"Capture Tag","Simulation","","G4bool");
        
        G4String entry("Entry Tag");
        (*store)[entry] = G4AttDef(entry,"Entry Tag","Simulation","","G4bool");
        
        G4String angle("Zenith Angle");
        (*store)[angle] = G4AttDef(angle,"Zenith Angle","Physics","G4BestUnit","G4double");
    }
    return store;
}

std::vector<G4AttValue>* NeutronHit::CreateAttValues() const {
    
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values->push_back(G4AttValue("Hit Type","Neutron Hit",""));
    values->push_back(G4AttValue("Energy",G4BestUnit(energy,"Energy"),""));
    values->push_back(G4AttValue("Initial Position",G4BestUnit(positionI,"Length"),""));
    values->push_back(G4AttValue("Final Position",G4BestUnit(positionF,"Length"),""));
    values->push_back(G4AttValue("Entry Time",G4BestUnit(bTime,"Time"),""));
    values->push_back(G4AttValue("Time",G4BestUnit(time,"Time"),""));
    values->push_back(G4AttValue("Track ID",trackID,""));
    values->push_back(G4AttValue("Thermalization Time",G4BestUnit(tTherm,"Time"),""));
    values->push_back(G4AttValue("Gamma Count",nGamma,""));
    values->push_back(G4AttValue("Tracker",left,""));
    values->push_back(G4AttValue("Capture Tag",captured,""));
    values->push_back(G4AttValue("Entry Tag",foreign,""));
    values->push_back(G4AttValue("Zenith Angle",G4BestUnit(zenith,"Angle"),""));
    
    return values;
}
