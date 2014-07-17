
///////////////////////////////////////////////////////////////////////////////
//                   Spectrum of radiative neutron capture by Gadolinium            
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               
                            
//Modified class from original G4NeutronHPCapture class to include 
//the gammas spectrum of radiative neutron capture by Gadolinium.

// Karim Zbiri, April, 2005
///////////////////////////////////////////////////////////////////////////////


#include "GdNeutronHPCapture.hh"
/////////#include "OtherHPCaptureFS.hh"
#include "G4NeutronHPCaptureFS.hh"
#include "G4NeutronHPDeExGammas.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

#include "GdNeutronHPCaptureFS.hh"

  GdNeutronHPCapture::GdNeutronHPCapture()
   :G4HadronicInteraction("GdNeutronHPCapture")
  {
    SetMinEnergy( 0.0 );
    SetMaxEnergy( 20.*MeV );
    G4cout << "Capture : start of construction!!!!!!!!" << G4endl;
    if(!getenv("G4NEUTRONHPDATA")) 
       throw G4HadronicException(__FILE__, __LINE__, "Please setenv NeutronHPCrossSections to point to the neutron cross-section files.");
    dirName = getenv("G4NEUTRONHPDATA");
    G4String tString = "/Capture/";
    dirName = dirName + tString;
    numEle = G4Element::GetNumberOfElements();
   G4cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << G4endl;
   //    G4cout << "Disname=" <<dirName<< " numEle=" <<numEle<< G4endl;
  
    theCapture = new G4NeutronHPChannel[numEle];
    //    G4cout << "G4NeutronHPChannel constructed" << G4endl;
      G4NeutronHPCaptureFS * theFS = new G4NeutronHPCaptureFS; 
    //    OtherHPCaptureFS * theFS = new OtherHPCaptureFS; 
    GdNeutronHPCaptureFS * theGdFS = new GdNeutronHPCaptureFS;

    for (G4int i=0; i<numEle; i++)
    {
//      G4cout << "initializing theCapture " <<i<< " " << numEle<< G4endl;
      //    if((*(G4Element::GetElementTable()))[i]->GetName()!="Gadolinium")
      if((*(G4Element::GetElementTable()))[i]->GetZ() != 64)
	{
	  theCapture[i].Init((*(G4Element::GetElementTable()))[i], dirName);
         G4cout<<(*(G4Element::GetElementTable()))[i]->GetName()<< G4endl;
	 	 theCapture[i].Register(theFS);
	 //	 theCapture[i].Register(theGdFS);
	}
      else {
      theCapture[i].Init((*(G4Element::GetElementTable()))[i], dirName);
       G4cout<<(*(G4Element::GetElementTable()))[i]->GetName()<< G4endl;
        theCapture[i].Register(theGdFS);
	// theCapture[i].Register(theFS);
      }
    }
    delete theFS;
    delete theGdFS;
//    G4cout << "-------------------------------------------------" << G4endl;
//    G4cout << "Leaving GdNeutronHPCapture::GdNeutronHPCapture" << G4endl;
  }
  
  GdNeutronHPCapture::~GdNeutronHPCapture()
  {
    delete [] theCapture;
//    G4cout << "Leaving GdNeutronHPCapture::~GdNeutronHPCapture" << G4endl;
  }
  
  #include "G4NeutronHPThermalBoost.hh"
  G4HadFinalState * GdNeutronHPCapture::ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& )
  {
    if(getenv("NeutronHPCapture")) G4cout << " ####### GdNeutronHPCapture called" << G4endl;
    const G4Material * theMaterial = aTrack.GetMaterial();
    G4int n = theMaterial->GetNumberOfElements();
    G4int index = theMaterial->GetElement(0)->GetIndex();
    if(n!=1)
    {
      xSec = new G4double[n];
      G4double sum=0;
      G4int i;
      const G4double * NumAtomsPerVolume = theMaterial->GetVecNbOfAtomsPerVolume();
      G4double rWeight;    
      G4NeutronHPThermalBoost aThermalE;
      for (i=0; i<n; i++)
      {
        index = theMaterial->GetElement(i)->GetIndex();
        rWeight = NumAtomsPerVolume[i];
        xSec[i] = theCapture[index].GetXsec(aThermalE.GetThermalEnergy(aTrack,
  		                                                     theMaterial->GetElement(i),
  								     theMaterial->GetTemperature()));
        xSec[i] *= rWeight;
        sum+=xSec[i];
      }
      G4double random = G4UniformRand();
      G4double running = 0;
      for (i=0; i<n; i++)
      {
        running += xSec[i];
        index = theMaterial->GetElement(i)->GetIndex();
        if(random<=running/sum) break;
      }
      if(i==n) i=std::max(0, n-1);
      delete [] xSec;
    }
    return theCapture[index].ApplyYourself(aTrack);
  }
