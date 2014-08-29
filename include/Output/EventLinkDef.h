#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ParticleVertex+;
#pragma link C++ class ParticleEvent+;
#pragma link C++ class EventIoniCluster+;
#pragma link C++ class EventNCapt+;
#pragma link C++ class Event+;

#include <vector>
#ifdef __MAKECINT__
#pragma link C++ class vector<vector<double> >+;
#endif

#endif
