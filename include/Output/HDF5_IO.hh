/// \file HDF5_IO.hh \brief HDF5 format IO classes.
#ifndef HDF5_IO_HH
#define HDF5_IO_HH

#ifdef WITH_HDF5

#include "strutils.hh"
#include "FileIO.hh"

#include "hdf5.h"
#include "hdf5_hl.h"

class HDF5_IO: public FileIO {
public:
    /// Constructor
    HDF5_IO();
    
    /// Destructor
    virtual ~HDF5_IO();
    
    /// record scintillator ionization
    virtual void addScIoniBranch();
    
    /// Open named output file
    virtual void SetFileName(G4String filename);
    /// Add current Event to output file
    virtual void SaveEvent();
    /// Finalize/close file output
    virtual void WriteFile();
    
protected:
    
    hid_t outfile_id = 0;       ///< output HDF5 file ID
    hid_t vec3_tid = 0;         ///< type ID for double[3]
};

#endif

#endif
