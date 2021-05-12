#!/bin/bash
# Script für die Flusskontrolle

#Globale Variablen
FILE=$1
ABSPATH=$(realpath $FILE)
ID=$$
TIMESTAMP=$(date +"%y%m%d%H%M%S")

#Funktionen

function checkTrashBin {
	if ! [ -d $HOME/.trashBin ]; then
		echo "TrashBin does not exist. Created one."
		mkdir $HOME/.trashBin
	fi
	if ! [ -f $HOME/.trashBin/.dir ]; then
		touch $HOME/.trashBin/.dir
		echo "TrashBin logFile does not exists. Created one."
	fi
}

function deleteFile {
	if [ -f $FILE ]; then
		mv $FILE $HOME/.trashBin/"$TIMESTAMP"_"$ID"".dat"
		echo "$TIMESTAMP"_"$ID"".dat!" "$ABSPATH" &>> $HOME/.trashBin/.dir
		echo $FILE "has been moved to TrashBin and was renamed to $TIMESTAMP"_"$ID"".dat"
	else
		echo $FILE "was not Found!"
	fi
}


#Programm

checkTrashBin
deleteFile

