// Unrestricted Use - Property of AECL
//
// LogSession.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Output Data Controller
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "LogSession.hh"

#include "G4ios.hh"			// Includes classes which allow reading/writing from standard input/output
#include <iomanip>
#include <iostream>
#include <fstream>
#include "globals.hh"

LogSession* LogSession::fLogSession = 0;

G4String LogSession::homedir = "";

	// ****** Singleton Status ****** //
G4bool LogSession::GetLogSessionStatus()
{
  if(!fLogSession)
  {
    return false;
  }
  else
  {
    return true;
  }
}

	// ****** Singleton Costructor ****** //
LogSession* LogSession::GetLogSessionPointer(G4String name, G4bool init)
{
  if(!fLogSession)
  {
    fLogSession = new LogSession(name,init);
  }
  return fLogSession;
}

	// ****** Constructor ****** //
LogSession::LogSession(G4String name, G4bool init)
{
  if(homedir.empty())
  {
	// Specifies home directory based on operating system of computer
#if defined __linux || defined __unix
    homedir = getenv("HOME");
    homedir += "/Desktop/";
#elif defined __WIN32
    homedir = getenv("USERPROFILE");
    homedir += "\\Desktop\\";
#else
    homedir = "/";
#endif
  }
  if(!name.empty() && name.at(0) == '/')
  {
    name.erase(0,name.find_first_not_of('/'));
  }
  if(init && initname.empty() && !name.empty())
  {
    initname = name;
  }
  if(name.empty())
  {
    name = "blagoblag.txt";	// Default file name - perhaps an unnecessary precaution
  }
  filename = name;
  copyNo = "";
}

	// ****** Destructor ****** //
LogSession::~LogSession()
{
  if(this->is_open())
  {
    this->close();
  }
  delete fLogSession;
}

	// ****** Change Home Directory ****** //
void LogSession::SetHomeDirectory(G4String home)
{
  if(home.at(0) == '~') { home.replace(0,1,getenv("HOME")); }
  if(home.at(0) != '/') { home.insert(0,"/"); }
  if(home.at(home.length()-1) != '/') { home.append("/"); }
  homedir = home;
  G4cout << "Home directory for all output files set to " << homedir << G4endl;
}

	// ****** Change File Name ****** //
void LogSession::SetOutputFileName(G4String file)
{
  if(file.at(0) == '/')
  {
	// Removes initial slash
    file.erase(0,file.find_first_not_of('/'));
  }
  filename = file;
}

	// ****** Return File Name ****** //
G4String LogSession::GetOutputFilePath() const
{
  return (filename + copyNo);
}

	// ****** Open File ****** //
void LogSession::OpenFile(G4bool first,G4bool keep)
{
  if(first) { GenerateNewFile(); }
  else { ReloadOldFile(); }
  if(!filename.empty())
  {
	// Close stream object if it is already open
    if(this->is_open())
    {
      this->close();
    }

	// Specify full file path for output file
    G4String theName = homedir + filename + copyNo;

	// Specifies append if 'keep' is requested, truncate otherwise
    if(keep) { this->open(theName,std::ios_base::app); }
    else     { this->open(theName,std::ios_base::trunc); }
  }
  else if(!initname.empty())	// Use name provided at construction
  {
	// Close stream object if it is already open
    if(this->is_open())
    {
      this->close();
    }

	// Specify full file path for output file
    G4String theName = homedir + initname + copyNo;

	// Specifies append if 'keep' is requested, truncate otherwise
    if(keep) { this->open(theName,std::ios_base::app); }
    else     { this->open(theName,std::ios_base::trunc); }
  }
  else
  {
    G4cout << "*** ERROR: File name not yet specified. Output file not created. ***" << G4endl;
  }
}

	// ****** Close File ****** //
void LogSession::CloseFile(G4bool reset)
{
	// Close stream object if it is open
  if(this->is_open());
  {
    this->close();
  }

	// If 'reset' is specified, destroy memory of previous output file
  if(reset)
  {
    if(initname.empty()) { fLogSession = 0; }
    else { filename.clear(); copyNo.clear(); }
  }
}

	// ****** Generate Unique File Label ****** //
void LogSession::GenerateNewFile()
{
  G4long identifier = 0;
  G4bool fileExists = true;
  while(fileExists)
  {
	// Generate a new label
    std::stringstream fileID;
    if(identifier > 0) { fileID << "(" << identifier << ")"; }
    else               { fileID << ""; }

	// Propose new full output file path using label
    G4String theFile = "";
    if(!filename.empty()) { theFile = homedir + filename + fileID.str(); }
    else if(!initname.empty()) { theFile = homedir + initname + fileID.str(); }

	// Check if proposed file path already exists by opening an input stream
    std::ifstream filecheck(theFile);
    fileExists = !filecheck.fail();

	// If it does already exist, increment label counter and repeat
    if(fileExists) { identifier++; }
  }

	// Successful label made permanent as the variable copyNo
  std::stringstream numCopy;
  if(identifier < 1) { numCopy << ""; }
  else               { numCopy << "(" << identifier << ")"; }
  copyNo = numCopy.str();
}

	// ****** Recall Last File Label ****** //
void LogSession::ReloadOldFile()
{
  G4long identifier = 0;
  G4bool fileExists = true;
  while(fileExists)
  {
	// Generate a new label
    std::stringstream fileID;
    if(identifier > 0) { fileID << "(" << identifier << ")"; }
    else               { fileID << ""; }

	// Propose new full output file path using label
    G4String theFile = "";
    if(!filename.empty()) { theFile = homedir + filename + fileID.str(); }
    else if(!initname.empty()) { theFile = homedir + initname + fileID.str(); }

	// Check if proposed file path already exists by opening an input stream
    std::ifstream filecheck(theFile);
    fileExists = !filecheck.fail();

	// If it does already exist, increment label coutner and repeat
    if(fileExists) { identifier++; }
	// If not, return label counter to last existing label and end loop
    else           { identifier--; }
  }

	// Successful label made permanent as the variable copyNo
  std::stringstream numCopy;
  if(identifier < 1) { numCopy << ""; }
  else               { numCopy << "(" << identifier << ")"; }
  copyNo = numCopy.str();
}

// EOF
