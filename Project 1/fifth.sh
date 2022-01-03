#!/bin/bash

mynumber=$1
result=0

if (( $1 <  10 ));
then
	result=$(($result +$1))
fi

while  (($mynumber > 9 ));
do
	twodigit=$(( $mynumber % 100 ))
	digit1=$(($twodigit %10))
	digit2=$(($twodigit/10))
        
	toBeAd=$(( $digit1 * 10 + $digit2 ))
	
	result=$(($result + $toBeAd))
	mynumber=$mynumber/10	
done	

echo "$result"
exit

