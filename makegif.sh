#bash file to get around make's tab formatting

gcc -W -Wall -O2 -g fractal_zoom.c -o out
./out
convert -delay 10 -loop 0 `ls -v *.pgm` fractal_zoom.gif 
