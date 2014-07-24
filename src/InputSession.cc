// Unrestricted Use - Property of AECL
//
// InputSession.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Input Data Controller
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "InputSession.hh"		// Specifies the file which contains the class structure

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output
#include <iomanip>
#include <iostream>
#include <fstream>

#include "globals.hh"			// Specifies class defining all global parameters and variable types

InputSession* InputSession::fInputSession = 0;	// Instantiates static pointer to an object of this type - *** REQUIRED ***

	// ****** Singleton Constructor ****** //
InputSession* InputSession::GetInputSessionPointer(G4String name)	// This function acts as the constructor and only allow ONE object of this type to be created
{
  if(!fInputSession)
  {
    fInputSession = new InputSession(name);	// Calls the real constructor if object does not already exist
  }
  return fInputSession;
}

	// ****** Constructor ****** //
InputSession::InputSession(G4String name)
{
  filename = name;
}

	// ****** Destructor ****** //
InputSession::~InputSession()
{
  if(this->is_open())
  {
    this->close();
  }
  delete fInputSession;
}

	// ****** Return File Name ****** //
G4String InputSession::GetInputFileName() const
{
  return filename;
}

	// ****** Change File Name ****** //
void InputSession::SetInputFileName(G4String file)
{
  filename = file;		// Previous name is overwritten
}

	// ****** Open File ****** //
void InputSession::OpenFile()
{
  if(!filename.empty())		// If target file is specified
  {
	// Close stream object if it is already open
    if(this->is_open())
    {
      this->close();
    }

	// Checks if file exists
    std::ifstream incheck(filename,std::ifstream::in);
    if(!incheck.fail())
    {
	// Opens an instream for the specified file
      incheck.close();
      this->open(filename,std::ifstream::in);
    }
    else
    {
      G4cout << "*** ERROR: File could not be opened or file not found. ***" << G4endl;
    }
  }
  else
  {
    G4cout << "*** ERROR: File name not yet specified. No target file to open. ***" << G4endl;
  }
}

	// ****** Close File ****** //
void InputSession::CloseFile(G4bool reset)
{
	// Close and destroy memory of previous input file
  if(this->is_open());
  {
    this->close();
    this->clear();
  }

	// If 'reset' specified, retain previous input file name until manual override
  if(reset)
  {
    filename.clear();
  }
}

// EOF
