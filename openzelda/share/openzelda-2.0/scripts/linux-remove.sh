#!/bin/bash

xdg-mime uninstall /opt/openzelda/share/mime/openzelda-quest.xml
xdg-desktop-menu uninstall --novendor /opt/openzelda/share/applications/OpenZelda.desktop 
xdg-desktop-menu uninstall --novendor /opt/openzelda/share/applications/QuestDesigner.desktop 

#echo "Do you wish to remove all user content?"
#select yn in "Yes" "No";
#do
#	case $yn in
#       		Yes ) rm -rI ~/openzekda-quests/; exit;;
#        	No ) exit;;
#    	esac;
#done

