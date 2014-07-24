/// Wrapper for random-number generator used by CRY
template<class T>
class RNGWrapper { 
public:
    static void set(T* object, double (T::*func)(void)) { m_obj = object; m_func = func; }
    static double rng(void) { return (m_obj->*m_func)(); }
private:
    static T* m_obj;
    static double (T::*m_func)(void);
};

template<class T> T* RNGWrapper<T>::m_obj;

template<class T> double (T::*RNGWrapper<T>::m_func)(void);
