/// \file Common/strutils.hh \brief String manipulation utilities.
#ifndef STRUTILS_HH
#define STRUTILS_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <map>

using std::string;
using std::vector;
using std::map;
using std::ostream;

/// display a map<int,int>
ostream& operator<<(ostream& os, const map<int,int>& mp);

/// display a map<T,U>
template<typename T, typename U>
void display_map(const map<T,U>& mp, double norm = -1, double thresh = 0) {
    U total = 0;
    for(typename map<T,U>::const_iterator it = mp.begin(); it != mp.end(); it++) {
        if(!norm) std::cout << it->first << ":\t" << it->second << "\n";
        total += it->second;       
    }
    std::cout << "\n--- Total:\t" << total/norm << " (" << total << ")---\n";
    if(norm) {
        if(norm == -1) norm = total;
        double others = 0;
        for(typename map<T,U>::const_iterator it = mp.begin(); it != mp.end(); it++) {
            double f = it->second/norm;
            if(f>=thresh) std::cout << it->first << ":\t" << f << " (" << it->second << ")\n";
            else others += f;
        }
        if(others) std::cout << "others: " << others << "\n";
    }
}

/// utility function for converting to string
template<typename T>
string to_str(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

/// small integer to roman numerals string
string itosRN(int i);

/// convert a vector of doubles to a string list
string vtos(const double* st, const double* en, string sep = ",");
/// convert a vector of doubles to a string list
string vtos(const vector<double>& ds,string sep = ",");
/// convert a vector of floats to a string list
string vtos(const float* st, const float* en, string sep = ",");
/// convert a vector of doubles to a string list
string vtos(const vector<float>& ds,string sep = ",");
/// convert a vector of ints to a string list
string vtos(const int* st, const int* en, string sep = ",");
/// convert a vector of ints to a string list
string vtos(const vector<int>& ds,string sep = ",") ;

/// convert a char to a string
string ctos(char c);
/// convert a string to lowercase
string lower(string st);
/// convert a string to uppercase
string upper(string st);
/// replace all of one character in a string with another
string replace(string str, char o, char n);
/// check whether string a begins with string b
bool startsWith(const string& a, const string& b);
/// split a string into substrings on given split characters
vector<string> split(const string& str, const string splitchars = " \t\r\n");
/// join a list of strings into a single string
string join(const vector<string>& ss, const string& sep = " ");
/// strip junk chars off start and end of string
string strip(const string& str, const string stripchars = " \t\r\n");
/// split a string into a vector of doubles
vector<double> sToDoubles(const string& str, const string splitchars = ", \t\r\n");
/// split a string into a vector of floats
vector<float> sToFloats(const string& str, const string splitchars = ", \t\r\n");
/// split a string into a vector of ints
vector<int> sToInts(const string& str, const string splitchars = ", \t\r\n");
/// read in an array from a file
vector< vector<float> > readArray(std::ifstream& fin, unsigned int minitems = 1, const string splitchars = ", \t\r\n");

#endif
