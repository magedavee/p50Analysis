#include <sstream>
#include <cassert>

#include "FileIO.hh"
#include "RootIO.hh"
#include "HDF5_IO.hh"
#include <G4ios.hh>

static FileIO* instance = NULL;

void FileIO::Clear() {
    for(auto it = subObjs.begin(); it != subObjs.end(); it++)
        (*it)->Clear();
}

FileIO* FileIO::GetInstance() {
    if (instance == NULL) {
#ifdef WITH_HDF5
        G4cout << "Instantiating HDF5 output instance" << G4endl;
        instance = new HDF5_IO();
#else
        G4cout << "Instantiating ROOT output instance" << G4endl;
        instance = new RootIO();
#endif
    }
    return instance;
}
