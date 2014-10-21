#ifndef FILEKEEPER_HH
#define FILEKEEPER_HH

#include <TFile.h>
#include <vector>

using std::vector;

/// Utility class for keeping TObjects to stuff into TFile
class FileKeeper {
public:
    /// Constructor
    FileKeeper(const std::string& fname);
    /// Destructor
    ~FileKeeper();
    /// Add object to file list
    TObject* add(TObject* O) { objs.push_back(O); return O; }

protected:
    TFile* f;                   ///< output file
    vector<TObject*> objs;      ///< objects list
};

#endif
