#include "ScintTankBuilder.hh"
#include "Utilities.hh"

#include <cassert>
#include <vector>

#include "MaterialsHelper.hh"
#include "PinwheelRodBuilder.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4TwoVector.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>
#include <G4LogicalBorderSurface.hh>

ScintTankBuilder::ScintTankBuilder(const std::string& snm): ScintSegVol(snm),
tank_depth(120*cm), gc_thick(0*cm), tank_wall_thick(2*cm), ls_buffer_thick(5*cm),
seg_size(14.4*cm), nSegX(14), nSegY(10), scint6LiLoading(0.001), theta_pw(0),
tank_ui_dir("/geom/tank/"),
nsegXcmd("/geom/tank/nSegX",this),
nsegYcmd("/geom/tank/nSegY",this),
segWcmd("/geom/tank/segSize",this),
gcTcmd("/geom/tank/gcThick",this),
scint6Licmd("/geom/tank/scint6Li",this),
tank_vis(G4Colour(0.6,0.6,0.6)),
gc_vis(G4Colour(1.0,0.5,1.0)),
scint_vis(G4Colour(0.5,0.5,1.0)) {
    
    tank_ui_dir.SetGuidance("Scintillator tank settings");
    tank_ui_dir.AvailableForStates(G4State_PreInit);
    
    nsegXcmd.SetGuidance("Set the number of detector segments along length");
    nsegXcmd.SetDefaultValue(nSegX);
    nsegXcmd.AvailableForStates(G4State_PreInit);
    
    nsegYcmd.SetGuidance("Set the number of detector segments in height");
    nsegYcmd.SetDefaultValue(nSegY);
    nsegYcmd.AvailableForStates(G4State_PreInit);
    
    segWcmd.SetGuidance("Set the size of each segment");
    segWcmd.SetDefaultValue(seg_size);
    segWcmd.AvailableForStates(G4State_PreInit);
    
    gcTcmd.SetGuidance("Set thickness of gamma catchers");
    gcTcmd.SetDefaultValue(gc_thick);
    gcTcmd.AvailableForStates(G4State_PreInit);
    
    scint6Licmd.SetGuidance("Scintillator 6Li loading fraction by mass");
    scint6Licmd.SetDefaultValue(scint6LiLoading);
    scint6Licmd.AvailableForStates(G4State_PreInit);
    
    addChild(&mySeparator);
}

void ScintTankBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &nsegXcmd)      nSegX = nsegXcmd.GetNewIntValue(newValue);
    else if(command == &nsegYcmd) nSegY = nsegYcmd.GetNewIntValue(newValue);
    else if(command == &segWcmd)  seg_size = segWcmd.GetNewDoubleValue(newValue);
    else if(command == &gcTcmd)  gc_thick = gcTcmd.GetNewDoubleValue(newValue);
    else if(command == &scint6Licmd)  scint6LiLoading = scint6Licmd.GetNewDoubleValue(newValue);
    else G4cout << "Unknown command!" << G4endl;
}

void ScintTankBuilder::construct() {
    
    lat_size = seg_size; // can change in setupDividers
    setupDividers();
    
    rotRod.rotateZ(theta_pw);
    cos_pw = cos(theta_pw);
    sin_pw = sin(theta_pw);
    
    dim = G4ThreeVector(nSegX*lat_size+2*(tank_wall_thick+ls_buffer_thick), 
                        nSegY*lat_size+2*(tank_wall_thick+ls_buffer_thick),
                        tank_depth);
    
    ///////////////
    // acrylic tank
    G4Box* tank_box = new G4Box("tank_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(tank_box, MaterialsHelper::M().PMMA_black, "ScintTank_main_log");
    main_log->SetVisAttributes(&tank_vis);
    
    
    if(gc_thick) {
        setupGammaCatcher();
        gammacatcher_log->SetVisAttributes(&gc_vis);
    } else gammacatcher_log = NULL;
        
    /////////////////////////////
    // liquid scintillator volume
    G4Box* scint_box = new G4Box("scint_box", dim[0]/2.-tank_wall_thick, dim[1]/2.-tank_wall_thick, dim[2]/2.);
    scint_log = new G4LogicalVolume(scint_box, MaterialsHelper::M().get6LiLS(MaterialsHelper::M().EJ309, scint6LiLoading), "ScintTank_scint_log");
    scint_log->SetVisAttributes(&scint_vis);
    scint_phys = new G4PVPlacement(NULL, G4ThreeVector(), scint_log, "ScintTank_scint_phys", main_log, false, 0, false);
    
    
    ///////////////////
    // pinwheel lattice
    G4ThreeVector r0(-0.5*nSegX*lat_size, -0.5*nSegY*lat_size, 0);      // starting point for rod placement
    G4ThreeVector sx0 = r0 + G4ThreeVector(lat_size/2., 0, 0);          // starting point for x-aligned separators
    G4ThreeVector sy0 = r0 + G4ThreeVector(0, lat_size/2., 0);          // starting point for y-aligned separators
    G4RotationMatrix* rotSepX = new G4RotationMatrix();                 // rotation for x-aligned separators (memory leaked!)
    rotSepX->rotateZ(90*deg+theta_pw);
    vector<G4PVPlacement*> seps;                                   // separators list (for optical surfaces)
    for(unsigned int nx = 0; nx <= nSegX; nx++) {
        for(unsigned int ny = 0; ny <= nSegY; ny++) {
            
            unsigned int copynum = nx + (nSegX+1)*ny;
            
            G4PVPlacement* rod = new G4PVPlacement(&rotRod, r0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                   myRod->main_log, "ScintTank_rod_phys_"+to_str(copynum),
                                                   scint_log, true, copynum, true);
            new G4LogicalBorderSurface("RodOpticalBorder_"+to_str(copynum), scint_phys, rod, myRod->myOptSurf.S);
            
            if(gammacatcher_log && nx < nSegX && ny < nSegY) {
                for(int sgn = -1; sgn <= 1; sgn += 2) {
                    new G4PVPlacement (&rotRod, r0 + G4ThreeVector((nx+0.5)*lat_size, (ny+0.5)*lat_size, sgn*(tank_depth-gc_thick)/2.),
                                       gammacatcher_log, "ScintTank_gc_phys_"+to_str(2*copynum + (sgn+1)/2),
                                       scint_log, true, 2*copynum + (sgn+1)/2, true);
                }
            }
            
            if(nx < nSegX)
                seps.push_back(new G4PVPlacement(rotSepX, sx0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                 mySeparator.main_log, "ScintTank_sepX_phys"+to_str(copynum),
                                                 scint_log, true, copynum, true));
            if(ny < nSegY)
                seps.push_back(new G4PVPlacement(&rotRod, sy0 + G4ThreeVector(nx*lat_size, ny*lat_size, 0),
                                                 mySeparator.main_log, "ScintTank_sepY_phys"+to_str(copynum),
                                                 scint_log, true, copynum, true));
        }
    }
    
    // apply separator boundary reflections; TODO gamma catchers, or try SkinSurface on separators
    for(unsigned int i=0; i<seps.size(); i++)
        new G4LogicalBorderSurface("SepOpticalBorder_"+to_str(i), scint_phys, seps[i], mySeparator.myOptSurf.S);
}

G4ThreeVector ScintTankBuilder::getSegmentPosition(unsigned int n) const {
    assert(n<getNSeg());
    return G4ThreeVector( ((n%nSegX) - 0.5*nSegX + 0.5)*lat_size, ((n/nSegX) - 0.5*nSegY + 0.5)*lat_size, 0 );
}

void ScintTankBuilder::setScintSD(G4VSensitiveDetector* SD) {
    ScintSegVol::setScintSD(SD);
    if(gammacatcher_log) gammacatcher_log->SetSensitiveDetector(SD);
}

int ScintTankBuilder::getSegmentNum(const G4ThreeVector& pos) const {
    // rod lattice coordinates
    int nx = floor(pos[0]/lat_size + nSegX/2.);
    int ny = floor(pos[1]/lat_size + nSegY/2.);

    if(theta_pw) {
        // position relative to rod lattice segment center
        G4TwoVector x(pos[0] - (nx + 0.5 - 0.5*nSegX)*lat_size, pos[1] - (ny + 0.5 - 0.5*nSegY)*lat_size);
        // rod center holes, plus a little bit of positioning slop
        if( pow(fabs(x[0])-0.5*lat_size,2) + pow(fabs(x[1])-0.5*lat_size,2) <= pow(myRod->r_hole,2) ) return -2;
        // rotated to pinwheeled segment coordinates
        x = G4TwoVector(cos_pw*x[0]-sin_pw*x[1], sin_pw*x[0]+cos_pw*x[1]);
        
        // adjust coordinates in adjacent squares
        if(x[0] > seg_size/2) nx++;
        else if(x[0] < -seg_size/2) nx--;
        if(x[1] > seg_size/2) ny++;
        else if(x[1] < -seg_size/2) ny--;
    }
    
    // outside lattice
    if(nx < 0 || nx >= (int)nSegX || ny < 0 || ny >= (int)nSegY) return -1;
    
    int segnum = nx + nSegX*ny;
            
    // gamma catcher
    if(pos[2] < -(tank_depth-2*gc_thick)/2.) return -(1000+segnum);
    if(pos[2] > (tank_depth-2*gc_thick)/2.) return -(2000+segnum);
    
    return segnum;
}

void ScintTankBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    addAttr(E, "seg_size", G4BestUnit(seg_size,"Length"));
    if(theta_pw) {
        addAttr(E, "lat_size", G4BestUnit(lat_size,"Length"));
        addAttr(E, "theta_pw", theta_pw);
    }
    addAttr(E, "gc_thick", G4BestUnit(gc_thick,"Length"));
    addAttr(E, "wall", G4BestUnit(tank_wall_thick,"Length"));
    addAttr(E, "buffer", G4BestUnit(ls_buffer_thick,"Length"));
    addAttr(E, "scint", scint_log->GetMaterial()->GetName());
    addAttrI(E, "nSegX", nSegX);
    addAttrI(E, "nSegY", nSegY);
}



///////////////////////////////
///////////////////////////////
///////////////////////////////

void SquareTankBuilder::setupDividers() {
    double sep_gap = mySlottedRod.r_hole + 0.5*mm; // gap between edge of separators and center of rods
    mySeparator.width = seg_size - 2*sep_gap;
    mySeparator.length = tank_depth;
    mySeparator.construct();
    mySlottedRod.construct(tank_depth-2*gc_thick, sep_gap, mySeparator.totalThick);
    
    myRod = &mySlottedRod;
}

void SquareTankBuilder::setupGammaCatcher() {
    double gc_width = seg_size-mySeparator.totalThick;
    G4Box* gc_box = new G4Box("gc_box", gc_width/2, gc_width/2, gc_thick/2);
    gammacatcher_log = new G4LogicalVolume(gc_box, MaterialsHelper::M().PMMA, "gammacatcher_log");
}
