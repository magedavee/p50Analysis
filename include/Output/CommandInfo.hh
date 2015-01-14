/// \file CommandInfo.hh \brief XML output for macro commands.
#ifndef COMMANDINFO_HH
#define COMMANDINFO_HH

#include "XMLProvider.hh"

/// Class for collecting and XML-izing run command history
class CommandInfo: public XMLProvider {
public:
    /// Constructor
    CommandInfo(): XMLProvider("Commands") { }
    
protected:
    /// fill in XML data
    virtual void fillNode(TXMLEngine& E);
    vector<XMLProvider> cmds;      ///< list of commands issued
};

#endif
