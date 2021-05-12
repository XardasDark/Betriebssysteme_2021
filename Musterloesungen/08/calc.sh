#!/bin/bash
# Script für die Flusskontrolle


# Globale Variablen

if [ $# -lt 2 ]; then
	echo "Fehler: Es wurde nicht genug Argumente eingegeben!" >&2; exit 1
fi

DIVISOR=$1
shift 1
MULT=$@
ADDEDNUMBER=1

# Funktionen

function checkNumbers {

	re='^[+-]?[0-9]+$'

	if ! [[ $DIVISOR =~ $re ]] ; then
		echo "Fehler: Es wurden keine ganze Zahl als Divisor eingegeben!" >&2; exit 1
	fi

	#re='^[+-]?[0-9]+([.][0-9]+)?$' für Kommazahlen
	for number in $MULT
	do

		if ! [[ $number =~ $re ]] ; then
			echo "Fehler: Es wurden nicht nur ganze Zahlen als Dividenden eingegeben!" >&2; exit 1
		else
			ADDEDNUMBER="$( bc <<<"$ADDEDNUMBER * $number" )"
		fi
	done
}

function divide {
	echo $ADDEDNUMBER
	echo $DIVISOR 

	quotient="$( bc <<<"$ADDEDNUMBER / $DIVISOR" )"
	modulo="$( bc <<<"$ADDEDNUMBER % $DIVISOR" )"
	
	
	echo "Ergebnis:" $quotient", Rest:" $modulo
}

function printFileName {
	me=`basename "$0"`
	echo "Der Dateiname des Skriptes lautet:" $me
}


# Programm

checkNumbers
divide
printFileName