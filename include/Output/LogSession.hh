// Unrestricted Use - Property of AECL
//
// LogSession.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Output Data Controller
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef LogSession_H		// Only carries out if object is not defined
#define LogSession_H 1		// Defines object

#include <iostream>		// Specifies the classes which allow reading/writing to standard input/output
#include <fstream>

#include "globals.hh"		// Specifies class defining all global parameters and variable types

/* -------- Class Definition --------- */

class LogSession : public std::ofstream		// This class inherits functions from std::ofstream
{
  public:	// Static Methods

    static G4bool GetLogSessionStatus();	// Checks if singleton object is instantiated or not
    static LogSession* GetLogSessionPointer(G4String name = "", G4bool init = false);		// Singleton "Constructor"
    static G4String GetHomeDirectory() { return homedir; };

  private:	// Static Member Data

    static LogSession* fLogSession;		// Static pointer to singleton object
    static G4String homedir;			// Static variable for home directory

  protected:	// Constructors

    LogSession(G4String, G4bool);	// Constructor

  public:	// Destructors

    virtual ~LogSession();		// Destructor

  public:	// Accessible Methods

    void SetHomeDirectory(G4String);
    void SetOutputFileName(G4String);
    void OpenFile(G4bool first = true,G4bool keep = false);
    void CloseFile(G4bool reset = true);

    G4String GetOutputFileName() const { return filename; };
    G4String GetOutputFilePath() const;

  protected:	// Internal Methods

    void GenerateNewFile();
    void ReloadOldFile();

  private:	// Member Data

    G4String filename;
    G4String initname;
    G4String copyNo;
};

/* ----------------------------------- */

#endif				// End of the if clause

// EOF
