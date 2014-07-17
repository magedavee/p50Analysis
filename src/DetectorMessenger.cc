// Unrestricted Use - Property of AECL
//
// DetectorMessenger.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Geometry and Optical Module Messenger
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
//  Modified 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "DetectorMessenger.hh"		// Specifies the file which contains the class structure

#include "DetectorConstruction.hh"		// Specifies user-defined classes which are called upon in this class

#include "G4UIdirectory.hh"			// Specifies the classes which contain structures called upon in this class
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UImanager.hh"
#include "G4Tokenizer.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"				// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
DetectorMessenger::DetectorMessenger(DetectorConstruction* det_const): detector(det_const)		// Allows retrieval of DetectorConstruction to set user-defined values
{
    // Specifies all commands available for DetectorConstruction.cc

    scintDir = new G4UIdirectory("/scint/");
    scintDir->SetGuidance("Custom scintillator volume settings.");
    scintDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    opticalCmd = new G4UIcmdWithABool("/scint/setOpticalProcesses",this);
    opticalCmd->SetGuidance("Toggle the production and transportation of optical photons in simulation");
    opticalCmd->SetGuidance("   true : optical photons enabled");
    opticalCmd->SetGuidance("   false: optical photons disabled");
    opticalCmd->SetParameterName("optical",true);
    opticalCmd->SetDefaultValue(true);
    opticalCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    birksCmd = new G4UIcmdWithADouble("/scint/setBirksConstant",this);
    birksCmd->SetGuidance("Set the Birks constant for quenching in scintillator");
    birksCmd->SetGuidance("   Input value in units of [mm/MeV].");
    birksCmd->SetParameterName("kB",true);
    birksCmd->SetDefaultValue(0.1);
    birksCmd->SetRange("kB>=0.0");
    birksCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    scintCompCmd = new G4UIcmdWithAString("/scint/setComposition",this);
    scintCompCmd->SetGuidance("Choose which preset material the Inner Tank will be filled with");
    scintCompCmd->SetGuidance("   The default is 0.1wt% Gd-loaded pseudocumene (T)");
    scintCompCmd->SetGuidance("     S = Pure pseudocumene (C9H12)");
    scintCompCmd->SetGuidance("     L = 0.15wt% Li in pseudocumene");
    scintCompCmd->SetGuidance("     M = 0.30wt% Li in pseudocumene");
    scintCompCmd->SetGuidance("     T = 0.1wt% Gd in pseudocumene");
    scintCompCmd->SetGuidance("     H = 0.5wt% Gd in pseudocumene");
    scintCompCmd->SetGuidance("     G = 100% Gd with density of pseudocumene");
    scintCompCmd->SetGuidance("     A = Pure pseudocumene scintillator, 0.5wt% Gd in reflector");
    scintCompCmd->SetGuidance("     B = Pure pseudocumene scintillator, 1.0wt% Gd in reflector");
    scintCompCmd->SetGuidance("     C = Pure pseudocumene scintillator, 2.0wt% Gd in reflector");
    scintCompCmd->SetParameterName("composition",true);
    scintCompCmd->SetDefaultValue("T");
    scintCompCmd->SetCandidates("S L T H M G A B C");
    scintCompCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    detectDir = new G4UIdirectory("/detector/");
    detectDir->SetGuidance("Custom detector geometry settings.");
    detectDir->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    QECmd = new G4UIcmdWithADouble("/detector/setQE",this);
    QECmd->SetGuidance("Set the quantum efficiency of the PMTs to a fixed value (0 results in default spectrum)");
    QECmd->SetParameterName("QE",false);
    QECmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segXCmd = new G4UIcmdWithAnInteger("/detector/setNumberSegmentsX",this);
    segXCmd->SetGuidance("Set the number of detector segments in the X dimension(1-10)");
    segXCmd->SetParameterName("SegX",false);
    segXCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    segYCmd = new G4UIcmdWithAnInteger("/detector/setNumberSegmentsY",this);
    segYCmd->SetGuidance("Set the number of detector segments in the Y dimension(1-10)");
    segYCmd->SetParameterName("SegY",false);
    segYCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    segLCmd = new G4UIcmdWithADoubleAndUnit("/detector/setSegmentLength",this);
    segLCmd->SetGuidance("Set the length of a segment (does not include PMT length)");
    segLCmd->SetParameterName("SegL",false);
    segLCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    segWCmd = new G4UIcmdWithADoubleAndUnit("/detector/setSegmentWidth",this);
    segWCmd->SetGuidance("Set the width of a segment");
    segWCmd->SetParameterName("SegW",false);
    segWCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    segHCmd = new G4UIcmdWithADoubleAndUnit("/detector/setSegmentHeight",this);
    segHCmd->SetGuidance("Set the height of a segment");
    segHCmd->SetParameterName("SegH",false);
    segHCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    shLeadCmd = new G4UIcmdWithADoubleAndUnit("/detector/setLeadShieldThickness",this);
    shLeadCmd->SetGuidance("Set the thickness of the lead shield");
    shLeadCmd->SetParameterName("ShLead",false);
    shLeadCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    shBPolyCmd = new G4UIcmdWithADoubleAndUnit("/detector/setBPolyShieldThickness",this);
    shBPolyCmd->SetGuidance("Set the thickness of the borated poly shield");
    shBPolyCmd->SetParameterName("ShBPoly",false);
    shBPolyCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    shLiPolyCmd = new G4UIcmdWithADoubleAndUnit("/detector/setLiPolyShieldThickness",this);
    shLiPolyCmd->SetGuidance("Set the thickness of the lithiated poly shield");
    shLiPolyCmd->SetParameterName("ShLiPoly",false);
    shLiPolyCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    scinLCmd = new G4UIcmdWithADoubleAndUnit("/detector/setScintillatorLength",this);
    scinLCmd->SetGuidance("Set the length of the scintillator");
    scinLCmd->SetParameterName("ScinL",false);
    scinLCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    scinWCmd = new G4UIcmdWithADoubleAndUnit("/detector/setScintillatorWidth",this);
    scinWCmd->SetGuidance("Set the width of the scintillator");
    scinWCmd->SetParameterName("ScinW",false);
    scinWCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    scinHCmd = new G4UIcmdWithADoubleAndUnit("/detector/setScintillatorHeight",this);
    scinHCmd->SetGuidance("Set the height of the scintillator");
    scinHCmd->SetParameterName("ScinH",false);
    scinHCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    segBuffCmd = new G4UIcmdWithADoubleAndUnit("/detector/setSegmentBuffer",this);
    segBuffCmd->SetGuidance("Set the buffer distance between the PMTs and scintilator");
    segBuffCmd->SetParameterName("SegBuff",false);
    segBuffCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segAirCmd = new G4UIcmdWithADoubleAndUnit("/detector/setAirGap",this);
    segAirCmd->SetGuidance("Set the air gap between segments");
    segAirCmd->SetParameterName("SegAir",false);
    segAirCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segWrapGapCmd = new G4UIcmdWithADoubleAndUnit("/detector/setWrapGap",this);
    segWrapGapCmd->SetGuidance("Set the air gap between segments");
    segWrapGapCmd->SetParameterName("SegWrapGap",false);
    segWrapGapCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segWrapCmd = new G4UIcmdWithADoubleAndUnit("/detector/setWrapThickness",this);
    segWrapCmd->SetGuidance("set the thickness of the outer segment wrap (diffuse reflector)");
    segWrapCmd->SetParameterName("SegWrap",false);
    segWrapCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segAcrylCmd = new G4UIcmdWithADoubleAndUnit("/detector/setAcrylThickness",this);
    segAcrylCmd->SetGuidance("set the thickness of the segment acrylic (excludes buffer dimension)");
    segAcrylCmd->SetParameterName("SegAcryl",false);
    segAcrylCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    segPMTCmd = new G4UIcmdWithADoubleAndUnit("/detector/setPMTDiameter",this);
    segPMTCmd->SetGuidance("set the diameter of the PMT");
    segPMTCmd->SetParameterName("SegPMT",false);
    segPMTCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    vertCmd = new G4UIcmdWithABool("/detector/setVertical",this);
    vertCmd->SetGuidance("true for vertical segments, false for horizontal segments");
    vertCmd->SetParameterName("vert",false);
    vertCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    innerCmd = new G4UIcmdWithABool("/detector/setInnerTankGeometry",this);
    innerCmd->SetGuidance("Enable/disable the inner tank geometry and associated volumes of simulation geometry");
    innerCmd->SetParameterName("Inner",false);
    innerCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    outerCmd = new G4UIcmdWithABool("/detector/setOuterTankGeometry",this);
    outerCmd->SetGuidance("Enable/disable the outer tank geometry of simulation geometry");
    outerCmd->SetParameterName("Outer",false);
    outerCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    optSurfCmd = new G4UIcmdWithAString("/detector/setSurfaceFinish",this);
    optSurfCmd->SetGuidance("Sets the surface finish for the segment wrapping:");
    optSurfCmd->SetCandidates("polished polishedfrontpainted polishedbackpainted ground groundfrontpainted groundbackpainted polishedlumirrorair polishedlumirrorglue polishedair polishedteflonair polishedtioair polishedtyvekair polishedvm2000air polishedvm2000glue etchedlumirrorair etchedlumirrorglue etchedair etchedteflonair etchedtioair etchedtyvekair etchedvm2000air etchedvm2000glue groundlumirrorair groundlumirrorglue groundair groundteflonair groundtioair groundtyvekair groundvm2000air groundvm2000glue");
    optSurfCmd->SetParameterName("OptSur",false);
    optSurfCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    shieldCmd = new G4UIcmdWithABool("/detector/setShield",this);
    shieldCmd->SetGuidance("Enable/disable the shielding volumes of simulation geometry");
    shieldCmd->SetParameterName("Shield",false);
    shieldCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    cylinderCmd = new G4UIcmdWithABool("/detector/setCylinder",this);
    cylinderCmd->SetGuidance("Enable/disable cylindrical sdetector segments");
    cylinderCmd->SetParameterName("Cylinder",false);
    cylinderCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    vetoCmd = new G4UIcmdWithABool("/detector/setMuonVetoPanel",this);
    vetoCmd->SetGuidance("Enable/disable the muon veto panels and associated volumes of simulation geometry");
    vetoCmd->SetParameterName("Veto",false);
    vetoCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    innerVCmd = new G4UIcmdWithABool("/detector/setInnerTankVisualization",this);
    innerVCmd->SetGuidance("Enable/disable the inner tank geometry and associated volumes of simulation geometry");
    innerVCmd->SetParameterName("InnerV",false);
    innerVCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    outerVCmd = new G4UIcmdWithABool("/detector/setOuterTankVisualization",this);
    outerVCmd->SetGuidance("Enable/disable the outer tank geometry of simulation geometry");
    outerVCmd->SetParameterName("OuterV",false);
    outerVCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    lobeCmd = new G4UIcmdWithADouble("/detector/setSpecularLobe",this);
    lobeCmd->SetGuidance("Set the SpecularLobe value (between 0.0 and 1.0");
    lobeCmd->SetParameterName("SpecLobe",false);
    lobeCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    sigalCmd = new G4UIcmdWithADouble("/detector/setSigmaAlpha",this);
    sigalCmd->SetGuidance("Set the SigmaAlpha value (between 0.0 and 3.14");
    sigalCmd->SetParameterName("SigmaAl",false);
    sigalCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    spikeCmd = new G4UIcmdWithADouble("/detector/setSpecularSpike",this);
    spikeCmd->SetGuidance("Set the SpecularSpike value (between 0.0 and 1.0");
    spikeCmd->SetParameterName("SpecSpike",false);
    spikeCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    reflCmd = new G4UIcmdWithADouble("/detector/setReflectivity",this);
    reflCmd->SetGuidance("Set the Reflectivity value (between 0.0 and 1.0");
    reflCmd->SetParameterName("Reflect",false);
    reflCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    effCmd = new G4UIcmdWithADouble("/detector/setEfficiency",this);
    effCmd->SetGuidance("Set the optical surface Efficiency value (between 0.0 and 1.0");
    effCmd->SetParameterName("Eff",false);
    effCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    backCmd = new G4UIcmdWithADouble("/detector/setBackScatter",this);
    backCmd->SetGuidance("Set the BackScatter value (between 0.0 and 1.0");
    backCmd->SetParameterName("Back",false);
    backCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    polyVCmd = new G4UIcmdWithABool("/detector/setBoratedPolyVisualization",this);
    polyVCmd->SetGuidance("Enable/disable the borated polyethylene layer of simulation geometry");
    polyVCmd->SetParameterName("BPolyV",false);
    polyVCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    shieldVCmd = new G4UIcmdWithABool("/detector/setWaterShieldVisualization",this);
    shieldVCmd->SetGuidance("Enable/disable the water shielding tanks of simulation geometry");
    shieldVCmd->SetParameterName("ShieldV",false);
    shieldVCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    vetoVCmd = new G4UIcmdWithABool("/detector/setMuonVetoVisualization",this);
    vetoVCmd->SetGuidance("Enable/disable the muon veto panels and associated volumes of simulation geometry");
    vetoVCmd->SetParameterName("VetoV",false);
    vetoVCmd->AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);

    printCmd = new G4UIcmdWithoutParameter("/detector/list",this);
    printCmd->SetGuidance("Lists all active geometry (physical) volumes");
    printCmd->AvailableForStates(G4State_Idle,G4State_GeomClosed);
}

DetectorMessenger::~DetectorMessenger() {
    delete scintDir;
    delete detectDir;
    delete opticalCmd;
    delete birksCmd;
    delete spikeCmd;
    delete effCmd;
    delete backCmd;
    delete lobeCmd;
    delete sigalCmd;
    delete reflCmd;
    delete scintCompCmd;
    delete innerCmd;
    delete vertCmd;
    delete outerCmd;
    delete polyCmd;
    delete optSurfCmd;
    delete shieldCmd;
    delete cylinderCmd;
    delete vetoCmd;
    delete innerVCmd;
    delete outerVCmd;
    delete polyVCmd;
    delete shieldVCmd;
    delete vetoVCmd;
    delete printCmd;
    delete segXCmd;
    delete QECmd;
    delete segYCmd;
    delete segLCmd;
    delete segHCmd;
    delete segWCmd;
    delete shLeadCmd;
    delete shBPolyCmd;
    delete shLiPolyCmd;
    delete scinLCmd;
    delete scinHCmd;
    delete scinWCmd;
    delete segBuffCmd;
    delete segAirCmd;
    delete segWrapCmd;
    delete segAcrylCmd;
    delete segPMTCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4VVisManager* vis_manager = G4VVisManager::GetConcreteInstance();
    
    if(command == opticalCmd) {
        G4bool setting = opticalCmd->GetNewBoolValue(newValue);
        if(detector->SetOpticalProcesses(setting)) {
            if(!setting) {
                UI->ApplyCommand("/process/inactivate Scintillation");
                UI->ApplyCommand("/process/inactivate Cerenkov");
                G4cerr << "optical generation turned off" << G4endl;
            } else {
                UI->ApplyCommand("/process/activate Scintillation");
                UI->ApplyCommand("/process/activate Cerenkov");
            }
        }
    } else if(command == birksCmd) detector->SetScintillatorBirksConstant(birksCmd->GetNewDoubleValue(newValue)*mm/MeV);
    else if(command == spikeCmd) detector->SetSpecularSpikeConstant(spikeCmd->GetNewDoubleValue(newValue));
    else if(command == effCmd) detector->SetEfficiency(effCmd->GetNewDoubleValue(newValue));
    else if(command == backCmd) detector->SetBackScatterConstant(backCmd->GetNewDoubleValue(newValue));
    else if(command == QECmd) detector->SetQE(QECmd->GetNewDoubleValue(newValue));
    else if(command == segXCmd) detector->SetXSegments(segXCmd->GetNewIntValue(newValue));
    else if(command == segYCmd) detector->SetYSegments(segYCmd->GetNewIntValue(newValue));
    else if(command == segLCmd) detector->SetSegmentLength(segLCmd->GetNewDoubleValue(newValue));
    else if(command == segWCmd) detector->SetSegmentWidth(segWCmd->GetNewDoubleValue(newValue));
    else if(command == segHCmd) detector->SetSegmentHeight(segHCmd->GetNewDoubleValue(newValue));
    else if(command == shLeadCmd) detector->SetLeadShieldThickness(shLeadCmd->GetNewDoubleValue(newValue));
    else if(command == shBPolyCmd) detector->SetBPolyShieldThickness(shBPolyCmd->GetNewDoubleValue(newValue));
    else if(command == shLiPolyCmd) detector->SetLiPolyShieldThickness(shLiPolyCmd->GetNewDoubleValue(newValue));
    else if(command == scinLCmd) detector->SetScintLength(scinLCmd->GetNewDoubleValue(newValue));
    else if(command == scinWCmd) detector->SetScintWidth(scinWCmd->GetNewDoubleValue(newValue));
    else if(command == scinHCmd) detector->SetScintHeight(scinHCmd->GetNewDoubleValue(newValue));
    else if(command == segBuffCmd) detector->SetSegmentBuffer(segBuffCmd->GetNewDoubleValue(newValue));
    else if(command == segAirCmd) detector->SetAirGap(segAirCmd->GetNewDoubleValue(newValue));
    else if(command == segWrapGapCmd) detector->SetWrapGap(segWrapGapCmd->GetNewDoubleValue(newValue));
    else if(command == segWrapCmd) detector->SetWrapThickness(segWrapCmd->GetNewDoubleValue(newValue));
    else if(command == segAcrylCmd) detector->SetAcrylThickness(segAcrylCmd->GetNewDoubleValue(newValue));
    else if(command == segPMTCmd) detector->SetPMTDiameter(segPMTCmd->GetNewDoubleValue(newValue));
    else if(command == lobeCmd) detector->SetSpecularLobeConstant(lobeCmd->GetNewDoubleValue(newValue));
    else if(command == sigalCmd) detector->SetSigmaAlpha(sigalCmd->GetNewDoubleValue(newValue));
    else if(command == reflCmd) detector->SetReflectivityConstant(reflCmd->GetNewDoubleValue(newValue));
    else if(command == scintCompCmd) detector->SetScintillatorComposition(newValue);
    else if(command == vertCmd) detector->SetVertical(vertCmd->GetNewBoolValue(newValue));
    else if(command == innerCmd) detector->SetInnerTankGeometry(innerCmd->GetNewBoolValue(newValue));
    else if(command == outerCmd) detector->SetOuterTankGeometry(outerCmd->GetNewBoolValue(newValue));
    else if(command == polyCmd) detector->SetBoratedPolyLayer(polyCmd->GetNewBoolValue(newValue));
    else if(command == optSurfCmd) {
        // detector->SetOpticalFinish(&optSurfCmd);
      
        if(newValue=="polished") detector->SetOpticalFinish(0);                   
        else if(newValue=="polishedfrontpainted") detector->SetOpticalFinish(1);
        else if(newValue=="polishedbackpainted") detector->SetOpticalFinish(2);          
        
        else if(newValue=="ground") detector->SetOpticalFinish(3);                       
        else if(newValue=="groundfrontpainted") detector->SetOpticalFinish(4);           
        else if(newValue=="groundbackpainted") detector->SetOpticalFinish(5);            

        else if(newValue=="polishedlumirrorair") detector->SetOpticalFinish(6);          
        else if(newValue=="polishedlumirrorglue") detector->SetOpticalFinish(7);         
        else if(newValue=="polishedair") detector->SetOpticalFinish(8);                  
        else if(newValue=="polishedteflonair") detector->SetOpticalFinish(9);            
        else if(newValue=="polishedtioair") detector->SetOpticalFinish(10);               
        else if(newValue=="polishedtyvekair") detector->SetOpticalFinish(11);             
        else if(newValue=="polishedvm2000air") detector->SetOpticalFinish(12);            
        else if(newValue=="polishedvm2000glue") detector->SetOpticalFinish(13);           
        
        else if(newValue=="etchedlumirrorair") detector->SetOpticalFinish(14);              
        else if(newValue=="etchedlumirrorglue") detector->SetOpticalFinish(15);           
        else if(newValue=="etchedair") detector->SetOpticalFinish(16);                    
        else if(newValue=="etchedteflonair") detector->SetOpticalFinish(17);              
        else if(newValue=="etchedtioair") detector->SetOpticalFinish(18);                 
        else if(newValue=="etchedtyvekair") detector->SetOpticalFinish(19);               
        else if(newValue=="etchedvm2000air") detector->SetOpticalFinish(20);              
        else if(newValue=="etchedvm2000glue") detector->SetOpticalFinish(21);             
	
        else if(newValue=="groundlumirrorair") detector->SetOpticalFinish(22);            
        else if(newValue=="groundlumirrorglue") detector->SetOpticalFinish(23);           
        else if(newValue=="groundair") detector->SetOpticalFinish(24);                    
        else if(newValue=="groundteflonair") detector->SetOpticalFinish(25);              
        else if(newValue=="groundtioair") detector->SetOpticalFinish(26);                 
        else if(newValue=="groundtyvekair") detector->SetOpticalFinish(27);               
        else if(newValue=="groundvm2000air") detector->SetOpticalFinish(28);              
        else if(newValue=="groundvm2000glue") detector->SetOpticalFinish(29);    
    
    } else if(command == shieldCmd) detector->SetShieldLayer(shieldCmd->GetNewBoolValue(newValue));
    else if(command == cylinderCmd) detector->SetCylinder(cylinderCmd->GetNewBoolValue(newValue));
    else if(command == vetoCmd) detector->SetMuonVetoLayer(vetoCmd->GetNewBoolValue(newValue));
    else if(command == innerVCmd) {
#ifdef G4VIS_USE
        if(vis_manager) {
            if(innerVCmd->GetNewBoolValue(newValue)) {
                UI->ApplyCommand("/vis/geometry/set/visibility InnerTankLogical 0 1");
                //        UI->ApplyCommand("/vis/geometry/set/visibility OpticalWindowLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility InnerScintLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility AirGapLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility InnerPMTBraceLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PseudocumenePMTLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPCCoverLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPCBaseLogical 0 1");
            } else {
                UI->ApplyCommand("/vis/geometry/set/visibility InnerTankLogical 0 0");
                //        UI->ApplyCommand("/vis/geometry/set/visibility OpticalWindowLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility InnerScintLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility AirGapLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility InnerPMTBraceLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PseudocumenePMTLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPCCoverLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPCBaseLogical 0 0");
            }
        }
#endif
    } else if(command == outerVCmd) {
#ifdef G4VIS_USE
        if(vis_manager) {
            if(outerVCmd->GetNewBoolValue(newValue)) {
                UI->ApplyCommand("/vis/geometry/set/visibility OuterTankLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility MineralOilLogical 0 1");
            } else {
                UI->ApplyCommand("/vis/geometry/set/visibility OuterTankLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility MineralOilLogical 0 0");
            }
        }
#endif
    } else if(command == polyVCmd) {
#ifdef G4VIS_USE
        if(vis_manager) {
            if(polyVCmd->GetNewBoolValue(newValue)) {
                UI->ApplyCommand("/vis/geometry/set/visibility BPolySideLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility BPolyEndLogical 0 1");
            } else {
                UI->ApplyCommand("/vis/geometry/set/visibility BPolySideLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility BPolyEndLogical 0 0");
            }
        }
#endif
    } else if(command == shieldVCmd) {
#ifdef G4VIS_USE
        if(vis_manager) {
            if(shieldVCmd->GetNewBoolValue(newValue)) {
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallALogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallBLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallCLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldALogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldBLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldCLogical 0 1");
            } else {
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallALogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallBLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility ShieldWallCLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldALogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldBLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility WaterShieldCLogical 0 0");
            }
        }
#endif
    } else if(command == vetoVCmd) {
#ifdef G4VIS_USE
        if(vis_manager) {
            if(vetoVCmd->GetNewBoolValue(newValue)) {
                UI->ApplyCommand("/vis/geometry/set/visibility VetoStraightLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoStraightSmLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoDoubleLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoSingleLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoWGuideLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoTGuideLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoSGuideLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PolyvinylToluenePMTLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPVTCoverLogical 0 1");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPVTBaseLogical 0 1");
            } else {
                UI->ApplyCommand("/vis/geometry/set/visibility VetoStraightLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoStraightSmLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoDoubleLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoSingleLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoWGuideLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoTGuideLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility VetoSGuideLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PolyvinylToluenePMTLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPVTCoverLogical 0 0");
                UI->ApplyCommand("/vis/geometry/set/visibility PMTPVTBaseLogical 0 0");
            }
        }
#endif
    } else if(command == printCmd) detector->PrintPhysicalVolumes();
    else G4cout << "Command not found." << G4endl;
}

	// ****** Return Command String ****** //
G4String DetectorMessenger::GetCurrentValue(G4UIcommand* /*command*/)
{
  G4String cv;
  cv = "Command not implemented.";
  return cv;
}

// EOF
