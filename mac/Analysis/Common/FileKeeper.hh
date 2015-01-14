/// \file FileKeeper.hh \brief Utility wrapper for memory-managing TObjects stuffed into a TFile.
#ifndef FILEKEEPER_HH
#define FILEKEEPER_HH

#include <TFile.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

/// Utility class for keeping TObjects to stuff into TFile
class FileKeeper {
public:
    /// Constructor
    FileKeeper(const string& fname);
    /// Destructor
    ~FileKeeper();
    /// Add object to file list
    TObject* add(TObject* O) { objs.push_back(O); return O; }

protected:
    TFile* f;                   ///< output file
    vector<TObject*> objs;      ///< objects list
};

#endif
