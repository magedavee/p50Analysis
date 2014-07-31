#include "PMTBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>

#include <cmath>

PMTBuilder::PMTBuilder(): diameter(8*2.54*cm), length(0),
outer_vis(G4Color(0.2,0.2,0.4)), capsule_vis(G4Color(1.0,1.0,0.0)) { }

void PMTBuilder::construct() {
    length = 1.0*diameter;
    double capsule_length = 0.66*length;        // length of vacuum capsule
    double capsule_radius = diameter/2. - 2.*mm;// radius of vacuum capsule
    double capsule_thick = 5.*mm;               // thickness of vacuum capsule
    
    G4Tubs* pmt_tube = new G4Tubs("pmt_tube", 0, diameter/2., length/2., 0, 2*M_PI);
    main_log = new G4LogicalVolume(pmt_tube, MaterialsHelper::M().SS444, "PMT_main_log");
    main_log->SetVisAttributes(&outer_vis);
    
    G4Tubs* pmt_capsule_tube = new G4Tubs("pmt_capsule_tube", 0, capsule_radius, capsule_length/2., 0, 2*M_PI);
    G4LogicalVolume* capsule_log = new G4LogicalVolume(pmt_capsule_tube, MaterialsHelper::M().Quartz, "PMT_capsule_log");
    capsule_log->SetVisAttributes(&capsule_vis);
    new G4PVPlacement(NULL, G4ThreeVector(0,0,(length-capsule_length)/2.), capsule_log, "PMT_capsule_phys", main_log, false, 0, true);
    
    G4Tubs* pmt_vacuum_tube = new G4Tubs("pmt_vacuum_tube", 0, capsule_radius - capsule_thick, capsule_length/2.-capsule_thick, 0, 2*M_PI);
    G4LogicalVolume* vacuum_log = new G4LogicalVolume(pmt_vacuum_tube, MaterialsHelper::M().Vacuum, "PMT_vacuum_log");
    new G4PVPlacement(NULL, G4ThreeVector(0,0,0), vacuum_log, "PMT_vacuum_phys", capsule_log, false, 0, true);
}
