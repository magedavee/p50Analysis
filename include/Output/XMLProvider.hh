#ifndef XMLPROVIDER_HH
/// Assure this header file is loaded only once
#define XMLPROVIDER_HH

#include <TXMLEngine.h>
#include <vector>
#include <string>

/// Base class for objects providing XML output via TXMLEngine
class XMLProvider {
public:
    /// Constructor
    XMLProvider(const std::string& n): nodeName(n), myNode(NULL) { }
    /// Destructor TODO: memory management for class!
    virtual ~XMLProvider() { }
    
    /// Write to file
    void writeToFile(const std::string& fname);
    
protected:
    /// Append child node provider to list
    void addChild(XMLProvider* p) { children.push_back(p); }
    /// Add string-valued attribute to node
    void addAttr(TXMLEngine& E, const std::string& nm, const std::string& val) { E.NewAttr(myNode, NULL, nm.c_str(), val.c_str()); }
    /// Add int-valued attribute to node
    void addAttr(TXMLEngine& E, const std::string& nm, int i) { E.NewIntAttr(myNode, nm.c_str(), i); }
    /// Subclass me to fill in node data!
    virtual void fillNode(TXMLEngine&) { }
        
    /// Generate node into parent node
    void genNode(TXMLEngine& E, XMLNodePointer_t parent);
    
    std::string nodeName;               ///< name for this object's node
    std::vector<XMLProvider*> children; ///< objects providing child nodes
    XMLNodePointer_t myNode;            ///< node produced by this object
};

#endif
