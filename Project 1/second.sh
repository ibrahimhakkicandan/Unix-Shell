#!/bin/bash
DIRECTORY=$1
if [ -d "$DIRECTORY" ]; then
  # Takee action if $DIR exists. #
  
cd -- "$1"
for f in *;
do
	find . -type f ! -name "*.c" ! -name "*.sh" ! -name "Makefile" ! -name "makefile" -exec rm {} \;
done

else
	echo " There is no directory named by ${DIRECTORY}"
	exit
fi
