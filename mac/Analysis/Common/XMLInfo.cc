#include "XMLInfo.hh"
#include "strutils.hh"
#include "PathUtils.hh"
#include <cassert>
#include <iostream>

XMLInfo::XMLInfo(const string& fname) {
    myDoc = E.ParseFile(fname.c_str());
    docRoot = E.DocGetRootElement(myDoc);
    
    // Geant4 units
    unitconv["s"] = 1e9;
    unitconv["ms"] = 1e6;
    unitconv["us"] = 1e3;
    unitconv["ns"] = 1;
    unitconv["m"] = 1000;
    unitconv["cm"] = 10;
    unitconv["mm"] = 1;
    unitconv["fm"] = 1e-12;
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

double XMLInfo::getCalcTime() {
    return atof(E.GetAttr(docRoot,"t_end"))-atof(E.GetAttr(docRoot,"t_start"));
}

double XMLInfo::getSegments(int& nx, int& ny) {
    nx = ny = 0;
    XMLNodePointer_t gnode = findChildRecursive(docRoot,"ScintTank");
    if(!gnode) return 0;
    nx = atoi(E.GetAttr(gnode,"nSegX"));
    ny = atoi(E.GetAttr(gnode,"nSegY"));
    return fromUnits(E.GetAttr(gnode,"seg_size"));
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

XMLNodePointer_t XMLInfo::findChildRecursive(XMLNodePointer_t N, const string& nm) {
    if(!N) return NULL;
    XMLNodePointer_t nd = findChild(N,nm);
    if(nd) return nd;
    XMLNodePointer_t C = E.GetChild(N);
    while(C) {
        nd = findChildRecursive(C,nm);
        if(nd) return nd;
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
    map<string,double>::const_iterator it = unitconv.find(v[1]);
    assert(it != unitconv.end());
    if(it == unitconv.end()) return 0;
    return x*it->second;
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

OutDirLoader::OutDirLoader(const string& basepath, unsigned int limit): 
genTime(0), calcTime(0), segSize(0), nx(0), ny(0), bpath(basepath) {
    std::cout << "Loading directory '" << basepath << "'...\n";
    vector<string> flist = listdir(basepath);
    genTime = 0;
    for(auto it = flist.begin(); it != flist.end(); it++) {
        auto v = split(*it,".");
        if(v.size() >= 2 && v.back()=="xml") {
            auto vv = split(v[0],"_");
            if(vv.size() == 2) {
                XMLInfo* x = new XMLInfo(basepath+"/"+*it);
                genTime += x->getGenTime()*1e-9;
                calcTime += x->getCalcTime();
                if(!nx) segSize = x->getSegments(nx,ny);
                myInfo[atoi(vv[1].c_str())] = x;
            }
        }
        if(myInfo.size() >= limit) break;
    }
    std::cout << "\tTotal simulation time: " << genTime << " s calculated in " << calcTime/3600 << " h\n";
}

vector<int> OutDirLoader::getRunlist() const {
    vector<int> v;
    for(map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        v.push_back(it->first);
    return v;
}

vector<string> OutDirLoader::getFileList() const {
    vector<string> v;
    for(map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
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

bool OutDirLoader::isAdjacent(int v1, int v2) const {
    int x1,y1,x2,y2;
    volToXY(v1,x1,y1);
    volToXY(v2,x2,y2);
    return (abs(x1-x2) == 1 && y1==y2) || (x1==x2 && abs(y1-y2) == 1);
}
