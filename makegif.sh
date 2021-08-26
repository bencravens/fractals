#!/bin/bash

#run fractal generation program. saves as .pgm files
gcc -W -Wall -O2 -g fractal_zoom.c -o out
./out

#convert all .pgm files to .png files
num_files=`find ./ -name '*.pgm' | wc -l`
#0 based indexing
num_files=$((num_files-1))
for file in $(seq 0 $num_files)
do
    echo "converting $file.pgm to $file.png"
    convert $file.pgm $file.png
done

#now convert png files to a gif
convert -delay 10 -loop 0 `ls *.png | sort -V` fractal_zoom.gif 
