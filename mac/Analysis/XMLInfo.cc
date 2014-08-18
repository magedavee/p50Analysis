#include "XMLInfo.hh"
#include "strutils.hh"

XMLInfo::XMLInfo(const std::string& fname) { }

double XMLInfo::getGenTime() { return 1.234; }


OutDirLoader::OutDirLoader(const std::string& basepath): bpath(basepath) { }

std::vector<int> OutDirLoader::getRunlist() const {
    std::vector<int> v;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        v.push_back(it->first);
    return v;
}

std::vector<std::string> OutDirLoader::getFileList() const {
    std::vector<std::string> v;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        v.push_back(bpath+"/Run_"+to_str(it->first)+".root");
    return v;
}

TChain* OutDirLoader::makeTChain() const {
    TChain* T = new TChain("sblmc");
    T->SetDirectory(NULL);
    std::vector<std::string> v = getFileList();
    for(std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); it++)
        T->Add(it->c_str());
    return T;
}

double OutDirLoader::getTotalGenTime() {
    double t = 0;
    for(std::map<int, XMLInfo*>::const_iterator it = myInfo.begin(); it != myInfo.end(); it++)
        it->second->getGenTime();
    return t;
}
