#!/bin/sh

chmod +Xx /opt/openzelda/share/applications/OpenZelda.desktop
chmod +Xx /opt/openzelda/share/applications/QuestDesigner.desktop


xdg-mime install /opt/openzelda/share/mime/openzelda-quest.xml
xdg-desktop-menu install --novendor /opt/openzelda/share/applications/OpenZelda.desktop 
xdg-desktop-menu install --novendor /opt/openzelda/share/applications/QuestDesigner.desktop 


