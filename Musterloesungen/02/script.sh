#!/bin/bash
# Script für die Flusskontrolle

#Globale Variablen
FILE=$1

#Funktionen
function testIfExists {
	if [ -a $FILE ]; then
		echo "Datei/Ordner" $FILE "existiert!"
	else
		echo "Die Datei/Ordner" $FILE "existiert nicht!"
	fi
}

function testIsFileOrDirectory {
	if [ -f $FILE ]; then
		echo $FILE "ist eine reguläre Datei!"
	elif [ -d $FILE ]; then
		echo $FILE "ist ein Ordner!"
	else
		echo $FILE "ist keine reguläre Datei und kein Ordner!"
	fi
}

function testIsLink {
	if [ -h $FILE ]; then
		echo $FILE "ist ein symbolischer Link!"
	else
		echo $FILE "ist kein symbolischer Link!"
	fi
}

function testCallerIsOwner {
	if [ -O $FILE ]; then
		echo $FILE "wurde vom Besitzer aufgerufen!"
	else
		echo $FILE "wurde nicht vom Besitzer aufgerufen!"
	fi
}

function tellOwner {
	USER=$(stat -c '%U' $FILE)
	echo $FILE "gehört dem Nutzer" $USER "!"
}


#Programm

PS3='Wählen Sie Ihre Option: '
options=("Check ob Datei/Verzeichnis existiert" "Check ob Eingabe Datei/Verzeichnis ist" "Check auf symbolischen Link" "Check ob Aufrufer Besitzer" "Gib Besitzer aus" "Beenden")
select opt in "${options[@]}"
do
    case $opt in
        "Check ob Datei/Verzeichnis existiert")
            testIfExists
            ;;
        "Check ob Eingabe Datei/Verzeichnis ist")
            testIsFileOrDirectory
            ;;
        "Check auf symbolischen Link")
            testIsLink
            ;;
	"Check ob Aufrufer Besitzer")
            testCallerIsOwner
            ;;
	"Gib Besitzer aus")
            tellOwner
            ;;
        "Beenden")
            break
            ;;
        *) echo "Ungültige Option $REPLY";;
    esac
done




