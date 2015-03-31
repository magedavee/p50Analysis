#include "ProcessInfo.hh"


#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <G4Version.hh>

void ProcessInfo::fillNode(TXMLEngine& E) {
    uid_t uid = getuid();
    addAttr(E, "uid", int(uid));
    
    passwd* p = getpwuid(uid);
    if(p && p->pw_name) addAttr(E, "uname",p->pw_name);
    
    char hname[1024];
    gethostname(hname, 1024);
    addAttr(E, "host", hname);
    addAttr(E, "Geant4", G4Version);
}
