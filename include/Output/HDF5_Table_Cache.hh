/// \file HDF5_Table_Cache.hh \brief Template utility class for memory-cache buffered HDF5 tables IO
#ifndef HDF5_TABLE_CACHE_HH
#define HDF5_TABLE_CACHE_HH

#include "hdf5.h"
#include "hdf5_hl.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <cassert>

/// Cacheing HDF5 table reader
template<typename T>
class HDF5_Table_Cache {
public:
    /// Constructor, from name of table and struct offsets/sizes
    HDF5_Table_Cache(const string& nm, const size_t* ofs, const size_t* szs, hsize_t nc = 1024):
    tablename(nm), offsets(ofs), sizes(szs), nchunk(nc)  { }
    
    /// get next table row; return whether successful or failed (end-of-file)
    bool next(T& val);
    /// (re)set read file
    void setFile(hid_t f);
    /// get number of rows read
    hsize_t getNRead() const { return nread; }
    
protected:
    string tablename;           ///< name of table to read
    const size_t* offsets;      ///< data type offsets
    const size_t* sizes;        ///< data type sizes
    hid_t infile_id = 0;        ///< file to read from
    
    vector<T> cached;           ///< cached read data
    size_t cache_idx = 0;       ///< index in cached data
    hsize_t nread = 0;          ///< number of rows read
    hsize_t maxread = 0;        ///< number of rows in table
    hsize_t nfields = 0;        ///< number of fields in table
    hsize_t nchunk;             ///< cacheing chunk size
};

/// Cacheing HDF5 table writer
template<typename T>
class HDF5_Table_Writer {
public:
    /// Constructor, from name of table and struct offsets/sizes
    HDF5_Table_Writer(const string& nm, const size_t* ofs, const size_t* szs, hsize_t nc = 1024):
    tablename(nm), offsets(ofs), sizes(szs), nchunk(nc)  { }
    /// Destructor
    ~HDF5_Table_Writer() { flush(); }
    
    /// write table row
    void write(const T& val);
    /// write table rows
    void write(const vector<T>& vals);
    /// (re)set output file
    void setFile(hid_t f);
    /// flush to disk
    void flush();
    
protected:
    string tablename;           ///< name of table to read
    const size_t* offsets;      ///< data type offsets
    const size_t* sizes;        ///< data type sizes
    hid_t outfile_id = 0;       ///< file to read from
    
    vector<T> cached;           ///< cached output data
    hsize_t nchunk;             ///< cacheing chunk size
};

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

template<typename T>
void HDF5_Table_Writer<T>::write(const vector<T>& vals) {
    cached.insert(cached.end(), vals.begin(), vals.end());
    if(cached.size() >= nchunk) flush();
}

template<typename T>
void HDF5_Table_Writer<T>::write(const T& val) {
    cached.push_back(val);
    if(cached.size() >= nchunk) flush();
}

template<typename T>
void HDF5_Table_Writer<T>::setFile(hid_t f) {
    flush();
    outfile_id = f;
}

template<typename T>
void HDF5_Table_Writer<T>::flush() {
    if(outfile_id && cached.size()) {
        herr_t err = H5TBappend_records(outfile_id, tablename.c_str(), cached.size(), sizeof(T), offsets, sizes, cached.data());
        assert(err >= 0);
    }
    cached.clear();
}
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

template<typename T>
void HDF5_Table_Cache<T>::setFile(hid_t f) {
    infile_id = f;
    cached.clear();
    cache_idx = nread = maxread = 0;
    if(f && H5Lexists(infile_id, tablename.c_str(), H5P_DEFAULT)) {
        herr_t err = H5TBget_table_info(infile_id, tablename.c_str(), &nfields, &maxread);
        assert(err >= 0);
    }
}

template<typename T>
bool HDF5_Table_Cache<T>::next(T& val) {
    if(!infile_id) return false;
    if(cache_idx == cached.size()) {
        hsize_t nToRead = std::min(nchunk, maxread-nread);
        cached.resize(nToRead);
        cache_idx = 0;
        herr_t err = H5TBread_records(infile_id, tablename.c_str(), nread, nToRead, sizeof(T),  offsets, sizes, cached.data());
        assert(err >= 0);
        nread += nToRead;
    }
    if(cache_idx >= cached.size()) return false;
    val = cached[cache_idx++];
    return true;
}

#endif
