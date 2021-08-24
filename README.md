# fractals
C code to generate gifs of the classic Mandelbrot set zoom.

TODO: Rewrite Image generation in C

- [ ] Normalize intensity values to be from 0-255 (greyscale)
- [ ] Convert greyscale to RGB using RGB struct using same intensity for each R,G,B value:

Something like this:
```C
int normalized_intensity = 120;

typedef struct RGB {
    double r = 120.0;
    double g = 120.0;
     double b = 120.0;
}
```
- [ ] Convert RGB struct array to image using a C image library


To run C script just use make
```Bash
make
```

Example output: still image
<img src="./fractal.png">

Example output: gif (can be much higher zoom/resolution, this was rendered on my home laptop)
<img src="./fractal_zoom.gif">
