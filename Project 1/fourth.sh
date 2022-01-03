#!/bin/bash

BIGGESTFILESIZE=0;
SMALLESTFILESIZE=10000;

for FILE in *; 
do 
	if ((  $(stat -c%s $FILE) >  $BIGGESTFILESIZE )); then
	BIGGESTFILESIZE=$(stat -c%s $FILE)
	fi
	
	if (( $(stat -c%s $FILE) < $SMALLESTFILESIZE )); then
	SMALLESTFILESIZE=$(stat -c%s $FILE)
	fi
done

echo $SMALLESTFILESIZE
echo $BIGGESTFILESIZE

mkdir smallest
mkdir biggest

for FILE in *;
do
	if [ "$(stat -c%s $FILE)" -eq "$BIGGESTFILESIZE" ];
	then
		#echo "biggestasd"
		#echo "$FILE"
		mv "$FILE" "biggest"
		#move the biggest one
	fi
done

for FILE in *;
do 
	if [ "$(stat -c%s $FILE)" -eq "$SMALLESTFILESIZE" ];
	then
		mv "$FILE" "smallest"
	fi
done




exit

