// Unrestricted Use - Property of AECL
//
// DetectorConstruction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Geometry, Material, and Sensitive Volume Specifications
//  Contains class functions/variables
//
// --------------------------------------------------------
//  Version 1.01 - 2011/04/29 - A. Ho
//  Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef DetectorConstruction_H
/// Assure header is only loaded once
#define DetectorConstruction_H

#include "G4VUserDetectorConstruction.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpBoundaryProcess.hh"

#include "globals.hh"

#include <cassert>

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VisAttributes;
class G4Material;
class DetectorMessenger;
class G4MultiFunctionalDetector;

class DetectorConstruction: public G4VUserDetectorConstruction {
    
  friend class DetectorMessenger;

public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    virtual ~DetectorConstruction();

    /// Assembles geometry of the simulation, returns world volume
    G4VPhysicalVolume* Construct();

    G4double GetWorldSizeX() const { return modSizeX+2.0*m; }
    G4double GetWorldSizeY() const { return modSizeY+2.0*m; }
    G4double GetWorldSizeZ() const { return modSizeZ+2.0*m; }
    G4double GetReflectivity() const { return refl;}
    G4double GetMaxHalfDimension();
    G4bool IsOpticalTransported() const { return fOptical; }

    G4bool GetInnerTankInclusion() const { return fInnerActivated; }
    G4bool GetOuterTankInclusion() const { return fOuterActivated; }
    G4bool GetBPolyInclusion() const { return fBPolyActivated; }
    G4bool GetShieldInclusion() const { return fShieldActivated; }
    G4bool GetMuonVetoInclusion() const { return fVetoActivated; }

    int GetNSegX() const { return NSegX; }
    int GetNSegY() const { return NSegY; }
    double GetPositionX(int i) { return PositionX[i]; }
    double GetPositionY(int i) { return PositionY[i]; }
    int GetFinish() const { return OptFinish; }
    double GetAirGap() const { return AirGap; }
    double GetWrapGap() const { return WrapGap; }
    double GetWrapThickness() const { return WrapThickness; }
    double GetAcrylThickness() const { return AcrylThickness; }
    double GetSegBuffer() const { return SegBuffer; }
    double GetScintLength() const { return ScintLength; }
    double GetScintHeight() const { return ScintHeight; }
    double GetScintWidth() const { return ScintWidth; }
    
    /// total length of the segment (not including PMTs) (this is defined as the z dimension)
    double GetSegLength() const { return  ScintLength+2*SegBuffer+2*WrapThickness; }
    /// total width of the segment (defined as x)
    double GetSegWidth() const { return ScintWidth + 2*AcrylThickness + 2*WrapThickness+2*WrapGap; }
    /// total height of the segment (defined as y)
    double GetSegHeight() const { return ScintHeight + 2*AcrylThickness + 2*WrapThickness + 2*WrapGap; }
    
    double GetPMTscale() const { return PMTscale; }
    double GetShieldLead() const { return ShieldLead; }
    double GetShieldPolyB() const { return ShieldPolyB; }
    double GetShieldPolyLi() const { return ShieldPolyLi; }
    bool GetVertical() const { return fVertical; }
    //int GetScint() const { return Scint; }
    G4LogicalVolume* GetScintLog() { return scint_log; }

    void PrintPhysicalVolumes() const;

protected:

    G4bool SetOpticalProcesses(G4bool);
    void SetVertical(G4bool sp) { fVertical = sp; }
    /// Change Liquid Scintillator Birks Quenching Constant
    void SetScintillatorBirksConstant(G4double);
    /// Change Plastic Scintillator Birks Quenching Constant TODO
    void SetVetoBirksConstant(G4double) { assert(false); }
    void SetSpecularLobeConstant(G4double sp) { lobe = sp < 0 ? 0 : sp > 1 ? 1 : sp; }
    void SetSigmaAlpha(G4double sp) { sigal = sp < 0 ? 0 : sp > M_PI ? M_PI : sp; }
    void SetSpecularSpikeConstant(G4double sp) { spike = sp < 0 ? 0 : sp > 1 ? 1 : sp; }
    void SetEfficiency(G4double sp) { efficiency = sp < 0 ? 0 : sp > 1 ? 1 : sp; }
    void SetBackScatterConstant(G4double sp) { back = sp < 0 ? 0 : sp > 1 ? 1 : sp; }
    void SetReflectivityConstant(G4double sp) { refl = sp < 0 ? 0 : sp > 1 ? 1 : sp; }
    void SetScintillatorComposition(G4String);
    void SetXSegments(G4int);
    void SetYSegments(G4int);
    void SetSegmentLength(G4double);
    void SetSegmentWidth(G4double);
    void SetSegmentHeight(G4double);
    void SetSegmentBuffer(G4double);
    void SetLeadShieldThickness(G4double);
    void SetBPolyShieldThickness(G4double);
    void SetLiPolyShieldThickness(G4double);
    void SetAirGap(G4double);
    void SetWrapGap(G4double);
    void SetWrapThickness(G4double);
    void SetAcrylThickness(G4double);
    void SetScintLength(G4double);
    void SetScintWidth(G4double);
    void SetScintHeight(G4double);
    void SetPMTDiameter(G4double);
    void SetQE(G4double qeval) { QE = qeval>0 ? (qeval<1 ? qeval : 1) : 0; }
    void SetOpticalFinish(G4int);

    /// Inner Tank Geometry Switch TODO
    void SetInnerTankGeometry(G4bool, G4bool = true) { assert(false); }
    /// Outer Tank Geometry Switch TODO
    void SetOuterTankGeometry(G4bool, G4bool = true) { assert(false); }
    /// Gamma Catcher Switch TODO
    void SetGammaCatcherGeometry(G4bool, G4bool = true) { assert(false); }
    /// Borated Polyehtylene Switch TODO
    void SetBoratedPolyLayer(G4bool, G4bool = true) { assert(false); }
    /// Water Shield Tank Switch
    void SetShieldLayer(G4bool flag, G4bool = true) { fShieldActivated=flag; }
    /// Cylindrical segment switch Switch
    void SetCylinder(G4bool flag, G4bool = true) { fCylinderActivated=flag; }
    /// Muon Veto Panel Switch TODO
    void SetMuonVetoLayer(G4bool, G4bool = true) { assert(false); }

private:

    /// Materials composition
    void ConstructMaterials();
    /// Material Optical Properties - for Scintillation and Optical Photons
    void ConstructMaterialProperties();
    /// Construct world/detector geometry
    void ConstructGeometry();
    /// Optical Surface Specifications
    void ConstructOpticalSurfaces();
    /// Sensitive Detectors setup
    void ConstructSDs();
    /// Visualization options specification
    void SetupVisualization();
    
    // World Sizes
    G4double modSizeX;
    G4double modSizeY;
    G4double modSizeZ;
    static const G4int MaxSegX = 100;
    static const G4int MaxSegY = 100;
    static const G4int MaxSeg = 2500;
    G4double QE;
    
    // Segment Variables
    G4int NSegX;                ///< the number of segments across
    G4int NSegY;                ///< the number of segments in height
    G4double PositionX[MaxSeg];
    G4double PositionY[MaxSeg];
    G4OpticalSurfaceFinish OptFinish;  ///< G4OpticalSurfaceFinish(4)

    G4double ScintLength;
    G4double ScintWidth;
    G4double ScintHeight;
    G4double SegBuffer;         ///< the ammount of buffer material(acrylic) between the PMT and scintillator
    G4double AirGap;            ///< air gap width between segments
    G4double WrapGap;           ///< air gap between outer wrap and acrylic
    G4double WrapThickness;     ///< Thickness of Outer Tank - approximately 1/8"
    G4double AcrylThickness;    ///< thickness of the sides of the acrylic segments
    G4double PMTscale;          ///< PMT photocathode radius
    G4double ShieldLead;
    G4double ShieldPolyB;
    G4double ShieldPolyLi;
    
    // Materials
    G4Material** MainScintMat;  ///< main scintillator material; set to one of the variants below
    G4Material** ScintSegMat;   ///< scintillator volumes segmentation material
    G4Material* Air;
    G4Material* StSteel;        ///< Type 444 Stainless Steel
    G4Material* concrete;       ///< concrete
    G4Material* MinOil;
    G4Material* PMMA;
    G4Material* RawPsiCumene;
    G4Material* PsiCumeneL;
    G4Material* PsiCumeneM;
    G4Material* PsiCumeneT;
    G4Material* PsiCumeneH;
    G4Material* PsiCumeneG;
    G4Material* MuonScint;
    G4Material* Water;
    G4Material* Polyeth;
    G4Material* GdPaintA;
    G4Material* GdPaintB;
    G4Material* GdPaintC;
    G4Material* Pb;
    G4Material* BPoly;
    G4Material* LiPoly;
    G4Material* Vacuum;
    G4Material* Quartz;
    
    // Logical Volumes
    G4LogicalVolume* build_log;
    G4LogicalVolume* wrapgap_log;
    G4LogicalVolume* hall_log;
    G4LogicalVolume* shieldlead_log;
    G4LogicalVolume* shieldpolyb_log;
    G4LogicalVolume* shieldpolyli_log;
    G4LogicalVolume* shell_log;
    G4LogicalVolume* layer_log;
    G4LogicalVolume* segment_log;
    G4LogicalVolume* target_log;
    G4LogicalVolume* scint_log;
    G4LogicalVolume* cathSEG_log;
    G4LogicalVolume* pmtSEG_log;
    G4LogicalVolume* coverSEG_log;
    G4LogicalVolume* baseSEG_log;

    // Physical Volumes
    G4VPhysicalVolume* build_phys;              ///< concrete-walled building ("world" volume)
    G4VPhysicalVolume* hall_phys;               ///< experimental hall air volume
    G4VPhysicalVolume* shieldlead_phys;         ///< lead shield layer
    G4VPhysicalVolume* shieldpolyb_phys;        ///< borated poly shield layer
    G4VPhysicalVolume* shieldpolyli_phys;       ///< 6Li poly shield layer
    G4VPhysicalVolume* shell_phys;              ///< detector housing shell (air)
    G4VPhysicalVolume* segment_phys[MaxSegX][MaxSegY];
    G4VPhysicalVolume* wrapgap_phys[MaxSegX][MaxSegY];
    G4VPhysicalVolume* target_phys[MaxSegX][MaxSegY];
    G4VPhysicalVolume* scint_phys[MaxSegX][MaxSegY];
    G4VPhysicalVolume* shield_phys[3][2];
    G4VPhysicalVolume* door_phys[12];
    G4VPhysicalVolume* side_phys[10];
    G4VPhysicalVolume* top_phys[11];
    G4VPhysicalVolume* cathSEG_phys[2][MaxSegX][MaxSegY];
    G4VPhysicalVolume* baseSEG_phys[2][MaxSegX][MaxSegY];
    G4VPhysicalVolume* pmtSEG_phys[2][MaxSegX][MaxSegY];
    G4VPhysicalVolume* coverSEG_phys[2][MaxSegX][MaxSegY];

    // Detectors
    G4MultiFunctionalDetector* scintHitInner;
    // G4MultiFunctionalDetector* scintHitVeto;
    G4MultiFunctionalDetector* opticalPMT;
    // G4MultiFunctionalDetector* vetoPMT;
    G4MultiFunctionalDetector* SegmentPMT;

    // Visualization
    G4VisAttributes* build_vis;
    G4VisAttributes* hall_vis;
    G4VisAttributes* shieldlead_vis;
    G4VisAttributes* shieldpolyb_vis;
    G4VisAttributes* shieldpolyli_vis;
    G4VisAttributes* shell_vis;
    G4VisAttributes* segment_vis;
    G4VisAttributes* wrapgap_vis;
    G4VisAttributes* target_vis;
    G4VisAttributes* scint_vis;
    G4VisAttributes* shield_vis;
    G4VisAttributes* pmt_vis;
    G4VisAttributes* cover_vis;
    G4VisAttributes* cath_vis;
    G4VisAttributes* base_vis;

    // Extra Stuffs
    DetectorMessenger* det_messenger;   ///< UI for detector construction control
    G4double birksPC, birksPVT;         ///< Birks constants modifier
    G4double lobe, spike, refl,back, diff, efficiency, sigal;   ///< optical surface parameters
    G4bool fOptical;                    ///< Flag to specify optical processes
    G4bool fInnerActivated;             ///< Flag to specify inner tank geometry
    G4bool fOuterActivated;             ///< Flag to specify outer tank geometry
    G4bool fBPolyActivated;             ///< Flag to specify polyethylene layer
    G4bool fShieldActivated;            ///< Flag to specify water shield tanks
    G4bool fCylinderActivated;          ///< Flag to specify cyllindrical segments
    G4bool fVetoActivated;              ///< Flag to specify veto panel layer
    G4bool fVertical;                   ///< Flag to specify vertical segments
};

#endif
