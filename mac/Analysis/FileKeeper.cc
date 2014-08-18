#include "FileKeeper.hh"

#include <iostream>
#include <cassert>

FileKeeper::FileKeeper(const std::string& fname): f(NULL) {
    f = new TFile(fname.c_str(),"RECREATE");
    std::cout << "Opened file " << fname << "\n";
}

FileKeeper::~FileKeeper() {
    assert(f);
    f->cd();
    for(size_t i=0; i<objs.size(); i++) objs[i]->Write();
    f->Close();
    delete f;
    //for(int i=0; i<objs.size(); i++) delete objs[i];
}
