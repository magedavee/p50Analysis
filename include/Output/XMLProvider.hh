#ifndef XMLPROVIDER_HH
/// Assure this header file is loaded only once
#define XMLPROVIDER_HH

#include <TXMLEngine.h>
#include <vector>
#include <string>
#include <utility>

using std::string;
using std::pair;
using std::vector;

/// Base class for objects providing XML output via TXMLEngine
class XMLProvider {
public:
    /// Constructor
    XMLProvider(const string& n): nodeName(n), myNode(NULL) { }
    /// Destructor TODO: memory management for class!
    virtual ~XMLProvider() { }
    
    // TODO: TXMLEngine::AddNodeContent is in documentation, but missing from my ROOT 5.34/18 install's headers ?!
    string content;     ///< (non-XML) contents of node
    
    /// Add attribute
    void addAttr(const string& key, const string& val) { xattrs.push_back(pair<string, string>(key,val)); }
    /// Write to file
    void writeToFile(const string& fname);
    
protected:
    /// Append child node provider to list
    void addChild(XMLProvider* p) { children.push_back(p); }
    /// Add string-valued attribute to node
    void addAttr(TXMLEngine& E, const string& nm, const string& val);
    /// Add double-valued attribute to node
    void addAttr(TXMLEngine& E, const string& nm, double v);
    /// Add int-valued attribute to node
    void addAttrI(TXMLEngine& E, const string& nm, int i) { E.NewIntAttr(myNode, nm.c_str(), i); }
    /// Subclass me to fill in node data!
    virtual void fillNode(TXMLEngine&) { }
        
    /// Generate node into parent node
    void genNode(TXMLEngine& E, XMLNodePointer_t parent);
    
    string nodeName;                            ///< name for this object's node
    vector<XMLProvider*> children;              ///< objects providing child nodes
    XMLNodePointer_t myNode;                    ///< node produced by this object
    vector< pair<string, string> > xattrs;      ///< additional attributes to include
};

#endif
