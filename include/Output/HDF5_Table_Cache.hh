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
    HDF5_Table_Cache(const string& tname, const size_t* ofs, const size_t* szs, hsize_t nc = 1024):
    tablename(tname), offsets(ofs), sizes(szs), nchunk(nc)  { }
    
    /// get next table row; return whether successful or failed (end-of-file)
    bool next(T& val);
    /// (re)set read file
    void setFile(hid_t f);
    /// get number of rows read
    hsize_t getNRead() const { return nread; }
    /// get number of rows available
    hsize_t getNRows() const { return maxread; }
    /// get identifying number for value type
    static int64_t getIdentifier(const T& val);
    /// set identifying number for value type
    static void setIdentifier(T& val, int64_t id);
    
    /// load next "event" of entries with same identifer
    bool loadEvent();           
    vector<T> event_read;       ///< list of items in event
    
protected:
    string tablename;           ///< name of table to read
    const size_t* offsets;      ///< data type offsets
    const size_t* sizes;        ///< data type sizes
    hid_t infile_id = 0;        ///< file to read from
    T next_read;                ///< next item read in for event list reads
    
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
    HDF5_Table_Writer(const string& tname, const size_t* ofs, const size_t* szs, hsize_t nc = 1024):
    tablename(tname), offsets(ofs), sizes(szs), nchunk(nc)  { }
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

/// Combined HDF5 reader/writer for transferring select events subset
template<typename T>
class HDF5_Table_Transfer {
public:
    /// Constructor
    HDF5_Table_Transfer(const string& tname, const size_t* ofs, const size_t* szs, hsize_t nc = 1024);
    /// Transfer all entries with specified ID (assumed ascending), optionally re-numbering; return false at EOF.
    bool transferID(int64_t id, int64_t newID = -1);
    /// Transfer a (sorted ascending) list of ID-numbered rows
    bool transferIDs(const vector<int64_t>& ids, int64_t newID);
    
    string tablename;                   ///< name of table to transfer
    T row;                              ///< table row being transferred
    HDF5_Table_Cache<T> tableIn;        ///< input table
    HDF5_Table_Writer<T> tableOut;      ///< output table
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
    setIdentifier(next_read, -1);
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

template<typename T>
bool HDF5_Table_Cache<T>::loadEvent() {
    event_read.clear();
    int64_t current_evt = getIdentifier(next_read); // = -1 on the first and last time
    if(current_evt > 0) event_read.push_back(next_read);
    else if(getNRead()) return false; // file is exhausted
    
    while(true) {
        if(!next(next_read)) {
            setIdentifier(next_read, -1);
            break;
        }
        if(current_evt < 0) current_evt = getIdentifier(next_read);
        else if(getIdentifier(next_read) != current_evt) break;
        event_read.push_back(next_read);
    }
    return event_read.size();
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

template<typename T>
HDF5_Table_Transfer<T>::HDF5_Table_Transfer(const string& tname, const size_t* ofs, const size_t* szs, hsize_t nc):
tablename(tname), tableIn(tname,ofs,szs,nc), tableOut(tname,ofs,szs,nc) { }

template<typename T>
bool HDF5_Table_Transfer<T>::transferID(int64_t id, int64_t newID) {
    int64_t current_id;
    if(!tableIn.getNRead() && !tableIn.next(row)) return false;
    while((current_id = tableIn.getIdentifier(row)) <= id) {
        if(current_id == id) {
            if(newID >= 0) tableIn.setIdentifier(row, newID);
            tableOut.write(row);
        }
        if(!tableIn.next(row)) return false;
    }
    return true;
}

template<typename T>
bool HDF5_Table_Transfer<T>::transferIDs(const vector<int64_t>& ids, int64_t newID) {
    for(auto it = ids.begin(); it != ids.end(); it++) {
        if(!transferID(*it, newID)) return false;
        if(newID >= 0) newID++;
    }
    tableOut.flush();
    return true;
}

#endif
