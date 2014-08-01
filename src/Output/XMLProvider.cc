#include "XMLProvider.hh"

#include <cassert>
#include <G4ios.hh>

void XMLProvider::genNode(TXMLEngine& E, XMLNodePointer_t parent) {    
    myNode = E.NewChild(parent, NULL, nodeName.c_str());
    assert(myNode);
    fillNode(E);
    for(std::vector<XMLProvider*>::iterator it = children.begin(); it != children.end(); it++) {
        assert(*it);
        (*it)->genNode(E, myNode);
    }
}

void XMLProvider::writeToFile(const std::string& fname) {
    G4cerr << "Writing XML output '" << fname << "'" << G4endl;
    
    TXMLEngine E;
    genNode(E, NULL);
    XMLDocPointer_t xmldoc = E.NewDoc();
    E.DocSetRootElement(xmldoc, myNode);
    E.SaveDoc(xmldoc, fname.c_str());
}
