#ifndef XMLINFO_HH
#define XMLINFO_HH

#include <TXMLEngine.h>
#include <TChain.h>

#include <string>
#include <vector>
#include <map>

/// Reader for PROSPECT-G4 XML metadata files
class XMLInfo {
public:
    /// Constructor
    XMLInfo(const std::string& fname);
    
    /// Get event generator "real" time
    double getGenTime();
    
protected:
    TXMLEngine E;       ///< ROOT XML reader engine
};

/// Loader for directory full of simulation runs
class OutDirLoader {
public:
    /// Constructor
    OutDirLoader(const std::string& basepath);
    
    /// get list of run numbers in directory
    std::vector<int> getRunlist() const;
    
    /// get list of run file names in directory
    std::vector<std::string> getFileList() const;
    
    /// make TChain from files in directory
    TChain* makeTChain() const;
    
    double getTotalGenTime();
    
protected:
    std::map<int, XMLInfo*> myInfo;     ///< XML files info
    const std::string bpath;            ///< base directory containing files
};

#endif
