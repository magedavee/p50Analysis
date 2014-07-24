// Unrestricted Use - Property of AECL
//
// InputSession.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Input Data Controller
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef InputSession_H			// Only carries out if object is not defined
#define InputSession_H 1		// Defines object

#include <iostream>			// Specifies the classes which allow reading/writing to standard input/output
#include <fstream>

#include "globals.hh"			// Specifies class defining all global parameters and variable types

/* -------- Class Definition --------- */

class InputSession : public std::ifstream	// This class inherits functions from std::ifstream
{
  public:	// Static Methods

    static InputSession* GetInputSessionPointer(G4String name = "");	// Singleton "Constructor"

  private:	// Static Member Data

    static InputSession* fInputSession;		// Static pointer to singleton object

  protected:	// Constructors

    InputSession(G4String);		// Constructor

  public:	// Destructors

    virtual ~InputSession();		// Destructor

  public:	// Accessible Methods

    G4String GetInputFileName() const;
    void SetInputFileName(G4String);
    void OpenFile();
    void CloseFile(G4bool reset = true);

  private:	// Member Data

    G4String filename;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
