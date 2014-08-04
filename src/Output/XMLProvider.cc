#include "XMLProvider.hh"

#include <cassert>
#include <G4ios.hh>

void XMLProvider::genNode(TXMLEngine& E, XMLNodePointer_t parent) {    
    myNode = E.NewChild(parent, NULL, nodeName.c_str());
    assert(myNode);
    fillNode(E);
    for(vector< pair<string, string> >::iterator it = xattrs.begin(); it != xattrs.end(); it++)
        addAttr(E, it->first, it->second);
    for(vector<XMLProvider*>::iterator it = children.begin(); it != children.end(); it++) {
        assert(*it);
        (*it)->genNode(E, myNode);
    }
    //if(content.size())
    //    E.AddNodeContent(myNode, content.c_str());
}

void XMLProvider::writeToFile(const string& fname) {
    G4cerr << "Writing XML output '" << fname << "'" << G4endl;
    
    TXMLEngine E;
    genNode(E, NULL);
    XMLDocPointer_t xmldoc = E.NewDoc();
    E.DocSetRootElement(xmldoc, myNode);
    E.SaveDoc(xmldoc, fname.c_str());
}
