#!/bin/bash
filename=$1
search=$2
replace=$3
COUNTER=0

    for file in $filename ; do
        COUNTER=$(grep -o "$search" $file | wc -l)
        sed -i 's/'$search'/'$replace'/g' $file
	echo "All $COUNTER  occurrences of “$search” in “$file” has changed with “$replace”"
        let COUNTER=0
    done

exit 0;
