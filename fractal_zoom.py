import copy
import numpy as np
from matplotlib import pyplot as plt
from numpy import exp
from math import isclose
"""
The mandelbrot set is the set of values of c in the complex plane
for which the iteration:
    
    z_{n+1} = (z_{n})^2 + c 

Remains bounded.

Plot for each pixel x_0, y_0 by letting the following be true:

z = x + iy
z^2 = x^2 + 2ixy - y^2
c = x_0 + iy_0

So: 

x_n = Re((z_n)^2 + c) = (x_n)^2 + x_0 - (y_n)^2
y_n = Im((z_n)^2 + c) = 2(x_n)(y_n) + y_0

We iterate each pixel max_iter number of times, or until z converges to a value (z^2 < 4).
We then plot the iteration number n
"""

#use https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Escape_time_algorithm
#naive implementation
def fractal_zoom(max_iters,x_size,y_size):
    #Mandelbrot X scale is (-2.5,1)
    x_range = np.linspace(-2.5,1,x_size)
    #Mandelbrot Y scale is (-1,1) 
    y_range = np.linspace(-1,1,y_size)
    print(x_range)
    print(y_range)
    Z = np.zeros([y_size,x_size])
    for x_index,x_cur in enumerate(x_range):
        for y_index,y_cur in enumerate(y_range):
            print(x_cur,y_cur)
            x_0 = copy.copy(x_cur)
            y_0 = copy.copy(y_cur)
            x = copy.copy(x_cur)
            y = copy.copy(y_cur)
            count = 0
            #iterate until convergence for each pixel
            while (x**2 + y**2 < 4) and (count < max_iters):
                x_temp = x**2 - y**2 + x_0
                y = 2*x*y + y_0
                x = x_temp
                count += 1
            Z[y_index,x_index] = count
    plt.pcolor(x_range, y_range, Z,cmap='plasma')
    plt.xlabel('X range')
    plt.ylabel('Y range')
    plt.show()

#period checking algorithm (see wiki page)
def fractal_zoom_period(max_iters,x_size,y_size):
    #Mandelbrot X scale is (-2.5,1)
    x_range = np.linspace(-2.5,1,x_size)
    #Mandelbrot Y scale is (-1,1) 
    y_range = np.linspace(-1,1,y_size)
    print(x_range)
    print(y_range)
    Z = np.zeros([y_size,x_size])
    for x_index,x_cur in enumerate(x_range):
        for y_index,y_cur in enumerate(y_range):
            print(x_cur,y_cur)
            x_0 = copy.copy(x_cur)
            y_0 = copy.copy(y_cur)
            x = copy.copy(x_cur)
            y = copy.copy(y_cur)
            x_old = 0
            y_old = 0
            period=0
            count = 0
            #iterate until convergence for each pixel
            while (x**2 + y**2 < 4) and (count < max_iters):
                x_temp = x**2 - y**2 + x_0
                y = 2*x*y + y_0
                x = x_temp
                count += 1

                #check to see if we in a iteration loop
                if isclose(x,x_old,rel_tol=1e-2) and isclose(y,y_old,rel_tol=1e-2):
                    print("converged")
                    count=max_iters
                    break
                period += 1
                if period > 20:
                    period = 0
                    x_old = x
                    y_old = y
            Z[y_index,x_index] = count
    Z = np.log(Z)
    plt.pcolor(x_range, y_range, Z,cmap='plasma')
    plt.xlabel('X range')
    plt.ylabel('Y range')
    plt.show()

if __name__=="__main__":
    fractal_zoom_period(1000,1000,1000)
