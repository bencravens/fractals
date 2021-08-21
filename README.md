# fractals
C and python code to generate pictures of the Mandelbrot set.

To run C script (a lot faster than python)
```Bash
gcc -W -Wall -O2 -g fractal_zoom.c -o output
./output
python3 plotc.py
```

TODO: 
- [] Generate gif of fractal zoom
- [] Higher resolution in C without segfault

Example output:
<img src="./fractal.png">
