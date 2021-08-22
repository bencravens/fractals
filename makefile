makegif:
	gcc -W -Wall -O2 -g fractal_zoom.c -o out
	./out
	python3 plotc.py
	convert -delay 20 {0..10}.png -loop 0 fractal_zoom.gif 
