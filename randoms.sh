#! /bin/bash

i=0
for i in $(seq 1 20000) ;do
echo $i	
echo $(( $RANDOM % 10000 + 1 ))
	
done

