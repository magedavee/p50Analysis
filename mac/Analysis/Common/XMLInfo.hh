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
    
    /// Get event generator "real" time, Geant4 units [ns]
    double getGenTime();
    /// Get simulation wall clock running time [s]
    double getCalcTime();
    /// get scintillator segmentation; returns segment size
    double getSegments(int& nx, int& ny);
    
protected:
    TXMLEngine E;               ///< ROOT XML reader engine
    XMLDocPointer_t myDoc;      ///< read-in document
    XMLNodePointer_t docRoot;   ///< root node of document
    
    /// find named child node of given node
    XMLNodePointer_t findChild(XMLNodePointer_t N, const std::string& nm);
    /// recursively search whole tree for first such named node
    XMLNodePointer_t findChildRecursive(XMLNodePointer_t N, const std::string& nm);
    
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

    /// get representative XMLInfo file
    XMLInfo* getXML() { return myInfo.size()? myInfo.begin()->second : NULL; }
    
    /// convert volume number to x,y segment number
    void volToXY(int v, int& x, int& y) const { if(v < 0) x = y = -1; else { x = v%nx; y = v/nx; } }
    /// check if segments are (non-diagonal) adjacent
    bool isAdjacent(int v1, int v2) const;
    
    double genTime;     ///< total event generator "real" time, [s]
    double calcTime;    ///< total calculation time [s]
    double segSize;     ///< segment short-dimension size [mm]
    int nx;             ///< scintillator x segmentation
    int ny;             ///< scintillator y segmentation
    
protected:
    std::map<int, XMLInfo*> myInfo;     ///< XML files info
    const std::string bpath;            ///< base directory containing files
};

#endif
