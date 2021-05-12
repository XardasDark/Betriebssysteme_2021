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

function undeleteFile {
	fileFound=0
	while read line; do
		tempname="$(cut -d'!' -f1 <<<"$line")"
		realpath="$(cut -d'!' -f2- <<<"$line")"
		realpath=$(echo $realpath | xargs)
		filename=$(basename $realpath)
		directory=$(dirname $realpath)
		if [ $tempname = $FILE ]; then
			if [ ! -d $directory ]; then
				fileFound=-1
				break
			fi
			mv $HOME/.trashBin/$tempname $realpath
			grep -v $tempname $HOME/.trashBin/.dir >.deleted
			fileFound=1
			break
		fi
	done <$HOME/.trashBin/.dir
	if [ $fileFound = 1 ]; then
		mv -f .deleted $HOME/.trashBin/.dir
		echo "File was restored to its original path"
	elif [ $fileFound = -1 ]; then
		echo "Could not restore the file because the path no longer exists"
	else
		echo "File was not found in the TrashBin"
	fi
	#if [ -f $FILE ]; then
	#	mv $FILE $HOME/.trashBin/"$TIMESTAMP"_"$ID"".dat"
	#	echo "$TIMESTAMP"_"$ID"".dat!" "$ABSPATH" &>> $HOME/.trashBin/.dir
	#	echo $FILE "has been moved to TrashBin and was renamed to $TIMESTAMP"_"$ID"".dat"
	#else
	#	echo $FILE "was not Found!"
	#fi
}


#Programm

checkTrashBin
undeleteFile

