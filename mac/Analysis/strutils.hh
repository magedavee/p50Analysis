#ifndef STRUTILS_HH
#define STRUTILS_HH

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

using std::string;

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
string vtos(const std::vector<double>& ds,string sep = ",");
/// convert a vector of floats to a string list
string vtos(const float* st, const float* en, string sep = ",");
/// convert a vector of doubles to a string list
string vtos(const std::vector<float>& ds,string sep = ",");
/// convert a vector of ints to a string list
string vtos(const int* st, const int* en, string sep = ",");
/// convert a vector of ints to a string list
string vtos(const std::vector<int>& ds,string sep = ",") ;

/// convert a char to a string
string ctos(char c);
/// convert a string to lowercase
string lower(string s);
/// convert a string to uppercase
string upper(string s);
/// replace all of one character in a string with another
string replace(string str, char o, char n);
/// check whether string a begins with string b
bool startsWith(const string& a, const string& b);
/// split a string into substrings on given split characters
std::vector<string> split(const string& str, const string splitchars = " \t\r\n");
/// join a list of strings into a single string
string join(const std::vector<string>& ss, const string& sep = " ");
/// strip junk chars off start and end of string
string strip(const string& str, const string stripchars = " \t\r\n");
/// split a string into a vector of doubles
std::vector<double> sToDoubles(const string& str, const string splitchars = ", \t\r\n");
/// split a string into a vector of floats
std::vector<float> sToFloats(const string& str, const string splitchars = ", \t\r\n");
/// split a string into a vector of ints
std::vector<int> sToInts(const string& str, const string splitchars = ", \t\r\n");
/// read in an array from a file
std::vector< std::vector<float> > readArray(std::ifstream& fin, unsigned int minitems = 1, const string splitchars = ", \t\r\n");

#endif
