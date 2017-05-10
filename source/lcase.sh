#!/bin/sh

for file in * 
do
	lcfile=$( eval echo -n $file | tr '[:upper:]' '[:lower:]')
	echo "$file -> $lcfile"
	if [ "$file" != $lcfile ]
	then
		mv $file $lcfile
	fi
done
