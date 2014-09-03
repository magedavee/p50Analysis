#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ParticleVertex+;
#pragma link C++ class ParticleEvent+;
#pragma link C++ class IoniCluster+;
#pragma link C++ class IoniClusterEvent+;
#pragma link C++ class NCapt+;
#pragma link C++ class NCaptEvent+;
#pragma link C++ class Event+;

#include <vector>
#ifdef __MAKECINT__
#pragma link C++ class vector<vector<double> >+;
#endif

#endif
