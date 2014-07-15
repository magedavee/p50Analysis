
// Unrestricted Use - Property of AECL
//
// DetectorConstruction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Geometry, Material, and Sensitive Volume Specifications
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef DetectorConstruction_H			// Only carries out following if object is undefined
#define DetectorConstruction_H 1		// Defines file

#include "G4VUserDetectorConstruction.hh" 	// Specifies base class or parent class
#include "G4LogicalBorderSurface.hh"		// These are the GEANT4 classes necessary to construct the desired surfaces
#include "G4LogicalSkinSurface.hh"		//	|
#include "G4OpBoundaryProcess.hh"

#include "globals.hh"				// Specifies class defining all global parameters and variable types

class G4LogicalVolume;				// Structures necessary for class definition
class G4VPhysicalVolume;
class G4VisAttributes;
class G4Material;
class DetectorMessenger;
class G4MultiFunctionalDetector;

/* -------- Class Definition --------- */

class DetectorConstruction: public G4VUserDetectorConstruction		// This class inherits functions from G4VUserDetectorConstruction
{
  friend class DetectorMessenger;		// Friend classes have access to all member data and internal methods of this class

public:	// Constructors and Destructors

  DetectorConstruction();		// Constructor
  virtual ~DetectorConstruction();	// Destructor

public:	// Accesible Methods

  G4VPhysicalVolume* Construct();	// Assembles geometry of the simulation, returns a G4VPhysicalVolume pointer
  void ReConstruct();	// Assembles geometry of the simulation, returns a G4VPhysicalVolume pointer

  G4double GetWorldSizeX() const { return modSizeX+2.0*m; };
  G4double GetWorldSizeY() const { return modSizeY+2.0*m; };
  G4double GetWorldSizeZ() const { return modSizeZ+2.0*m; };
  G4double GetNeutronPositionX() const { return nPos_x; };
  G4double GetNeutronPositionY() const { return nPos_y; };
  G4double GetNeutronPositionZ() const { return nPos_z; };
  G4double GetAntiNeutrinoPositionX() const { return nuPos_x; };
  G4double GetAntiNeutrinoPositionY() const { return nuPos_y; };
  G4double GetAntiNeutrinoPositionZ() const { return nuPos_z; };
  G4double GetMuonPositionX() const { return muPos_x; };
  G4double GetMuonPositionY() const { return muPos_y; };
  G4double GetMuonPositionZ() const { return muPos_z; };
  G4double GetInnerChamberOffset() const { return inner_offset; };
  G4double GetScintVolumeOffset() const { return scint_offset; };
  G4double GetReflectivity() const { return refl;};
  G4double GetMaxHalfDimension();
  G4String GetScintillatorComposition() const { return material; };
  G4bool IsOpticalTransported() const { return fOptical; };

  G4bool GetInnerTankInclusion() const { return fInnerActivated; };
  G4bool GetOuterTankInclusion() const { return fOuterActivated; };
  G4bool GetBPolyInclusion() const { return fBPolyActivated; };
  G4bool GetShieldInclusion() const { return fShieldActivated; };
  G4bool GetMuonVetoInclusion() const { return fVetoActivated; };

  int GetNSegX() const { return NSegX; };
  int GetNSegY() const { return NSegY; };
  double GetPositionX(int i) { return PositionX[i];}
  double GetPositionY(int i) { return PositionY[i];}
  int GetFinish() const { return OptFinish; };
  double GetAirGap() const { return AirGap;};
  double GetWrapGap() const { return WrapGap;};
  double GetWrapThickness() const { return WrapThickness;};
  double GetAcrylThickness() const { return AcrylThickness;};
  double GetSegBuffer() const { return SegBuffer;};
  double GetScintLength() const { return ScintLength;};
  double GetScintHeight() const { return ScintHeight;};
  double GetScintWidth() const { return ScintWidth;};
  double GetSegLength() const { return  SegLength;};
  double GetSegWidth() const { return SegWidth;};
  double GetSegHeight() const { return SegHeight;};
  double GetPMTscale() const { return PMTscale;};
  double GetShieldLead() const { return ShieldLead;};
  double GetShieldPolyB() const { return ShieldPolyB;};
  double GetShieldPolyLi() const { return ShieldPolyLi;};
  bool GetVertical() const { return fVertical;};
  int GetScint() const { return Scint;};
  G4LogicalVolume* GetScintLog(G4int x, G4int y) {if(y<NSegY && x<NSegX) return scint_log[x][y]; else return scint_log[0][0];};





  void PrintPhysicalVolumes() const;

protected:

  G4bool SetOpticalProcesses(G4bool);	
  void SetVertical(G4bool);			
  void Modify();				
  void ConstructNew();			
  void SetScintillatorBirksConstant(G4double);
  void SetVetoBirksConstant(G4double);
  void SetSpecularLobeConstant(G4double);
  void SetSigmaAlpha(G4double);
  void SetSpecularSpikeConstant(G4double);
  void SetEfficiency(G4double);
  void SetBackScatterConstant(G4double);
  void SetReflectivityConstant(G4double);
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
  void SetQE(G4double);
  void SetOpticalFinish(G4int);

  void SetInnerTankGeometry(G4bool,G4bool verb = true);	// Geometrical settings
  void SetOuterTankGeometry(G4bool,G4bool verb = true);
  void SetGammaCatcherGeometry(G4bool,G4bool verb = true);
  void SetBoratedPolyLayer(G4bool,G4bool verb = true);
  void SetShieldLayer(G4bool,G4bool verb = true);
  void SetCylinder(G4bool,G4bool verb = true);
  void SetMuonVetoLayer(G4bool,G4bool verb = true);
  void SetExplodedView(G4bool);

  void ConstructMaterials();					// Sub-construction methods
  void ConstructMaterialProperties();
  G4VPhysicalVolume* ConstructDetector();
  void ConstructOpticalSurfaces();

private:	// Member Data

  // World Sizes
  G4double modSizeX;
  G4double modSizeY;
  G4double modSizeZ;
  static const G4int MaxSegX = 100;
  static const G4int MaxSegY = 100;
  static const G4int MaxSeg = 2500;
  G4double QE;
    
  // Segment Variables
  G4int NSegX;
  G4int NSegY;
  G4double PositionX[MaxSeg];
  G4double PositionY[MaxSeg];
  G4OpticalSurfaceFinish OptFinish;

  G4double SegLength;
  G4double SegWidth;
  G4double SegHeight;
  G4double ScintLength;
  G4double ScintWidth;
  G4double ScintHeight;
  G4double SegBuffer;
  G4double AirGap;
  G4double WrapGap;
  G4double WrapThickness;
  G4double AcrylThickness;
  G4double PMTscale;
  G4double ShieldLead;
  G4double ShieldPolyB;
  G4double ShieldPolyLi;
  G4int Scint;

  // Source Positions
  G4double nPos_x, nPos_y, nPos_z;
  G4double nuPos_x, nuPos_y, nuPos_z;
  G4double muPos_x, muPos_y, muPos_z;
  G4double inner_offset, scint_offset;

  // Logical Volumes
  G4LogicalVolume* build_log;
  G4LogicalVolume* wrapgap_log[MaxSegX][MaxSegY];
  G4LogicalVolume* world_log;
  G4LogicalVolume* bg_log;
  G4LogicalVolume* innerbg_log;
  G4LogicalVolume* shieldlead_log;
  G4LogicalVolume* shieldpolyb_log;
  G4LogicalVolume* shieldpolyli_log;
  G4LogicalVolume* shell_log;
  G4LogicalVolume* floor_log;
  G4LogicalVolume* layer_log;
  G4LogicalVolume* outer_log;
  G4LogicalVolume* oil_log;
  G4LogicalVolume* optical_log;
  G4LogicalVolume* segment_log[MaxSegX][MaxSegY];
  G4LogicalVolume* end_log;
  G4LogicalVolume* separator_log1;
  G4LogicalVolume* separator_log2;	
  G4LogicalVolume* inner_log;
  G4LogicalVolume* target_log[MaxSegX][MaxSegY];
  G4LogicalVolume* innerwrap_log;
  G4LogicalVolume* scint_log[MaxSegX][MaxSegY];
  G4LogicalVolume* gc_scint_log;
  G4LogicalVolume* air_log;
  G4LogicalVolume* borate_log[2];
  G4LogicalVolume* shield_log[3];
  G4LogicalVolume* water_log[3];
  G4LogicalVolume* panel_log[4];
  G4LogicalVolume* guide_log[3];
  G4LogicalVolume* pmtPVT_log;
  G4LogicalVolume* coverPVT_log;
  G4LogicalVolume* basePVT_log;
  G4LogicalVolume* cathSEG_log;
  G4LogicalVolume* pmtSEG_log;
  G4LogicalVolume* coverSEG_log;
  G4LogicalVolume* baseSEG_log;
  G4LogicalVolume* brace_log;
  G4LogicalVolume* pmtPC_log;
  G4LogicalVolume* coverPC_log;
  G4LogicalVolume* basePC_log;

  // Physical Volumes
  G4VPhysicalVolume* build_phys;
  G4VPhysicalVolume* world_phys;
  G4VPhysicalVolume* bg_phys;
  G4VPhysicalVolume* innerbg_phys;
  G4VPhysicalVolume* shieldlead_phys;
  G4VPhysicalVolume* shieldpolyb_phys;
  G4VPhysicalVolume* shieldpolyli_phys;
  G4VPhysicalVolume* shell_phys;
  G4VPhysicalVolume* floor_phys;
  G4VPhysicalVolume* layer_phys;
  G4VPhysicalVolume* outer_phys;
  G4VPhysicalVolume* segment_phys[MaxSegX][MaxSegY];
  G4VPhysicalVolume* wrapgap_phys[MaxSegX][MaxSegY];
  G4VPhysicalVolume* oil_physN;
  G4VPhysicalVolume* oil_physS;
  G4VPhysicalVolume* optical_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* optical_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* inner_phys;
  G4VPhysicalVolume* target_phys[MaxSegX][MaxSegY];
  G4VPhysicalVolume* scint_phys[MaxSegX][MaxSegY];
  G4VPhysicalVolume* innerwrap_phys[MaxSegX][MaxSegY];
  G4VPhysicalVolume* gc_scint_phys;
  G4VPhysicalVolume* air_phys;
  G4VPhysicalVolume* borate_phys[6];
  G4VPhysicalVolume* shield_phys[3][2];
  G4VPhysicalVolume* water_phys[3];
  G4VPhysicalVolume* door_phys[12];
  G4VPhysicalVolume* side_phys[10];
  G4VPhysicalVolume* top_phys[11];
  G4VPhysicalVolume* sideG_phys[10][2];
  G4VPhysicalVolume* topG_phys[11][2];
  G4VPhysicalVolume* pmtPVT_phys[66];
  G4VPhysicalVolume* coverPVT_phys[66];
  G4VPhysicalVolume* cap_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* cap_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* cathSEG_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* baseSEG_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* pmtSEG_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* coverSEG_physN[MaxSegX][MaxSegY];
  G4VPhysicalVolume* cathSEG_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* baseSEG_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* pmtSEG_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* coverSEG_physS[MaxSegX][MaxSegY];
  G4VPhysicalVolume* basePVT_phys[66];
  G4VPhysicalVolume* brace_phys[2];
  G4VPhysicalVolume* pmtPC_phys[32];
  G4VPhysicalVolume* coverPC_phys[32];
  G4VPhysicalVolume* basePC_phys[32];

  // Detectors
  G4MultiFunctionalDetector* scintHitInner;
  //    G4MultiFunctionalDetector* scintHitVeto;
  G4MultiFunctionalDetector* opticalPMT;
  // G4MultiFunctionalDetector* vetoPMT;
  G4MultiFunctionalDetector* SegmentPMT;

  // Visualization
  G4VisAttributes* cath_vis;	
  G4VisAttributes* build_vis;	
  G4VisAttributes* wrapgap_vis;	
  G4VisAttributes* world_vis;		// Visualization attributes of various physical volumes
  G4VisAttributes* bg_vis;
  G4VisAttributes* ibg_vis;
  G4VisAttributes* shieldlead_vis;
  G4VisAttributes* shieldpolyb_vis;
  G4VisAttributes* shieldpolyli_vis;
  G4VisAttributes* shell_vis;
  G4VisAttributes* floor_vis;
  G4VisAttributes* layer_vis;
  G4VisAttributes* segment_vis;
  G4VisAttributes* end_vis;
  G4VisAttributes* outer_vis;
  G4VisAttributes* oil_vis;
  G4VisAttributes* optical_vis;
  G4VisAttributes* separator_vis;
  G4VisAttributes* inner_vis;
  G4VisAttributes* target_vis;
  G4VisAttributes* scint_vis;
  G4VisAttributes* innerwrap_vis;
  G4VisAttributes* gc_scint_vis;
  G4VisAttributes* air_vis;
  G4VisAttributes* borate_vis;
  G4VisAttributes* shield_vis;
  G4VisAttributes* water_vis;
  G4VisAttributes* panel_vis;
  G4VisAttributes* guide_vis;
  G4VisAttributes* brace_vis;
  G4VisAttributes* pmt_vis;
  G4VisAttributes* cover_vis;
  G4VisAttributes* base_vis;

  // Extra Stuffs
  DetectorMessenger* det_messenger;
  G4String material;			// Material modifier
  G4double birksPC, birksPVT;		// Birks constants modifier
  G4double lobe, spike, refl,back, diff, efficiency, sigal;         //optical surface parameters
  G4bool fOptical;			// Flag to specify optical processes
  G4bool fInnerActivated;		// Flag to specify inner tank geometry
  G4bool fOuterActivated;		// Flag to specify outer tank geometry
  G4bool fBPolyActivated;		// Flag to specify polyethylene layer
  G4bool fShieldActivated;		// Flag to specify water shield tanks
  G4bool fCylinderActivated;		// Flag to specify cyllindrical segments
  G4bool fVetoActivated;		// Flag to specify veto panel layer
  G4bool fExploded;			// Flag to specify exploded view
  G4bool fVertical;			// Flag to specify vertical segments
  G4bool modifying;			// Flag to specify the geometry is being modified 
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
