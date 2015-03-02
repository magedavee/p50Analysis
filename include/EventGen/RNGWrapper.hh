/// \file RNGWrapper.hh \brief Random-number generator wrapper class for CRY.

/// Wrapper for random-number generator used by CRY
template<class T>
class RNGWrapper { 
public:
    /// set object and function to call for random number
    static void set(T* object, double (T::*func)(void)) { m_obj = object; m_func = func; }
    /// get random number
    static double rng(void) { return (m_obj->*m_func)(); }
private:
    static T* m_obj;                    ///< object providing random numbers
    static double (T::*m_func)(void);   ///< object's random number function
};

template<class T> T* RNGWrapper<T>::m_obj;

template<class T> double (T::*RNGWrapper<T>::m_func)(void);
