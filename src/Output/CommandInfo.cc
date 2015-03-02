#include "CommandInfo.hh"
#include <G4UImanager.hh>
#include <G4ios.hh>

void CommandInfo::fillNode(TXMLEngine&) {
    children.clear();
    cmds.clear();
    G4UImanager* UI = G4UImanager::GetUIpointer();
    for(int i=0; i<UI->GetNumberOfHistory(); i++) {
        cmds.push_back(XMLProvider("uicmd"));
        cmds.back().addAttr("cmd",UI->GetPreviousCommand(i));
    }
    for(auto it = cmds.begin(); it != cmds.end(); it++)
        addChild(&(*it));
}
