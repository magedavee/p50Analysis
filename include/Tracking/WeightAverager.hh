/// \file WeightAverager.hh \brief Utility for recording multivariate weighted-average sums.
#ifndef WEIGHTAVERAGER_HH
#define WEIGHTAVERAGER_HH

#include <cassert>
#include <cmath>

/// Utility class for weighted sums
template<unsigned int N>
class WeightAverager {
public:
    /// Constructor
    WeightAverager(): sum_w(0) { for(unsigned int i=0; i<N; i++) sum_wx[i] = sum_wxx[i] = 0; }
    /// Destructor
    virtual ~WeightAverager() {}
        
    /// add weighted sum
    void operator+=(const WeightAverager<N>& W) {
        sum_w += W.sum_w;
        for(unsigned int i=0; i<N; i++) {
            sum_wx[i] += W.sum_wx[i];
            sum_wxx[i] += W.sum_wxx[i];
        }
    }

    /// get averaged value
    double get_avg(unsigned int i) const { assert(i<N); return sum_wx[i]/sum_w; }
    /// get RMS
    double get_rms(unsigned int i) const { assert(i<N); double u = sum_wxx[i]*sum_w - sum_wx[i]*sum_wx[i]; return (u>0 && sum_w>0)?sqrt(u)/sum_w:0; }
    
protected:

    /// add weighted term
    void fill_with_weight(double w) {
        sum_w += w;
        for(unsigned int i=0; i<N; i++) {
            sum_wx[i] += w*x_in[i];
            sum_wxx[i] += w*x_in[i]*x_in[i];
        }
    }
    
    double x_in[N];     ///< input for each variable
    double sum_w;       ///< sum of weights
    double sum_wx[N];   ///< weighted sum
    double sum_wxx[N];  ///< weighted sum of squares
};

#endif
