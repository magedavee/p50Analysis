#include "XMLInfo.hh"
#include "strutils.hh"
#include "PathUtils.hh"
#include <cassert>
#include <iostream>

XMLInfo::XMLInfo(const string& fname) {
    myDoc = E.ParseFile(fname.c_str());
    docRoot = E.DocGetRootElement(myDoc);
    
    // cgs units
    unitconv["s"] = 1.;
    unitconv["ms"] = 1e-3;
    unitconv["us"] = 1e-6;
    unitconv["ns"] = 1e-9;
    unitconv["m"] = 100.;
    unitconv["cm"] = 1.;
    unitconv["mm"] = 0.1;
}

XMLInfo::~XMLInfo() {
    E.FreeDoc(myDoc);
}

double XMLInfo::getGenTime() {
    XMLNodePointer_t gnode = findChild(docRoot,"PrimaryGenerator");
    assert(gnode);
    if(!gnode) return 0;
    return fromUnits(E.GetAttr(gnode,"time"));
}

XMLNodePointer_t XMLInfo::findChild(XMLNodePointer_t N, const string& nm) {
    if(!N) return NULL;
    //std::cout << "Searching for child node '" << nm << "'...\n";
    XMLNodePointer_t C = E.GetChild(N);
    while(C) {
        //std::cout << "\t" <<  E.GetNodeName(C) << "\n";
        if(nm == E.GetNodeName(C)) return C;
        C = E.GetNext(C);
    }
    return NULL;
}

double XMLInfo::fromUnits(const string& s) const {
    //std::cout << "Converting '" << s << "' to double...\n";
    vector<string> v = split(s," ");
    assert(v.size()==2);
    if(!v.size()) return 0;
    double x = atof(v[0].c_str());
    if(v.size()<2) return x;
    std::map<string,double>::const_iterator it = unitconv.find(v[1]);
    assert(it != unitconv.end());
    if(it == unitconv.end()) return 0;
    return x*it->second;
}

OutDirLoader::OutDirLoader(const string& basepath): bpath(basepath) {
    std::cout << "Loading directory '" << basepath << "'...\n";
    vector<string> flist = listdir(basepath);
    for(auto it = flist.begin(); it != flist.end(); it++) {
        auto v = split(*it,".");
        if(v.size() >= 2 && v.back()=="xml") {
            auto vv = split(v[0],"_");
            if(vv.size() == 2)
                myInfo[atoi(vv[1].c_str())] = new XMLInfo(basepath+"/"+*it);
        }
    }
    std::cout << "\tTotal simulation time: " << getTotalGenTime() << " s\n";
}

vector<int> OutDirLoader::getRunlist() const {
    vector<int> v;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        v.push_back(it->first);
    return v;
}

vector<string> OutDirLoader::getFileList() const {
    vector<string> v;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        v.push_back(bpath+"/Run_"+to_str(it->first)+".root");
    return v;
}

TChain* OutDirLoader::makeTChain() const {
    TChain* T = new TChain("PG4");
    T->SetDirectory(NULL);
    vector<string> v = getFileList();
    for(vector<string>::const_iterator it = v.begin(); it != v.end(); it++)
        T->Add(it->c_str());
    return T;
}

double OutDirLoader::getTotalGenTime() {
    double t = 0;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        t += it->second->getGenTime();
    return t;
}
