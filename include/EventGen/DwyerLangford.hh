/// \file DwyerLangford.hh \brief Container class for the Dwyer-Langford spectral data
#ifndef DwyerLangford_H
#define DwyerLangford_H

#include <vector>
using std::vector;
#include <string>
using std::string;

class DwyerLangford {
public:
    /// Constructor
    DwyerLangford();
    
    vector<double> Dwyer235;    ///< Dwyer Langford U235 spectrum table
    vector<double> Dwyer238;    ///< Dwyer Langford U238 spectrum table
    vector<double> Dwyer239;    ///< Dwyer Langford Pu239 spectrum table
    vector<double> Dwyer241;    ///< Dwyer Langford Pu241 spectrum table
    vector<double> DwyerE;      ///< Dwyer Langford Energy values

protected:
    /// load data tables from file
    void load_data(const string& fname, vector<double>& E, vector<double>& rate) const;
};

#endif
