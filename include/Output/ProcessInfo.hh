#ifndef PROCESSINFO_HH
/// Assure this header is loaded only once
#define PROCESSINFO_HH

#include "XMLProvider.hh"

/// Class for collecting and XML-izing run context info
class ProcessInfo: public XMLProvider {
public:
    /// Constructor
    ProcessInfo(): XMLProvider("Process") { }
    
protected:
    /// fill in XML data
    virtual void fillNode(TXMLEngine& E);
};

#endif
