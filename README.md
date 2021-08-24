# fractals
C code to generate gifs of the classic Mandelbrot set zoom.

TODO: Rewrite Image generation in C

- [ ] Normalize intensity values to be from 0-255 (greyscale)
- [ ] Convert greyscale array to .PGM format using this code: https://stackoverflow.com/questions/22580812/writing-a-png-in-c/22580958#22580958
- [ ] Use imagemagick to convert to a gif


To run C script just use make
```Bash
make
```

Example output: still image
<img src="./fractal.png">

Example output: gif (can be much higher zoom/resolution, this was rendered on my home laptop)
<img src="./fractal_zoom.gif">
