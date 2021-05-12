#!/bin/bash
# Script für die Flusskontrolle

#Globale Variablen
FILE=$@

#Funktionen
function testIfExists {
	for path in $FILE
	do
		if [ -a $path ]; then
			echo "Datei/Ordner" $path "existiert!"
		else
			echo "Die Datei/Ordner" $path "existiert nicht!"
		fi
	done
}

function testIsFileOrDirectory {
	for path in $FILE
	do
		if [ -f $path ]; then
			echo $path "ist eine reguläre Datei!"
		elif [ -d $path ]; then
			echo $path "ist ein Ordner!"
		else
			echo $path "ist keine reguläre Datei und kein Ordner!"
		fi
	done
}

function testIsLink {
	for path in $FILE
	do
		if [ -h $path ]; then
			echo $path "ist ein symbolischer Link!"
		else
			echo $path "ist kein symbolischer Link!"
		fi
	done
}

function testCallerIsOwner {
	for path in $FILE
	do
		if [ -O $path ]; then
			echo $path "wurde vom Besitzer aufgerufen!"
		else
			echo $path "wurde nicht vom Besitzer aufgerufen!"
		fi
	done
}

function tellOwner {
	for path in $FILE
	do
		if [ -a $path ]; then
			USER=$(stat -c '%U' $path)
			echo $path "gehört dem Nutzer" $USER "!"
		else
			echo $path "hat kein besitzer, weil es nicht existiert"
		fi
	done
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




