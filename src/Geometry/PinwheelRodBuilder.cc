#include "PinwheelRodBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4Tubs.hh>
#include <G4ExtrudedSolid.hh>
#include <G4RotationMatrix.hh>

#include <cmath>
#include <cassert>

void rotvert(std::vector<G4TwoVector>& vertices, unsigned int nrot) {
    size_t nvert = vertices.size();
    for(int i=1; i<nrot; i++) {
        double th = -i*2*M_PI/nrot;
        double cs = cos(th);
        double sn = sin(th);
        for(size_t j=0; j<nvert; j++) {
            const G4TwoVector& v = vertices[j];
            vertices.push_back(G4TwoVector(cs*v[0]-sn*v[1], sn*v[0]+cs*v[1]));
        }
    }
}


PinwheelRodBuilder::PinwheelRodBuilder(): RodBuilder("PinwheelRod"),
w_inner(6*mm), t_end(1*mm), t_panel(0), t_hook(1*mm), l_hook(2*mm),
ui_dir("/geom/pwrod/"),
w_in_cmd("/geom/pwrod/width",this),
hole_cmd("/geom/pwrod/r_hole",this) {
    
    ui_dir.SetGuidance("Pinwheel rod settings");
    ui_dir.AvailableForStates(G4State_PreInit);
    
    w_in_cmd.SetGuidance("Set the width of pinwheel rod square core");
    w_in_cmd.SetDefaultValue(w_inner);
    w_in_cmd.AvailableForStates(G4State_PreInit);
    
    hole_cmd.SetGuidance("Set the radius of pinwheel rod inner hole");
    hole_cmd.SetDefaultValue(r_hole);
    hole_cmd.AvailableForStates(G4State_PreInit);
}

void PinwheelRodBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &w_in_cmd)  w_inner = w_in_cmd.GetNewDoubleValue(newValue);
    else if(command == &hole_cmd)  r_hole = hole_cmd.GetNewDoubleValue(newValue);
}

void PinwheelRodBuilder::construct() {
    myOptSurf.construct();
    
    double w_total = w_inner + 2*(t_panel+t_hook);
    dim = G4ThreeVector(w_total, w_total, length);
        
    // extrusion cross-section
    std::vector<G4TwoVector> vertices;
    vertices.push_back(G4TwoVector(-0.5*w_inner, 0.5*w_total));
    vertices.push_back(G4TwoVector(-0.5*w_inner + t_end + l_hook, 0.5*w_total));
    vertices.push_back(G4TwoVector(-0.5*w_inner + t_end + l_hook, 0.5*w_total-t_hook));
    vertices.push_back(G4TwoVector(-0.5*w_inner + t_end, 0.5*w_total-t_hook));
    vertices.push_back(G4TwoVector(-0.5*w_inner + t_end, 0.5*w_inner));
    rotvert(vertices,4);
    G4ExtrudedSolid* extruded = new G4ExtrudedSolid("pwrod_extruded", vertices, length/2, G4TwoVector(), 1, G4TwoVector(), 1);
    
    // bore out central hole
    G4VSolid* bored = extruded;
    if(r_hole) {
        G4Tubs* center_tube = new G4Tubs("pwrod_center_tube", 0, r_hole, length, 0, 2*M_PI);
        bored = new G4SubtractionSolid("pwrod_bored", extruded, center_tube);
    }
    
    main_log = new G4LogicalVolume(bored, MaterialsHelper::M().PEEK, "PinwheelRod_main_Log");
    main_log->SetVisAttributes(&rod_vis);
}

void PinwheelRodBuilder::fillNode(TXMLEngine& E) {
    RodBuilder::fillNode(E);
    addAttr(E, "w_inner", G4BestUnit(w_inner,"Length"));
    addAttr(E, "t_end", G4BestUnit(t_end,"Length"));
    addAttr(E, "t_panel", G4BestUnit(t_panel,"Length"));
    addAttr(E, "t_hook", G4BestUnit(t_hook,"Length"));
    addAttr(E, "l_hook", G4BestUnit(l_hook,"Length"));
}
