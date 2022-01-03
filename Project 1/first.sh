#! bin/bash
input=$1
while IFS= read -r line
do
	if test -z "$line"
	then
		exit
	fi
for((i = 0 ; i < $line ; i++))
do
		echo -ne "*"

	done
	echo -e ""

done < "$input"

