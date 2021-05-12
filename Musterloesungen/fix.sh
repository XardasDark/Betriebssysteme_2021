#!/bin/bash
FILE=$1

sed -i -e 's/\r$//' $FILE
