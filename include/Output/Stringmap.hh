#ifndef STRINGMAP_HH
#define STRINGMAP_HH

#include <map>
#include <vector>
#include <string>

using std::multimap;
using std::vector;
using std::string;

/// wrapper for multimap<string,string> with useful functions
class Stringmap {
public:
    
    /// constructor
    Stringmap(const string& str = "");
    /// copy constructor from another Stringmap
    Stringmap(const Stringmap& m);
    /// destructor
    virtual ~Stringmap() {}
    
    
    /// insert key/(string)value pair
    void insert(const string& str, const string& v);
    /// insert key/(double)value
    void insert(const string& str, double d);
    /// retrieve key values
    vector<string> retrieve(const string& str) const;
    /// get first key value (string) or default
    string getDefault(const string& str, const string& d) const;
    /// return number of elements
    unsigned int size() const { return dat.size(); }
    /// return count of entries with key
    unsigned int count(const string& str) const { return dat.count(str); }
    /// serialize to a string
    string toString() const;
    
    /// get first key value (double) or default
    double getDefault(const string& str, double d) const;
    /// retrieve key values as doubles
    vector<double> retrieveDouble(const string& str) const;
    /// remove a key
    void erase(const string& str);
    
    /// display to screen
    void display(string linepfx = "") const;
    
    /// merge data from another stringmap
    void operator+=(const Stringmap& S) { S.mergeInto(*this); }
    
    multimap<string, string> dat;       ///< key-value multimap
    
protected:
    
    /// merge data into another stringmap
    void mergeInto(Stringmap& S) const;
};

/// wrapper for multimap<std::string,Stringmap> with useful functions
class QFile {
public:
    
    /// constructor given a string
    QFile(const std::string& s = "", bool readit = true);
    
    /// insert key/(string)value pair
    void insert(const std::string& str, const Stringmap& v);
    /// remove a key
    void erase(const std::string& str);
    /// retrieve values for key
    vector<Stringmap> retrieve(const std::string& s) const;
    /// retrieve first value for key
    Stringmap getFirst(const std::string& str, const Stringmap& dflt = Stringmap()) const;
    /// retrieve all sub-key values
    vector<string> retrieve(const std::string& k1, const std::string& k2) const;
    /// retreive sub-key with default
    string getDefault(const std::string& k1, const std::string& k2, const std::string& d) const;
    /// retrieve sub-key as double with default
    double getDefault(const std::string& k1, const std::string& k2, double d) const;
    /// retrieve all sub-key values as doubles
    vector<double> retrieveDouble(const std::string& k1, const std::string& k2) const;	
    /// return number of elements
    unsigned int size() const { return dat.size(); }
    /// transfer all data for given key from other QFile
    void transfer(const QFile& Q, const std::string& k);
    
    /// set output file location
    void setOutfile(string fnm) { name = fnm; }
    /// commit data to file
    void commit(string outname = "") const;
    /// display to stdout
    void display() const;
    
protected:
    
    string name;                        ///< name for this object
    multimap<string, Stringmap> dat;    ///< key-value multimap
    
};

#endif
