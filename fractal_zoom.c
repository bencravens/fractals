#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*malloc with error checking*/
void* emalloc(size_t n) {
    void* result = malloc(n);
    if (result == NULL) {
        fprintf(stderr, "memory allocation failed!");
        exit(EXIT_FAILURE);
    }
    return result;
}

/*function adapted from cython github implementation*/
static int isclose(double a, double b, double rel_tol, double abs_tol) {
    double diff = 0.0;

    /* sanity check on the inputs */
    if (rel_tol < 0.0 || abs_tol < 0.0 ) {
        fprintf(stderr,"tolerances must be non negative.");
        return -1;
    }

    if ( a == b ) {
        /* short circuit exact equality -- needed to catch two infinities of
           the same sign. And perhaps speeds things up a bit sometimes.
        */
        return 1;
    }

    /* now do the regular computation
       this is essentially the "weak" test from the Boost library
    */

    diff = fabs(b - a);

    return (((diff <= fabs(rel_tol * b)) ||
             (diff <= fabs(rel_tol * a))) ||
            (diff <= abs_tol));
}

/*my own rewrite of python's arange*/
double* arange(double start, double end, double increment) {
    int i;
    double* result;
    double diff;
    int num;

    /*sanity checks*/
    if (start > end) {
        fprintf(stderr, "start must be less than end");
    } else if (increment > (end - start)) {
        fprintf(stderr, "increment too large");
    }

    diff = end - start;
    num = (diff/increment) + 1;
    result = emalloc(num * sizeof result[0]);
    for (i=0;i<num;i++) {
        result[i] = start + increment * i;
    }

    for (i=0;i<num;i++) {
        printf("%f\n",result[i]);
    }

    /*another error check. Last value of array should be "end"*/
    if (result[num-1] != end) {
        fprintf(stderr,"last entry: %f. Should be %f\n",result[num-1],end);
    }

    return result;
}

/*generate the fractal*/
void fractal(char* filename, double x_min, double x_max, double y_min, double y_max, int max_iters, double increment) {
    double* x_range = arange(x_min,x_max,increment);
    int x_len = (int) ((fabs(x_max-x_min)/increment) + 1);
    double* y_range = arange(y_min,y_max,increment);
    int y_len = (int) ((fabs(y_max-y_min)/increment) + 1);
    double result[y_len][x_len];
    int i;
    int j;
    double x_0;
    double y_0;
    double x;
    double y;
    double x_temp;
    double x2;
    double y2;
    double x_old;
    double y_old;
    int period;
    int count;
    FILE* fp1;

    /*make the result matrix*/
    for(i=0;i<y_len;i++){
        for(j=0;j<x_len;j++){
            printf("%f, %f\n",x_range[j],y_range[i]);
            x_0 = x_range[j];
            y_0 = y_range[i];
            x = x_range[j];
            y = y_range[i];
            count = 0;
            /*iterate until convergence for each pixel*/
            while ((pow(x,2.0) + pow(y,2.0) < 4) && (count < max_iters)) {
                x2 = pow(x,2.0);
                y2 = pow(y,2.0);
                x_temp = x2 - y2 + x_0;
                y = 2*x*y + y_0;
                x = x_temp;
                count++;

                /*check to see if we are trapped in a periodic iteration loop*/
                if (isclose(x,x_old,1e-3,0.0001) && isclose(y,y_old,1e-3,0.0001)) {
                    printf("converged");
                    count = max_iters;
                    break;
                }
                period++;
                if (period > 20) {
                    period = 0;
                    x_old = x;
                    y_old = y;
                }
            }
            result[i][j] = count;
        }
    }

    /*write result to csv file so we can plot it with python*/
    fp1 = fopen(filename, "w");
    if (fp1 == NULL) {
        fprintf(stderr, "could not open file\n");
        exit(EXIT_FAILURE);
    }
    for (i=0; i<y_len; i++) {
        for (j=0; j<x_len; j++) {
            fprintf(fp1, "%lf", result[i][j]);
            if (j<(x_len-1)) {
                fprintf(fp1, ",");
            }
        }
        fprintf(fp1,"\n");
    }
    fclose(fp1);

    free(x_range);
    free(y_range);
}

/*generate zoom_num csv files zooming in on x_midpoint, y_midpoint*/
void zoom(double x_midpoint, double y_midpoint, int zoom_num) {
    int i;
    /*hardcode mandelbrot range*/
    double x_min_boundary = -2.5;
    double x_max_boundary = 1.0;
    double y_min_boundary = -1.0;
    double y_max_boundary = 1.0;
    double x_radius = (x_max_boundary - x_min_boundary);
    double y_radius = (y_max_boundary - y_min_boundary);
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    char filename[32]; // The filename buffer.
    for (i=0;i<zoom_num;i++) {
        /*zoom in a bit*/
        x_radius = x_radius / 2;
        y_radius = y_radius / 2;
        x_min = x_midpoint - x_radius;
        x_max = x_midpoint + x_radius;
        y_min = y_midpoint - y_radius;
        y_max = y_midpoint + y_radius;
        // Put "file" then k then ".txt" in to filename.
        snprintf(filename, sizeof(char) * 32, "%i.csv", i);
        printf("filename is %s",filename);
        fractal(filename, x_min, x_max, y_min, y_max,1000,0.003);
    }
}

int main() {
    printf("hello world\n");
    zoom(-0.75, 0, 10);
    return 0;
}
