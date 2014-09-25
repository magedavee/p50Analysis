#ifndef XMLINFO_HH
#define XMLINFO_HH

#include <TXMLEngine.h>
#include <TChain.h>

#include <climits>
#include <string>
#include <vector>
#include <map>

/// Reader for PROSPECT-G4 XML metadata files
class XMLInfo {
public:
    /// Constructor
    XMLInfo(const std::string& fname);
    /// Destructor
    ~XMLInfo();
    
    /// Get event generator "real" time
    double getGenTime();
    
protected:
    TXMLEngine E;               ///< ROOT XML reader engine
    XMLDocPointer_t myDoc;      ///< read-in document
    XMLNodePointer_t docRoot;   ///< root node of document
    
    /// find named child node of given node
    XMLNodePointer_t findChild(XMLNodePointer_t N, const std::string& nm);
    
    /// parse Geant4 string with units
    double fromUnits(const std::string& s) const;
    std::map<std::string, double> unitconv;     ///< Geant4 unit conversions 
};

/// Loader for directory full of simulation runs
class OutDirLoader {
public:
    /// Constructor
    OutDirLoader(const std::string& basepath, unsigned int limit = INT_MAX);
    
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
