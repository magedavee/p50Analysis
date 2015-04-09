#include "XMLProvider.hh"
#include "strutils.hh"

#include "SMExcept.hh"
#include <cassert>
#include <G4ios.hh>

void XMLProvider::genNode(TXMLEngine& E, XMLNodePointer_t parent) {
    if(!nodeName.size()) nodeName = "???";
    myNode = E.NewChild(parent, NULL, nodeName.c_str());
    smassert(myNode);
    fillNode(E);
    for(auto it = xattrs.begin(); it != xattrs.end(); it++) {
        addAttr(E, it->first, it->second);
    }
    for(auto it = children.begin(); it != children.end(); it++) {
        smassert(*it);
        (*it)->genNode(E, myNode);
    }
    //if(content.size())
    //    E.AddNodeContent(myNode, content.c_str(), content.size());
}

void XMLProvider::writeToFile(const string& fname) {
    G4cout << "Writing XML output '" << fname << "'" << G4endl;
    
    TXMLEngine E;
    genNode(E, NULL);
    XMLDocPointer_t xmldoc = E.NewDoc();
    E.DocSetRootElement(xmldoc, myNode);
    E.SaveDoc(xmldoc, fname.c_str());
}

void XMLProvider::addAttr(TXMLEngine& E, const string& n, double v) { addAttr(E, n, to_str(v)); }

void XMLProvider::addAttr(TXMLEngine& E, const string& n, const string& val) { E.NewAttr(myNode, NULL, n.c_str(), strip(val).c_str()); }
