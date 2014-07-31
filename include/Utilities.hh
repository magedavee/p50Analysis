#ifndef UTILITIES_HH
/// Assure this header is loaded only once
#define UTILITIES_HH

#include <string>
#include <sstream>

/// utility function for converting to string
template<typename T>
std::string to_str(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

#endif
