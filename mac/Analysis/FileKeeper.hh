#ifndef FILEKEEPER_HH
#define FILEKEEPER_HH

#include <TFile.h>
#include <vector>
#include <iostream>

using std::vector;

/// Utility class for keeping TObjects to stuff into TFile
class FileKeeper {
public:
    FileKeeper(const std::string& fname): f(NULL) {
        f = new TFile(fname.c_str(),"RECREATE");
        std::cout << "Opened file " << fname << "\n";
    }
    ~FileKeeper() {
        assert(f);
        f->cd();
        for(int i=0; i<objs.size(); i++) objs[i]->Write();
        f->Close();
        delete f;
        //for(int i=0; i<objs.size(); i++) delete objs[i];
    }
    TObject* add(TObject* O) { objs.push_back(O); return O; }
    TFile* f;
    vector<TObject*> objs;
};

#endif
